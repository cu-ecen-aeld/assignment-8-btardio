#define CIRCULAR_INCREMENT(number, limit) ((number + 1) % limit)

// TODO: this is what becomes the circular buffer
/*
 * Representation of aesd quantum sets.
 */
struct aesd_qset {
	void **data;
	struct aesd_qset *next;
};





#ifndef SCULL_P_BUFFER
#define SCULL_P_BUFFER 4000
#endif




// my understanding of these is bad but i think its doing what this does:
// https://tldp.org/LDP/lkmpg/2.4/html/c768.htm
//
//
/* This function decides whether to allow an operation 
 * (return zero) or not allow it (return a non-zero 
 * which indicates why it is not allowed).
 *
 * The operation can be one of the following values:
 * 0 - Execute (run the "file" - meaningless in our case)
 * 2 - Write (input to the kernel module)
 * 4 - Read (output from the kernel module)
 *
 * This is the real function that checks file 
 * permissions. The permissions returned by ls -l are 
 * for referece only, and can be overridden here. 
 */
//static int module_permission(struct inode *inode, int op)
//{
//  /* We allow everybody to read from our module, but 
//   * only root (uid 0) may write to it */ 
//  if (op == 4 || (op == 2 && current->euid == 0))
//    return 0; 
//
//  /* If it's anything else, access is denied */
//  return -EACCES;
//}
//
//
//
//
//


#define SCULL_IOCSQUANTUM _IOW(SCULL_IOC_MAGIC,  1, int)
#define SCULL_IOCSQSET    _IOW(SCULL_IOC_MAGIC,  2, int)
#define SCULL_IOCTQUANTUM _IO(SCULL_IOC_MAGIC,   3)
#define SCULL_IOCTQSET    _IO(SCULL_IOC_MAGIC,   4)
#define SCULL_IOCGQUANTUM _IOR(SCULL_IOC_MAGIC,  5, int)
#define SCULL_IOCGQSET    _IOR(SCULL_IOC_MAGIC,  6, int)
#define SCULL_IOCQQUANTUM _IO(SCULL_IOC_MAGIC,   7)
#define SCULL_IOCQQSET    _IO(SCULL_IOC_MAGIC,   8)
#define SCULL_IOCXQUANTUM _IOWR(SCULL_IOC_MAGIC, 9, int)
#define SCULL_IOCXQSET    _IOWR(SCULL_IOC_MAGIC,10, int)
#define SCULL_IOCHQUANTUM _IO(SCULL_IOC_MAGIC,  11)
#define SCULL_IOCHQSET    _IO(SCULL_IOC_MAGIC,  12)





// this structure stays the same
//



struct aesd_dev {
	struct aesd_qset *data;  /* Pointer to first quantum set */
	int quantum;              /* the current quantum size */
	int qset;                 /* the current array size */
	unsigned long size;       /* amount of data stored here */
	unsigned int access_key;  /* used by aesduid and aesdpriv */
	struct mutex lock;     /* mutual exclusion semaphore     */
	struct cdev cdev;	  /* Char device structure		*/
	char* newlineb;
	int s_newlineb;
};













/*
 * Prototypes for shared functions
 */

int     aesd_p_init(dev_t dev);
void    aesd_p_cleanup(void);
int     aesd_access_init(dev_t dev);
void    aesd_access_cleanup(void);
void	aesd_class_cleanup(void);

int     aesd_trim(struct aesd_dev *dev);

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
                   loff_t *f_pos);
ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *f_pos);
loff_t  aesd_llseek(struct file *filp, loff_t off, int whence);
long     aesd_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);









#ifndef SCULL_MAJOR
#define SCULL_MAJOR 0   /* dynamic major by default */
#endif

#ifndef SCULL_NR_DEVS
#define SCULL_NR_DEVS 1    /* aesd0 through aesd3 */
#endif

//#ifndef SCULL_P_NR_DEVS
//#define SCULL_P_NR_DEVS 4  /* aesdpipe0 through aesdpipe3 */
//#endif





#define SCULL_IOCRESET    _IO(SCULL_IOC_MAGIC, 0)


#ifndef SCULL_QUANTUM
#define SCULL_QUANTUM 4000
#endif

#ifndef SCULL_QSET
#define SCULL_QSET    1000
#endif


#define SCULL_P_IOCTSIZE _IO(SCULL_IOC_MAGIC,   13)
#define SCULL_P_IOCQSIZE _IO(SCULL_IOC_MAGIC,   14)

#define SCULL_IOC_MAXNR 14

#define SCULL_IOC_MAGIC  'k'





// required by access.c

extern int aesd_quantum;
extern int aesd_qset;







// debugging - not a thing setup yet
//
//



#undef PDEBUG             /* undef it, just in case */
#ifdef SCULL_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "scull: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif
