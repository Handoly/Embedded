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

/////////////////////////UCOSII��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			15 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				128
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);	
 			   
//task1����
//�����������ȼ�
#define TASK1_PRO       			6 
//���������ջ��С
#define TASK1_STK_SIZE  		    		128
//�����ջ	
OS_STK TASK1_TASK_STK[TASK1_STK_SIZE];
//������
void task1_task(void *pdata);

//task2����
//�����������ȼ�
#define TASK2_PRO       			7
//���������ջ��С
#define TASK2_STK_SIZE  					128
//�����ջ
OS_STK TASK2_TASK_STK[TASK2_STK_SIZE];
//������
void task2_task(void *pdata);

//task3����
//�����������ȼ�
#define TASK3_PRO       			8 
//���������ջ��С
#define TASK3_STK_SIZE  					128
//�����ջ
OS_STK TASK3_TASK_STK[TASK3_STK_SIZE];
//������
void task3_task(void *pdata);

//task4����
//�����������ȼ�
#define TASK4_PRO       			9
//���������ջ��С
#define TASK4_STK_SIZE  					128
//�����ջ
OS_STK TASK4_TASK_STK[TASK4_STK_SIZE];
//������
void task4_task(void *pdata);

//task5����
//�����������ȼ�
#define TASK5_PRO       			10 
//���������ջ��С
#define TASK5_STK_SIZE  					128
//�����ջ
OS_STK TASK5_TASK_STK[TASK5_STK_SIZE];
//������
void task5_task(void *pdata);

/* ����һ�������¼�ָ�� */
OS_EVENT *MesageBox;

#define SEND_BUF_SIZE 8200	//�������ݳ���,��õ���sizeof(TEXT_TO_SEND)+2��������.

u8 SendBuff[SEND_BUF_SIZE];	//�������ݻ�����

void display(void);

#if PICTURE_ON
const unsigned char *arr[3] = {gImage_cp,gImage_zyy,gImage_lyf};
#endif

//�����Ļ�������Ͻ���ʾ"RST"
void Load_Drow_Dialog(void)
{
    int x1 = 35;
    int x2 = 110;
    int y1 = 210;
    int y2 = 210;
	//LCD_Clear(WHITE);//����   
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
    LCD_ShowChinese(x1,y1,28,16,0);
    LCD_ShowChinese(x1+16,y1,29,16,0);
    LCD_ShowChinese(x1+2*16,y1,32,16,0);
    LCD_ShowChinese(x1+3*16,y1,33,16,0);
    
    LCD_ShowChinese(x2,y2,30,16,0);
    LCD_ShowChinese(x2+16,y2,31,16,0);
    LCD_ShowChinese(x2+2*16,y2,32,16,0);
    LCD_ShowChinese(x2+3*16,y2,33,16,0);
  	POINT_COLOR=RED;//���û�����ɫ 
}


//������֮��ľ���ֵ 
//x1,x2����ȡ��ֵ��������
//����ֵ��|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  

int main(void)
{ 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);		  //��ʼ����ʱ����
    uart_init(115200);	//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		        //��ʼ��LED�˿� 
    LCD_Init();         //��ʼ��LCD�ӿ�
    Adc_Init();
    Adc3_Init();         //��ʼ��ADC
//    BEEP_Init();
    Lsens_Init();           //��ʼ��������ߴ�����
    TIM14_PWM_Init(100-1,420-1);
    KEY_Init();
    tp_dev.init();				//��������ʼ��
    display();
    OSInit();   
    OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
    OSStart();
}

