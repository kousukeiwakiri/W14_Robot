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

//���{�b�g
#define ROBOT_SPEED_P 0.6 //[mm^2/s] 1mm����Ă���0.1mm/s 1000mm����Ă���100mm/s
#define ROBOT_ROLL_SPEED_P 0.45 //��]�̑��x[mm/s]
#define ROBOT_BACK_SPEED 40	//[mm/s] ��ނ͈��
#define ROLL_PIXEL_THRESHOLD 80		//����]��臒l kinect.h�ɍ��킹��
#define IMAGE_WIDTH_SIZE 640	//��ʂ̕��@kinect�̐ݒ�ɍ��킹��
#define LENGHT_HUMAN_ROBOT 975	//�l�Ԃƃ��{�b�g�̓K������
#define LENGHT_HUMAN_ROBOT_THRESHOLD 150	//�c�����ւ�臒l (���U���Ȃ��悤��)
#define MAX_LENGHT_HUMAN_ROBOT 2500			//�����֖\�����Ȃ��悤��
#define MAX_SONAR_NUM 8		//���{�b�g�̃\�[�i�[��
#define MIN_SONAR_RANGE 250	//�\�i�[�̃����W�l
#define SONAR_NEAR_MAX 500	//�\�i�[�̉�������	150~7000[mm]
#define MAX_SPEED 1000 //[mm/s]


struct UserData{
	int center_x;			//�d�S_x
	double center_depth;
};

class CRobot
{
public:
	CRobot(void);
	~CRobot(void);
	bool connect_flag;	//�ڑ�����Ă��邩
	void Init();
	void Main();
	void Close();	//�I��
	UserData user_data;

	//���{�b�g
	ArRobot *myRobot;
	ArSerialConnection *myConnection;
	ArSick *mySonar;

	//�\�i�[�֌W
	void Sonar();
	int sonar_val[MAX_SONAR_NUM];
	CString sonar_level[MAX_SONAR_NUM];
	void EmergencyStop();
	//�ً}��~
	bool emergency_stop_flag;

	bool robot_backflag;

private:
	void Move();	//���{�b�g�̈ړ�
	int mv_turn;	//���S���牽pixel����Ă��邩
	double mv_forward;	//�K����������ǂ̂��炢����Ă��邩
	//���b�֌W
	CTalk Talk;

	bool robot_back_talk_flag;
	bool robot_quick_talk_flag;

	void RobotTalk(void);

	//std::ofstream ofs;	//�I�h���g���m�F
	clock_t time;



};

