#include "StdAfx.h"
#include "Face.h"


CFace::CFace(void)
{
	old_face_num=0;
	face_img =  cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
}


CFace::~CFace(void)
{
	cvReleaseImage(&face_img);
}



void CFace::CreateFase(int face_num){

	if(old_face_num != face_num){
		switch(face_num) {
			//START
		case START_FACE:
			face_img = cvLoadImage("./face_img/start.jpg", CV_LOAD_IMAGE_COLOR);
			break;

			// Ç±ÇÒÇ…ÇøÇÕ
		case HELLO_FACE:
			face_img = cvLoadImage("./face_img/hello.jpg", CV_LOAD_IMAGE_COLOR);
			break;

			// í«ê’íÜ
		case TRACKING_FACE:
			face_img = cvLoadImage("./face_img/tracking.jpg", CV_LOAD_IMAGE_COLOR);
			break;

			//Ç«Ç±Ç÷Ç¢Ç¡ÇΩÇÃÅH
		case LOST_FACE:
			face_img = cvLoadImage("./face_img/lost.jpg", CV_LOAD_IMAGE_COLOR);
			break;

			// Ç≥ÇÊÇ§Ç»ÇÁ
		case BYEBYE_FACE:
			face_img = cvLoadImage("./face_img/byebye.jpg", CV_LOAD_IMAGE_COLOR);
			break;

			// ãŸã}í‚é~
		case EMERGENCY_FACE:
			face_img = cvLoadImage("./face_img/emergency.jpg", CV_LOAD_IMAGE_COLOR);
			
			break;

		default:
			break;
		}
		old_face_num = face_num;
		cvShowImage("face_img",face_img); 
	}
	return;
}