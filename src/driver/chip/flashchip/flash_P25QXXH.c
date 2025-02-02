/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "driver/chip/flashchip/flash_chip.h"
#include "sys/param.h"
#include "../hal_base.h"
#include "sys/xr_debug.h"

#define FLASH_DEBUG(fmt, arg...)    XR_DEBUG((DBG_OFF | XR_LEVEL_ALL), NOEXPAND, "[Flash chip DBG] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define FLASH_ALERT(fmt, arg...)    XR_ALERT((DBG_ON | XR_LEVEL_ALL), NOEXPAND, "[Flash chip ALT] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define FLASH_ERROR(fmt, arg...)    XR_ERROR((DBG_ON | XR_LEVEL_ALL), NOEXPAND, "[Flash chip ERR] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define FLASH_NOWAY()               XR_ERROR((DBG_ON | XR_LEVEL_ALL), NOEXPAND, "[Flash chip should not be here] <%s : %d> \n", __func__, __LINE__)
#define FLASH_NOTSUPPORT()          FLASH_ALERT("not support CMD")
#define PCHECK(p)

#define P25Q40H_JEDEC 0x136085
#define P25Q80H_JEDEC 0x146085
#define P25Q16H_JEDEC 0x156085
#define P25Q64H_JEDEC 0x176085

typedef enum {
	FLASH_INSTRUCTION_RDSR  = 0x05,    /* read status register */
	FLASH_INSTRUCTION_RDSR2 = 0x35,    /* read status register-1 */
	FLASH_INSTRUCTION_RDCR  = 0x15,    /* read configure register */
	FLASH_INSTRUCTION_WRSR  = 0x01,    /* write status register, P25Q64H 1 byte, other 2 byte */
#ifdef FLASH_P25Q64H
	FLASH_INSTRUCTION_WRSR2 = 0x31,    /* write status register-1  1 byte*/
	FLASH_INSTRUCTION_WRCR  = 0x11,    /* write configure register */
#else
	FLASH_INSTRUCTION_WRCR  = 0x31,    /* write configure register */
#endif
} eSF_Instruction;

/* internal macros for flash chip instruction */
#define FCI_CMD(idx)    instruction[idx]
#define FCI_ADDR(idx)   instruction[idx]
#define FCI_DUMMY(idx)  instruction[idx]
#define FCI_DATA(idx)   instruction[idx]

static int P25QXXH_WriteStatus(struct FlashChip *chip, FlashStatus reg, uint8_t *status)
{
	int ret;
	uint8_t status_buf[2];
	InstructionField instruction[2];

	PCHECK(chip);

	if (!(reg & chip->cfg.mWriteStatusSupport)) {
		FLASH_NOTSUPPORT();
		return HAL_INVALID;
	}
/*
	HAL_Memset(&instruction, 0, sizeof(instruction));
	FCI_CMD(0).data = FLASH_INSTRUCTION_SRWREN;
	FCI_CMD(0).line = 1;
	chip->driverWrite(chip, &FCI_CMD(0), NULL, NULL, NULL);
*/

	HAL_Memset(&instruction, 0, sizeof(instruction));

	if (reg == FLASH_STATUS1) {
#ifndef FLASH_P25Q64H
		FCI_CMD(0).data = FLASH_INSTRUCTION_RDSR2;
		FCI_CMD(0).line = 1;

		FCI_DATA(1).pdata = (uint8_t *)&status_buf[1];
		FCI_DATA(1).len = 1;
		FCI_DATA(1).line = 1;

		chip->driverRead(chip, &FCI_CMD(0), NULL, NULL, &FCI_DATA(1));
#endif
		status_buf[0] = *status;

		FCI_CMD(0).data = FLASH_INSTRUCTION_WRSR;

		FCI_DATA(1).pdata = status_buf;
		FCI_DATA(1).len = 2;
		FCI_DATA(1).line = 1;

		//printf("FLASH_STATUS1\n");
		//printf("SR1:0x%02x\n", status_buf[0]);
		//printf("SR2:0x%02x\n", status_buf[1]);
	} else if (reg == FLASH_STATUS2) {
#ifndef FLASH_P25Q64H
		FCI_CMD(0).data = FLASH_INSTRUCTION_RDSR;
		FCI_CMD(0).line = 1;

		FCI_DATA(1).pdata = (uint8_t *)status_buf;
		FCI_DATA(1).len = 1;
		FCI_DATA(1).line = 1;

		chip->driverRead(chip, &FCI_CMD(0), NULL, NULL, &FCI_DATA(1));

		status_buf[1] = *status;

		FCI_CMD(0).data = FLASH_INSTRUCTION_WRSR;

		FCI_DATA(1).pdata = status_buf;
		FCI_DATA(1).len = 2;
		FCI_DATA(1).line = 1;

		//printf("FLASH_STATUS2\n");
		//printf("SR1:0x%02x\n", status_buf[0]);
		//printf("SR2:0x%02x\n", status_buf[1]);
#else
	status_buf[0] = *status;

	FCI_CMD(0).data = FLASH_INSTRUCTION_WRSR2;

	FCI_DATA(1).pdata = &status_buf[0];
	FCI_DATA(1).len = 1;
	FCI_DATA(1).line = 1;
#endif
	} else if (reg == FLASH_STATUS3) {
		FCI_CMD(0).data = FLASH_INSTRUCTION_WRCR;

		FCI_DATA(1).pdata = (uint8_t *)status;
		FCI_DATA(1).len = 1;
		FCI_DATA(1).line = 1;
	} else {
		FLASH_NOWAY();
	}

	chip->writeEnable(chip);

	ret = chip->driverWrite(chip, &FCI_CMD(0), NULL, NULL, &FCI_DATA(1));

	chip->writeDisable(chip);

/*
	while (chip->isBusy(chip)) {
		//printf("busy...\n");
	}
*/
	return ret;
}

