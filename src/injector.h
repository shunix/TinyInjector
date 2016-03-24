#include <stdio.h>

#ifndef INJECTOR_H_
#define INJECTOR_H_

long CallMmap(pid_t pid, size_t length, long function_addr);
long CallDlopen(pid_t pid, const char* library_path, long mmap_ret, long function_addr);
long CallDlsym(pid_t pid, long so_handle, const char* symbol, long mmap_ret, long function_addr);
long CallDlclose(pid_t pid, long so_handle, long function_addr);
long InjectLibrary(pid_t pid, const char* library_path)
#endif
