#include <libusb-1.0/libusb.h>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <iterator>
#include <fstream>

#define  vendor_id  0x0547
#define  product_id 0x1309


// module protocol
#define VENDOR_I2C_EEPROM_WRITE       (0xBA)
#define VENDOR_FLASH_WRITE            (0xC2)
#define VENDOR_FLASH_ERASE            (0xC4)
#define VENDOR_FLASH_FINISH           (0xC5)
#define VENDOR_WRITE_SID              (0xD3)
#define VENDOR_READ_SID               (0xD2)
#define VENDOR_READ_FPGA_VERSION      (0xD4)

class Device;

struct Option {
  const char * name;
  Option** children;
  int chidrensize;
  Option* parent;
  int argument_size;
  char** args;
  bool enabled;
  void (*runAction)(Option* option, void *);
  Option(const char * _name, int _argument_size,Option* _parent, void(*_runAction)(Option* option, void *)) ;
};

enum ErrorCode{
	SUCCESS = 0,
	DEVICE_NOT_FOUND,
	DEVICE_CANNOT_BE_OPENED,
	SESSION_INITIALIZATION_FAILED,
	UNKNOWN,
	NULL_POINTER,
	LIBUSB_ERROR,
	WRONG_VALUE
};

std::vector<Device*> * getDeviceList();

class Device {
 private:
  libusb_device_handle * handle;
  libusb_device * device;
  int progress;
  ErrorCode error;
  char * ep0buffer;
  ErrorCode read(int request, uint16_t value, uint16_t index, size_t lenght, void *data);
  ErrorCode write(int request, uint16_t value, uint16_t index, size_t lenght, void *data);
 public:
  Device(libusb_device * device);
  ~Device();

  void open();
  void close();
  void uploadFX3Firmware(size_t lenght, char *data);
  void uploadFPGAFirmware(size_t lenght, char *data, char *filename);
  unsigned char readSID();
  void writeSID(unsigned char value);
  void readFPGAVersion(char *data);
  void writeFPGAVersion(char *data);
  ErrorCode getError();
  libusb_device *getDevice() const;
};

void uploadFX3_action(Option* option, void *data);
void uploadFPGA_action(Option* option, void *data);
void devinfo_action(Option* option, void *data);
void write_sid_action(Option* option, void *data);
void write_version_action(Option* option, void *data);
void help_action(Option* option, void *data);
void list_action(Option* option, void *data);
void dev_action(Option* option, void *data);


