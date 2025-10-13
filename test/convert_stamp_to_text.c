#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  int run_number;
  char data_filename[256];
  char text_filename[256];
  FILE *data_fp;
  FILE *text_fp;
  unsigned int file_size;
  int nevt;
  int evt;
  char data[8];
  int channel;
  double time_tag; 
  int tmp;
  double ftmp;

  if (argc > 1) 
    run_number = atoi(argv[1]);
  else {
    printf("Enter run number : ");
    scanf("%d", &run_number);
  }

  sprintf(data_filename, "time_stamp_%d.dat", run_number);
  sprintf(text_filename, "time_stamp_%d.txt", run_number);
  
  // get file size to know # of events, 1 event = 8 byte
  data_fp = fopen(data_filename, "rb");
  fseek(data_fp, 0L, SEEK_END);
  file_size = ftell(data_fp);
  fclose(data_fp);
  nevt = file_size / 8;
  
  data_fp = fopen(data_filename, "rb");
  text_fp = fopen(text_filename, "wt");
  
  fprintf(text_fp, "Channel, Time stamp(s)\n");
  fprintf(text_fp, "-------------------------------\n");
  
  for (evt = 0; evt < nevt; evt++) {
    fread(data, 1, 8, data_fp);
    
    // get channel
    channel = data[0] & 0xFF;
    
    // get time tag
    tmp = data[1] & 0xFF;
    ftmp = tmp;
    time_tag = ftmp * 0.000000008;
    
    tmp = data[2] & 0xFF;
    ftmp = tmp;
    ftmp = ftmp * 0.000001;
    time_tag = time_tag + ftmp;

    tmp = data[3] & 0xFF;
    ftmp = tmp;
    ftmp = ftmp * 0.000001 * 256.0;
    time_tag = time_tag + ftmp;

    tmp = data[4] & 0xFF;
    ftmp = tmp;
    ftmp = ftmp * 0.000001 * 256.0 * 256.0;
    time_tag = time_tag + ftmp;

    tmp = data[5] & 0xFF;
    ftmp = tmp;
    ftmp = ftmp * 0.000001 * 256.0 * 256.0 * 256.0;
    time_tag = time_tag + ftmp;

    tmp = data[6] & 0xFF;
    ftmp = tmp;
    ftmp = ftmp * 0.000001 * 256.0 * 256.0 * 256.0 * 256.0;
    time_tag = time_tag + ftmp;

    tmp = data[7] & 0xFF;
    ftmp = tmp;
    ftmp = ftmp * 0.000001 * 256.0 * 256.0 * 256.0 * 256.0 * 256.0;
    time_tag = time_tag + ftmp;
   
    fprintf(text_fp, "%d, %lf\n", channel, time_tag);
  }
  
  fclose(data_fp);
  fclose(text_fp);

  return 0;
}

