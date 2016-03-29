#include <stdbool.h>
#include <stdio.h>

#ifndef UTILS_H_
#define UTILS_H_
pid_t GetPid(const char* process_name); // get pid of specified process
bool IsSelinuxEnabled(); // check the status of SELinux
void DisableSelinux(); // disable SELinux
long GetModuleBaseAddr(pid_t pid, const char* module_name); // get base address of specified module in given process
long GetRemoteFuctionAddr(pid_t remote_pid, const char* module_name, long local_function_addr); // get fuction address in remote process
#endif
