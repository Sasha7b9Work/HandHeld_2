// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Utils/String.h"
#include "Modules/PAN3060/PAN3060.h"

#ifdef GD32E230
	#include <gd32e23x.h>
#endif
#ifdef GD32F303
	#include <gd32f30x.h>
	#include "system_gd32f30x.c"
#endif

#define OPERATION_TIMEOUT	300000	//300s = 5 min
#define RX_BUFFER_LENGTH	134

#ifdef GD32E230
static const uint BEGIN_FIRMWARE = 0x8002000;
static const uint NUM_PAGES = 54;
#define FLASH_PAGE_SIZE		1024
#define LED_RED_PORT 			GPIOA
#define LED_RED_PIN 			GPIO_PIN_9
#define LED_GREEN_PORT 		GPIOA
#define LED_GREEN_PIN 		GPIO_PIN_11
#define LED_BLUE_PORT 		GPIOA
#define LED_BLUE_PIN 			GPIO_PIN_10
#define BUTTON_DOWN_PORT 	GPIOB
#define BUTTON_DOWN_PIN		GPIO_PIN_5
#define VIBRO_PORT 				GPIOB
#define VIBRO_PIN					GPIO_PIN_4
#define PWR_CTRL_PORT 		GPIOC
#define PWR_CTRL_PIN			GPIO_PIN_13
#define TFT_COLUMN_NUMBER 0
#define TFT_LINE_NUMBER 	0
#define TFT_COLUMN_OFFSET 0
#endif
#ifdef GD32F303
static const uint BEGIN_FIRMWARE = 0x8002000;
static const uint NUM_PAGES = 91;
#define FLASH_PAGE_SIZE		2048
#define LED_RED_PORT 			GPIOA
#define LED_RED_PIN 			GPIO_PIN_9
#define LED_GREEN_PORT 		GPIOA
#define LED_GREEN_PIN 		GPIO_PIN_11
#define LED_BLUE_PORT 		GPIOA
#define LED_BLUE_PIN 			GPIO_PIN_10
#define BUTTON_DOWN_PORT 	GPIOB
#define BUTTON_DOWN_PIN		GPIO_PIN_5
#define VIBRO_PORT 				GPIOB
#define VIBRO_PIN					GPIO_PIN_4
#define PWR_CTRL_PORT 		GPIOA
#define PWR_CTRL_PIN			GPIO_PIN_0
#define TFT_COLUMN_NUMBER 240
#define TFT_LINE_NUMBER 	320
#define TFT_COLUMN_OFFSET 0
#endif

#define TFT_ON_PORT				GPIOB
#define TFT_ON_PIN				GPIO_PIN_2
#define TFT_BKG_PORT			GPIOA
#define TFT_BKG_PIN				GPIO_PIN_4
#define TFT_SPI_SCK_PORT	GPIOA
#define TFT_SPI_SCK_PIN		GPIO_PIN_5
#define TFT_SPI_SDA_PORT	GPIOA
#define TFT_SPI_SDA_PIN		GPIO_PIN_7
#define TFT_DC_PORT				GPIOB
#define TFT_DC_PIN				GPIO_PIN_11
#define TFT_RST_PORT			GPIOB
#define TFT_RST_PIN				GPIO_PIN_10

#define END_FIRMWARE			(BEGIN_FIRMWARE + NUM_PAGES * FLASH_PAGE_SIZE - 1)

#ifndef WIN32
    #if (__ARMCC_VERSION < 6180000)
        #error("Требуется компилятор версии 6.18. Прошивка, скомпилированная на другом компиляторе, может неправильно работать")
    #endif
#endif

uint rx_irq_set_f = 0;
uint8 rx_buffer[RX_BUFFER_LENGTH] __attribute__((aligned(4)));
uint8 rx_page[FLASH_PAGE_SIZE] __attribute__((aligned(4)));
uint _crc_incoming;

static void JumpToMainApplication();
static void flash_operation(uint _address, uint8* rx_page);
static void Power_off_error();
static void Blink_green();
static void TFT_init();
static void TFT_clear();

