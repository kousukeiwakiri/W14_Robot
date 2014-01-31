#include "StdAfx.h"
#include "Kalman.h"


CKalman::CKalman(void)
{
	kalman_init_flag = true;
	camera_prediction_pos_d = 0;
	kalman = cvCreateKalman(4,2);
}


CKalman::~CKalman(void)
{
}


void CKalman::main(void){
	RobotToWorld();	
	if(kalman_init_flag == true){
		init();
		kalman_init_flag = false;
	}else{
		KalmanFilter();
		WorldToCamera();
	}
}


//世界座標に変換(人間の現在位置を取得)
void CKalman::RobotToWorld(void){
	world_r_x = -robot_y;
	world_r_y = robot_x;
	world_r_w = robot_w * 3.1415/180;

	delta_x = human_x * cos(world_r_w) - human_d * sin(world_r_w);
	delta_y = human_x * sin(world_r_w) + human_d * cos(world_r_w);
	world_h_x = delta_x + world_r_x;
	world_h_y = delta_y + world_r_y;
}


//カルマンフィルタ初期化
void CKalman::init(void){
	cvReleaseKalman(&kalman);
	kalman = cvCreateKalman(4,2);

	//推移行列Fは時刻kと時刻k+1のモデルパラメータ間の関係を記述する
	kalman->DynamMatr[0]  = 1.0; kalman->DynamMatr[1]  = 0.0; kalman->DynamMatr[2]  = 1.0; kalman->DynamMatr[3]  = 0.0;
	kalman->DynamMatr[4]  = 0.0; kalman->DynamMatr[5]  = 1.0; kalman->DynamMatr[6]  = 0.0; kalman->DynamMatr[7]  = 1.0;
	kalman->DynamMatr[8]  = 0.0; kalman->DynamMatr[9]  = 0.0; kalman->DynamMatr[10] = 1.0; kalman->DynamMatr[11] = 0.0;
	kalman->DynamMatr[12] = 0.0; kalman->DynamMatr[13] = 0.0; kalman->DynamMatr[14] = 0.0; kalman->DynamMatr[15] = 1.0;

	//他のKalmanフィルタのパラメータを初期化する
	//共分散行列の設定
	cvSetIdentity(kalman->measurement_matrix , cvRealScalar(1));
	cvSetIdentity(kalman->process_noise_cov , cvRealScalar(1e-5));
	cvSetIdentity(kalman->measurement_noise_cov , cvRealScalar(1e-1));
	cvSetIdentity(kalman->error_cov_post , cvRealScalar(1));


	///位置補正(初期値を0にするための)
	first_h_x = world_h_x;
	first_h_y = world_h_y;

	world_prediction_pos_x = first_h_x;
	world_prediction_pos_y = first_h_y;

	range_diff = 300;
	camera_prediction_pos_x = human_x;
	camera_prediction_pos_d = human_d;
}



//次点位置予測
void CKalman::KalmanFilter(void){

	//次の時間の点の位置を予測する
	// 観測値
	float m[] = {world_h_x - first_h_x, world_h_y-first_h_y};	//初期値の補正
	CvMat measurement = cvMat(2, 1, CV_32FC1, m);

	// 修正フェーズ
	const CvMat *correction = cvKalmanCorrect(kalman, &measurement);

	// 更新フェーズ
	const CvMat *prediction = cvKalmanPredict(kalman);

	//次の位置予測
	world_prediction_pos_x = prediction->data.fl[0] +first_h_x;
	world_prediction_pos_y = prediction->data.fl[1] +first_h_y;
	range_diff = sqrt( (prediction->data.fl[0] +first_h_x -world_h_x)*(prediction->data.fl[0] +first_h_x -world_h_x) + (prediction->data.fl[1] + first_h_y -world_h_y)*(prediction->data.fl[1] + first_h_y -world_h_y));	//推定と観測誤差

}

//予測位置をカメラ座標に変換
void CKalman::WorldToCamera(void){
	double prediction_delta_x = world_prediction_pos_x - world_r_x;
	double prediction_delta_y = world_prediction_pos_y - world_r_y;
	camera_prediction_pos_x = prediction_delta_x * cos( -world_r_w ) - prediction_delta_y * sin(-world_r_w);
	camera_prediction_pos_d = prediction_delta_x * sin( -world_r_w ) + prediction_delta_y * cos(-world_r_w);
}


