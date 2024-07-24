#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "adc3.h"
#include "adc1.h"
#include "dma.h"
#include "lsens.h"
#include "pwm.h"
#include "key.h"
#include "touch.h" 
#include "beep.h"
#include "picture.h"


#include "includes.h"

/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			15 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				128
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	
 			   
//task1任务
//设置任务优先级
#define TASK1_PRO       			6 
//设置任务堆栈大小
#define TASK1_STK_SIZE  		    		128
//任务堆栈	
OS_STK TASK1_TASK_STK[TASK1_STK_SIZE];
//任务函数
void task1_task(void *pdata);

//task2任务
//设置任务优先级
#define TASK2_PRO       			7
//设置任务堆栈大小
#define TASK2_STK_SIZE  					128
//任务堆栈
OS_STK TASK2_TASK_STK[TASK2_STK_SIZE];
//任务函数
void task2_task(void *pdata);

//task3任务
//设置任务优先级
#define TASK3_PRO       			8 
//设置任务堆栈大小
#define TASK3_STK_SIZE  					128
//任务堆栈
OS_STK TASK3_TASK_STK[TASK3_STK_SIZE];
//任务函数
void task3_task(void *pdata);

//task4任务
//设置任务优先级
#define TASK4_PRO       			9
//设置任务堆栈大小
#define TASK4_STK_SIZE  					128
//任务堆栈
OS_STK TASK4_TASK_STK[TASK4_STK_SIZE];
//任务函数
void task4_task(void *pdata);

//task5任务
//设置任务优先级
#define TASK5_PRO       			10 
//设置任务堆栈大小
#define TASK5_STK_SIZE  					128
//任务堆栈
OS_STK TASK5_TASK_STK[TASK5_STK_SIZE];
//任务函数
void task5_task(void *pdata);

/* 定义一个邮箱事件指针 */
OS_EVENT *MesageBox;

#define SEND_BUF_SIZE 8200	//发送数据长度,最好等于sizeof(TEXT_TO_SEND)+2的整数倍.

u8 SendBuff[SEND_BUF_SIZE];	//发送数据缓冲区

void display(void);

#if PICTURE_ON
const unsigned char *arr[3] = {gImage_cp,gImage_zyy,gImage_lyf};
#endif

//清空屏幕并在右上角显示"RST"
void Load_Drow_Dialog(void)
{
    int x1 = 35;
    int x2 = 110;
    int y1 = 210;
    int y2 = 210;
	//LCD_Clear(WHITE);//清屏   
 	POINT_COLOR=BLUE;//设置字体为蓝色 
    LCD_ShowChinese(x1,y1,28,16,0);
    LCD_ShowChinese(x1+16,y1,29,16,0);
    LCD_ShowChinese(x1+2*16,y1,32,16,0);
    LCD_ShowChinese(x1+3*16,y1,33,16,0);
    
    LCD_ShowChinese(x2,y2,30,16,0);
    LCD_ShowChinese(x2+16,y2,31,16,0);
    LCD_ShowChinese(x2+2*16,y2,32,16,0);
    LCD_ShowChinese(x2+3*16,y2,33,16,0);
  	POINT_COLOR=RED;//设置画笔蓝色 
}


//两个数之差的绝对值 
//x1,x2：需取差值的两个数
//返回值：|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  

int main(void)
{ 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);		  //初始化延时函数
    uart_init(115200);	//初始化串口波特率为115200
	LED_Init();		        //初始化LED端口 
    LCD_Init();         //初始化LCD接口
    Adc_Init();
    Adc3_Init();         //初始化ADC
//    BEEP_Init();
    Lsens_Init();           //初始化虚拟光线传感器
    TIM14_PWM_Init(100-1,420-1);
    KEY_Init();
    tp_dev.init();				//触摸屏初始化
    display();
    OSInit();   
    OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
    OSStart();
}

