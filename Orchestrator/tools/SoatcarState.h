#ifndef DEF_SOATCARSTATE
#define DEF_SOATCARSTATE

#include <stdbool.h>

#define STOP_FLAG			0
#define THROTTLE_VALUE		150
#define IMAGE_NO			230
#define CAMERA_IMAGE		1048576

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)
typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

struct DataBuf
{
    u32 length;		// data length
    char* buffer; 	// data
};
typedef struct DataBuf DataBuf;

char* _mmap;
u32 _getInt(int offset);
u64 _getLong(int offset);
char* _getString(int offset);
DataBuf _getData(int offset);

void InitMMap(char* filename);
bool GetStopFlag();
u32 GetThrottle();
u64 GetImageNo();
DataBuf GetImage();
#endif