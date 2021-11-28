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

Eleph uses [RFM98W](https://www.hoperf.com/modules/lora/RFM98.html) transceiver module which operates in LPD 433 band.

You are able to use any version of RFM module which operates in allowed band in your country. Original transceivers by Semtech could be supported too. 

To change base frequency you should make changes at least in "**rfm98_config.h**" file in "RFM_CONF_ARRAY" . Replacement of the module with another assumes changes in radio driver "**rfm98.c**".

Transceiver modules other than RFM98W are not tested.

## LCD images preparation

If you were curious enough, you may found that Eleph's "devices" and "radar" screens use predefined bitmap templates that appear on LCD before printing other strings. You could find them in _"/Supplementary/ScreenTemplates/"_ directory.

In case if you want to change those templates, you cold use:
* FastLCD (by Bojan I. - MICRODESIGN, v1.2.0) in order to change it (currently available here http://avrproject.ru/nokia3310/FastLCD.exe). Also any graphical editor could be used.
* Image Generate (by Alex_EXE, v2.104) in order to convert bitmap into HEX array (available here https://alex-exe.ru/programm/image-generate/).
* Also i recommend to use DotMatrixTool (http://dotmatrixtool.com/) to generate separate symbols.