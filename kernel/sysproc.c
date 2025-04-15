#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{ // part 3 
  int status;
  char msg[32];  // buffer for the exit message

  // Get first syscall argument: exit status
  argint(0, &status);

  // Get second syscall argument: string from user space into msg buffer
  if (argstr(1, msg, sizeof(msg)) < 0) {
    // If fetching the string fails, store a fallback message
    safestrcpy(msg, "no message", sizeof(msg));
  }

  // Call kernel-level exit with the status and message
  exit(status, msg);

  return 0; // not reached
}


uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  uint64 msg_addr;
  argaddr(0, &p);
  argaddr(1, &msg_addr);
  return wait(p, msg_addr);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}



// return memory bytes allocated for all running processes
// since start.
uint64
sys_memsize(void)
{
  struct proc *p = myproc();
  return p->sz; // memory size
}
