#include <iostream>
#include <vector>
#include <iterator>
#include <cstring>
#include <fstream>

#include "update_scaler.h"

using namespace std;

static vector<Option *>* options = new vector<Option *>;

static Option * findOption(char* name) {
  int size = options->size();
  for (int i = 0; i < size; i++) {
    if (!strcmp(name+1, options->at(i)->name)) {
      return options->at(i);
    }
  }
  return NULL;
}

int main(int argv, char** argc) {

  if(argv < 2){
    help_action(NULL,NULL);
  }

  Option help = Option("help", 0, NULL, help_action);
  Option list = Option("list", 0, NULL, list_action);
  Option dev = Option("dev", 1, NULL, dev_action);
  Option info = Option("info", 0, &dev, devinfo_action);
  Option set_a = Option("sid", 1, &dev, write_sid_action);
  Option up_fpga = Option("fpga", 1, &dev, uploadFPGA_action);
  Option up_fx3 = Option("fx3", 1, &dev, uploadFX3_action);

  Option* children[] = {&up_fx3, &up_fpga, &info, &set_a};
  dev.chidrensize = 4;
  dev.children = children;

  options->push_back(&help);
  options->push_back(&list);
  options->push_back(&dev);
  options->push_back(&set_a);
  options->push_back(&up_fpga);
  options->push_back(&up_fx3);
  options->push_back(&info);


  for (int i = 1; i < argv; i++) {
    if (argc[i][0] == '-') {
      Option* option = findOption(argc[i]);
      if (option == NULL || option->argument_size + i >= argv) {
	help_action(&help, NULL);
	return 0;
      }
      if(option->argument_size == 0){
	option->enabled = true;
      }else if (argc[i + 1][0] != '-') {
	option->args = new char*[option->argument_size];
	option->args[0] = argc[++i];
	option->enabled = true;
      } else {
	help_action(&help, NULL);
	return 0;
      }
    }
  }
  int size = options->size();
  for (int i = 0; i < size; i++) {
    Option* option = options->at(i);
    if (option->enabled) {
      if (option->parent == NULL) {
	option->runAction(option, NULL);
      } else if (!option->parent->enabled) {
	help_action(&help, NULL);
      }
    }
  }
  return 0;
}

Option::Option(const char * _name, int _argument_size,Option* _parent, void(*_runAction)(Option* option, void *)) {
  name = _name;
  children = NULL;
  chidrensize = 0;
  parent= _parent;
  argument_size=_argument_size;
  args=NULL;
  enabled=false;
  runAction = _runAction;
}

libusb_context *ctx;
std::vector<Device*>* _devices = NULL;

std::vector<Device*>* getDeviceList() {
  if (_devices != NULL)
    return _devices;
  _devices = new std::vector<Device*>;
  int r;
  int i =0;
  r = libusb_init(&ctx);
  if (r < 0) {
    return NULL;
  }
  libusb_device **devs;
  libusb_device *dev;
  if (libusb_get_device_list(ctx, &devs) < 0) {
    return NULL;
  }
  while ((dev = devs[i++]) != NULL) {
    struct libusb_device_descriptor desc;
    r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
      return NULL;
    }
    if (desc.idVendor == vendor_id && desc.idProduct == product_id) {
      _devices->push_back(new Device(dev));
    }
  }
  return _devices;
}

void uploadFX3_action(Option* option, void *data) {
  Device * dev = (Device*) data;
  fstream file;
  file.open(option->args[0]);
  if (file.is_open()) {
    int length;
    char * buffer;
    file.seekg(0, ios::end);
    length = file.tellg();
    file.seekg(0, ios::beg);
    buffer = new char[length];
    file.read(buffer, length);
    cout << "Uploading firmware to fx3. Please wait." << endl;
    dev->uploadFX3Firmware(length, buffer);
    if (dev->getError() == SUCCESS) {
      cout << "FX3 Firmware has been uploaded successfully.\n";
    } else {
      cout << "Failed to upload FX3 firmware.\n";
    }
    delete[] buffer;
  } else {
    cout << "Could not open file: " << option->args[0] << endl;
  }
  file.close();
}

