# Eleph Firmware
This folder contains firmware of the ELEPH

## Source code
In the "**CubeIDE**" folder you can find project with the source code. As you can see it is created in CubeIDE and compiled using built-in **arm-none-eabi-gcc**.

The project is configured that way it should compile on your computer without problems. Follow the next steps:

* Open "**.project**" in [CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) (tested on Version: 1.5.0)
* Current configuration should be **Debug**, otherwise "_Project -> Build Configurations -> Set Active -> Debug_"
* Clean current configuration "_Project -> Clean... -> Clean_"
* Build "_Project -> Build Project_"
* Enshure that you got "**Build Finished. 0 errors, 0 warnings.**" console message
* Flash the firmware directly from CubeIDE "_Run -> Run_", or use "**Eleph.hex**" or "**Eleph.bin**" from "_/CubeIDE/Debug_" folder to flash any of them using classic way via [ST-LINK Utility](https://www.st.com/en/development-tools/stsw-link004.html)

## Different frequency programming

Eleph uses [SI4463](https://www.silabs.com/wireless/proprietary/ezradiopro-sub-ghz-ics/device.si4463) transceiver, therefore everything that is related to the SI4463 registers programming was pre-configured using WDS3 software from Silicon Labs. You could download it from Silabs web-site: http://www.silabs.com/Support%20Documents/Software/WDS3-Setup.exe 

Since you have installed the WDS3, you could open ***.xml** file provided in _"/Supplementary/WDS/"_ directory, make changes and then click _"Generate source"_ button and select _"Save custom radio configuration header file"_. This file should have "**radio_config_Si4463.h**" name. Being placed in _"/Code/inc/"_ directory, it will incorporate changes to your build after compilation.

Tested on WDS3 version 3.2.11.0

## LCD images preparation

If you was curious enough, you may found that Eleph's "devices" and "radar" screens use predefined bitmap templates that appear on LCD before printing other strings. You could find them in _"/Supplementary/ScreenTemplates/"_ directory.

In case if you want to change those templates, you cold use:
* FastLCD (by Bojan I. - MICRODESIGN, v1.2.0) in order to change it (currently available here http://avrproject.ru/nokia3310/FastLCD.exe). Also any graphical editor could be used.
* Image Generate (by Alex_EXE, v2.104) in order to convert bitmap into HEX array (available here https://alex-exe.ru/programm/image-generate/).
* Also i recommend to use DotMatrixTool (http://dotmatrixtool.com/) to generate separate symbols.