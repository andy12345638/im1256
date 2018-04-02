#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <mysql.h>
//gcc ireader2.c -O3 -lmysqlclient -I/usr/include/mysql/
//01 03 1c 
//2d ea 00 4d 00 04 00 00 00 0e 01 c7 17 75 
//2d d5 00 4c 00 04 00 00 00 13 01 cd 17 76 
//d1 a7 



//crc http://mcommit.hatenadiary.com/entry/2015/04/08/224244
unsigned short calc_crc(unsigned char *buf, int length) {
	unsigned short crc = 0xFFFF;
	int i,j;
	unsigned char LSB;
	for (i = 0; i < length; i++) {
		crc ^= buf[i];
		for (j = 0; j < 8; j++) {
			LSB= crc & 1;
			crc = crc >> 1;
			if (LSB) {
				crc ^= 0xA001;
  			}
 		}
	}
	return ((crc & 0xFF00) >> 8)|((crc & 0x0FF) << 8 );
}

int set_interface_attribs(int fd, int speed)
{
	struct termios tty;

	if (tcgetattr(fd, &tty) < 0) {
		printf("Error from tcgetattr: %s\n", strerror(errno));
		return -1;
	}

	cfsetospeed(&tty, (speed_t)speed);
	cfsetispeed(&tty, (speed_t)speed);

	tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;         /* 8-bit characters */
	tty.c_cflag &= ~PARENB;     /* no parity bit */
	tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
	tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

	/* setup for non-canonical mode */
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty.c_oflag &= ~OPOST;

	/* fetch bytes as they become available */
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 1;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		printf("Error from tcsetattr: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}



int ireader (int fd,unsigned char * record){
	int wlen;
	unsigned char buffer[174];
	unsigned char   *p = &buffer[0];
	int rlength = 0;
	int length;
	unsigned short crc;


	wlen = write(fd, "\x01\x03\x00\x40\x00\x54\x45\xE1", 8);//12

	printf("sent 01 03 0040 0054 45 E1\n");//read v
	if (wlen != 8) {
		printf("Error from write: %d, %d\n", wlen, errno);
	}
	tcdrain(fd);    /* delay for output */
	do
	{	
		length = read(fd, buffer + rlength, 1);
		rlength += length;

	} while(rlength < 173);


	printf("%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X \n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],buffer[8],buffer[9],buffer[10],buffer[11],buffer[12],buffer[13],buffer[14],buffer[15],buffer[16],buffer[17],buffer[18],buffer[19]);
	crc=(((short)p[172]) << 8) | p[173];

	if (buffer[0] != '\x01' || calc_crc(buffer,172) != crc){
		printf("CRC error! \n");
		return -1;
	}
	buffer[rlength] = '\0';

	memcpy(record,buffer,  174);
		//printf("%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X \n",record[0],record[1],record[2],record[3],record[4],record[5],record[6],record[7],record[8],record[9],record[10],record[11],record[12],record[13],record[14],record[15],record[16],record[17],record[18],record[19]);

	printf("crc1:%04x \n",calc_crc(buffer,172));
	printf("crc2:%04x \n",crc);
	tcflush(fd, TCIFLUSH);
	return 0; 
}

int decode (int num,float * v,float * a,float * w,float * wh,float * pf,float * fq,unsigned char * record){
	int locate = 0;
	locate = 14*(num-1)+3;
	*v=(((short)record[locate])<<8 |(short)record[locate+1])/100.0;
	*a=(((short)record[locate+2])<<8 |(short)record[locate+3])/1000.0;
	*w=(((short)record[locate+4])<<8 |(short)record[locate+5]);
	*wh=((short)record[locate+6]<< 24 |(short)record[locate+7]<< 16 |(short)record[locate+8]<< 8 |(short)record[locate+9])*10;
	*pf=((short)record[locate+10]<< 8 |(short)record[locate+11])/1000.0;
	*fq=((short)record[locate+12]<< 8 |(short)record[locate+13])/100.0;
}


int main()
{
	int fdp1;
	int i;
	unsigned char Record[174]="0";
	float V[12];
	float A[12];
	float W[12];
	float WH[12];
	float PF[12];
	float FQ[12];

	int num = 1;
	printf(" Default Port: /dev/ttyUSB0 \n");
	fdp1 = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_SYNC);

	if (fdp1 < 0) {
		printf("Error opening %s: %s\n", "/dev/ttyUSB0", strerror(errno));
	return -1;
	}
	/*baudrate 115200, 8 bits, no parity, 1 stop bit */
	set_interface_attribs(fdp1, B9600);

	tcflush(fdp1, TCIOFLUSH);
	usleep(700);
	//printf("%u\n",ireader(fdp1,Record));
	if (ireader(fdp1,Record) < 0) {
		printf("Error Recording, %s\n",  strerror(errno));
	return -1;
	}
	close(fdp1);

	printf("%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X \n",Record[0],Record[1],Record[2],Record[3],Record[4],Record[5],Record[6],Record[7],Record[8],Record[9],Record[10],Record[11],Record[12],Record[13],Record[14],Record[15],Record[16],Record[17],Record[18],Record[19]);



	decode(1,&V[0],&A[0],&W[0],&WH[0],&PF[0],&FQ[0],Record);
	printf("V:%02.2f\n",V[0]);
	printf("A:%02.2f\n",A[0]);
	printf("W:%02.0f\n",W[0]);
	printf("WH:%02.0f\n",WH[0]);
	printf("PF:%02.2f\n",PF[0]);
	printf("FQ:%02.2f\n",FQ[0]);

	decode(2,&V[1],&A[1],&W[1],&WH[1],&PF[1],&FQ[1],Record);
	printf("V:%02.2f\n",V[1]);
	printf("A:%02.2f\n",A[1]);
	printf("W:%02.0f\n",W[1]);
	printf("WH:%02.0f\n",WH[1]);
	printf("PF:%02.2f\n",PF[1]);
	printf("FQ:%02.2f\n",FQ[1]);

	for (i=1; i<=12; i++) {
	decode(i,&V[i-1],&A[i-1],&W[i-1],&WH[i-1],&PF[i-1],&FQ[i-1],Record);
	}

	MYSQL *mysql = NULL;
	mysql = mysql_init(mysql);
	if(!mysql_real_connect(mysql,"127.0.0.1","root","1234","ireader",3306,NULL,0))
	printf("MYSQL Init Error\n");
	else
	printf("MYSQL Init OK\n");

	char query[2000];
	sprintf(query,"INSERT INTO powerdata(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,w1,w2,w3,w4,w5,w6,w7,w8,w9,w10,w11,w12,wh1,wh2,wh3,wh4,wh5,wh6,wh7,wh8,wh9,wh10,wh11,wh12,pf1,pf2,pf3,pf4,pf5,pf6,pf7,pf8,pf9,pf10,pf11,pf12,fq1,fq2,fq3,fq4,fq5,fq6,fq7,fq8,fq9,fq10,fq11,fq12)VALUES(%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f)",V[0],V[1],V[2],V[3],V[4],V[5],V[6],V[7],V[8],V[9],V[10],V[11],A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],W[0],W[1],W[2],W[3],W[4],W[5],W[6],W[7],W[8],W[9],W[10],W[11],WH[0],WH[1],WH[2],WH[3],WH[4],WH[5],WH[6],WH[7],WH[8],WH[9],WH[10],WH[11],PF[0],PF[1],PF[2],PF[3],PF[4],PF[5],PF[6],PF[7],PF[8],PF[9],PF[10],PF[11],FQ[0],FQ[1],FQ[2],FQ[3],FQ[4],FQ[5],FQ[6],FQ[7],FQ[8],FQ[9],FQ[10],FQ[11]);

	printf(query);
	if(mysql_query(mysql,query))
	printf("MYSQL Query Error!\n");
	else
	printf("MYSQL Query Done!\n");

	return 0;



}

