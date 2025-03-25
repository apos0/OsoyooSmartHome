# Osoyoo Smart Home Kit

This code is designed for the **Osoyoo Smart Home IoT Learning Kit with Mega2560**. For more details, visit the official page: [Osoyoo Smart Home Kit](https://osoyoo.com/2019/10/18/osoyoo-smart-home-iot-learning-kit-with-mega2560-introduction/)</br>

**Note:** This project was created in a short time and will not be updated
### Connections Outline

```
D02 - DHT11 (Temperature & Humidity Sensor)
D03 - Buzzer
D04 - Green LED
D05 - White LED
D08 - Yellow LED
D09 - Red Button (Alarm Reset - Fire)
D10 - PIR Sensor (Motion Detection)
D11 - Servo (Door Lock)
D12 - Blue Button (Alarm Reset - Gas)
D13 - Red LED

A0 - Sound Sensor
A1 - Flame Sensor
A2 - LDR Sensor (Light Detection)
A3 - MQ2 Sensor (Gas Detection)
```

### LCD Display Layout

```
|----------------|
|ZONE X:  UNARMED|
|DOOR:     LOCKED|
|----------------|

|----------------|
|TMP|HUM|GAS|LIGH|
|XX°C|XX%|XX%|XX%|
|----------------|
```

## Operating Modes

- **Fire Detection:** The system overrides and triggers the alarm. The alarm stops when the **Red Button** is pressed.
- **Gas Leakage Detection:** The system overrides and triggers the alarm. The alarm stops when the **Blue Button** is pressed.
- **Brightness Control:** The LDR sensor adjusts the brightness of the external **White LED**.
- **RFID Control:** Passing an **RFID card** over the sensor **locks/unlocks the door**.

### Zone Selection & Security System

- Press **A, B, or C** to select a zone.
- Enter the **4-digit password ("1234")** to **ARM/DISARM** the selected zone.
- Press **D** to display Temperature, Humidity, Gas Percentage*, and Light Percentage*.

*Gas and Light percentages are calculated based on a maximum ADC value of **1024**, which is the highest reading from an Arduino analog input.

### Security Zones & Triggers

- **Zone A - PIR Sensor:** Triggers when motion is detected.
- **Zone B - Sound Sensor:** Triggers when loud noises are detected.
- **Zone C - Ultrasonic Sensor:** Triggers when an object gets **closer than 5 cm**.


<img src="media/media1.jpg" width="49%">
<img src="media/media2.jpg" width="49%">
<img src="media/media3.jpg" width="49%">
<img src="media/media4.jpg" width="49%">