#include <unistd.h>
#include <stdio.h>
#include "NoticeCENS_SCALER.h"

int main(void)
{
  int scaler_found;
  int scaler_sid[10];
  int sid;
  FILE *fp;
  char filename[100];
  int down_scale[3];
  int sig_level[20];
  int ch;
  
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

  // read setup file
  sprintf(filename, "setup.txt");
  fp = fopen(filename, "rt");
  for (ch = 0; ch < 3; ch++) 
    fscanf(fp, "%d", down_scale + ch);
  for (ch = 0; ch < 20; ch++) 
    fscanf(fp, "%d", sig_level + ch);
  fclose(fp);
  
  // write settings
  for (ch = 0; ch < 3; ch++) 
    SCALER_write_DOWN_SCALE(sid, ch + 17, down_scale[ch]);
  for (ch = 0; ch < 20; ch++) 
    SCALER_write_INPUT_TYPE(sid, ch + 1, sig_level[ch]);
  
  // readback settings
  for (ch = 17; ch <= 19; ch++) 
    printf("CH%d down-scale = %d\n", ch, SCALER_read_DOWN_SCALE(sid, ch));
  for (ch = 1; ch <= 20; ch++) 
    printf("CH%d signal level = %d\n", ch, SCALER_read_INPUT_TYPE(sid, ch));
  
  // close scaler  
  SCALER_close(sid);

  // close usb
  USB3Exit();
  
  return 0;
}

