/************************************
	Added by Austin Herring
************************************/
#include <linux/mailbox.h>
#include <linux/list.h>
#include <linux/types.h>
#include <asm/semaphore.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

#define MAILBOX_NUMBER_BUCKETS 128

struct message_struct
{
	pid_t sender;
	pid_t receiver;
	char *message;
	int length;
	struct list_head message_list;
};

struct mailbox
{
	struct list_head message_lists[MAILBOX_NUMBER_BUCKETS];
	struct semaphore lock;
};

static struct mailbox mailbox;

void init_mailbox(void)
{
	int i;
	for (i = 0; i < MAILBOX_NUMBER_BUCKETS; i++)
	{
		INIT_LIST_HEAD(&mailbox.message_lists[i]);
	}
	init_MUTEX(&mailbox.lock);
}

static size_t mailbox_hash(pid_t pid)
{
	return pid % MAILBOX_NUMBER_BUCKETS;
}

asmlinkage long sys_mysend(pid_t pid, char __user *buff, size_t n)
{
	size_t box = mailbox_hash(pid);
	struct message_struct *msg = kmalloc(sizeof *msg, GFP_KERNEL);
	msg->sender = current->pid;
	msg->receiver = pid;

	msg->message = kmalloc(n * sizeof *msg->message, GFP_KERNEL);
	unsigned int uncopied = copy_from_user(msg->message, buff, n);
	msg->length = n - uncopied;

	INIT_LIST_HEAD(&msg->message_list);
	
	down(&mailbox.lock);
	list_add_tail(&mailbox.message_lists[box], &msg->message_list);
	up(&mailbox.lock);

	return msg->length;
}

asmlinkage long sys_myreceive(pid_t pid, char __user *buff, size_t n)
{
	pid_t current_pid = current->pid;
	size_t box = mailbox_hash(current_pid);
	struct message_struct *msg, *next;
	unsigned short success = 0;

	down(&mailbox.lock);
	list_for_each_entry_safe(msg, next, &mailbox.message_lists[box], message_list)
	{
		if (msg->receiver == current_pid && (pid < 0 || msg->sender == pid))
		{
			list_del(&msg->message_list);
			success = 1;
			break;
		}
	}
	up(&mailbox.lock);

	if (success)
	{
		//Copy the minimum of n and the message length back to user space
		size_t minimum = n < msg->length ? n : msg->length;
		unsigned int uncopied = copy_to_user(buff, msg->message, minimum);
		kfree(msg->message);
		kfree(msg);
		return minimum - uncopied;
	}

	return -1;
}
/*Finish additions******************/
