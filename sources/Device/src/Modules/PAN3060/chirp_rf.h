#pragma once


enum
{
    RF_OK = 0, OK = 0, RF_FAIL = 1, FAIL = 1
};
enum PAGE_SEL
{
    PAGE0_SEL, PAGE1_SEL, PAGE2_SEL, PAGE3_SEL
};

typedef struct
{
    enum PAGE_SEL page;
    uint8 addr;
    uint8 data;
}pan_reg_cfg_t;

#define    VIBROLINE_HEAD                0x7E    //constant head of the packet
#define    VIBROLINE_DEVICE_DOORBELL        0x01    //bit7 set = low battery
#define    VIBROLINE_DEVICE_PHONE          0x02    //bit7 set = low battery
#define    VIBROLINE_DEVICE_INTERCOM        0x04    //bit7 set = low battery
#define    VIBROLINE_DEVICE_BABYCRY        0x08    //bit7 set = low battery
#define    PACKET_PAYLOAD_LENGTH            4

#define SPI_WRITE_CHECK                 1

#define RF_GPIO_0                0
#define RF_GPIO_3                3
#define RF_GPIO_10                2
#define RF_GPIO_11                3
#define MODULE_GPIO_TX                  0
#define MODULE_GPIO_RX                  10
#define MODULE_GPIO_LED                  11

/* RF mode define*/
#define RF_MODE_DEEP_SLEEP              0
#define RF_MODE_SLEEP                   1
#define RF_MODE_STB1                    2
#define RF_MODE_STB2                    3
#define RF_MODE_STB3                    4
#define RF_MODE_TX                      5
#define RF_MODE_RX                      6

/* RF Tx mode */
#define RF_TX_SINGLE                    0
#define RF_TX_CONTINOUS                 1

/* RF Rx mode */
#define RF_RX_SINGLE                    0
#define RF_RX_SINGLE_TIMEOUT            1
#define RF_RX_CONTINOUS                 2

/* RF power maximum ramp */
#define RF_MAX_RAMP                     22
#define RF_MIN_RAMP                     1

/* System control register */
#define REG_SYS_CTL                     0x00
#define REG_FIFO_ACC_ADDR               0x01

/* 3V Logical area register */
#define REG_OP_MODE                     0x02

/* dcdc calibration select */
#define CALIBR_REF_CMP  0x01
#define CALIBR_ZERO_CMP 0x02
#define CALIBR_IMAX_CMP 0x03

#define MODEM_MODE_NORMAL               0x01
#define MODEM_MODE_MULTI_SECTOR         0x02

#define MODEM_MPA                       0x01
#define MODEM_MPB                       0x02

#define freq_360000000                  360000000
#define freq_370000000                  370000000
#define freq_385000000                  385000000
#define freq_405000000                  405000000
#define freq_415000000                  415000000
#define freq_430000000                  430000000
#define freq_445000000                  445000000
#define freq_465000000                  465000000
#define freq_485000000                  485000000
#define freq_495000000                  495000000
#define freq_505000000                  505000000
#define freq_530000000                  530000000
#define freq_565000000                  565000000
#define freq_600000000                  600000000

#define freq_720000000                  720000000
#define freq_740000000                  740000000
#define freq_770000000                  770000000
#define freq_810000000                  810000000
#define freq_830000000                  830000000
#define freq_860000000                  860000000
#define freq_890000000                  890000000
#define freq_930000000                  930000000
#define freq_970000000                  970000000
#define freq_1010000000                 1010000000
#define freq_1060000000                 1060000000
#define freq_1080000000                 1080000000

#define CODE_RATE_45                    0x01
#define CODE_RATE_46                    0x02
#define CODE_RATE_47                    0x03
#define CODE_RATE_48                    0x04

#define SF_5                            5
#define SF_6                            6
#define SF_7                            7
#define SF_8                            8
#define SF_9                            9
#define SF_10                           10
#define SF_11                           11
#define SF_12                           12

#define BW_62_5K                        6
#define BW_125K                         7
#define BW_250K                         8
#define BW_500K                         9

#define CRC_OFF                         0
#define CRC_ON                          1

#define PLHD_IRQ_OFF                    0
#define PLHD_IRQ_ON                     1

#define PLHD_OFF                        0
#define PLHD_ON                         1

#define PLHD_LEN8                       0
#define PLHD_LEN16                      1

#define AGC_ON                          1
#define AGC_OFF                         0

#define LO_400M                         0
#define LO_800M                         1

#define DCDC_OFF                        0
#define DCDC_ON                         1

#define LDR_OFF                         0
#define LDR_ON                          1

#define MAPM_OFF                        0
#define MAPM_ON                         1

#define CAD_DETECT_THRESHOLD_0A         0x0A
#define CAD_DETECT_THRESHOLD_10         0x10
#define CAD_DETECT_THRESHOLD_15         0x15
#define CAD_DETECT_THRESHOLD_20         0x20

#define CAD_DETECT_NUMBER_1             0x01
#define CAD_DETECT_NUMBER_2             0x02
#define CAD_DETECT_NUMBER_3             0x03

#define REG_PAYLOAD_LEN                 0x0C

/*IRQ BIT MASK*/
#define REG_IRQ_MAPM_DONE               0x40
#define REG_IRQ_RX_PLHD_DONE            0x10
#define REG_IRQ_RX_DONE                 0x08
#define REG_IRQ_CRC_ERR                 0x04
#define REG_IRQ_RX_TIMEOUT              0x02
#define REG_IRQ_TX_DONE                 0x01

