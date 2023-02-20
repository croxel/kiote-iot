#if defined(CONFIG_SHIELD_MODULE_NRF9160_TEST_WIDGET) || defined(CONFIG_SHIELD_MODULE_NRF9160_TEST_FIXTURE)

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define NAND_CS_GPIO_PIN 28

static int configure_gpio_idle(const struct device *dev)
{
	ARG_UNUSED(dev);

	const struct gpio_dt_spec nand_cs = GPIO_DT_SPEC_GET(DT_NODELABEL(nand_cs), gpios);

	gpio_pin_configure_dt(&nand_cs, GPIO_OUTPUT);
	gpio_pin_set_dt(&nand_cs, 0);

	return 0;
}

SYS_INIT(configure_gpio_idle, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);

#endif