int main()
{
		TimeMeterMS meter;
		uint _buffer_offset;

    HAL::Init();
		//init button 'down' and vibro port
		#ifdef GD32E230
		gpio_mode_set(BUTTON_DOWN_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, BUTTON_DOWN_PIN);
    gpio_mode_set(VIBRO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, VIBRO_PIN);
    gpio_output_options_set(VIBRO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, VIBRO_PIN);
		#endif
		#ifdef GD32F303
		gpio_init(BUTTON_DOWN_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, BUTTON_DOWN_PIN);
		gpio_init(VIBRO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, VIBRO_PIN);
		#endif
    gpio_bit_reset(VIBRO_PORT, VIBRO_PIN);
	
		//check crc of existing firmware (90ms of 54kb calculation for GD32E230) and check down button 
    uint crc_existing_full = SU::CalculateCRC32((const void *)BEGIN_FIRMWARE, NUM_PAGES * FLASH_PAGE_SIZE);
    uint crc_existing_without_crc = SU::CalculateCRC32((const void *)BEGIN_FIRMWARE, NUM_PAGES * FLASH_PAGE_SIZE - 4);
    if(!gpio_input_bit_get(BUTTON_DOWN_PORT, BUTTON_DOWN_PIN) || (crc_existing_full != 0))
    {
				//turn on white led
				#ifdef GD32E230
				gpio_mode_set(LED_RED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_RED_PIN);
				gpio_output_options_set(LED_RED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED_RED_PIN);
				gpio_mode_set(LED_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_GREEN_PIN);
				gpio_output_options_set(LED_GREEN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED_GREEN_PIN);
				gpio_mode_set(LED_BLUE_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_BLUE_PIN);
				gpio_output_options_set(LED_BLUE_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED_BLUE_PIN);
				#endif
				#ifdef GD32F303
				gpio_init(LED_RED_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED_RED_PIN);
				gpio_init(LED_GREEN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED_GREEN_PIN);
				gpio_init(LED_BLUE_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED_BLUE_PIN);
				#endif

				//color red
				gpio_bit_set(LED_RED_PORT, LED_RED_PIN);
				//color green
				gpio_bit_set(LED_GREEN_PORT, LED_GREEN_PIN);
				//color blue
				gpio_bit_set(LED_BLUE_PORT, LED_BLUE_PIN);

				//display test
				#ifdef GD32F303
				TFT_init();
				TFT_clear();
				gpio_bit_set(TFT_BKG_PORT, TFT_BKG_PIN);
				Timer::Delay(1000);
				gpio_bit_reset(TFT_BKG_PORT, TFT_BKG_PIN);
				gpio_bit_set(TFT_ON_PORT, TFT_ON_PIN);
				#endif

				//short vibration
				#ifdef GD32E230
		    gpio_bit_set(VIBRO_PORT, VIBRO_PIN);
				Timer::Delay(200);
				gpio_bit_reset(VIBRO_PORT, VIBRO_PIN);
				#endif

				//turn on receiver
				PAN3060::Init();

				//wait for first valid packet or timeout
				while(1)
				{
						if(meter.ElapsedTime() > OPERATION_TIMEOUT)
								Power_off_error();

						if(rx_irq_set_f)
						{
								//check if incoming crc is the same as existing firmware
								_crc_incoming = rx_buffer[RX_BUFFER_LENGTH-4] + (rx_buffer[RX_BUFFER_LENGTH-3] << 8) + (rx_buffer[RX_BUFFER_LENGTH-2] << 16) + (rx_buffer[RX_BUFFER_LENGTH-1] << 24);
								if(crc_existing_without_crc == _crc_incoming)
								{
										Blink_green();
										JumpToMainApplication();	//actual firmware is already exists
								}

								if((rx_buffer[0] == 0) && (rx_buffer[1] == 0))
										break;
						};
				};

				while(1)
				{
						//blue led
						gpio_bit_reset(LED_RED_PORT, LED_RED_PIN);				//red
						gpio_bit_set(LED_BLUE_PORT, LED_BLUE_PIN);				//blue
						gpio_bit_reset(LED_GREEN_PORT, LED_GREEN_PIN);		//green

						_buffer_offset = 0;
						while(_buffer_offset < (FLASH_PAGE_SIZE / 128))
						{
								if(meter.ElapsedTime() > OPERATION_TIMEOUT)
										Power_off_error();
								
								if(rx_irq_set_f)
								{
										rx_irq_set_f = 0;
										std::memcpy(rx_page + (_buffer_offset << 7), rx_buffer + 2, 128);
										_buffer_offset++;
								    meter.Reset();							//reset timer if valid packet has been received
								};
						};
						
						//turn off led
						gpio_bit_reset(LED_BLUE_PORT, LED_BLUE_PIN);		//blue
						
						//erase page (datasheet max time 4ms) and write page, it takes typically 15ms, datasheet max time 43ms for GD32E230
						unsigned long _address = BEGIN_FIRMWARE + ((((rx_buffer[1] << 8) | rx_buffer[0]) << 7) & ~((unsigned long)(FLASH_PAGE_SIZE-1)));
						if(_address < END_FIRMWARE)
						{
								//if last page, check CRC of entire firmware
								if(_address == END_FIRMWARE - (FLASH_PAGE_SIZE-1))
								{
										uint _crc_new = SU::CalculateCRC32((const void *)BEGIN_FIRMWARE, NUM_PAGES * FLASH_PAGE_SIZE - 4);
										if(_crc_new == _crc_incoming)
										{
												//write new CRC to the end of memory
												rx_page[FLASH_PAGE_SIZE-4] = rx_buffer[130];
												rx_page[FLASH_PAGE_SIZE-3] = rx_buffer[131];
												rx_page[FLASH_PAGE_SIZE-2] = rx_buffer[132];
												rx_page[FLASH_PAGE_SIZE-1] = rx_buffer[133];
										}
										else
											Power_off_error();
								};

								flash_operation(_address, rx_page);
						};

						//if last page, exit
						if(_address == END_FIRMWARE - (FLASH_PAGE_SIZE-1))
						{
								Blink_green();
								JumpToMainApplication();
						};
				};
		}
		else
				JumpToMainApplication();
}

