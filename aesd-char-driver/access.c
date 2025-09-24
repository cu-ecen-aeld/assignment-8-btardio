/*
 * access.c -- the files with access control on open
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 * $Id: access.c,v 1.17 2004/09/26 07:29:56 gregkh Exp $
 */

/* FIXME: cloned devices as a use for kobjects? */
 
#include <linux/kernel.h> /* printk() */
#include <linux/module.h>
#include <linux/slab.h>   /* kmalloc() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/fcntl.h>
#include <linux/cdev.h>
#include <linux/tty.h>
#include <asm/atomic.h>
#include <linux/list.h>
#include <linux/cred.h> /* current_uid(), current_euid() */
#include <linux/sched.h>
#include <linux/sched/signal.h>

#include "aesd.h"        /* local definitions */

static dev_t aesd_a_firstdev;  /* Where our range begins */

/*
 * These devices fall back on the main aesd operations. They only
 * differ in the implementation of open() and close()
 */



/************************************************************************
 *
 * The first device is the single-open one,
 *  it has an hw structure and an open count
 */

static struct aesd_dev aesd_s_device;
static atomic_t aesd_s_available = ATOMIC_INIT(1);

static int aesd_s_open(struct inode *inode, struct file *filp)
{
	struct aesd_dev *dev = &aesd_s_device; /* device information */

	if (! atomic_dec_and_test (&aesd_s_available)) {
		atomic_inc(&aesd_s_available);
		return -EBUSY; /* already open */
	}

	/* then, everything else is copied from the bare aesd device */
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY)
		aesd_trim(dev);
	filp->private_data = dev;
	return 0;          /* success */
}

static int aesd_s_release(struct inode *inode, struct file *filp)
{
	atomic_inc(&aesd_s_available); /* release the device */
	return 0;
}


/*
 * The other operations for the single-open device come from the bare device
 */
struct file_operations aesd_sngl_fops = {
	.owner =	THIS_MODULE,
	.llseek =     	aesd_llseek,
	.read =       	aesd_read,
	.write =      	aesd_write,
	.unlocked_ioctl = aesd_ioctl,
	.open =       	aesd_s_open,
	.release =    	aesd_s_release,
};


/************************************************************************
 *
 * Next, the "uid" device. It can be opened multiple times by the
 * same user, but access is denied to other users if the device is open
 */

static struct aesd_dev aesd_u_device;
static int aesd_u_count;	/* initialized to 0 by default */
static uid_t aesd_u_owner;	/* initialized to 0 by default */
static DEFINE_SPINLOCK(aesd_u_lock);

static int aesd_u_open(struct inode *inode, struct file *filp)
{
	struct aesd_dev *dev = &aesd_u_device; /* device information */

	spin_lock(&aesd_u_lock);
	if (aesd_u_count && 
	                (aesd_u_owner != current_uid().val) &&  /* allow user */
	                (aesd_u_owner != current_euid().val) && /* allow whoever did su */
			!capable(CAP_DAC_OVERRIDE)) { /* still allow root */
		spin_unlock(&aesd_u_lock);
		return -EBUSY;   /* -EPERM would confuse the user */
	}

	if (aesd_u_count == 0)
		aesd_u_owner = current_uid().val; /* grab it */

	aesd_u_count++;
	spin_unlock(&aesd_u_lock);

/* then, everything else is copied from the bare aesd device */

	if ((filp->f_flags & O_ACCMODE) == O_WRONLY)
		aesd_trim(dev);
	filp->private_data = dev;
	return 0;          /* success */
}

static int aesd_u_release(struct inode *inode, struct file *filp)
{
	spin_lock(&aesd_u_lock);
	aesd_u_count--; /* nothing else */
	spin_unlock(&aesd_u_lock);
	return 0;
}



/*
 * The other operations for the device come from the bare device
 */
struct file_operations aesd_user_fops = {
	.owner =      THIS_MODULE,
	.llseek =     aesd_llseek,
	.read =       aesd_read,
	.write =      aesd_write,
	.unlocked_ioctl = aesd_ioctl,
	.open =       aesd_u_open,
	.release =    aesd_u_release,
};


