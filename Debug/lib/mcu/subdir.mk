################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/mcu/mcu.c 

OBJS += \
./lib/mcu/mcu.o 

C_DEPS += \
./lib/mcu/mcu.d 


# Each subdirectory must supply rules for building sources it contributes
lib/mcu/%.o: ../lib/mcu/%.c lib/mcu/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I"C:/Users/progr/Desktop/lUprog/lib/oled" -I"C:/Users/progr/Desktop/lUprog/lib/avr_programmer" -I"C:/Users/progr/Desktop/lUprog/lib/fsm" -I"C:/Users/progr/Desktop/lUprog/lib/encoder" -I../FATFS/App -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../FATFS/Target -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/progr/Desktop/lUprog/lib/mcu" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

