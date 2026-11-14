# ESP32-S3 Smart Safe Lock System 🔐

An embedded systems project built using an ESP32-S3 microcontroller, a 4x4 matrix keypad, a 16x2 LCD display, LEDs, and a buzzer.

## Project Overview

This project demonstrates a password-protected electronic safe system using hardware and software integration.

## Features

- Password entry using a 4x4 matrix keypad
- Password information displayed on a 16x2 LCD
- Randomly generated password length
- Green LED indication for successful access
- Red LED and buzzer alerts for incorrect passwords
- Maximum of three incorrect password attempts
- 10-second countdown after three failed attempts

## Hardware Components

- ESP32-S3 microcontroller
- 4x4 matrix keypad
- 16x2 LCD display
- Green LED
- Red LED
- Buzzer
- Breadboard and jumper wires

## Software and Technologies

- C++
- PlatformIO
- Visual Studio Code
- Arduino Framework

## How It Works

The user enters a password using the keypad. The ESP32-S3 processes the input and provides feedback through the LCD, LEDs, and buzzer.

Correct passwords trigger the green LED, while incorrect passwords activate the red LED and buzzer. After three failed attempts, the system starts a countdown.

## Future Improvements

- Servo motor for physical locking
- RFID authentication
- Wi-Fi monitoring
- Mobile application integration

## Project Status

Prototype developed and tested using ESP32-S3 and PlatformIO.
