#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/sched.h>
 
 
struct task_struct *task;        
struct task_struct *task_child;       
struct list_head *list;            
int iterate_init(void)                  
{
    printk(KERN_INFO "Loading lkm1 module.\n");
    struct task_struct *task;
    for_each_process(task) {
        if(task->__state==TASK_RUNNING){
            printk("PID: %d\n", task->pid);
        }
    }
    return 0;
}                
     
void cleanup_exit(void)       
{
    printk(KERN_INFO "%s","REMOVING MODULE\n");
 
}                
 
module_init(iterate_init);    
module_exit(cleanup_exit);   
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Iterrate through all processes");
MODULE_AUTHOR("Bhumik");
