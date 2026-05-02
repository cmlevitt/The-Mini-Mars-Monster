# The-Mini-Mars-Monster
### A final Project for UVM CMPE3815 Microcontroller Systems by Callie Levitt


## Project Overview

This project develops an autonomous ground vehicle equipped with a LiDAR sensor for real-time obstacle detection and navigation. The system features dual-mode operation, autonomous obstacle avoidance and manual remote control, along with a mounted robotic arm for environmental interaction.

I was inspired by planetary rovers and monster trucks to make a vehicle platform that combines sensing with practical manipulation capabilities for field exploration and autonomous data collection.

## Core Features

**Autonomous Navigation**: The vehicle uses LiDAR to scan its surroundings and autonomously navigate through obstacle fields without collision. A state machine processes distance measurements to adjust the car's heading.

**Dual-Mode Operation**: 
- **Autonomous Mode**: Self-navigating obstacle avoidance using LiDAR
- **RC Mode**: Full remote control operation with real-time feedback, allowing manual steering and robotic arm control

**Robotic Arm Integration**: A robotic arm mounted to the vehicle enables manipulation tasks. 

## System Architecture

The vehicle integrates LiDAR sensing, motor control, sensor data logging, and decision-making into a cohesive autonomous platform. A microcontroller (Arduino Uno) handles real-time motor and servo control, while another holds environmental sensors and logs data.

## Hardware Components

- **LiDAR Sensor**: TOF scanning sensor for obstacle detection 
- **Drive Motors**: DC motors for forward/backward motion and steering
- **Motor Drivers**: From LAFVIN 2WD car kit
- **Servo Motors**: Controllers for arm joints and steering servo
- **Microcontroller**: Arduino Uno x2
- **Robotic Arm**: Laser cut robotic arm with servo-controlled joints and gripper - all design credit to https://www.instructables.com/MeArm-Build-a-Small-Hackable-Robot-Arm/
- **IR Receiver**:  For manual remote control operation
- **Lux Sensor**:
- **SD Card Reader**:
- **DHT11 Temperature / Humidity Sensor**:
- **Custom Wheels**: 3D printed wheels designed for traversing difficult terrain


## Modes of Operation

**Autonomous Mode**: The vehicle independently detects obstacles using LiDAR, and navigates to explore unknown terrain while collecting environmental data.

**RC Mode**: A remote operator controls the vehicle's steering, and robotic arm through an IR Receiver. 

Both modes can be switched dynamically, allowing the vehicle to operate autonomously during navigation phases and revert to manual control when manipulation or human judgment is needed.

## Applications

Designed for field exploration, and environmental data logging. 
