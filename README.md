# Serpentia - Snake Game Run on Arduino
coming


**If you'd like to experiment with the prototype, you can interact with the design using this interactive [wokwi sketch](https://wokwi.com/projects/394153136625249281).**



<p align="center">
  <img src="img/IMG_6036.jpeg" alt="Breadboard Prototype with Arduino Uno" width="80%" height="80%">
</p>
<p align="center">
  Breadboard Prototype with Arduino Uno (clone with Hat).
</p>



## Setup 

While SCLK and DN must be connected to pins 13 and 11, the rest can use any combination of digital output pins but to be able to dim the light, you'd want to keep the LED connected to a PWM capable pin.

**Nokia5110 to Arduino:**
CE ----------------- 6

RST ---------------- 7

DC ----------------- 8
DN ----------------- 11
SCLK --------------- 13
LED ----330 Ohm----- 10
VCC ---------------- Vcc
GND ---------------- GND