void display(void)
{
    int x1 = 35;
    int x2 = 110;
    int y1 = 210;
    int y2 = 210;
    
    int i=0;
    POINT_COLOR=RED;//��������Ϊ��ɫ
    LCD_ShowString(30,50,200,16,16,(unsigned char *)"Explorer STM32F4");	
    for(i=0;i<8;i++)LCD_ShowChinese(30+i*16,70,i,16,0);//������������⡿
    for(i=0;i<3;i++)LCD_ShowChinese(30+i*16,90,i+8,16,0);//���ڣ�
    LCD_ShowString(30+3*16,90,200,16,16,(unsigned char *)"2024/3/29");	  
    POINT_COLOR=BLUE;//��������Ϊ��ɫ
    
    LCD_ShowChinese(30,110,25,16,0);//��
    LCD_ShowChinese(78,110,26,16,0);//��
    LCD_ShowChinese(94,110,27,16,0);//��
    for(i=0;i<3;i++)LCD_ShowChinese(100+i*16,110,i+11,16,0);//����
    LCD_ShowChinese(30,130,14,16,0);//ѧ
    LCD_ShowChinese(78,130,15,16,0);//��
    LCD_ShowChinese(94,130,16,16,0);//��
    LCD_ShowNum(40+3*20,130,2021213068,10,16);  //2021213068
    for(i=0;i<5;i++)LCD_ShowChinese(30+i*16,150,i+17,16,0);//����ǿ�ȣ�
    LCD_ShowString(30,170,200,16,16,(unsigned char *)"           00.00C");//���ڹ̶�λ����ʾС����	
    LCD_ShowChinese(30,170,22,16,0);//��
    LCD_ShowChinese(78,170,23,16,0);//�ȣ�
    LCD_ShowChinese(94,170,24,16,0);//
    
    POINT_COLOR=BLACK;      //������ɫ����ɫ
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
    
    //LCD_Clear(WHITE);//����   
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
    LCD_ShowChinese(x1,y1,28,16,0);
    LCD_ShowChinese(x1+16,y1,29,16,0);
    LCD_ShowChinese(x1+2*16,y1,32,16,0);
    LCD_ShowChinese(x1+3*16,y1,33,16,0);
    
    LCD_ShowChinese(x2,y2,30,16,0);
    LCD_ShowChinese(x2+16,y2,31,16,0);
    LCD_ShowChinese(x2+2*16,y2,32,16,0);
    LCD_ShowChinese(x2+3*16,y2,33,16,0);
  	POINT_COLOR=RED;//���û�����ɫ 
}

 //��ʼ����
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
    
  	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
    
    /* �������� */
    MesageBox = OSMboxCreate((void *)0);
    
    OSTaskCreate(task1_task,(void *)0,(OS_STK*)&TASK1_TASK_STK[TASK1_STK_SIZE-1],TASK1_PRO);
    OSTaskCreate(task2_task,(void *)0,(OS_STK*)&TASK2_TASK_STK[TASK2_STK_SIZE-1],TASK2_PRO);
    OSTaskCreate(task3_task,(void *)0,(OS_STK*)&TASK3_TASK_STK[TASK3_STK_SIZE-1],TASK3_PRO);
    OSTaskCreate(task4_task,(void *)0,(OS_STK*)&TASK4_TASK_STK[TASK4_STK_SIZE-1],TASK4_PRO);
    OSTaskCreate(task5_task,(void *)0,(OS_STK*)&TASK5_TASK_STK[TASK5_STK_SIZE-1],TASK5_PRO);
    
    OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
} 

//task1����
void task1_task(void *pdata)
{
    
    short temp; 
    u16 adcx;
    while(1)
    {
        adcx=Lsens_Get_Val();
        OSMboxPost(MesageBox, &adcx);
        LCD_ShowxNum(30+5*16,150,adcx,4,16,0);
        temp=Get_Temprate();	//�õ��¶�ֵ
        if(temp<0)
        {
            temp=-temp;
            LCD_ShowString(30+10*8,170,16,16,16,(unsigned char *)"-");	    //��ʾ����
        }else{
            LCD_ShowString(30+10*8,170,16,16,16,(unsigned char *)" ");	//�޷���
        }
        LCD_ShowxNum(30+11*8,170,temp/100,2,16,0);		//��ʾ��������
        LCD_ShowxNum(30+14*8,170,temp%100,2,16,0);		//��ʾС������
    };
}

