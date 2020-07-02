/*
 * 3.3V		3.3V		CN14-6
 * DGND		GND			CN14-5
 * SCL		P52			CN14-3
 * SDA		P50			CN14-2
 * VS		PB6			CN14-10
 * HS		PB7			CN14-9
 * PLK		PJ5			CN14-1
 * XLK		P55			CN14-8
 * D7		P51			CN14-4
 * D6		PC6			CN14-7
 * D5		PC5			CN5-6
 * D4		PC4			CN5-4
 * D3		P33			CN13-3
 * D2		P32			CN13-2
 * D1		PC1			CN12-5
 * D0		PC0			CN12-8
 * RET		-			NC
 * PWDN		-			NC
 */
#include <stdbool.h>
#include <stdint.h>
#include "iodefine.h"

#define _00_TMR_CNT_CLR_DISABLE                     (0x00U) /* Clearing is disabled */
#define _08_TMR_CNT_CLR_COMP_MATCH_A                (0x08U) /* Cleared by compare match A */
#define _00_TMR_CMIA_INT_DISABLE                    (0x00U) /* Compare match A interrupt (CMIAn) are disabled */
#define _00_TMR_CMIB_INT_DISABLE                    (0x00U) /* Compare match B interrupt (CMIBn) are disabled */
#define _00_TMR_OVI_INT_DISABLE                     (0x00U) /* Overflow interrupt requests (OVIn) are disabled */
#define _00_TMR_COMP_MATCH_B_OUTPUT_RETAIN          (0x00U) /* No change */
#define _03_TMR_COMP_MATCH_A_OUTPUT_TOGGLE          (0x03U) /* Toggle output */
#define _F0_TMR13_TCSR_DEFAULT                      (0xF0U) /* Default TCSR write value for TMR1/3 */
#define _01_TMR3_COMP_MATCH_VALUE_A             	(0x01U)
#define _01_TMR3_COMP_MATCH_VALUE_B             	(0x01U)
#define _08_TMR_CLK_SRC_PCLK                        (0x08U) /* Use PCLK */
#define _00_TMR_PCLK_DIV_1                          (0x00U) /* Count at PCLK */
#define BSP_REG_PROTECT_MPC
#define OV7670_WRITE_ADDR 							0x42

typedef enum
{
	start_condition,		/**< Request to create I2C start condition */
	stop_condition  		/**< Request to create I2C stop condition */
} start_stop_condition_t;

/**
 * Enumeration of I2C frame types
 */
typedef enum
{
	read_frame,				/**< Frame type is a read frame */
	write_frame				/**< Frame type is a write frame */
} frame_type_t;

