# otp_manager

## Running the kernel module

### Compiling

You can compile the OTP manager kernel module leveraging its [Makefile](./src/otp_manager/Makefile) running:

```sh
make
``` 

### Loading

If the build succeeded you should be able to locate a `otp_manager.ko` that you can load to your kernel with the command:

```sh
sudo insmod otp_manager.ko
```

If the kernel module loading succeeded you should be able to see the following log in your kernel diagnostic messages with the command `dmesg | tail`:

```
[ 1234.567890 ] Hello, World! The OTP Manager LKM has been loaded.
```

### Unloading

To unload the kernel module you can run the command:

```sh
sudo rmmod otp_manager
```

Likewise you should be able to see a new message in the kernel diagnostic messages with `dmesg | tail`:

```
[ 1234.567890 ] Goodbye, World! The OTP Manager LKM has been unloaded.
```

### Cleaning build artifacts
You can clean the build artifacts running the command:

```
make clean
```
