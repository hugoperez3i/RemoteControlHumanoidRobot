# MCU Selection
---

The robot selection query (switch MCU, using the 4-byte code `sMCU`) is used by the _client_ to inform the _server_ which _mcu_ it wants to control. For this, the query uses a single additional information field, `[MCU_UNIQUE_NAME]`, containing the unique identifier the _mcu_ used at _server_ login. The query structure will be:

```
!s-sMCU-[MCU_UNIQUE_NAME]-e!
```

On a successful request, the server will return a positive control query. While if there's an error, the server will return one of the following error codes:

- _`_NACK_InvalidQuery`_ (`#!Cpp uint8_t c=255`) → If the [query format](Protocol%20Definition.md/#client-server-query-format) is invalid: contains null-bytes, invalid header or tail sequences, or the query function code is not recognized.
- _`_NACK_NoActiveMCU`_ (`#!Cpp uint8_t c=254`) → If the variable `[MCU_UNIQUE_NAME]` is empty (`!s-sMCU--e!`), or if the given `[MCU_UNIQUE_NAME]` identifier does not exist on the _server_ database.

As such, the communication schema for this query type will look like the one illustrated on the following diagram.

<figure markdown="span">
  ![MCU Selection comm schema](../assets/mcuSelect.png#only-light)
  ![MCU Selection comm schema](../assets/mcuSelect_w.png#only-dark)
  <figcaption>Diagram for a successful and failed mcu selection query</figcaption>
</figure>
