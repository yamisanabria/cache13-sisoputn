################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fileParse/src/main.c 

OBJS += \
./fileParse/src/main.o 

C_DEPS += \
./fileParse/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
fileParse/src/%.o: ../fileParse/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


