#if defined(CONFIG_SHIELD_MODULE_NRF9160_TEST_WIDGET) || defined(CONFIG_SHIELD_MODULE_NRF9160_TEST_FIXTURE)

#include <init.h>
#include <zephyr.h>
#include <drivers/gpio.h>

#define NAND_CS_GPIO_PIN 28

static int configure_gpio_idle(const struct device *dev)
{
	ARG_UNUSED(dev);

	const struct device *gpio_nand_cs = device_get_binding(DT_LABEL(DT_NODELABEL(gpio0)));

	gpio_pin_configure(gpio_nand_cs, NAND_CS_GPIO_PIN, GPIO_OUTPUT_HIGH);

	return 0;
}

SYS_INIT(configure_gpio_idle, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);

#endif