#include <stdio.h>

#ifndef INJECTOR_H_
#define INJECTOR_H_

#if defined(__aarch64__)
#define LIBC_PATH_OLD		"/system/lib64/libc.so"
#define LIBC_PATH_NEW		"/apex/com.android.runtime/lib64/bionic/libc.so"
#define LINKER_PATH_OLD		"/system/lib64/libdl.so"
#define LINKER_PATH_NEW		"/apex/com.android.runtime/lib64/bionic/libdl.so"
#define VNDK_LIB_PATH		"/system/lib64/libRS.so"
#else
#define LIBC_PATH_OLD		"/system/lib/libc.so"
#define LIBC_PATH_NEW		"/apex/com.android.runtime/lib/bionic/libc.so"
#define LINKER_PATH_OLD		"/system/lib/libdl.so"
#define LINKER_PATH_NEW		"/apex/com.android.runtime/lib/bionic/libdl.so"
#define VNDK_LIB_PATH		"/system/lib/libRS.so"
#endif

long CallMmap(pid_t pid, size_t length);
long CallDlopen(pid_t pid, const char* library_path);
long CallDlsym(pid_t pid, long so_handle, const char* symbol);
long CallDlclose(pid_t pid, long so_handle);
long InjectLibrary(pid_t pid, const char* library_path);
#endif
