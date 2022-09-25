#include <zephyr.h>
#include <drivers/sensor.h>
#include <stdio.h>

BUILD_ASSERT((DT_HAS_COMPAT_STATUS_OKAY(memsic_mc34x9) == 1), "unable to find ocu3_can_bus in device tree");

void main(void)
{
    int err = -1;
    /** @NOTE: Only included to allow the SYS_INIT() LOG msgs do not collide with the printk's here */
    k_sleep(K_SECONDS(5));

    printk("MC34X9 Sample!\n");

	const struct device *dev = DEVICE_DT_GET_ANY(memsic_mc34x9);
	if (!device_is_ready(dev)) {
		printk("Device %s is not ready\n", dev->name);
		return;
	}

    while (1) {
        static int i = 0;
        k_sleep(K_MSEC(100));

        err = sensor_sample_fetch(dev);
        if (err) {
            printk("Failed to fetch sensor sample: %d\n", err);
            return;
        }

        struct sensor_value accel[3];
        err = sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel);
        if (err) {
            printk("Failed to get sensor channel: %d\n", err);
            return;
        }

        printf("\t%d - %llu - AX=%10.2f AY=%10.2f AZ=%10.2f (m/s^2)\n",
            ++i,
            k_uptime_get(),
            sensor_value_to_double(&accel[0]),
            sensor_value_to_double(&accel[1]),
            sensor_value_to_double(&accel[2]));
    }
}
