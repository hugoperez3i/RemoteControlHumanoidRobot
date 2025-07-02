# Code for the MCU

This dir contains all the code used throught the project on the mcu. It's divided into the following sections:

- `/robotControl_Calibration/` - **Contains the code used to calibrate the robot's servos.**
- `/robotControl_Firebase/` - **Contains all code related to the Firebase solution.**
    - `/directConnection/` - Access individually each value when needed.
    - `/usingJSON/` - Downloads the whole database each cycle, and checks locally each value.
- `/robotControl_Server/` - **Contains the code required for the custom telerobotic platform.**