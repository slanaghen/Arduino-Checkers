################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../arduino/arduino.c \
../arduino/rs232.c 

OBJS += \
./arduino/arduino.o \
./arduino/rs232.o 

C_DEPS += \
./arduino/arduino.d \
./arduino/rs232.d 


# Each subdirectory must supply rules for building sources it contributes
arduino/%.o: ../arduino/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


