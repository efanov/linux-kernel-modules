#include <linux/module.h>
#include <linux/proc_fs.h>
//#include <linux/stat.h>
#include <asm/uaccess.h> // для copy_from_user()

#define NAME_DIR  "mephi_dir"
#define NAME_NODE "mephi"
#define LEN_MSG 160

// описание модуля

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "MEPhI" );

static int  __init mephi_init( void );
static void __exit mephi_exit( void );

module_init( mephi_init );
module_exit( mephi_exit ); 

// буфер для хранения информации 
static char *get_rw_buf( void )
{
   static char buf_msg[ LEN_MSG + 1 ] =
          ".........1.........2.........3.........4.........5\n";
   return buf_msg;
}

/*
* Прототип функции чтения из include/linux/fs.h
* ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
*/
static ssize_t mephi_read( struct file *file, char *buf, size_t count, loff_t *ppos )
{
	char *buf_msg = get_rw_buf();
	int res;

	pr_info( "read: %d bytes (ppos=%lld)", count, *ppos );
	if( *ppos >= strlen( buf_msg ) ) {     // EOF
		*ppos = 0;
		pr_info( "EOF" );
		return 0;
	}
	if( count > strlen( buf_msg ) - *ppos ) 
		count = strlen( buf_msg ) - *ppos;  
	res = copy_to_user( (void*)buf, buf_msg + *ppos, count );
	*ppos += count;
	pr_info( "return %d bytes", count );
	return count;
}

/*
* Прототип функции записи из include/linux/fs.h
* ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
*/
static ssize_t mephi_write( struct file *file, const char *buf, size_t count, loff_t *ppos )
{
	char *buf_msg = get_rw_buf();
	int res, len = count < LEN_MSG ? count : LEN_MSG;
	pr_info( "write: %d bytes", (int) count );
	res = copy_from_user( buf_msg, (void*)buf, len );
	buf_msg[ len ] = '\0';
	pr_info( "put %d bytes", len );
	return len;
}

// заполняем таблицу файловых операций
static const struct file_operations mephi_fops = {
	.owner = THIS_MODULE,
	.read = mephi_read,
	.write = mephi_write
};

static int __init mephi_init( void )
{
	struct proc_dir_entry *mephi_proc_file;

// можно права к файлу задать в символьном виде: S_IFREG | S_IRUGO | S_IWUGO
	mephi_proc_file = proc_create("mephi", 0666, NULL, &mephi_fops);

	if( mephi_proc_file == NULL ) {
		pr_err( "/proc/%s не создан.", NAME_NODE );
		return -ENOENT;
	}
	pr_info( "/proc/%s создан.", NAME_NODE );
	return 0;
}

static void __exit mephi_exit( void )
{
	remove_proc_entry( NAME_NODE, NULL );
	pr_info( "/proc/%s удален.", NAME_NODE );
} 

