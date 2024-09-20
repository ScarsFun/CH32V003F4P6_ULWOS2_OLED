################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ULWOS2.c \
../User/ULWOS2_HAL.c \
../User/ch32v00x_it.c \
../User/fonts.c \
../User/main.c \
../User/oled_display.c \
../User/system_ch32v00x.c 

OBJS += \
./User/ULWOS2.o \
./User/ULWOS2_HAL.o \
./User/ch32v00x_it.o \
./User/fonts.o \
./User/main.o \
./User/oled_display.o \
./User/system_ch32v00x.o 

C_DEPS += \
./User/ULWOS2.d \
./User/ULWOS2_HAL.d \
./User/ch32v00x_it.d \
./User/fonts.d \
./User/main.d \
./User/oled_display.d \
./User/system_ch32v00x.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32ec -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -DULWOS2_SIGNALS -I"/home/max/mrs_community-workspace/CH32V003F4P6_ULWOS2/Debug" -I"/home/max/mrs_community-workspace/CH32V003F4P6_ULWOS2/User/inc" -I"/home/max/mrs_community-workspace/CH32V003F4P6_ULWOS2/Core" -I"/home/max/mrs_community-workspace/CH32V003F4P6_ULWOS2/User" -I"/home/max/mrs_community-workspace/CH32V003F4P6_ULWOS2/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