/************************************************************************
 *
 * Next, the device with blocking-open based on uid
 */

static struct aesd_dev aesd_w_device;
static int aesd_w_count;	/* initialized to 0 by default */
static uid_t aesd_w_owner;	/* initialized to 0 by default */
static DECLARE_WAIT_QUEUE_HEAD(aesd_w_wait);
static DEFINE_SPINLOCK(aesd_w_lock);

static inline int aesd_w_available(void)
{
	return aesd_w_count == 0 ||
		aesd_w_owner == current_uid().val ||
		aesd_w_owner == current_euid().val ||
		capable(CAP_DAC_OVERRIDE);
}


static int aesd_w_open(struct inode *inode, struct file *filp)
{
	struct aesd_dev *dev = &aesd_w_device; /* device information */

	spin_lock(&aesd_w_lock);
	while (! aesd_w_available()) {
		spin_unlock(&aesd_w_lock);
		if (filp->f_flags & O_NONBLOCK) return -EAGAIN;
		if (wait_event_interruptible (aesd_w_wait, aesd_w_available()))
			return -ERESTARTSYS; /* tell the fs layer to handle it */
		spin_lock(&aesd_w_lock);
	}
	if (aesd_w_count == 0)
		aesd_w_owner = current_uid().val; /* grab it */
	aesd_w_count++;
	spin_unlock(&aesd_w_lock);

	/* then, everything else is copied from the bare aesd device */
	if ((filp->f_flags & O_ACCMODE) == O_WRONLY)
		aesd_trim(dev);
	filp->private_data = dev;
	return 0;          /* success */
}

static int aesd_w_release(struct inode *inode, struct file *filp)
{
	int temp;

	spin_lock(&aesd_w_lock);
	aesd_w_count--;
	temp = aesd_w_count;
	spin_unlock(&aesd_w_lock);

	if (temp == 0)
		wake_up_interruptible_sync(&aesd_w_wait); /* awake other uid's */
	return 0;
}


/*
 * The other operations for the device come from the bare device
 */
struct file_operations aesd_wusr_fops = {
	.owner =      THIS_MODULE,
	.llseek =     aesd_llseek,
	.read =       aesd_read,
	.write =      aesd_write,
	.unlocked_ioctl = aesd_ioctl,
	.open =       aesd_w_open,
	.release =    aesd_w_release,
};

/************************************************************************
 *
 * Finally the `cloned' private device. This is trickier because it
 * involves list management, and dynamic allocation.
 */

/* The clone-specific data structure includes a key field */

struct aesd_listitem {
	struct aesd_dev device;
	dev_t key;
	struct list_head list;
    
};

/* The list of devices, and a lock to protect it */
static LIST_HEAD(aesd_c_list);
static DEFINE_SPINLOCK(aesd_c_lock);

/* A placeholder aesd_dev which really just holds the cdev stuff. */
static struct aesd_dev aesd_c_device;   

/* Look for a device or create one if missing */
static struct aesd_dev *aesd_c_lookfor_device(dev_t key)
{
	struct aesd_listitem *lptr;

	list_for_each_entry(lptr, &aesd_c_list, list) {
		if (lptr->key == key)
			return &(lptr->device);
	}

	/* not found */
	lptr = kmalloc(sizeof(struct aesd_listitem), GFP_KERNEL);
	if (!lptr)
		return NULL;

	/* initialize the device */
	memset(lptr, 0, sizeof(struct aesd_listitem));
	lptr->key = key;
	aesd_trim(&(lptr->device)); /* initialize it */
	mutex_init(&lptr->device.lock);

	/* place it in the list */
	list_add(&lptr->list, &aesd_c_list);

	return &(lptr->device);
}

