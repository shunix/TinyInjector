#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>
#include "config.h"
#include "injector.h"
#include "ptrace.h"
#include "utils.h"

long CallMmap(pid_t pid, size_t length) {
  long function_addr = GetRemoteFuctionAddr(pid, LIBC_PATH, ((long) (void*)mmap));
  long params[6];
  params[0] = 0;
  params[1] = length;
  params[2] = PROT_READ | PROT_WRITE | PROT_EXEC;
  params[3] = MAP_PRIVATE | MAP_ANONYMOUS;
  params[4] = 0;
  params[5] = 0;
  return CallRemoteFunction(pid, function_addr, params, 6);
}

long CallDlopen(pid_t pid, const char* library_path) {
  long function_addr = GetRemoteFuctionAddr(pid, LINKER_PATH, ((long) (void*)dlopen));
  long mmap_ret = CallMmap(pid, 0x400);
  PtraceWrite(pid, (uint8_t*)mmap_ret, (uint8_t*)library_path, strlen(library_path) + 1);
  long params[2];
  params[0] = mmap_ret;
  params[1] = RTLD_NOW | RTLD_LOCAL;
  return CallRemoteFunction(pid, function_addr, params, 2);
}

long CallDlsym(pid_t pid, long so_handle, const char* symbol) {
  long function_addr = GetRemoteFuctionAddr(pid, LINKER_PATH, ((long) (void*)dlsym));
  long mmap_ret = CallMmap(pid, 0x400);
  PtraceWrite(pid, (uint8_t*)mmap_ret, (uint8_t*)symbol, strlen(symbol) + 1);
  long params[2];
  params[0] = so_handle;
  params[1] = mmap_ret;
  return CallRemoteFunction(pid, function_addr, params, 2);
}

long CallDlclose(pid_t pid, long so_handle) {
  long function_addr = GetRemoteFuctionAddr(pid, LINKER_PATH, ((long) (void*)dlclose));
  long params[1];
  params[0] = so_handle;
  return CallRemoteFunction(pid, function_addr, params, 1);
}

long InjectLibrary(pid_t pid, const char* library_path) {
  PtraceAttach(pid);
  CallDlopen(pid, library_path);
}
