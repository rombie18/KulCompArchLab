################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/interrupts.c \
../src/main.c \
../src/syscalls.c \
../src/sysmem.c 

C_DEPS += \
./src/interrupts.d \
./src/main.d \
./src/syscalls.d \
./src/sysmem.d 

OBJS += \
./src/interrupts.o \
./src/main.o \
./src/syscalls.o \
./src/sysmem.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	arm-none-eabi-gcc "$<" -std=gnu11 -g -c -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