static int P25QXXH_FlashInit(struct FlashChip *chip)
{
	PCHECK(chip);

	chip->writeEnable = defaultWriteEnable;
	chip->writeDisable = defaultWriteDisable;
	chip->readStatus = defaultReadStatus;
	chip->erase = defaultErase;
	chip->jedecID = defaultGetJedecID;
	chip->pageProgram = defaultPageProgram;
	chip->read = defaultRead;

	chip->driverWrite = defaultDriverWrite;
	chip->driverRead = defaultDriverRead;
	chip->xipDriverCfg = defaultXipDriverCfg;
	chip->setFreq = defaultSetFreq;
	chip->switchReadMode = defaultSwitchReadMode;
	chip->enableXIP = defaultEnableXIP;
	chip->disableXIP = defaultDisableXIP;
	chip->isBusy = defaultIsBusy;
	chip->control = defaultControl;
	chip->minEraseSize = defaultGetMinEraseSize;
	//chip->writeStatus = defaultWriteStatus;
	chip->writeStatus = P25QXXH_WriteStatus;
	chip->enableQPIMode = defaultEnableQPIMode;
	chip->disableQPIMode = defaultDisableQPIMode;
	//chip->enableReset = defaultEnableReset;
	chip->reset = defaultReset;

	chip->suspendErasePageprogram = NULL;
	chip->resumeErasePageprogram = NULL;
	chip->powerDown = NULL;
	chip->releasePowerDown = NULL;
	chip->uniqueID = NULL;
	/*TODO: a NULL interface for showing invalid interface*/

	FLASH_DEBUG("P25QXXH_Flash inited");

	return 0;
}

static int P25QXXH_FlashDeinit(struct FlashChip *chip)
{
	PCHECK(chip);

	//HAL_Free(chip);

	return 0;
}

static struct FlashChip *P25QXXH_FlashCtor(struct FlashChip *chip, uint32_t arg)
{
	uint32_t jedec = arg;
	uint32_t size;
	PCHECK(chip);

	if (jedec == P25Q80H_JEDEC) {
		size = 16 * 16 * 0x1000;
	} else if (jedec == P25Q40H_JEDEC) {
		size = 16 * 8 * 0x1000;
	} else if (jedec == P25Q16H_JEDEC) {
		size = 32 * 16 * 0x1000;
	} else if (jedec == P25Q64H_JEDEC) {
		size = 8 * 1024 * 1024;
	} else {
		return NULL;
	}

	/* TODO: use HAL_Memcpy() and const to init chip->cfg to save code size */
	chip->cfg.mJedec = jedec;
	chip->cfg.mSize = size;

	if (jedec == P25Q64H_JEDEC) {
		chip->cfg.mMaxFreq = 96 * 1000 * 1000; //P25Q64H_JEDEC  2.3~3.6  96M
	} else {
		chip->cfg.mMaxFreq = 104 * 1000 * 1000;
	}
	chip->cfg.mMaxReadFreq = 55 * 1000 * 1000;
	chip->cfg.mEraseSizeSupport = FLASH_ERASE_64KB | FLASH_ERASE_32KB | FLASH_ERASE_4KB | FLASH_ERASE_CHIP;
	chip->cfg.mPageProgramSupport = FLASH_PAGEPROGRAM | FLASH_QUAD_PAGEPROGRAM;
	chip->cfg.mReadStausSupport = FLASH_STATUS1 | FLASH_STATUS2 | FLASH_STATUS3;
	chip->cfg.mWriteStatusSupport = FLASH_STATUS1 | FLASH_STATUS2 | FLASH_STATUS3;//
	chip->cfg.mReadSupport = FLASH_READ_NORMAL_MODE | FLASH_READ_FAST_MODE | FLASH_READ_DUAL_O_MODE
	             | FLASH_READ_DUAL_IO_MODE | FLASH_READ_QUAD_O_MODE | FLASH_READ_QUAD_IO_MODE;
	if (jedec == P25Q64H_JEDEC) {
		chip->cfg.mReadSupport = chip->cfg.mReadSupport | FLASH_READ_QPI_MODE; //P25Q64H_JEDEC, qpi
	}
	chip->mPageSize = 256;
	chip->mFlashStatus = 0;
	chip->mDummyCount = 1;

	return chip;
}

FlashChipCtor  P25Q64H_FlashChip = {
	.mJedecId = P25Q64H_JEDEC,
	.enumerate = P25QXXH_FlashCtor,
	.init = P25QXXH_FlashInit,
	.destory = P25QXXH_FlashDeinit,
};

