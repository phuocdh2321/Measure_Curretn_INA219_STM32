################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../INA219/ina219.cpp 

OBJS += \
./INA219/ina219.o 

CPP_DEPS += \
./INA219/ina219.d 


# Each subdirectory must supply rules for building sources it contributes
INA219/%.o INA219/%.su: ../INA219/%.cpp INA219/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/STM32_Project/Measure_Current/INA219" -I"D:/STM32_Project/Measure_Current/Terminal" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=standard_c_nano_cpp.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-INA219

clean-INA219:
	-$(RM) ./INA219/ina219.d ./INA219/ina219.o ./INA219/ina219.su

.PHONY: clean-INA219