void JumpToMainApplication()
{
#define APP_ADDRESS 0x8002000

#ifndef WIN32

    typedef void (*pFunction)(void);
    pFunction jump_to_app;
    uint jump_address;

    __disable_irq();

    SCB->VTOR = APP_ADDRESS;

    __set_MSP(*(__IO uint *)APP_ADDRESS);

    jump_address = *(__IO uint32_t *)(APP_ADDRESS + 4);
    jump_to_app = (pFunction)jump_address;
    __enable_irq();

    jump_to_app();

#endif
}

__attribute__((section("ramfunc")))
void flash_operation(uint _address, uint8* rx_page)
{
								#ifdef GD32E230
								HAL_ROM::ErasePage(_address);
								gpio_bit_set(LED_BLUE_PORT, LED_BLUE_PIN);		//blue
								uint *data = (uint *)rx_page;
								__disable_irq();
								fmc_unlock();
//							HAL_ROM::WritePage(_address, rx_page);
								for(int i = 0; i < FLASH_PAGE_SIZE; i += 4)
								{
										fmc_word_program(_address, *data);
										fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
										_address += 4U;
										data++;
								};
								fmc_lock();
								__enable_irq();
								#endif
								#ifdef GD32F303
								fmc_bank0_unlock();
								fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
								fmc_page_erase(_address);
								gpio_bit_set(LED_BLUE_PORT, LED_BLUE_PIN);		//blue
								uint *data = (uint *)rx_page;
								__disable_irq();
								for(int i = 0; i < FLASH_PAGE_SIZE; i += 4)
								{
										fmc_word_program(_address, *data);
										fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
										_address += 4;
										data++;
								};
								__enable_irq();
								fmc_bank0_lock();
								#endif
}

