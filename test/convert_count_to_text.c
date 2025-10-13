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
  int data[16];
  int header;
  int index;
  int count[10];
  double time_tag; 
  int trailer;
  int ch;
  unsigned int tmp;
  double ftmp;

  if (argc > 1) 
    run_number = atoi(argv[1]);
  else {
    printf("Enter run number : ");
    scanf("%d", &run_number);
  }

  sprintf(data_filename, "counter_%d.dat", run_number);
  sprintf(text_filename, "counter_%d.txt", run_number);

  
  // get file size to know # of events, 1 event = 64 byte
  data_fp = fopen(data_filename, "rb");
  fseek(data_fp, 0L, SEEK_END);
  file_size = ftell(data_fp);
  fclose(data_fp);
  nevt = file_size / 64;
  
  data_fp = fopen(data_filename, "rb");
  text_fp = fopen(text_filename, "wt");

  fprintf(text_fp, "index, Ch11, Ch12, Ch13, Ch14, Ch15, Ch16, Ch17, Ch18, Ch19, Ch20, Time(s) \n");
  fprintf(text_fp, "-----------------------------------------------------------------------------------\n");

  for (evt = 0; evt < nevt; evt++) {
    fread(data, 4, 16, data_fp);
    
    // get header
    header = data[0] & 0xFFFFFFFF;
    
    // get index
    index = data[1];
    
    // get channel's count
    for (ch = 0; ch < 10; ch++)
      count[ch] = data[ch + 2];
      
    // get time  
    tmp = data[12];
    ftmp = tmp;
    time_tag = ftmp * 0.000000002;
    
    tmp = data[13] & 0xFFFFFFFF;
    ftmp = tmp;
    ftmp = ftmp * 0.000001;
    time_tag = time_tag + ftmp;

    tmp = data[14] & 0xFFFFFFFF;
    ftmp = tmp;
    ftmp = ftmp * 0.000001 * 256.0 * 256.0 * 256.0 * 256.0;
    time_tag = time_tag + ftmp;
    
    // get trailer
    trailer = data[15] & 0xFFFFFFFF;
    
    fprintf(text_fp, "%d, ", index);
    for (ch = 0; ch < 10; ch++)
      fprintf(text_fp, "%d, ", count[ch]);
    fprintf(text_fp, "%lf\n", time_tag);
  }
      
  fclose(data_fp);
  fclose(text_fp);

  return 0;
}

