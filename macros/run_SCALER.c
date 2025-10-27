#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdatomic.h>
#include "NoticeCENS_SCALER.h"

#define num_channels 16

volatile sig_atomic_t signalInterrupted = 0;

static void signalHandler(int signum) {
    // async-signal-safe output instead of printf
    const char msg[] = "Interrupt received\n";
    (void)write(STDOUT_FILENO, msg, sizeof msg - 1);
    signalInterrupted = 1;
}

int main(int argc, char *argv[])
{
    struct sigaction sa = {0};
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    // Do NOT set SA_RESTART so syscalls like sleep/read will return with EINTR
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    int scaler_sid[10];
    int run_number = -1;
    int acq_time = 60*60*24*30;
    int count_period = 1000;
    char stamp_filename[256];
    char count_filename[256];
    FILE *fp_stamp;
    FILE *fp_count;
    int data_size;
    int run;  
    int selection;

    // XXX
    FILE *fileScaler;
    unsigned int file_size = 0L;
    unsigned int file_size_old = 0L;
    int countsInEvent[num_channels];
    double countSumInEvent[num_channels] = {0.};
    double countSOSInEvent[num_channels] = {0.};
    int iEventLast = 0;

    // init usb
    USB3Init();

    // scan scaler
    int scaler_found = USB3Scan_SCALER(scaler_sid);

    if (!scaler_found) {
        USB3Exit();
        return -1;
    }

    // use only first one
    int sid = scaler_sid[0];

    printf("SCALER found, sid = %d\n", sid);

    // open scaler
    SCALER_open(sid);

    // reset scaler
    SCALER_reset(sid);

    // write settings
    SCALER_write_ACQ_TIME(sid, acq_time);
    SCALER_write_COUNT_PERIOD(sid, count_period);

    // readback settings
    printf("Acquision time = %d s\n", SCALER_read_ACQ_TIME(sid));
    printf("Counting period = %d ms\n", SCALER_read_COUNT_PERIOD(sid));

    // assign data buffer
    char *data = (char *)malloc(128 * 1024); 

    // reset data size taken
    int data_size_stamp = 0;
    int data_size_count = 0;

    // reset scaler
    SCALER_reset(sid);

    // start scaler
    printf("Starting acquisition...\n");
    SCALER_start(sid);

    while (1)
    {
        if (run_number>=0)
        {
            selection = 2;
            puts("");
            puts("0) End");
            puts("1) New run ");
            puts("2) Continue ");
            printf("== Select from above : ");
            scanf("%d", &selection);
            if (selection==0)
                break;
        }

        if (selection==1)
        {
            printf("Enter run number : ");
            scanf("%d", &run_number);

            // open data file
            sprintf(stamp_filename, "time_stamp_%d.dat", run_number);
            sprintf(count_filename, "counter_%d.dat", run_number);
            fp_stamp = fopen(stamp_filename, "wb");
            fp_count = fopen(count_filename, "wb");
        }

        run = SCALER_read_RUN(sid);

        while (run && !signalInterrupted)
        {
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

            // XXX
            fileScaler = fopen(count_filename, "rb");
            if (fileScaler==NULL) {
                printf("==ERROR== Cannot open %s", count_filename);
                break;
            }
            fseek(fileScaler, 0L, SEEK_END);
            file_size = ftell(fileScaler);
            if (file_size==file_size_old)
                usleep(2*1000000);
            fclose(fileScaler);
            int numEvents = file_size/64;

            //XXX
            // find file size and number of events
            fileScaler = fopen(count_filename, "rb");
            fseek(fileScaler, file_size_old, SEEK_SET);
            file_size_old = file_size;
            for (auto iEvent=iEventLast; iEvent<numEvents; ++iEvent)
            {
                fread(countsInEvent,4,num_channels,fileScaler);
                int eventNumber = countsInEvent[0];
                for (auto iChannel=0; iChannel<num_channels; ++iChannel)
                {
                    auto count = countsInEvent[iChannel+1];
                    countSumInEvent[iChannel] += count;
                    countSOSInEvent[iChannel] += count*count;
                }
            }
            iEventLast = numEvents;

            run = SCALER_read_RUN(sid);
        }
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
    puts("== Exit");

    return 0;
}

