#include <sys/syscall.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/module.h>

int main()
{
   char *endptr;
   int syscall_num;
   struct module_stat stat;

   stat.version = sizeof(stat);
   modstat(modfind("sc_refcount"),&stat);
   syscall_num = stat.data.intval;
   return syscall(syscall_num);
}