void display(void)
{
    int x1 = 35;
    int x2 = 110;
    int y1 = 210;
    int y2 = 210;
    
    int i=0;
    POINT_COLOR=RED;//设置字体为红色
    LCD_ShowString(30,50,200,16,16,(unsigned char *)"Explorer STM32F4");	
    for(i=0;i<8;i++)LCD_ShowChinese(30+i*16,70,i,16,0);//【环境参数检测】
    for(i=0;i<3;i++)LCD_ShowChinese(30+i*16,90,i+8,16,0);//日期：
    LCD_ShowString(30+3*16,90,200,16,16,(unsigned char *)"2024/3/29");	  
    POINT_COLOR=BLUE;//设置字体为蓝色
    
    LCD_ShowChinese(30,110,25,16,0);//姓
    LCD_ShowChinese(78,110,26,16,0);//名
    LCD_ShowChinese(94,110,27,16,0);//：
    for(i=0;i<3;i++)LCD_ShowChinese(100+i*16,110,i+11,16,0);//邓万穿
    LCD_ShowChinese(30,130,14,16,0);//学
    LCD_ShowChinese(78,130,15,16,0);//号
    LCD_ShowChinese(94,130,16,16,0);//：
    LCD_ShowNum(40+3*20,130,2021213068,10,16);  //2021213068
    for(i=0;i<5;i++)LCD_ShowChinese(30+i*16,150,i+17,16,0);//光照强度：
    LCD_ShowString(30,170,200,16,16,(unsigned char *)"           00.00C");//先在固定位置显示小数点	
    LCD_ShowChinese(30,170,22,16,0);//温
    LCD_ShowChinese(78,170,23,16,0);//度：
    LCD_ShowChinese(94,170,24,16,0);//
    
    POINT_COLOR=BLACK;      //画笔颜色：黑色
    LCD_DrawRectangle(29,108,180,192);
    LCD_DrawLine(29,128,180,128);
    LCD_DrawLine(29,128+20,180,128+20);
    LCD_DrawLine(29,128+40,180,128+40);
    
    POINT_COLOR=LBBLUE;
    LCD_DrawRectangle(35-1,210-1,35+64+1,210+16+1);
    LCD_DrawRectangle(35-1-1,210-1-1,35+64+1+1,210+16+1+1);
    
    POINT_COLOR=RED;
    LCD_DrawRectangle(110-1,210-1,110+64+1,210+16+1);
    LCD_DrawRectangle(110-1-1,210-1-1,110+64+1+1,210+16+1+1);
    
    //LCD_Clear(WHITE);//清屏   
 	POINT_COLOR=BLUE;//设置字体为蓝色 
    LCD_ShowChinese(x1,y1,28,16,0);
    LCD_ShowChinese(x1+16,y1,29,16,0);
    LCD_ShowChinese(x1+2*16,y1,32,16,0);
    LCD_ShowChinese(x1+3*16,y1,33,16,0);
    
    LCD_ShowChinese(x2,y2,30,16,0);
    LCD_ShowChinese(x2+16,y2,31,16,0);
    LCD_ShowChinese(x2+2*16,y2,32,16,0);
    LCD_ShowChinese(x2+3*16,y2,33,16,0);
  	POINT_COLOR=RED;//设置画笔蓝色 
}

 //开始任务
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
    
  	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
    
    /* 创建邮箱 */
    MesageBox = OSMboxCreate((void *)0);
    
    OSTaskCreate(task1_task,(void *)0,(OS_STK*)&TASK1_TASK_STK[TASK1_STK_SIZE-1],TASK1_PRO);
    OSTaskCreate(task2_task,(void *)0,(OS_STK*)&TASK2_TASK_STK[TASK2_STK_SIZE-1],TASK2_PRO);
    OSTaskCreate(task3_task,(void *)0,(OS_STK*)&TASK3_TASK_STK[TASK3_STK_SIZE-1],TASK3_PRO);
    OSTaskCreate(task4_task,(void *)0,(OS_STK*)&TASK4_TASK_STK[TASK4_STK_SIZE-1],TASK4_PRO);
    OSTaskCreate(task5_task,(void *)0,(OS_STK*)&TASK5_TASK_STK[TASK5_STK_SIZE-1],TASK5_PRO);
    
    OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
} 

//task1任务
void task1_task(void *pdata)
{
    
    short temp; 
    u16 adcx;
    while(1)
    {
        adcx=Lsens_Get_Val();
        OSMboxPost(MesageBox, &adcx);
        LCD_ShowxNum(30+5*16,150,adcx,4,16,0);
        temp=Get_Temprate();	//得到温度值
        if(temp<0)
        {
            temp=-temp;
            LCD_ShowString(30+10*8,170,16,16,16,(unsigned char *)"-");	    //显示负号
        }else{
            LCD_ShowString(30+10*8,170,16,16,16,(unsigned char *)" ");	//无符号
        }
        LCD_ShowxNum(30+11*8,170,temp/100,2,16,0);		//显示整数部分
        LCD_ShowxNum(30+14*8,170,temp%100,2,16,0);		//显示小数部分
    };
}

