#include <stdio.h>

#ifndef INJECTOR_H_
#define INJECTOR_H_

#if defined(__aarch64__)
#define LIBC_PATH      "/system/lib64/libc.so"
#define LINKER_PATH    "/system/lib64/libdl.so"
#else
#define LIBC_PATH      "/system/lib/libc.so"
#define LINKER_PATH    "/system/lib/libdl.so"
#endif

long CallMmap(pid_t pid, size_t length);
long CallDlopen(pid_t pid, const char* library_path);
long CallDlsym(pid_t pid, long so_handle, const char* symbol);
long CallDlclose(pid_t pid, long so_handle);
long InjectLibrary(pid_t pid, const char* library_path);
#endif
