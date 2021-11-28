################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Code/src/adc.c \
../Code/src/buttons.c \
../Code/src/gpio.c \
../Code/src/gps.c \
../Code/src/i2c.c \
../Code/src/lrns.c \
../Code/src/m24c64.c \
../Code/src/main.c \
../Code/src/menu.c \
../Code/src/points.c \
../Code/src/rfm98.c \
../Code/src/service.c \
../Code/src/settings.c \
../Code/src/spi.c \
../Code/src/ssd1306.c \
../Code/src/ssd1306_bitmaps.c \
../Code/src/timer.c \
../Code/src/uart.c 

OBJS += \
./Code/src/adc.o \
./Code/src/buttons.o \
./Code/src/gpio.o \
./Code/src/gps.o \
./Code/src/i2c.o \
./Code/src/lrns.o \
./Code/src/m24c64.o \
./Code/src/main.o \
./Code/src/menu.o \
./Code/src/points.o \
./Code/src/rfm98.o \
./Code/src/service.o \
./Code/src/settings.o \
./Code/src/spi.o \
./Code/src/ssd1306.o \
./Code/src/ssd1306_bitmaps.o \
./Code/src/timer.o \
./Code/src/uart.o 

C_DEPS += \
./Code/src/adc.d \
./Code/src/buttons.d \
./Code/src/gpio.d \
./Code/src/gps.d \
./Code/src/i2c.d \
./Code/src/lrns.d \
./Code/src/m24c64.d \
./Code/src/main.d \
./Code/src/menu.d \
./Code/src/points.d \
./Code/src/rfm98.d \
./Code/src/service.d \
./Code/src/settings.d \
./Code/src/spi.d \
./Code/src/ssd1306.d \
./Code/src/ssd1306_bitmaps.d \
./Code/src/timer.d \
./Code/src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Code/src/adc.o: ../Code/src/adc.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/adc.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/buttons.o: ../Code/src/buttons.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/buttons.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/gpio.o: ../Code/src/gpio.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/gpio.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/gps.o: ../Code/src/gps.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/gps.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/i2c.o: ../Code/src/i2c.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/i2c.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/lrns.o: ../Code/src/lrns.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/lrns.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/m24c64.o: ../Code/src/m24c64.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/m24c64.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/main.o: ../Code/src/main.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/main.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/menu.o: ../Code/src/menu.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/menu.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/points.o: ../Code/src/points.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/points.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/rfm98.o: ../Code/src/rfm98.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/rfm98.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/service.o: ../Code/src/service.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/service.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/settings.o: ../Code/src/settings.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/settings.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/spi.o: ../Code/src/spi.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/spi.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/ssd1306.o: ../Code/src/ssd1306.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/ssd1306.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/ssd1306_bitmaps.o: ../Code/src/ssd1306_bitmaps.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/ssd1306_bitmaps.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/timer.o: ../Code/src/timer.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/timer.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/uart.o: ../Code/src/uart.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/uart.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