void uploadFPGA_action(Option* option, void *data) {
  Device * dev = (Device*) data;
  fstream file;
  file.open(option->args[0]);
  if (file.is_open()) {
    int length;
    char * buffer;
    file.seekg(0, ios::end);
    length = file.tellg();
    file.seekg(0, ios::beg);
    buffer = new char[length];
    file.read(buffer, length);
    cout << "Uploading fpga firmware. Please wait.\n";
    dev->uploadFPGAFirmware(length, buffer, option->args[0]);
    if (dev->getError() == SUCCESS) {
      cout << "FPGA Firmware has been uploaded successfully.\n";
    } else
      cout << "Failed to upload FPGA firmware.\n";
    delete[] buffer;
  } else {
    cout << "Could not open file: " << option->args[0] << endl;
  }
  file.close();
}

void devinfo_action(Option* option, void *data) {
  Device * dev = (Device*) data;
  int i;
  char version[256]; 
  int flag;

  dev->readFPGAVersion(version);

  if (dev->getError() != SUCCESS) {
    cout << "Failed to read FPGA vesrion  " << dev->getError() << endl;
    return;
  }

  printf("Version: ");
  for (i = 0; i < 256; i++) {
    flag = version[i] & 0xFF;
    if (flag == 0xFF) {
      printf("\n");
      i = 256;
    }
    else
      printf("%c", version[i]);
  }

  int address = dev->readSID();
  if (dev->getError() != SUCCESS) {
    cout << "Failed to read device SID " << dev->getError() << endl;
    return;
  }
  cout << "SID: " << address << endl;
}

void write_sid_action(Option* option, void *data) {
  Device * dev = (Device*) data;
  unsigned char newaddress = atoi(option->args[0]);
  dev->writeSID(newaddress);
  if (dev->getError() != SUCCESS) {
    cout << "Failed to set new SID  " << dev->getError() << endl;
  }
}

void help_action(Option* option, void *data) {
  cout << "NOTICE Korea (c) 2012 <http://www.noticekorea.com/>\n"
    "Progarmmer, version 1.0\n"
    "Usage: Programmer <options>\n"
    "Options:\n"
    "  -dev <deviceid>            :Choose a device. Deviceid format is\n"
    "                             usb_bus:usb_address. Use option -list\n"
    "                             to get available device list. Use \"all\"\n"
    "                             instead of usb_bus:usb_address to\n"
    "                             select all available devices.\n"
    "  -fx3 <filename>            :Upload fx3 firmware.\n"
    "                             Option -dev has to be set.\n"
    "  -fpga <filename>           :Upload fpga firmware. \n"
    "                             Option -dev has to be set.\n"
    "  -list                      :Lists all available devices.\n"
    "  -sid <board Id#>           :Sets board SID, range is [0..255]\n"
    "                             Option -dev has to be set.\n"
    "  -info                      :Shows device information such as\n"
    "                             firmware version, sid.\n"
    "                             Option -dev has to be set.\n"
    "  -help                      :Shows this message." << endl;
  //TODO: update help message
}

void list_action(Option* option, void *data) {
  std::vector<Device*>* devices = getDeviceList();
  if (devices == NULL) {
    cout << "Failed to init libusb. " << endl;
    return;
  }
  int size = devices->size();
  if (size == 0) {
    cout << "No available devices. " << endl;
    return;
  }
  for (int i = 0; i < size; i++) {
    Device * dev = devices->at(i);
    cout << int(libusb_get_bus_number(dev->getDevice())) << ":" << int(libusb_get_device_address(dev->getDevice())) << endl;
  }
}

