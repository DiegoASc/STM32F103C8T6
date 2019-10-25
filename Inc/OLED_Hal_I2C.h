void Oled_Init(void);
void Oled_Refresh(void);//Atualiza a tela!!!!
char Print_String(short linha,short coluna,char *string);
void Plot_Line(signed char xi,signed char yi,signed char xf,signed char yf,char valor);
void Plot_xy (unsigned char x,unsigned char y,char valor,char *vetorp,char len);
void Plot_BMP(unsigned char x,unsigned char y,unsigned char xs,unsigned char ys,unsigned char* bmp);

extern char oled_string[];
/* Exemplos
	Oled_Init();
  	Print_String(0,0,"STM32");//8 Linhas por 21 colunas
	sprintf(oled_string,"%i",123);
	Print_String(3,4,oled_string);
	Oled_xy(  0, 0,1);//128 x 64 Pixels
	Oled_xy(  0,31,1);//(0 a 127)x e (0 a 63)y
	Oled_xy(127, 0,1);
	Oled_xy(127,31,1);
	Print_String(2,4,"abc");
	Plot_xy(64,10,1,vetor,31);//Plota o gráfico apartir da origem
	Plot_xy(64,10,0,vetor,31);//Apaga a gráfico plotado	
	Plot_xy(70,10,1,vetor,31);//Plota o gráfico apartir da origem
	Oled_Refresh();//Sempre no final do while(1) ou Loop
*/
