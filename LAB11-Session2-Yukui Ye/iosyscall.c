/Yukui Ye  lab11-project3-milestone3-session2/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
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
    int pid;
    int fd1[2],fd2[2],fd3[2],fd4[2],fd5[2];

    pipe(fd1); //children write back to parent
    pipe(fd2); // parent send input_pid to children
    pipe(fd3); // parent send choice to children
    pipe(fd4); //return choice from children
    pipe(fd5); //when choice=1,and we cannot find the process id,return to parent two different printout

    char input_pid[1024],data[1024];
    char choose[1024];          
    char buf[1025];
    char choice[1024];

    pid=fork();

//parent process
    if(pid !=0 && pid!=-1)
    {    
        for(;;)
        {  
            int ret;
            fd_set rd_set;
            int maxfd = 0>fd1[0] ? 0:fd1[0];
            FD_ZERO(&rd_set);
            FD_SET(0,&rd_set);
            FD_SET(fd1[0],&rd_set);
            ret=select(maxfd+1,&rd_set,NULL,NULL,NULL);   
           
            if(FD_ISSET(0,&rd_set))
            {
                printf("\nChoose from the following command:\n");
                printf("1:Print in parent:\t2:Print in children\t3:Print in file\n");
                scanf("%s",&choose);
                write(fd3[1],choose,1024);

                printf("Please input the process ID\n");
                scanf("%s",&input_pid);
                write(fd2[1],input_pid,strlen(input_pid));
             }
             if(FD_ISSET(fd1[0],&rd_set))
             {
                 read(fd4[0],choice,1024);
                 if(choice[0]=='1')
                 {
                    char ynid[2];
                    read(fd5[0],ynid,1024);
                    if(ynid[0]=='y')
                    {
                        char retstore[6][100];
                        read(fd1[0],retstore,600);
                        printf("%s",retstore[0]);
                        printf("%s",retstore[1]);
                        printf("%s",retstore[2]);
                        printf("%s",retstore[3]);
                        printf("%s",retstore[4]);
                        printf("%s",retstore[5]);
                    }
                    if(ynid[0]=='n')
                    {
                        char msgret[1024];
                        read(fd1[0],msgret,1024);
                        printf("%s",msgret);
                    }
                 }
                 if(choice[0]=='2')
                 {
                     read(fd1[0],data,1024);
                     printf("\n%s\n",data);                              
                 }
              }
        }
    }

//syscallvariable
    char *endptr;
    int syscall_num;
    struct module_stat stat;
    stat.version = sizeof(stat);
    modstat( modfind("sc_procinfor"),&stat);
    syscall_num = stat.data.intval;
    int processpid;
    int a=0;
    struct proc_message *msg;
    msg=malloc(sizeof(struct proc_message));
     
//children process
    if(pid==0)
    {
        for(;;)
        {
            read(fd3[0],choice,1024);               
            if(choice[0]=='1')
            {
                read(fd2[0],buf,1024); //read input pid from pipe2 to buf
                processpid=atoi(buf);  //for doing syscall
                a=syscall(syscall_num,processpid,msg); 
                if(a==0)
                {
                    char store[6][100];    //store infor from syscall
                    sprintf(store[0],"Number of open file inside process's file descriptor table:%d\n",msg->nfiles);
                    sprintf(store[1],"Number of voluntary context swithes:%ld\n",msg->nvcsw);
                    sprintf(store[2],"Number of involuntary context swithces:%ld\n",msg->nivcsw);
                    sprintf(store[3],"Effective UID of the process:%d\n",msg->euid);
                    sprintf(store[4],"Saved UID of the process:%d\n",msg->suid);
                    sprintf(store[5],"Real UID of the process:%d\n",msg->ruid);
                    write(fd1[1],store,600); //write back to parent
                    char *retchoice="1";
                    write(fd4[1],retchoice,1024);
                    char *ynid="y";
                    write(fd5[1],ynid,1024);                    
                }
                else
                {
                    char *retchoice="1";
                    write(fd4[1],retchoice,1024);
                    char *ynid="n";
                    write(fd5[1],ynid,1024);
                    char *msgret=" Process cannot find.\n";
                    write(fd1[1],msgret,strlen(msgret));
                }
            }
           if(choice[0]=='2')
            {
                read(fd2[0],buf,1024); //read input pid to buf
                processpid=atoi(buf);
                a=syscall(syscall_num,processpid,msg); 
                if(a==0)
                {
                    printf("Number of open file inside process's file descriptor table:%d\n",msg->nfiles);
                    printf("Number of voluntary context swithes:%ld\n",msg->nvcsw);
                    printf("Number of involuntary context swithces:%ld\n",msg->nivcsw);
                    printf("Effective UID of the process:%d\n",msg->euid);
                    printf("Saved UID of the process:%d\n",msg->suid);
                    printf("Real UID of the process:%d\n",msg->ruid);
                    char *msgret="Infor print in children.\n";
                    write(fd1[1],msgret,1024);
                    char *retchoice="2";
                    write(fd4[1],retchoice,1024);
                }
                else
                {
                    char *msgtp="Infor print in Children && the Process cannot find.\n";
                    write(fd1[1],msgtp,1024);
                    char *retchoice="2";
                    write(fd4[1],retchoice,1024);
                }

            }
            if(choice[0]=='3')
            {
             /*   
                int fd;
                char name[]="molly.txt";
                int flag = O_CREAT|O_WRONLY;
                if( fd = open(name,flag) >0 )
                {
                    dup2(fd,1);
                }
              */

                processpid=atoi(buf);  //for doing syscall
                a=syscall(syscall_num,processpid,msg); 
                printf("Print into a file named mollysmile_ye, GO TO CHECK IT");

                if(a==0)
                {
                    char store[6][100];    //store infor from syscall
                    sprintf(store[0],"Number of open file inside process's file descriptor table:%d\n",msg->nfiles);
                    sprintf(store[1],"Number of voluntary context swithes:%ld\n",msg->nvcsw);
                    sprintf(store[2],"Number of involuntary context swithces:%ld\n",msg->nivcsw);
                    sprintf(store[3],"Effective UID of the process:%d\n",msg->euid);
                    sprintf(store[4],"Saved UID of the process:%d\n",msg->suid);
                    sprintf(store[5],"Real UID of the process:%d\n",msg->ruid);
                    store[0][99]='\n';
                    store[1][99]='\n';
                    store[2][99]='\n';
                    store[3][99]='\n';
                    store[4][99]='\n';
                    store[5][99]='\n';
       
                    FILE *fl = fopen("./mollysmile_ye","wb");
                    fputs(store[0],fl);
                    fputs(store[1],fl);
                    fputs(store[2],fl);
                    fputs(store[3],fl);
                    fputs(store[4],fl);
                    fputs(store[5],fl);
                    fclose(fl);
                }
                else
                {
                    char *msgret="Infor Print in file\n Process cannot find.\n";
                    FILE *fl=fopen("./mollysmile_ye","wb");
                    fputs(msgret,fl);
                    fclose(fl);
                }                
            }
        }
    }
}















