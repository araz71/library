#include <mifaire_rfid.h>

//key default A and B
uint8_t keyA_default[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t keyB_default[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

/*
initialize rc522
*/

uint8_t spi_transmit(char _ch){

	uint8_t x = spi_write(SPI2, _ch);
//	usart_putch(USART2, x);
	return x;
}

void ENABLE_CHIP(){

	pin_clr(SPI2_CS_PORT, SPI2_CS_PIN);
}

void DISABLE_CHIP(){

	pin_set(SPI2_CS_PORT, SPI2_CS_PIN);
}

void mfrc522_init()
{
	uint8_t byte;
	mfrc522_reset();
	
	mfrc522_write(TModeReg, 0x8D);
    mfrc522_write(TPrescalerReg, 0x3E);
    mfrc522_write(TReloadReg_1, 30);
    mfrc522_write(TReloadReg_2, 0);
	mfrc522_write(TxASKReg, 0x40);
	mfrc522_write(ModeReg, 0x3D);

    // antenna on
	byte = mfrc522_read(TxControlReg);
	if(!(byte&0x03))
	{
		mfrc522_write(TxControlReg,byte|0x03);
	}
}

/*
write data to rc522 register
*/
void mfrc522_write(uint8_t reg, uint8_t data)
{
	ENABLE_CHIP();
	spi_transmit((reg<<1)&0x7E);
	spi_transmit(data);
	DISABLE_CHIP();
}

/*
read data from rc522 register
*/
uint8_t mfrc522_read(uint8_t reg)
{
	uint8_t data;
	ENABLE_CHIP();
	spi_transmit(((reg<<1)&0x7E)|0x80);
	data = spi_transmit(0x00);
	DISABLE_CHIP();
	return data;
}

/*
soft reset rc522
*/
void mfrc522_reset()
{
	mfrc522_write(CommandReg,SoftReset_CMD);
}

/*
make command request to rc522
*/
uint8_t	mfrc522_request(uint8_t req_mode, uint8_t * tag_type)
{
	uint8_t  status;
	uint32_t backBits;//The received data bits

	mfrc522_write(BitFramingReg, 0x07);//TxLastBists = BitFramingReg[2..0]	???

	tag_type[0] = req_mode;
	status = mfrc522_to_card(Transceive_CMD, tag_type, 1, tag_type, &backBits);

	if ((status != CARD_FOUND) || (backBits != 0x10))
	{
		status = ERROR;
	}

	return status;
}

/*
send command to rc522 to card
*/
uint8_t mfrc522_to_card(uint8_t cmd, uint8_t *send_data, uint8_t send_data_len, uint8_t *back_data, uint32_t *back_data_len)
{
	uint8_t status = ERROR;
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint8_t	tmp;
    uint32_t i;

    switch (cmd)
    {
        case MFAuthent_CMD:		//Certification cards close
		{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case Transceive_CMD:	//Transmit FIFO data
		{
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default:
			break;
    }

    //mfrc522_write(ComIEnReg, irqEn|0x80);	//Interrupt request
    n=mfrc522_read(ComIrqReg);
    mfrc522_write(ComIrqReg,n&(~0x80));//clear all interrupt bits
    n=mfrc522_read(FIFOLevelReg);
    mfrc522_write(FIFOLevelReg,n|0x80);//flush FIFO data

	mfrc522_write(CommandReg, Idle_CMD);	//NO action; Cancel the current cmd???

	//Writing data to the FIFO
    for (i=0; i<send_data_len; i++)
    {
		mfrc522_write(FIFODataReg, send_data[i]);
	}

	//Execute the cmd
	mfrc522_write(CommandReg, cmd);
    if (cmd == Transceive_CMD)
    {
		n=mfrc522_read(BitFramingReg);
		mfrc522_write(BitFramingReg,n|0x80);
	}   

	//Waiting to receive data to complete
	i = 2000;	//i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
    do
    {
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = mfrc522_read(ComIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

	tmp=mfrc522_read(BitFramingReg);
	mfrc522_write(BitFramingReg,tmp&(~0x80));

    if (i != 0)
    {
        if(!(mfrc522_read(ErrorReg) & 0x1B))	//BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = CARD_FOUND;
            if (n & irqEn & 0x01)
            {
				status = CARD_NOT_FOUND;			//??
			}

            if (cmd == Transceive_CMD)
            {
               	n = mfrc522_read(FIFOLevelReg);
              	lastBits = mfrc522_read(ControlReg) & 0x07;
                if (lastBits)
                {
					*back_data_len = (uint32_t)(n-1)*8 + (uint32_t)lastBits;
				}
                else
                {
					*back_data_len = (uint32_t)n*8;
				}

                if (n == 0)
                {
					n = 1;    
				}
                if (n > MAX_LEN)
                {
					n = MAX_LEN;
				}

				//Reading the received data in FIFO
                for (i=0; i<n; i++)
                {
					back_data[i] = mfrc522_read(FIFODataReg);
				}
            }
        }
        else
        {
			status = ERROR;
		}

    }

    //SetBitMask(ControlReg,0x80);           //timer stops
    //mfrc522_write(cmdReg, PCD_IDLE);

    return status;
}

/*
get card serial
*/
uint8_t mfrc522_get_card_serial(uint8_t * serial_out)
{
	uint8_t status;
    uint8_t i;
	uint8_t serNumCheck=0;
    uint32_t unLen;

	mfrc522_write(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]

    serial_out[0] = PICC_ANTICOLL;
    serial_out[1] = 0x20;
    status = mfrc522_to_card(Transceive_CMD, serial_out, 2, serial_out, &unLen);

    if (status == CARD_FOUND)
	{
		//Check card serial number
		for (i=0; i<4; i++)
		{
		 	serNumCheck ^= serial_out[i];
		}
		if (serNumCheck != serial_out[i])
		{
			status = ERROR;
		}
    }
    return status;
}

/*
set bit mask
*/
void mfrc522_setBitMask(uint8_t reg, uint8_t mask)
{
	uint8_t tmp;
	tmp = mfrc522_read(reg);
	mfrc522_write(reg, tmp | mask);  // set bit mask
}

/*
clear bit mask
*/
void mfrc522_clearBitMask(uint8_t reg, uint8_t mask)
{
	uint8_t tmp;
	tmp = mfrc522_read(reg);
	mfrc522_write(reg, tmp & (~mask));  // clear bit mask
}

/*
calculate crc using rc522 chip
*/
void mfrc522_calculateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData)
{
	uint8_t i, n;

	mfrc522_clearBitMask(DivIrqReg, 0x04);			//CRCIrq = 0
	mfrc522_setBitMask(FIFOLevelReg, 0x80);			//Claro puntero FIFO
	//Write_MFRC522(CommandReg, PCD_IDLE);

	//Escribir datos en el FIFO
	for (i=0; i<len; i++)
	{
		mfrc522_write(FIFODataReg, *(pIndata+i));
	}
	mfrc522_write(CommandReg, PCD_CALCCRC);

	// Esperar a la finalización de cálculo del CRC
	i = 0xFF;
	do
	{
		n = mfrc522_read(DivIrqReg);
		i--;
	}
	while ((i!=0) && !(n&0x04));			//CRCIrq = 1

	//Lea el cálculo de CRC
	pOutData[0] = mfrc522_read(CRCResultReg_2);
	pOutData[1] = mfrc522_read(CRCResultReg_1);
}

/*
halt the card (release it to be able to read again)
*/
uint8_t mfrc522_halt()
{
	uint8_t status;
	uint32_t unLen;
	uint8_t buff[4];

	buff[0] = PICC_HALT;
	buff[1] = 0;
	mfrc522_calculateCRC(buff, 2, &buff[2]);

	mfrc522_clearBitMask(Status2Reg, 0x08); // turn off encryption

	status = mfrc522_to_card(Transceive_CMD, buff, 4, buff,&unLen);

	return status;
}

/*
get reader version
*/
uint8_t mfrc522_get_version()
{
	return mfrc522_read(VersionReg);
}

/*
check if card is in range
*/
uint8_t mfrc522_is_card(uint16_t *card_type)
{
	uint8_t buff_data[MAX_LEN],
    status = mfrc522_request(PICC_REQIDL,buff_data);
    if(status == CARD_FOUND){
        *card_type = (buff_data[0]<<8)+buff_data[1];
        return 1;
    }
    else{
        return 0;
    }
}

/*
 * Function Name : MFRC522_Auth
 * Description : Verify card password
 * Input parameters : authMode - Password Authentication Mode
                 0x60 = A key authentication
                 0x61 = B key authentication
             BlockAddr - block address
             Sectorkey - Sector password
             serNum - card serial number, 4-byte
 * Return value: the successful return CARD_FOUND
 */
uint8_t mfrc522_auth(uint8_t authMode, uint8_t BlockAddr, uint8_t *Sectorkey, uint8_t *serNum)
{
    uint8_t status;
    uint32_t recvBits;
    uint8_t i;
	uint8_t buff[12]; 

    // Validate instruction block address + sector + password + card serial number
    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i=0; i<6; i++)
    {
		buff[i+2] = *(Sectorkey+i);   
	}
    for (i=0; i<4; i++)
    {
		buff[i+8] = *(serNum+i);   
	}
    status = mfrc522_to_card(PCD_AUTHENT, buff, 12, buff, &recvBits);
    i = mfrc522_read(Status2Reg);

    if ((status != CARD_FOUND) || (!(i & 0x08)))
    {
		status = ERROR;
	}

    return status;
}

/*
 * Function Name : MFRC522_Write
 * Description : Write block data
 * Input parameters : blockAddr - block address ; writeData - to 16-byte data block write
 * Return value: the successful return CARD_FOUND
 */
uint8_t mfrc522_write_block(uint8_t blockAddr, uint8_t *writeData)
{
    uint8_t status;
    uint32_t recvBits;
    uint8_t i;
	uint8_t buff[18]; 

    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    mfrc522_calculateCRC(buff, 2, &buff[2]);
    status = mfrc522_to_card(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

    //cek
    //printf("w1 = %d\t%d\t%.2X\n", status, recvBits, buff[0]);

    if ((status != CARD_FOUND) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    {
		status = ERROR;
	}

    if (status == CARD_FOUND)
    {
        for (i=0; i<16; i++)		//?FIFO?16Byte??
        {
        	buff[i] = *(writeData+i);
        }
        mfrc522_calculateCRC(buff, 16, &buff[16]);
        status = mfrc522_to_card(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

        //cek
        //printf("w2 = %d\t%d\t%.2X\n", status, recvBits, buff[0]);

		if ((status != CARD_FOUND) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
        {
			status = ERROR;
		}
    }

    return status;
}

/*
 * Function Name : MFRC522_Read
 * Description : Read block data
 * Input parameters : blockAddr - block address ; recvData - read block data
 * Return value: the successful return MI_OK
 */
uint8_t mfrc522_read_block(uint8_t blockAddr, uint8_t *recvData)
{
    uint8_t status;
    uint32_t unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    mfrc522_calculateCRC(recvData,2, &recvData[2]);
    status = mfrc522_to_card(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

    //cek
//    printf("read block #%d = %.2X %.4X\n", blockAddr, status, unLen);

    if ((status != CARD_FOUND) || (unLen != 0x90))
    {
        status = ERROR;
    }

    return status;
}

/*
 * Function Name : MFRC522_SelectTag
 * Description: election card , read the card memory capacity
 * Input parameters : serNum - Incoming card serial number
 * Return value: the successful return of card capacity
 */
uint8_t mfrc522_select_tag(uint8_t *serNum)
{
    uint8_t i;
	uint8_t status;
	uint8_t size;
    uint32_t recvBits;
    uint8_t buffer[9];

	//ClearBitMask(Status2Reg, 0x08);			//MFCrypto1On=0

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;
    for (i=0; i<5; i++)
    {
    	buffer[i+2] = *(serNum+i);
    }
	mfrc522_calculateCRC(buffer, 7, &buffer[7]);		//??
    status = mfrc522_to_card(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);

    if ((status == CARD_FOUND) && (recvBits == 0x18))
    {
		size = buffer[0];
	}
    else
    {
		size = 0;
	}

    return size;
}

void mfrc522_check_reader()
{
	uint8_t curr_read_status = mfrc522_get_version();
	if (curr_read_status<0x90 || curr_read_status>0x92)
	{
        #if defined(DEBUG_ON)
		    printf("NO READER\n");
        #endif
		for(;;);
	}
}

/*
read card serial id
*/
uint8_t mfrc522_read_card_id(uint8_t *card_id, uint16_t *card_type)
{
	uint8_t status, resx = 0;
	uint8_t buff_data[MAX_LEN];

    *card_type = 0;
	if(mfrc522_is_card(card_type))
	{
		status = mfrc522_get_card_serial(buff_data);
		if (status==CARD_FOUND)
		{
			memcpy(card_id,buff_data,5);//kopi id and checksum at last byte (5th)
			resx = 1;
		}
		else
		{
			resx = 0;
		}
	}
	else
	{
		resx = 0;
	}

	return resx;
}
