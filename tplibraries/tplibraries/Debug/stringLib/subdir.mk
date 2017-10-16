################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../stringLib/stringLib.c 

OBJS += \
./stringLib/stringLib.o 

C_DEPS += \
./stringLib/stringLib.d 


# Each subdirectory must supply rules for building sources it contributes
stringLib/%.o: ../stringLib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


