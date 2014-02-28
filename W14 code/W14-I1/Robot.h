#pragma once
#include "Aria.h"
#ifdef _DEBUG
#pragma comment(lib,"AriaDebugVC10.lib")
#else
#pragma comment(lib,"AriaVC10.lib")
#endif
#include<iostream>
#include<stdexcept>
#include <fstream>
#include"Talk.h"
#include <time.h>

//ロボット
#define ROBOT_SPEED_P 0.6 //[mm^2/s] 1mm離れてたら0.1mm/s 1000mm離れてたら100mm/s
#define ROBOT_ROLL_SPEED_P 0.45 //回転の速度[mm/s]
#define ROBOT_BACK_SPEED 40	//[mm/s] 後退は一定
#define ROLL_PIXEL_THRESHOLD 80		//横回転の閾値 kinect.hに合わせる
#define IMAGE_WIDTH_SIZE 640	//画面の幅　kinectの設定に合わせる
#define LENGHT_HUMAN_ROBOT 975	//人間とロボットの適正距離
#define LENGHT_HUMAN_ROBOT_THRESHOLD 150	//縦方向への閾値 (発散しないように)
#define MAX_LENGHT_HUMAN_ROBOT 2500			//遠くへ暴走しないように
#define MAX_SONAR_NUM 8		//ロボットのソーナー数
#define MIN_SONAR_RANGE 250	//ソナーのレンジ値
#define SONAR_NEAR_MAX 500	//ソナーの応答距離	150~7000[mm]
#define MAX_SPEED 1000 //[mm/s]


struct UserData{
	int center_x;			//重心_x
	double center_depth;
};

class CRobot
{
public:
	CRobot(void);
	~CRobot(void);
	bool connect_flag;	//接続されているか
	void Init();
	void Main();
	void Close();	//終了
	UserData user_data;

	//ロボット
	ArRobot *myRobot;
	ArSerialConnection *myConnection;
	ArSick *mySonar;

	//ソナー関係
	void Sonar();
	int sonar_val[MAX_SONAR_NUM];
	CString sonar_level[MAX_SONAR_NUM];
	void EmergencyStop();
	//緊急停止
	bool emergency_stop_flag;

	bool robot_backflag;

private:
	void Move();	//ロボットの移動
	int mv_turn;	//中心から何pixel離れているか
	double mv_forward;	//適正距離からどのくらい離れているか
	//発話関係
	CTalk Talk;

	bool robot_back_talk_flag;
	bool robot_quick_talk_flag;

	void RobotTalk(void);

	//std::ofstream ofs;	//オドメトリ確認
	clock_t time;



};