void dev_action(Option* option, void *data) {
  std::vector<Device*> *devices = getDeviceList();
  std::vector<Device*>* tempdevices;
  int size;
  if (!strcmp(option->args[0], "all")) {
    tempdevices = devices;
  } else {
    size = devices->size();
    tempdevices = new std::vector<Device*>;
    int bus;
    int address;
    char * devname = strtok(option->args[0], ":\n\0");
    if (devices == NULL) {
      cout << "Wrong format. Please see help for more details." << endl;
      return ;
    }
    bus = atoi(devname);
    devname = strtok(NULL, ":\n\0");
    if (devices == NULL) {
      cout << "Wrong format. Please see help for more details." << endl;
      return ;
    }
    address = atoi(devname);
    for (int i = 0; i < size; i++) {
      Device * dev = devices->at(i);
      if(libusb_get_bus_number(dev->getDevice()) == bus && libusb_get_device_address(dev->getDevice())==address){
	tempdevices->push_back(dev);
      }
    }
  }
  size = tempdevices->size();
  if (size == 0) {
    cout << "Device not found devices. " << endl;
    return;
  }
  for (int i = 0; i < size; i++) {
    Device * dev = tempdevices->at(i);
    cout << int(libusb_get_bus_number(dev->getDevice())) << ":" << int(libusb_get_device_address(dev->getDevice())) << endl;
    dev->open();
    if(dev->getError()!=SUCCESS){
      cout << "Failed to open device" << endl;
      continue;
    }
    for (int j = 0; j < option->chidrensize; j++) {
      if (option->children[j]->enabled) {
	option->children[j]->runAction(option->children[j], dev);
      }
    }
  }
}

ErrorCode Device::read(int request, uint16_t value, uint16_t index, size_t lenght, void *data) {
  int res;
  if (handle == NULL) {
    return NULL_POINTER;
  }
  res = libusb_control_transfer(handle, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN, request, value, index,
				(unsigned char *) data, lenght, 5000);
  if (res < 0) {
    return LIBUSB_ERROR;
  }
  return SUCCESS;
}

ErrorCode Device::write(int request, uint16_t value, uint16_t index, size_t lenght, void *data) {
  int res;
  if (handle == NULL) {
    return NULL_POINTER;
  }
  res = libusb_control_transfer(handle, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT, request, value, index,
				(unsigned char *) data, lenght, 5000);
  if (res < 0) {
    std::cout << "ERROR CODE  " << res << std::endl;
    return LIBUSB_ERROR;
  }
  return SUCCESS;
}

void Device::uploadFX3Firmware(size_t size, char *data) 
{
  int nsector;
  int naddr;
  int rem;
  int sector;
  int addr;
  int j;
  
  nsector = size / 65536;
  naddr = (size % 65536) / 256;
  rem = (size % 65536) % 256;
  
  for (sector = 0; sector < nsector; sector++) {
    for (addr = 0; addr < 256; addr++) {
      ep0buffer[0] = sector & 0xFF;
      ep0buffer[1] = addr & 0xFF;
      for (j = 0; j < 256; j++)
	ep0buffer[j + 2] = data[sector * 65536 + addr * 256 + j];

      error = write(VENDOR_I2C_EEPROM_WRITE, 0, 0, 258, ep0buffer);
      std::cerr << ".";
    }
  }

  for (addr = 0; addr < naddr; addr++) {
    ep0buffer[0] = nsector & 0xFF;
    ep0buffer[1] = addr & 0xFF;
    for (j = 0; j < 256; j++)
      ep0buffer[j + 2] = data[nsector * 65536 + addr * 256 + j];
    
    error = write(VENDOR_I2C_EEPROM_WRITE, 0, 0, 258, ep0buffer);
    std::cerr << ".";
  }

  if (rem) {
    ep0buffer[0] = nsector & 0xFF;
    ep0buffer[1] = naddr & 0xFF;
    for (j = 0; j < rem; j++)
      ep0buffer[j + 2] = data[nsector * 65536 + naddr * 256 + j];

    for (j = rem; j < 256; j++)
      ep0buffer[j + 2] = data[nsector * 65536 + naddr * 256 + rem - 1];

    error = write(VENDOR_I2C_EEPROM_WRITE, 0, 0, 258, ep0buffer);
    std::cerr << ".";
  }

  std::cerr << std::endl;
  error = SUCCESS;
}

