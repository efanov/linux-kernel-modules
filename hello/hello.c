#include <linux/module.h>

MODULE_LICENSE("Укажите вид лицензии");
MODULE_AUTHOR("Впишите имя, фамилию и контакты");
MODULE_DESCRIPTION("Описание модуля");
MODULE_VERSION("Версия модуля");

/*
 * Функция инициализации модуля, которая выполняется, когда модуль загружается в ядро. Ключевое слово __init говорит ядру, что функция будет выполнена только один раз при загрузке модуля.
 */
static int __init hello_init(void)
{
	printk("Здравствуй, мир!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk("Прощай, мир!\n");
}

module_init(hello_init);
module_exit(hello_exit);
