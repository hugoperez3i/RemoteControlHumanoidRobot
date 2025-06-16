
---

The positive (ACK) and negative (NACK) control queries let the client know whether a requested action was successful or not. In case of failure, NACKs also provide context on what went wrong or where the error occurred.

These queries follow the standard protocol structure, using `_ACK` and `NACK` as their 4-byte function codes. They include a single byte `c` as the error code (for positive control queries, `c` will take always the value `#!cpp uint8_t c=255`)..

=== "Positive (Acknowledge) Query"

    ```
    !s-_ACK-c-e!
    ```

=== "Negative (No Acknowledge) Query"

    ```
    !s-NACK-c-e!
    ```

As such, the communication schema in which these queries will be used will look like the one illustrated on the following diagram.

<figure markdown="span">
  ![Control query comm schema](../assets/cntrlQuery.png#only-light)
  ![Control query comm schema](../assets/cntrlQuery_w.png#only-dark)
  <figcaption/>
</figure>