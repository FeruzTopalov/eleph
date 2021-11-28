# **ELEPH** - _Never Get Lost!_

<p align="center">
  <img src="Supplementary/Pictures/Eleph_logo.png" width="256" height="256">
</p>

<p align="center">
  <i>Dedicated to my friend Alexander Sadaev, rock climber.</i>
</p>

Eleph is a GPS-based Local Relative Navigation System (LRNS) for group of up to 6 members. It allows you to share your positional data in **real-time** among other Eleph-devices and navigate to each of them in a relative manner by providing **Distance** and **Azimuth** information - like RADAR does.





## Demonstration

Before diving into the details, watch the demo on **YouTube**: <https://youtu.be/EMacKdg9Z_0>

![YouTube Video!](Supplementary/Pictures/Preview.jpg)






## Problem & Solution

As long as you stay under cell-coverage a navigation, either absolute or relative, is not a problem at all. You just use your phone with maps and messengers to find or share locations. Things change when you go off-grid. Hiking, skiing, sailing or whatever outdoor activity are a potential place to lose sight of your teammates. How to navigate in the group? Hopefully, there are couple of solutions available on the market:

* [LynQ](https://lynqme.com/pages/dev-consumer)
* [XQUAD](https://www.indiegogo.com/projects/xquad-smart-location-tracking-without-phones#/)
* [GoFindMe](https://www.indiegogo.com/projects/gofindme-a-gps-tracker-works-without-cell-service/#/)
* [GoTenna](https://gotennamesh.com/products/mesh)
* Also [GARMIN Dog collar](https://buy.garmin.com/en-US/US/c12522-p1.html)
* Also any [AIS](https://en.wikipedia.org/wiki/Automatic_identification_system) system

unfortunately all of them are commercial. But not the **Eleph**! 

**Eleph** is _open-source/open-hardware_ device which contains all major features needed for group navigation, such as:

* Detailed absolute positional information about each device in group
* RADAR-like interface with relative positional information about each device in group
* Memory slots to hold the position of any device and navigate it later
* GeoFence feature
* Timeout feature
* Alarm button

Due to open-source and simple and cheap hardware, Eleph is highly flexible solution that could be adapted to any use case you want:

* Hiking
* Climbing
* Sailing
* Hunting with the dogs
* Assets tracking
* and more...







## Technology

Eleph is written in pure C and runs on STM32 microcontroller. It has GPS module to receive positional data and synchronization signal, radio transceiver to exchange with radio packets between other devices, graphical display to show information on it, EEPROM memory IC to store coordinates, tactile buttons to control the device, buzzer to notify about events and couple of LEDs.

![Packet structure](Supplementary/Pictures/Main_components.jpg)

GPS module provides NMEA-0183 stream at 9600 baud. Microcontroller process the stream using DMA and parse it. Fields being extracted are RMC, GGA, GSA, GSV. Those give us information about time, date, latitude, longitude, speed, course, altitude, satellites in view and in use, navigation mode and validness of data.

GPS module also provides time synchronization signal - PPS. It is used as a time reference for the transmitting and receiving radio packets inside a current group of devices. Each group operates at the specific frequency channel, the way like regular radios. Each device in a group has unique predefined number from 1 to 6, so there are 6 members in a group maximum. Eleph uses TDMA technique to give channel access for each group member, so the device number corresponds to the time-slot occupied by device. There are 125 ms timer which starts counting from the rise of the PPS pulse. First 125 ms time-slot is used to parse and prepare positional data of the device. Next six time-slots are used to exchange with the positional data between devices via radio. A device is in TX state when time-slot number is equal to the device number, and in RX state otherwise. Remaining time before next PPS is reserved for relative positions calculation and displaying the results.

Eleph uses FSK transceiver and operates in LPD 433 MHz band (please make sure you are allowed to use those frequencies in your region, change otherwise). Data rate is 2400 bps, deviation is 2400 Hz (mod index = 2), channel spacing is 25 kHz. Packet structure is shown below. It consist of 29 bytes total and takes ~97 ms to be transmitted over-the-air.

![Packet structure](Supplementary/Pictures/Radio_packet_structure.png)

In order to calculate relative positions of other devices, Eleph uses two formulas. First one is [Haversine formula](https://en.wikipedia.org/wiki/Haversine_formula) which determines the distance between two points on a sphere. The second one is [Loxodrome formula](https://en.wikipedia.org/wiki/Rhumb_line) which determines bearing to a point on a sphere relative to true north. All calculations are performed with a highest possible, double precision, so the results are in a great agreement with my simulations on PC. Precision of the formulas is another matter. It is known that haversine formula is numerically better-conditioned for small distances, but on the other side it doesn't take into account ellipsoidness of the Earth and uses it's mean radius. Also it is known that loxodrome (or rhumb line) shows not-the-shortest way to reach point of interest (the shortest way is provided by orthodrome, or great circle route), but the constant bearing, which is more useful in practice. Anyway, my simulations using Google Earth Pro ruler leave me no concerns about accuracy of used formulas. According to them the error for both is less than 1% as long as the distance is smaller than 300 km (!).

![Mercator](Supplementary/Pictures/Mercator_projection.png)

<p align="center">
  <i>(courtesy of <a href="https://www.esri.com/arcgis-blog/products/product/mapping/mercators-500th-birthday/">ESRI</a>)</i>
</p>

---





## FAQ

* _**Why is it called Eleph?**_ -Eleph stands for elephant. Elephants are a social animals, they form close bonds and stay in-touch - that is the main aim of the Eleph device.
* _**Why no LoRa yet?**_ -Believe or not, but i see no reason to use LoRa in the Eleph right now. Mainly due to low data rate. Probably next time, when i shorten payload, i will switch to LoRa mode.
* _**Why there is no compass?**_ -When i started the project i thought that COG (Course over Ground) information from the GPS would be enough, and that the COG relates to the true north, while compass gives magnetic north. Obviously information about true north is more valuable. The drawback for now is that COG requires you to move, there is no way to find the direction standing still.
* _**Any plans to resume the project?**_ -Next milestone is to rework user interface, make it simpler; refuse from excessive payload information, leaving just Lat/Lon data; update schematic/hardware. 
* _**How can i support the project?**_ -Eleph really needs your support. Main functionality is tested, but full and comprehensive outdoor tests are nor performed well yet. You could build it, test it on your side and report an issue if so - that is the best way to support.
