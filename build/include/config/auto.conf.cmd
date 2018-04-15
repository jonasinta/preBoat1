deps_config := \
	/home/jonas/workspace/esp32workspace/get-started/helloworld/components/SparkfunLCDDriver/Kconfig \
	/home/jonas/esp32/esp-idf/components/aws_iot/Kconfig \
	/home/jonas/workspace/esp32workspace/get-started/helloworld/components/bmp180Driver/Kconfig \
	/home/jonas/esp32/esp-idf/components/bt/Kconfig \
	/home/jonas/esp32/esp-idf/components/esp32/Kconfig \
	/home/jonas/esp32/esp-idf/components/ethernet/Kconfig \
	/home/jonas/esp32/esp-idf/components/fatfs/Kconfig \
	/home/jonas/esp32/esp-idf/components/freertos/Kconfig \
	/home/jonas/esp32/esp-idf/components/log/Kconfig \
	/home/jonas/esp32/esp-idf/components/lwip/Kconfig \
	/home/jonas/esp32/esp-idf/components/mbedtls/Kconfig \
	/home/jonas/esp32/esp-idf/components/openssl/Kconfig \
	/home/jonas/esp32/esp-idf/components/spi_flash/Kconfig \
	/home/jonas/esp32/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/jonas/esp32/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/jonas/esp32/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/jonas/esp32/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
