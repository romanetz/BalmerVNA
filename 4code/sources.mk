################################################################################
# Automatically-generated file. Do not edit!
################################################################################


DEPS += \
	output/CMSIS/startup_stm32f4xx.d \
	output/base/src/ad9958_drv.d \
	output/base/src/cs4272.d \
	output/base/src/dac.d \
	output/base/src/data_process.d \
	output/base/src/delay.d \
	output/base/src/main.d \
	output/base/src/process_sound.d \
	output/base/src/stm32f4xx_it.d \
	output/base/src/system_stm32f4xx.d \
	output/base/src/usb_bsp.d \
	output/base/src/usbd_cdc_vcp.d \
	output/base/src/usbd_desc.d \
	output/base/src/usbd_usr.d \
	output/base/src/ili/DefaultFonts.d \
	output/base/src/ili/UTFT.d \
	output/base/src/ili/hw_ili9341.d \
	output/base/lib/USB_OTG/src/usb_core.d \
	output/base/lib/USB_OTG/src/usb_dcd.d \
	output/base/lib/USB_OTG/src/usb_dcd_int.d \
	output/base/lib/USB_OTG/src/usb_hcd.d \
	output/base/lib/USB_OTG/src/usb_hcd_int.d \
	output/base/lib/USB_OTG/src/usb_otg.d \
	output/base/lib/USB_Device/Core/src/usbd_core.d \
	output/base/lib/USB_Device/Core/src/usbd_ioreq.d \
	output/base/lib/USB_Device/Core/src/usbd_req.d \
	output/base/lib/USB_Device/Class/cdc/src/usbd_cdc_core.d \
	output/StdPeriph_Driver/misc.d \
	output/StdPeriph_Driver/stm32f4xx_dac.d \
	output/StdPeriph_Driver/stm32f4xx_dma.d \
	output/StdPeriph_Driver/stm32f4xx_exti.d \
	output/StdPeriph_Driver/stm32f4xx_gpio.d \
	output/StdPeriph_Driver/stm32f4xx_i2c.d \
	output/StdPeriph_Driver/stm32f4xx_rcc.d \
	output/StdPeriph_Driver/stm32f4xx_spi.d \
	output/StdPeriph_Driver/stm32f4xx_syscfg.d \
	output/StdPeriph_Driver/stm32f4xx_tim.d \
	output/FFT/CommonTables/arm_common_tables.d \
	output/FFT/ComplexMathFunctions/arm_cmplx_mag_f32.d \
	output/FFT/TransformFunctions/arm_cfft_radix4_f32.d \
	output/FFT/TransformFunctions/arm_cfft_radix4_init_f32.d \
	output/FFT/TransformFunctions/arm_rfft_init_f32.d \
	output/FFT/TransformFunctions/arm_rfft_f32.d \


