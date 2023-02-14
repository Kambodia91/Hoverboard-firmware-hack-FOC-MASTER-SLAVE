# Hoverboard firmware hack FOC MASTER/SLAVE (HOVER-1 RIVAL)
![HOVER-1](/docs/pictures/HOVERBOARD-HOVER-RIVAL-LED.jpg)

This repository implements Field Oriented Control (FOC) for hoverboards witch two boards Master and Slave. The boards communicate with each other via UART2, The motors are controlled via UART1 on the master board and Arduino. The left bldc motor was cut from the original program. 

The software is being tested.! More info: https://github.com/EFeru/hoverboard-firmware-hack-FOC/issues/364


Compared to the commutation method, this new FOC control method offers superior performance featuring:
 - reduced noise and vibrations 	
 - smooth torque output and improved motor efficiency. Thus, lower energy consumption
 - field weakening to increase maximum speed range

## Hardware
The originals are MM32 processors. We need to replace them with STM32. The program is used to support the [STM32F103CBT6](https://github.com/Kambodia91/Hoverboard-firmware-hack-FOC-MASTER-SLAVE/blob/main/docs/literature/STM32F103xB.pdf) processor. 
More information can be found in the [docs](/docs/).

Board pinouts: Slave / Master
![mainboard_pinout](/docs/Board%20MASTER-SLAVE.png)
 
---
## FOC Firmware
 
In this firmware 3 control types are available, it can be set in config.h file via CTRL_TYP_SEL parameter:
- Commutation (COM_CTRL)
- Sinusoidal (SIN_CTRL)
- Field Oriented Control (FOC_CTRL) with the following 3 control modes that can be set in config.h file with parameter CTRL_MOD_REQ:
  - **VOLTAGE MODE(VLT_MODE)**: in this mode the controller applies a constant Voltage to the motors. Recommended for robotics applications or applications where a fast motor response is required.
  - **SPEED MODE(SPD_MODE)**: in this mode a closed-loop controller realizes the input speed RPM target by rejecting any of the disturbance (resistive load) applied to the motor. Recommended for robotics applications or constant speed applications.
  - **TORQUE MODE(TRQ_MODE)**: in this mode the input torque target is realized. This mode enables motor "freewheeling" when the torque target is `0`. Recommended for most applications with a sitting human driver.

### FOC Webview

To explore the controller without a Matlab/Simulink installation click on the link below:

[https://eferu.github.io/bldc-motor-control-FOC/](https://eferu.github.io/bldc-motor-control-FOC/)

---
## Example Variants

- **VARIANT_USART**: The motors are controlled via serial protocol (e.g. on USART1). The commands can be sent from an Arduino. Check out the [hoverserial](/Arduino/hoverserial.cpp) as an example sketch.

Of course the firmware can be further customized for other needs or projects.

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
