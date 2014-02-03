#pragma once
#include<XnCppWrapper.h>
#include<iostream>
#include<stdexcept>
#include<opencv/cv.h>
#include<opencv/highgui.h>
#include<fstream>
#include"Talk.h"
#define CONFIG_XML_PATH "./config.xml"	//設定PATH


#include <stdio.h>

#define CV_MAX_THREADS 1
#define  CV_SQR(a) ((a) * (a))

#define TEMPLATESIZE_X	60
#define TEMPLATESIZE_Y	120
#define DEPTH_IMG_RANGE 500   //[mm]
#define MATTING_X_RANGE 200 //pixel
#define MATTING_Y_RANGE 100 //pixel
#define USER_DISSIMILARITY 7500 //相違度
#define USER_PIXEL_RANGE 30000 //pixel
//テンプレート画像PATH
#define FRONT_TEMPLATE_PATH "./usertemplate/template1.jpg"	

//輪郭追跡するとき
#define DEPTH_HIGTH_LIMIT 100	//[mm]
#define D_UPPER 1
#define D_RIGHT 2
#define D_UNDER 3
#define D_LEFT 4
//ユーザがロストする時の閾値
#define MAX_LOST_COUNT 15
#define MAX_USER_NUM 16					//15人まで登録可能(配列のため+1で設定)
//ZNCCマッチング閾値
#define TEMPLATE_SIMILARITY 0.3

#define ROLL_PIXEL_THRESHOLD 60		//横回転の閾値 Robot.hに合わせる

#define MAX_NULL_POINT_NUM	1800			//テンプレート更新判定  7200/4

//ジェスチャー画像の深度範囲
#define GESTURE_DEPTH_MIN_LIMIT	150
#define GESTURE_DEPTH_MAX_LIMIT 500


//顔用パラメータ
#define START_FACE 1
#define HELLO_FACE 2
#define TRACKING_FACE 3
#define LOST_FACE 4
#define BYEBYE_FACE 5
#define EMERGENCY_FACE 6


//マウスの構造体
struct Mouse{
	bool flag;
	int pointX;
	int pointY;
};

//ユーザの構造体
struct UserInfo{
	double hight;		//身長
	int upper_end;		//ユーザの上端
	int lower_end;		//        下端
	int left_end;		//		  左端
	int right_end;		//		  右端
	int center_x;		//中心_x
	int center_y;		//中心_y
	int center_depth;	//重心の深度
	int pixel_num;		//ユーザのpixel数
};

class CKinect
{
public:
	CKinect(void);
	~CKinect(void);
	void Init();
	void Main();
	UserInfo user;			//登録ユーザ
	bool main_sign_in_flag;		//2回目のテンプレート登録flag
	bool lost_flag;			//ユーザ見失いフラグ
	bool byebye_flag;		//ユーザ追跡終了

	//ユーザのX距離
	double user_x;

	//カルマンフィルタの値たち
	double camera_prediction_pos_x , camera_prediction_pos_d;	//予想の値
	bool user_reprobe_flag;
	double kalman_range_diff;

	//顔用パラメータ
	int face_num;

private:
	void CheckOpenNIError(XnStatus status);
	//実行後のステータス
	XnStatus rc;
	xn::Context context;
	XnMapOutputMode outputMode;

	//ジェネレータ
	xn::ImageGenerator imageGenerator;
	xn::DepthGenerator depthGenerator;
	xn::UserGenerator userGenerator;

	//メタデータ
	xn::ImageMetaData imageMD;
	xn::SceneMetaData sceneMD;
	xn::DepthMetaData depthMD;

	//主要画像
	IplImage *user_img;
	IplImage *depth_img;
	IplImage *camera;

	//ユーザ登録関係
	void UserSignin();
	bool first_sign_in_flag;	//1回目のテンプレート登録flag
	int user_number;
	int init_user_count;	//こんにちはの時のエラーカウンタ
	int user_count;
	int upper_count;
	int lower_count;

	//ユーザ切出し
	void UserDetection();

	//身長測定
	double Measure(int x, int y , int depth);

	//ユーザから DEPTHP_IMG_RANGE mm前後の深度画像
	void MakeDepthImage(void);

	//データ収集 テンプレートマッチングして、そのユーザを矩形で囲むetc
	void DataCollection(void);

	IplImage *temp_img;
	double temp_depth;	//テンプレート画像深度情報取得

	IplImage *update_temp_img;	//本人の可能性のあるテンプレート画像の保存
	UserInfo update_user;
	bool temp_user_flag;		//テンプレートが本人かどうか

	//本人認証、本人データの更新
	void UserAuthentication(void);

	//ユーザのみ身長を返す(正面からの深度情報を用いて)輪郭追跡
	double UserHightData(int point_x,int point_y);

	//最初の輪郭を見つける
	CvPoint FindContour(int point_x,int point_y);
	int x_min;
	int x_max;
	int x_when_y_min;
	int y_min;
	int y_max;

	//本人を見失っている回数
	int lost_count;

	//ユーザを見失ったら
	void UserReprobe(void);
	UserInfo userInfo[MAX_USER_NUM];

	//Robotからの処理イメージ
	void MakeRobotImage(void);
	IplImage *robot_img;
	IplImage *c_robot_img;

	//発話関係
	CTalk Talk;


	IplImage *gesture_img;	//ジェスチャーを認識する画像
	//バイバイ検出
	void ByeByeSign(void);
	CvPoint hand_point;		//掌の位置を保存
	CvPoint first_hand_point;
	int hand_count;			//手があるか検出
	int right_count;
	int left_count;
	int init_hand_count;	//初期化するカウンタ
	int start_count;

	//背景を抜いたテンプレートマッチング
	void ExpectBackTemplateMatting(void);
	double temp_min_val;
	CvPoint temp_min_point;
};

