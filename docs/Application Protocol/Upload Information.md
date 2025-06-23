# Upload MCU Information Query

The upload mcu information query can be used by the _client_ to upload the _mcu_ PWM information. This information is required in order to operate a "[**DumbMCU**](Protocol%20Definition.md//#mcu-types)". The query uses the format:

``` 
!s-uINF-[SERVO_COUNT]-[PWM_MIN(0):PWM_MAX(0)]-<...>-[PWM_MIN(SERVO_COUNT-1):PWM_MAX(SERVO_COUNT-1)]-e!
```

- _`[SERVO_COUNT]`_ → A single `#!Cpp uint8_t` indicating how many servos the _mcu_ has. Since this number cannot be zero, it does not need any offset applied.
- _`[PWM_MIN:PWM_MAX]`_ → The information pair `PWM_MIN`, the minimum PWM signal position; and `PWM_MAX`, the maximum PWM signal for the given servo; which represent the calibration information for a given servo. These two PWM values are separated by the ASCII character `:` (`#!Cpp uint8_t c=58`), and for multiple movements, each information pair `PWM_MIN(i):PWM_MAX(i)` is separated by a dash `-` (`#!Cpp uint8_t c=45`) from the information pair of the following servo calibration. Both of the `PWM_SIGNAL` (`#!Cpp uint16_t` values) must have [the positive +1 offset applied](Protocol%20Definition.md/#no-binary-zero-convention), as they can take value zero.

The possible responses for this query by the server are either a generic ACK [control query](Control%20Query.md), or a negative [control query](Control%20Query.md) with one of the following control codes:

- _`_NACK_InvalidQuery`_ (`#!Cpp uint8_t c=255`) → If the [query format](Protocol%20Definition.md/#client-server-query-format) is invalid: contains null-bytes, invalid header or tail sequences, or the query function code is not recognized.
- _`_NACK_NoActiveMCU`_ (`#!Cpp uint8_t c=254`) → If _client_ has not selected a _mcu_ before issuing the command.
- _`_NACK_InvalidParameter`_ (`#!Cpp uint8_t c=252`) → If the query parameters are out of range: a servo id exceeds the selected _mcu_ servo count, or a PWM signal value out of the range [0→32766].
- _`_NACK_ServoCountMissmatch`_ (`#!Cpp uint8_t c=251`) → If the user tries to update information for a different number of servos than the selected _mcu_ has.
- _`_NACK_ErrorLoadingMINMAX`_ (`#!Cpp uint8_t c=247`) → If the PWM information could not be loaded into the _server_ database.

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

## uINF Communication Overview

<figure markdown="span">
    ![uint16](../assets/uinf.png#only-light)
    ![uint16](../assets/uinf_w.png#only-dark)
    <figcaption/>
</figure>