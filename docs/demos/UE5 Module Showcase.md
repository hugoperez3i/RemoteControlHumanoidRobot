# UE5 Module Showcase: RemoteClientSystem

This demo intends to show the RemoteClientSystem (RCS) module developed to incorporate this platform's controls into the Unreal Engine 5 ecosystem. 

---

## Function Suite

The RCS module abstracts away the platform communication protocols into a set of Blueprint-friendly functions that make possible integration of the robotic platform interface into Unreal's scripting system.

![UE5 Blueprint with RCS functions](../assets/RCS%20Functions.png)

--- 

## Support Data Structures

In order to increase the compatibility and make the usage of the RCS module possible on the Blueprint system, it incorporates the `#!Cpp struct FServoInfo`, which combines the `#!Cpp uint8 servoId` and `#!Cpp uint8 servoPosition` information into a simple, Blueprint compatible data structure.

![UE5 Blueprint with RCS FServoInfo struct](../assets/RCS%20FServoInfo.png)

RCS also incorporates `#!Cpp enum ECLIErrorCode`, an enum listing the possible error codes returned by this module's functions. This code can be retrieved using the `#!cpp GetErr()` Blueprint function, and filtered against an enum switch, simplifying error handling on UE Blueprints.

<figure markdown="span">
    ![UE5 Blueprint with RCS ECLIErrorCode enum](../assets/RCS%20ECLIErrorCode.png){align=center}
    <figcaption/>
</figure>

--- 

## RCS Demo

A brief video showcasing how the UE5 RemoteClientSystem can be used to operate the Youbionic Robot, in this case performing head rotation on each movement order.

<div align="center">
<video width="400"  controls muted> 
    <source src="../../assets/demoUE5.mp4" type="video/mp4">
</video>
</div>

### Demo Blueprint used

The Blueprint used for the demo follows a simple logic: check the `ServoID=26` position, if it's greater than 90, set its value to zero; otherwise, if it's under 90, set it to 179. After this step, the movement order is sent to the server, which in turn makes the Youbionic Half perform the head rotation movement.

<figure markdown="span">
    ![UE5 Blueprint for the demo video](../assets/RCS%20demoBlueprint.png){align=center}
    <figcaption/>
</figure>