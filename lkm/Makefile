obj-m := otp_module.o
otp_module-objs := src/otp_module.o src/methods/dict.o src/methods/time.o src/utils/dictionnary.o src/password.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

ccflags-y := -I$(src)/include -I$(src)/../common/include -Wall

all:
	${MAKE} -C $(KDIR) M=$(PWD) $(MAKE_OPTS) modules

clean:
	${MAKE} -C $(KDIR) M=$(PWD) clean