void Device::uploadFPGAFirmware(size_t size, char *data, char *filename) 
{
  int nsector;
  int naddr;
  int rem;
  int sector;
  int addr;
  int j;
  
  nsector = size / 65536;
  naddr = (size % 65536) / 256;
  rem = (size % 65536) % 256;

  for (sector = 0; sector < nsector; sector++) {
    // erase sector
    ep0buffer[0] = (sector + 2) & 0xFF;
    error = write(VENDOR_FLASH_ERASE, 0, 0, 1, ep0buffer);
    
    // write data
    for (addr = 0; addr < 256; addr++) {
      ep0buffer[0] = (sector + 2) & 0xFF;
      ep0buffer[1] = addr & 0xFF;
      for (j = 0; j < 256; j++)
	ep0buffer[j + 2] = data[sector * 65536 + addr * 256 + j];

      error = write(VENDOR_FLASH_WRITE, 0, 0, 258, ep0buffer);
    }
    
    std::cerr << ".";
  }

  // erase sector
  ep0buffer[0] = (nsector + 2) & 0xFF;
  error = write(VENDOR_FLASH_ERASE, 0, 0, 1, ep0buffer);

  // write data
  for (addr = 0; addr < naddr; addr++) {
    ep0buffer[0] = (nsector + 2) & 0xFF;
    ep0buffer[1] = addr & 0xFF;
    for (j = 0; j < 256; j++)
      ep0buffer[j + 2] = data[nsector * 65536 + addr * 256 + j];
    
    error = write(VENDOR_FLASH_WRITE, 0, 0, 258, ep0buffer);
  }

  if (rem) {
    ep0buffer[0] = (nsector + 2) & 0xFF;
    ep0buffer[1] = naddr & 0xFF;
    for (j = 0; j < rem; j++)
      ep0buffer[j + 2] = data[nsector * 65536 + naddr * 256 + j];

    for (j = rem; j < 256; j++)
      ep0buffer[j + 2] = data[nsector * 65536 + naddr * 256 + rem - 1];

    error = write(VENDOR_FLASH_WRITE, 0, 0, 258, ep0buffer);
  }
  
  std::cerr << ".";

  ep0buffer[0] = 1;
  error = write(VENDOR_FLASH_ERASE, 0, 0, 1, ep0buffer);

  ep0buffer[0] = 1;
  ep0buffer[1] = 0;
  rem = 256;

  for (j = 0; j < 256; j++) {
    if (filename[j] == 0) {
      rem = j;
      j = 256;
    }
    else
      ep0buffer[j + 2] = filename[j] & 0xFF;
  }

  for (j = rem; j < 256; j++)
    ep0buffer[j + 2] = 0xFF;
  
  error = write(VENDOR_FLASH_WRITE, 0, 0, 258, ep0buffer);
  std::cerr << ".";

  // finish flash
  ep0buffer[0] = 0;
  error = write(VENDOR_FLASH_FINISH, 0, 0, 0, ep0buffer);

  std::cerr << std::endl;
  error = SUCCESS;
}

unsigned char Device::readSID() 
{
  unsigned char result;

  error = read(VENDOR_READ_SID, 0, 0, 1, ep0buffer);
  if (error != SUCCESS) {
    return 0;
  }
  result = ep0buffer[0];

  return result;
}

void Device::writeSID(unsigned char value) 
{
  if (value > 255) {
    error = WRONG_VALUE;
    return;
  }

  ep0buffer[0] = value & 0xFF;
  error = write(VENDOR_WRITE_SID, 0, 0, 1, ep0buffer);
  if (error != SUCCESS) {
    return;
  }
}

void Device::readFPGAVersion(char *data) 
{
  int i;

  error = read(VENDOR_READ_FPGA_VERSION, 0, 0, 256, ep0buffer);
  if (error != SUCCESS) {
    return;
  }

  for (i = 0; i < 256; i++) 
    data[i] = ep0buffer[i] & 0xFF;
}

Device::Device(libusb_device * _device) {
  ep0buffer = new char[260];
  device = _device;
}

Device::~Device() {
  delete[] ep0buffer;
}

void Device::open() {
  handle = NULL;
  int r = libusb_open(device, &handle);
  if (r < 0) {
    handle = NULL;
    error = DEVICE_CANNOT_BE_OPENED;
  } else {
    error = SUCCESS;
  }

}

void Device::close() {
  if (handle != NULL) {
    libusb_close(handle);
  }
}

libusb_device *Device::getDevice() const {
  return device;
}

ErrorCode Device::getError() {
  return error;
}



