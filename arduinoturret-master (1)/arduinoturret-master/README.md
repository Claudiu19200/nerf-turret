# arduinoturret
Some generic arduino turret with auto-aim using facial recognition.
It uses OpenCV 4.7.0 (the latest one as of writing this) and communicates with the Arduino controller using the Windows serial protocol (so compatibility with other OSes may not work).
The aiming process has been callibrated for [this 3D printed build from the internet](https://www.littlefrenchkev.com/bluetooth-nerf-turret) and having this custom code written for automatically aiming (instead of having it manually controlled).

### Build instructions
1. Compile *facerec.cpp* and *serial.cpp* using any Windows compiler (tested using Visual Studio IDE).
2. Upload *facerec_arduino.ino* to any Arduino board.

You should be able to now work with the turret.

### Usage
After the turret is powered on (especially the Arduino board) and the correct serial port is selected, you should be able to launch the compiled executable which is going to send commands to the microcontroller. The turret will follow the face. Press F to make the turret shoot.
