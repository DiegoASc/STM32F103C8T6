#include "OLED_Hal_I2C.h"
#include "main.h"
#include "stm32f1xx_hal.h"

#define ssd1306_128x64//3k3 Pull Up no SDA
#include "Font_5x8.h"

extern I2C_HandleTypeDef hi2c1;

#define OLED_COLUNAS		128
#define OLED_LINHAS			64
#define OLED_OK		      40
#define OLED_INI_lINHA	2
#define BUFFER_LEN (OLED_COLUNAS*OLED_LINHAS)/8
unsigned char oled_buffer[BUFFER_LEN+1];
unsigned char oled_buffer_apl[BUFFER_LEN+1];

char oled_string[80];//vetor para os caracteres a serem escritos no display, 21 caracteres mais o '\0' no final.
const unsigned char init_oled[32]={
0xAE,0xD5,0x80,0xA8,0x3F,0xd3,
0x00,0x40,0x8d,0x14,0x20,0x00,
0xa1,0xc8,0xda,0x12,0x81,0xcf,//0º
//0xa0,0xc0,0xda,0x12,0x81,0xcf,//180º
0xd9,0xf1,0xdb,0x40,0xa4,0xa6,
0x2e,0xaf,0x22,0x00,0xff,0x21,
0x00,0x7f,
};

