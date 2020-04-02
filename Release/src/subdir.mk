################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Attribute.cpp \
../src/DivisonQuery.cpp \
../src/HierarchyParser.cpp \
../src/SetManager.cpp \
../src/app.cpp 

OBJS += \
./src/Attribute.o \
./src/DivisonQuery.o \
./src/HierarchyParser.o \
./src/SetManager.o \
./src/app.o 

CPP_DEPS += \
./src/Attribute.d \
./src/DivisonQuery.d \
./src/HierarchyParser.d \
./src/SetManager.d \
./src/app.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


