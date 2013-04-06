#include <sys/param.h>
#include <sys/cdefs.h>
#include <sys/sysent.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <vm/vm.h>
#include <vm/vm_object.h>

static int sc_refcount(struct proc *p, void *uap)
{
    vm_object_t object;
    int maxref=0;
    int printref=0;

    TAILQ_FOREACH(object,&vm_object_list,object_list){
        if(object->ref_count > maxref)
           maxref=object->ref_count;
    }
    int count[maxref+1];
    for(int i=0; i<maxref; i++)
       count[i]=0;

    TAILQ_FOREACH(object,&vm_object_list,object_list){
        count[object->ref_count]++;
    }
    for(printref=0; printref<maxref; printref++)
    {
       if(count[printref] != 0)
            uprintf("The number of objects with reference %d:%d\n",
                printref,count[printref]);
    }
    return(0);
}

static struct sysent sc_refcount_sysent= {
    0,
    sc_refcount 
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

SYSCALL_MODULE(sc_refcount,
               &syscall_num,
               &sc_refcount_sysent,
               module_handler,
               NULL);
