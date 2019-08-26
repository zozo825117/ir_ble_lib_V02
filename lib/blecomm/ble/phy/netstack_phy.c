//#define ALLOCATE_EXTERN

#include "../../blecomm.h"
// #include "delay.h"
// #include "spi.h"
//#include "GW32L0xx_port.h"

#define WL1600_OP_READ      (0x80)

//#define ENABLE_INTERRUPT()          EA = 1
//#define DISABLE_INTERRUPT()         EA = 0
//#define ENABLE_LL_INTERRUPT()       EX8_15 = 1
//#define DISABLE_LL_INTERRUPT()      EX8_15 = 0
   
/*
 * Default WL1600 register settings.
 */
#define REG_END_MARK    (255)
const uint8_t rf_regs[][2]  = {   //小米5能收到
	
	#if 1
     {0,  0x6F},//6F  7F
    {1,  0xE8},
		// {2,  0x57},//56  57
    {2,  0x52},//56  57
    {3,  0x0D},
    {4,  0xC4}, // 
    {8,  0x25}, //
    {9,  0x41},//41  14
    {10, 0x57},
    {11, 0x3B},
    {16, 0x86},
		
		{17,0x68},//7A
		{18,0x0C},
		// {17,0x3A},//7A
		// {18,0x18},
		{34,0x48},//48
		{41,0x10},
		{45,0x00},
		{46,0x09},//0x01
		{52,0x19},//19
		{53,0x40},
		
		{64, 0x0A},//0x0A
    {72, 0xBE},//BE
    {73, 0xD6},//D6 
    {78, 0x8E},//8E
    {79, 0x89}, //89
		{80,0xF8},
		{81, 0x42},//0x44
		{82, 0x70},  
		
//		{65, 0x20},
    {66, 0x08},
		
    {67, 0x00},  //A0
		{68,0x20},// 9C
//		{69,0x0A},
		
		{255, 255} 
		
		#endif 
		
	
};



// uint8_t g24_regs[][2] = {
// 	{17,0x3A},
// 	{18,0x0C},
// 	{34,0x08},
// 	{45,0x00},
// 	{46,0x09},
// 	{52,0x19},
// 	{53,0x40},
// 	{80,0xF8},
// 	{64,0xA8},
// 	{73,0XC7},
// 	{79,0XC8},
// 	{78,0X03},
// 	{82,0X20},
// 	{81,0X41},
// 	{68,0X60},
// 	{41,0X70},
// 	{4,0XC2},
// 	{35,0X08},
// 	{255,255}
	
	
// };

//void P0_IRQHandler(void)
//{
//  if(PORT_EXTI_GetITStatus(PORT_0, PORT_Pin_12) == SET)
//  {                     
//    PORT_EXTI_ClearITFlag(PORT_0, PORT_Pin_12);
//    PORT_ToggleBit(PORT_0, PORT_Pin_1);
//  }
//}

/**
 * 功能：延时
 */
// void delay_1ms(uint16_t u32Cnt){
//   uint32_t counter = u32Cnt<<11;
//   while (counter-- > 0) {
//     __nop();
//   }
// }
// void delay_1us(uint32_t u32Cnt){
//   uint32_t counter = u32Cnt<<1;
//   while (counter-- > 0) {
//     __nop();
//   }
// }

//void phy_pkt_flag_init(void)
//{
//	PORT_InitTypeDef PORT_InitStruct;
//	PORT_EXTI_InitTypeDef PORT_EXTI_InitStruct;
//	
//	CMU_APBPeriph1ClockCmd(CMU_APBPeriph1_PORT, ENABLE);
//	
//  PORT_InitStruct.PORT_Pin = PORT_Pin_12;
//  PORT_InitStruct.PORT_Properity = PORT_Properity_Digital;
//  PORT_InitStruct.PORT_Mode = PORT_Mode_IN;
//  PORT_InitStruct.PORT_PullHigh = PORT_PH_NoPullHigh;
//  PORT_InitStruct.PORT_DriveSink = PORT_DS_DriveSinkNormal;
//	
//  PORT_Init(PORT_0, &PORT_InitStruct);
//	
//// Init P0.12 for EXTI  0.12
//  PORT_EXTI_StructInit(&PORT_EXTI_InitStruct);
//  PORT_EXTI_InitStruct.PORT_EXTI_InputSource = PORT_EXTI_InputSource_Edge;
//  PORT_EXTI_InitStruct.PORT_EXTI_Trigger = PORT_EXTI_EdgeTrigger_Falling; 
//  PORT_EXTI_InitStruct.PORT_EXTI_Cmd = ENABLE;
//  PORT_EXTI_Init(PORT_0, PORT_PinSource12, &PORT_EXTI_InitStruct);
//  
//	PORT_EXTI_ClearITFlag(PORT_0, PORT_Pin_12);	
//	PORT_EXTI_ITConfig(PORT_0, PORT_Pin_12, ENABLE);
//	
//	__disable_irq();
//	NVIC_DisableIRQ(P0_IRQn);   
//	NVIC_ClearPendingIRQ(P0_IRQn);   
//	NVIC_SetPriority(P0_IRQn, 0x2);
//	NVIC_EnableIRQ(P0_IRQn);
//	__enable_irq();
//}





