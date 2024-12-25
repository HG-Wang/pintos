#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#define max_syscall 20
static void syscall_handler (struct intr_frame *);
static void (*syscalls[max_syscall]) (struct intr_frame *);

void sys_exit(struct intr_frame *f);
void sys_write(struct intr_frame *f);
void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  syscalls[SYS_EXIT] = &sys_exit;
  syscalls[SYS_WRITE] = &sys_write;
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int type = *(int *)f->esp;
  if(type < 0 || type >= max_syscall){
    printf("invalid system call!\n");
    thread_exit();
  }
  if(type == SYS_EXIT || type == SYS_WRITE){
    syscalls[type](f);
  }else{
    printf ("system call!\n");
    thread_exit ();
  }
  
}

void sys_exit(struct intr_frame *f){
  uint32_t *user_ptr = f->esp;
  *user_ptr++;
  thread_current()->st_exit = *user_ptr;
  thread_exit();
}

void sys_write(struct intr_frame *f){
  uint32_t *user_ptr = f->esp;
  *user_ptr++;
  printf("user_ptr:%x, fesp:%x\n", user_ptr, f->esp);
  int tmp = *user_ptr;
  const char *buffer = (const char *)*(user_ptr+1);
  unsigned size = *(user_ptr+2);
  if(tmp == 1){
    putbuf(buffer, size);
    f->eax = size;
  }
}