const uint8_t OV7670_register_setup[][2] =
{
	{0x12, 0x80},		//Reset registers

	// Image format
	{0x12, 0x14},		//QVGA size, RGB mode

	{0x40, 0xd0},		//RGB565
	{0xb0, 0x84},		//Color mode

	// Hardware window
	{0x11, 0x01},		//PCLK settings, 15fps
	{0x32, 0x80},		//HREF
	{0x17, 0x17},		//HSTART
	{0x18, 0x05},		//HSTOP
	{0x03, 0x0a},		//VREF
	{0x19, 0x02},		//VSTART
	{0x1a, 0x7a},		//VSTOP

	// Scaling numbers
	{0x70, 0x3a},		//X_SCALING
	{0x71, 0x35},		//Y_SCALING
	{0x72, 0x11},		//DCW_SCALING
	{0x73, 0xf0},		//PCLK_DIV_SCALING
	{0xa2, 0x02},		//PCLK_DELAY_SCALING

	// Matrix coefficients
	{0x4f, 0x80},		{0x50, 0x80},
	{0x51, 0x00},		{0x52, 0x22},
	{0x53, 0x5e},		{0x54, 0x80},
	{0x58, 0x9e},

	// Gamma curve values
	{0x7a, 0x20},		{0x7b, 0x10},
	{0x7c, 0x1e},		{0x7d, 0x35},
	{0x7e, 0x5a},		{0x7f, 0x69},
	{0x80, 0x76},		{0x81, 0x80},
	{0x82, 0x88},		{0x83, 0x8f},
	{0x84, 0x96},		{0x85, 0xa3},
	{0x86, 0xaf},		{0x87, 0xc4},
	{0x88, 0xd7},		{0x89, 0xe8},

	// AGC and AEC parameters
	{0xa5, 0x05},		{0xab, 0x07},
	{0x24, 0x95},		{0x25, 0x33},
	{0x26, 0xe3},		{0x9f, 0x78},
	{0xa0, 0x68},		{0xa1, 0x03},
	{0xa6, 0xd8},		{0xa7, 0xd8},
	{0xa8, 0xf0},		{0xa9, 0x90},
	{0xaa, 0x94},		{0x10, 0x00},

	// AWB parameters
	{0x43, 0x0a},		{0x44, 0xf0},
	{0x45, 0x34},		{0x46, 0x58},
	{0x47, 0x28},		{0x48, 0x3a},
	{0x59, 0x88},		{0x5a, 0x88},
	{0x5b, 0x44},		{0x5c, 0x67},
	{0x5d, 0x49},		{0x5e, 0x0e},
	{0x6c, 0x0a},		{0x6d, 0x55},
	{0x6e, 0x11},		{0x6f, 0x9f},
	{0x6a, 0x40},		{0x01, 0x40},
	{0x02, 0x60},		{0x13, 0xe7},

	// Additional parameters
	{0x34, 0x11},		{0x3f, 0x00},
	{0x75, 0x05},		{0x76, 0xe1},
	{0x4c, 0x00},		{0x77, 0x01},
	{0xb8, 0x0a},		{0x41, 0x18},
	{0x3b, 0x12},		{0xa4, 0x88},
	{0x96, 0x00},		{0x97, 0x30},
	{0x98, 0x20},		{0x99, 0x30},
	{0x9a, 0x84},		{0x9b, 0x29},
	{0x9c, 0x03},		{0x9d, 0x4c},
	{0x9e, 0x3f},		{0x78, 0x04},
	{0x0e, 0x61},		{0x0f, 0x4b},
	{0x16, 0x02},		{0x1e, 0x00},
	{0x21, 0x02},		{0x22, 0x91},
	{0x29, 0x07},		{0x33, 0x0b},
	{0x35, 0x0b},		{0x37, 0x1d},
	{0x38, 0x71},		{0x39, 0x2a},
	{0x3c, 0x78},		{0x4d, 0x40},
	{0x4e, 0x20},		{0x69, 0x00},
	{0x6b, 0x3a},		{0x74, 0x10},
	{0x8d, 0x4f},		{0x8e, 0x00},
	{0x8f, 0x00},		{0x90, 0x00},
	{0x91, 0x00},		{0x96, 0x00},
	{0x9a, 0x00},		{0xb1, 0x0c},
	{0xb2, 0x0e},		{0xb3, 0x82},
	{0x4b, 0x01},
};

static void xclk_init(void);
static void sccb_init(void);
static void generate_start_stop_condition(start_stop_condition_t start_stop_condition);
static bool write_device_address(uint8_t device_address, frame_type_t frame_type);
static bool write_device_data(uint8_t device_address, const uint8_t *data, uint16_t length);
static void delay(volatile uint16_t count);
static void cam_paralell_io_init(void);

static volatile bool rx_complete_interrupt_flag = false;
static volatile bool tx_complete_interrupt_flag = false;
static volatile uint8_t received_byte;
static uint8_t frame_buffer[160 * 120 * 2];

static void xclk_init(void)
{
    /* Enable writing to MPC pin function control registers */
    MPC.PWPR.BIT.B0WI = 0U;
    MPC.PWPR.BIT.PFSWE = 1U;

    /* Set TMO3 pin */
    MPC.P55PFS.BYTE = 0x05U;
    PORT5.PMR.BYTE |= 0x20U;

    /* Disable writing to MPC pin function control registers */
    MPC.PWPR.BIT.PFSWE = 0U;
    MPC.PWPR.BIT.B0WI = 1U;

    /* Disable protection */
    SYSTEM.PRCR.WORD = 0xA50BU;

    /* Cancel TMR module stop state */
    MSTP(TMR23) = 0U;

    /* Enable protection */
    SYSTEM.PRCR.WORD = 0xA500U;

    /* Set counter clear and interrupt */
    TMR3.TCR.BYTE = _08_TMR_CNT_CLR_COMP_MATCH_A | _00_TMR_CMIA_INT_DISABLE | _00_TMR_CMIB_INT_DISABLE |
                    _00_TMR_OVI_INT_DISABLE;

    /* Set output */
    TMR3.TCSR.BYTE = _00_TMR_COMP_MATCH_B_OUTPUT_RETAIN | _03_TMR_COMP_MATCH_A_OUTPUT_TOGGLE | _F0_TMR13_TCSR_DEFAULT;

    /* Set compare match value */
    TMR3.TCORA = _01_TMR3_COMP_MATCH_VALUE_A;
    TMR3.TCORB = _01_TMR3_COMP_MATCH_VALUE_B;

    /*Start counting*/
    TMR3.TCCR.BYTE = _08_TMR_CLK_SRC_PCLK | _00_TMR_PCLK_DIV_1;
}

