# MCU Information
---

The MCU Information query, whose code is `iMCU`, can be used by the _client_ to retrieve the latest _mcu_ data stored at the _server_.

The client → server query takes the form:
```
!s-iMCU-e!
```

While the server response, instead of using a standard [control query](Control%20Query.md), it mirrors the structure of the additional information from the [SmartMCU login query](Identification%20Query.md//#mcu-login-query):
```
!s-iMCU-[SERVO_COUNT]-[SERVOPOS(0)]-<...>-[SERVOPOS(SERVO_COUNT-1)]-e!
```
Where the variable `SERVO_COUNT` is a single `uint8_t` binary value representing the number of servos the selected mcu has, and each `SERVOPOS(i)` variable is a single `uint8_t` binary value in the range [1→180], for the current position of the servo with ID=i.

In the event an error occurs, the server response will use an error control query, with the following possible error codes:

- _`_NACK_InvalidQuery`_ (`#!Cpp uint8_t c=255`) → If the [query format](Protocol%20Definition.md/#client-server-query-format) is invalid: contains null-bytes, invalid header or tail sequences, or the query function code is not recognized.
- _`_NACK_NoActiveMCU`_ (`#!Cpp uint8_t c=254`) → If _client_ has not selected a _mcu_ before issuing the MCU Information command.

The possible communication schemas for this query take the following form:

<figure markdown="span">
  ![MCU Information comm schema](../assets/imcu.png#only-light)
  ![MCU Information comm schema](../assets/imcu_w.png#only-dark)
  <figcaption>Diagram for a successful and failed mcu information query</figcaption>
</figure>