//task2����
void task2_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
    
    u8 t;
    u8 len;
    while(1)
    {
        
        if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
            if(USART_RX_BUF[0] == '1')
            {
                printf("�ر�LCD\n");
                LCD_LED = 0;
            }
            if(USART_RX_BUF[0] == '2')
            {
                printf("����LCD\n");
                LCD_LED = 1;
            }
            if(USART_RX_BUF[0] == '3')
            {
                printf("�رղɼ�\n");
                OSTaskDel(TASK1_PRO);
                
            }
			if(USART_RX_BUF[0] == '4')
            {
                
                printf("��ʼ�ɼ�\n");
                OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
                OSTaskCreate(task1_task,(void *)0,(OS_STK*)&TASK1_TASK_STK[TASK1_STK_SIZE-1],TASK1_PRO);						   
                OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
            }
            printf("\r\n�����͵���ϢΪ:\r\n");
			for(t=0;t<len;t++)
			{
				USART_SendData(USART1, USART_RX_BUF[t]);         //�򴮿�1��������
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}
			printf("\r\n\r\n");//���뻻��
			USART_RX_STA=0;
		}
        LED1 = 0;
        delay_ms(300);
        LED1 = 1;
        delay_ms(300);
        
    };
}

//task3����
void task3_task(void *pdata)
{
    INT8U err;
    u16 adcx;
    while(1)
    {
        adcx = *(INT8U *)OSMboxPend(MesageBox, 0, &err);
        TIM_SetCompare1(TIM14,adcx);//�޸ıȽ�ֵ���޸�ռ�ձ�
    };
}

//task4����
void task4_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
    
    u8 key;           //�����ֵ
#if PICTURE_ON
    int i=0;
#endif
    while(1)
    {
        tp_dev.scan(0); 		 
        if(tp_dev.sta&TP_PRES_DOWN)			//������������
        {
            if(tp_dev.x[0]<lcddev.width && tp_dev.y[0]<lcddev.height)
            {
                
                if(tp_dev.x[0]>(110) &&  tp_dev.y[0]<236)
                {
                    if(tp_dev.x[0]<(158) &&  tp_dev.y[0]>210)
                    {
                        OSTaskDel(TASK1_PRO);
                        //30+10*8,170
                        LCD_ShowString(30+5*16,150,50,16,16,"   OFF    ");//��ʾ��������
                        LCD_ShowString(30+10*8,170,50,16,16,"   OFF    ");//��ʾ��������
                    }
                }
                if(tp_dev.x[0]>(35) &&  tp_dev.y[0]<236)
                {
                    if(tp_dev.x[0]<(83) &&  tp_dev.y[0]>210)
                    {
                        LCD_Clear(WHITE);
                        OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
                        OSTaskCreate(task1_task,(void *)0,(OS_STK*)&TASK1_TASK_STK[TASK1_STK_SIZE-1],TASK1_PRO);						   
                        OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
                        display();
                    }
                }
            }
        }else delay_ms(10);	//û�а������µ�ʱ�� 	  
                key=KEY_Scan(0);		//�õ���ֵ
        
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
            OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
            OSTaskCreate(task1_task,(void *)0,(OS_STK*)&TASK1_TASK_STK[TASK1_STK_SIZE-1],TASK1_PRO);						   
            OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
            display();
        }
    }      
}

//task5����
void task5_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
    u8 key;           //�����ֵ
#if PICTURE_ON
    int i=0;
#endif
    while(1)
    {
        key=KEY_Scan(0);		//�õ���ֵ
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
            OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
            OSTaskCreate(task1_task,(void *)0,(OS_STK*)&TASK1_TASK_STK[TASK1_STK_SIZE-1],TASK1_PRO);						   
            OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
            display();
        }
    };  
}


