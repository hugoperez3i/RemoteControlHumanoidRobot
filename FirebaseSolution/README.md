# Firebase Solution

This dir contains the java application used to remotely operate the robot through Firebase. This app serves as a proxy server, exposing a similar control interface as the server solution. 

The dir `/testcli/` contains a simple cpp application that connects to this proxy server and, through it, is able to control any robot using the Firebase solution.

---

**DISCLAIMER:** This solution was discarded once the delay was deemed too high for remote operation in real time, as such, this part of the project should be considered both deprecated and unstable. It _should_ still work, but this is not guaranteed.

---