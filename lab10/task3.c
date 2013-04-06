#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/module.h>

struct proc_message
{
    int nfiles;
    long  nvcsw;
    long  nivcsw;
    uid_t  euid;
    uid_t  suid;
    uid_t  ruid; 
};

int main(int argc, char **argv)
{
   char *endptr;
   int syscall_num;
   struct module_stat stat;
   stat.version = sizeof(stat);
   modstat( modfind("sc_procinfor"),&stat);
   syscall_num = stat.data.intval;

   int processpid;
   int a=0;
   processpid= atoi(argv[1]);
   struct proc_message *msg;
   msg=malloc(sizeof(struct proc_message));

   a=syscall(syscall_num,processpid,msg);
   if(a==0)
   {
       printf("Number of open file inside process's file descriptor table:%d\n",msg->nfiles);
       printf("Number of voluntary context swithes:%ld\n",msg->nvcsw);
       printf("Number of involuntary context swithces:%ld\n",msg->nivcsw);
       printf("Effective UID of the process:%d\n",msg->euid);
       printf("Saved UID of the process:%d\n",msg->suid);
       printf("Real UID of the process:%d\n",msg->ruid);
   }
   else
       printf("Process PID cannot Find\n");
   return 0;
}
