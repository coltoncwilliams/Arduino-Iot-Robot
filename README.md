# Arduino-Iot-Robot
### This project documents a fun little robot with an OLED face that says whatever you want. It has a servo steering in the front and DC motor to drive, with an Arduino Nano 33 iot for brains using the Adafruit GFX and WiFiNINA libraries

The nano is attached to a breadboard for easy connection to the motors and display. The DC motor is attached to pin 4 with a PNP transister for more current, servo to pin 9, and display to A4 and A5. There is also a 4 AA battery pack on the back, connected to vin and ground pins for enough cuurent, as shown below:
![Top](./media/top.jpg)


When booted up, the robot will display "connecting" and then an IP address for the user to go to, to access the website
![Front-1](./media/front-1.jpg)
![Front-2](./media/front-2.jpg)


The website will appear on a local IP address, using GET commands to control the robot. With a static IP, port forwarding can be used to control it from anywhere!
Screenshot of the website from a mobile device:
![Website](./media/website.png)


When a user does connect, the robots face and text can be seen:
![Front 3](./media/front-3.jpg)

Here it is in action!  
[Controlling movement](./media/controlling-movement.MOV)  
[Controlling display](./media/controlling-display.MOV)  

