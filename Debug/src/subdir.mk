################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Arc.cpp \
../src/HLPN.cpp \
../src/HLPN_test.cpp \
../src/PetriNet.cpp \
../src/PetriNet_test.cpp \
../src/Place.cpp \
../src/StreamBundle.cpp \
../src/Symbol.cpp \
../src/SymbolStream.cpp \
../src/SymbolStream_test.cpp \
../src/Token.cpp \
../src/Transition.cpp \
../src/tester.cpp 

OBJS += \
./src/Arc.o \
./src/HLPN.o \
./src/HLPN_test.o \
./src/PetriNet.o \
./src/PetriNet_test.o \
./src/Place.o \
./src/StreamBundle.o \
./src/Symbol.o \
./src/SymbolStream.o \
./src/SymbolStream_test.o \
./src/Token.o \
./src/Transition.o \
./src/tester.o 

CPP_DEPS += \
./src/Arc.d \
./src/HLPN.d \
./src/HLPN_test.d \
./src/PetriNet.d \
./src/PetriNet_test.d \
./src/Place.d \
./src/StreamBundle.d \
./src/Symbol.d \
./src/SymbolStream.d \
./src/SymbolStream_test.d \
./src/Token.d \
./src/Transition.d \
./src/tester.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I"/home/ir-lab/Thabet/PetriNet_builder/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


