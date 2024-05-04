// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2015, 2017-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
 */
#include <linux/atomic.h>
#include <linux/debugfs.h>
#include <linux/device.h>
#include "esoc.h"

/* esoc debug engine parent dir */
static struct dentry *esoc_dbg;

/*
 * cmd_mask : Specifies if a command/notifier is masked, and
 * whats the trigger value for mask to take effect.
 * @mask_trigger: trigger value for mask.
 * @mask: boolean to determine if command should be masked.
 */
struct esoc_mask {
	atomic_t mask_trigger;
	bool mask;
};

/*
 * manual_to_esoc_cmd: Converts a user provided command
 * to a corresponding esoc command.
 * @cmd: ESOC command
 * @manual_cmd: user specified command string.
 */
struct manual_to_esoc_cmd {
	unsigned int cmd;
	char manual_cmd[20];
};

/*
 * manual_to_esoc_notify: Converts a user provided notification
 * to corresponding esoc notification for Primary SOC.
 * @notfication: ESOC notification.
 * @manual_notifier: user specified notification string.
 */
struct manual_to_esoc_notify {
	unsigned int notify;
	char manual_notify[20];
};

static const struct manual_to_esoc_cmd cmd_map[] = {
	{
		.cmd = ESOC_PWR_ON,
		.manual_cmd = "PON",
	},
	{
		.cmd = ESOC_PREPARE_DEBUG,
		.manual_cmd = "ENTER_DLOAD",
	},
	{	.cmd = ESOC_PWR_OFF,
		.manual_cmd = "POFF",
	},
	{
		.cmd = ESOC_FORCE_PWR_OFF,
		.manual_cmd = "FORCE_POFF",
	},
};

static struct esoc_mask cmd_mask[] = {
	[ESOC_PWR_ON] = {
		.mask = false,
		.mask_trigger = ATOMIC_INIT(1),
	},
	[ESOC_PREPARE_DEBUG] = {
		.mask = false,
		.mask_trigger = ATOMIC_INIT(0),
	},
	[ESOC_PWR_OFF] = {
		.mask = false,
		.mask_trigger = ATOMIC_INIT(0),
	},
	[ESOC_FORCE_PWR_OFF] = {
		.mask = false,
		.mask_trigger = ATOMIC_INIT(0),
	},
};

static const struct manual_to_esoc_notify notify_map[] = {
	{
		.notify = ESOC_PRIMARY_REBOOT,
		.manual_notify = "REBOOT",
	},
	{
		.notify = ESOC_PRIMARY_CRASH,
		.manual_notify = "PANIC",
	},
};

static struct esoc_mask notify_mask[] = {
	[ESOC_PRIMARY_REBOOT] = {
		.mask = false,
		.mask_trigger = ATOMIC_INIT(0),
	},
	[ESOC_PRIMARY_CRASH] = {
		.mask = false,
		.mask_trigger = ATOMIC_INIT(0),
	},
};

bool dbg_check_cmd_mask(unsigned int cmd)
{
	pr_debug("command to mask %d\n", cmd);
	if (cmd_mask[cmd].mask)
		return atomic_add_negative(-1, &cmd_mask[cmd].mask_trigger);
	else
		return false;
}
EXPORT_SYMBOL(dbg_check_cmd_mask);

bool dbg_check_notify_mask(unsigned int notify)
{
	pr_debug("notifier to mask %d\n", notify);
	if (notify_mask[notify].mask)
		return atomic_add_negative(-1, &notify_mask[notify].mask_trigger);
	else
		return false;
}
EXPORT_SYMBOL(dbg_check_notify_mask);
/*
 * Create driver attributes that let you mask
 * specific commands.
 */
static ssize_t command_mask_write(struct file *filp, const char __user *user_buf, size_t count,
				  loff_t *ppos)
{
	unsigned int cmd, i, ret;
	char buf[20];

	if (count > sizeof(buf))
		return -EINVAL;

	ret = copy_from_user(buf, user_buf, count);
	if (ret)
		return -EFAULT;

	pr_debug("user input command %s\n", buf);
	for (i = 0; i < ARRAY_SIZE(cmd_map); i++) {
		if (!strcmp(cmd_map[i].manual_cmd, buf)) {
			/*
			 * Map manual command string to ESOC command
			 * set mask for ESOC command
			 */
			cmd = cmd_map[i].cmd;
			cmd_mask[cmd].mask = true;
			pr_debug("Setting mask for manual command %s\n", buf);
			break;
		}
	}
	if (i >= ARRAY_SIZE(cmd_map))
		pr_err("invalid command specified\n");
	return count;
}

