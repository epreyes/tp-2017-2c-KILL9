################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/connectionsManager.c \
../src/utils.c \
../src/workerClient.c 

OBJS += \
./src/connectionsManager.o \
./src/utils.o \
./src/workerClient.o 

C_DEPS += \
./src/connectionsManager.d \
./src/utils.d \
./src/workerClient.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


