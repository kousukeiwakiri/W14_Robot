#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <math.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>
#include <opencv2\video\tracking.hpp>

class CKalman
{
public:
	CKalman(void);
	~CKalman(void);
	void main(void);
	double human_x , human_d;	//�摜�nhuman ���S��(0,0) 0mm
	double robot_x , robot_y , robot_w;	//���{�b�g���W�nrobot
	double range_diff;	//�T���͈�
	double camera_prediction_pos_x , camera_prediction_pos_d;	//�\���ʒucamera���W�n
	bool kalman_init_flag;

private:
	void init(void);
	double world_h_x , world_h_y;	//���E���W�nhuman
	double world_r_x , world_r_y;	//���E���W�nrobot
	double delta_x , delta_y;	//���E���W�n,�l�Ԃ�delta
	double world_r_w ;	//���E���W�n�̊p�x;

	void RobotToWorld(void);


	void KalmanFilter(void);

	CvKalman* kalman;
	double first_h_x , first_h_y;
	double world_prediction_pos_x,world_prediction_pos_y;


	void WorldToCamera(void);
};