void Power_off_error()
{
		//blink red light for 0.5s
		gpio_bit_set(LED_RED_PORT, LED_RED_PIN);				//red
		gpio_bit_reset(LED_BLUE_PORT, LED_BLUE_PIN);		//blue
		gpio_bit_reset(LED_GREEN_PORT, LED_GREEN_PIN);	//green
		Timer::Delay(500);
		gpio_bit_reset(LED_RED_PORT, LED_RED_PIN);
	
		//power off - set 1 to PWR_CTRL
		#ifdef GD32E230
    gpio_mode_set(PWR_CTRL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PWR_CTRL_PIN);
    gpio_output_options_set(PWR_CTRL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PWR_CTRL_PIN);
		#endif
		#ifdef GD32F303
		gpio_init(PWR_CTRL_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, PWR_CTRL_PIN);
		#endif
		gpio_bit_set(PWR_CTRL_PORT, PWR_CTRL_PIN);
		while(1)
			;
}

void Blink_green()
{
		//blink green light for 0.3s
		gpio_bit_reset(LED_RED_PORT, LED_RED_PIN);			//red
		gpio_bit_reset(LED_BLUE_PORT, LED_BLUE_PIN);		//blue
		gpio_bit_set(LED_GREEN_PORT, LED_GREEN_PIN);		//green
	
		//turn on vibro motor
    gpio_bit_set(VIBRO_PORT, VIBRO_PIN);

		Timer::Delay(300);
		gpio_bit_reset(LED_GREEN_PORT, LED_GREEN_PIN);
    gpio_bit_reset(VIBRO_PORT, VIBRO_PIN);
}

void SPI_SendByte(unsigned  char byte)
{
	spi_i2s_data_transmit(SPI0, byte);
}

void TFT_SEND_CMD(unsigned char o_command)
  {
		gpio_bit_reset(TFT_DC_PORT, TFT_DC_PIN);
    SPI_SendByte(o_command);
  }

//ПтТєѕ§ЖБРґТ»ёц8О»КэѕЭ
void TFT_SEND_DATA(unsigned  char o_data)
  { 
		gpio_bit_set(TFT_DC_PORT, TFT_DC_PIN);
    SPI_SendByte(o_data);
   }

void TFT_clear(void)
  {
#ifdef GD32F303
		uint32_t _buffer = 0xFFFFFFFF;	//white
		
    TFT_SEND_CMD(0x2a);     //Column address set
		TFT_SEND_DATA(0x00);    //start column
		TFT_SEND_DATA(0x00); 
		TFT_SEND_DATA(0x00);    //end column
		TFT_SEND_DATA(0xF0);

		TFT_SEND_CMD(0x2b);     //Row address set
		TFT_SEND_DATA(0x00);    //start row
		TFT_SEND_DATA(0x00); 
		TFT_SEND_DATA(0x01);    //end row
		TFT_SEND_DATA(0x40);
    TFT_SEND_CMD(0x2C);     //Memory write

		gpio_bit_set(TFT_DC_PORT, TFT_DC_PIN);

		//init PLL
    RCU_APB1EN |= RCU_APB1EN_PMUEN;		//LDO output voltage high mode
    PMU_CTL |= PMU_CTL_LDOVS;

    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

    RCU_CFG0 &= ~(RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_4 | RCU_CFG0_PLLMF_5);	//CK_PLL = (CK_IRC8M/2) * 18 = 120 MHz
    RCU_CFG0 |= RCU_PLL_MUL30;

    RCU_CTL |= RCU_CTL_PLLEN;					//enable PLL
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)){
    }
    
    PMU_CTL |= PMU_CTL_HDEN;					//enable the high-drive to extend the clock frequency to 120 MHz
    while(0U == (PMU_CS & PMU_CS_HDRF)){
    }
    
    PMU_CTL |= PMU_CTL_HDS;						//select the high-drive mode
    while(0U == (PMU_CS & PMU_CS_HDSRF)){
    }

    __IO uint32_t reg_temp = RCU_CFG0;
    reg_temp &= ~RCU_CFG0_SCS;				//select PLL as system clock
    reg_temp |= RCU_CKSYSSRC_PLL;
    RCU_CFG0 = reg_temp;

    while(0U == (RCU_CFG0 & RCU_SCSS_PLL)){	//wait until PLL is selected as system clock
    }
		
		//init DMA
    dma_parameter_struct dma_init_struct;
    dma_deinit(DMA0, DMA_CH2); // Очистка канала перед новой передачей
        
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI0);
    dma_init_struct.memory_addr  = (uint32_t)&_buffer;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.number       = TFT_COLUMN_NUMBER*TFT_LINE_NUMBER*2/4;					//counter is 16-bit only
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;

		//Приходится разбить на 4 части, потому что счетчик DMA 16-битный и его разрядности недостаточно
		for(uint _i=0; _i < 4;_i++)
		{
			//Инициализация
			dma_init(DMA0, DMA_CH2, &dma_init_struct);

			// Запуск передачи
			dma_channel_enable(DMA0, DMA_CH2);
        
			// Ждем завершения
			while(RESET == dma_flag_get(DMA0, DMA_CH2, DMA_FLAG_FTF));
			dma_deinit(DMA0, DMA_CH2);
		};
		
		//switch back to 8MHz IRC
		system_clock_config();