#pragma interrupt (INT_Excep_SCI2_RXI2(vect=VECT_SCI2_RXI2))
void INT_Excep_SCI2_RXI2(void)
{
	received_byte = SCI2.RDR;
	rx_complete_interrupt_flag = true;
}

#pragma interrupt (INT_Excep_SCI2_TXI2(vect=VECT_SCI2_TXI2))
void INT_Excep_SCI2_TXI2(void)
{
	tx_complete_interrupt_flag = true;
}

static void sccb_init(void)
{
	/* set up pins in pin controller */

	/* enable writing to MPC */
    MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;

    /* set SSDA2 pin to P50 */
    MPC.P50PFS.BYTE = 0x0AU;

    /* set SSCL2 pin to P52 */
    MPC.P52PFS.BYTE = 0x0AU;

    /* disable writing MPC */
    MPC.PWPR.BIT.PFSWE = 0U;
    MPC.PWPR.BIT.B0WI = 1U;


	/* release SCI2 from stop */
	SYSTEM.PRCR.WORD = 0xa502U;
	SYSTEM.MSTPCRB.BIT.MSTPB29 = 0U;
	SYSTEM.PRCR.WORD = 0xa500U;

	/* disable SCI2 and all its interrupts */
    SCI2.SCR.BYTE = 0U;

	/* set P52 drive capacity control high */
	PORT5.DSCR.BIT.B2 = 1U;

	/* set P52 pull-Up resistor off */
	PORT5.PCR.BIT.B2 = 1U;

	/* set P52 port direction to input */
	PORT5.PDR.BIT.B2 = 0U;

	/* set P52 open-drain to NMOS OD output */
	PORT5.ODR0.BIT.B4 = 1U;

	/* set P52 mode to not GPIO */
    PORT5.PMR.BIT.B2 = 1U;

	/* set P50 drive capacity control high */
	PORT5.DSCR.BIT.B0 = 1U;

	/* set P50 pull-up resistor off */
	PORT5.PCR.BIT.B0 = 1U;

	/* set P50 port direction to input */
	PORT5.PDR.BIT.B0 = 0U;

	/* set P50 open-drain to NMOS OD output */
	PORT5.ODR0.BIT.B0 = 1U;

	/* set P50 mode to not GPIO */
    PORT5.PMR.BIT.B0 = 1U;

	/* place i2c pins in high impedance */
	SCI2.SIMR3.BIT.IICSCLS = 3U;
	SCI2.SIMR3.BIT.IICSDAS = 3U;

    /* clock select */
    SCI2.SMR.BYTE = 0;

    /* msb first */
    SCI2.SCMR.BIT.SDIR = 1U;

    /* no inversion */
    SCI2.SCMR.BIT.SINV = 0U;

    /* not smart card mode */
    SCI2.SCMR.BIT.SMIF = 0U;

    /* baud rate */
    SCI2.BRR = 18U;

    /* set noise filter enable */
    SCI2.SEMR.BIT.NFEN = 1U;

    /* bit rate modulation disable */
    SCI2.SEMR.BIT.BRME = 0U;

    /* noie filter clock select */
    SCI2.SNFR.BIT.NFCS = 1U;

    /* select simple IIC mode */
    SCI2.SIMR1.BIT.IICM = 1U;

    /* set SDA output delay */
    SCI2.SIMR1.BIT.IICDL = 18U;

    /* set reception/transmission of ACK/NACK */
    SCI2.SIMR2.BIT.IICACKT = 1U;

    /* clock synch on */
    SCI2.SIMR2.BIT.IICCSC = 1U;

    /* use tx/rx interrupts */
    SCI2.SIMR2.BIT.IICINTM = 1U;

    /* SPI mode off */
    SCI2.SPMR.BYTE = 0U;

    /* enable tx/tx, txi/rxi and tei (used as start/stop bit generation complete) interrupts although the TEI interrupt is polled */
	SCI2.SCR.BYTE = 0xb4U;

	/* enable group BL0 interupt 4 tei (used as start/stop bit generation complete) interrupt) */
	ICU.GENBL0.BIT.EN4 = 1U;

	/* set up rx interrupt */
	IR(SCI2, RXI2) = 0U;
	IPR(SCI2, RXI2) = 5U;
	IEN(SCI2, RXI2) = 1U;

	/* set up transmit interrupt */
	IR(SCI2, TXI2) = 0U;
	IPR(SCI2, TXI2) = 5U;
	IEN(SCI2, TXI2) = 1U;
}

