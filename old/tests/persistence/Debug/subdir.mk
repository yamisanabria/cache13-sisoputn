################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../commands.c \
../fileSystem.c \
../persistence.c 

OBJS += \
./commands.o \
./fileSystem.o \
./persistence.o 

C_DEPS += \
./commands.d \
./fileSystem.d \
./persistence.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -lcommons -lm -I"/home/utnso/workspace/tp-2015-1c-power-rangers/Includes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


