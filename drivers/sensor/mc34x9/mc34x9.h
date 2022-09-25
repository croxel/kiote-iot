#ifndef _MC34X9_H_
#define _MC34X9_H_

/**
 * Ported from: https://github.com/mcubemems/mCube_mc34x9_arduino_driver
 */

/******************************************************************************
 *** Register Map
 *****************************************************************************/
#define MC34X9_REG_DEV_STAT         (0x05)
#define MC34X9_REG_INTR_CTRL        (0x06)
#define MC34X9_REG_MODE             (0x07)
#define MC34X9_REG_SR               (0x08)
#define MC34X9_REG_MOTION_CTRL      (0x09)
#define MC34X9_REG_FIFO_STAT        (0x0A)
#define MC34X9_REG_FIFO_RD_P        (0x0B)
#define MC34X9_REG_FIFO_WR_P        (0x0C)
#define MC34X9_REG_XOUT_LSB         (0x0D)
#define MC34X9_REG_XOUT_MSB         (0x0E)
#define MC34X9_REG_YOUT_LSB         (0x0F)
#define MC34X9_REG_YOUT_MSB         (0x10)
#define MC34X9_REG_ZOUT_LSB         (0x11)
#define MC34X9_REG_ZOUT_MSB         (0x12)
#define MC34X9_REG_STATUS           (0x13)
#define MC34X9_REG_INTR_STAT        (0x14)
#define MC34X9_REG_PROD             (0x18)
#define MC34X9_REG_RANGE_C          (0x20)
#define MC34X9_REG_XOFFL            (0x21)
#define MC34X9_REG_XOFFH            (0x22)
#define MC34X9_REG_YOFFL            (0x23)
#define MC34X9_REG_YOFFH            (0x24)
#define MC34X9_REG_ZOFFL            (0x25)
#define MC34X9_REG_ZOFFH            (0x26)
#define MC34X9_REG_XGAIN            (0x27)
#define MC34X9_REG_YGAIN            (0x28)
#define MC34X9_REG_ZGAIN            (0x29)
#define MC34X9_REG_FIFO_CTRL        (0x2D)
#define MC34X9_REG_FIFO_TH          (0x2E)
#define MC34X9_REG_FIFO_INTR        (0x2F)
#define MC34X9_REG_FIFO_CTRL_SR2    (0x30)
#define MC34X9_REG_COMM_CTRL        (0x31)
#define MC34X9_REG_GPIO_CTRL        (0x33)
#define MC34X9_REG_TF_THRESH_LSB    (0x40)
#define MC34X9_REG_TF_THRESH_MSB    (0x41)
#define MC34X9_REG_TF_DB            (0x42)
#define MC34X9_REG_AM_THRESH_LSB    (0x43)
#define MC34X9_REG_AM_THRESH_MSB    (0x44)
#define MC34X9_REG_AM_DB            (0x45)
#define MC34X9_REG_SHK_THRESH_LSB   (0x46)
#define MC34X9_REG_SHK_THRESH_MSB   (0x47)
#define MC34X9_REG_PK_P2P_DUR_THRESH_LSB    (0x48)
#define MC34X9_REG_PK_P2P_DUR_THRESH_MSB    (0x49)
#define MC34X9_REG_TIMER_CTRL       (0x4A)

#define MC34X9_CHIP_ID (0xA4)

typedef enum
{
  MC34X9_MODE_SLEEP    = 0b000,
  MC34X9_MODE_CWAKE      = 0b001,
  MC34X9_MODE_RESERVED   = 0b010,
  MC34X9_MODE_STANDBY  = 0b011,
}   MC34X9_mode_t;

typedef enum
{
  MC34X9_RANGE_2G    = 0b000,
  MC34X9_RANGE_4G    = 0b001,
  MC34X9_RANGE_8G    = 0b010,
  MC34X9_RANGE_16G   = 0b011,
  MC34X9_RANGE_12G   = 0b100,
  MC34X9_RANGE_END,
}   MC34X9_range_t;

typedef enum
{
  MC34X9_SR_50Hz            = 0x08,
  MC34X9_SR_100Hz           = 0x09,
  MC34X9_SR_125Hz           = 0x0A,
  MC34X9_SR_200Hz           = 0x0B,
  MC34X9_SR_250Hz           = 0x0C,
  MC34X9_SR_500Hz           = 0x0D,
  MC34X9_SR_1000Hz          = 0x0E,
  MC34X9_SR_2000Hz          = 0x0F,
}   MC34X9_sr_t;

/** @NOTE: Added from datasheet, not from driver */
typedef enum
{
  MC34X9_DECIMATION_OFF = 0,
  MC34X9_DECIMATION_X2,
  MC34X9_DECIMATION_X4,
  MC34X9_DECIMATION_X5,
  MC34X9_DECIMATION_X8,
  MC34X9_DECIMATION_X10,
  MC34X9_DECIMATION_X16,
  MC34X9_DECIMATION_X20,
  MC34X9_DECIMATION_X40,
  MC34X9_DECIMATION_X67,
  MC34X9_DECIMATION_X80,
  MC34X9_DECIMATION_X100,
  MC34X9_DECIMATION_X200,
  MC34X9_DECIMATION_X250,
  MC34X9_DECIMATION_X500,
  MC34X9_DECIMATION_X1000,
}  MC34X9_decimation_t;

#endif /* _MC34X9_H_ */
