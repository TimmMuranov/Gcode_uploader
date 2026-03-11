# Gcode_sender
This structure allows you to send gcode to the CNC.

## Example
``` c++
#include <iostream>
#include <string>
#include "uploader.hpp

int main(){
  std::string port = "/dev/ttyUSB0"; //change to your port
  Uploader cnc;
  cnc.connect(port);
  cnc.send("$H");
  cnc.waitForIdle(); // wait for the movements to complete
  return 0;
}
```
