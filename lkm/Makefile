obj-m := otp_module.o
otp_module-objs := src/otp_module.o src/methods/vige.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

ccflags-y := -I$(src)/include

$(info KDIR: $(KDIR))
$(info PWD: $(PWD))
$(info EXTRA_CFLAGS: $(ccflags-y))

all:
	${MAKE} -C $(KDIR) M=$(PWD) $(MAKE_OPTS) modules

clean:
	${MAKE} -C $(KDIR) M=$(PWD) clean

# Test targets
test-objs := tests/test_lkm.o

test: 
	$(MAKE) -C $(KDIR) M=$(PWD) modules CONFIG_KUNIT=y CONFIG_KUNIT_TEST=m \
		obj-m="otp_module.o test.o"
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install
	depmod -a
	@echo "Running tests..."
	kunit.py run --timeout=30 --kernel_args="loglevel=0"

.PHONY: all clean test
