# Define module name
obj-m += otp_manager.o

# Define module source objects
otp_manager-objs := main.o \
					list_based_otp/ioctl.o \
					time_based_otp/ioctl.o

# Kernel source directory
KERNEL_SOURCE := /lib/modules/$(shell uname -r)/build

# Add include directory to module compilation include paths
EXTRA_CFLAGS += -I $(PWD)/../../include

all:
	$(MAKE) -C "$(KERNEL_SOURCE)" M=$(PWD) $(MAKE_OPTS) modules

clean:
	$(MAKE) -C "$(KERNEL_SOURCE)" M=$(PWD) $(MAKE_OPTS) clean
