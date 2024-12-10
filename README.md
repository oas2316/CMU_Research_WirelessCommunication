# CMU_Research_WirelessCommunication
This project integrates with my ongoing research at Carnegie Mellon University, where I am exploring the use of LoRa (Long Range) wireless communication for search-and-rescue missions. The system involves heterogeneous robot collaboration, utilizing centimeter-scale spherical robots that interface with an inflatable vine robot developed by Purdue University. These robots will work collectively to navigate and gather data in challenging environments. Each spherical robot will carry specific sensors, and data will be transmitted wirelessly to a handheld receiver held by a rescue operator.
The communication system leverages LoRa technology, which operates at 900 MHz and uses chirp spread spectrum (CSS) modulation. This technique ensures robust, interference-resistant communication over ranges of up to 10-15 kilometers (6-9 miles), making it well-suited for long-range, low-power, and low-data-rate applications.
The design includes the development of three custom PCBs:
1.	Receiver PCB: A handheld, wireless device equipped with an ATMEGA328 microcontroller, a LoRa module, and an OLED display to show live sensor data from the robots.
2.	Transmitter 1 PCB: Mounted on a spherical robot, featuring an ATMEGA328, a LoRa module, and sensors for air quality (CCS811) and metal oxide gas detection (ENS160).
3.	Transmitter 2 PCB: Also mounted on a spherical robot, incorporating an ATMEGA328, a LoRa module, a flammable gas sensor (MQS), and an I2S microphone.
   
The PCBs were designed to be compact (1-2 inches in all dimensions) to fit the scale of the robots. I utilized Altium Designer to create the schematics and layouts and sourced most of the required components from my research lab at CMU.
