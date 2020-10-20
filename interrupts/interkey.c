/* Focal Point Academy RHKI */
/* Lab8: Interrupt Handler Lab */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/interrupt.h>
#include <linux/io.h>

#define DRIVER_AUTHOR "Focal Point"
#define DRIVER_DESC   "Lab8"

MODULE_LICENSE("GPL");           // Get rid of taint message by declaring code as GPL.

/*  Or with defines, like this: */
MODULE_AUTHOR(DRIVER_AUTHOR);    // Who wrote this module?
MODULE_DESCRIPTION(DRIVER_DESC); // What does this module do?

int init(void);
void cleanup(void);

static int devId = 5;

/* service keyboard interrupts handler */
irqreturn_t irq_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	static unsigned char scancode;

	/* read keyboard */
	//printk("Entering interrupt handler routine\n");
	scancode = inb( 0x60 );
	printk("Got scancode %x\n", scancode);
	if((scancode == 0x01) || (scancode ==0x81)) printk("interkey: ESC pressed\n");

	return IRQ_HANDLED;
}

/* register the irq handler */
static int keybrd_int_register(void)
{
	int result;
	/* request irq 1 for keyboard using request_irq */
	result = request_irq(1, irq_handler, IRQF_SHARED, "i8042", &devId);
	/* check for success/failure */
	if(result) printk("interkey: failed to get shared interrupt for keyboard irq 1\n");
	//printk("interkey: end of register\n");
	return result;
}

/* remove the handler */
static void keybrd_int_unregister(void)
{
	/* free irq handler using free_irq */	
	free_irq(1, &devId);
}

int init(void)
{
	printk(KERN_INFO "init_module() called\n");

	printk("interkey: registering keyboard interrupt handler\n");
	keybrd_int_register();

	return 0;
}

void cleanup(void)
{
	printk("interkey: unregistering keyboard interrupt handler\n");
	keybrd_int_unregister();
	printk(KERN_ALERT "Unloading interkey ...\n");
}

module_init(init);
module_exit(cleanup);

