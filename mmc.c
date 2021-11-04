#include <mmc.h>


void dcs() {
	sspi_cs(0);
	sspi_write(0xFF);
}
void cs() {
	sspi_cs(1);
}
void __delay_ms(uint16_t _m) {
	uint64_t ts = get_timestamp();
	while(!delay_ms(ts, _m));
}
int mmc_cmd(char cmd,unsigned long int arg, uint8_t _crc){
	char r = 0xff;
	int d = 0;
	sspi_write(cmd | 0x40);
	sspi_write(arg >> 24);
	sspi_write(arg >> 16);
	sspi_write(arg >> 8);
	sspi_write(arg);
	sspi_write(_crc);
	do {
		r = sspi_write(0xFF);
		d++;
		if (d == 1000) {
			break;
		}
	} while (r == 0xff);
	return r;	
}
int mmc_init() {
	int cnt = 0;
	uint8_t r;
	dcs();
	for (cnt = 0; cnt < 100; cnt++) sspi_write(0xFF);
	__delay_ms(10);
	sspi_write(0xFF);
	cs();
	if (mmc_cmd(0, 0, 0x95) != 1) {
		dcs();
		return -1;
	}
	dcs();
	sspi_write(0xFF);
	cs();

	r = mmc_cmd(8, 0x000001AA, 0x87);
	if (r == 1) {
		 sspi_write(0xFF);
		 sspi_write(0xFF);
		 sspi_write(0xFF);
		 sspi_write(0xFF);
		 sspi_write(0xFF);
		 sspi_write(0xFF);
	} else {
		dcs();
		return -1;
	}
	dcs();
	__delay_ms(10);
	 sspi_write(0xFF);
	 cs();

	 while(1) {
		 cs();
		if (mmc_cmd(55, 0, 0) != 1) {
			dcs();
			return -1;
		}
		dcs();
		__delay_ms(1);
		cs();
		if (mmc_cmd(41, 0x40000000, 1) == 0x00) {
			break;
		}
		dcs();
		__delay_ms(1);
		cnt++;
		if (cnt >= 1000) {
			dcs();
			return -1;
		}
	 }
	 dcs();
	__delay_ms(10);
	sspi_write(0xFF);
	cs();
	mmc_cmd(59, 0, 0);
	sspi_write(0xFF);
	mmc_cmd(16, 512, 0);
	dcs();
	return 1;
}
int8_t mmc_write(unsigned long int sector,char *buffer){
	char r;
	int i = 0;
	sspi_write(0xFF);

	cs();
	r = mmc_cmd(MMC_WRITE_BLOCK, sector, 0xFF);
	if (r != 0) {
		return -1;
	}

	r = sspi_write(0xFF);
	r = sspi_write(0xFF);
	r = sspi_write(0xFF);

	sspi_write(MMC_STARTBLOCK_WRITE);
	for (i = 0; i < 512; i++) sspi_write(*buffer++);
	sspi_write(0xff);
	sspi_write(0xff);

	while(1) {
		r = sspi_write(0xff);
		if (r != 0xFF) break;
	}

	if ((r & MMC_DR_MASK) != MMC_DR_ACCEPT) {
		dcs();
		return -1;
	}

    while(sspi_write(0xFF) != 0xFF);
	dcs();
	sspi_write(0xFF);
	return 1;
}
int8_t mmc_read(unsigned long int sector, char *buffer) {
	char r1;
	int i = 0;
	cs();
	r1 = mmc_cmd(MMC_READ_SINGLE_BLOCK, sector, 1);
	if (r1 != 0x00) return -1;
	while (sspi_write(0xFF) != MMC_STARTBLOCK_READ);
	for (i = 0; i < 512; i++) {
		r1 = sspi_write(0xFF);
		buffer[i] = r1;
	}
	sspi_write(0xff);
	sspi_write(0xff);
	dcs();
	return 1;
}
