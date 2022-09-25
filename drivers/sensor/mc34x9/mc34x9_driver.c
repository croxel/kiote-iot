#define DT_DRV_COMPAT memsic_mc34x9

/**
 * Ported from: https://github.com/mcubemems/mCube_mc34x9_arduino_driver
 */

#include <drivers/sensor.h>
#include <init.h>
#include <drivers/gpio.h>
#include <drivers/i2c.h>
#include <logging/log.h>
#include <mc34x9.h>

LOG_MODULE_REGISTER(MC34X9, CONFIG_SENSOR_LOG_LEVEL);

/** @FIXME: Added to make it compatible with NCS v1.7.1
 * Remove when updating NCS version.
*/
struct i2c_dt_spec {
    const struct device *bus;
    uint16_t addr;
};


struct mc34x9_config {
    struct {
        const char *name;        
        uint16_t addr;
    } bus;
};

struct mc34x9_data {
   struct i2c_dt_spec bus;
   struct {
        int16_t x;
        int16_t y;
        int16_t z;
    } __packed accel;
    struct {
        MC34X9_range_t range;
        MC34X9_sr_t sample_rate;
        bool lpf;
        uint8_t offset[6];
        uint8_t gain[3];
    } settings;
};

static struct mc34x9_data mc34x9_data;

#define MC34X9_CFG_MODE_DEFAULT                 MC34X9_MODE_STANDBY
#define MC34X9_CFG_SAMPLE_RATE_DEFAULT          MC34X9_SR_2000Hz
#define MC34X9_CFG_DECIMATION_DEFAULT           MC34X9_DECIMATION_X200
#define MC34X9_CFG_RANGE_DEFAULT                MC34X9_RANGE_2G

/** @FIXME: Added to make it compatible with NCS v1.7.1
 * Remove when updating NCS version.
*/
static inline int i2c_reg_write_byte_dt(const struct i2c_dt_spec *spec,
					uint8_t reg_addr, uint8_t value)
{
	return i2c_reg_write_byte(spec->bus, spec->addr, reg_addr, value);
}

/** @FIXME: Added to make it compatible with NCS v1.7.1
 * Remove when updating NCS version.
*/
static inline int i2c_burst_write_dt(const struct i2c_dt_spec *spec,
				     uint8_t start_addr,
				     const uint8_t *buf,
				     uint32_t num_bytes)
{
	return i2c_burst_write(spec->bus, spec->addr,
			       start_addr, buf, num_bytes);
}

/** @FIXME: Added to make it compatible with NCS v1.7.1
 * Remove when updating NCS version.
*/
static inline int i2c_burst_read_dt(const struct i2c_dt_spec *spec,
				     uint8_t start_addr,
				     uint8_t *buf,
				     uint32_t num_bytes)
{
	return i2c_burst_read(spec->bus, spec->addr,
			       start_addr, buf, num_bytes);
}


static inline int mc34x9_set_reg(const struct device *dev,
                  uint8_t register_address, uint8_t value)
{
    const struct mc34x9_data *data = dev->data;

    return i2c_reg_write_byte_dt(&data->bus, register_address, value);
}

static inline int mc34x9_set_data(const struct device *dev, uint8_t *value, 
				  uint8_t register_address, uint8_t count)
{
    const struct mc34x9_data *data = dev->data;

    return i2c_burst_write_dt(&data->bus, register_address, value, count);
}

static inline int mc34x9_get_data(const struct device *dev, uint8_t *read_buf,
				  uint8_t register_address, uint8_t count)
{
    const struct mc34x9_data *data = dev->data;

    return i2c_burst_read_dt(&data->bus, register_address, read_buf, count);
}

static int set_mode(const struct device *dev, MC34X9_mode_t mode)
{
    uint8_t value;
    int err = -1;
    
    err = mc34x9_get_data(dev, &value, MC34X9_REG_MODE, 1);
    if (err) {
        return err;
    }

    value &= 0b11110000;
    value |= mode;

    return mc34x9_set_reg(dev, MC34X9_REG_MODE, value);
}