static const struct file_operations esoc_cmd_mask_fops = {
	.write = command_mask_write,
	.open = simple_open,
	.llseek = generic_file_llseek,
};

static ssize_t notifier_mask_write(struct file *filp, const char __user *user_buf, size_t count,
				  loff_t *ppos)
{
	unsigned int notify, i, ret;
	char buf[20];

	if (count > sizeof(buf))
		return -EINVAL;

	ret = copy_from_user(buf, user_buf, count);
	if (ret)
		return -EFAULT;

	pr_debug("user input notifier %s\n", buf);
	for (i = 0; i < ARRAY_SIZE(notify_map); i++) {
		if (!strcmp(buf, notify_map[i].manual_notify)) {
			/*
			 * Map manual notifier string to primary soc
			 * notifier. Also set mask for the notifier.
			 */
			notify = notify_map[i].notify;
			notify_mask[notify].mask = true;
			pr_debug("Setting mask for manual notification %s\n", buf);
			break;
		}
	}
	if (i >= ARRAY_SIZE(notify_map))
		pr_err("invalid notifier specified\n");
	return count;
}

static const struct file_operations esoc_notifier_mask_fops = {
	.write = notifier_mask_write,
	.open = simple_open,
	.llseek = generic_file_llseek,
};

/* Last recorded request from esoc */
static enum esoc_req last_req;
static DEFINE_SPINLOCK(req_lock);
/*
 * esoc_to_user: Conversion of esoc ids to user visible strings
 * id: esoc request, command, notifier, event id
 * str: string equivalent of the above
 */
struct esoc_to_user {
	unsigned int id;
	char str[20];
};

static struct esoc_to_user in_to_resp[] = {
	{
		.id = ESOC_IMG_XFER_DONE,
		.str = "XFER_DONE",
	},
	{
		.id = ESOC_BOOT_DONE,
		.str = "BOOT_DONE",
	},
	{
		.id = ESOC_BOOT_FAIL,
		.str = "BOOT_FAIL",
	},
	{
		.id = ESOC_IMG_XFER_RETRY,
		.str = "XFER_RETRY",
	},
	{	.id = ESOC_IMG_XFER_FAIL,
		.str = "XFER_FAIL",
	},
	{
		.id = ESOC_UPGRADE_AVAILABLE,
		.str = "UPGRADE",
	},
	{	.id = ESOC_DEBUG_DONE,
		.str = "DEBUG_DONE",
	},
	{
		.id = ESOC_DEBUG_FAIL,
		.str = "DEBUG_FAIL",
	},
};

static struct esoc_to_user req_to_str[] = {
	{
		.id = ESOC_REQ_IMG,
		.str = "REQ_IMG",
	},
	{
		.id = ESOC_REQ_DEBUG,
		.str = "REQ_DEBUG",
	},
	{
		.id = ESOC_REQ_SHUTDOWN,
		.str = "REQ_SHUTDOWN",
	},
};

static ssize_t req_eng_resp_write(struct file *filp, const char __user *user_buf, size_t count,
				  loff_t *ppos)
{
	unsigned int i, ret;
	struct esoc_clink *clink = filp->private_data;
	const struct esoc_clink_ops *const clink_ops = clink->clink_ops;
	char buf[20];

	if (count > sizeof(buf))
		return -EINVAL;

	ret = copy_from_user(buf, user_buf, count);
	if (ret)
		return -EFAULT;

	dev_dbg(&clink->dev, "user input req eng response %s\n", buf);
	for (i = 0; i < ARRAY_SIZE(in_to_resp); i++) {
		size_t len1 = strlen(buf);
		size_t len2 = strlen(in_to_resp[i].str);

		if (len1 == len2 && !strcmp(buf, in_to_resp[i].str)) {
			clink_ops->notify(in_to_resp[i].id, clink);
			break;
		}
	}
	if (i > ARRAY_SIZE(in_to_resp))
		dev_err(&clink->dev, "Invalid resp %s, specified\n", buf);
	return count;
}

static const struct file_operations esoc_req_eng_resp_fops = {
	.write = req_eng_resp_write,
	.open = simple_open,
	.llseek = generic_file_llseek,
};

