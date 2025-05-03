# Kernel OTP Manager (m-krn-900)

## Overview

This project implements a one-time password (OTP) management system in the Linux kernel, along with user-space tools to configure the OTP system and validate OTPs. The kernel module provides OTP generation and verification with two methods:

- **Dictionary-based OTP** – OTPs are generated as combinations of words from a configurable word list.
- **Time-based OTP** – OTPs are generated using the current time and a secret key (similar to TOTP), producing a 6-digit numeric code.

The purpose is to demonstrate a simple kernel-space OTP generator/validator and user-space utilities to interact with it. Once the kernel module is loaded, it registers character devices that user programs can use to configure OTP settings (like adding words or setting the secret key) and to retrieve or verify one-time passwords.

## Architecture and Internal Design

### Kernel Module Components

The core of the system is a Linux kernel module (`otp_module.ko`) that manages OTP generation and validation. When loaded, the module registers two devices (via **misc char devices**): one for the dictionary method and one for the time-based method. These appear as `/dev/kotp_dict` and `/dev/kotp_time`. Each device corresponds to an OTP method and has its own file operations for reading and ioctl commands.

Key kernel data structures and components include:

- **OTP Linked List:** The module maintains a linked list of generated OTPs (`OneTimePassword_t`). Each OTP entry stores the OTP string, the method it belongs to (`"dict"` or `"time"`), an expiration timestamp (if any), and a flag indicating if the OTP has been issued to a user. This list is used to verify if an entered password is valid and unused.
- **OTP Methods Interface:** An `OtpMethod_t` struct represents an OTP method. It contains pointers to method-specific initialization and cleanup functions, and custom file operations (`struct file_operations`) for that method’s device. On module load, two `OtpMethod_t` instances are created – one for `"dict"` and one for `"time"`. They are initialized and registered as misc devices:
  - **Dictionary Method:** Generates OTPs as two random words concatenated. It uses a kernel-resident dictionary (word list) from which it picks words.
  - **Time-based Method:** Generates OTPs as a 6-digit code derived from the current time and a secret key. The code is computed by taking the current time (nanosecond portion) XORed with a simple hash of the secret key, then reduced modulo 1,000,000 (to yield 6 decimal digits).  
- **Device Read Operation:** Reading from the device file (e.g. using `cat`) triggers the generation of a new OTP for that method:
  - For the dictionary device, the read handler checks that at least two words are available in the list. It then selects two distinct random words and concatenates them to form the OTP (e.g. `"apple"+"banana" = "applebanana"`). If an expiration time is set for dictionary OTPs, the OTP entry is marked with an expiration timestamp (current time + validity period). If no expiration is set, the validity is stored as 0 (meaning “no expiry”). The OTP text (with a newline) is returned to the reading process, and the OTP is stored in the kernel’s OTP list as “assigned” (issued to a user).
  - For the time-based device, the read handler uses the secret key and current time to compute a 6-digit code. If an expiration window is configured for time OTPs, the code’s expiration timestamp is set (current time + validity period); otherwise no expiry. The code (as a zero-padded 6-digit string with a newline) is returned, and the OTP is stored in the kernel list as assigned.
  - Both read operations enforce that only one read is needed per OTP (if the user-space program tries to read again without closing, it will get an end-of-file).
- **Ioctl Interface:** The module defines several ioctl commands (in `io.h`) to allow user-space configuration and password verification:
  - `VERIFY_PASSWORD_CMD` – Verify a provided OTP. The kernel checks if the given password exists in its OTP list (for the specified method device) and whether it’s unexpired. If valid, it consumes (deletes) that OTP from the list. The result is reported via a response message.
  - **Dictionary-Method Commands:**  
    - `DICT_ADD` – Add a new word to the kernel’s dictionary list.  
    - `DICT_DELETE` – Remove a word (by its ID in the list) from the dictionary.  
    - `DICT_CONFIG` – Query the dictionary configuration: the current expiration time setting and the full list of words (with their IDs).  
    - `DICT_TIME` – Set the OTP validity period (expiration time in seconds) for dictionary-generated OTPs (use `0` for no expiration).
  - **Time-Method Commands:**  
    - `TIME_KEY` – Set the secret key used for time-based OTP generation.  
    - `TIME_CONFIG` – Query the time-method configuration: current expiration time and secret key value.  
    - `TIME_TIME` – Set the validity period (in seconds) for time-based OTPs (how long a generated code remains valid; `0` means no expiration).  

