#ifndef __JUDG_INFO_H__
#define __JUDG_INFO_H__

#include "stm32f4xx.h"


#define JudgeBufferLength       150
#define JudgeFrameHeader        0xA5        //֡ͷ 


#ifdef  __DRIVER_GLOBALS
#define __DRIVER_EXT
#else
#define __DRIVER_EXT extern
#endif

typedef enum
{
  WEI_KAI_SHI,            //�Լ�ƴ
	ZHUN_BEI,
	ZI_JIAN,
	DAO_JI_SHI,
	DUI_ZHAN_ZHONG,
	JIE_SUAN,
}GameState_t;

typedef union
{
 uint8_t U[4];
 float FF;
 int I;
}FormatTrans;


//������״̬
typedef __packed struct
{
 uint16_t remain_time;   //����ʣ��ʱ��
 uint8_t gameProgress;   //����״̬
 uint8_t robotLevel;     //�����˵ȼ�
 uint16_t robotHp;       //������Ѫ��
 uint16_t robotbuff;     //������BUFF
}GameRobotState_t;


//ʵʱ�����Ϣ�ṹ��
typedef __packed struct
{
 uint8_t bulletType;   				//�ӵ�����
 uint8_t bulletFreq;					//�ӵ���Ƶ
 float bulletSpeed;						//�ӵ��ٶ�
 float bulletSpeed_last;
}RealShootData_Struct;

//������λ�ó�����Ϣ
typedef __packed struct
{
 float x;
 float y;
 float z;
 float yaw;
}GameRobotPos_Struct;


//����ϵͳ�ṹ��
typedef struct
{
 float RealVoltage;                  //ʵʱ��ѹ
 float RealCurrent;                  //ʵʱ����
 float	remainPower;									//ʣ������
 float	last_power;										//��һ״̬����
 float	power;												//ʵʱ����
 float k_power;											//���ʱ仯
 uint16_t shooterHeat17;                  //17mmǹ������
 uint16_t shooterHeat42;                 //42mmǹ������
 float LastShotSpeed;                //�ϴ�����ٶ�
#if INFANTRY == 7
	uint16_t ShootNum;                  //�ѷ����ӵ���
	uint8_t BulletUseUp;                //1 �����ӵ�����          0 �����ӵ�δ����
	uint16_t ShootFail;                 //����ʧ��ʱ�� 
#endif
}InfantryJudge_Struct;


//����ϵͳ���ݻ���
__DRIVER_EXT uint8_t JudgeDataBuffer[JudgeBufferLength];
//ʵʱ��ѹ
__DRIVER_EXT InfantryJudge_Struct InfantryJudge;
//֡�ʼ�����
__DRIVER_EXT float JudgeFrameCounter;
//֡��
__DRIVER_EXT float JudgeFrameRate;

__DRIVER_EXT RealShootData_Struct bullet_data;

__DRIVER_EXT GameRobotState_t GameRobotState;

__DRIVER_EXT GameRobotPos_Struct GameRobotPos;


unsigned char Get_CRC8_Check_Sum_1(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8);
unsigned int Verify_CRC8_Check_Sum_1(unsigned char *pchMessage, unsigned int dwLength);
void Append_CRC8_Check_Sum_1(unsigned char *pchMessage, unsigned int dwLength);
uint16_t Get_CRC16_Check_Sum_1(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);
uint32_t Verify_CRC16_Check_Sum_1(uint8_t *pchMessage, uint32_t dwLength);
void Append_CRC16_Check_Sum_1(uint8_t * pchMessage,uint32_t dwLength);
    
void Judge_InitConfig(void);

void Judge_Receive(uint8_t *pData);


#endif