void Oled_Print (void){
	oled_buffer[0]=0x40;
	HAL_I2C_Master_Transmit(&hi2c1,0x78,(unsigned char*)&oled_buffer[0],BUFFER_LEN+1,1000);
}
void Oled_Init (void){
	unsigned char comandos[3];
	int i;
	
	RCC->APB1ENR|= RCC_APB1ENR_I2C1EN;
	for(i=0;i<32;i++){
		comandos[0]=0;
		comandos[1]=init_oled[i];
		HAL_I2C_Master_Transmit(&hi2c1,0x78,comandos,2,1000);
	}
	Oled_Print();
	RCC->APB1ENR&=~RCC_APB1ENR_I2C1EN;
}
void Oled_Refresh (void){
	unsigned char comandos[3],init=1,temp;
	int i,inicio=BUFFER_LEN,fim=0;

	for(i=1;i<=BUFFER_LEN;i++){
		if(oled_buffer_apl[i]!=oled_buffer[i]){
			oled_buffer_apl[i]=oled_buffer[i];
			if(inicio>i)inicio=i;
			if(fim<i)fim=i;
		}
		if(fim && ((i-8)>fim || (i%128)==127)){//8 caracteres n�o atualizados ou fim de linha
			RCC->APB1ENR|= RCC_APB1ENR_I2C1EN;
			if(init){
				comandos[0]=0x00;
				comandos[1]=0x00;//Column 4LSBits
				HAL_I2C_Master_Transmit(&hi2c1,0x78,comandos,2,1000);
				comandos[0]=0x00;
				comandos[1]=0x10;//Column 4HSBits
				HAL_I2C_Master_Transmit(&hi2c1,0x78,comandos,2,1000);
				comandos[0]=0x00;
				comandos[1]=0x40;//Set line 
				HAL_I2C_Master_Transmit(&hi2c1,0x78,comandos,2,1000);
				
				init=0;
			}
			comandos[0]=0x00;
			comandos[1]=0xb0+  (inicio/128);//Set Page
			HAL_I2C_Master_Transmit(&hi2c1,0x78,comandos,2,1000);
			comandos[0]=0x00;
			comandos[1]=0x00+ ((inicio+OLED_INI_lINHA)%16);//Column 4LSBits
			HAL_I2C_Master_Transmit(&hi2c1,0x78,comandos,2,1000);
			comandos[0]=0x00;
			comandos[1]=0x10+(((inicio+OLED_INI_lINHA)%128)>>4);//Column 4HSBits
			HAL_I2C_Master_Transmit(&hi2c1,0x78,comandos,2,1000);

			//I2C1_Send(0x78,0x40,(unsigned char*)&oled_buffer[inicio],1+(fim-inicio));//atializa o meio da linha	
			temp=oled_buffer[inicio-1];
			oled_buffer[inicio-1]=0x40;
			HAL_I2C_Master_Transmit(&hi2c1,0x78,(unsigned char*)&oled_buffer[inicio-1],2+(fim-inicio),1000);
			oled_buffer[inicio-1]=temp;
			inicio=BUFFER_LEN;fim=0;
			RCC->APB1ENR&=~RCC_APB1ENR_I2C1EN;
		}
	}
}
short Print_Char(short linha,short coluna,unsigned char char_){
	static short tipo=1;
	if(coluna>=(OLED_COLUNAS/6) || linha>=(OLED_LINHAS/8))return(0);
	coluna*=6;
	linha*=128;
	//if(char_=='�')char_=129;
	//if(char_=='�')char_=130;
	if(char_<' ' || char_>130)return(0);
	char_-=' ';
	if(tipo==1){
		oled_buffer[linha+coluna+1]=Font_5x8[char_][0];
		oled_buffer[linha+coluna+2]=Font_5x8[char_][1];
		oled_buffer[linha+coluna+3]=Font_5x8[char_][2];
		oled_buffer[linha+coluna+4]=Font_5x8[char_][3];
		oled_buffer[linha+coluna+5]=Font_5x8[char_][4];
		oled_buffer[linha+coluna+6]=0;
	}
	else{
		oled_buffer[linha+coluna+1]|=Font_5x8[char_][0];
		oled_buffer[linha+coluna+2]|=Font_5x8[char_][1];
		oled_buffer[linha+coluna+3]|=Font_5x8[char_][2];
		oled_buffer[linha+coluna+4]|=Font_5x8[char_][3];
		oled_buffer[linha+coluna+5]|=Font_5x8[char_][4];
		oled_buffer[linha+coluna+6]  =0;
	}
	char_+=' ';
	if(char_==129 || char_=='`' || char_=='~' || char_=='^')tipo=0;
	else tipo=1;
	return(tipo);
}
char Print_String(short linha,short coluna,char *string){
	unsigned char pos_string=0;
	while(string[pos_string] && pos_string<21){
		if(string[pos_string]=='\n')return(pos_string+1);
		coluna+=Print_Char(linha,coluna,string[pos_string]);
		pos_string++;
	}
	return(pos_string);
}
void Oled_xy(unsigned char x,unsigned char y,char valor){
	if(valor)oled_buffer[128*(y/8)+x]|= (1<<(y%8));//Acende o pixel selecionado
	else     oled_buffer[128*(y/8)+x]&=~(1<<(y%8));//Apaga o pixel selecionado
}
void Plot_Line(signed char xi,signed char yi,signed char xf,signed char yf,char valor){
	float x,y,xp,yp;
	
	yi=OLED_LINHAS-yi;
	yf=OLED_LINHAS-yf;
	if(xi>xf){x=xf;xf=xi;xi=x;y=yf;yf=yi;yi=y;}
	else {x=xi;y=yi;}
	xp=(float)(xf-xi)/(float)(yf-yi);
	yp=(float)(yf-yi)/(float)(xf-xi);
	
	if(xp<=-1 || xp>=1){
		for(;xi<=xf;xi++,y+=yp){
			yi=y;
			if(valor)oled_buffer[128*(yi/8)+xi]|= (1<<(yi%8));//Acende o pixel selecionado
			else     oled_buffer[128*(yi/8)+xi]&=~(1<<(yi%8));//Apaga o pixel selecionado
		}
	}
	else if(yp<=-1 || yp>=1){
		if(yi>yf){x=xf;xf=xi;xi=x;y=yf;yf=yi;yi=y;}
		else {x=xi;y=yi;}
		for(;yi<=yf;yi++,x+=xp){
			xi=x;
			if(valor)oled_buffer[128*(yi/8)+xi]|= (1<<(yi%8));//Acende o pixel selecionado
			else     oled_buffer[128*(yi/8)+xi]&=~(1<<(yi%8));//Apaga o pixel selecionado
		}
	}
}
void Plot_xy (unsigned char x,unsigned char y,char valor,char *vetorp,char len){
	unsigned char pos;
	for(pos=1;pos<OLED_COLUNAS && pos<len;pos++){
		Plot_Line(pos-1+x,vetorp[pos-1]+y,pos+x,vetorp[pos]+y,1);
	}
}
void Plot_BMP(unsigned char x,unsigned char y,unsigned char xs,unsigned char ys,unsigned char* bmp){
	unsigned char xp,yp;
	y /=8;
	ys/=8;
	for(yp=0;yp<ys;yp++){
		for(xp=0;xp<xs;xp++){
			oled_buffer[128*(yp+y)+(xp+x)]=bmp[xp+(yp*xs)];
		}
	}
}