#define DEFAULT_PWR                     22
#define DEFAULT_FREQ                    (470000000)
#define DEFAULT_SF                      SF_5
#define DEFAULT_BW                      BW_500K
#define DEFAULT_CR                      CODE_RATE_45

#define RADIO_FLAG_IDLE                 0
#define RADIO_FLAG_TXDONE               1
#define RADIO_FLAG_RXDONE               2
#define RADIO_FLAG_RXTIMEOUT            3
#define RADIO_FLAG_RXERR                4
#define RADIO_FLAG_PLHDRXDONE           5
#define RADIO_FLAG_MAPM                 6

#define LEVEL_INACTIVE                  0
#define LEVEL_ACTIVE                    1

const pan_reg_cfg_t g_reg_cfg[] =
{
    {PAGE0_SEL, 0x03, 0x1B},
    {PAGE0_SEL, 0x04, 0x76},
    {PAGE0_SEL, 0x06, 0x01},
    {PAGE0_SEL, 0x15, 0x21},
    {PAGE0_SEL, 0x31, 0xD0},
    {PAGE0_SEL, 0x36, 0x66},
    {PAGE0_SEL, 0x37, 0x6B},
    {PAGE0_SEL, 0x38, 0xCC},
    {PAGE0_SEL, 0x39, 0x09},
    {PAGE0_SEL, 0x3C, 0xB4},
    {PAGE0_SEL, 0x3E, 0x42},
    {PAGE0_SEL, 0x40, 0x6A},
    {PAGE0_SEL, 0x41, 0x06},
    {PAGE0_SEL, 0x42, 0xAA},
    {PAGE0_SEL, 0x48, 0x77},
    {PAGE0_SEL, 0x49, 0x77},
    {PAGE0_SEL, 0x4A, 0x77},
    {PAGE0_SEL, 0x4B, 0x05},
    {PAGE0_SEL, 0x4F, 0x04},
    {PAGE0_SEL, 0x50, 0xD2},
    {PAGE0_SEL, 0x5E, 0x80},
    {PAGE1_SEL, 0x03, 0x1B},
    {PAGE1_SEL, 0x04, 0x76},
    {PAGE1_SEL, 0x0B, 0x08},
    {PAGE1_SEL, 0x0F, 0x0A},
    {PAGE1_SEL, 0x19, 0x00},
    {PAGE1_SEL, 0x2F, 0xD0},
    {PAGE1_SEL, 0x43, 0xDA},
    {PAGE2_SEL, 0x03, 0x1B},
    {PAGE2_SEL, 0x04, 0x76},
    {PAGE2_SEL, 0x2C, 0xC0},
    {PAGE2_SEL, 0x2D, 0x27},
    {PAGE2_SEL, 0x2E, 0x09},
    {PAGE2_SEL, 0x2F, 0x00},
    {PAGE2_SEL, 0x30, 0x10},
    {PAGE3_SEL, 0x03, 0x1B},
    {PAGE3_SEL, 0x04, 0x76},
    {PAGE3_SEL, 0x0A, 0x0E},
    {PAGE3_SEL, 0x0B, 0xCF},
    {PAGE3_SEL, 0x0C, 0x19},
    {PAGE3_SEL, 0x0D, 0x98},
    {PAGE3_SEL, 0x12, 0x16},
    {PAGE3_SEL, 0x13, 0x14},
    {PAGE3_SEL, 0x16, 0xF4},
    {PAGE3_SEL, 0x17, 0x01},
    {PAGE3_SEL, 0x1F, 0xD9},
    {PAGE3_SEL, 0x26, 0x20},
};


const uint8 reg_agc_freq400[40] =
{
    0x06, 0x00, 0xf8, 0x06, 0x06, 0x00,
    0xf8, 0x06, 0x06, 0x00, 0xf8, 0x06, 0x06, 0x00, 0xf8, 0x06, 0x14, 0xc0, 0xf9, 0x14, 0x22, 0xd4,
    0xf9, 0x22, 0x30, 0xd8, 0xf9, 0x30, 0x3e, 0xde, 0xf9, 0x3e, 0x0e, 0xff, 0x80, 0x4f, 0x12, 0x80,
    0x38, 0x01
};


void rf_init(void);
uint8 rf_read_reg(uint8 _addr);
void rf_write_reg(uint8 _addr, uint8 _data);
void rf_write_fifo_otp(enum PAGE_SEL, uint8 _addr, const uint8 *_buffer, uint8 _cnt);
void rf_read_fifo(uint8 _addr, uint8 *_buffer, uint8 _cnt);
void rf_switch_page(enum PAGE_SEL);
uint8 rf_read_spec_page_reg(enum PAGE_SEL _page, uint8 _addr);
void rf_write_spec_page_reg(enum PAGE_SEL _page, uint8 _addr, uint8 _data);
//void rf_ft_calibr(void);
void rf_efuse_on(void);
void rf_efuse_off(void);
uint8 rf_efuse_read_encry_byte(uint8);
//void rf_set_agc_on(void);
void rf_deepsleep(void);
void rf_set_default_para(void);
void rf_sleep_wakeup(void);
void rf_single_tx_data_otp(const uint8 *_buffer, uint8 _cnt);
void rf_enter_continous_rx(void);
void rf_enter_single_timeout_rx(uint16 _timeout);
void rf_sleep(void);
//void rf_irq_process(void);
uint8 rf_clr_irq(void);
