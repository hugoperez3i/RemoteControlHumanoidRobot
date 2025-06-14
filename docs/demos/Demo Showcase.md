# Demo Showcase: Scripted actions

A simple showcase of the telerobotic platform working, in which the Youbionic Half performs a scripted movement.

## Showcase Video 
<div align="center">
<video width="480" height="480" controls>
    <source src="../../assets/demoMovs.mp4" type="video/mp4">
</video>
</div>
---

## Demo walkthrough

This demo intends to show how the platform operates. For this task a simple client is used to control the Youbionic Half robot, and perform a series of scripted movements that resemble the action of looking towards your hand, open and close it, and get back to your original position.

### Server Connection

The first part of the cpp script, available on the [next section](#code-for-the-demo), creates a client that connects to a given IP at the predefined port `57573`:

``` cpp
/* Start connection */
Client c(ip);
    usleep(500000);
```

### Robot Selection

Then the script selects the robot it wants to control, in this case _Maroon_ - the ID used by the Youbionic Half robot in our setup, using the `#!cpp connectToMCU()` function:

``` cpp
/* Select the mcu to control */
std::cout<<c.connectToMCU("Maroon") <<"\n";
    usleep(2500000); // Wait 2.5 seconds
```

### Movement Configuration and Execution

For each movement, the client needs to generate a `#!cpp std::vector<int>` in which to store the desired movements. For this client implementation, a `#!cpp uint32_t flag` bitmask is also needed. This flag serves as an efficient way of indicating `#!cpp moveServos()` the IDs of the servos that we want to move.

```cpp
/* Combined movement 1 - Rise hand towards face, tilt head */
std::vector<int> pV1;{
    flag=0;
    pV1.push_back(0); // rotate palm towards face
    flag = (flag | (1<<13)); 
    pV1.push_back(0); // flex arm towards face
    flag = (flag | (1<<15)); 
    pV1.push_back(100); // Rotate arm towards body
    flag = (flag | (1<<17)); 
    pV1.push_back(179); // Tilt head towards hand
    flag = (flag | (1<<25)); 
    pV1.push_back(141); // Rotate head towards hand
    flag = (flag | (1<<26)); 
}
```
Lastly, the `#!cpp moveServos()` function is used to send the movement order to the server, which will validate it and provide a two-step response (assuming it passes validation). The first ACK will be "intercepted" by `#!cpp moveServos()`, which confirms that the order is valid, and that it will be sent to the robot for execution. The second response is read by the `#!cpp readServerResponse()`, and it corresponds to the movement confirmation by the robot MCU.

```cpp
std::cout<<c.moveServos(flag,pV1) <<" - Servo movemet confirmation\n";
std::cout<<c.readServerResponse() <<" - MCU confirmation of movement\n";
    usleep(1500000); // Wait 1.5 seconds
```

After every code section, the function `#!cpp usleep()` is used to introduce a delay between instructions. While during normal usage this might not be required, in this script it comes in handy to force a minor pause between each block execution - making the individual movements much more obvious and deliberate.

---

## Code for the Demo

Below is the main.cpp used for the demo. The complete source files for the client can be found in [the TestClient dir of this github repo](https://github.com/hugoperez3i/RemoteControlHumanoidRobot/blob/main/ServerSolution/TestClient/).


``` cpp title="main.cpp" linenums="1"
#include "Client.h"

#include <iostream>
#include <thread>
#include <unistd.h>


int main(int argc, char const *argv[]){

    uint32_t flag=0;

    /* Start connection */
    Client c(ip);
        usleep(500000);

    /* Select the mcu to control */
    std::cout<<c.connectToMCU("Maroon") <<"\n";
        usleep(2500000); // Wait 2.5 seconds


    /* Combined movement 1 - Rise hand towards face, tilt head */
    std::vector<int> pV1;{
        flag=0;
        pV1.push_back(0); // rotate palm towards face
        flag = (flag | (1<<13)); 
        pV1.push_back(0); // flex arm towards face
        flag = (flag | (1<<15)); 
        pV1.push_back(100); // Rotate arm towards body
        flag = (flag | (1<<17)); 
        pV1.push_back(179); // Tilt head towards hand
        flag = (flag | (1<<25)); 
        pV1.push_back(141); // Rotate head towards hand
        flag = (flag | (1<<26)); 
    }
    std::cout<<c.moveServos(flag,pV1) <<" - Servo movemet comprobation\n";
    std::cout<<c.readServerResponse() <<" - MCU confirmation of movement\n";
        usleep(1500000); // Wait 1.5 seconds

    /* Combined movement 3 - Close hand */
    std::vector<int> pV3;{
        flag=0;
        pV3.push_back(0); // Thumb
        flag = (flag | (1<<6)); 
        pV3.push_back(0); // Index
        flag = (flag | (1<<7)); 
        pV3.push_back(0); // Middle
        flag = (flag | (1<<8)); 
        pV3.push_back(0); // Ring
        flag = (flag | (1<<9)); 
        pV3.push_back(0); // Little
        flag = (flag | (1<<10));     }
    std::cout<<c.moveServos(flag,pV3) <<" - Servo movemet comprobation\n";
    std::cout<<c.readServerResponse() <<" - MCU confirmation of movement\n";
        usleep(1500000); // Wait 1.5 seconds

    /* Combined movement 4 - rest hand */
    std::vector<int> pV4;{
        flag=0;
        pV4.push_back(90); // Thumb
        flag = (flag | (1<<6)); 
        pV4.push_back(90); // Index
        flag = (flag | (1<<7)); 
        pV4.push_back(90); // Middle
        flag = (flag | (1<<8)); 
        pV4.push_back(90); // Ring
        flag = (flag | (1<<9)); 
        pV4.push_back(90); // Little
        flag = (flag | (1<<10)); 
    }
    std::cout<<c.moveServos(flag,pV4) <<" - Servo movemet comprobation\n";
    std::cout<<c.readServerResponse() <<" - MCU confirmation of movement\n";
        usleep(1500000); // Wait 1.5 seconds
        
    /* Combined movement 5 - Move arm and head back to "resting" position */
    std::vector<int> pV5;{
        flag=0;
        pV5.push_back(90); // rotate palm towards face
        flag = (flag | (1<<13)); 
        pV5.push_back(60); // flex arm towards face
        flag = (flag | (1<<15)); 
        pV5.push_back(70); // Rotate arm towards body
        flag = (flag | (1<<17)); 
        pV5.push_back(126); // Tilt head towards hand
        flag = (flag | (1<<25)); 
        pV5.push_back(100); // Rotate head towards hand
        flag = (flag | (1<<26)); 
    }
    std::cout<<c.moveServos(flag,pV5) <<" - Servo movemet comprobation\n";
    std::cout<<c.readServerResponse() <<" - MCU confirmation of movement\n";
        usleep(2500000); // Wait 2.5 seconds
        
}
```