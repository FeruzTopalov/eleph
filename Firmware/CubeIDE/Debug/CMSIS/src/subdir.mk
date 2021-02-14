################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/src/core_cm3.c \
../CMSIS/src/system_stm32f10x.c 

OBJS += \
./CMSIS/src/core_cm3.o \
./CMSIS/src/system_stm32f10x.o 

C_DEPS += \
./CMSIS/src/core_cm3.d \
./CMSIS/src/system_stm32f10x.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/src/core_cm3.o: ../CMSIS/src/core_cm3.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"CMSIS/src/core_cm3.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
CMSIS/src/system_stm32f10x.o: ../CMSIS/src/system_stm32f10x.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/CMSIS/inc" -I"D:/Projects/ARM/!ELEPH/eleph/Firmware/CubeIDE/Code/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"CMSIS/src/system_stm32f10x.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