#endif
  }
void TFT_init(void)        
  {
		#ifdef GD32E230
		gpio_mode_set(TFT_ON_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TFT_ON_PIN);
		gpio_output_options_set(TFT_ON_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TFT_ON_PIN);
		gpio_mode_set(TFT_BKG_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TFT_BKG_PIN);
		gpio_output_options_set(TFT_BKG_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TFT_BKG_PIN);
		gpio_mode_set(TFT_RST_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TFT_RST_PIN);
		gpio_output_options_set(TFT_RST_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TFT_RST_PIN);
		gpio_mode_set(TFT_DC_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TFT_DC_PIN);
		gpio_output_options_set(TFT_DC_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TFT_DC_PIN);
		gpio_mode_set(TFT_SPI_SCK_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TFT_SPI_SCK_PIN);
		gpio_output_options_set(TFT_SPI_SCK_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TFT_SPI_SCK_PIN);
		gpio_mode_set(TFT_SPI_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TFT_SPI_SDA_PIN);
		gpio_output_options_set(TFT_SPI_SDA_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TFT_SPI_SDA_PIN);
		#endif
		
		#ifdef GD32F303
		gpio_init(TFT_ON_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, TFT_ON_PIN);
		gpio_init(TFT_BKG_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, TFT_BKG_PIN);
		gpio_init(TFT_RST_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, TFT_RST_PIN);
		gpio_init(TFT_DC_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, TFT_DC_PIN);
		gpio_init(TFT_SPI_SCK_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, TFT_SPI_SCK_PIN);
		gpio_init(TFT_SPI_SDA_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, TFT_SPI_SDA_PIN);
		#endif

		#ifdef GD32F303
		spi_parameter_struct spi_is;
    spi_i2s_deinit(SPI0);
    spi_struct_para_init(&spi_is);

    spi_is.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_is.device_mode = SPI_MASTER;
    spi_is.frame_size = SPI_FRAMESIZE_8BIT;
    spi_is.clock_polarity_phase = SPI_CK_PL_HIGH_PH_1EDGE;
    spi_is.nss = SPI_NSS_SOFT;
    spi_is.prescale = SPI_PSC_2;
    spi_is.endian = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_is);
		spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
    spi_enable(SPI0);

		gpio_bit_reset(TFT_ON_PORT, TFT_ON_PIN);
		Timer::Delay(200);
		gpio_bit_reset(TFT_RST_PORT, TFT_RST_PIN);
		gpio_bit_reset(TFT_DC_PORT, TFT_DC_PIN);
		Timer::Delay(50);
		gpio_bit_set(TFT_RST_PORT, TFT_RST_PIN);
		Timer::Delay(150);
