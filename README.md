# RFID Attendance and Access Control System 🎫🔐

An automated attendance and access control solution using RFID technology, built with ESP32 and MFRC522 RFID reader for seamless and secure user authentication.

## Features ✨
- 📡 Real-time attendance tracking with ESP32 + MFRC522 RFID reader
- 🗄️ Dual database integration: **MongoDB** & **MySQL** for secure user authentication and data management
- 📱💻 Dual-access control with a mobile application and web interface for flexible management
- 🔒 Secure user authentication ensuring reliable attendance and access control

## Hardware Components 🛠️
- ESP32 microcontroller (e.g., ESP32 DevKit V1)
- MFRC522 RFID reader module
- RFID tags/cards
- Optional: OLED display, buzzer, LEDs for status indication

## Software Components 💻
- Arduino IDE or PlatformIO for ESP32 firmware development
- MongoDB and MySQL for backend data storage
- Mobile app and web interface (details in `/app` folder or link to repo if separate)

## Wiring & Pin Configuration 🔌
| RFID Module Pin | ESP32 Pin | Description                |
|-----------------|-----------|----------------------------|
| SDA             | GPIO 5    | SPI Chip Select            |
| SCK             | GPIO 18   | SPI Clock                  |
| MOSI            | GPIO 23   | SPI Master Out Slave In    |
| MISO            | GPIO 19   | SPI Master In Slave Out    |
| RST             | GPIO 21   | Reset module               |
| GND             | GND       | Ground                    |
| 3.3V            | 3.3V      | Power supply               |

## Installation & Setup 🚀
1. Clone this repository:
2. Open the project folder in VSCode or Arduino IDE.
3. Install required libraries (MFRC522, WiFi, MongoDB connector, etc.).
4. Configure Wi-Fi credentials and database connection details in the firmware.
5. Upload the code to the ESP32 board.
6. Set up the backend databases (MongoDB & MySQL) as per instructions in `/backend` folder.
7. Launch the mobile app and/or web interface for user access.

## Usage 📝
- Tap RFID cards on the MFRC522 reader to log attendance or access.
- Authenticate users via mobile app or web interface.
- Monitor attendance records and access logs stored securely in databases.

## Contributing 🤝
Contributions are welcome! Please open issues or submit pull requests.

## License 📄
This project is licensed under the MIT License.

---

Made with ❤️ by Karan Mondal

---

Feel free to ask if you want me to generate specific sections such as mobile app usage, backend setup, or detailed coding examples.Here is a detailed and visually engaging README you can use for your GitHub project:

