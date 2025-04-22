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
  int n;
  char msg[32];  // buffer for the exit message
  argint(0,&n);
  argstr(1,msg, sizeof(msg)); // part 3 - fetching the msg
  exit(n,msg); // part 3 - passing the msg to exit
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
sys_forkn(void)
{
  int n;
  int pids[16]; // מקצה מקום למערך של 16 תהליכים
  uint64 pids_user;

  // קריאת הפרמטרים מהמשתמש
  argint(0, &n);
  argaddr(1, &pids_user);

  if (n < 1 || n > 16) {
    return -1;
  }

  int ret = forkn(n, pids);


  // הורה: מעתיק את המערך למרחב המשתמש
  if(ret==0) {
    if (copyout(myproc()->pagetable, pids_user, (char *)pids, n * sizeof(int)) < 0) {
      return -1;
    }
  }

  return ret;
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
sys_waitall(void)
{
  uint64 n_addr, statuses_addr;
  int statuses[NPROC]; 
  int n;
  
  argaddr(0, &n_addr);
  argaddr(1, &statuses_addr);
  
  int ret = waitall(&n, statuses);
  
  if(ret == 0) {
    if(copyout(myproc()->pagetable, n_addr, (char*)&n, sizeof(int)) < 0)
      return -1;
    
    if(n > 0) {
      if(copyout(myproc()->pagetable, statuses_addr, (char*)statuses, n * sizeof(int)) < 0)
        return -1;
    }
  }
  
  return ret;
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



