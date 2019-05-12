#include <sys/mman.h>	// For mmap(), PROT_READ, PROT_WRITE, MAP_SHARED, MAP_FAILED: memory map file
#include <sys/stat.h>	// For fstat(): size of file
#include <fcntl.h>		// For O_RDWR: open file flag
#include <stdio.h>		// For perror, printf
#include <stdlib.h>		// For malloc, exit, EXIT_FAILURE
#include <unistd.h>		// For close: close file
#include <string.h>		// For memcpy: copy memory
#include "SoatcarState.hpp"

// -------------------------
//         PRIVATE
// -------------------------
bool SoatcarState::_getFlag(int offset)
{
	if(_mmap[offset] == 0) return false;
	else return true;
}

void SoatcarState::_setFlag(int offset, bool flag)
{
	if(flag) _mmap[offset] = 1;
	else _mmap[offset] = 0;
}


u32 SoatcarState::_getInt(int offset)
{
	return ((u8)_mmap[offset + 3] << 24) | ((u8)_mmap[offset + 2] << 16) | ((u8)_mmap[offset + 1] << 8) | (u8)_mmap[offset + 0];
}

void SoatcarState::_setInt(int offset, u32 val)
{
	_mmap[offset + 3] = val >> 24 & 0xFF;
	_mmap[offset + 2] = val >> 16 & 0xFF;
	_mmap[offset + 1] = val >> 8 & 0xFF;
	_mmap[offset + 0] = val >> 0 & 0xFF;
}


u64 SoatcarState::_getLong(int offset)
{
	return ((u64)_mmap[offset + 7] << 56) | ((u64)_mmap[offset + 6] << 48) | ((u64)_mmap[offset + 5] << 40) | ((u64)_mmap[offset + 4] << 32)
		| ((u64)_mmap[offset + 3] << 24) | ((u64)_mmap[offset + 2] << 16) | ((u64)_mmap[offset + 1] << 8) | ((u64)_mmap[offset + 0] << 0);
}

void SoatcarState::_setLong(int offset, u64 val)
{
	_mmap[offset + 7] = val >> 56 & 0xFF;
	_mmap[offset + 6] = val >> 48 & 0xFF;
	_mmap[offset + 5] = val >> 40 & 0xFF;
	_mmap[offset + 4] = val >> 32 & 0xFF;
	_mmap[offset + 3] = val >> 24 & 0xFF;
	_mmap[offset + 2] = val >> 16 & 0xFF;
	_mmap[offset + 1] = val >> 8 & 0xFF;
	_mmap[offset + 0] = val >> 0 & 0xFF;
}


char* SoatcarState::_getString(int offset)
{
	u32 length = _getInt(offset);
	char* data = (char*)malloc(length + 1);
	memcpy(data, _mmap + offset + 4, length);
	data[length] = '\0';
	return data;
}

void SoatcarState::_setString(int offset, char* str)
{
	u32 length = strlen(str);
	_setInt(offset, length);
	memcpy(_mmap + offset + 4, str, length);
}


DataBuf SoatcarState::_getData(int offset)
{
	u32 length = _getInt(offset);
	u8* data = (u8*)malloc(length);
	memcpy(data, _mmap + offset + 4, length);
	DataBuf ret;
	ret.length = length;
	ret.buffer = data;
	return ret;
}

void SoatcarState::_setData(int offset, DataBuf data)
{
	_setInt(offset, data.length);
	memcpy(_mmap + offset + 4, data.buffer, data.length);
}



// -------------------------
//         PUBLIC
// -------------------------
SoatcarState::SoatcarState(const char *fileName)
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

	_mmap = (u8*)mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (_mmap == MAP_FAILED)
		handle_error("mmap");

	close(fd);
}

bool SoatcarState::GetStopFlag()
{
	return _getFlag(CONST_STOP_FLAG);
}

void SoatcarState::SetStopFlag(bool flag)
{
	_setFlag(CONST_STOP_FLAG, flag);
}

u32 SoatcarState::GetThrottle()
{
	return _getInt(CONST_THROTTLE_VALUE);
}

void SoatcarState::SetThrottle(u32 throttle)
{
	_setInt(CONST_THROTTLE_VALUE, throttle);
}

u64 SoatcarState::GetImageNo()
{
	return _getLong(CONST_IMAGE_NO);
}

void SoatcarState::SetImageNo(u64 imageNo)
{
	_setLong(CONST_IMAGE_NO, imageNo);
}

DataBuf SoatcarState::GetImage()
{	
	return _getData(CONST_CAMERA_IMAGE);
}

void SoatcarState::SetImage(DataBuf data)
{
	_setData(CONST_CAMERA_IMAGE, data);
}


