#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/pid.h>
#include <linux/pagemap.h>
#include <linux/mm_types.h>
#include <linux/vmalloc.h>

#define PAGE_SIZE 4096

static int pid = -1;
module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "Process ID");

static int __init access_all_ptes(void)
{
    struct task_struct *task;
    struct mm_struct *mm;
    unsigned long address;
    struct vm_area_struct *vma;
    int flag = 1;
     unsigned long vsize=0, psize=0;
    // Find the task_struct of the specified process ID
    task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task)
    {
        pr_err("Process with PID %d not found\n", pid);
        return 1;
    }

    // Get the memory descriptor of the process
    mm = get_task_mm(task);
    if (!mm)
    {
        pr_err("Failed to get mm_struct for PID %d\n", pid);
        return 1;
    }
    
    VMA_ITERATOR(iter, mm, 0);

    for_each_vma(iter, vma)
    {
        vsize += vma->vm_end - vma->vm_start;
        for (address = vma->vm_start; address < vma->vm_end; address += PAGE_SIZE)
        {
            pgd_t *pgd;
            pud_t *pud;
            pmd_t *pmd;
            pte_t *pte;
            p4d_t *p4d;

            // Get the PGD entry
            pgd = pgd_offset(mm, address);
            if (pgd_none(*pgd) || unlikely(pgd_bad(*pgd)))
            {
                flag = 0;
                continue;
            }
            p4d = p4d_offset(pgd,address);
            if(!p4d_present(*p4d) || unlikely(p4d_bad(*p4d))){
                
                flag = 0;
                continue;
            }

            pud = pud_offset(p4d, address);
            if (pud_none(*pud) || unlikely(pud_bad(*pud)))
            {
                flag = 0;
                continue;
            }

            pmd = pmd_offset(pud, address);
            if (pmd_none(*pmd) || unlikely(pmd_bad(*pmd)))
            {
                flag = 0;
                continue;
            }

            pte = pte_offset_map(pmd, address);
            if (!pte)
            {
                flag = 0;
                continue;
            }
            if(pte_pfn(*pte)!=0){
                psize += PAGE_SIZE;
            }
            
            pte_unmap(pte);
        }
    }
    pr_info("Process %d: vsize = %lu, psize = %lu\n", pid, vsize, psize);

    mmput(mm);
    return 0;
}

static void __exit lkm4_exit(void)
{
    pr_info("Exiting mod module.\n");
}

module_init(access_all_ptes);
module_exit(lkm4_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("VMA and RSS");
MODULE_AUTHOR("Bhumik");

