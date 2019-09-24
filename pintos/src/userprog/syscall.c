#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include <string.h>

static void syscall_handler (struct intr_frame *);
static void valid_up (const void *);

static int
halt (void *esp)
{
  power_off();
}

int
exit (void *esp)
{
  int status = 0;
  if (esp != NULL){
    status = *((int *)esp);
    esp += sizeof (int);
  }
  else {
    status = -1;
  }
  
  char *name = thread_current ()->name, *save;
  name = strtok_r (name, " ", &save);
  
  printf ("%s: exit(%d)\n", name, status);
  thread_exit ();
}

static int
exec (void *esp)
{
  return 0;
}

static int
wait (void *esp)
{
  return 0;
}

static int
create (void *esp)
{
  return 0;
}

static int
remove (void *esp)
{
  return 0;
}

static int
open (void *esp)
{
  return 0;
}

static int
filesize (void *esp)
{
  return 0;
}

static int
read (void *esp)
{
  return 0;
}

static int
write (void *esp)
{
  int fd = *((int *)esp);
  esp += sizeof (int);

  valid_up (esp);
  const void *buffer = *((void **)esp);
  valid_up (buffer);
  esp += sizeof (void *);

  valid_up (esp);
  unsigned size = *((unsigned *)esp);
  esp += sizeof (unsigned);
  
  if (fd == STDOUT_FILENO)
  {
    int i;
    for (i = 0; i<size; i++)
    {
      putchar (*((char *) buffer + i));
    }
    return i;
  }
  return 0;
}

static int
seek (void *esp)
{
  return 0;
}

static int
tell (void *esp)
{
  return 0;
}

static int
close (void *esp)
{
  return 0;
}

static int (*syscalls []) (void *) =
  {
    halt,
    exit,
    exec,
    wait,
    create,
    remove,
    open,
    filesize,
    read,
    write,
    seek,
    tell,
    close
  };

const int num_calls = sizeof (syscalls) / sizeof (syscalls[0]);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall"); 
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  void *esp = f->esp;

  valid_up (esp);
  int syscall_num = *((int *) esp);
  esp += sizeof(int);

  valid_up (esp);
  if (syscall_num >= 0 && syscall_num < num_calls)
  {
    int (*function) (void *) = syscalls[syscall_num];
    int ret = function (esp);
    f->eax = ret;
  }
  else
  {
    printf ("\nError, invalid syscall number.");
    thread_exit ();
  }
}

static void
valid_up (const void *ptr)
{
  uint32_t *pd = thread_current ()->pagedir;
  if ( ptr == NULL || !is_user_vaddr (ptr) || pagedir_get_page (pd, ptr) == NULL)
  {
    exit (NULL);
  }
}
