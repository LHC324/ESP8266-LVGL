# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/esp_littlefs/include
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/esp_littlefs -lesp_littlefs
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += $(IDF_PATH)/components/esp_littlefs/src/littlefs
COMPONENT_LIBRARIES += esp_littlefs
COMPONENT_LDFRAGMENTS += 
component-esp_littlefs-build: 
