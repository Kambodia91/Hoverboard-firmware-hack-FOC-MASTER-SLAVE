# hoverboard firmware hack FOC MASTER/SLAVE

This repository implements Field Oriented Control (FOC) for hoverboards witch two boards. Compared to the commutation method, this new FOC control method offers superior performance featuring:
 - reduced noise and vibrations 	
 - smooth torque output and improved motor efficiency. Thus, lower energy consumption
 - field weakening to increase maximum speed range

## Hardware
 
![mainboard_pinout](/docs/pictures/mainboard_pinout.png)

The original Hardware supports two 4-pin cables that originally were connected to the two sideboards. They break out GND, 12/15V and USART2&3 of the Hoverboard mainboard. Both USART2&3 support UART, PWM, PPM, and iBUS input. Additionally, the USART2 can be used as 12bit ADC, while USART3 can be used for I2C. Note that while USART3 (right sideboard cable) is 5V tolerant, USART2 (left sideboard cable) is **not** 5V tolerant.

Typically, the mainboard brain is an [STM32F103RCT6](/docs/literature/[10]_STM32F103xC_datasheet.pdf), however some mainboards feature a [GD32F103RCT6](/docs/literature/[11]_GD32F103xx-Datasheet-Rev-2.7.pdf) which is also supported by this firmware.

For the reverse-engineered schematics of the mainboard, see [20150722_hoverboard_sch.pdf](/docs/20150722_hoverboard_sch.pdf)

 
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


- **VARIANT_USART**: The motors are controlled via serial protocol (e.g. on USART3 right sensor cable, the short wired cable). The commands can be sent from an Arduino. Check out the [hoverserial.ino](/Arduino/hoverserial) as an example sketch.

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
## Stargazers

[![Stargazers over time](https://starchart.cc/EFeru/hoverboard-firmware-hack-FOC.svg)](https://starchart.cc/EFeru/hoverboard-firmware-hack-FOC)

---
## Contributions

Every contribution to this repository is highly appreciated! Feel free to create pull requests to improve this firmware as ultimately you are going to help everyone. 

If you want to donate to keep this firmware updated, please use the link below:

[![paypal](https://www.paypalobjects.com/en_US/NL/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=CU2SWN2XV9SCY&currency_code=EUR&source=url)

---
