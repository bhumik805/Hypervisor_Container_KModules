#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/sched/task.h>

 
static int pid=1;
struct task_struct *task;        
struct task_struct *task_child;       
struct list_head *list; 
module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "A pid to match");           
 
static int __init iterate_init(void)                  
{
    printk(KERN_INFO "Loading lkm2 module.\n");
    struct task_struct *task;
    struct list_head children;

    for_each_process(task) {
        if(task->pid == pid){
            struct list_head *list;
            struct task_struct *child;

            list_for_each(list, &task->children) {
                child = list_entry(list, struct task_struct, sibling);
                if(child->__state==TASK_RUNNING){
                    printk(KERN_INFO "Child PID: %d\n", child->pid);
                }
            }
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
MODULE_DESCRIPTION("Iterate through child process");
MODULE_AUTHOR("Bhumik");

