# EcoSort: Smart Waste Management System

EcoSort is an IoT-based automated waste segregation system designed to revolutionize urban waste management by identifying and separating waste into Bio-degradable and Non-biodegradable categories at the point of disposal.

---

## 🚀 Features

* **Automatic Segregation**: Uses moisture and IR sensors to categorize waste in real-time without human intervention.
* **Automated Rotation**: Features a motorized mechanism to direct waste into the correct compartment automatically.
* **Efficiency**: Designed to reduce manual labor and improve the quality of recyclable materials.

---

## 🛠️ Hardware Components

The system utilizes the following core hardware:

* **Microcontroller**: Arduino UNO (Atmega328P).
* **Sensors**:
    * **IR Sensor**: Detects the presence of waste in the entry slot.
    * **Moisture Sensor**: Distinguishes between organic and inorganic waste based on moisture content.
    * **Inductive Proximity Sensor**: Senses the inductive properties of waste and classifies it as metal or non-metal.
* **Actuators**: 
    * **Servo Motor**: Controls the mechanical flap for sorting.
    * **DC Motor**: Powers the rotating bin mechanism.

---

## ⚙️ How It Works

1.  **Object Detection**: The IR sensor detects an object placed in the waste slot.
2.  **Sensing & Logic**: The moisture sensor analyzes the object; high moisture levels indicate bio-degradable waste, while low levels indicate non-biodegradable waste.
3.  **Mechanical Sorting**: The Arduino processes sensor data and signals the servo or DC motor to rotate the bin/flap to the appropriate compartment.

---

## 📂 Project Structure

```text
/EcoSort
├── /Firmware        # Arduino (.ino) code for sensor logic and motor control
├── /Hardware        # Circuit diagrams and pin connection maps
├── /Documentation   # Project reports and component datasheets
└── README.md        # Project documentation