static int enable_lpf(const struct device *dev, bool enable)
{
    uint8_t value;
    int err = -1;
    uint8_t lpf_setting = 0b1101; /* BW Setting 5, Fc = IDR/16 */

    struct mc34x9_data *data = dev->data;
    data->settings.lpf = enable;

    err = set_mode(dev, MC34X9_MODE_STANDBY);
    if (err) {
        return err;
    }

    err = mc34x9_get_data(dev, &value, MC34X9_REG_RANGE_C, 1);
    if (err) {
        return err;
    }

    value &= 0b11110000;

    if (enable) {
        value |= lpf_setting;
    }

    return mc34x9_set_reg(dev, MC34X9_REG_RANGE_C, value);
}

static int set_range_ctl(const struct device *dev, MC34X9_range_t range)
{
    uint8_t value;
    int err = -1;

    struct mc34x9_data *data = dev->data;
    data->settings.range = range;

    err = set_mode(dev, MC34X9_MODE_STANDBY);
    if (err) {
        return err;
    }

    err = mc34x9_get_data(dev, &value, MC34X9_REG_RANGE_C, 1);
    if (err) {
        return err;
    }

    value &= 0b00000111;
    value |= (range << 4) & 0x70;

    return mc34x9_set_reg(dev, MC34X9_REG_RANGE_C, value);
}

static int set_sample_rate(const struct device *dev, MC34X9_sr_t sample_rate)
{
    int err = -1;

    err = set_mode(dev, MC34X9_MODE_STANDBY);
    if (err) {
        return err;
    }

    struct mc34x9_data *data = dev->data;
    data->settings.sample_rate = sample_rate;

    return mc34x9_set_reg(dev, MC34X9_REG_SR, sample_rate);
}

static int set_decimation(const struct device *dev, MC34X9_decimation_t decimation)
{
    uint8_t value;
    int err = -1;

    err = set_mode(dev, MC34X9_MODE_STANDBY);
    if (err) {
        return err;
    }

    err = mc34x9_get_data(dev, &value, MC34X9_REG_FIFO_CTRL_SR2, 1);
    if (err) {
        return err;
    }

    value &= ~(0b1111);
    value |= (decimation & 0b1111);

    return mc34x9_set_reg(dev, MC34X9_REG_FIFO_CTRL_SR2, value);
}

static int mc34x9_attr_set(const struct device *dev,
			    enum sensor_channel chan,
			    enum sensor_attribute attr,
			    const struct sensor_value *val)
{
    return -1;
}

static int mc34x9_sample_fetch(const struct device *dev,
				enum sensor_channel chan)
{
    if (!dev) {
        return -EINVAL;
    }

    struct mc34x9_data *data = dev->data;

    return mc34x9_get_data(dev, (uint8_t *)&data->accel, MC34X9_REG_XOUT_LSB, 6);
}

static int convert_raw_to_g(const struct device *dev, int16_t raw_val, struct sensor_value *val)
{
    if (!dev || !val) {
        return -EINVAL;
    }

    struct mc34x9_data *data = dev->data;

    //{2g, 4g, 8g, 16g, 12g}
    const int faRange[5] = { 2, 4, 8, 16, 12};
    // 16bit
    const int faResolution = 32768;

    long micro_ms2 = raw_val * SENSOR_G * faRange[data->settings.range] / faResolution;

	val->val1 = micro_ms2 / 1000000;
	val->val2 = micro_ms2 % 1000000;

    return 0;
}

static int mc34x9_channel_get(const struct device *dev,
			       enum sensor_channel chan,
			       struct sensor_value *val)
{
    if (!dev || !val) {
        return -EINVAL;
    }

    struct mc34x9_data *data = dev->data;
    int err = -1;

