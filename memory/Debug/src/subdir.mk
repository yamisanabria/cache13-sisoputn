################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/connections.c \
../src/main.c \
../src/memory.c 

OBJS += \
./src/connections.o \
./src/main.o \
./src/memory.o 

C_DEPS += \
./src/connections.d \
./src/main.d \
./src/memory.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -lcommons -pthread -I"/home/utnso/workspace/tp-2015-2c-Aprobados/includes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


