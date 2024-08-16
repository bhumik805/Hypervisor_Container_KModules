#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/pid.h>
#include <asm/io.h>
#include <asm/page.h>

static int pid = -1;
module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "Process ID");

static unsigned long virt_addr = 0;
module_param(virt_addr, ulong, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(virt_addr, "Virtual Address");

static int __init lkm3_init(void)
{
    printk(KERN_INFO "Loading lkm3 module.\n");
    struct task_struct *task;
    struct mm_struct *mm;
    unsigned long phys_addr;

    task = pid_task(find_get_pid(pid), PIDTYPE_PID);
    if (!task) {
        printk(KERN_INFO "Process with PID %d not found.\n", pid);
        return -EINVAL;
    }

    mm = get_task_mm(task);
    if (!mm) {
        printk(KERN_INFO "Failed to get mm_struct for PID %d.\n", pid);
        return -EINVAL;
    }
     phys_addr = virt_to_phys(virt_addr);
        printk(KERN_INFO "PID: %d, Virtual Address: 0x%lx, Physical Address: 0x%lx\n", pid, virt_addr, phys_addr);
   


    return 0;
}

static void __exit lkm3_exit(void)
{
    printk(KERN_INFO "Exiting lkm3 module.\n");
}

module_init(lkm3_init);
module_exit(lkm3_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("VA to PA");
MODULE_AUTHOR("Bhumik");

