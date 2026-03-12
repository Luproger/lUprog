################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/encoder/encBtn.c 

OBJS += \
./lib/encoder/encBtn.o 

C_DEPS += \
./lib/encoder/encBtn.d 


# Each subdirectory must supply rules for building sources it contributes
lib/encoder/%.o: ../lib/encoder/%.c lib/encoder/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I"C:/Users/progr/Desktop/lUprog/lib/oled" -I"C:/Users/progr/Desktop/lUprog/lib/avr_programmer" -I"C:/Users/progr/Desktop/lUprog/lib/fsm" -I"C:/Users/progr/Desktop/lUprog/lib/encoder" -I../FATFS/App -I../USB_DEVICE/Target -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../USB_DEVICE/App -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../FATFS/Target -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/progr/Desktop/lUprog/lib/mcu" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

