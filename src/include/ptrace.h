#include <stdio.h>

#ifndef PTRACE_H_
#define PTRACE_H_
#define CPSR_T_MASK (1u << 5)

int PtraceAttach(pid_t pid);
int PtraceDetach(pid_t pid);
void PtraceWrite(pid_t pid, uint8_t* addr, uint8_t* data, size_t size);
long CallRemoteFunction(pid_t pid, long function_addr, long* args, size_t argc); // call function in remote process
long CallRemoteFunctionFromNamespace(pid_t pid, long function_addr, long namespace_addr, long* args, size_t argc);
#endif