bool write_device_data(uint8_t device_address, const uint8_t *data, uint16_t length)
{
	bool success = false;
	uint16_t i;

	/* disable read interrupt generation */
	SCI2.SCR.BIT.RIE = 0U;

	generate_start_stop_condition(start_condition);

	if (write_device_address(device_address, write_frame))
	{
		success = true;

		for (i = 0U; i < length; i++)
		{
			SCI2.TDR = data[i];

			/* wait for tx interrupt */
			while (!tx_complete_interrupt_flag)
			{
			}
			tx_complete_interrupt_flag = false;
		}
	}

	generate_start_stop_condition(stop_condition);

	return success;
}

static bool write_device_address(uint8_t device_address, frame_type_t frame_type)
{
	uint8_t byte_to_write;

	byte_to_write = device_address << 1;
	if (frame_type == read_frame)
	{
		byte_to_write++;
	}

	/* send device address to transmit register with write bit */
	SCI2.TDR = byte_to_write;

	/* wait for tx interrupt */
	while (!tx_complete_interrupt_flag)
	{
	}
	tx_complete_interrupt_flag = false;

	/* check ACK received */
	if (SCI2.SISR.BIT.IICACKR == 0U)
	{
		return true;
	}

	return false;
}

static void generate_start_stop_condition(start_stop_condition_t start_stop_condition)
{
	switch (start_stop_condition)
	{
	case start_condition:
		/* generate a start condition */
		SCI2.SIMR3.BYTE = 0x51U;
		break;

	case stop_condition:
		/* generate a stop condition */
		SCI2.SIMR3.BYTE = 0x54U;
		break;

	default:
		return;
		break;				/* keep MISRA happy */
	}

	/* wait for start condition completed interrupt flag is set */
	while (SCI2.SIMR3.BIT.IICSTIF == 0U)
	{
	}
	SCI2.SIMR3.BIT.IICSTIF = 0U;

	switch (start_stop_condition)
	{
	case start_condition:
		/* end start condition generation */
		SCI2.SIMR3.BIT.IICSCLS = 0U;
		SCI2.SIMR3.BIT.IICSDAS = 0U;
		break;

	case stop_condition:
		/* end stop condition generation */
		SCI2.SIMR3.BIT.IICSCLS = 3U;
		SCI2.SIMR3.BIT.IICSDAS = 3U;
		break;

	default:
		/* keep MISRA happy */
		break;
	}
}

static bool ov7670_init(void)
{
	uint8_t i;
	bool result;

	for (i = 0U; i < sizeof(OV7670_register_setup) / 2; i++)
	{
		result = write_device_data(OV7670_WRITE_ADDR >> 1, &OV7670_register_setup[i][0], 2U);

		if (result == false)
		{
			break;
		}

		delay(0xFFFF);
	}

	return result;
}

static void delay(volatile uint16_t count)
{
	while (count--)
	{
	}
}

