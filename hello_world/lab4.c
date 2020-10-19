/* FocalPoint LKI */
/* Lab 4: My First Module */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#define DRIVER_AUTHOR "FocalPoint"
#define DRIVER_DESC   "A sample driver"

MODULE_LICENSE("GPL");				//Get rid of taint message by declaring code as GPL.

/* Or with defines, like this: */
MODULE_AUTHOR(DRIVER_AUTHOR); 		//Who wrote this module?
MODULE_DESCRIPTION(DRIVER_DESC); 	//What does this module do?

int init_hello(void);
void cleanup_hello(void);

int init_hello(void)
{
	printk(KERN_ALERT "Hello from My First Module\n");
    printk(KERN_ALERT "My init function is at 0x%p\n", init_hello);
	return 0;
}

void cleanup_hello(void)
{
	printk(KERN_ALERT "Unloading My First Module\n");
}

module_init(init_hello);
module_exit(cleanup_hello);
