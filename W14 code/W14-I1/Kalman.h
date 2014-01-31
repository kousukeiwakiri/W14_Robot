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
	double human_x , human_d;	//‰æ‘œŒnhuman ’†S‚ª(0,0) 0mm
	double robot_x , robot_y , robot_w;	//ƒƒ{ƒbƒgÀ•WŒnrobot
	double range_diff;	//’Tõ”ÍˆÍ
	double camera_prediction_pos_x , camera_prediction_pos_d;	//—\‘ªˆÊ’ucameraÀ•WŒn
	bool kalman_init_flag;

private:
	void init(void);
	double world_h_x , world_h_y;	//¢ŠEÀ•WŒnhuman
	double world_r_x , world_r_y;	//¢ŠEÀ•WŒnrobot
	double delta_x , delta_y;	//¢ŠEÀ•WŒn,lŠÔ‚Ìdelta
	double world_r_w ;	//¢ŠEÀ•WŒn‚ÌŠp“x;

	void RobotToWorld(void);


	void KalmanFilter(void);

	CvKalman* kalman;
	double first_h_x , first_h_y;
	double world_prediction_pos_x,world_prediction_pos_y;


	void WorldToCamera(void);
};

