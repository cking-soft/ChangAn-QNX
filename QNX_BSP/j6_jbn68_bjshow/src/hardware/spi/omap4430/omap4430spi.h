/*
 * $QNXLicenseC:
 * Copyright 2009, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#ifndef __OMAP4430_SPI_H_INCLUDED
#define __OMAP4430_SPI_H_INCLUDED

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <hw/spi-master.h>
#include "sdma.h"

#ifndef write_omap
#define	write_omap(__port,__val)	out32(__port,__val)
#endif

#ifndef read_omap
#define	read_omap(__port)	in32(__port)
#endif

#define	OMAP4430_SPI_PRIORITY		21
#define	OMAP4430_SPI_EVENT			1
#define	OMAP4430_SDMA_EVENT			2
#define	OMAP4430_SPI_MAXDMALEN		(1024 * 8)

#define NUM_OF_SPI_DEVS				4

// Offset between register sets for the 4 SPI devices
#define OMAP4430_SPI_DEVICE_OFFSET	20

// Offset between register sets for the 4 SPI devices
#define OMAP4430_INTERRUPT_BITS_PER_SPI_CHANNEL 4

/* SPI slave interrupt-type definitions */
#define INTR_TYPE_TX0_EMPTY			0x00001
#define INTR_TYPE_TX0_UNDERFLOW		0x00002
#define INTR_TYPE_RX0_FULL			0x00004
#define INTR_TYPE_RX0_OVERFLOW		0x00008
#define INTR_TYPE_TX1_EMPTY		 	0x00010
#define INTR_TYPE_TX1_UNDERFLOW	 	0x00020
#define INTR_TYPE_RX1_FULL			0x00040
#define INTR_TYPE_TX2_EMPTY		 	0x00100
#define INTR_TYPE_TX2_UNDERFLOW	 	0x00200
#define INTR_TYPE_RX2_FULL			0x00400
#define INTR_TYPE_TX3_EMPTY		 	0x01000
#define INTR_TYPE_TX3_UNDERFLOW	 	0x02000
#define INTR_TYPE_RX3_FULL			0x04000
#define INTR_TYPE_WKE				0x10000
#define INTR_TYPE_EOWKE				0x20000


/* The SPI duplex communication mode
 */
#define SPI_COMM_TX_RX				0
#define SPI_COMM_RX_ONLY			1
#define SPI_COMM_TX_ONLY			2

/* The structure which maintains the various parameters 
 * of the SPI module. 
 */ 
typedef struct {
	SPIDEV		spi;		/* This has to be the first element */
	uint64_t	pbase;
	uint32_t	sdmapbase;
	uint32_t	clkctrlpbase;
	uintptr_t	vbase;
	uintptr_t	sdmavbase;
	uintptr_t	clkctrlvbase;
	uintptr_t   clkstctrlpbase;
	uintptr_t	clkstctrlvbase;
	int			spi_int;
	int		 	irq_spi;
	int		 	chid, coid;
	int		 	irq_sdma;
	int		 	sdma_coid;
	int		 	sdma_rx_chid;
	int		 	sdma_tx_chid;
	int		 	iid_spi;
	int		 	iid_sdma;
	uint32_t	bitrate;
	uint32_t	clock;
	uint8_t	 	*pbuf;
	int		 	xlen, tlen, rlen;
	int		 	dlen;
	int		 	dtime;	/* usec per data, for time out use */
	uint8_t	 	channel_num;
	uint8_t	 	force;
	uint8_t	 	spi_tx_dma_req;
	uint8_t	 	spi_rx_dma_req;
	int		 	sdma;
	int		 	num_cs;
	dma4_t		*dma4;
	void		*dmabuf;
	uint32_t	pdmabuf;
	struct sigevent		spievent;
	struct sigevent		sdmaevent;
	int		 	cs_delay;
	uintptr_t			mcspi_context_paddr;
	unsigned			mcspi_context_vaddr;
	struct {
		unsigned		captured;
		unsigned		ch_config[NUM_OF_SPI_DEVS];
		unsigned		module_ctrl;
		unsigned		sys_config;
	} state;
} omap4430_spi_t;

