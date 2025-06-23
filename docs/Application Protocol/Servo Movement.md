# Servo Movement
---

## _Client → Server_ Servo Movement Query Format

The servo movement query (which uses the 4-byte code `SRVP`) is used by the _client_ to issue movement commands for the selected _mcu_. The query has the structure:

=== "SmartMCU SRVP Query"

    ``` 
    !s-SRVP-[NUMBER_OF_MOVEMENTS]-[SERVO_ID:SERVO_POS]-<...>-[SERVO_ID:SERVO_POS]-e!
    ```

    - _`[NUMBER_OF_MOVEMENTS]`_ → A single `#!Cpp uint8_t` indicating how many servo movements the query contains. Since it cannot be zero, it's not affected by the offset. 
    - _`[SERVO_ID:SERVO_POS]`_ → The information pair `SERVO_ID`, the id of the servo the client wants to move; and `SERVO_POS`, the position the servo has to take. These two values are separated by the ASCII character `:` (`#!Cpp uint8_t c=58`), and for multiple movements, each information pair `SERVO_ID:SERVO_POS` is separated by a dash `-` (`#!Cpp uint8_t c=45`) from the information pair of the following servo movement. Both `SERVO_ID` and `SERVO_POS` are single `#!Cpp uint8_t` values, and must have [the positive +1 offset applied](Protocol%20Definition.md/#no-binary-zero-convention), as they can take value zero.

=== "DumbMCU SRVP Query"

    ``` 
    !s-SRVP-[NUMBER_OF_MOVEMENTS]-[SERVO_ID:PWM_SIGNAL]-<...>-[SERVO_ID:PWM_SIGNAL]-e!
    ```

    - _`[NUMBER_OF_MOVEMENTS]`_ → A single `#!Cpp uint8_t` indicating how many servo movements the query contains. Since it cannot be zero, it's not affected by the offset. 
    - _`[SERVO_ID:PWM_SIGNAL]`_ → The information pair `SERVO_ID`, the id of the servo the client wants to move; and `PWM_SIGNAL`, the PWM signal for the given servo ID. These two values are separated by the ASCII character `:` (`#!Cpp uint8_t c=58`), and for multiple movements, each information pair `SERVO_ID:PWM_SIGNAL` is separated by a dash `-` (`#!Cpp uint8_t c=45`) from the information pair of the following servo movement. Both `SERVO_ID` (a single `#!Cpp uint8_t` value) and `PWM_SIGNAL` (a `#!Cpp uint16_t` value) must have [the positive +1 offset applied](Protocol%20Definition.md/#no-binary-zero-convention), as they can take value zero.

Due to the _server_ function of query parameter validation, the response for this query can take two forms: 

If during validation, the _server_ rejects a query for any reason, it will return a single NACK control query to the _client_, ending the SRVP process. The defined possible NACK codes for this first reponse are:

- _`_NACK_InvalidQuery`_ (`#!Cpp uint8_t c=255`) → If the [query format](Protocol%20Definition.md/#client-server-query-format) is invalid: contains null-bytes, invalid header or tail sequences, or the query function code is not recognized.
- _`_NACK_NoActiveMCU`_ (`#!Cpp uint8_t c=254`) → If _client_ has not selected a _mcu_ before issuing the movement command.
- _`_NACK_InvalidParameter`_ (`#!Cpp uint8_t c=252`) → If the query parameters are out of range: a servo id exceeds the selected _mcu_ servo count, or a servo position out of the range [1→180].
- _`_NACK_ServoCountMissmatch`_ (`#!Cpp uint8_t c=251`) → If the movement order tries to move more servos than the selected _mcu_ has.
- _`_NACK_NoMCUInfo`_ (`#!Cpp uint8_t c=250`) → If the selected _mcu_ is a "[**DumbMCU**](Protocol%20Definition.md//#mcu-types)", and there's no PWM information available in the _server_ database.
- _`_NACK_MCUOffline`_ (`#!Cpp uint8_t c=249`) → If the selected _mcu_ is not currently connected to the _server_.

On the other hand, if the _server_ validates the parameters, it will return a first ACK control query to the _client_ and forward the SRVP query to the target _mcu_. Once the _mcu_ performs the movement, it will reply with a control query to the _server_, which is then be relayed back to the _client_ as a second control response. For this second control query, the defined possible NACK codes are:

- _`_NACK_InvalidQuery`_ (`#!Cpp uint8_t c=255`) → **Should never happen:** If the [query format](Protocol%20Definition.md/#client-server-query-format) is invalid: contains null-bytes, invalid header or tail sequences, or the query function code is not recognized.
- _`_NACK_NoActiveMCU`_ (`#!Cpp uint8_t c=254`) → If _client_ has not selected a _mcu_ before issuing the movement command. Would happen if another _client_ selects the same _mcu_ as the current _client_.
- _`_NACK_MCUOffline`_ (`#!Cpp uint8_t c=249`) → If the selected _mcu_ is not currently connected to the _server_. Would happen if the _mcu_ disconnects between the _client_ query validation and the _server_ message to the _mcu_.
- _`_NACK_ErrorContactingMCU`_ (`#!Cpp uint8_t c=248`) → If the selected _mcu_ is not currently connected to the _server_.

During the second control response, both `_NACK_NoActiveMCU` and `_NACK_MCUOffline` represent time-of-check time-of-use conditions catched by the server during query dispatch.

## _Server → MCU_ Servo Movement Query Format

The servo movement query sent by the _client_ must be translated from the _client → server_ into the _server → mcu_ format before forwarding it to the _mcu_, which makes the query take the form:

=== "SmartMCU SRVP Query"

    ``` 
    -m-[NUMBER_OF_MOVEMENTS]-[SERVO_ID:SERVO_POS]-<...>-[SERVO_ID:SERVO_POS]-!
    ```

=== "DumbMCU SRVP Query"

    ``` 
    -m-[NUMBER_OF_MOVEMENTS]-[SERVO_ID:PWM_SIGNAL]-<...>-[SERVO_ID:PWM_SIGNAL]-!
    ```

Where the query parameters `[NUMBER_OF_MOVEMENTS]`, `[SERVO_ID:SERVO_POS]` and `[SERVO_ID:PWM_SIGNAL]` remain unchanged and adhere to the same rules as the _client → server_ format.

## PWM Signal codification

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



## Examples: SRVP Communication Overview

The servo movement query takes on a different communication path depending on whether the _server_ validation passes or fails. The possible schemas are:

=== "Successful SRVP Query"

    For instance, the movement query `#!Cpp !s-SRVP-2-9:13-7:18-e!` would translate to "_Perform 2 servo movements: move servo ID 8 to position 12, and servo ID 6 to position 17_". Assuming the parameters are valid, and the selected mcu is online, the query validation would succeed, returning the first ACK to the client while forwarding the SRVP query to the mcu. Once the mcu completes the movements, it will reply with a positive control query, which will be relayed back to the client as a second ACK.

    <figure markdown="span">
        ![](../assets/SRVP.png#only-light)
        ![](../assets/SRVP_w.png#only-dark)
        <figcaption/>
    </figure>

=== "Rejected SRVP Query"

    On the other hand, if the user sends the query `#!Cpp !s-SRVP-1-3:187-e!` the movement order (move servo id=2, to position 186) would be invalid, as the servo position is out-of-range. In this case, the server validation would catch it, returning a negative control code (`_NACK_InvalidParameter`) to the client, and ending the SRVP query "process". 
    
    <figure markdown="span">
        ![](../assets/iSRVP.png#only-light){width=690}
        ![](../assets/iSRVP_w.png#only-dark){width=690}
        <figcaption/>
    </figure>

=== "Rejected by MCU"

    The final option is that, once validated by the _server_, the _mcu_ deems the movement order invalid. This would lead to a positive first control response, followed by a negative control query. In this case, similar to the SRVP rejection by server, **no movement is performed**.
    
    <figure markdown="span">
        ![](../assets/iSRVPmcu.png#only-light)
        ![](../assets/iSRVPmcu_w.png#only-dark)
        <figcaption/>
    </figure>    