OBJS += \
	output/CMSIS/startup_stm32f4xx.o \
	output/base/src/ad9958_drv.o \
	output/base/src/cs4272.o \
	output/base/src/dac.o \
	output/base/src/data_process.o \
	output/base/src/delay.o \
	output/base/src/main.o \
	output/base/src/process_sound.o \
	output/base/src/stm32f4xx_it.o \
	output/base/src/system_stm32f4xx.o \
	output/base/src/usb_bsp.o \
	output/base/src/usbd_cdc_vcp.o \
	output/base/src/usbd_desc.o \
	output/base/src/usbd_usr.o \
	output/base/src/ili/DefaultFonts.o \
	output/base/src/ili/UTFT.o \
	output/base/src/ili/hw_ili9341.o \
	output/base/lib/USB_OTG/src/usb_core.o \
	output/base/lib/USB_OTG/src/usb_dcd.o \
	output/base/lib/USB_OTG/src/usb_dcd_int.o \
	output/base/lib/USB_OTG/src/usb_hcd.o \
	output/base/lib/USB_OTG/src/usb_hcd_int.o \
	output/base/lib/USB_OTG/src/usb_otg.o \
	output/base/lib/USB_Device/Core/src/usbd_core.o \
	output/base/lib/USB_Device/Core/src/usbd_ioreq.o \
	output/base/lib/USB_Device/Core/src/usbd_req.o \
	output/base/lib/USB_Device/Class/cdc/src/usbd_cdc_core.o \
	output/StdPeriph_Driver/misc.o \
	output/StdPeriph_Driver/stm32f4xx_dac.o \
	output/StdPeriph_Driver/stm32f4xx_dma.o \
	output/StdPeriph_Driver/stm32f4xx_exti.o \
	output/StdPeriph_Driver/stm32f4xx_gpio.o \
	output/StdPeriph_Driver/stm32f4xx_i2c.o \
	output/StdPeriph_Driver/stm32f4xx_rcc.o \
	output/StdPeriph_Driver/stm32f4xx_spi.o \
	output/StdPeriph_Driver/stm32f4xx_syscfg.o \
	output/StdPeriph_Driver/stm32f4xx_tim.o \
	output/FFT/CommonTables/arm_common_tables.o \
	output/FFT/ComplexMathFunctions/arm_cmplx_mag_f32.o \
	output/FFT/TransformFunctions/arm_cfft_radix4_f32.o \
	output/FFT/TransformFunctions/arm_cfft_radix4_init_f32.o \
	output/FFT/TransformFunctions/arm_rfft_init_f32.o \
	output/FFT/TransformFunctions/arm_rfft_f32.o \


output/CMSIS/startup_stm32f4xx.o: ../../SoundCard/code/Libraries/CMSIS/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f4xx.S
	@echo 'Building target: startup_stm32f4xx.S'
	@$(CC) $(ASM_FLAGS) -o "$@" "$<"