#define OMAP4430_SPI_INPUT_CLOCK	48000000
#define OMAP4430_FORCE_ENABLE		1

#define OMAP4430_SPI_SPI1_INTR		65 
#define OMAP4430_SPI_SPI2_INTR		66
#define OMAP4430_SPI_SPI3_INTR		91
#define OMAP4430_SPI_SPI4_INTR		48 

#define OMAP4430_SPI1_BASE			0x48098000
#define OMAP4430_SPI2_BASE			0x4809A000
#define OMAP4430_SPI3_BASE			0x480B8000
#define OMAP4430_SPI4_BASE			0x480BA000

#define OMAP4430_SPI_REGLEN			0x2000
#define OMAP4430_SPI_RXFIFOLEN		16 /* Half of the available FIFO for transmit/receive */
#define OMAP4430_SPI_TXFIFOLEN		16 /* Half of the available FIFO for transmit/receive */

#define OMAP4430_SPI_REV			0x00
#define OMAP4430_SPI_SCR			0x10
#define OMAP4430_SPI_SSR			0x14
#define OMAP4430_SPI_ISR			0x18
#define OMAP4430_SPI_IER			0x1C
#define OMAP4430_SPI_SET1			0x24
#define OMAP4430_SPI_SET2			0x28
#define OMAP4430_SPI_CTRL			0x2c
#define OMAP4430_SPI_DSR			0x30
#define OMAP4430_SPI_TX		 		0x34
#define OMAP4430_SPI_RX		 		0x38
#define OMAP4430_SPI_TEST			0x3C

#define OMAP4430_SPI_CEN_0			0
#define OMAP4430_SPI_CEN_1			1
#define OMAP4430_SPI_CEN_2			2
#define OMAP4430_SPI_CEN_3			3
#define OMAP4430_SPI_CEN_4			4

#define OMAP4430_SPI_CIN_LOW		0
#define OMAP4430_SPI_CIN_HIGH		1

#define OMAP4430_SPI_CPOL_LOW		0
#define OMAP4430_SPI_CPOL_HIGH		1	

#define OMAP4430_SPI_CPHA_MID		0
#define OMAP4430_SPI_CPHA_START		1	

/* Misc Defines */
#define OMAP4430_SPI_MSK0_ENABLE	(1 << 0)
#define OMAP4430_SPI_RE_STATUS		(1 << 0)
#define OMAP4430_SPI_RESET			(1 << 1)
#define OMAP4430_SPI_MASTER			(1 << 15)
#define OMAP4430_SPI_32BIT			0x1F
#define OMAP4430_SPI_DELAY			1000

#define OMAP4430_SPI_DEF_CEN		OMAP4430_SPI_CEN_0
#define OMAP4430_SPI_DEF_CIN		OMAP4430_SPI_CIN_LOW
#define OMAP4430_SPI_DEF_CPOL		OMAP4430_SPI_CPOL_LOW
#define OMAP4430_SPI_DEF_CPHA		OMAP4430_SPI_CPHA_MID


#define OMAP4430_SPI_MASTER_ONE		0x01
#define OMAP4430_SPI_MASTER_TWO		0x02
#define OMAP4430_SPI_MASTER_THREE	0x03
#define OMAP4430_SPI_MASTER_FOUR	0x04

#define OMAP4430_SPI_MASTER_CHANNEL_ONE		0x00
#define OMAP4430_SPI_MASTER_CHANNEL_TWO		0x01
#define OMAP4430_SPI_MASTER_CHANNEL_THREE	0x02
#define OMAP4430_SPI_MASTER_CHANNEL_FOUR	0x03

