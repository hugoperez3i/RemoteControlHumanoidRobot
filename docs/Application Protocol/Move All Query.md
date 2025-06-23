# Move Servos (Delayed Mode) Query

The move servos query on delayed mode uses the message:

```
!s-mALL-e!
``` 

While acting under the _[delayed execution mode](Execution%20Mode.md)_, the _client_ can use this query to execute movements previously loaded through the normal [servo movement query](Servo%20Movement.md). 

The possible responses for this query by the server are either a relayed generic ACK [control query](Control%20Query.md) from the _mcu_ after movement execution, or a negative [control query](Control%20Query.md) with one of the following control codes:

- _`_NACK_InvalidQuery`_ (`#!Cpp uint8_t c=255`) → If the [query format](Protocol%20Definition.md/#client-server-query-format) is invalid: contains null-bytes, invalid header or tail sequences, or the query function code is not recognized.
- _`_NACK_NoActiveMCU`_ (`#!Cpp uint8_t c=254`) → If _client_ has not selected a _mcu_ before issuing the movement command.
- _`_NACK_OnRTMode`_ (`#!Cpp uint8_t c=253`) → If the _server_ is not operating on _delayed mode_.
- _`_NACK_InvalidParameter`_ (`#!Cpp uint8_t c=252`) → If no movements have been previously configured.
- _`_NACK_NoMCUInfo`_ (`#!Cpp uint8_t c=250`) → If the selected _mcu_ is a "[**DumbMCU**](Protocol%20Definition.md//#mcu-types)", and there's no PWM information available in the _server_ database.
- _`_NACK_MCUOffline`_ (`#!Cpp uint8_t c=249`) → If the selected _mcu_ is not currently connected to the _server_.

---

The _server → mcu_ portion of this query behaves and uses the same messages than the normal [servo movement query relayed to the mcu](Servo%20Movement.md/#dumbmcu-srvp-query_1), with the exception of the message sent to the **SmartMCU**, which uses a different query:

=== "SmartMCU mALL Query"
    ```
    -e-!
    ```
=== "DumbMCU mALL Query"
    ``` 
    -m-[NUMBER_OF_MOVEMENTS]-[SERVO_ID:PWM_SIGNAL]-<...>-[SERVO_ID:PWM_SIGNAL]-!
    ```

    - _`[NUMBER_OF_MOVEMENTS]`_ → A single `#!Cpp uint8_t` indicating how many servo movements the query contains. Since it cannot be zero, it's not affected by the offset. 
    - _`[SERVO_ID:PWM_SIGNAL]`_ → The information pair `SERVO_ID`, the id of the servo the client wants to move; and `PWM_SIGNAL`, the PWM signal for the given servo ID. These two values are separated by the ASCII character `:` (`#!Cpp uint8_t c=58`), and for multiple movements, each information pair `SERVO_ID:PWM_SIGNAL` is separated by a dash `-` (`#!Cpp uint8_t c=45`) from the information pair of the following servo movement. Both `SERVO_ID` (a single `#!Cpp uint8_t` value) and `PWM_SIGNAL` (a `#!Cpp uint16_t` value) must have [the positive +1 offset applied](Protocol%20Definition.md/#no-binary-zero-convention), as they can take value zero.

---

The communication schema for this query is as follows:

=== "Successful Query (SmartMCU)"
    <figure markdown="span">
        ![mALL comm schema](../assets/small.png#only-light)
        ![mALL comm schema](../assets/small_w.png#only-dark)
        <figcaption/>
    </figure>
=== "Successful Query (DumbMCU)"
    <figure markdown="span">
        ![mALL comm schema](../assets/mall.png#only-light)
        ![mALL comm schema](../assets/mall_w.png#only-dark)
        <figcaption/>
    </figure>
=== "Failed Query"
    <figure markdown="span">
        ![failed mALL comm schema](../assets/mallf.png#only-light){width=690}
        ![failed mALL comm schema](../assets/mallf_w.png#only-dark){width=690}
        <figcaption/>
    </figure>