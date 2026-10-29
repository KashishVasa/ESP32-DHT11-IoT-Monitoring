# ESP32 DHT11 IoT Temperature and Humidity Monitoring

## Project Overview
This project uses an ESP32 microcontroller and a DHT11 sensor to monitor temperature and humidity. The readings are displayed on a live web dashboard that can be accessed through a web browser on the same network.

## Objectives
- Measure temperature and humidity using the DHT11 sensor.
- Process sensor data using the ESP32.
- Display live readings on a web dashboard.
- Explore IoT-based environmental monitoring.

## Components Used
- ESP32 development board
- DHT11 temperature and humidity sensor
- Jumper wires
- USB cable
- Computer and Wi-Fi connection

## Features
- Live temperature monitoring
- Live humidity monitoring
- Web-based dashboard
- Wireless connectivity using ESP32
- Real-time sensor data visualization

## Software and Technologies
- Arduino IDE
- Embedded C/C++ (Arduino framework)
- ESP32 Wi-Fi
- HTML, CSS and JavaScript for the dashboard, if used in the project

## How It Works
1. The DHT11 sensor measures temperature and humidity.
2. The ESP32 reads the sensor values.
3. The ESP32 connects to a Wi-Fi network.
4. The web server provides a dashboard displaying the readings.
5. Users can open the dashboard in a browser using the ESP32's IP address.

## How to Run
1. Install Arduino IDE.
2. Install the ESP32 board package.
3. Install the DHT sensor library required by the code.
4. Open `ESP32_DHT11_IoT.ino` in Arduino IDE.
5. Enter your Wi-Fi credentials locally.
6. Select the correct ESP32 board and COM port.
7. Upload the code to the ESP32.
8. Open Serial Monitor to find the ESP32's IP address.
9. Open that IP address in a browser connected to the same network.

## Applications
- Indoor temperature monitoring
- Humidity monitoring
- Smart home projects
- IoT learning and experimentation

## Future Improvements
- Add historical graphs and data logging.
- Store readings in a cloud database.
- Add alerts for temperature or humidity thresholds.
- Improve dashboard design and mobile responsiveness.

## Author
Kashish Vasa

Electronics and Communication Engineering Student
