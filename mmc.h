/*
 * mmc.h
 *
 *  Created on: Aug 4, 2021
 *      Author: Duman
 */

#ifndef MMC_H_
#define MMC_H_

#include <def.h>
#include <delay.h>
#include <sspi.h>

#define MMC_GO_IDLE_STATE                   0                ///< initialize card to SPI-type access
#define MMC_SEND_OP_COND               		1                ///< set card operational mode
#define MMC_SEND_CSD                        9                ///< get card's CSD
#define MMC_SEND_CID                        10                ///< get card's CID
#define MMC_SEND_STATUS                     13
#define MMC_SET_BLOCKLEN                	16                ///< Set number of bytes to transfer per block
#define MMC_READ_SINGLE_BLOCK               17                ///< read a block
#define MMC_WRITE_BLOCK                     24                ///< write a block
#define MMC_PROGRAM_CSD                     27
#define MMC_SET_WRITE_PROT                	28
#define MMC_CLR_WRITE_PROT                	29
#define MMC_SEND_WRITE_PROT                	30
#define MMC_TAG_SECTOR_START                32
#define MMC_TAG_SECTOR_END                	33
#define MMC_UNTAG_SECTOR                	34
#define MMC_TAG_ERASE_GROUP_START         	35                ///< Sets beginning of erase group (mass erase)
#define MMC_TAG_ERARE_GROUP_END             36                ///< Sets end of erase group (mass erase)
#define MMC_UNTAG_ERASE_GROUP               37                ///< Untag (unset) erase group (mass erase)
#define MMC_ERASE                        	38                ///< Perform block/mass erase
#define MMC_CRC_ON_OFF                      59                ///< Turns CRC check on/off
// R1 Response bit-defines
#define MMC_R1_BUSY                 	    0x80        ///< R1 response: bit indicates card is busy
#define MMC_R1_PARAMETER           			0x40
#define MMC_R1_ADDRESS                      0x20
#define MMC_R1_ERASE_SEQ                	0x10
#define MMC_R1_COM_CRC                      0x08
#define MMC_R1_ILLEGAL_COM                	0x04
#define MMC_R1_ERASE_RESET                	0x02
#define MMC_R1_IDLE_STATE                	0x01
// Data Start tokens
#define MMC_STARTBLOCK_READ                 0xFE        ///< when received from card, indicates that a block of data will follow
#define MMC_STARTBLOCK_WRITE                0xFE        ///< when sent to card, indicates that a block of data will follow
#define MMC_STARTBLOCK_MWRITE               0xFC
// Data Stop tokens
#define MMC_STOPTRAN_WRITE                	0xFD
// Data Error Token values
#define MMC_DE_MASK                         0x1F
#define MMC_DE_ERROR                        0x01
#define MMC_DE_CC_ERROR                     0x02
#define MMC_DE_ECC_FAIL                     0x04
#define MMC_DE_OUT_OF_RANGE                 0x04
#define MMC_DE_CARD_LOCKED                  0x04
// Data Response Token values
#define MMC_DR_MASK                         0x1F
#define MMC_DR_ACCEPT                       0x05
#define MMC_DR_REJECT_CRC                	0x0B
#define MMC_DR_REJECT_WRITE_ERROR        	0x0D

int mmc_init();
int8_t mmc_write(unsigned long int sector,char *buffer);
int8_t mmc_read(unsigned long int sector, char *buffer);
char mmcBuf[512];

#endif /* MMC_H_ */
