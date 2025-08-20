// Cross-platform file handling
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define mkdir(dir, mode) _mkdir(dir)
#define stat _stat
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#define HANDLE int
#define INVALID_HANDLE_VALUE (-1)
#define GENERIC_READ O_RDONLY
#define GENERIC_WRITE (O_WRONLY | O_CREAT | O_TRUNC)
#define FILE_ATTRIBUTE_NORMAL 0

// Correzione delle macro ReadFile e WriteFile
static int cross_ReadFile(int handle, void* buffer, unsigned long size, unsigned long* bytesRead, void* overlapped) {
    ssize_t result = read(handle, buffer, size);
    if (result >= 0) {
        *bytesRead = result;
        return 1;
    }
    return 0;
}

static int cross_WriteFile(int handle, const void* buffer, unsigned long size, unsigned long* bytesWritten, void* overlapped) {
    ssize_t result = write(handle, buffer, size);
    if (result >= 0) {
        *bytesWritten = result;
        return 1;
    }
    return 0;
}

#define ReadFile cross_ReadFile
#define WriteFile cross_WriteFile
#define CloseHandle(handle) close(handle)
#define OPEN_EXISTING 0
#define CREATE_ALWAYS (O_CREAT | O_TRUNC)

static int cross_CreateFile(const char* name, int access, int share, void* sec, int disp, int flags, void* temp) {
    int fd = open(name, access, 0666);
    return fd;
}
#define CreateFile cross_CreateFile
#endif

