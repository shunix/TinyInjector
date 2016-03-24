#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include "ptrace.h"

int PtraceAttach(pid_t pid) {
  if (pid == -1) {
    return -1;
  }
  if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) < 0) {
    perror(NULL);
    return -1;
  }
  waitpid(pid, NULL, WUNTRACED);
  return 0;
}

int PtraceDetach(pid_t pid) {
  if (pid == -1) {
    return -1;
  }
  if (ptrace(PTRACE_DETACH, pid, NULL, NULL) < 0) {
    perror(NULL);
    return -1;
  }
  return 0;
}

void PtraceWrite(pid_t pid, uint8_t* addr, uint8_t* data, size_t size) {
  const size_t WORD_SIZE = sizeof(long);
  int mod = size % WORD_SIZE;
  int loop_count = size / WORD_SIZE;
  uint8_t* tmp_addr = addr;
  uint8_t* tmp_data = data;
  for(int i = 0; i < loop_count; ++i) {
    ptrace(PTRACE_POKEDATA, pid, tmp_addr, *((long*)tmp_data));
    tmp_addr += WORD_SIZE;
    tmp_data += WORD_SIZE;
  }
  if (mod > 0) {
    long val = ptrace(PTRACE_PEEKDATA, pid, tmp_addr, NULL);
    uint8_t* p = (uint8_t*) &val;
    for(int i = 0; i < mod; ++i) {
      *p = *(tmp_data);
      p++;
      tmp_data++;
    }
    ptrace(PTRACE_POKEDATA, pid, tmp_addr, val);
  }
}

long CallRemoteFunction(pid_t pid, long function_addr, long* args, size_t argc) {
  struct pt_regs regs;
  // backup the original regs
  struct pt_regs backup_regs;
  ptrace(PTRACE_GETREGS, pid, NULL, &regs);
  memcpy(&backup_regs, &regs, sizeof(struct pt_regs));
  // put the first 4 args to r0-r3
  for(int i = 0; i < argc && i < 4; ++i) {
    regs.uregs[i] = args[i];
  }
  // push the remainder to stack
  if (argc > 4) {
    regs.ARM_sp -= (argc - 4) * sizeof(long);
    long* data = args + 4;
    PtraceWrite(pid, (uint8_t*)regs.ARM_sp, (uint8_t*)data, (argc - 4) * sizeof(long));
  }
  // set return addr to 0, so we could catch SIGSEGV
  regs.ARM_lr = 0;
  regs.ARM_pc = function_addr;
  if (regs.ARM_pc & 1) {
    // thumb
    regs.ARM_pc &= (~1u);
    regs.ARM_cpsr |= CPSR_T_MASK;
  } else {
    // arm
    regs.ARM_cpsr &= ~CPSR_T_MASK;
  }
  ptrace(PTRACE_SETREGS, pid, NULL, &regs);
  ptrace(PTRACE_CONT, pid, NULL, NULL);
  waitpid(pid, NULL, WUNTRACED);
  // to get return value;
  ptrace(PTRACE_GETREGS, pid, NULL, &regs);
  ptrace(PTRACE_SETREGS, pid, NULL, &backup_regs);
  // Fuction return value
  return regs.ARM_r0;
}
