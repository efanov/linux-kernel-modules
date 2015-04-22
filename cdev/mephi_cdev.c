#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru> + MEPhI" );
MODULE_VERSION( "6.4" );

/*
 * Передать старший номер устройства через параметр модуля major.
 */
static int major = 0;
module_param( major, int, S_IRUGO );

/*
 * Счетчик использования устройства.
 */
static int device_open = 0;

static int dev_open( struct inode *n, struct file *f ) {
// Если файл устройства уже был открыт, возвратить ошибку.
   if( device_open ) return -EBUSY;
// Если файл устройства открывается в первый раз, увеличить счетчик.
   device_open++;
   return 0;
}

static int dev_release( struct inode *n, struct file *f ) {
// Если файл устройства закрывается, уменьшить счетчик.
   device_open--;
   return 0;
}

// Строка, которую выдает драйвер при чтении из файла устройства.
static char *hello_str = "Hello, world!\n";

static ssize_t dev_read( struct file * file, char * buf,
                           size_t count, loff_t *ppos ) {
   int len = strlen( hello_str );
   printk( KERN_INFO "=== read : %ld\n", (long)count );
// зачем?
   if( count < len ) return -EINVAL;
   if( *ppos != 0 ) {
      printk( KERN_INFO "=== read return : 0\n" );  // EOF
      return 0;
   }
   if( copy_to_user( buf, hello_str, len ) ) return -EINVAL;
   *ppos = len;
   printk( KERN_INFO "=== read return : %d\n", len );
   return len;
}

static const struct file_operations dev_fops = {
   .owner = THIS_MODULE,
   .open = dev_open,
   .release = dev_release,
   .read  = dev_read,
};

#define DEVICE_FIRST  0
// Задаём количество младших номеров, которые можно использовать при создании специальных файлов
#define DEVICE_COUNT  3
#define MODNAME "mephi_cdev"

static struct cdev mephi_cdev;

/*
 * Функция загрузки модуля
 */

static int __init dev_init( void ) {
   int ret;
   dev_t dev;
   if( major != 0 ) {
      dev = MKDEV( major, DEVICE_FIRST );
      ret = register_chrdev_region( dev, DEVICE_COUNT, MODNAME );
   }
   else {
      ret = alloc_chrdev_region( &dev, DEVICE_FIRST, DEVICE_COUNT, MODNAME );
      major = MAJOR( dev );  // не забыть зафиксировать!
   }
   if( ret < 0 ) {
      pr_info( "=== Can not register char device region\n" );
      goto err;
   }
   cdev_init( &mephi_cdev, &dev_fops );
   mephi_cdev.owner = THIS_MODULE;
   ret = cdev_add( &mephi_cdev, dev, DEVICE_COUNT );
   if( ret < 0 ) {
      unregister_chrdev_region( MKDEV( major, DEVICE_FIRST ), DEVICE_COUNT );
      pr_info( "=== Can not add char device\n" );
      goto err;
   }
   pr_info( "=========== module installed %d:%d ==============\n",
           MAJOR( dev ), MINOR( dev ) );
err:
   return ret;
}

/*
 * Функция выгрузки модуля
 */

static void __exit dev_exit( void ) {
   cdev_del( &mephi_cdev );
// Освобождаем старшие и младшие номера устройств.
   unregister_chrdev_region( MKDEV( major, DEVICE_FIRST ), DEVICE_COUNT );
	pr_info( "=============== module removed ==================\n" );
}

module_init( dev_init );
module_exit( dev_exit );
