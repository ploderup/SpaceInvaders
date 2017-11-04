################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/alien.c \
../src/bullet.c \
../src/bunker.c \
../src/functions.c \
../src/globals.c \
../src/main.c \
../src/platform.c \
../src/render.c \
../src/saucer.c \
../src/sound.c \
../src/tank.c \
../src/xac97_l.c 

LD_SRCS += \
../src/lscript.ld 

OBJS += \
./src/alien.o \
./src/bullet.o \
./src/bunker.o \
./src/functions.o \
./src/globals.o \
./src/main.o \
./src/platform.o \
./src/render.o \
./src/saucer.o \
./src/sound.o \
./src/tank.o \
./src/xac97_l.o 

C_DEPS += \
./src/alien.d \
./src/bullet.d \
./src/bunker.d \
./src/functions.d \
./src/globals.d \
./src/main.d \
./src/platform.d \
./src/render.d \
./src/saucer.d \
./src/sound.d \
./src/tank.d \
./src/xac97_l.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo Building file: $<
	@echo Invoking: MicroBlaze gcc compiler
	mb-gcc -Wall -O0 -g3 -c -fmessage-length=0 -I../../space_invaders_bsp_0/microblaze_0/include -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mcpu=v8.20.b -mno-xl-soft-mul -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<
	@echo ' '