When a configuration ioctl is called, the kernel populates a response string (`resp` buffer in the `data_transfer_t` structure) with a success or error message. For example, adding a word might return a success message, or querying config will return the list of words or current settings. The user-space tools display these messages to the user.

- **OTP Consumption Logic:** All OTP verification uses a common function in the kernel (`consume_password`). When an OTP is verified via `VERIFY_PASSWORD_CMD`, the kernel searches the OTP linked list for a matching entry (by method and password). If found, it checks if the OTP has an expiration and if it has expired. It returns one of three results: success (found and valid), not found (no matching OTP), or expired (found but time expired). On success or expiration, that OTP entry is removed from the list (so it cannot be reused). This ensures each OTP can only be used once. The result is communicated back via the ioctl call’s return value and the message in the response buffer (e.g. “Success: Password is correct”, “Error: Password has expired”, or “Error: Invalid password”).

- **Lifetime and Cleanup:** The module’s initialization (`otp_init`) allocates devices and makes them available. The methods are marked as initialized if their setup succeeded. On module exit (`otp_exit`), it deregisters the devices, frees any allocated resources (including the device names and any remaining OTPs in the list), and clears the dictionary. It also logs messages via `printk` (`pr_info`/`pr_err`) at each step for debugging.

### User-Space Utilities

Two C programs are provided: **`otp_config`** for configuration and **`otp_client`** for OTP consumption. They use the ioctl interface to communicate with the kernel module. A small common library (`common/`) provides shared routines (like device name construction and device enumeration).

- **libkotp (common library):** Provides helper functions for the user programs, such as `create_device_name()` which prepends the required `"/dev/kotp_"` prefix to a method name (to form the device path), and functions to list available OTP methods by scanning `/dev` (looking for device names starting with `kotp_`). These help the tools dynamically adapt to available methods.

- **OTP Configuration Utility (`otp_config`):** This command-line tool is used to set up or query the OTP system. It must be run with appropriate privileges (in practice, adding words or keys requires interacting with the device, which is world-writable by default in this project for simplicity, but loading the module requires root). Its usage format is:  

  ```
  otp_config <method> <command> [argument]
  ```  

  where `<method>` is either "`dict`" or "`time`", and `<command>` depends on the method. For each method, `-h` (or `help`) will display usage instructions. Key commands include:
  - **Dictionary mode (`otp_config dict ...`):**  
    - `otp_config dict add <word>` – Add a word to the kernel’s dictionary list. The word must be 3–16 characters long (an error is returned if outside this range or if the word already exists in the list).  
    - `otp_config dict delete <id>` – Remove a word by its numerical ID. The IDs for words can be found via the `config` command (see below). If the ID does not exist, an error is shown.  
    - `otp_config dict time <seconds>` – Configure the validity period for dictionary OTPs. This sets how long (in seconds) a generated word-combination OTP remains valid. Use `0` to disable expiration (OTPs will not expire over time, only consumed once when used).  
    - `otp_config dict config` – Display the current configuration of the dictionary method. This prints the expiration time (in seconds) currently set, followed by a "Wordlist" listing each word in the dictionary and its ID. For example, it might output an ID and word per line like "`1: apple`".  
  - **Time-based mode (`otp_config time ...`):**  
    - `otp_config time key <string>` – Set the secret key for time-based OTP generation. The key must be 3–32 characters long. This key is used by the kernel to compute OTP codes (it’s not stored encrypted, just kept in kernel memory). Changing the key will affect newly generated OTP codes.  
    - `otp_config time time <seconds>` – Set the validity period for time-based OTPs. This defines how long a generated 6-digit code remains valid before it is considered expired. For example, setting `30` means an OTP code must be used within 30 seconds of being generated; `0` means no time expiration (though a new code is only generated when requested).  
    - `otp_config time config` – Display the current configuration of the time-based method. This shows the expiration time (in seconds) and the current secret key. The secret key is shown in plaintext as stored (for a real system, you might not want to display it, but here it’s for demonstration).  

  Internally, `otp_config` will open the appropriate device (based on the `<method>` argument) and issue the corresponding ioctl with any provided argument. It prints out either a success message or an error message returned by the kernel (for example, after `add` it might print "Success: <word> has been added to the dictionary.", or after `config` it will print the configuration details). All output is human-readable.

