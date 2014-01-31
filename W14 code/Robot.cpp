#include "StdAfx.h"
#include "Robot.h"


bool robot_stop_flag=true;


CRobot::CRobot(void)
{
	connect_flag=false;
	robot_back_talk_flag=false;
	robot_quick_talk_flag=false;
	emergency_stop_flag=false; 
}


CRobot::~CRobot(void)
{
}


//�I��
void CRobot::Close(void){
	myRobot->disconnect();
	myConnection->close();
	connect_flag=false;
}

void CRobot::Init(void){
	//robot init
	myRobot = new ArRobot();
	myConnection = new ArSerialConnection();
	mySonar = new ArSick();

	myConnection->open();
	myRobot->setDeviceConnection(myConnection);
	myRobot->addRangeDevice(mySonar);

	if (myRobot->blockingConnect()) {
		myRobot->enableMotors();
		myRobot->runAsync(false);
	}
	connect_flag=true;


}


void CRobot::Main(void){
	mv_turn=user_data.center_x - IMAGE_WIDTH_SIZE/2;	//���S���牽pixel ����Ă��邩 �E����+
	if(-1*ROLL_PIXEL_THRESHOLD < mv_turn && mv_turn < ROLL_PIXEL_THRESHOLD){	//����ROLL_PIXEL_THRESHOLD�͈͂ɓ����Ă�Ȃ璼�i
		mv_forward = user_data.center_depth-LENGHT_HUMAN_ROBOT;
		if(MAX_LENGHT_HUMAN_ROBOT > user_data.center_depth && user_data.center_depth >LENGHT_HUMAN_ROBOT+LENGHT_HUMAN_ROBOT_THRESHOLD){	//���i�̓K���͈͂ɓ����Ă��邩
			if(mv_forward * ROBOT_SPEED_P < MAX_SPEED){
				myRobot->setVel2(mv_forward * ROBOT_SPEED_P , mv_forward * ROBOT_SPEED_P);	
			}else{
				myRobot->setVel2(MAX_SPEED , MAX_SPEED);	
			}

		}else if(user_data.center_depth < LENGHT_HUMAN_ROBOT-LENGHT_HUMAN_ROBOT_THRESHOLD){		//��ޔ͈͂ɓ����Ă��邩
			myRobot->setVel2( -ROBOT_BACK_SPEED, -ROBOT_BACK_SPEED);
		}else{
			myRobot->stop();

		}
	}else{																	///////////////���S�ɐl�����Ȃ��Ƃ�/////////////
		mv_forward = user_data.center_depth-LENGHT_HUMAN_ROBOT;												
		if( MAX_LENGHT_HUMAN_ROBOT > user_data.center_depth){
			if(user_data.center_depth >LENGHT_HUMAN_ROBOT+LENGHT_HUMAN_ROBOT_THRESHOLD ){
				if(mv_forward * ROBOT_SPEED_P < MAX_SPEED){
					myRobot -> setVel2((mv_forward* ROBOT_SPEED_P)  + (mv_turn * ROBOT_ROLL_SPEED_P) , (mv_forward* ROBOT_SPEED_P)  - (mv_turn * ROBOT_ROLL_SPEED_P));		//���̒l�𐳊m�Ɍ��߂Ȃ���
				}else{
					myRobot -> setVel2(MAX_SPEED  + (mv_turn * ROBOT_ROLL_SPEED_P) , MAX_SPEED  - (mv_turn * ROBOT_ROLL_SPEED_P));
				}
			}else{
				myRobot->setVel2(mv_turn * ROBOT_ROLL_SPEED_P,-1 * mv_turn * ROBOT_ROLL_SPEED_P);
			}
		}else{
			myRobot->stop();
		}
	}
	//�~�܂��Ă��邩�ǂ�������
	if(myRobot->getVel() == 0){
		robot_stop_flag=true;
	}else{
		robot_stop_flag=false;
	}


	RobotTalk();

}

void CRobot::RobotTalk(void){
	//������Ǘ�
	if(user_data.center_depth < LENGHT_HUMAN_ROBOT-LENGHT_HUMAN_ROBOT_THRESHOLD){	//�߂��Ƃ�
		if(robot_back_talk_flag == false){
			Talk.TalkLanguage(TALK_NEAR);
			robot_back_talk_flag=true;
		}
	}else {
		robot_back_talk_flag=false;
	}
	if(MAX_LENGHT_HUMAN_ROBOT-1000 < user_data.center_depth){	//���[�U�������Ƃ�
		if(robot_quick_talk_flag==false){
			Talk.TalkLanguage(TALK_QUICK);
			robot_quick_talk_flag=true;
		}
	}else{
		robot_quick_talk_flag=false;
	}
}

//�\�i�[���擾
void CRobot::Sonar(void){
	CString str;
	for(int sonar_num=0 ; sonar_num<MAX_SONAR_NUM ; sonar_num++){
		sonar_val[sonar_num]=myRobot->getSonarRange(sonar_num);
		sonar_level[sonar_num]="";
		for(int j=0 ; j<sonar_val[sonar_num]/MIN_SONAR_RANGE ; j++){
			sonar_level[sonar_num]+="��";
		}
		str.Format(_T("%d"),sonar_val[sonar_num]);
		sonar_level[sonar_num] += str;
	}
}

void CRobot::EmergencyStop(void){
	int near_sonar_count=0;
	for(int sonar_num=2 ; sonar_num<=5 ; sonar_num++){
		if(sonar_val[sonar_num] < SONAR_NEAR_MAX){
			near_sonar_count++;
			if(emergency_stop_flag==false){
				emergency_stop_flag=true;
				Talk.TalkLanguage(TALK_EMERGENCY);
			}
		}
	}
	if(near_sonar_count==0){
		emergency_stop_flag=false;
	}
}