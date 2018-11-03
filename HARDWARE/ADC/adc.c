#include "adc.h"
#include "sys.h" 
#include "delay.h"

void ADC1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitTStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOC,ENABLE);	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);  //����ADC��Ƶ����6

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	ADC_InitTStruct.ADC_ContinuousConvMode = ENABLE;	//����ת��
	ADC_InitTStruct.ADC_DataAlign = ADC_DataAlign_Right; //ADC�����Ҷ���
	ADC_InitTStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //ѡ��SWSATART��Ϊ�����¼�
	ADC_InitTStruct.ADC_Mode = ADC_Mode_Independent;  //����ģʽ
	ADC_InitTStruct.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_InitTStruct.ADC_ScanConvMode = DISABLE; //����Ϊ��ͨ��ģʽ
	ADC_Init(ADC1, &ADC_InitTStruct);	 
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);//����ָ��ADC�Ĺ�����ͨ��	
	ADC_Cmd(ADC1, ENABLE); 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ʹ��SWSATART��ʼת������ͨ��
	
	ADC_ResetCalibration(ADC1); //ʹ�ܸ�λУ׼ ADC_CR2 RSTCAL 
	while(ADC_GetResetCalibrationStatus(ADC1)); //�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1); //����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1));  //�ȴ�У׼����
	
}

//��ȡ�����������ֵ��Χ0-10
u16 Get_RandNum(void)
{
	u32 tem = 0;
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); //ADC_FLAG_EOC = 0 ת��δ���
	tem = ADC_GetConversionValue(ADC1); //Returns the last ADCx conversion result data for regular channel
	return tem % 10;
} 	 


