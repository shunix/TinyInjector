#include <linux/elf.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include "config.h"
#include "ptrace.h"

#if defined(__aarch64__)
  #define pt_regs  user_pt_regs
  #define uregs    regs
  #define ARM_r0   regs[0]
  #define ARM_lr   regs[30]
  #define ARM_sp   sp
  #define ARM_pc   pc
  #define ARM_cpsr pstate
#endif

static void PtraceGetRegs(pid_t pid, struct pt_regs *regs);
static void PtraceSetRegs(pid_t pid, struct pt_regs *regs);
static void PtraceCont(pid_t pid);

int PtraceAttach(pid_t pid) {
  if (pid == -1) {
    return -1;
  }
  if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) < 0) {
    perror(NULL);
    return -1;
  }
  waitpid(pid, NULL, WUNTRACED);
  if (DEBUG) {
    printf("Attached to process %d\n", pid);
  }
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
  if (DEBUG) {
    printf("Detached from process %d\n", pid);
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
  if (DEBUG) {
    printf("Write %zu bytes to %p process %d\n", size, addr, pid);
  }
}

static void PtraceGetRegs(pid_t pid, struct pt_regs *regs) {
  #if defined(__aarch64__)
    struct {
      void* ufb;
      size_t len;
    } regsvec = { regs, sizeof(struct pt_regs) };

    ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &regsvec);
  #else
    ptrace(PTRACE_GETREGS, pid, NULL, regs);
  #endif
}

static void PtraceSetRegs(pid_t pid, struct pt_regs *regs) {
  #if defined(__aarch64__)
    struct {
      void* ufb;
      size_t len;
    } regsvec = { regs, sizeof(struct pt_regs) };

    ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &regsvec);
  #else
    ptrace(PTRACE_SETREGS, pid, NULL, regs);
  #endif
}

static void PtraceCont(pid_t pid) {
  ptrace(PTRACE_CONT, pid, NULL, NULL);
}

long CallRemoteFunction(pid_t pid, long function_addr, long* args, size_t argc) {
  #if defined(__aarch64__)
    #define REGS_ARG_NUM    6
  #else
    #define REGS_ARG_NUM    4
  #endif

  struct pt_regs regs;
  // backup the original regs
  struct pt_regs backup_regs;

  PtraceGetRegs(pid, &regs);
  memcpy(&backup_regs, &regs, sizeof(struct pt_regs));
  // put the first 4 args to r0-r3
  for(int i = 0; i < argc && i < REGS_ARG_NUM; ++i) {
    regs.uregs[i] = args[i];
  }
  // push the remainder to stack
  if (argc > REGS_ARG_NUM) {
    regs.ARM_sp -= (argc - REGS_ARG_NUM) * sizeof(long);
    long* data = args + REGS_ARG_NUM;
    PtraceWrite(pid, (uint8_t*)regs.ARM_sp, (uint8_t*)data, (argc - REGS_ARG_NUM) * sizeof(long));
  }
  // set return addr to 0, so we could catch SIGSEGV
  regs.ARM_lr = 0;
  regs.ARM_pc = function_addr;

  #if !defined(__aarch64__)
    if (regs.ARM_pc & 1) {
      // thumb
      regs.ARM_pc &= (~1u);
      regs.ARM_cpsr |= CPSR_T_MASK;
    } else {
      // arm
      regs.ARM_cpsr &= ~CPSR_T_MASK;
    }
  #endif

  PtraceSetRegs(pid, &regs);
  PtraceCont(pid);

  waitpid(pid, NULL, WUNTRACED);

  // to get return value;
  PtraceGetRegs(pid, &regs);
  PtraceSetRegs(pid, &backup_regs);
  // Fuction return value
  if (DEBUG) {
    printf("Call remote function %lx with %zu arguments, return value is %llx\n",
            function_addr, argc, (long long)regs.ARM_r0);
  }
  return regs.ARM_r0;
}
