/*
 * i2c.c
 *
 * Created: 08/05/2015 00:54:37
 *  Author: David
 */
#include <avr/io.h>
#include <avr/eeprom.h>
#include "uart.h"
#include "twi.h"
#include "tas3103.h"
#include "utility.h"

//Help function for printing bits in uint32_t data
void print_uint32_t(uint32_t data)
{
	int buff[32]={0};
	int i,j=0;
	SerialNewLine();

	for(i=31;i>=0;i--)
	{
		buff[i]=(data>>j)&0x00000001;
		j++;
	}

	for(i=1;i<33;i++)
	{

		if(buff[i-1]==1)
		{SerialSend('1');}
		else
		{SerialSend('0');}

		if((i%4)==0)
		{
			SerialSend(' ');
		}

	}
}
//Functin for saving current configuration of TAS3103 in digital crossover
void save()
{
	char adrese[39]={A_a, A_d, A_e, B_b, B_d, B_e,
			CH1_Bq1, CH1_Bq2, CH1_Bq3, CH1_Bq4, CH1_Bq5, CH1_Bq6,
			CH1_Bq7, CH1_Bq8, CH1_Bq9, CH1_Bq10, CH1_Bq11, CH1_Bq12,
			CH2_Bq1, CH2_Bq2, CH2_Bq3, CH2_Bq4, CH2_Bq5, CH2_Bq6,
			CH2_Bq7, CH2_Bq8, CH2_Bq9, CH2_Bq10, CH2_Bq11, CH2_Bq12,
			z_U,z_V,y_U,y_V, Mute, Vol_CH1, Vol_CH2, Delays, Config};

	int blokovi[39]={1,1,1,1,1,1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
			1,1,1,1,1,1,1,3,1};
	int i;
	uint32_t *data;
	int adr=0;

	SerialSendString("Saving Configuration!",1);
	SerialSendString("0%                                     100%",1);

	for(i=0;i<39;i++)
	{
		data=ReadTAS(adrese[i],blokovi[i]);

		while(!eeprom_is_ready()){}
		eeprom_write_block((const void*)data,(void*)adr,blokovi[i]*sizeof(uint32_t));

		SerialSend('#');

		adr=adr+(blokovi[i]*sizeof(uint32_t));

	}
	SerialNewLine();
	SerialSendString("Saved!",1);
}
//Function for loading configuration of TAS3103 in digital crossover
void init()
{
	char adrese[39]={A_a, A_d, A_e, B_b, B_d, B_e,
			CH1_Bq1, CH1_Bq2, CH1_Bq3, CH1_Bq4, CH1_Bq5, CH1_Bq6,
			CH1_Bq7, CH1_Bq8, CH1_Bq9, CH1_Bq10, CH1_Bq11, CH1_Bq12,
			CH2_Bq1, CH2_Bq2, CH2_Bq3, CH2_Bq4, CH2_Bq5, CH2_Bq6,
			CH2_Bq7, CH2_Bq8, CH2_Bq9, CH2_Bq10, CH2_Bq11, CH2_Bq12,
			z_U,z_V,y_U,y_V, Mute, Vol_CH1, Vol_CH2, Delays, Config};

	int blokovi[39]={1,1,1,1,1,1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
			1,1,1,1,1,1,1,3,1};
	int i;
	uint32_t data_read[5]={0};
	int adr=0;

	SerialSendString("Initialization",1);
	SerialSendString("0%                                     100%",1);

	for(i=0;i<39;i++)
	{
		eeprom_read_block((void*)data_read,(const void*)adr,blokovi[i]*sizeof(uint32_t));

		WriteTAS(adrese[i],data_read,blokovi[i]);

		SerialSend('#');

		adr=adr+(blokovi[i]*sizeof(uint32_t));

	}
	SerialNewLine();
	SerialSendString("Initialized!",1);
}
uint32_t format_select()
{
	char c;
	uint32_t Configuration[1]={0x01031100};
	SerialNewLine();
	SerialSendString("1-LJ, 2-RJ, 3-I2S",1);
	SerialSendString("Select data format for inputs: ",0);
	c=SerialRecive();
	SerialSend(c);
	switch(c)
	{
		case '1':
			Configuration[0]|=(0x00000000);
			break;
		case '2':
			Configuration[0]|=(0x00000020);
			break;
		case '3':
			Configuration[0]|=(0x00000030);
			break;
		default:
			SerialSendString("Invalid input",1);
			break;
	}

	SerialNewLine();
	SerialSendString("1-LJ, 2-RJ, 3-I2S",1);
	SerialSendString("Select data format for output: ",0);
	c=SerialRecive();
	SerialSend(c);
	switch(c)
	{
		case '1':
			Configuration[0]^=(0x00000000);
			break;
		case '2':
			Configuration[0]^=(0x00000002);
			break;
		case '3':
			Configuration[0]^=(0x00000003);
			break;
		default:
			SerialSendString("Invalid input",1);
			break;
	}
	SerialNewLine();
	SerialSendString("1-16-bit, 2-24-bit",1);
	SerialSendString("Select number of bits for input data format ",0);
	c=SerialRecive();
	SerialSend(c);
	switch(c)
	{
		case '1':
			Configuration[0]^=(0x00001000);
			break;
		case '2':
			Configuration[0]^=(0x00002000);
			break;
		default:
			SerialSendString("Invalid input",1);
			break;
	}
	SerialNewLine();
	SerialSendString("1-16-bit, 2-24-bit",1);
	SerialSendString("Select number of bits for output data format ",0);
	c=SerialRecive();
	SerialSend(c);
	switch(c)
	{
		case '1':
			Configuration[0]^=(0x00000100);
			break;
		case '2':
			Configuration[0]^=(0x00000200);
			break;
		default:
			SerialSendString("Invalid input",1);
			break;
	}
	return Configuration[0];
}
void utility()
{
	char c,sel;
	int i=0;
	uint32_t Zero[1]={0x00000000};
	uint32_t One[1]= {0x00800000};
	uint32_t Mute_ON={0x00000007};
	uint32_t Configuration[1]={0x01031100};
	uint32_t New_config[1]={0};
	SerialNewLine();
	SerialSendString("Utilites:",1);
	SerialSendString("1: Use Left channel for input",1);
	SerialSendString("2: Use Right channel for input",1);
	SerialSendString("3: Select input & output data format",1);

	SerialSendString("Enter menu number: ",0);

	do
	{
		c=SerialRecive();

		if(c==BACKSPACE)
		{
			i--;
			if(i>=0)
			{SerialSend(c);}

		}

		if((c>48)&&(c<58))
		{
			sel=c;
			SerialSend(sel);
			i++;
		}
	}while(c!=ENTER);

	switch (sel)
	{
		case '1':
			//Mix A to d (CH1)
			WriteTAS(A_d,One,1);
			//Unmix A to a
			WriteTAS(A_a,Zero,1);
			//Mix A to e (CH2)
			WriteTAS(A_e,One,1);
			//Unimix B to b (CH2-effects block)
			WriteTAS(B_b,Zero,1);
			break;
		case '2':
			//Mix B to d (CH1)
			WriteTAS(B_d,One,1);
			//Unmix A to a
			WriteTAS(A_a,Zero,1);
			//Mix B to e (CH2)
			WriteTAS(B_e,One,1);
			//Unimix B to b (CH2-effects block)
			WriteTAS(B_b,Zero,1);
			break;
		case '3':
			New_config[0]=format_select();
			WriteTAS(Config,New_config,1);
			break;

		default:
			SerialNewLine();
			SerialSendString("***Nothing selected!***",1);
			break;
	}

}
void single()
{
	char c,sub;
	uint32_t data[5]={0};

	int no_of_bytes=0;
	int j=0;
	int i=0;
	int k;
	//Entering subadress
	SUBadr:
	i=0;
	SerialNewLine();
	SerialSendString("Enter subbadress: 0x",0);

	while((c!=ENTER) | (i<2))
	{
		c=SerialRecive();
		if(c==ESC)
		{ return;}
		if(c==ENTER)
		{break;}

		if((c>47 & c<59) | (c>64 & c<71) | (c>96 & c<103))
		{
			data[i]=c;
			SerialSend(c);
			i++;
		}
		if(c==BACKSPACE)
		{
			i--;
			if(i>=0)
			{SerialSend(c);}

		}

	}

	if(i!=2)
	{
		SerialNewLine();
		SerialSendString("Invalid subadress",1);
		goto SUBadr;
	}
	else
	{
		sub=tohex(data[0])<<4 | tohex(data[1]);
	}

	//entering number od 32-bit words
	WORDS:
	i=0;
	SerialNewLine();
	SerialSendString("Enter number of 32-bit words: ",0);

	while((c!=ENTER) | (i<1))
	{
		c=SerialRecive();
		if(c==ESC)
		{ return;}
		if(c==ENTER)
		{break;}

		if((c>47 & c<59))
		{
			no_of_bytes=c-48;
			SerialSend(c);
			i++;
		}
		if(c==BACKSPACE)
		{
			i--;
			if(i>=0)
			{SerialSend(c);}

		}

	}
	for(k=0;k<5;k++)
	{
		data[i]=0;
	}
	if(i!=1)
	{
		SerialNewLine();
		SerialSendString("Invalid numbers of 32-bit words",1);
		goto WORDS;
	}
	//entering 32-bit words
	else
	{
		for(i=0;i<5;i++)
		{
			data[i]=0;
		}

		for(k=0;k<no_of_bytes;k++)
		{
			KOEF:
			i=0;
			SerialNewLine();
			SerialSendString("Unesite ",0);
			SerialSendInt(k+1);
			SerialSendString(". koeficijent: ",0);

			for(j=7;j>=0;j--)
			{
				while((c!=ENTER) | (i<7))
				{
					c=SerialRecive();
					if(c==ESC)
					{ return;}
					if(c==ENTER)
					{break;}

					if((c>47 & c<59) | (c>64 & c<71) | (c>96 & c<103))
					{
						data[i]|=((uint32_t)tohex(c))<<(j*4);
						SerialSend(c);
						i++;
					}
					if(c==BACKSPACE)
					{
						i--;
						if(i>=0)
						{SerialSend(c);}

					}
				}

				if(i!=8)
				{
					SerialNewLine();
					SerialSendString("Invalid 32-bit word",1);
					goto KOEF;
				}

			}
		}
	}
	SerialNewLine();
	SerialSendString("Writing...",1);
	WriteTAS(sub,data,no_of_bytes);
	SerialNewLine();
	SerialSendString("Write successful!",1);
}


