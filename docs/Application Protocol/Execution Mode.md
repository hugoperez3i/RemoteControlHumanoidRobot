# Execution Mode
---

The execution mode query is used by the _client_ to change the execution mode between _delayed_ and _real time_. It uses the query:

```
!s-eMOD-[EXECUTION_MODE]-e!
```

Where the `EXECUTION_MODE` variable is a single `uint8_t` binary value that can take two values:

- _`_eMOD_Delayed`_ (`#!Cpp uint8_t c=100`) → In order to change the execution mode to _delayed_.
- _`_eMOD_RealTime`_ (`#!Cpp uint8_t c=101`) → In order to change the execution mode to _real time_.

And the response from the server is either a positive control query, with the same code provided by the _client_ as confirmation, or a negative control query with the error codes:

- _`_NACK_InvalidQuery`_ (`#!Cpp uint8_t c=255`) → If the [query format](Protocol%20Definition.md/#client-server-query-format) is invalid: contains null-bytes, invalid header or tail sequences, or the query function code is not recognized.
- _`_NACK_InvalidParameter`_ (`#!Cpp uint8_t c=252`) → If `EXECUTION_MODE` has a value different than `#!Cpp uint8_t c=100` (delayed mode) or `#!Cpp uint8_t c=101` (real time mode).

The possible communication schemas for this query are:

<figure markdown="span">
  ![Execution mode comm schema](../assets/emod.png#only-light)
  ![Execution mode comm schema](../assets/emod_w.png#only-dark)
  <figcaption>Diagram for a successful and failed change of execution mode</figcaption>
</figure>

## Delayed Execution Mode

While operating under the _delayed execution mode_, the [servo movement query](Servo%20Movement.md) alters its functionality. Instead of relaying a servo movement query to the _mcu_, the _server_ will update its internal "target position". If the _mcu_ is of the type **SmartMCU**, the _server_ will send these movements for the _mcu_ to store, but not execute, using the same movement query but replacing the `m` movement code with a `u`:

``` 
-u-[NUMBER_OF_MOVEMENTS]-[SERVO_ID:SERVO_POS]-<...>-[SERVO_ID:SERVO_POS]-!
```

The query still follows a similar connection schema as the [servo movement query](Servo%20Movement.md) on _real time_ mode:

<figure markdown="span">
  ![delayed srvp comm schema](../assets/srvp_d.png#only-light)
  ![delayed srvp comm schema](../assets/srvp_d_w.png#only-dark)
  <figcaption>Diagram for a SRVP query on delayed mode</figcaption>
</figure>

This query will not be sent to a _mcu_ of the type **DumbMCU**. 

To execute movements while working under _delayed execution mode_, the client must first configure them by sending one or more [servo movement](Servo%20Movement.md) queries. After loading the movements, the client can issue a [move servos (delayed mode) query](Move%20All%20Query.md) to execute them.
