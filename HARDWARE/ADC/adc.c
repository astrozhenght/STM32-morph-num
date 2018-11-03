#include "adc.h"
#include "sys.h" 
#include "delay.h"

void ADC1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitTStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOC,ENABLE);	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);  //设置ADC分频因子6

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	ADC_InitTStruct.ADC_ContinuousConvMode = ENABLE;	//连续转换
	ADC_InitTStruct.ADC_DataAlign = ADC_DataAlign_Right; //ADC数据右对齐
	ADC_InitTStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //选择SWSATART作为触发事件
	ADC_InitTStruct.ADC_Mode = ADC_Mode_Independent;  //单次模式
	ADC_InitTStruct.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_InitTStruct.ADC_ScanConvMode = DISABLE; //设置为单通道模式
	ADC_Init(ADC1, &ADC_InitTStruct);	 
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);//设置指定ADC的规则组通道	
	ADC_Cmd(ADC1, ENABLE); 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使用SWSATART开始转换规则通道
	
	ADC_ResetCalibration(ADC1); //使能复位校准 ADC_CR2 RSTCAL 
	while(ADC_GetResetCalibrationStatus(ADC1)); //等待复位校准结束
	
	ADC_StartCalibration(ADC1); //开启AD校准
	while(ADC_GetCalibrationStatus(ADC1));  //等待校准结束
	
}

//获取随机数，返回值范围0-10
u16 Get_RandNum(void)
{
	u32 tem = 0;
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); //ADC_FLAG_EOC = 0 转换未完成
	tem = ADC_GetConversionValue(ADC1); //Returns the last ADCx conversion result data for regular channel
	return tem % 10;
} 	 


