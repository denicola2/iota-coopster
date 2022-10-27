# IoT-Anywhere Coopster
Chicken Coop monitor based on the WisBlock RAK4631, RAK1906, and the Grove Vision AI module, using the [WisBlock-API library](https://github.com/beegee-tokyo/WisBlock-API)

## Hardware Used
Here is the list of hardware used, substitutions can be made based on use-case
- [WisBlock Base RAK5005-O](https://docs.rakwireless.com/Product-Categories/WisBlock/RAK5005-O/Datasheet/)
- [WisBlock Core RAK4631](https://docs.rakwireless.com/Product-Categories/WisBlock/RAK4631/Datasheet/)
- [WisBlock Environmental Sensor Module RAK1906](https://docs.rakwireless.com/Product-Categories/WisBlock/RAK1906/Datasheet/)
- [WisBlock Sensor Adapter Module RAK1920](https://docs.rakwireless.com/Product-Categories/WisBlock/RAK1920/Datasheet/)
- [Grove Vision AI Module](https://wiki.seeedstudio.com/Grove-Vision-AI-Module/)

## Vision AI Dataset
A public dataset from [Roboflow Universe](https://universe.roboflow.com/) was loaded onto the Grove Vision AI module via the instructions provided [HERE](https://wiki.seeedstudio.com/Train-Deploy-AI-Model-Grove-Vision-AI/#jump1)

## Sample Serial Output
```
[APP] Timer wakeup
[BME] Timestamp [ms], raw temperature [C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [C], relative humidity [%], Static IAQ, CO2 equivalent, breath VOC equivalent
[BME] 146639, 25.23, 100913.00, 53.70, 2949.00, 25.00, 0, 25.23, 53.70, 25.00, 500.00, 0.50
[AI] Time consuming: 710
[AI] Number of chickens: 1
[AI] result:detected
[AI] Detecting and calculating: 1
[AI] confidence: 53
[APP] Packet enqueued
[APP] LPWAN TX cycle finished ACK
```
