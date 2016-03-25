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
  if (DEBUG) {
    printf("mmap called, function address %lx process %d size %d\n", function_addr, pid, length);
  }
  return CallRemoteFunction(pid, function_addr, params, 6);
}

long CallMunmap(pid_t pid, long addr, size_t length) {
  long function_addr = GetRemoteFuctionAddr(pid, LIBC_PATH, ((long) (void*)munmap));
  long params[2];
  params[0] = addr;
  params[1] = length;
  if (DEBUG) {
    printf("munmap called, function address %lx process %d address %lx size %d\n", function_addr, pid, addr, length);
  }
  return CallRemoteFunction(pid, function_addr, params, 2);
}

long CallDlopen(pid_t pid, const char* library_path) {
  long function_addr = GetRemoteFuctionAddr(pid, LINKER_PATH, ((long) (void*)dlopen));
  long mmap_ret = CallMmap(pid, 0x400);
  PtraceWrite(pid, (uint8_t*)mmap_ret, (uint8_t*)library_path, strlen(library_path) + 1);
  long params[2];
  params[0] = mmap_ret;
  params[1] = RTLD_NOW | RTLD_LOCAL;
  if (DEBUG) {
    printf("dlopen called, function address %lx process %d library path %s\n", function_addr, pid, library_path);
  }
  long ret = CallRemoteFunction(pid, function_addr, params, 2);
  CallMunmap(pid, mmap_ret, 0x400);
  return ret;
}

long CallDlsym(pid_t pid, long so_handle, const char* symbol) {
  long function_addr = GetRemoteFuctionAddr(pid, LINKER_PATH, ((long) (void*)dlsym));
  long mmap_ret = CallMmap(pid, 0x400);
  PtraceWrite(pid, (uint8_t*)mmap_ret, (uint8_t*)symbol, strlen(symbol) + 1);
  long params[2];
  params[0] = so_handle;
  params[1] = mmap_ret;
  if (DEBUG) {
    printf("dlsym called, function address %lx process %d so handle %lx symbol name %s\n", function_addr, pid, so_handle, symbol);
  }
  long ret = CallRemoteFunction(pid, function_addr, params, 2);
  CallMunmap(pid, mmap_ret, 0x400);
  return ret;
}

long CallDlclose(pid_t pid, long so_handle) {
  long function_addr = GetRemoteFuctionAddr(pid, LINKER_PATH, ((long) (void*)dlclose));
  long params[1];
  params[0] = so_handle;
  if (DEBUG) {
    printf("dlclose called, function address %lx process %d so handle %lx\n", function_addr, pid, so_handle);
  }
  return CallRemoteFunction(pid, function_addr, params, 1);
}

long InjectLibrary(pid_t pid, const char* library_path) {
  if (DEBUG) {
    printf("Injection started...\n");
  }
  PtraceAttach(pid);
  long so_handle = CallDlopen(pid, library_path);
  if (DEBUG) {
    printf("Injection ended...\n");
  }
  PtraceDetach(pid);
  return so_handle;
}
