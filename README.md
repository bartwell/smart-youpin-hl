# Smart Xiaomi Youpin HL
Make your Aromadiffuser controllable remotely

[![Aromadiffuser photo](https://github.com/bartwell/smart-youpin-hl/raw/main/images/aromadiffuser.jpg)](https://github.com/bartwell/smart-youpin-hl/raw/main/images/aromadiffuser.jpg) 

## Features

Using MQTT you can:
* Get state
* Switch on aromadiffuser 
* Switch off it
* Change light color

## Components

1. D1 Mini
2. 2x 470 ohm resistors
3. 10 kiloohm resistor
4. 2x PC817 optrons

[![Optron photo](https://github.com/bartwell/smart-youpin-hl/raw/main/images/optron.jpg)](https://github.com/bartwell/smart-youpin-hl/raw/main/images/optron.jpg) 

## Assembling

1. Open the sketch in Arduino IDE and set WiFi and MQTT broker credentials. Connect D1 Mini to computer and upload the sketch. 
2. Disassemble aromadiffuser.
3. Connect 10 kiloohm resistor to GND and D6. 
4. Connect led of first optron with 470 ohm resistor to fan contacts. Connect optotransistor to D6 and 3.3v. It's allow to get aromadiffuser state.
5. Connect led of second optron with 470 resistor to D7 and GND. Connect optotransistor to button contacts.
6. Provide power for D1 Mini from USB of aromadiffuser. 

Now you can assemble aromadiffuser. D1 Mini perfectly fit under the main board. 

## Usage

* state is available in state topic (on, off and error for low water level)
* to switch on send press to command topic
* to change color send press to command topic
* to switch off send press_long to command topic

