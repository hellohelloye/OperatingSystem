#include <sys/param.h>
#include <sys/cdefs.h>
#include <sys/sysent.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/proc.h>


struct sc_example_args
{
    char *str;
    int val;
};

static int sc_example(struct proc *p, struct sc_example_args *uap)
{
    char kstr[1024+1];
    int err=0;
    int size=0;
    err = copyinstr(uap->str, &kstr, 1024, &size);
    if(err == EFAULT)
        return(err);
    uprintf("The string passed was : %s\n",kstr);
    uprintf("The value passed was : %d\n",uap->val);
    return(0);
}

static struct sysent sc_example_sysent= {
    2,
    sc_example 
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

SYSCALL_MODULE(sc_example,
               &syscall_num,
               &sc_example_sysent,
               module_handler,
               NULL);