//task2任务
void task2_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
    
    u8 t;
    u8 len;
    while(1)
    {
        
        if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
            if(USART_RX_BUF[0] == '1')
            {
                printf("关闭LCD\n");
                LCD_LED = 0;
            }
            if(USART_RX_BUF[0] == '2')
            {
                printf("开启LCD\n");
                LCD_LED = 1;
            }
            if(USART_RX_BUF[0] == '3')
            {
                printf("关闭采集\n");
                OSTaskDel(TASK1_PRO);
                
            }
			if(USART_RX_BUF[0] == '4')
            {
                
                printf("开始采集\n");
                OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
                OSTaskCreate(task1_task,(void *)0,(OS_STK*)&TASK1_TASK_STK[TASK1_STK_SIZE-1],TASK1_PRO);						   
                OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
            }
            printf("\r\n您发送的消息为:\r\n");
			for(t=0;t<len;t++)
			{
				USART_SendData(USART1, USART_RX_BUF[t]);         //向串口1发送数据
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			}
			printf("\r\n\r\n");//插入换行
			USART_RX_STA=0;
		}
        LED1 = 0;
        delay_ms(300);
        LED1 = 1;
        delay_ms(300);
        
    };
}

//task3任务
void task3_task(void *pdata)
{
    INT8U err;
    u16 adcx;
    while(1)
    {
        adcx = *(INT8U *)OSMboxPend(MesageBox, 0, &err);
        TIM_SetCompare1(TIM14,adcx);//修改比较值，修改占空比
    };
}

//task4任务
void task4_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
    
    u8 key;           //保存键值
#if PICTURE_ON
    int i=0;
#endif
    while(1)
    {
        tp_dev.scan(0); 		 
        if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
        {
            if(tp_dev.x[0]<lcddev.width && tp_dev.y[0]<lcddev.height)
            {
                
                if(tp_dev.x[0]>(110) &&  tp_dev.y[0]<236)
                {
                    if(tp_dev.x[0]<(158) &&  tp_dev.y[0]>210)
                    {
                        OSTaskDel(TASK1_PRO);
                        //30+10*8,170
                        LCD_ShowString(30+5*16,150,50,16,16,"   OFF    ");//显示清屏区域
                        LCD_ShowString(30+10*8,170,50,16,16,"   OFF    ");//显示清屏区域
                    }
                }
                if(tp_dev.x[0]>(35) &&  tp_dev.y[0]<236)
                {
                    if(tp_dev.x[0]<(83) &&  tp_dev.y[0]>210)
                    {
                        LCD_Clear(WHITE);
                        OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
                        OSTaskCreate(task1_task,(void *)0,(OS_STK*)&TASK1_TASK_STK[TASK1_STK_SIZE-1],TASK1_PRO);						   
                        OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
                        display();
                    }
                }
            }
        }else delay_ms(10);	//没有按键按下的时候 	  
                key=KEY_Scan(0);		//得到键值
        
	   	if(key==1)
		{	
#if PICTURE_ON
            OSTaskDel(TASK1_PRO);
            LCD_Clear(BLACK);
            if(i>2)i=0;
            if(i==0)LCD_ShowPicture(0,80,240,165,(u16*)arr[i]);
            if(i==1)LCD_ShowPicture(0,0,232,360,(u16*)arr[i]);
            if(i==2)LCD_ShowPicture(20,0,202,360,(u16*)arr[i]);
            i++;
#endif
        }
        if(key==2)
		{	
            LCD_Clear(WHITE);
            OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
            OSTaskCreate(task1_task,(void *)0,(OS_STK*)&TASK1_TASK_STK[TASK1_STK_SIZE-1],TASK1_PRO);						   
            OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
            display();
        }
    }      
}

//task5任务
void task5_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
    u8 key;           //保存键值
#if PICTURE_ON
    int i=0;
#endif
    while(1)
    {
        key=KEY_Scan(0);		//得到键值
	   	if(key==1)
		{	
#if PICTURE_ON
            OSTaskDel(TASK1_PRO);
            LCD_Clear(BLACK);
            if(i>2)i=0;
            if(i==0)LCD_ShowPicture(0,80,240,165,(u16*)arr[i]);
            if(i==1)LCD_ShowPicture(0,0,232,360,(u16*)arr[i]);
            if(i==2)LCD_ShowPicture(20,0,202,360,(u16*)arr[i]);
            i++;
#endif
        }
        if(key==2)
		{	
            LCD_Clear(WHITE);
            OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
            OSTaskCreate(task1_task,(void *)0,(OS_STK*)&TASK1_TASK_STK[TASK1_STK_SIZE-1],TASK1_PRO);						   
            OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
            display();
        }
    };  
}


