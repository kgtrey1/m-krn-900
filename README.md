# m-krn-900

## Overview

The objective of this project is to develop a compact loadable kernel module for managing OTPs (One-Time Password) on Linux platforms, along with various tools for interacting with the kernel module, you can find more information in the dedicated [subject file](./SUBJECT.md).

## Quickstart

### Setting up a compatible host

To quickly get a compatible machine for the kernel module look at the [vagrant configuration](./.dev/Vagrantfile) for an Ubuntu 22.04 VM in the directory (see [here](https://developer.hashicorp.com/vagrant/install) to download vagrant, please note that you'll need an hypervisor such as VirtualBox to make it work).  
  
After installation, you can get start the VM by running `vagrant up` from the [.dev directory](./.dev/) (it can take a while at the first time), and then ssh into it with `vagrant ssh`.

### Running the kernel module

⚠️ Please, ensure your platform is compatible for Linux kernel development, you can take a look at the [previous section](#setting-up-a-compatible-host) to setup a compatible host.  
  
To run the kernel module look at the [corresponding section the OTP manager README](./src/otp_manager/README.md#running-the-kernel-module).

## A Project made by

- Lukas Chiaradia   - lukas.chiaradia@epitech.eu
- Kevin Gouyet      - kevin.gouyet@epitech.eu
- Guilhem Santé     - guilhem.sante@epitech.eu

Epitech 2025
