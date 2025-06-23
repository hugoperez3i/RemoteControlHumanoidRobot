
---

The application protocol runs over a persistent socket connection through the TCP/IP stack. Since it relies on TCP for several of its core behaviors, it's recommended, but not required, to disable [Nagle's Algorithm](https://en.wikipedia.org/wiki/Nagle%27s_algorithm) by enabling the TCP_NODELAY socket option; and to enable the [TCP keepalive](https://tldp.org/HOWTO/TCP-Keepalive-HOWTO/overview.html) with an aggressive setting. By default, the protocol uses port `TCP:54817`.

## Architecture Components

The protocol contemplates three types of nodes: _client_, _server_ and _mcu_.

- **Client:** Refers to the remote user, which can control a _mcu_.
- **Server:** Refers to the orchestrator service, in charge of query validation and routing _client_ queries to the indicated _MCU_. 
- **MCU:** Refers to the remote robotic system, which may be remotely operated by a _client_.

### Client Role

The _client_ is the only proactive node in the architecture: it sends commands to the _server_, which either relays them to the _mcu_ or handles them directly before replying with a [control response](Control%20Query.md).

### Server Role

The _server_ acts as the orchestrator in this architecture. Its functions are routing and validation, interconnecting each _client_ with the _mcu_ it intends to control, and validating the parameters sent by the _client_ before relaying them to the _mcu_ for execution. The _server_ will never send any message to the _client_ or _mcu_ unless prompted by a _client_ query.

### MCU Role

The _mcu_ is the passive executor node, responsible solely for carrying out orders received from the _server_. On startup, it sends a [login message](Identification%20Query.md) to the _server_, then waits for incoming commands, executes them, and replies with an ACK control message.

## MCU Types

The protocol contemplates two types of _MCU_ robotic systems:

- **SmartMCU:** Refers to a _mcu_ that is capable of generating its own PWM signal values for a given position. This means that a _SmartMCU_ must be able to store locally the PWM calibration values, it also needs to be capable of storing _pending movements_, and will be required to, on connection, send the current position of each of its servos.
- **DumbMCU:** Refers to a _mcu_ that executes movements by reading the PWM signal value directly from the query. This means that a _DumbMCU_ will not receive any information until a movement is expected to be executed, at which point, it will receive a query with the **PWM signal value** for each servo, instead of a servo position (0º-179º). On connection, a _DumbMCU_ is required to send the number of servos it has. 

## No binary zero convention

In order to avoid potential issues with the zero-value bytes (`#!Cpp uint8_t b=0`), **every numerical field that could possibly take the value zero gets offset by +1**. This means that fields such as `servoId` (range 0-31) and `servoPosition` (range 0-179) will be sent with a +1 positive offset applied: `servoId` would take the range (1-32) and `servoPosition` the range (1-180).

## Protocol Query Format

The protocol distinguishes two types of query formats: A _client - server_ format, and a _server - mcu_ format. These two formats were defined to improve the clarity of the protocol, making it obvious which part of the communication chain a given query belongs to.
However, the [login query](Identification%20Query.md) poses an exception to these formats, where _client_ and _mcu_ have a unique query style, also different from the other query formats.

The protocol query uses plain, raw binary, with no codification or encryption. All format and structure characters are standard ASCII.

### _Client - Server_ Query Format

The message follows a set structure: it always starts with a constant header, `!s-`, followed by a 4-byte function selector code `CODE`, and is terminated by a constant tail `-e!`. This structure can accommodate additional information, for those functions that require it: the information will be added behind the four-byte function selector code (`CODE`), separated with an additional slash `-`.

=== "No additional information"

    ```
    !s-CODE-e!
    ```

=== "Containing additional information"

    ```
    !s-CODE-[ADDITIONAL_INFORMATION]-e!  
    ```

This means that the minimum query length will be `11` bytes (3 from the header `!s-`, +4 from the selection code `CODE`, and +3 from the tail sequence `-e!`). Any query with less than `11 bytes` can be safely deleted, as it would mean that some kind of error happened during query formation.

### _Server - MCU_ Query Format

Similarly to the _client - server_ query, the _server - mcu_ query also uses an structured format with header and tail. However, in this second format, the header is reduced to a single `-`, while the tail sequence takes the form `-!`. The 4-byte function selector code becomes a single-byte code `c`, and the additional information, when needed, is added in the exact same format as the _client - server_ format: behind the single-byte code `c`, and separated with a dash character `-`.

=== "No additional information"

    ```
    -c-!
    ```

=== "Containing additional information"

    ```
    -c-[ADDITIONAL_INFORMATION]-!
    ```

### _PWM Signal_ Encoding

Some queries, such as [Upload Calibration Data](Upload%20Information.md), [Servo Movement](Servo%20Movement.md), and [Servo Movement (Delayed Mode)](Move%20All%20Query.md), rely on a custom `#!Cpp uint16_t` encoding to transmit the PWM signal.
This codification is explained in the documentation for each relevant query. It's also available on the following area:

??? abstract "PWM Signal codification"

    Since the PWM signal can take various bit resolution levels, usually ranging from 8-bit to 16-bit, with >16bit being niche or application-specific territory, the procotol allocates 2 bytes (up to 15-bit pwm resolution) for PWM signals on SRVP queries. 

    This means that in order to follow [the protocol no-zero convention](Protocol%20Definition.md/#no-binary-zero-convention), each individual byte needs to incorporate this offset. This is required because, for values of `PWM_SIGNAL<255`, the upper-byte will always be zero, and for the specific value `PWM_SIGNAL=256`, the lower-byte will be zero. 

    !!! info "How is offset applied"

        The offset is introduced by fixing the most significant bit of the `#!Cpp uint16_t PWM_SIGNAL` to `1`, and then adding 1 to the normal PWM signal value. This results in a 15-bit signal with one fewer representable PWM value, a range of [0 → 32766]. On PWM servos that use the full 15-bit resolution over a 180º linear range, this results in a precision loss of approximately 5.493 millidegrees reduction in total range, and 0.1676 microdegrees decrease in angular resolution per step, which is effectively negligible for most use-cases.

        To introduce this offset, the formula `#!Cpp PWM_SIGNAL = pwm + (1+(0b1<<15))` is used; and the formula `#!Cpp pwm = PWM_SIGNAL - (1+(0b1<<15))` is used on the receiving end to undo the offset applied.

        ???+ Danger "Endianness"

            To avoid the [endianness](https://en.wikipedia.org/wiki/Endianness#Networking) problem, the protocol introduces a normalized implementation through which `#!Cpp uint16_t` values are split into two `#!Cpp uint8_t` values in a set order (the first byte will be the upper-byte, followed by the lower-byte). The visual representation of this transformation would be:

            <figure markdown="span">
                ![uint16](../assets/pwmformat.png#only-light){width=400}
                ![uint16](../assets/pwmformat_w.png#only-dark){width=400}
                <figcaption/>
            </figure>

            And the functions used for this are:

            ```cpp title="divideIntoBytes()" linenums="1"
            std::string QueryGenerator::divideIntoBytes(uint16_t u){
                std::string hl = "";
                hl+=(uint8_t)(u>>8);
                hl+=(uint8_t)(u&0x00ff);
                return hl;
            }
            ```
            
            ```cpp title="restore16int()" linenums="1"
            inline uint16_t QueryGenerator::restore16int(uint8_t c0,uint8_t c1){return (c0<<8)+c1;}
            ```
---

## Protocol Queries Overview

The following table lists all queries defined by the protocol, as well as a brief description and links to each of their documentation pages:

| Query | Additional Information | Description | Documentation |
| :--: | :--: | :--- | :--: |
| [`!s-_ACK-[c]-e!`](Control%20Query.md) | Success code | Sent by server or MCU to acknowledge successful execution | [Link](Control%20Query.md) |
| [`!s-NACK-[c]-e!`](Control%20Query.md) | Error code | Sent by server or MCU to indicate an error | [Link](Control%20Query.md) |
| [`!s-Client_here-e!`](Identification%20Query.md/#client-login-query) | None | Sent by client to identify and initiate login | [Link](Identification%20Query.md/#client-login-query) |
| [`!s-NodeMCU_here-[info]-e!`](Identification%20Query.md/#mcu-login-query) | Unique ID + Servo positions and count | Sent by MCU to register itself with the server | [Link](Identification%20Query.md/#mcu-login-query) |
| [`!s-sMCU-[info]-e!`](Robot%20Selection.md) | MCU Unique ID | Sent by client to select which MCU to control | [Link](Robot%20Selection.md) |
| [`!s-SRVP-[info]-e!`](Servo%20Movement.md) | Number of movements + Information for each servo movement | Sent by client to execute servo movement on selected MCU | [Link](Servo%20Movement.md) |
| [`-m-[info]-!`](Servo%20Movement.md/#server-mcu-servo-movement-query-format) | Number of movements + Information for each servo movement | Sent by server to MCU to execute servo movements | [Link](Servo%20Movement.md/#server-mcu-servo-movement-query-format) |
| [`-u-[info]-!`](Execution%20Mode.md/#delayed-execution-mode) | Number of movements + Information for each servo movement | Sent by server to MCU to store delayed servo movements | [Link](Servo%20Movement.md) |
| [`!s-iMCU-[info]-e!`](MCU%20Information.md) | On server response: Servo positions and count | Sent by client to request current MCU servo positions | [Link](MCU%20Information.md) |
| [`!s-eMOD-[info]-e!`](Execution%20Mode.md) | The execution mode | Sent by client to set MCU execution mode (real-time or delayed) | [Link](Execution%20Mode.md) |
| [`!s-mALL-e!`](Move%20All%20Query.md) | None | Sent by client to trigger execution of stored servo movements | [Link](Move%20All%20Query.md) |
| [`-e-!`](Move%20All%20Query.md) | None | Sent by server to MCU for execution of stored movements | [Link](Move%20All%20Query.md) |
| [`!s-uINF-[info]-e!`](Upload%20Information.md) | Number of servos + calibration data for each servo | Sent by client to upload MCU calibration (PWM) data to Server | [Link](Upload%20Information.md) |
| [`!s-sOFF-e!`](Server%20Shutdown.md) | None | Sent by client to shut down the server remotely | [Link](Server%20Shutdown.md) |