output/base/src/ad9958_drv.o: ./src/ad9958_drv.c
	@echo 'Building target: ad9958_drv.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/cs4272.o: ./src/cs4272.c
	@echo 'Building target: cs4272.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/dac.o: ./src/dac.c
	@echo 'Building target: dac.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/data_process.o: ./src/data_process.c
	@echo 'Building target: data_process.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/delay.o: ./src/delay.c
	@echo 'Building target: delay.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/main.o: ./src/main.c
	@echo 'Building target: main.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/process_sound.o: ./src/process_sound.c
	@echo 'Building target: process_sound.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/stm32f4xx_it.o: ./src/stm32f4xx_it.c
	@echo 'Building target: stm32f4xx_it.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/system_stm32f4xx.o: ./src/system_stm32f4xx.c
	@echo 'Building target: system_stm32f4xx.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/usb_bsp.o: ./src/usb_bsp.c
	@echo 'Building target: usb_bsp.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/usbd_cdc_vcp.o: ./src/usbd_cdc_vcp.c
	@echo 'Building target: usbd_cdc_vcp.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/usbd_desc.o: ./src/usbd_desc.c
	@echo 'Building target: usbd_desc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/usbd_usr.o: ./src/usbd_usr.c
	@echo 'Building target: usbd_usr.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/ili/DefaultFonts.o: ./src/ili/DefaultFonts.c
	@echo 'Building target: DefaultFonts.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/ili/UTFT.o: ./src/ili/UTFT.c
	@echo 'Building target: UTFT.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/ili/hw_ili9341.o: ./src/ili/hw_ili9341.c
	@echo 'Building target: hw_ili9341.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/lib/USB_OTG/src/usb_core.o: ./lib/USB_OTG/src/usb_core.c
	@echo 'Building target: usb_core.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/lib/USB_OTG/src/usb_dcd.o: ./lib/USB_OTG/src/usb_dcd.c
	@echo 'Building target: usb_dcd.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/lib/USB_OTG/src/usb_dcd_int.o: ./lib/USB_OTG/src/usb_dcd_int.c
	@echo 'Building target: usb_dcd_int.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/lib/USB_OTG/src/usb_hcd.o: ./lib/USB_OTG/src/usb_hcd.c
	@echo 'Building target: usb_hcd.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/lib/USB_OTG/src/usb_hcd_int.o: ./lib/USB_OTG/src/usb_hcd_int.c
	@echo 'Building target: usb_hcd_int.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/lib/USB_OTG/src/usb_otg.o: ./lib/USB_OTG/src/usb_otg.c
	@echo 'Building target: usb_otg.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/lib/USB_Device/Core/src/usbd_core.o: ./lib/USB_Device/Core/src/usbd_core.c
	@echo 'Building target: usbd_core.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/lib/USB_Device/Core/src/usbd_ioreq.o: ./lib/USB_Device/Core/src/usbd_ioreq.c
	@echo 'Building target: usbd_ioreq.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/lib/USB_Device/Core/src/usbd_req.o: ./lib/USB_Device/Core/src/usbd_req.c
	@echo 'Building target: usbd_req.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/lib/USB_Device/Class/cdc/src/usbd_cdc_core.o: ./lib/USB_Device/Class/cdc/src/usbd_cdc_core.c
	@echo 'Building target: usbd_cdc_core.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/misc.o: ../../SoundCard/code/Libraries/STM32F4xx_StdPeriph_Driver/src/misc.c
	@echo 'Building target: misc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_dac.o: ../../SoundCard/code/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c
	@echo 'Building target: stm32f4xx_dac.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_dma.o: ../../SoundCard/code/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c
	@echo 'Building target: stm32f4xx_dma.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_exti.o: ../../SoundCard/code/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c
	@echo 'Building target: stm32f4xx_exti.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_gpio.o: ../../SoundCard/code/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
	@echo 'Building target: stm32f4xx_gpio.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_i2c.o: ../../SoundCard/code/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c
	@echo 'Building target: stm32f4xx_i2c.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_rcc.o: ../../SoundCard/code/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
	@echo 'Building target: stm32f4xx_rcc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_spi.o: ../../SoundCard/code/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c
	@echo 'Building target: stm32f4xx_spi.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_syscfg.o: ../../SoundCard/code/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c
	@echo 'Building target: stm32f4xx_syscfg.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_tim.o: ../../SoundCard/code/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c
	@echo 'Building target: stm32f4xx_tim.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/FFT/CommonTables/arm_common_tables.o: ../../SoundCard/code/Libraries/CMSIS/DSP_Lib/Source/CommonTables/arm_common_tables.c
	@echo 'Building target: arm_common_tables.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/FFT/ComplexMathFunctions/arm_cmplx_mag_f32.o: ../../SoundCard/code/Libraries/CMSIS/DSP_Lib/Source/ComplexMathFunctions/arm_cmplx_mag_f32.c
	@echo 'Building target: arm_cmplx_mag_f32.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/FFT/TransformFunctions/arm_cfft_radix4_f32.o: ../../SoundCard/code/Libraries/CMSIS/DSP_Lib/Source/TransformFunctions/arm_cfft_radix4_f32.c
	@echo 'Building target: arm_cfft_radix4_f32.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/FFT/TransformFunctions/arm_cfft_radix4_init_f32.o: ../../SoundCard/code/Libraries/CMSIS/DSP_Lib/Source/TransformFunctions/arm_cfft_radix4_init_f32.c
	@echo 'Building target: arm_cfft_radix4_init_f32.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/FFT/TransformFunctions/arm_rfft_init_f32.o: ../../SoundCard/code/Libraries/CMSIS/DSP_Lib/Source/TransformFunctions/arm_rfft_init_f32.c
	@echo 'Building target: arm_rfft_init_f32.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/FFT/TransformFunctions/arm_rfft_f32.o: ../../SoundCard/code/Libraries/CMSIS/DSP_Lib/Source/TransformFunctions/arm_rfft_f32.c
	@echo 'Building target: arm_rfft_f32.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"



