#include <linux/init.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/debugfs.h>
#include <linux/dynamic_debug.h>

// DEBUG
#pragma clang optimize off


#define MAX_DEV 1

#define MODULE_NAME "kage_guest_test"

struct my_tasklet_data {
    int counter;
};

static void my_tasklet_handler(unsigned long data) {
    struct my_tasklet_data *tasklet_data = (struct my_tasklet_data *)data;
    tasklet_data->counter++;
    pr_info( "Tasklet counter: %d\n", tasklet_data->counter);
}

// Static tasklet initialization (example)
struct tasklet_struct my_tasklet;
struct my_tasklet_data my_tasklet_data;

static dev_t dev;
static struct cdev cdev;
static struct class *cl;
static struct device *dev_ret;
static struct dentry *my_debugfs_dir;

static int kageguest_open(struct inode *i, struct file *f)
{
	pr_info("kageguest_open called\n");
	return 0;
}

static const struct file_operations kageguest_fops = {
	.owner = THIS_MODULE,
	.open = kageguest_open,
};

static void noinline function_call(int i, int j, int k)
{
	pr_info("%s: %d%d%d\n", __func__, i, j, k);
}


static int __init kage_guest_test_init(void)
{
	pr_info("printk test. Start of %s. Num test: %d%d%d%d%d%d%d%d\n", __func__, 42,
		1, 2, 3, 4, 5, 6, 7);

	function_call(1, 2, 3);

	tasklet_init(&my_tasklet, my_tasklet_handler, (unsigned long)&my_tasklet_data);
	// Schedule the tasklet to run
	tasklet_schedule(&my_tasklet);

	pr_info("kmalloc test started\n");
	void * mem = kmalloc(4096, GFP_KERNEL);
	if (!mem) {
		pr_err(MODULE_NAME " kmalloc failed\n");
		return -ENOMEM;
	}
	pr_info("kmalloc test succeeded\n");

	int ret;

	ret = alloc_chrdev_region(&dev, 0, 1, "kage_guest_test");
	if (ret < 0) {
		pr_err("alloc_chrdev_region failed\n");
		return ret;
	}

	if (IS_ERR(cl = class_create("kage_guest_test"))) {
		pr_err("class_create failed\n");
		unregister_chrdev_region(dev, 1);
		return PTR_ERR(cl);
	}

	if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "kage_guest_test"))) {
		pr_err("device_create failed\n");
		class_destroy(cl);
		unregister_chrdev_region(dev, 1);
		return PTR_ERR(cl);
	}

	cdev_init(&cdev, &kageguest_fops);
	ret = cdev_add(&cdev, dev, 1);
	if (ret < 0) {
		pr_err("cdev_add failed\n");
		device_destroy(cl, dev);
		class_destroy(cl);
		unregister_chrdev_region(dev, 1);
		return ret;
	}
        pr_info("cdev_add succeeded\n");

	my_debugfs_dir = debugfs_create_dir("kage_guest_test", NULL);
	if (IS_ERR_OR_NULL(my_debugfs_dir)) {
		pr_err("debugfs_create_dir failed\n");
		my_debugfs_dir = NULL;
	} else {
		pr_info("my_debugfs_dir succeeded\n");
		debugfs_create_file("kage_guest_test_file", 0644, my_debugfs_dir, NULL, &kageguest_fops);
	}

	_dev_err(dev_ret, "test error message %d\n", 42);
	_dev_info(dev_ret, "test info message\n");


	void *devm_mem = devm_kmalloc(dev_ret, 4096, GFP_KERNEL);
	if (!devm_mem) {
		pr_err(MODULE_NAME " devm_kmalloc failed\n");
	}

	dev_printk_emit(6, dev_ret, "test dev_printk_emit message\n");
	_dev_warn(dev_ret, "test warning message\n");

	pr_debug("test dynamic_pr_debug message\n");

	struct wakeup_source *ws = wakeup_source_register(NULL, "kage_test_ws");
	if (ws)
		wakeup_source_unregister(ws);

	__warn_printk("test warn printk message\n");

	return 0;
}

static void __exit kage_guest_test_exit(void)
{
	tasklet_kill(&my_tasklet);
	if (my_debugfs_dir)
		debugfs_remove_recursive(my_debugfs_dir);
	cdev_del(&cdev);
	device_destroy(cl, dev);
	class_destroy(cl);
	unregister_chrdev_region(dev, 1);
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nic Watson");


module_init(kage_guest_test_init);
module_exit(kage_guest_test_exit)
