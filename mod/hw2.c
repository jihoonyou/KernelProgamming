#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/random.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/moduleparam.h>
#include <asm/pgtable.h>
#define PROC_NAME "hw2"
#define STUDENT_NAME "Jihon You"
#define STUDENT_ID "2018123123"

int period = 0;
module_param(period, int, 0);
#define placeholder 0
/**
* This function is called at the beginning of a sequence.
* ie, when:
* − the /proc file is read (first time)
* − after the function stop (end of sequence)
*
*/
static void *my_seq_start(struct seq_file *s, loff_t *pos)
{
static unsigned long counter = 0;
	/* beginning a new sequence ? */
	if ( *pos == 0 )
	{
	/* yes => return a non null value to begin the sequence */
	return &counter;
	}
	else
	{
	/* no => it's the end of the sequence, return end to stop reading */
	*pos = 0;
	return NULL;
	}
}
/**
* This function is called after the beginning of a sequence.
* It's called untill the return is NULL (this ends the sequence).
*
*/
static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	unsigned long *tmp_v = (unsigned long *)v;
	(*tmp_v)++;
	(*pos)++;
	return NULL;
}
/**
* This function is called at the end of a sequence
*
*/
static void my_seq_stop(struct seq_file *s, void *v)
{
/* nothing to do, we use a static value in start() */
}

static void print_bar(struct seq_file *s) {
	seq_printf(s, "***********************************************************************\n");
}

//check process
static int get_total_process(struct task_struct *task) {
	int counter = 0;	
	for_each_process(task) {
		//task->mm == null means kernel
		if(task->mm != NULL){
			counter++;
		}
	}
	return counter;
}




static struct task_struct *check;
static struct task_struct *picked;

static void func(unsigned long data);
//declare tasklet
DECLARE_TASKLET(name, func, placeholder);
//tasklet to find process
 static void func(unsigned long data){
	//Current is a global variable of type struct task_struct
	check = current;
	int totalProcess = get_total_process(check);
		
	int counter = 0;
	int random = 0;
	//get random number in bound of total process
	random = get_random_int();
	
	random = random%totalProcess;
	
	for_each_process(check) {
		//task->mm == null means kernel
		if(check->mm != NULL) {
			counter++;
			if(counter == random) {
				picked = check; 
			}
		}
	}

}
struct timer_list Jtime;
void tf(unsigned long data)
{
	//to schedule a tasklet to run soon
	tasklet_schedule(&name);
	Jtime.expires = get_jiffies_64()+period;
	add_timer(&Jtime);
}
//bitwise operation to get bit in corresponding bit (pgd)
char* bit_checker(int bit, unsigned long padr) {
	char* result = "";
switch(bit) {
		case 7:
			if(padr & 128) {
				result = "4MB";
			}
			else {
				result = "4KB";
			}
		break;
		case 5:
			if(padr & 32) {
				result = "1";
			}
			else {
				result = "0";
			}
		break;
		case 4:
			if(padr & 16) {
				result = "true";
			}
			else {
				result = "false";
			}
		break;
		case 3:
			if(padr & 8) {
				result = "write-through";
			}
			else {
				result = "write-back";
			}
		break;		
		case 2:
			if(padr & 4) {
				result = "user";
			}
			else {
				result = "supervisor";
			}
		break;
		case 1:
			if(padr & 2) {
				result = "read/write";
			}
			else {
				result = "read-only";
			}
		break;
		case 0:
			if(padr & 1) {
				result = "1";
			}
			else {
				result = "0";
			}
		break;
		default:
			break;			
	}


	return result;
} 
//bitwise operation to get bit in corresponding bit (pte)
char* bit_checker2(int bit, unsigned long padr) {
	char* result = "";
switch(bit) {
		case 6:
			if(padr & 64) {
				result = "1";
			}
			else {
				result = "0";
			}
		break;
		case 5:
			if(padr & 32) {
				result = "1";
			}
			else {
				result = "0";
			}
		break;
		case 4:
			if(padr & 16) {
				result = "true";
			}
			else {
				result = "false";
			}
		break;
		case 3:
			if(padr & 8) {
				result = "write-through";
			}
			else {
				result = "write-back";
			}
		break;		
		case 2:
			if(padr & 4) {
				result = "user";
			}
			else {
				result = "supervisor";
			}
		break;
		case 1:
			if(padr & 2) {
				result = "read/write";
			}
			else {
				result = "read-only";
			}
		break;
		case 0:
			if(padr & 1) {
				result = "1";
			}
			else {
				result = "0";
			}
		break;
		default:
			break;			
	}

	return result;
} 
/**
* This function is called for each "step" of a sequence
*
*/
static int my_seq_show(struct seq_file *s, void *v)
{	

	print_bar(s);
	seq_printf(s, "Student ID: %s	Name: %s\n", STUDENT_ID, STUDENT_NAME);
	seq_printf(s, "Virtual Memory Address Information\n");
	seq_printf(s, "Process (%15s:%lu)\n", picked->comm, picked->pid);
	
	seq_printf(s, "Last update time %llu ms\n", get_jiffies_64());
	print_bar(s);

	// print info about each area
	seq_printf(s, "0x%08lx - 0x%08lx : Code Area, %lu page(s)\n", picked->mm->start_code, picked->mm->end_code, (picked->mm->end_code - picked->mm->start_code)/PAGE_SIZE);
	seq_printf(s, "0x%08lx - 0x%08lx : Data Area, %lu page(s)\n", picked->mm->start_data, picked->mm->end_data, (picked->mm->end_data - picked->mm->start_data)/PAGE_SIZE);
	seq_printf(s, "0x%08lx - 0x%08lx : BSS Area, %lu page(s)\n", picked->mm->end_data, picked->mm->start_brk, (picked->mm->start_brk - picked->mm->end_data)/PAGE_SIZE);
	seq_printf(s, "0x%08lx - 0x%08lx : Heap Area, %lu page(s)\n", picked->mm->start_brk, picked->mm->brk, (picked->mm->brk - picked->mm->start_brk)/PAGE_SIZE);

	//where are you guys..
	//seq_printf(s, "0x%08lx - 0x%08lx : Shared Libraries Area, %lu page(s)\n", 0x7fc1f8225000, 0x7fc1fbadb000, picked->mm->shared_vm);
	seq_printf(s, "0x%08lx - 0x%08lx : Shared Libraries Area, %lu page(s)\n", picked->mm->mmap->vm_start, picked->mm->mmap->vm_end, (picked->mm->mmap->vm_end - picked->mm->mmap->vm_start)/PAGE_SIZE);
	seq_printf(s, "0x%08lx - 0x%08lx : Stack Area, %lu page(s)\n", picked->mm->start_stack, end_of_stack(picked), (*end_of_stack(picked)-picked->mm->start_stack)/PAGE_SIZE);

	// 1 level paging (PGD Info)
	print_bar(s);
	seq_printf(s, "1 Level Paging: Page Directory Entry Information \n");
	print_bar(s);
	//declare pgd table base address
	pgd_t *j_pgd = picked->mm->pgd;
	unsigned long linearAddr = picked->mm->start_code;
	seq_printf(s, "PGD     Base Address            : 0x%08lx\n", j_pgd);
	//pgd_offset macro to calculate
	pgd_t *pgdAddr = pgd_offset(picked->mm, linearAddr);
	seq_printf(s, "code    PGD Address             : 0x%08lx\n", pgdAddr);
	unsigned long pgdV = pgdAddr->pgd;
	seq_printf(s, "        PGD Value               : 0x%08lx\n", pgdV);
	seq_printf(s, "        +PFN Address            : 0x%08lx\n", pgdV/PAGE_SIZE);

	seq_printf(s, "        +Page Size              : %s\n", bit_checker(7, (unsigned long)pgdAddr));
	seq_printf(s, "        +Accessed Bit           : %s\n", bit_checker(5, (unsigned long)pgdAddr));
	seq_printf(s, "        +Cache Disable Bit      : %s\n", bit_checker(4, (unsigned long)pgdAddr));
	seq_printf(s, "        +Page Write-Through     : %s\n", bit_checker(3, (unsigned long)pgdAddr));
	seq_printf(s, "        +User/Supervisor Bit    : %s\n", bit_checker(2, (unsigned long)pgdAddr));
	seq_printf(s, "        +Read/Write Bit         : %s\n", bit_checker(1, (unsigned long)pgdAddr));
	seq_printf(s, "        +Page Present Bit       : %s\n", bit_checker(0, (unsigned long)pgdAddr));

	// 2 level paging (PUD Info)
	print_bar(s);
	seq_printf(s, "2 Level Paging: Page Upper Directory Entry Information \n");
	print_bar(s);
	//declare pud table base address
	pud_t *j_pud = pud_offset(pgdAddr, linearAddr);
	seq_printf(s, "code    PUD Address             : 0x%08lx\n", j_pud);
	unsigned long pudV = j_pud->pud;
	seq_printf(s, "        PUD Value               : 0x%08lx\n", pudV);
	seq_printf(s, "        +PFN Address            : 0x%08lx\n", pudV/PAGE_SIZE);
	
	// 3 level paging (PMD Info)
	print_bar(s);
	seq_printf(s, "3 Level Paging: Page Middle Directory Entry Information \n");
	print_bar(s);
	//declare pmd table base address
	pmd_t *j_pmd = pmd_offset(j_pud, linearAddr);
	seq_printf(s, "code    PMD Address             : 0x%08lx\n", j_pmd);
	unsigned long pmdV = j_pmd->pmd;
	seq_printf(s, "        PMD Value               : 0x%08lx\n", pmdV);
	seq_printf(s, "        +PFN Address            : 0x%08lx\n", pmdV/PAGE_SIZE);

	// 4 level paging (PTE Info)
	print_bar(s);
	seq_printf(s, "4 Level Paging: Page Table Entry Information \n");
	print_bar(s);
	//delcare pte table base address
	pte_t *j_pte = pte_offset_kernel(j_pmd, linearAddr);

	seq_printf(s, "code    PTE Address             : 0x%08lx\n", j_pte);
	unsigned long pteV = j_pte->pte;
	seq_printf(s, "        PTE Value               : 0x%08lx\n", pteV);
	seq_printf(s, "        +Page Base Address      : 0x%08lx\n", pteV/PAGE_SIZE);	

	seq_printf(s, "        +Dirty Bit              : %s\n", bit_checker2(6,(unsigned long)j_pte));
	seq_printf(s, "        +Accessed Bit           : %s\n", bit_checker2(5,(unsigned long)j_pte));
	seq_printf(s, "        +Cache Disable Bit      : %s\n", bit_checker2(4,(unsigned long)j_pte));
	seq_printf(s, "        +Page Write-Through     : %s\n", bit_checker2(3,(unsigned long)j_pte));
	seq_printf(s, "        +User/Supervisor        : %s\n", bit_checker2(2,(unsigned long)j_pte));
	seq_printf(s, "        +Read/Write Bit         : %s\n", bit_checker2(1,(unsigned long)j_pte));
	seq_printf(s, "        +Page Present Bit       : %s\n", bit_checker2(0,(unsigned long)j_pte));

	unsigned long phys = pteV & PAGE_MASK;
	unsigned long phys_off = linearAddr & ~PAGE_MASK;
	print_bar(s);
	seq_printf(s, "Start of Physical Address       : 0x%08lx\n", phys | phys_off);
	print_bar(s);
	seq_printf(s, "Start of Virtual Address       : 0x%08lx\n", phys_to_virt(phys | phys_off));
	print_bar(s);

return 0;
}

/**
* This structure gather "function" to manage the sequence
*
*/
static struct seq_operations my_seq_ops = {
	.start = my_seq_start,
	.next = my_seq_next,
	.stop = my_seq_stop,
	.show = my_seq_show
};
/**
* This function is called when the /proc file is open.
*
*/
static int my_open(struct inode *inode, struct file *file)
{
	
	return seq_open(file, &my_seq_ops);
};
/**
* This structure gather "function" that manage the /proc file
*
*/
static struct file_operations my_file_ops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release
};



static int __init hw2_init(void) {


	init_timer(&Jtime);
	Jtime.function = tf;
	Jtime.expires = get_jiffies_64();
	Jtime.data = placeholder;
	add_timer(&Jtime);

	struct proc_dir_entry *entry;
	entry = proc_create(PROC_NAME, 0, NULL, &my_file_ops);
	
	printk(KERN_INFO "TESTING\n");
	return 0;
}



static void __exit hw2_exit(void) {
	remove_proc_entry(PROC_NAME, NULL);
	del_timer(&Jtime);
	printk(KERN_INFO "TESTING ENDS\n");
}

module_init(hw2_init);
module_exit(hw2_exit);

