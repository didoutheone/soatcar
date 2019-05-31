#include <sys/stat.h>   // For fstat(): size of file
#include <fcntl.h>              // For O_RDWR, O_RDONLY open file flag
#include <stdio.h>              // For perror, printf
#include <stdlib.h>             // For malloc, exit, EXIT_FAILURE
#include <unistd.h>             // For close: close file
#include "SoatcarState.hpp"


int main(int argc, char *argv[])
{
        SoatcarState state("/var/tmp/soatcarmmf.tmp");

        printf("Set StopFlag to: %d\n", false);
        state.SetStopFlag(false);
        bool stopflag = state.GetStopFlag();
        printf("StopFlag is: %d\n", stopflag);

        printf("Set StopFlag to: %d\n", true);
        state.SetStopFlag(true);
        stopflag = state.GetStopFlag();
        printf("StopFlag is: %d\n", stopflag);

        printf("Set Throttle to: %d\n", 12345);
        state.SetThrottleOutput(12345);
        int throttle = state.GetThrottleOutput();
        printf("Throttle is: %d\n", throttle);

        printf("Set Image no to: %llu\n", 9223372036854775800);
        state.SetRawImageNo(9223372036854775800);
        u64 imageNo = state.GetRawImageNo();
        printf("Image no is: %llu\n", imageNo);

        int fd = open("testfile", O_RDONLY);
        if (fd == -1)
                handle_error("open");
        struct stat sb;
        if (fstat(fd, &sb) == -1)      // Pour obtenir la taille du fichier
                handle_error("fstat");
        DataBuf data;
        data.length = sb.st_size;
        data.buffer = (u8*)malloc(data.length);
        read(fd, data.buffer, data.length);
        printf("Set image to: length %d\n", data.length);
        for(int i = 0; i < 100; i++)
        {
                printf("%02x", data.buffer[i]);
        }
        printf("...");
        for(int i = data.length-100; i < data.length; i++)
        {
                printf("%02x", data.buffer[i]);
        }
        printf("\n");
        state.SetRawImage(data);
        free(data.buffer);

        DataBuf imageData = state.GetRawImage();
        printf("Image size is: %d\n", imageData.length);
        for(int i = 0; i < 100; i++)
        {
                printf("%02x", imageData.buffer[i]);
        }
        printf("...");
        for(int i = imageData.length-100; i < imageData.length; i++)
        {
                printf("%02x", imageData.buffer[i]);
        }
        printf("\n");
        free(imageData.buffer);

}
