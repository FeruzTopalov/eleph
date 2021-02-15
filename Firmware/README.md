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