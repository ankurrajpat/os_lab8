#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
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
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
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

uint64
sys_va2pa(void)
{
  uint64 va;
  uint64 pa;
  pte_t *pte;
  struct proc *p = myproc();

  // Get the virtual address passed by the user.
  argaddr(0, &va);

  // Reject addresses outside the valid virtual address range.
  if (va >= MAXVA) {
    printk("va2pa: invalid virtual address\n");
    return -1;
  }

  // Walk the page table to find the page-table entry.
  pte = walk(p->pagetable, va, 0);

  // Check whether the address is mapped and user-accessible.
  if (pte == 0 || (*pte & PTE_V) == 0 ||
      (*pte & PTE_U) == 0) {
    printk("va2pa: address is unmapped or not user-accessible\n");
    return -1;
  }

  // Calculate the physical address, including the page offset.
  pa = PTE2PA(*pte) + (va & (PGSIZE - 1));

  // Print virtual and physical addresses in hexadecimal.
  printk("va2pa: VA=%lx PA=%lx\n", va, pa);

  // Print the virtual page number indices and page offset.
  printk("VPN[2]=%lu VPN[1]=%lu VPN[0]=%lu Offset=%lu\n",
         (uint64)PX(2, va),
         (uint64)PX(1, va),
         (uint64)PX(0, va),
         va & (PGSIZE - 1));

  // Print page-table entry flags.
  printk("Flags: P=%d W=%d U=%d\n",
         (*pte & PTE_V) != 0,
         (*pte & PTE_W) != 0,
         (*pte & PTE_U) != 0);

  return pa;
}
