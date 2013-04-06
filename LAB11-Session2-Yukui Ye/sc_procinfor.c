#include <sys/param.h>
#include <sys/cdefs.h>
#include <sys/sysent.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/ucred.h>
#include <sys/filedesc.h>
#include <sys/resourcevar.h>
#include <sys/filedesc.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/sx.h>

struct sc_procinfor_args
{
    int procid;
    int ptr;
};

struct sc_procinfor
{
    int nfiles;
    long nvcsw;
    long nivcsw;
    uid_t euid;
    uid_t suid;
    uid_t ruid;
};

static int sc_procinfor(struct proc *p, struct sc_procinfor_args *uap)
{
    struct proc *process;
    struct sc_procinfor *msg;
    msg=(struct sc_procinfor *)(uap->ptr);
    int retval=1;
    sx_slock(&allproc_lock);
    FOREACH_PROC_IN_SYSTEM(process)
    {
        PROC_LOCK(process);
        if(process->p_pid==uap->procid)
        {
            msg->nfiles= process->p_fd->fd_nfiles;
            msg->nvcsw = process->p_stats->p_ru.ru_nvcsw;
            msg->nivcsw= process->p_stats->p_ru.ru_nivcsw;
            msg->euid  = process->p_ucred->cr_uid;
            msg->suid  = process->p_ucred->cr_svuid;
            msg->ruid  = process->p_ucred->cr_ruid;
            retval= 0;
            PROC_UNLOCK(process);
            break;
         }
         PROC_UNLOCK(process);
     }
           
     sx_sunlock(&allproc_lock); 
   return retval;
}


static struct sysent sc_procinfor_sysent= {
    2,
    sc_procinfor 
};

static int syscall_num=NO_SYSCALL;

static int module_handler(struct module *m, int what, void *arg)
{
    int error=0;
 switch (what)
 {
 		case MOD_LOAD:
 		uprintf("System call loaded at slot: %d\n",syscall_num);
        break;

		case MOD_UNLOAD:
 		uprintf("System call unloaded from slot: %d\n",syscall_num);
 		break;

		default:
 		error = EINVAL;
 		break;
 }
 return(error);
}

SYSCALL_MODULE(sc_procinfor,
               &syscall_num,
               &sc_procinfor_sysent,
               module_handler,
               NULL);
