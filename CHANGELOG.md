# Eleph changelog
**Legend:** _"Hardware version/Software version"_  



---
#### v1/2.0 - April 10, 2021
- Lower Fclk down to 24 MHz
- Put MCU in a sleep when idle
- Add screen on/off function via PWR button
- Disable peripheral while unused
- Make buttons scan on timer interrupts, optimize routine
- Make ADC reading on interrupts
- Switch 1 second counter from SysTick to RTC with LSI
- Replace beep gating timer
- Update occurrences of the beep
- Fix I2C stop condition bug
- Update green led behavior
- Add include guards to the *.h files
- Add bit banding support
- A lot of minor improvements



---
#### v1/1.0 - February 14, 2021
- Initial release