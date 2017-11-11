################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/connectionsManager.c \
../src/finalStorage.c \
../src/globalReduction.c \
../src/localReduction.c \
../src/masterClient.c \
../src/metrics.c \
../src/transform.c \
../src/utils.c 

OBJS += \
./src/connectionsManager.o \
./src/finalStorage.o \
./src/globalReduction.o \
./src/localReduction.o \
./src/masterClient.o \
./src/metrics.o \
./src/transform.o \
./src/utils.o 

C_DEPS += \
./src/connectionsManager.d \
./src/finalStorage.d \
./src/globalReduction.d \
./src/localReduction.d \
./src/masterClient.d \
./src/metrics.d \
./src/transform.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2017-2c-KILL9/tplibraries" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


