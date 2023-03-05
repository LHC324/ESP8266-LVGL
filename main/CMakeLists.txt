idf_component_register(SRCS
    "src/main.c"
    "src/lv_port_disp.c"
    "src/lv_port_indev.c"
    INCLUDE_DIRS "" "inc")

# Create a SPIFFS image from the contents of the 'spiffs' directory
# that fits the partition named 'storage'. FLASH_IN_PROJECT indicates that
# the generated image should be flashed when the entire project is flashed to
# the target with 'idf.py -p PORT flash'.
# spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)