//		TFT_SEND_CMD(0x11); //Exit Sleep // НЛіцЛЇГЯДЈКЅ
//		Timer::Delay(1200);

//-----------------------ST7789V Frame rate setting-----------------//
//************************************************

								TFT_SEND_CMD(0x3A);        //65k mode
                TFT_SEND_DATA(0x05);
                TFT_SEND_CMD(0xC5); 		//VCOM
                TFT_SEND_DATA(0x1A);
                TFT_SEND_CMD(0x36);                 // ЖБД»ПФКѕ·ЅПтЙиЦГ
                TFT_SEND_DATA(0x00);
                //-------------ST7789V Frame rate setting-----------//
                TFT_SEND_CMD(0xb2);		//Porch Setting
                TFT_SEND_DATA(0x05);
                TFT_SEND_DATA(0x05);
                TFT_SEND_DATA(0x00);
                TFT_SEND_DATA(0x33);
                TFT_SEND_DATA(0x33);

                TFT_SEND_CMD(0xb7);			//Gate Control
                TFT_SEND_DATA(0x05);			//12.2v   -10.43v
                //--------------ST7789V Power setting---------------//
                TFT_SEND_CMD(0xBB);//VCOM
                TFT_SEND_DATA(0x3F);

                TFT_SEND_CMD(0xC0); //Power control
                TFT_SEND_DATA(0x2c);

                TFT_SEND_CMD(0xC2);		//VDV and VRH Command Enable
                TFT_SEND_DATA(0x01);

                TFT_SEND_CMD(0xC3);			//VRH Set
                TFT_SEND_DATA(0x0F);		//4.3+( vcom+vcom offset+vdv)

                TFT_SEND_CMD(0xC4);			//VDV Set
                TFT_SEND_DATA(0x20);				//0v

                TFT_SEND_CMD(0xC6);				//Frame Rate Control in Normal Mode
                TFT_SEND_DATA(0X01);			//111Hz

                TFT_SEND_CMD(0xd0);				//Power Control 1
                TFT_SEND_DATA(0xa4);
                TFT_SEND_DATA(0xa1);

                TFT_SEND_CMD(0xE8);				//Power Control 1
                TFT_SEND_DATA(0x03);

                TFT_SEND_CMD(0xE9);				//Equalize time control
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x08);
                //---------------ST7789V gamma setting-------------//
                TFT_SEND_CMD(0xE0); //Set Gamma
                TFT_SEND_DATA(0xD0);
                TFT_SEND_DATA(0x05);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x08);
                TFT_SEND_DATA(0x14);
                TFT_SEND_DATA(0x28);
                TFT_SEND_DATA(0x33);
                TFT_SEND_DATA(0x3F);
                TFT_SEND_DATA(0x07);
                TFT_SEND_DATA(0x13);
                TFT_SEND_DATA(0x14);
                TFT_SEND_DATA(0x28);
                TFT_SEND_DATA(0x30);
                 
                TFT_SEND_CMD(0XE1); //Set Gamma
                TFT_SEND_DATA(0xD0);
                TFT_SEND_DATA(0x05);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x08);
                TFT_SEND_DATA(0x03);
                TFT_SEND_DATA(0x24);
                TFT_SEND_DATA(0x32);
                TFT_SEND_DATA(0x32);
                TFT_SEND_DATA(0x3B);
                TFT_SEND_DATA(0x14);
                TFT_SEND_DATA(0x13);
                TFT_SEND_DATA(0x28);
                TFT_SEND_DATA(0x2F);

                TFT_SEND_CMD(0x20); 		//·ґПФ
	TFT_SEND_CMD(0x11); //Exit Sleep // НЛіцЛЇГЯДЈКЅ
	Timer::Delay(120);
	TFT_SEND_CMD(0x29); //Display on // їЄПФКѕ
	#endif
  }
