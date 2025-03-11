obj-m := otp_module.o
otp_module-objs := src/otp_module.o src/methods/vige.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

ccflags-y := -I$(src)/include

$(info KDIR: $(KDIR))
$(info PWD: $(PWD))
$(info EXTRA_CFLAGS: $(ccflags-y))

all:
	${MAKE} -C $(KDIR) M=$(PWD)  $(MAKE_OPTS) modules

clean:
	${MAKE} -C $(KDIR) M=$(PWD) clean
