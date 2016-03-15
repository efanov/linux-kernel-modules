// для макросов
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/tty.h>
#include <linux/rcupdate.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/dcache.h>
#include <linux/slab.h> // для kmalloc
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/stat.h>
// PID процесса
#include <asm/current.h> 
#include <linux/sched.h>

#define BUFSIZE 100

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MEPhI");

static int pid = 0;
module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "идентификатор процесса");

static int __init vfs_init(void) {
    pr_info("=== модуль загружен, процесс: \"%s\", PID=%i\n", current->comm, current->pid);

    struct task_struct *p, *task;
    struct fdtable *fdt;

    char *buf = (char *) kmalloc(GFP_KERNEL, BUFSIZE * sizeof(char));

    unsigned char comm[sizeof(task->comm)];

    for_each_process(p){
        if (p->pid == pid) {
            task = p;
            break;
        }
    }

    if (task != NULL) {
        get_task_comm(comm, task);
        pr_info("=== процесс: %s, PID=%d\n", comm, task->pid);
        pr_info("=== Таблица открытых файлов процесса:");
        pr_info("=== files->count.counter: %d ", task->files->count.counter);
        pr_info("=== files->next_fd: %d ", task->files->next_fd);

        fdt = files_fdtable(task->files);

	pr_info("=== fdt->max_fds: %d ", fdt->max_fds);
	pr_info("=== Дескрипторы открытых файлов процесса:");
	int i = 0;
        char *path = NULL;
	while (fdt->fd[i]){
		path = d_path(&fdt->fd[i]->f_path, buf, BUFSIZE * sizeof( char));
		pr_info("=== \tfdt->fd[%d]: %s\n", i, path);
		i++;
	}
	struct fs_struct *fs = task->fs;
	pr_info("=== Корневой каталог: %s\n", task->fs->root.dentry->d_name.name);
	pr_info("=== Рабочий каталог: %s\n", fs->pwd.dentry->d_name.name);
	path = d_path(&fs->pwd, buf, BUFSIZE * sizeof( char) );
	pr_info("=== \tРабочий каталог: %s\n", path);
    } else {
	pr_info("=== процесс с PID=%d не найден\n", pid);
    }
    return -1;
    return 0;
}

static void __exit vfs_exit( void) {
    pr_info("=== модуль выгружен\n");
}

module_init(vfs_init);
module_exit(vfs_exit);

