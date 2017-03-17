################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Checkers.c \
../CheckersEngine.c \
../CheckersEngineAuto.c \
../CheckersEngineInteractive.c \
../CheckersEngineTest.c \
../Core.c \
../Display.c \
../Main.c 

OBJS += \
./Checkers.o \
./CheckersEngine.o \
./CheckersEngineAuto.o \
./CheckersEngineInteractive.o \
./CheckersEngineTest.o \
./Core.o \
./Display.o \
./Main.o 

C_DEPS += \
./Checkers.d \
./CheckersEngine.d \
./CheckersEngineAuto.d \
./CheckersEngineInteractive.d \
./CheckersEngineTest.d \
./Core.d \
./Display.d \
./Main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


