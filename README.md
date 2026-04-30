# The-Mini-Mars-Monster
A final Project for UVM CMPE3815 Microcontroller Systems: 


## Project Overview

This project develops an autonomous ground vehicle equipped with a LiDAR sensor for real-time obstacle detection and navigation. The system features dual-mode operation, autonomous obstacle avoidance and manual remote control, along with a mounted robotic arm for environmental interaction.

Inspired by Mars rovers' autonomy algorithms and the coolness of monster trucks, this vehicle platform combines sensing with practical manipulation capabilities for field exploration and autonomous data collection.

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
- **Motor Drivers**: 
- **Servo Motors**: Controllers for arm joints and steering servo
- **Microcontroller**: Arduino Uno 
- **Robotic Arm**: Laser cut robotic arm with servo-controlled joints and gripper
- **IR Receiver**  For manual remote control operation


## Modes of Operation

**Autonomous Mode**: The vehicle independently detects obstacles using LiDAR, and navigates to explore unknown terrain.

**RC Mode**: A remote operator controls the vehicle's steering, and robotic arm through an IR Receiver. 

Both modes can be switched dynamically, allowing the vehicle to operate autonomously during navigation phases and revert to manual control when manipulation or human judgment is needed.

## Applications

Designed for field exploration, and environmental data logging. 
