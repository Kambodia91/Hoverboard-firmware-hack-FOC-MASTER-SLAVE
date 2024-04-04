# Hoverboard firmware hack FOC MASTER/SLAVE (HOVER-1 RIVAL)
![HOVER-1](/docs/pictures/HOVERBOARD-HOVER-RIVAL-LED.jpg)

This repository implements Field Oriented Control (FOC) for hoverboards witch two boards Master and Slave. The boards communicate with each other via UART2, The motors are controlled via UART1 on the master board and Arduino. [Communication diagram](/docs/CommunicationDiagram) The left bldc motor was cut from the original program. 

The software is being tested.! More info: https://github.com/EFeru/hoverboard-firmware-hack-FOC/issues/364

---
YOUTUBE:
- [Test communication usart1 with esp32, blynkAPP](https://youtu.be/ViJhMbIGTzU).
- [Test safety communication](https://youtu.be/5jPmf3Z_Q6c).
- [Driving test 2 rear wheels](https://youtu.be/ALolYnv8E9s).
- [Driving test 01 4x4 new wheels](https://youtu.be/f5kqq9XtkOA?si=Ek2Kk6Emhx_Iep77).
- [Driving test 02 4x4 new wheels](https://youtu.be/5k37qx_ut7Y?si=PjrUhJfBA4WDzB2Y).
- [Drobiny test 03 with engine cutter](https://youtu.be/J5ug9tpqyGo?si=b4b2j3KFuMFDtWn7).
- [Cuttint height adjustment](https://youtu.be/1qUQQXA1uOE?si=6t5uq69G1FAZiTjl).
- [First cut grass](https://youtu.be/wi3ILRYedm8).

---

Compared to the commutation method, this new FOC control method offers superior performance featuring:
 - reduced noise and vibrations 	
 - smooth torque output and improved motor efficiency. Thus, lower energy consumption
 - field weakening to increase maximum speed range

---
## Hardware
The originals are MM32 processors. We need to replace them with STM32. The program is used to support the [STM32F103CBT6](https://github.com/Kambodia91/Hoverboard-firmware-hack-FOC-MASTER-SLAVE/blob/main/docs/literature/STM32F103xB.pdf) processor. 
More information can be found in the [docs](/docs/).

Board pinouts: Slave / Master
![mainboard_pinout](/docs/Board%20MASTER-SLAVE.png)

---
## My Project
I am trying to create a 4-wheeled vehicle. Initial sketch idea: ![vehicle](docs/pictures/Lawn%20Project/IMG20231008132136.jpg) (rigid without turning the wheels) using 2 howerboards for the wheels, communication will be made using uart and esp32. Currently, I was thinking about the actual position in space of the vehicle and came to the conclusion that the best solution would be to use the DW1000. In the future, there may be a lidar sensor. A lawn mower will be installed on the wheels to mow the lawn automatically. I invite you to the docs tab there are more [photos](/docs/pictures/) from the construction.

-- --

To do: 
- [x] Hack howerboard motherboards.  
- [x] Convert the LED Out socket to a USART1 socket for control.  
- [x] Change tire size.  
- [x] Extend the wheel current cables and the hall sensor signal.  
- [x] Insert the wheel controller into the housing.  
- [x] Make connections for programming and debugging without having to remove the boards.  
- [x] Vehicle frame structure.
- [x] Cutting height.
- [x] Installation of the combustion engine.
- [x] Making a hub to connect the alternator and the cutting knife.
- [x] Connecting the engine shaft with the alternator pulley with a multi-grooved belt.  
- [ ] Converting a 12v alternator to 42v.  
- [x] Installing the alternator to the vehicle frame.
- [x] Selecting a remote control for manual control.
- [ ] Installation of a PIXHAWK controller for remote planning and autonomous mission
- [ ] Combine the GPS system with RTK correction to obtain high spatial orientation accuracy
- [ ] installation of sensors to avoid obstacles
      

---
## FOC Firmware
 
In this firmware 3 control types are available, it can be set in config.h file via CTRL_TYP_SEL parameter:
- Commutation (COM_CTRL)
- Sinusoidal (SIN_CTRL)
- Field Oriented Control (FOC_CTRL) with the following 3 control modes that can be set in config.h file with parameter CTRL_MOD_REQ:
  - **VOLTAGE MODE(VLT_MODE)**: in this mode the controller applies a constant Voltage to the motors. Recommended for robotics applications or applications where a fast motor response is required.
  - **SPEED MODE(SPD_MODE)**: in this mode a closed-loop controller realizes the input speed RPM target by rejecting any of the disturbance (resistive load) applied to the motor. Recommended for robotics applications or constant speed applications.
  - **TORQUE MODE(TRQ_MODE)**: in this mode the input torque target is realized. This mode enables motor "freewheeling" when the torque target is `0`. Recommended for most applications with a sitting human driver.

---
### FOC Webview

To explore the controller without a Matlab/Simulink installation click on the link below:

[https://eferu.github.io/bldc-motor-control-FOC/](https://eferu.github.io/bldc-motor-control-FOC/)

---
## Example Variants

- **VARIANT_ADC**: The motors are controlled by one potentiometer connected to the white cable (connector hall). EnableMotors will turn on automatically.

- **VARIANT_USART**: The motors are controlled via serial protocol (e.g. on USART1). The commands can be sent from an Arduino. Check out the [hoverserial](/Arduino/hoverserial.cpp) as an example sketch.

- **VARIANT_PPM**: RC remote control with PPM Sum signal. Reciver connect to pin RX USATR1. Ch1: MasterSpeed, Ch2: SlaveSpeed, Ch3: EnableMotors, Ch4: -, Ch5: Button1, Ch6: Button2. When Button_Support is disabled, EnnableMotors will be enabled automatically.
  
- **VARIANT_PWM:** RC remote control with PWM signal. PWM signal 1 (MasterSpeed) connect to TX USART1, PWM signal 2 (SlaveSpeed) connect to RX USART1. EnableMotors will turn on automatically.

Of course the firmware can be further customized for other needs or projects.

---
## Status
![HOVER-1](/docs/Status.png)


---
## Projects and Links

- **Original firmware:** [https://github.com/lucysrausch/hoverboard-firmware-hack](https://github.com/lucysrausch/hoverboard-firmware-hack)
- **[Candas](https://github.com/Candas1/) Hoverboard Web Serial Control:** [https://github.com/Candas1/Hoverboard-Web-Serial-Control](https://github.com/Candas1/Hoverboard-Web-Serial-Control)
- **[RoboDurden's](https://github.com/RoboDurden) online compiler:** [https://pionierland.de/hoverhack/](https://pionierland.de/hoverhack/) 
- **Hoverboard hack for AT32F403RCT6 mainboards:** [https://github.com/cloidnerux/hoverboard-firmware-hack](https://github.com/cloidnerux/hoverboard-firmware-hack)
- **Hoverboard hack for split mainboards:** [https://github.com/flo199213/Hoverboard-Firmware-Hack-Gen2](https://github.com/flo199213/Hoverboard-Firmware-Hack-Gen2)
- **Hoverboard hack from BiPropellant:** [https://github.com/bipropellant](https://github.com/bipropellant)
- **Hoverboard breakout boards:** [https://github.com/Jana-Marie/hoverboard-breakout](https://github.com/Jana-Marie/hoverboard-breakout)

<a/>

- **Bobbycar** [https://github.com/larsmm/hoverboard-firmware-hack-FOC-bbcar](https://github.com/larsmm/hoverboard-firmware-hack-FOC-bbcar)
- **Wheel chair:** [https://github.com/Lahorde/steer_speed_ctrl](https://github.com/Lahorde/steer_speed_ctrl)
- **TranspOtterNG:** [https://github.com/Jan--Henrik/transpOtterNG](https://github.com/Jan--Henrik/transpOtterNG)
- **Hoverboard driver for ROS:** [https://github.com/alex-makarov/hoverboard-driver](https://github.com/alex-makarov/hoverboard-driver)
- **Ongoing OneWheel project:** [https://forum.esk8.news/t/yet-another-hoverboard-to-onewheel-project/60979/14](https://forum.esk8.news/t/yet-another-hoverboard-to-onewheel-project/60979/14)
- **ST Community:** [Custom FOC motor control](https://community.st.com/s/question/0D50X0000B28qTDSQY/custom-foc-control-current-measurement-dma-timer-interrupt-needs-review)

<a/>

- **Telegram Community:** If you are an enthusiast join our [Hooover Telegram Group](https://t.me/joinchat/BHWO_RKu2LT5ZxEkvUB8uw)

---
## Contributions

Every contribution to this repository is highly appreciated! Feel free to create pull requests to improve this firmware as ultimately you are going to help everyone. 
---