static int aesd_c_open(struct inode *inode, struct file *filp)
{
	struct aesd_dev *dev;
	dev_t key;
 
	if (!current->signal->tty) { 
		PDEBUG("Process \"%s\" has no ctl tty\n", current->comm);
		return -EINVAL;
	}
	key = tty_devnum(current->signal->tty);

	/* look for a aesdc device in the list */
	spin_lock(&aesd_c_lock);
	dev = aesd_c_lookfor_device(key);
	spin_unlock(&aesd_c_lock);

	if (!dev)
		return -ENOMEM;

	/* then, everything else is copied from the bare aesd device */
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY)
		aesd_trim(dev);
	filp->private_data = dev;
	return 0;          /* success */
}

static int aesd_c_release(struct inode *inode, struct file *filp)
{
	/*
	 * Nothing to do, because the device is persistent.
	 * A `real' cloned device should be freed on last close
	 */
	return 0;
}



/*
 * The other operations for the device come from the bare device
 */
struct file_operations aesd_priv_fops = {
	.owner =    THIS_MODULE,
	.llseek =   aesd_llseek,
	.read =     aesd_read,
	.write =    aesd_write,
	.unlocked_ioctl = aesd_ioctl,
	.open =     aesd_c_open,
	.release =  aesd_c_release,
};

/************************************************************************
 *
 * And the init and cleanup functions come last
 */

static struct aesd_adev_info {
	char *name;
	struct aesd_dev *aesddev;
	struct file_operations *fops;
} aesd_access_devs[] = {
	{ "aesdsingle", &aesd_s_device, &aesd_sngl_fops },
	{ "aesduid", &aesd_u_device, &aesd_user_fops },
	{ "aesdwuid", &aesd_w_device, &aesd_wusr_fops },
	{ "aesdpriv", &aesd_c_device, &aesd_priv_fops }
};
#define SCULL_N_ADEVS 4

/*
 * Set up a single device.
 */
static void aesd_access_setup (dev_t devno, struct aesd_adev_info *devinfo)
{
	struct aesd_dev *dev = devinfo->aesddev;
	int err;

	/* Initialize the device structure */
	dev->quantum = aesd_quantum;
	dev->qset = aesd_qset;
	mutex_init(&dev->lock);

	/* Do the cdev stuff. */
	cdev_init(&dev->cdev, devinfo->fops);
	kobject_set_name(&dev->cdev.kobj, devinfo->name);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add (&dev->cdev, devno, 1);
        /* Fail gracefully if need be */
	if (err) {
		printk(KERN_NOTICE "Error %d adding %s\n", err, devinfo->name);
		kobject_put(&dev->cdev.kobj);
	} else
		printk(KERN_NOTICE "%s registered at %x\n", devinfo->name, devno);
}


int aesd_access_init(dev_t firstdev)
{
	int result, i;

	/* Get our number space */
	result = register_chrdev_region (firstdev, SCULL_N_ADEVS, "aesda");
	if (result < 0) {
		printk(KERN_WARNING "aesda: device number registration failed\n");
		return 0;
	}
	aesd_a_firstdev = firstdev;

	/* Set up each device. */
	for (i = 0; i < SCULL_N_ADEVS; i++)
		aesd_access_setup (firstdev + i, aesd_access_devs + i);
	return SCULL_N_ADEVS;
}

/*
 * This is called by cleanup_module or on failure.
 * It is required to never fail, even if nothing was initialized first
 */
void aesd_access_cleanup(void)
{
	struct aesd_listitem *lptr, *next;
	int i;

	/* Clean up the static devs */
	for (i = 0; i < SCULL_N_ADEVS; i++) {
		struct aesd_dev *dev = aesd_access_devs[i].aesddev;
		cdev_del(&dev->cdev);
		aesd_trim(aesd_access_devs[i].aesddev);
	}

    	/* And all the cloned devices */
	list_for_each_entry_safe(lptr, next, &aesd_c_list, list) {
		list_del(&lptr->list);
		aesd_trim(&(lptr->device));
		kfree(lptr);
	}

	/* Free up our number space */
	unregister_chrdev_region(aesd_a_firstdev, SCULL_N_ADEVS);
	return;
}

