#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libusb.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCALER_VENDOR_ID  (0x0547)
#define SCALER_PRODUCT_ID (0x1309)

#define USB3_SF_READ   (0x82)
#define USB3_SF_WRITE  (0x06)

extern void USB3Init(void);
extern void USB3Exit(void);
extern int USB3Scan_SCALER(int *sid); 
extern int SCALER_open(int sid);
extern void SCALER_close(int sid);
extern void SCALER_reset(int sid);
extern void SCALER_start(int sid);
extern int SCALER_read_RUN(int sid);
extern void SCALER_write_ACQ_TIME(int sid, int data);
extern int SCALER_read_ACQ_TIME(int sid);
extern void SCALER_write_COUNT_PERIOD(int sid, int data);
extern int SCALER_read_COUNT_PERIOD(int sid);
extern void SCALER_write_DOWN_SCALE(int sid, int ch, int data);
extern int SCALER_read_DOWN_SCALE(int sid, int ch);
extern void SCALER_write_INPUT_TYPE(int sid, int ch, int data);
extern int SCALER_read_INPUT_TYPE(int sid, int ch);
extern int SCALER_read_STAMP_DATA_SIZE(int sid);
extern int SCALER_read_COUNTER_DATA_SIZE(int sid);
extern void SCALER_read_STAMP_DATA(int sid, int data_size, char *data);
extern void SCALER_read_COUNTER_DATA(int sid, int data_size, char *data);

#ifdef __cplusplus
}
#endif

