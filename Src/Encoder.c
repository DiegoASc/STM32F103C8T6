#include "Encoder.h"
#include "tim.h"

short encoder=0,tecla=0;
char tecla_reset=0;

void Encoder_Init(void){
	HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);
}
short Encoder(void){
	encoder=htim4.Instance->CNT/2;
	return(encoder);
}
void Tecla_Encoder (void){
	if(tecla_reset==0){
		tecla++;
		tecla_reset=250;
	}
}
void Tecla_Reset (void){
	if(tecla_reset)tecla_reset--;
}
