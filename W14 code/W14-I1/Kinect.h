#pragma once
#include<XnCppWrapper.h>
#include<iostream>
#include<stdexcept>
#include<opencv/cv.h>
#include<opencv/highgui.h>
#include<fstream>
#include"Talk.h"
#define CONFIG_XML_PATH "./config.xml"	//�ݒ�PATH


#include <stdio.h>

#define CV_MAX_THREADS 1
#define  CV_SQR(a) ((a) * (a))

#define TEMPLATESIZE_X	60
#define TEMPLATESIZE_Y	120
#define DEPTH_IMG_RANGE 500   //[mm]
#define MATTING_X_RANGE 200 //pixel
#define MATTING_Y_RANGE 100 //pixel
#define USER_DISSIMILARITY 7500 //����x
#define USER_PIXEL_RANGE 30000 //pixel
//�e���v���[�g�摜PATH
#define FRONT_TEMPLATE_PATH "./usertemplate/template1.jpg"	

//�֊s�ǐՂ���Ƃ�
#define DEPTH_HIGTH_LIMIT 100	//[mm]
#define D_UPPER 1
#define D_RIGHT 2
#define D_UNDER 3
#define D_LEFT 4
//���[�U�����X�g���鎞��臒l
#define MAX_LOST_COUNT 15
#define MAX_USER_NUM 16					//15�l�܂œo�^�\(�z��̂���+1�Őݒ�)
//ZNCC�}�b�`���O臒l
#define TEMPLATE_SIMILARITY 0.3

#define ROLL_PIXEL_THRESHOLD 60		//����]��臒l Robot.h�ɍ��킹��

#define MAX_NULL_POINT_NUM	1800			//�e���v���[�g�X�V����  7200/4

//�W�F�X�`���[�摜�̐[�x�͈�
#define GESTURE_DEPTH_MIN_LIMIT	150
#define GESTURE_DEPTH_MAX_LIMIT 500


//��p�p�����[�^
#define START_FACE 1
#define HELLO_FACE 2
#define TRACKING_FACE 3
#define LOST_FACE 4
#define BYEBYE_FACE 5
#define EMERGENCY_FACE 6


//�}�E�X�̍\����
struct Mouse{
	bool flag;
	int pointX;
	int pointY;
};

//���[�U�̍\����
struct UserInfo{
	double hight;		//�g��
	int upper_end;		//���[�U�̏�[
	int lower_end;		//        ���[
	int left_end;		//		  ���[
	int right_end;		//		  �E�[
	int center_x;		//���S_x
	int center_y;		//���S_y
	int center_depth;	//�d�S�̐[�x
	int pixel_num;		//���[�U��pixel��
};

class CKinect
{
public:
	CKinect(void);
	~CKinect(void);
	void Init();
	void Main();
	UserInfo user;			//�o�^���[�U
	bool main_sign_in_flag;		//2��ڂ̃e���v���[�g�o�^flag
	bool lost_flag;			//���[�U�������t���O
	bool byebye_flag;		//���[�U�ǐՏI��

	//���[�U��X����
	double user_x;

	//�J���}���t�B���^�̒l����
	double camera_prediction_pos_x , camera_prediction_pos_d;	//�\�z�̒l
	bool user_reprobe_flag;
	double kalman_range_diff;

	//��p�p�����[�^
	int face_num;

private:
	void CheckOpenNIError(XnStatus status);
	//���s��̃X�e�[�^�X
	XnStatus rc;
	xn::Context context;
	XnMapOutputMode outputMode;

	//�W�F�l���[�^
	xn::ImageGenerator imageGenerator;
	xn::DepthGenerator depthGenerator;
	xn::UserGenerator userGenerator;

	//���^�f�[�^
	xn::ImageMetaData imageMD;
	xn::SceneMetaData sceneMD;
	xn::DepthMetaData depthMD;

	//��v�摜
	IplImage *user_img;
	IplImage *depth_img;
	IplImage *camera;

	//���[�U�o�^�֌W
	void UserSignin();
	bool first_sign_in_flag;	//1��ڂ̃e���v���[�g�o�^flag
	int user_number;
	int init_user_count;	//����ɂ��͂̎��̃G���[�J�E���^
	int user_count;
	int upper_count;
	int lower_count;

	//���[�U�؏o��
	void UserDetection();

	//�g������
	double Measure(int x, int y , int depth);

	//���[�U���� DEPTHP_IMG_RANGE mm�O��̐[�x�摜
	void MakeDepthImage(void);

	//�f�[�^���W �e���v���[�g�}�b�`���O���āA���̃��[�U����`�ň͂�etc
	void DataCollection(void);

	IplImage *temp_img;
	double temp_depth;	//�e���v���[�g�摜�[�x���擾

	IplImage *update_temp_img;	//�{�l�̉\���̂���e���v���[�g�摜�̕ۑ�
	UserInfo update_user;
	bool temp_user_flag;		//�e���v���[�g���{�l���ǂ���

	//�{�l�F�؁A�{�l�f�[�^�̍X�V
	void UserAuthentication(void);

	//���[�U�̂ݐg����Ԃ�(���ʂ���̐[�x����p����)�֊s�ǐ�
	double UserHightData(int point_x,int point_y);

	//�ŏ��̗֊s��������
	CvPoint FindContour(int point_x,int point_y);
	int x_min;
	int x_max;
	int x_when_y_min;
	int y_min;
	int y_max;

	//�{�l���������Ă����
	int lost_count;

	//���[�U������������
	void UserReprobe(void);
	UserInfo userInfo[MAX_USER_NUM];

	//Robot����̏����C���[�W
	void MakeRobotImage(void);
	IplImage *robot_img;
	IplImage *c_robot_img;

	//���b�֌W
	CTalk Talk;


	IplImage *gesture_img;	//�W�F�X�`���[��F������摜
	//�o�C�o�C���o
	void ByeByeSign(void);
	CvPoint hand_point;		//���̈ʒu��ۑ�
	CvPoint first_hand_point;
	int hand_count;			//�肪���邩���o
	int right_count;
	int left_count;
	int init_hand_count;	//����������J�E���^
	int start_count;

	//�w�i�𔲂����e���v���[�g�}�b�`���O
	void ExpectBackTemplateMatting(void);
	double temp_min_val;
	CvPoint temp_min_point;
};