/**
 * 函数：wl1600_check_regs
 * 功能：1600寄存器检测
 * 参数：无
 * 返回：检测成功返回 REG_END_MARK（255），失败返回错误寄存器数组编号
 */
uint8_t phy_check_regs(void)
{
		uint8_t i;
		uint8_t val;
		for (i = 0; rf_regs[i][0] != REG_END_MARK; ++i) {
			if(rf_regs[i][0]==46){
			continue;
			}
				// delay_1ms(10);
			val = phy_read_reg(rf_regs[i][0]);
				if ( val != rf_regs[i][1]) {
						return (i);
				}
		}
    return (REG_END_MARK);
}

/**
 * 功能：物理层初始化
 * 参数：无
 * 返回：成功返回0
 */
uint8_t phy_init(void)
{
	uint8_t i;
	
	//phy_pkt_flag_init();

	/* Configure WL1601 */
	do{
		phy_write_reg(rf_regs[0][0], rf_regs[0][1]);
		 for (i = 0; rf_regs[i][0] != REG_END_MARK; ++i) {
			 phy_write_reg(rf_regs[i][0], rf_regs[i][1]);
		}
	}while(phy_check_regs() != REG_END_MARK);

	return 0;
}


// static uint8_t g24_check_regs(void)
// {
// 		uint8_t i;
// 		uint8_t val;
	
// 		for (i = 0; g24_regs[i][0] != REG_END_MARK; ++i) {
// 			if(g24_regs[i][0]==46){
// 			continue;
// 			}
// 				delay_1ms(10);
// 			val = phy_read_reg(g24_regs[i][0]);
// 				if ( val != g24_regs[i][1]) {
// 						return (i);
// 				}
// 		}
//     return (REG_END_MARK);
// }


// uint8_t g24_init(void)
// {
// 	uint8_t i;
// 	phy_pkt_flag_init();
// #ifdef WIRE_SPI_MODE4
// 	RF_DISABLE();
// 	RF_SPI_DESELECT();

// 	/* Reset WL1600 */
// 	RF_DISABLE();
// 	delay_1us(50); 
	
// 	RF_ENABLE();
// 	delay_1ms(5);

// 	/* Configure WL1600 */
// 	do{
// 		phy_write_reg(g24_regs[0][0], g24_regs[0][1]);
// 		 for (i = 0; g24_regs[i][0] != REG_END_MARK; ++i) {
// 			 phy_write_reg(g24_regs[i][0], g24_regs[i][1]);
// 		}
// 	}while(g24_check_regs() != REG_END_MARK);
// #endif
	
// #ifdef WIRE_SPI_MODE3
// 	DRVSPI_CSN1;
// 	delay_1us(20000);
// 	DrvWL1601B_WriteReg(94,0x80); //设置3线SPI通信  
// #endif
// 		//检测SPI接口
// 	while(DrvWL1601B_Check()){};//超过32ms复位MCUs
// 	//写寄存器表
	
// 	for(i=0;i<8;i++)
// 	{
// 		DrvWL1601B_WriteReg(g24_regs[i][0], g24_regs[i][1]);                      
// 		if(i!=4)
// 		{
// 			if(g24_regs[i][1] != DrvWL1601B_ReadReg(g24_regs[i][0]))
// 			{
// 				while(1);
// 			}
// 		}
// 	}  

//  //preamble Syncword 长度 trailer 长度设置
// 	DrvWL1601B_WriteReg(64,0xA8);//3Byte SYNCWORD	
// 	DrvWL1601B_WriteReg(82,0x20);//Miscellaneous REG1
// 	DrvWL1601B_WriteReg(81,0x41);//syncword 阈值 1bit
// 	DrvWL1601B_WriteReg(41,0x70);//关 Trailer LSB syncword 长度为奇数
// 	DrvWL1601B_WriteReg(95,0x80|16);//固定包长
	
// 	DrvWL1601B_WriteReg(04,0xC2);//Agc
// 	DrvWL1601B_WriteReg(35,0x08);//Lna
// 	DrvWL1601B_WriteReg(68,0x60);//CW	  

// 	DrvWL1601B_WriteReg(73,0XC7);//0xEE	0x77
// 	DrvWL1601B_WriteReg(78,0X03);//0x33	0xCC
// 	DrvWL1601B_WriteReg(79,0XC8);//0xC0	0x03   
// 	DrvWL1601B_WriteBit(82,5,1);//关闭数据长度发送
// 	DrvWL1601B_WriteBit(33,1,1); //使能PKT_flag
// 	return 0;
// }


// const uint8_t null_dat[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// uint8_t g24_send(uint8_t *dat,uint8_t len,uint8_t ch)
// {
// 	uint8_t i;
// 	phy_write_reg(95,0x80|18);
// 	phy_write_fifo(100,dat,len);
// 	if(len<18){
// 		phy_write_fifo(100,(uint8_t*)null_dat,18-len);
// 	}
// 	phy_write_reg(15,ch);
// 	phy_write_reg_bit(14,0,1);
	
// 	delay_1ms(1);
// 	phy_write_reg_bit(14,0,0);//使能发射
// 	return 0;
// }


























