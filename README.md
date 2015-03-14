# custom-pic-quad
= Introduction =

This project is about building Microchip PIC-based (PIC24EP512GP806) flight control board and respective software from the board design to the flight. 
All the code is developed from scratch with no third-party libraries and other "black-boxes". 

I am working on completely independent (not a port of existing autopilots) quadrocopter firmware for Microchip PIC24 microprocessors.
The project includes full source code of all the libraries (I2C, UART, ADC, RC Receiver) written from the ground up to be interrupt-based, 
which allowed to implement control loop that can operate at up to 500 Hz.

My quad is flying and I am planning to concentrate now on additions to the project to include more sensors like ultrasonic range finder, 
GPS, barometer as well as testing new RC components like RC receivers with the UART output.


= Details =

If you are interested in this project and would like to see more details about the board that I am working on, the SW presented here, and the project overall, please check my blog http://flightcontroller.blogspot.com/.