static void cam_paralell_io_init(void)
{
	PORTC.PMR.BIT.B0 = 0U;	/* mode to gpio */
	PORTC.PDR.BIT.B0 = 0U;	/* input */
	PORTC.PCR.BIT.B0 = 0U;  /* pull-up disable */

	PORTC.PMR.BIT.B1 = 0U;	/* mode to gpio */
	PORTC.PDR.BIT.B1 = 0U;	/* input */
	PORTC.PCR.BIT.B1 = 0U;  /* pull-up disable */

	PORT3.PMR.BIT.B2 = 0U;	/* mode to gpio */
	PORT3.PDR.BIT.B2 = 0U;	/* input */
	PORT3.PCR.BIT.B2 = 0U;  /* pull-up disable */

	PORT3.PMR.BIT.B3 = 0U;	/* mode to gpio */
	PORT3.PDR.BIT.B3 = 0U;	/* input */
	PORT3.PCR.BIT.B3 = 0U;  /* pull-up disable */

	PORTC.PMR.BIT.B4 = 0U;	/* mode to gpio */
	PORTC.PDR.BIT.B4 = 0U;	/* input */
	PORTC.PCR.BIT.B4 = 0U;  /* pull-up disable */

	PORTC.PMR.BIT.B5 = 0U;	/* mode to gpio */
	PORTC.PDR.BIT.B5 = 0U;	/* input */
	PORTC.PCR.BIT.B5 = 0U;  /* pull-up disable */

	PORTC.PMR.BIT.B6 = 0U;	/* mode to gpio */
	PORTC.PDR.BIT.B6 = 0U;	/* input */
	PORTC.PCR.BIT.B6 = 0U;  /* pull-up disable */

	PORT5.PMR.BIT.B1 = 0U;	/* mode to gpio */
	PORT5.PDR.BIT.B1 = 0U;	/* input */
	PORT5.PCR.BIT.B1 = 0U;  /* pull-up disable */

	PORTJ.PMR.BIT.B5 = 0U;	/* mode to gpio */
	PORTJ.PDR.BIT.B5 = 0U;	/* input */
	PORTJ.PCR.BIT.B5 = 0U;  /* pull-up disable */

	PORTB.PMR.BIT.B6 = 0U;	/* mode to gpio */
	PORTB.PDR.BIT.B6 = 0U;	/* input */
	PORTB.PCR.BIT.B6 = 0U;  /* pull-up disable */

	PORTB.PMR.BIT.B7 = 0U;	/* mode to gpio */
	PORTB.PDR.BIT.B7 = 0U;	/* input */
	PORTB.PCR.BIT.B7 = 0U;  /* pull-up disable */
}

void camlib_init(void)
{
	xclk_init();
	sccb_init();
	cam_paralell_io_init();
	ov7670_init();
}

void camlib_capture(void)
{
	uint_fast32_t i;

	// disable all interrupts that are active individually as disabling all interrupts globally via PSW is not possible in user mode
	IEN(CMT0, CMI0) = 0U;

	// wait for VS
	while (PORTB.PIDR.BIT.B6 == 0)
	{
	}
	while (PORTB.PIDR.BIT.B6 == 1)
	{
	}
	i = 0UL;

	for (uint_fast8_t y = 0U; y < 120U; y++)
	{
		// wait for first HS
		while (PORTB.PIDR.BIT.B7 == 0)
		{
		}

		while (PORTB.PIDR.BIT.B7 == 1)
		{
		}

		// wait for second HS
		while (PORTB.PIDR.BIT.B7 == 0)
		{
		}

		for (uint_fast16_t x = 0U; x < 160U; x++)
		{
			// wait for PLK
			while (PORTJ.PIDR.BIT.B5 == 0)
			{
			}

			frame_buffer[i] = PORTC.PIDR.BYTE & 0x73;
			frame_buffer[i] |= (PORT3.PIDR.BYTE & 0x0c);
			if (PORT5.PIDR.BIT.B1 == 1)
			{
				frame_buffer[i] |= 0x80;
			}
			i++;

			// wait for ~PLK
			while (PORTJ.PIDR.BIT.B5 == 1)
			{
			}

			// wait for PLK
			while (PORTJ.PIDR.BIT.B5 == 0)
			{
			}

			//frame_buffer[i] = GPIOA->IDR;
			frame_buffer[i] = PORTC.PIDR.BYTE & 0x73;
			frame_buffer[i] |= (PORT3.PIDR.BYTE & 0x0c);
			if (PORT5.PIDR.BIT.B1 == 1)
			{
				frame_buffer[i] |= 0x80;
			}
			i++;

			// wait for ~PLK
			while (PORTJ.PIDR.BIT.B5 == 1)
			{
			}

			// repeat doing dummy read
			while (PORTJ.PIDR.BIT.B5 == 0)
			{
			}

			while (PORTJ.PIDR.BIT.B5 == 1)
			{
			}

			while (PORTJ.PIDR.BIT.B5 == 0)
			{
			}

			while (PORTJ.PIDR.BIT.B5 == 1)
			{
			}
		}
	}

	// re-enable interrupts
	IEN(CMT0, CMI0) = 1U;
}

uint16_t *camlib_get_frame(void)
{
	return (uint16_t *)&frame_buffer[0];
}
