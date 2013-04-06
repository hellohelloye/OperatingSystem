#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/module.h>

int main(int argc, char **argv)
{
    char *endptr;
    int syscall_num;
    struct module_stat stat;
    
    stat.version = sizeof(stat);
    modstat( modfind("sc_example"), &stat);
    syscall_num = stat.data.intval;

    return  syscall(syscall_num,"molly sc_example1",5);
   
}
