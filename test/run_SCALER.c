#include <unistd.h>
#include <stdio.h>
#include "NoticeCENS_SCALER.h"

int main(int argc, char *argv[])
{
  int scaler_found;
  int scaler_sid[10];
  int sid;
  int run_number;
  int acq_time;
  int count_period;
  char stamp_filename[256];
  char count_filename[256];
  FILE *fp_stamp;
  FILE *fp_count;
  int data_size_stamp;
  int data_size_count;
  int data_size;
  char *data;
  int run;  
  
  // init usb
  USB3Init();
  
  // scan scaler
  scaler_found = USB3Scan_SCALER(scaler_sid);

  if (!scaler_found) {
    USB3Exit();
    return -1;
  }
  
  // use only first one
  sid = scaler_sid[0];
  
  printf("SCALER found, sid = %d\n", sid);
  
  // open scaler
  SCALER_open(sid);
  
  if (argc > 3) {
    run_number = atoi(argv[1]);
    acq_time = atoi(argv[2]);
    count_period = atoi(argv[3]);
  }
  else {
    printf("Enter run number : ");
    scanf("%d", &run_number);
    printf("Enter acquisition time in sec (1 ~ year) : ");
    scanf("%d", &acq_time);
    printf("Enter counting period in msec (1 ~ sec) : ");
    scanf("%d", &count_period);
  }

  // reset scaler
  SCALER_reset(sid);

  // write settings
  SCALER_write_ACQ_TIME(sid, acq_time);
  SCALER_write_COUNT_PERIOD(sid, count_period);
  
  // readback settings
  printf("Acquision time = %d s\n", SCALER_read_ACQ_TIME(sid));
  printf("Counting period = %d ms\n", SCALER_read_COUNT_PERIOD(sid));

  // assign data buffer
  data = (char *)malloc(128 * 1024); 

  // open data file
  sprintf(stamp_filename, "time_stamp_%d.dat", run_number);
  sprintf(count_filename, "counter_%d.dat", run_number);
  fp_stamp = fopen(stamp_filename, "wb");
  fp_count = fopen(count_filename, "wb");
  
  // reset data size taken
  data_size_stamp = 0;
  data_size_count = 0;
  
  // reset scaler
  SCALER_reset(sid);

  // start scaler
  printf("Starting acquisition...\n");
  SCALER_start(sid);
  
  run = SCALER_read_RUN(sid);
  
  while (run) {
    // read time stamp data
    data_size = SCALER_read_STAMP_DATA_SIZE(sid);
    if (data_size) {
      SCALER_read_STAMP_DATA(sid, data_size, data);
      fwrite(data, 1, data_size * 4, fp_stamp);
      data_size_stamp = data_size_stamp + data_size * 4;
      printf("Time stamp : %d byte written\n", data_size_stamp);
    }
    
    // read count data
    data_size = SCALER_read_COUNTER_DATA_SIZE(sid);
    if (data_size) {
      SCALER_read_COUNTER_DATA(sid, data_size, data);
      fwrite(data, 1, data_size * 4, fp_count);
      data_size_count = data_size_count + data_size * 4;
      printf("Counter : %d byte written\n", data_size_count);
    }

    run = SCALER_read_RUN(sid);
  }
    
  // read remaining time stamp data
  data_size = SCALER_read_STAMP_DATA_SIZE(sid);
  if (data_size) {
    SCALER_read_STAMP_DATA(sid, data_size, data);
    fwrite(data, 1, data_size * 4, fp_stamp);
    data_size_stamp = data_size_stamp + data_size * 4;
    printf("Time stamp : %d byte written\n", data_size_stamp);
  }
    
  // read remaining count data
  data_size = SCALER_read_COUNTER_DATA_SIZE(sid);
  if (data_size) {
    SCALER_read_COUNTER_DATA(sid, data_size, data);
    fwrite(data, 1, data_size * 4, fp_count);
    data_size_count = data_size_count + data_size * 4;
    printf("Counter : %d byte written\n", data_size_count);
  }

  // free data buffer
  free(data);

  // close file
  fclose(fp_stamp);  
  fclose(fp_count);  

  // close scaler  
  SCALER_close(sid);

  // close usb
  USB3Exit();
  
  return 0;
}

