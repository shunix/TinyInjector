#include <stdio.h>

#ifndef INJECTOR_H_
#define INJECTOR_H_

#if defined(__aarch64__)
#ifndef __BIONIC__
#define LIBC_PATH      "/system/lib64/libc.so"
#define LINKER_PATH    "/system/lib64/libdl.so"
#else
#define LIBC_PATH      "/apex/com.android.runtime/lib64/libc.so"
#define LINKER_PATH    "/apex/com.android.runtime/lib64/libdl.so"
#endif
#define VNDK_LIB_PATH  "/system/lib64/libRS.so"
#else
#ifndef __BIONIC__
#define LIBC_PATH      "/system/lib/libc.so"
#define LINKER_PATH    "/system/lib/libdl.so"
#else
#define LIBC_PATH      "/apex/com.android.runtime/lib/libc.so"
#define LINKER_PATH    "/apex/com.android.runtime/lib/libdl.so"
#endif
#define VNDK_LIB_PATH  "/system/lib/libRS.so"
#endif

long CallMmap(pid_t pid, size_t length);
long CallDlopen(pid_t pid, const char* library_path);
long CallDlsym(pid_t pid, long so_handle, const char* symbol);
long CallDlclose(pid_t pid, long so_handle);
long InjectLibrary(pid_t pid, const char* library_path);
#endif
