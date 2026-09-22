# Building ArduPilot firmware for the Saolatek H743VIT FC

> **Note:** Building requires Ubuntu 24.04. WSL can be used instead.

## Clone the ArduPilot source code

```bash
git clone https://github.com/ArduPilot/ardupilot.git
```

## Set up the build environment

```bash
cd ardupilot
```
## Update the submodules:
```bash
git submodule update --init --recursive
```
## Run the script that installs the packages required by the build environment:
```bash
Tools/environment_install/install-prereqs-ubuntu.sh -y
```
## When installation finishes, reload the environment variables:
```bash
source ~/.profile
```
## Check that the build tool installed correctly:
```bash
./waf --version
```
# Overview of building firmware from ArduPilot
A build needs a description of the hardware and of the bootloader, in 2 files:
* hwdef.dat
* bl-hwdef.dat
