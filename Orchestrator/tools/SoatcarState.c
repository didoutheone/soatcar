#include <sys/mman.h>	// For mmap(), PROT_READ, PROT_WRITE, MAP_SHARED, MAP_FAILED: memory map file
#include <sys/stat.h>	// For fstat(): size of file
#include <fcntl.h>		// For O_RDWR: open file flag
#include <stdio.h>		// For perror, printf
#include <stdlib.h>		// For malloc, exit, EXIT_FAILURE
#include <unistd.h>		// For close: close file
#include <string.h>		// For memcpy: copy memory
#include "SoatcarState.h"

// -------------------------
//         PRIVATE
// -------------------------
bool _getFlag(int offset)
{
	if(_mmap[offset] == 0) return false;
	else return true;
}

u32 _getInt(int offset)
{
	return ((u8)_mmap[offset + 3] << 24) | ((u8)_mmap[offset + 2] << 16) | ((u8)_mmap[offset + 1] << 8) | (u8)_mmap[offset + 0];
}

u64 _getLong(int offset)
{
	return ((u64)_mmap[offset + 7] << 56) | ((u64)_mmap[offset + 6] << 48) | ((u64)_mmap[offset + 5] << 40) | ((u64)_mmap[offset + 4] << 32)
		| ((u64)_mmap[offset + 3] << 24) | ((u64)_mmap[offset + 2] << 16) | ((u64)_mmap[offset + 1] << 8) | ((u64)_mmap[offset + 0] << 0);
}

char* _getString(int offset)
{
	u32 length = _getInt(offset);
	char* data = malloc(length + 1);
	memcpy(data, _mmap + offset + 4, length);
	data[length] = '\0';
	return data;
}

DataBuf _getData(int offset)
{
	u32 length = _getInt(offset);
	u8* data = malloc(length);
	memcpy(data, _mmap + offset + 4, length);
	DataBuf ret;
	ret.length = length;
	ret.buffer = data;
	return ret;
}


// -------------------------
//         PUBLIC
// -------------------------
void InitMMap(char *fileName)
{
	int fd;
	struct stat sb;
	size_t length;

	fd = open(fileName, O_RDWR);
	if (fd == -1)
		handle_error("open");

	if (fstat(fd, &sb) == -1)      // Pour obtenir la taille du fichier
		handle_error("fstat");
	length = sb.st_size;

	_mmap = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (_mmap == MAP_FAILED)
		handle_error("mmap");

	close(fd);
}

bool GetStopFlag()
{
	return _getFlag(STOP_FLAG);
}

u32 GetThrottle()
{
	return _getInt(THROTTLE_VALUE);
}

u64 GetImageNo()
{
	return _getLong(IMAGE_NO);
}

DataBuf GetImage()
{
	
	return _getData(CAMERA_IMAGE);
}

// -----------------
//       TESTS
// -----------------
int main(int argc, int *argv[])
{
	InitMMap("/var/tmp/soatcarmmf.tmp");
	
	bool stopflag = GetStopFlag();
	printf("StopFlag is: %d\n", stopflag);
	
	int throttle = GetThrottle();
	printf("Speed is: %d\n", throttle);

	u64 imageNo = GetImageNo();
	printf("Image no is: %llu\n", imageNo);
	
	DataBuf imageData = GetImage();
	printf("Image size is: %d\n", imageData.length);
	for(int i = 0; i < imageData.length; i++)
	{
		printf("%02x", imageData.buffer[i]);
	}
	printf("\n");
	free(imageData.buffer);

}
