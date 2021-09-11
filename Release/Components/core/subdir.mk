################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Components/core/engine.cpp \
../Components/core/system.cpp \
../Components/core/task.cpp 

OBJS += \
./Components/core/engine.o \
./Components/core/system.o \
./Components/core/task.o 

CPP_DEPS += \
./Components/core/engine.d \
./Components/core/system.d \
./Components/core/task.d 


# Each subdirectory must supply rules for building sources it contributes
Components/core/%.o: ../Components/core/%.cpp Components/core/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -DUSE_HAL_DRIVER -DSTM32F411xE -DUSE_FULL_LL_DRIVER -c -I"D:/CubeIDE/Education/mstack-education/b5-ESP32-AT-TCP-CLI/Components" -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

