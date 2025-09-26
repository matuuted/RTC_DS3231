################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Devices/API/Src/ds3231.c \
../Devices/API/Src/ds3231_port.c 

OBJS += \
./Devices/API/Src/ds3231.o \
./Devices/API/Src/ds3231_port.o 

C_DEPS += \
./Devices/API/Src/ds3231.d \
./Devices/API/Src/ds3231_port.d 


# Each subdirectory must supply rules for building sources it contributes
Devices/API/Src/%.o Devices/API/Src/%.su Devices/API/Src/%.cyclo: ../Devices/API/Src/%.c Devices/API/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/API/Inc -I../Devices/API/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Devices-2f-API-2f-Src

clean-Devices-2f-API-2f-Src:
	-$(RM) ./Devices/API/Src/ds3231.cyclo ./Devices/API/Src/ds3231.d ./Devices/API/Src/ds3231.o ./Devices/API/Src/ds3231.su ./Devices/API/Src/ds3231_port.cyclo ./Devices/API/Src/ds3231_port.d ./Devices/API/Src/ds3231_port.o ./Devices/API/Src/ds3231_port.su

.PHONY: clean-Devices-2f-API-2f-Src