- **OTP Client Utility (`otp_client`):** This is a simpler tool intended for consuming OTPs (i.e., verifying a one-time password). Its usage is:  

  ```
  otp_client <method> <password>
  ```  

  where `<method>` is `dict` or `time`, and `<password>` is the one-time password string the user wants to verify. The client will open the corresponding device and send the `VERIFY_PASSWORD_CMD` ioctl with the provided password. The kernel will check the OTP:
  - If the OTP is correct (matches an existing entry that has not expired), the kernel responds with a success message (and removes that OTP from its list so it cannot be reused). The client then prints something like "Success: Password is correct". The program’s exit code will be 0 for success.
  - If the OTP is not found or was already used, the kernel responds with "Error: Invalid password" (and the client prints this to stderr). If the OTP had existed but expired, the message will be "Error: Password has expired". In both error cases, the client returns a non-zero exit code (2 for a general error in this implementation).  
  The `otp_client` program also supports `-h` to list available methods (by scanning `/dev` for `kotp_*` devices) and usage. Since the device files are created with read/write permissions for all users, any user can run `otp_client` to attempt an OTP verification (provided the module is loaded and configured).

### Compilation and Installation

To build and run this project, you will need a Linux environment with kernel development support (appropriate kernel headers installed) since it includes a kernel module. The code is organized into directories by component:

- `lkm/` – The loadable kernel module source code.
- `common/` – A static library with code shared by the user programs (for device I/O helpers).
- `config/` – Source for the `otp_config` utility.
- `client/` – Source for the `otp_client` utility.

**Building:** Each component has a Makefile. The recommended build order is:
1. **Common library:**  
   Navigate to the `common/` directory and run `make`. This produces `libotp.a`, a static library required by the other programs.
2. **Config utility:**  
   Go to `config/` and run `make`. This will compile the configuration tool (`otp_config`) and link against the common library.
3. **Client utility:**  
   In the `client/` directory, run `make` to compile the OTP client (`otp_client`) and link with `libotp.a`.
4. **Kernel module:**  
   In the `lkm/` directory, run `make`. Ensure your `KDIR` (in the Makefile) points to your kernel build headers (the Makefile uses the current system kernel headers by default). This will produce `otp_module.ko`.

If all builds succeed, you will have the following outputs:
- `lkm/otp_module.ko` – the kernel module.
- `config/otp_config` – the configuration CLI tool.
- `client/otp_client` – the OTP client tool.
- (The common library `common/libotp.a` and intermediate object files in each directory are also built as part of this process.)

**Installing the Module:** 
To load the kernel module, you must have root privileges:
```bash
sudo insmod otp_module.ko
``` 
This inserts the module into the kernel. On insertion, you should see kernel log messages (via `dmesg`) indicating the module was loaded and that the OTP methods were initialized. The module registers two misc devices; you can verify they exist:
```bash
ls -l /dev/kotp_*
```
You should see `/dev/kotp_dict` and `/dev/kotp_time` with device permissions set to allow read/write for users (mode `rw-rw-rw-`). This means regular users can open these devices to get OTPs or perform ioctls, though configuring the system is typically done by an admin.

If you ever need to unload the module, use:
```bash
sudo rmmod otp_module
```
This will free all resources. The device files under `/dev` will be removed automatically by the kernel on unload.

**Note:** Ensure that the kernel version you built the module against matches the running kernel version.

## Usage Examples

Below are some examples of how to use the OTP system after installation:

1. **Setup Dictionary OTP Method:**  
   Load the module and add words to the dictionary:
   ```bash
   sudo insmod otp_module.ko
   ./otp_config dict add apple
   ./otp_config dict add banana
   ./otp_config dict add cherry
   ./otp_config dict config
   ```
   Then generate and consume an OTP:
   ```bash
   OUTPUT=$(cat /dev/kotp_dict)
   echo "OTP Code: $OUTPUT"
   ./otp_client dict $OUTPUT
   ```

2. **Setup Time-based OTP Method:**  
   Configure and generate a time-based OTP:
   ```bash
   ./otp_config time key MySecretKey123
   ./otp_config time time 30
   ./otp_config time config
   CODE=$(cat /dev/kotp_time)
   echo "Current OTP code: $CODE"
   ./otp_client time $CODE
   ```

3. **Listing Available Methods:**  
   ```bash
   ./otp_config -h
   ./otp_client -h
   ```

Remember that you can always adjust the dictionary words and expiration times on the fly. To clean up, unload the kernel module:
```bash
sudo rmmod otp_module
```
