#ifndef __JUDG_INFO_H__
#define __JUDG_INFO_H__

#include "stm32f4xx.h"


#define JudgeBufferLength       150
#define JudgeFrameHeader        0xA5        //帧头 


#ifdef  __DRIVER_GLOBALS
#define __DRIVER_EXT
#else
#define __DRIVER_EXT extern
#endif

typedef enum
{
  WEI_KAI_SHI,            //自己拼
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


//机器人状态
typedef __packed struct
{
 uint16_t remain_time;   //比赛剩余时间
 uint8_t gameProgress;   //比赛状态
 uint8_t robotLevel;     //机器人等级
 uint16_t robotHp;       //机器人血量
 uint16_t robotbuff;     //机器人BUFF
}GameRobotState_t;


//实时射击信息结构体
typedef __packed struct
{
 uint8_t bulletType;   				//子弹类型
 uint8_t bulletFreq;					//子弹射频
 float bulletSpeed;						//子弹速度
 float bulletSpeed_last;
}RealShootData_Struct;

//机器人位置朝向信息
typedef __packed struct
{
 float x;
 float y;
 float z;
 float yaw;
}GameRobotPos_Struct;


//裁判系统结构体
typedef struct
{
 float RealVoltage;                  //实时电压
 float RealCurrent;                  //实时电流
 float	remainPower;									//剩余能量
 float	last_power;										//上一状态功率
 float	power;												//实时功率
 float k_power;											//功率变化
 uint16_t shooterHeat17;                  //17mm枪口热量
 uint16_t shooterHeat42;                 //42mm枪口热量
 float LastShotSpeed;                //上次射击速度
#if INFANTRY == 7
	uint16_t ShootNum;                  //已发射子弹数
	uint8_t BulletUseUp;                //1 基地子弹射完          0 基地子弹未射完
	uint16_t ShootFail;                 //发射失败时间 
#endif
}InfantryJudge_Struct;


//裁判系统数据缓存
__DRIVER_EXT uint8_t JudgeDataBuffer[JudgeBufferLength];
//实时电压
__DRIVER_EXT InfantryJudge_Struct InfantryJudge;
//帧率计数器
__DRIVER_EXT float JudgeFrameCounter;
//帧率
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