    /** @FIXME: Convert raw units to meaningful units */
    switch (chan) {
    case SENSOR_CHAN_ACCEL_X:
        err = convert_raw_to_g(dev, data->accel.x, val);
        break;
    case SENSOR_CHAN_ACCEL_Y:
        err = convert_raw_to_g(dev, data->accel.y, val);
        break;
    case SENSOR_CHAN_ACCEL_Z:
        err = convert_raw_to_g(dev, data->accel.z, val);
        break;
    case SENSOR_CHAN_ACCEL_XYZ:
        LOG_DBG("X: %d, Y: %d, Z: %d", data->accel.x, data->accel.y, data->accel.z);
        err = convert_raw_to_g(dev, data->accel.x, &val[0]);
        err |= convert_raw_to_g(dev, data->accel.y, &val[1]);
        err |= convert_raw_to_g(dev, data->accel.z, &val[2]);
        break;
    default:
        return -EINVAL;
    }

    return err;
}

static const struct sensor_driver_api mc34x9_api_funcs = {
	.attr_set     = mc34x9_attr_set,
	.sample_fetch = mc34x9_sample_fetch,
	.channel_get  = mc34x9_channel_get,
};

static int mc34x9_init(const struct device *dev)
{
    const struct mc34x9_config *config = dev->config;
    struct mc34x9_data *data = dev->data;

    int err = -1;

    data->bus.bus = device_get_binding(config->bus.name);
    if (!data->bus.bus) {
        LOG_ERR("Bus is not ready");
        return -EIO;
    }
    data->bus.addr = DT_INST_REG_ADDR(0);

    /* Confirm device is expected */
    uint8_t chip_id = 0xFF;
    mc34x9_get_data(dev, &chip_id, MC34X9_REG_PROD, 1);
    if (chip_id != MC34X9_CHIP_ID) {
        LOG_ERR("Wrong Chip ID: 0x%02X", chip_id);
        return -ENODEV;
    }

    /* Initialize with basic config */
    {
        err = set_range_ctl(dev, MC34X9_CFG_RANGE_DEFAULT);
        __ASSERT(!err, "Failed to set range: %d", err);
        if (err) {
            return err;
        }

        err = enable_lpf(dev, true);
        __ASSERT(!err, "Failed to enable LPF: %d", err);
        if (err) {
            return err;
        }

        err = set_sample_rate(dev, MC34X9_CFG_SAMPLE_RATE_DEFAULT);
        __ASSERT(!err, "Failed to set sample rate: %d", err);
        if (err) {
            return err;
        }

        err = set_decimation(dev, MC34X9_CFG_DECIMATION_DEFAULT);
        __ASSERT(!err, "Failed to set decimation: %d", err);
        if (err) {
            return err;
        }

        err = set_mode(dev, MC34X9_MODE_CWAKE);
        __ASSERT(!err, "Failed to set mode: %d", err);
        if (err) {
            return err;
        }

        k_sleep(K_MSEC(50));
    }

    /* Gather gain and offset settings */
    {
        err = mc34x9_get_data(dev, data->settings.offset, MC34X9_REG_XOFFL, 6);
        __ASSERT(!err, "Failed to set mode: %d", err);
        if (err) {
            return err;
        }

        LOG_INF("Offset: X: %d, %d - Y: %d, %d, - Z: %d, %d",
            data->settings.offset[0], data->settings.offset[1],
            data->settings.offset[2], data->settings.offset[3],
            data->settings.offset[4], data->settings.offset[5]);

        err = mc34x9_get_data(dev, data->settings.gain, MC34X9_REG_XGAIN, 3);
        __ASSERT(!err, "Failed to set mode: %d", err);
        if (err) {
            return err;
        }

        LOG_INF("Gain: X: %d - Y: %d - Z: %d",
            data->settings.gain[0],
            data->settings.gain[1],
            data->settings.gain[2]);
    }

    LOG_INF("mc34x9 initialized!");

    return 0;
}

static const struct mc34x9_config mc34x9_config = {
    .bus = {
        .addr = DT_INST_REG_ADDR(0),
        .name = DT_INST_BUS_LABEL(0),
    },
};

DEVICE_DT_INST_DEFINE(0, mc34x9_init, NULL,
		    &mc34x9_data, &mc34x9_config, POST_KERNEL,
		    CONFIG_SENSOR_INIT_PRIORITY, &mc34x9_api_funcs);