static ssize_t last_esoc_req_read(struct file *filp, char __user *user_buf, size_t count,
				  loff_t *ppos)
{
	unsigned int i;
	unsigned long flags;

	spin_lock_irqsave(&req_lock, flags);
	for (i = 0; i < ARRAY_SIZE(req_to_str); i++) {
		if (last_req == req_to_str[i].id) {
			count = simple_read_from_buffer(user_buf, count, 0, req_to_str[i].str,
							strlen(req_to_str[i].str));
			break;
		}
	}
	spin_unlock_irqrestore(&req_lock, flags);
	return count;
}

static const struct file_operations esoc_last_esoc_req_fops = {
	.read = last_esoc_req_read,
	.open = simple_open,
	.llseek = generic_file_llseek,
};

static void esoc_handle_req(enum esoc_req req, struct esoc_eng *eng)
{
	unsigned long flags;

	spin_lock_irqsave(&req_lock, flags);
	last_req = req;
	spin_unlock_irqrestore(&req_lock, flags);
}

static void esoc_handle_evt(enum esoc_evt evt, struct esoc_eng *eng)
{
}

static struct esoc_eng dbg_req_eng = {
	.handle_clink_req = esoc_handle_req,
	.handle_clink_evt = esoc_handle_evt,
};

int register_dbg_req_eng(struct esoc_clink *clink)
{
	struct device *dev = &clink->dev;
	struct dentry *file;
	int ret;

	if (clink->dbg_dir)
		return 0;

	clink->dbg_dir = debugfs_create_dir(dev_name(dev), esoc_dbg);
	if (IS_ERR_OR_NULL(clink->dbg_dir)) {
		pr_err("can't create debugfs dir: %s\n", dev_name(dev));
		ret = PTR_ERR(clink->dbg_dir);
		goto dbg_req_err;
	}

	file = debugfs_create_file("req_eng_resp", 0600, clink->dbg_dir, clink,
				   &esoc_req_eng_resp_fops);
	if (IS_ERR_OR_NULL(file)) {
		pr_err("Unable to create req_eng_resp file\n");
		ret = PTR_ERR(file);
		goto dbg_req_err;
	}

	file = debugfs_create_file("last_esoc_req", 0400, clink->dbg_dir, clink,
				   &esoc_last_esoc_req_fops);
	if (IS_ERR_OR_NULL(file)) {
		pr_err("Unable to create last_esoc_req file\n");
		ret = PTR_ERR(file);
		goto dbg_req_err;
	}

	ret = esoc_clink_register_req_eng(clink, &dbg_req_eng);
	if (ret) {
		pr_err("Unable to register req eng\n");
		goto dbg_req_err;
	}
	spin_lock_init(&req_lock);
	return 0;
dbg_req_err:
	debugfs_remove(clink->dbg_dir);
	clink->dbg_dir = NULL;
	return ret;
}

void unregister_dbg_req_eng(struct esoc_clink *clink)
{
	esoc_clink_unregister_req_eng(clink, &dbg_req_eng);
	debugfs_remove(clink->dbg_dir);
}

int __init mdm_dbg_eng_init(void)
{
	int ret;
	struct dentry *file;

	if (!debugfs_initialized())
		return -EINVAL;

	if (esoc_dbg)
		return 0;

	esoc_dbg = debugfs_create_dir(KBUILD_MODNAME, NULL);
	if (IS_ERR_OR_NULL(esoc_dbg)) {
		pr_err("can't create debugfs dir\n");
		ret = PTR_ERR(esoc_dbg);
		goto dbg_eng_fail;
	}

	file = debugfs_create_file("command_mask", 0600, esoc_dbg, NULL, &esoc_cmd_mask_fops);
	if (IS_ERR_OR_NULL(file)) {
		pr_err("Unable to create command mask file\n");
		ret = PTR_ERR(file);
		goto dbg_eng_fail;
	}

	file = debugfs_create_file("notifier_mask", 0600, esoc_dbg, NULL, &esoc_notifier_mask_fops);
	if (IS_ERR_OR_NULL(file)) {
		pr_err("Unable to create notify mask file\n");
		ret = PTR_ERR(file);
		goto dbg_eng_fail;
	}
	return 0;
dbg_eng_fail:
	debugfs_remove(esoc_dbg);
	esoc_dbg = NULL;
	return ret;
}
EXPORT_SYMBOL(mdm_dbg_eng_init);

void __exit mdm_dbg_eng_exit(void)
{
	debugfs_remove(esoc_dbg);
}
EXPORT_SYMBOL(mdm_dbg_eng_exit);

MODULE_LICENSE("GPL v2");
