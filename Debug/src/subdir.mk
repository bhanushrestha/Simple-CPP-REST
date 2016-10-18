################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Configuration.cpp \
../src/Consumer.cpp \
../src/Producer.cpp \
../src/SimpleWebServer.cpp \
../src/Utils.cpp 

OBJS += \
./src/Configuration.o \
./src/Consumer.o \
./src/Producer.o \
./src/SimpleWebServer.o \
./src/Utils.o 

CPP_DEPS += \
./src/Configuration.d \
./src/Consumer.d \
./src/Producer.d \
./src/SimpleWebServer.d \
./src/Utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