/* Module System Configuration Register */
#define OMAP4430_MCSPI_SYS_CONFIG			0x10
#define OMAP4430_MCSPI_CONFIG_SOFT_RESET	0x02

/* Module System Configuration Status Register */
#define OMAP4430_MCSPI_SYS_CONFIG_STATUS	0x14
#define OMAP4430_MCSPI_CONFIG_SOFT_RESET_DONE	0x01

/* Module Interrupt Status Register */
#define OMAP4430_MCSPI_IRQ_STATUS_OFFSET	0x18
#define OMAP4430_MCSPI_IRQ_STATUS_RESET_ALL	0x0003777F
#define OMAP4430_MCSPI_IRQ_RESET_CHANNEL(id)	(0xF << (id * 4))
#define OMAP4430_MCSPI_IRQ_RESET_CHANNEL1	0x000F
#define OMAP4430_MCSPI_IRQ_RESET_CHANNEL2	0x0070
#define OMAP4430_MCSPI_IRQ_RESET_CHANNEL3	0x0700
#define OMAP4430_MCSPI_IRQ_RESET_CHANNEL4	0x7000

/* Module Interrupt Enable Register */
#define OMAP4430_MCSPI_IRQ_ENABLE_OFFSET	0x1C

/* Module Control Register */
#define OMAP4430_MCSPI_MODCTRL_OFFSET		0x28
#define OMAP4430_MCSPI_MODCTRL_MASTER_SEL	0xFFFFFFFB
#define OMAP4430_MCSPI_MODCTRL_SINGLE_CH_SEL 0x00000001

/* Module Channel Configuration register */
#define OMAP4430_MCSPI_CH1_CONFIG_OFFSET	0x2C
#define OMAP4430_MCSPI_CH2_CONFIG_OFFSET	0x40
#define OMAP4430_MCSPI_CH3_CONFIG_OFFSET	0x54
#define OMAP4430_MCSPI_CH4_CONFIG_OFFSET	0x68
#define OMAP4430_MCSPI_IS_SOMI_INPUT		0xFFFBFFFF
#define OMAP4430_MCSPI_IS_SOMI_INPUT_D1	 	0x00040000
#define OMAP4430_MCSPI_DPE0_SIMO_OUTPUT		0xFFFEFFFF
#define OMAP4430_MCSPI_DPE1_SIMO_OUTPUT		0xFFFDFFFF
#define OMAP4430_MCSPI_DPE0_TX_DISABLE		0x00010000
#define OMAP4430_MCSPI_DPE1_TX_DISABLE		0x00020000
#define OMAP4430_MCSPI_POLARIY				0x00000002
#define OMAP4430_MCSPI_PHASE				0x00000001
#define OMAP4430_MCSPI_CSPOL_ACTIVE_LOW		0x00000040
#define OMAP4430_MCSPI_TRMODE_TXRX			0x00000000
#define OMAP4430_MCSPI_TRMODE_RXONLY		0x00001000
#define OMAP4430_MCSPI_TRMODE_TXONLY		0x00002000
#define OMAP4430_MCSPI_FORCE_MODE_ONE		0x00100000
#define OMAP4430_MCSPI_EPOL_ONE				0x00000040
#define OMAP4430_MCSPI_FFER					(1<<28)
#define OMAP4430_MCSPI_FFEW					(1<<27)
#define OMAP4430_MCSPI_DMAR					(1<<15)
#define OMAP4430_MCSPI_DMAW					(1<<14)
#define OMAP4430_MCSPI_CS_DELAY_MASK		0xF9FFFFFF
#define OMAP4430_MCSPI_CS_DELAY_ONE_HALF	0x00000000
#define OMAP4430_MCSPI_CS_DELAY_ONE_AND_A_HALF	0x02000000

