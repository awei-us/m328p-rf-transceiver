################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
FLASH_IMAGE += \
MB-m328p-Control.hex \

EEPROM_IMAGE += \
MB-m328p-Control.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: MB-m328p-Control.elf secondary-outputs

# Tool invocations
MB-m328p-Control.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,MB-m328p-Control.map -mmcu=atmega328p -o "MB-m328p-Control.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

MB-m328p-Control.hex: MB-m328p-Control.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex MB-m328p-Control.elf  "MB-m328p-Control.hex"
	@echo 'Finished building: $@'
	@echo ' '

MB-m328p-Control.eep: MB-m328p-Control.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex MB-m328p-Control.elf  "MB-m328p-Control.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: MB-m328p-Control.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega328p MB-m328p-Control.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) MB-m328p-Control.elf
	-@echo ' '

secondary-outputs: $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