void filtar()
{
	//		a1	a2	b0		b1	b2

	biquadTAS(0x4F,0x006EF2EC,0x00000000,0x00088689,0x00088689,0x00000000);
	//biquadTAS(0x50,0x006EF2EC,0x00000000,0x00088689,0x00088689,0x00000000);
	//biquadTAS(0x51,0x006EF2EC,0x00000000,0x00088689,0x00088689,0x00000000);
	//biquadTAS(0x52,0x006EF2EC,0x00000000,0x00088689,0x00088689,0x00000000);

	biquadTAS(0x5B,0x00DDE5D8,0xFF9FD4AD,0x006F844B,0xFF20F76A,0x006F844B);
	//biquadTAS(0x5C,0x00DDE5D8,0xFF9FD4AD,0x006F844B,0xFF20F76A,0x006F844B);
	//biquadTAS(0x5D,0x00DDE5D8,0xFF9FD4AD,0x006F844B,0xFF20F76A,0x006F844B);
	//biquadTAS(0x5E,0x00DDE5D8,0xFF9FD4AD,0x006F844B,0xFF20F76A,0x006F844B);
}

void readbiquad(char sub)
{
	ReadTAS(0x4F,5);
}

void read()
{
	char c,sub;
	uint32_t data[5]={0};
	uint32_t *read_data;
	int no_of_bytes=0;
	int j=0;
	int i=0;
	int k;
	//Entering subadress
	SUBadr:
	i=0;
	SerialNewLine();
	SerialSendString("Enter subbadress: 0x",0);

	while((c!=ENTER) | (i<2))
	{
		c=SerialRecive();
		if(c==ESC)
		{ return;}
		if(c==ENTER)
		{break;}

		if((c>47 & c<59) | (c>64 & c<71) | (c>96 & c<103))
		{
			data[i]=c;
			SerialSend(c);
			i++;
		}
		if(c==BACKSPACE)
		{
			i--;
			if(i>=0)
			{SerialSend(c);}

		}

	}

	if(i!=2)
	{
		SerialNewLine();
		SerialSendString("Invalid subadress",1);
		goto SUBadr;
	}
	else
	{
		sub=tohex(data[0])<<4 | tohex(data[1]);
	}

	//entering number od 32-bit words
	WORDS:
	i=0;
	SerialNewLine();
	SerialSendString("Enter number of 32-bit words: ",0);

	while((c!=ENTER) | (i<1))
	{
		c=SerialRecive();
		if(c==ESC)
		{ return;}
		if(c==ENTER)
		{break;}

		if((c>47 & c<59))
		{
			no_of_bytes=c-48;
			SerialSend(c);
			i++;
		}
		if(c==BACKSPACE)
		{
			i--;
			if(i>=0)
			{SerialSend(c);}

		}

	}

	if(i!=1)
	{
		SerialNewLine();
		SerialSendString("Invalid numbers of 32-bit words",1);
		goto WORDS;
	}
	//entering 32-bit words
	else
	{
		read_data=ReadTAS(sub,no_of_bytes);
		for(i=0;i<no_of_bytes;i++)
		{
			SerialNewLine();
			SerialSendInt(i+1);
			SerialSendString(". koeficijent: 0x",0);
			SerialSend(toasci((uint8_t)(data[i]>>24),0xF0));
			SerialSend(toasci((uint8_t)(data[i]>>24),0x0F));
			SerialSend(toasci((uint8_t)(data[i]>>16),0xF0));
			SerialSend(toasci((uint8_t)(data[i]>>16),0x0F));
			SerialSend(toasci((uint8_t)(data[i]>>8),0xF0));
			SerialSend(toasci((uint8_t)(data[i]>>8),0x0F));
			SerialSend(toasci((uint8_t)(data[i]>>0),0xF0));
			SerialSend(toasci((uint8_t)(data[i]>>0),0x0F));
		}
		SerialNewLine();
	}

}
void check()
{
	char check=0;
	SerialSendString("Checking TAS3103 I2C comm!",1);

	check=CheckTAS();
	SerialNewLine();
	if(check==1)
	{SerialSendString("OK!",1);}
	else
	{
		SerialSendString("ERROR!",1);
	}
}
void menu()
{
	char c,sel;
	int i=0;
	SerialNewLine();
	SerialSendString("Menu:",1);
	SerialSendString("1: Write to TAS3103",1);
	SerialSendString("2: Read from TAS3103",1);
	SerialSendString("3: Initilize TAS3103",1);
	SerialSendString("4: Save configuration",1);
	SerialSendString("5: Reset TAS3103",1);
	//SerialSendString("6: Set biquad on Ch1",1);
	//SerialSendString("7: Read biquad",1);
	//SerialSendString("8: Debug numbers",1);
	//SerialSendString("9: Check TAS3103 comms",1);
	SerialSendString("Enter menu number: ",0);

	do
	{
		c=SerialRecive();

		if(c==BACKSPACE)
		{
			i--;
			if(i>=0)
			{

				SerialSend(BACKSPACE);

				}

		}

		if((c>48)&&(c<58))
		{
			sel=c;
			SerialSend(sel);
			i++;
		}
		
	}while(c!=ENTER);

	switch (sel)
	{
		case '1':
			single();
			break;
		case '2':
			read();
			break;
		case '3':
			init();
			break;
		case '4':
			save();
			break;
		case '5':
			ResetTAS();
			break;
		case '6':
			utility();
			break;
		case '7':

			//readbiquad(0x4F);
			break;
		case '8':
			InitTAS();
			break;
		case '9':

			break;
		case '0':

			break;
		default:
			SerialNewLine();
			SerialSendString("***Nothing selected!***",1);
			break;
	}

}
int main(void)
{

	DDRC|=(1<<PC3);
	PORTC|=(1<<PC3);

	SerialInit();
	TWIInit();

	DDRB|=(1<<PB0);

	SerialClr();


    	char c,startup=0;
    	while(1)
   	{
   		if(startup==0)
   		{
   			startup=1;
   			SerialSendString("Press any key to continue...",1);
   			//check();
   			c=SerialRecive();
   			if(c>0)
   			{
   				SerialClr();
   				menu();
   			}
   		}
   		else
   		{
   			SerialClr();
   			menu();
   		}

   	}
}
