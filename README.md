# nodeMCU wifi-nameplate

This is a low-cost, easy to build and configure wifi-nameplate based on the nodeMCU's ESP-8266-Chipset.

You can change the displayed text and the busy/free-status easilie by browser. The status can be changed also manually by pushing the push-button :-)

## Partlist

* **nodeMCU v3** (1-3 MB Flash)
* **i2c Converter-Board for LQ-Displays**
* **20x04 Liquid-Crystal Display** (only optimized for this size)
* **Push-Button**
* **2 x Resistor** 220 Ohm or 330 Ohm 
* **1 x Resistor** 10 kOhm
* **red LED**
* **green LED**

## Wiring

The display uses the 5 V VIN-Pin for powering.
The Push-Button Pin is externally pulled up by a 10k-Resistor.

![Bootscreen](https://github.com/Einstein2150/nodeMCU-wifi-nameplate/blob/master/Images/fritzing.png?raw=true)

##Configuration

There are 4 Files in the Project-Directory presenting the default configuration for each line.

Change the wifi-configuration to your settings and the individual i2c-hex-adress of your display.

##Pictures

###Bootsplash

![Bootscreen](https://github.com/Einstein2150/nodeMCU-wifi-nameplate/blob/master/Images/Disp_booting.jpg?raw=true)

###Webserver-GUI

![Bootscreen](https://github.com/Einstein2150/nodeMCU-wifi-nameplate/blob/master/Images/webserver_free.png?raw=true)

###Display in action

![Bootscreen](https://github.com/Einstein2150/nodeMCU-wifi-nameplate/blob/master/Images/Disp_free.jpg?raw=true)

![Bootscreen](https://github.com/Einstein2150/nodeMCU-wifi-nameplate/blob/master/Images/Disp_busy.jpg?raw=true)