#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>

const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(arduino_i2c));

void i2c_scan_devices(void)
{
    int err = -1;
    uint8_t result = 0;

    for (uint8_t i = 1 ; i < 0x7F ; i++) {
        err = i2c_write(i2c_dev, &result, 0, i);
        if (!err) {
            printk("\t- Found: 0x%02X\n", i);
        }
    }
}

void main(void)
{
    if (!i2c_dev) {
        printk("Could not find i2c_dev\n");
        return;
    }

    if (!device_is_ready(i2c_dev)) {
        printk("i2c_dev not ready!\n");
    }

    while (true) {
        printk("I2C Bus Scan sample!\n");
        i2c_scan_devices();
        k_sleep(K_SECONDS(5));
    }

    printk("End of sample\n");
}
