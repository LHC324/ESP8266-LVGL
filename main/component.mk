#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

#COMPONENT_SRCDIRS := src st7735
#COMPONENT_ADD_INCLUDEDIRS := inc

COMPONENT_SRCDIRS := . \
				  src \
				  inc\
#                  st7735/src \
#				  st7735/inc \

COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_SRCDIRS) .