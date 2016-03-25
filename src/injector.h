#include <stdio.h>

#ifndef INJECTOR_H_
#define INJECTOR_H_
#define LIBC_PATH "/system/lib/libc.so"
#define LINKER_PATH "/system/bin/linker"

long CallMmap(pid_t pid, size_t length);
long CallDlopen(pid_t pid, const char* library_path);
long CallDlsym(pid_t pid, long so_handle, const char* symbol);
long CallDlclose(pid_t pid, long so_handle);
long InjectLibrary(pid_t pid, const char* library_path);
#endif
