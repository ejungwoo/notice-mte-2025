# Master Trigger Electronics (MTE) by Notice Korea

## Update
- The firmware was updated on September 2025.
- 1-16 : Scaler
- 17-19 : prescaler (and prescaler output). You must RUN to downscale
- 20 : scaler, but clock 을 넣어서 사용...

## Note
- This repository was modified from the [notice-mte](https://gitlab.com/censibs/notice-mte-2025).
- Install libusb.
- Use root account.
- Check with `lsusb` and `dmesg` command for the USB connection.
- Name in the USB device list will show up as LTE.
- Port 1-10 is trigger input.
- Port 11-20 is scaler input.

## Connection to WSL
- Open WSL terminal.
- Open PowerShell with administrator and check usb connection:
```bash
upbipd list
```
- Find device with LTE and remember BUSID.
- Let's say BUSID is 2-10 for example. 
- Connect to WSL by:
```bash
usbipd bind --busid 2-10
usbipd attach --wsl --busid 2-10
```

## Installation
- Connect device using USB-3.0 (cable and port).
- From terminal:
```bash
lsusb
```
- Check if LTE is in the list
  - If not, check physical connection, USB-WSL connection (see above), check if you are using root account.
- Clone repository:
```bash
git clone https://github.com/ejungwoo/notice-mte-2025
cd notice-mte
```
- Compile:
```bash
cd source
make 
make install
cd ..
source notice-mte.sh
cd macros
make
```
- You might want to put `notice-mte.sh` in the login script like:
```bash
echo source notice-mte.sh >> ~/.bashrc 
```
or
```bash
echo source notice-mte.sh >> ~/.zshrc
```

# Run
...

## How to update firware
source notice_env.sh
cd programer # update firmware skip if
./update_scaler.exe # help
./update_scaler.exe -dev all -fpga mte_scaler2.bit
