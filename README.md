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
[APP] Setup IoT-Anywhere Coopster
[APP] Initialize Grove AI
[AI] AI Detection Setup Begin
[AI] Version: 256
[AI] ID: 256
[AI] Algo: 0
[AI] Model: 16
[AI] Confidence: 50
[APP] Result success
[APP] Initialize BME680
[BME] BSEC library version 1.4.8.0
[APP] Result success
[APP] Successfully joined network
[APP] Timer wakeup
[BME] Timestamp [ms] : 907005
[BME] Raw Temp  [C]  : 29.55
[BME] Pressure  [hPa]: 103145.00
[BME] Raw RH    [%]  : 28.72
[BME] Gas       [Ohm]: 5161.00
[BME] IAQ            : 25.00
[BME] IAQ Accuracy   : 0
[BME] Temp      [C]  : 29.55
[BME] RH        [%]  : 28.72
[BME] Statuc IAQ     : 25.00
[BME] CO2            : 500.00
[BME] Breath VOC     : 0.50
[AI] Time consuming: 710
[AI] Number of chickens: 1
[AI] result:detected
[AI] Detecting and calculating: 1
[AI] confidence: 53
[APP] Packet enqueued
[APP] LPWAN TX cycle finished ACK
```
