
---

The server shutdown message's only purpose is to remotely disconnect and shut down the server. It uses the fixed format:

```
!s-sOFF-e!
```

If successful, the server replies with an ACK control query. If it fails, mostly due to invalid format or a similar issue, it will return with a NACK: _`_NACK_InvalidQuery`_ (`#!Cpp uint8_t c=255`).
As such, the communication schema for this query will look like the one illustrated on the following diagram.

<figure markdown="span">
  ![Server OFF comm schema](../assets/shutdown.png#only-light){width=600}
  ![Server OFF comm schema](../assets/shutdown_w.png#only-dark){width=600}
  <figcaption/>
</figure>
