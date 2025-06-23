# Login Query
---

The login or identification query informs the _server_ of the type of node initiating the connection, whether it's a _client_ or an _mcu_.

This query poses an exception from the `4-byte code` structure, though it still follows the overall query structure. Instead of using said code, it relies on two constant sequences `Client_here` and `NodeMCU_here`, which represent the _client_ node and _mcu_ nodes respectively.

This query has a different behaviour depending on whether it was successful or not. When it succeeds, the server **will send no reponse** and, on failure, the _server_ will reply with a [control query](Control%20Query.md) containing an error code identifying the error. 
The possible error codes that may be returned by the server are:

- _`_NACK_InvalidQuery`_ (`#!Cpp uint8_t c=255`) → If the [query format](Protocol%20Definition.md/#client-server-query-format) is invalid: contains null-bytes, invalid header or tail sequences, or the query does not follow the structure for the _client_ or _mcu_ login queries.

---

## Client Login Query

The client login query is very straightfoward: The _client_ sends the login query and, assuming there's no unexpected/unknown errors on server-side, the _client_ will be logged to the _server_ successfully. Otherwise, a [negative control query](Control%20Query.md) will be returned, and the connection will be closed.

The login query is a constant sequence of characters with the form: `!s-Client_here-e!`.

<figure markdown="span">
  ![Client login comm schema](../assets/cliLogin.png#only-light)
  ![Client login comm schema](../assets/cliLogin_w.png#only-dark)
  <figcaption>Diagram for a successful and failed client login query</figcaption>
</figure>

---

## MCU Login Query

On the other hand, the mcu login query is a little bit more complex. The query will follow a structure similar to the _client_ one, using the constant sequence `!s-NodeMCU_here-[info]-e!` but with an additional information field, `[info]`, added to the message. This additional field will contain information about the _mcu_, such as the **unique id** or name, used to identify the _mcu_; the **number of servos**, which indicates how many servos the _mcu_ has; and either a fixed byte value (`#!Cpp uint8_t c=187`), if the _mcu_ is of the type [DumbMCU](Protocol%20Definition.md/#mcu-role); or the current position of each of the _mcu_ servos, if the _mcu_ is of the type [SmartMCU](Protocol%20Definition.md/#mcu-role).

=== "SmartMCU Login Query"

    ```
    !s-NodeMCU_here-[MCU_UNIQUE_NAME]-[SERVO_COUNT]-[SERVOPOS(0)]-<...>-[SERVOPOS(SERVO_COUNT-1)]-e!
    ```

    - _`[MCU_UNIQUE_NAME]`_ → The unique id used to identify the _mcu_ (`#!Cpp char[]`).
    - _`[SERVO_COUNT]`_ → A single `#!Cpp uint8_t` indicating how many servos the _mcu_ has. Since this number cannot be zero, it does not need any offset applied.
    - _`[SERVOPOS(i)]`_ → The position of each mcu servo (as a `#!Cpp uint8_t`), with a dash `-` between each of them, and ordered by ascending servo ID. Since this value can be zero (0-179 range), [by protocol it needs to be offset by +1](Protocol%20Definition.md/#no-binary-zero-convention).

=== "DumbMCU Login Query"

    ```
    !s-NodeMCU_here-[MCU_UNIQUE_NAME]-[SERVO_COUNT]-[187]-e!
    ```

    - _`[MCU_UNIQUE_NAME]`_ → The unique id used to identify the _mcu_ (`#!Cpp char[]`).
    - _`[SERVO_COUNT]`_ → A single `#!Cpp uint8_t` indicating how many servos the _mcu_ has. Since this number cannot be zero, it does not need any offset applied.
    - _`[187]`_ → The byte `#!Cpp uint8_t b=187`. Indicates the server that the _mcu_ is of the type **DumbMCU**.

<figure markdown="span">
  ![Client login comm schema](../assets/mcuLogin.png#only-light)
  ![Client login comm schema](../assets/mcuLogin_w.png#only-dark)
  <figcaption>Diagram for a successful and failed mcu login query</figcaption>
</figure>