/* Module Channel Status register */
#define OMAP4430_MCSPI_CH1_STATUS_OFFSET	0x30
#define OMAP4430_MCSPI_CH2_STATUS_OFFSET	0x44
#define OMAP4430_MCSPI_CH3_STATUS_OFFSET	0x58
#define OMAP4430_MCSPI_CH4_STATUS_OFFSET	0x6C
#define	OMAP4430_MCSPI_CH_TX_REG_EMPTY		0x02
#define	OMAP4430_MCSPI_CH_RX_REG_FULL		0x01
#define	OMAP4430_MCSPI_CH_TXFIFO_REG_FULL	0x10
#define	OMAP4430_MCSPI_CH_RXFIFO_REG_FULL	0x40


/* Module Channel Control register */
#define OMAP4430_MCSPI_CH1_CTRL_OFFSET		0x34
#define OMAP4430_MCSPI_CH2_CTRL_OFFSET		0x48
#define OMAP4430_MCSPI_CH3_CTRL_OFFSET		0x5C
#define OMAP4430_MCSPI_CH4_CTRL_OFFSET		0x70
#define OMAP4430_MCSPI_CHANNEL_ENABLE		0x01	

/* Module Channel Tx buffer register */
#define OMAP4430_MCSPI_CH1_TX_BUFFER_OFFSET	0x38
#define OMAP4430_MCSPI_CH2_TX_BUFFER_OFFSET	0x4C
#define OMAP4430_MCSPI_CH3_TX_BUFFER_OFFSET	0x60
#define OMAP4430_MCSPI_CH4_TX_BUFFER_OFFSET	0x74

/* Module Channel Rx buffer register */
#define OMAP4430_MCSPI_CH1_RX_BUFFER_OFFSET	0x3C
#define OMAP4430_MCSPI_CH2_RX_BUFFER_OFFSET	0x50
#define OMAP4430_MCSPI_CH3_RX_BUFFER_OFFSET	0x64
#define OMAP4430_MCSPI_CH4_RX_BUFFER_OFFSET	0x78

/*MCSPI_XFERLEVEL	Registers */
#define OMAP4430_MCSPI_XFERLEVEL_OFFSET				0x7C
#define OMAP4430_MCSPI_XFERLEVEL_OFFSET_AFL_MASK	(0x3F<<8)
#define OMAP4430_MCSPI_XFERLEVEL_OFFSET_AEL_MASK	0x3F

extern void set_port(unsigned port, unsigned mask, unsigned data);
extern void set_port(unsigned port, unsigned mask, unsigned data);

extern void *omap4430_init(void *hdl, char *options);
extern int omap4430_attach_intr(omap4430_spi_t *dev);
extern void omap4430_dinit(void *hdl);
extern int omap4430_drvinfo(void *hdl, spi_drvinfo_t *info);
extern int omap4430_devinfo(void *hdl, uint32_t device, spi_devinfo_t *info);
extern int omap4430_setcfg(void *hdl, uint16_t device, spi_cfg_t *cfg);
extern int omap4430_wait(omap4430_spi_t *dev, int len);
extern void *omap4430_xfer(void *hdl, uint32_t device, uint8_t *buf, int *len);
extern int omap4430_dmaxfer(void *hdl, uint32_t device, spi_dma_paddr_t *paddr, int len);
extern int omap4430_cfg(void *hdl, spi_cfg_t *cfg);

extern int omap4430_setup_sdma(omap4430_spi_t *omap4430, int device, spi_dma_paddr_t *paddr, int len);
extern int omap4430_init_sdma(omap4430_spi_t *omap4430);
extern void omap4430_sdma_disablespi(omap4430_spi_t *omap4430);
extern paddr_t mphys(void *);
extern void omap4430_sdma_detach(omap4430_spi_t *omap4430);
extern int32_t omap4430_sdma_attach(omap4430_spi_t *omap4430);
extern int omap4430_sdma_attach_intr(omap4430_spi_t *dev);
extern int omap_reg_map_init(omap4430_spi_t* dev);
#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/omap4430/omap4430spi.h $ $Rev: 738420 $")
#endif
