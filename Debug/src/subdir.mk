################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/DivisonQuery.cpp \
../src/HierarchyParser.cpp \
../src/app.cpp 

OBJS += \
./src/DivisonQuery.o \
./src/HierarchyParser.o \
./src/app.o 

CPP_DEPS += \
./src/DivisonQuery.d \
./src/HierarchyParser.d \
./src/app.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/alexis/PROJECTS/External/arboretumLast/include -I/home/alexis/PROJECTS/Eclipse/RelationalCondition/src -I/home/alexis/PROJECTS/Eclipse/RelationalCondition/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


