#include "StdAfx.h"
#include "Kinect.h"

//�\�[�X���Q�Ɗ֌W
Mouse mouse;
extern bool robot_stop_flag;	//���{�b�g�ǐ՗p


CKinect::CKinect(void)
{
	mouse.pointX=0;
	mouse.pointY=0;
	mouse.flag=false;
	first_sign_in_flag=false;
	main_sign_in_flag=false;
	user.right_end = 0;
	user.left_end =255;
	user.lower_end =0;
	user.upper_end =255;
	lost_count = 0;
	lost_flag=false;
	hand_count =0;
	right_count=0;
	left_count=0;
	byebye_flag=false;
	init_hand_count=0;
	init_user_count=0;
	user_count=0;
	upper_count=0;
	lower_count=0;
	start_count=0;
	camera_prediction_pos_d = 0;
	user_reprobe_flag =false;
	face_num = START_FACE;
}


CKinect::~CKinect(void)
{
}

// �}�E�X�C�x���g�̃R�[���o�b�N�֐�
void on_mouse(int event, int x, int y, int flags, void *param)
{
	// ���N���b�N��
	if(event == CV_EVENT_LBUTTONDOWN) {
		mouse.pointX=x;
		mouse.pointY=y;
		mouse.flag=true;
	}
}

//�G���[�`�F�b�N
void CKinect::CheckOpenNIError(XnStatus status)
{
	if(status != XN_STATUS_OK){
		throw std::runtime_error(xnGetStatusString(status));
	}
}

void CKinect::Init(void)
{
	try{

		//�R���e�L�X�g�̏�����
		rc = context.InitFromXmlFile(CONFIG_XML_PATH);
		CheckOpenNIError(rc);

		//�e�W�F�l���[�^�̍쐬
		rc = context.FindExistingNode(XN_NODE_TYPE_IMAGE, imageGenerator);		// Image�W�F�l���[�^
		CheckOpenNIError(rc);
		rc = context.FindExistingNode(XN_NODE_TYPE_DEPTH, depthGenerator);		// Depth�W�F�l���[�^
		CheckOpenNIError(rc);
		rc = context.FindExistingNode(XN_NODE_TYPE_USER, userGenerator);		// User�W�F�l���[�^
		CheckOpenNIError(rc);

		// �f�v�X�̍��W���C���[�W�ɍ��킹��
		depthGenerator.GetAlternativeViewPointCap().SetViewPoint(imageGenerator);

		//�L�����u���[�V�����|�[�Y�̎擾
		xn::SkeletonCapability skelton=userGenerator.GetSkeletonCap();


		//�J�����T�C�Y�̃C���[�W���쐬(8�r�b�g��RGB)
		imageGenerator.GetMapOutputMode(outputMode);
		camera=::cvCreateImage(cvSize(outputMode.nXRes,outputMode.nYRes),IPL_DEPTH_8U,3);
		user_img = cvCreateImage(cvSize(outputMode.nXRes, outputMode.nYRes), IPL_DEPTH_8U, 3);
		depth_img =  cvCreateImage(cvSize(outputMode.nXRes, outputMode.nYRes), IPL_DEPTH_8U, 3);
		robot_img =  cvCreateImage(cvSize(outputMode.nXRes, outputMode.nYRes), IPL_DEPTH_8U, 1);
		c_robot_img =  cvCreateImage(cvSize(outputMode.nXRes, outputMode.nYRes), IPL_DEPTH_8U, 3);
		gesture_img =  cvCreateImage(cvSize(outputMode.nXRes, outputMode.nYRes), IPL_DEPTH_8U, 3);
	}catch(std::exception){}
}

void CKinect::Main(void)
{
	//���ׂĂ̍X�V��҂��A�摜����уf�v�X�f�[�^���擾����
	context.WaitAndUpdateAll();
	// �R�[���o�b�N�֐��̓o�^
	cvSetMouseCallback("UserDetection", on_mouse, 0);

	//Meta�f�[�^�̎擾
	imageGenerator.GetMetaData(imageMD);
	userGenerator.GetUserPixels(0,sceneMD);
	depthGenerator.GetMetaData(depthMD);

	UserDetection();		//���[�U��؏o��
	//���[�U�o�^
	if(main_sign_in_flag==false){
		UserSignin();		//�o�^�t�F�[�Y��(�o�^����Ă��Ȃ��Ƃ�)
	}
	if(first_sign_in_flag==true){		
		cvCircle(user_img,cv::Point(600,30),8,cv::Scalar(200,200,200),1,8,0);
	}
	//���[�U�o�^��
	if(main_sign_in_flag==true){
		if(lost_flag==true){
			//cout<<"come in"<<endl;
			UserReprobe();			//���[�U�Ĕ���
		}else{
			MakeDepthImage();		//���[�U���� DEPTHP_IMG_RANGE mm�O��̐[�x�摜
			ExpectBackTemplateMatting();
			//DataCollection();		//�f�[�^���W
			UserAuthentication();	//�{�l�F��
			if(start_count >30){
				ByeByeSign();			//�΂��΂��F��
			}
			if(start_count <40){
				start_count++;
			}
			cvCircle(user_img,cv::Point(600,30),13,cv::Scalar(200,200,200),1,8,0);
		}
	}
	MakeRobotImage();
	cvLine(user_img,cvPoint(imageMD.XRes()/2 , 0) , cvPoint(imageMD.XRes()/2 , imageMD.YRes() ),CV_RGB(255,255,255),1,8,0);
	cvLine(user_img,cvPoint(0 , imageMD.YRes()/2),cvPoint(imageMD.XRes(),imageMD.YRes()/2),CV_RGB(255,255,255),1,8,0);
	memcpy(camera->imageData,imageMD.RGB24Data(),camera->imageSize);
	cvCvtColor(camera, camera, CV_BGR2RGB);
	cvShowImage("KinectImage", camera);
	cvCvtColor(user_img, user_img, CV_BGR2RGB);
	cvShowImage("UserDetection",user_img);
	cvShowImage("DepthImage",depth_img);
	//cvShowImage("gestureImage",gesture_img);
}

//���[�U�؏o��
void CKinect::UserDetection(void)
{

	cvZero(user_img);
	char* dest=user_img->imageData;

	for(int y=0; y<imageMD.YRes() ;++y){
		for(int x=0;x<imageMD.XRes();++x){
			XnRGB24Pixel rgb = imageMD.RGB24Map()(x, y);
			if(sceneMD(x,y)!=0){
				dest[0] = rgb.nRed;
				dest[1] = rgb.nGreen;
				dest[2] = rgb.nBlue;
			}
			dest+= 3;
		}
	}
}

//���[�U�o�^
void CKinect::UserSignin(void){
	if(sceneMD(imageMD.XRes()/2,imageMD.YRes()/2)!=0){		//���S�Ƀ��[�U������Ƃ�

		user_count ++;
		init_user_count = 0;	
	}else{
		if(init_user_count < 5){
			init_user_count++;
		}else{
			first_sign_in_flag = false;
			face_num = START_FACE;
			user_number = 0;
			init_user_count = 0;
		}
	}
	if(user_count >15){		//���S�Ƀ��[�U��������
		if(first_sign_in_flag == false){
			first_sign_in_flag = true;

			Talk.TalkLanguage(TALK_HELLO);//����ɂ���
			face_num = HELLO_FACE;
		}
		user_count=0; 
	}

	if(first_sign_in_flag == true){
		//�{���摜�̐����������!
		int point_x_sum =0 ;
		int point_y_sum =0 ;
		int point_num = 0 ;
		cvCopy(camera,gesture_img);
		//���̐l�̒��S����150~400mm��؏o����!
		user_number = sceneMD(imageMD.XRes()/2,imageMD.YRes()/2);
		for(int j=0 ; j< imageMD.YRes() ; ++j){
			for(int i=0 ; i<imageMD.XRes() ; ++i){
				if(depthMD(imageMD.XRes()/2,imageMD.YRes()/2) - GESTURE_DEPTH_MIN_LIMIT > depthMD(i,j) && 
					depthMD(i,j) > depthMD(imageMD.XRes()/2,imageMD.YRes()/2) - GESTURE_DEPTH_MAX_LIMIT &&   
					imageMD.YRes()/2 +imageMD.YRes()/4 > j && sceneMD(i,j)==user_number)
				{
					point_x_sum+= i ;
					point_y_sum+= j ;
					point_num++;
				}else
				{
					(gesture_img->imageData)[gesture_img->widthStep * j + i * 3 ] = 0;
					(gesture_img->imageData)[gesture_img->widthStep * j + i * 3 +1] = 0;
					(gesture_img->imageData)[gesture_img->widthStep * j + i * 3 +2] = 0;
				}
			}
		}
		if(point_num != 0){
			hand_point.x = point_x_sum /point_num;
			hand_point.y = point_y_sum /point_num;
			cvCircle(gesture_img,cvPoint(hand_point.x,hand_point.y),10,CV_RGB(255,255,255),-1,8,0);
			hand_count++;
			init_hand_count =0;
		}else{
			if(init_hand_count < 5){
				init_hand_count++;
			}else{
				//�����Ȃ��Ƃ�������
				hand_point.x =-1;
				hand_point.y =-1;
				hand_count = 0;
				init_hand_count =0;
			}
		}
		if(hand_count > 10){
			if(first_hand_point.x < 0 ){
				first_hand_point.x = hand_point.x;
				first_hand_point.y = hand_point.y;
			}else{
				if(first_hand_point.y + 20 < hand_point.y){
					lower_count++;
				}else if(first_hand_point.y - 20 > hand_point.y)
					upper_count++;
			}
			if((upper_count > 5 && lower_count>5) || upper_count > 10 || lower_count>10){
				main_sign_in_flag=true;
				Talk.TalkLanguage(TALK_START);
				face_num = TRACKING_FACE;
			}
			cout<<"upper="<<upper_count<<" lower="<<lower_count<<endl;
		}else{ 
			//������
			first_hand_point.x = -1;
			first_hand_point.y = -1;
			upper_count=0;
			lower_count=0;
		}
	}else{

		//������
		first_hand_point.x = -1;
		first_hand_point.y = -1;
		upper_count=0;
		lower_count=0;
	}

	//ROI�Ńe���v���摜�؂�o��
	if(main_sign_in_flag==true){
		int depth_sum=0;
		int depth_num=0;
		for(int j=0;j<imageMD.YRes();++j){
			for(int i=0; i<imageMD.XRes();++i){
				if(sceneMD(i,j)==user_number){
					if(user.upper_end > j)user.upper_end = j;
					if(user.lower_end < j)user.lower_end = j;
					if(user.right_end < i)user.right_end = i;
					if(user.left_end > i)user.left_end =i;
					depth_sum+=depthMD(i,j);
					depth_num++;
				}
			}
		}
		user.center_x= (user.right_end + user.left_end)/2;
		user.center_y= (user.lower_end + user.upper_end)/2;
		user.center_depth = depth_sum/depth_num;
		temp_depth = user.center_depth;
		cvSetImageROI(camera,cvRect(user.center_x-TEMPLATESIZE_X/2,user.center_y-TEMPLATESIZE_Y/2  , TEMPLATESIZE_X , TEMPLATESIZE_Y));
		cvSaveImage(FRONT_TEMPLATE_PATH,camera);
		cvResetImageROI(camera);
		temp_img = cvLoadImage(FRONT_TEMPLATE_PATH, CV_LOAD_IMAGE_COLOR);			// �e���v���摜�����[�h
		update_temp_img = cvLoadImage(FRONT_TEMPLATE_PATH, CV_LOAD_IMAGE_COLOR);	// �X�V�p�e���v���摜�����[�h
	}
}


//�{�l�F��
void CKinect::UserAuthentication(){
	if(temp_user_flag==true){
		//�l����X����
		user_x = Measure(user.center_x,user.center_y,user.center_depth);
		//�S�s�N�Z�����̕ϓ��m�F
		int update_user_area = (update_user.right_end - update_user.left_end) * (update_user.lower_end - update_user.upper_end);
		int user_area = (user.right_end - user.left_end) * (user.lower_end - user.upper_end);
		if(user_area-USER_PIXEL_RANGE<update_user_area && update_user_area < user_area+USER_PIXEL_RANGE){	//�s�N�Z�����Ŗ{�l�m�F
			user.right_end = update_user.right_end;
			user.left_end = update_user.left_end;
			user.upper_end = update_user.upper_end;
			user.lower_end = update_user.lower_end;
			user.center_x = (user.right_end + user.left_end)/2;
			user.center_y = (user.upper_end + user.lower_end)/2;
			//�d�S�v�Z
			user.center_depth = depthMD( user.center_x , user.center_y);
			/*int depth_sum=0;
			int depth_num=0;
			for(int j=user.upper_end ; j<user.lower_end ; ++j){
			for(int i=user.left_end ; i<user.right_end ; ++i){
			if((depth_img->imageData)[depth_img->widthStep * j + i * 3 ] == 0
			&& (depth_img->imageData)[depth_img->widthStep * j + i * 3 +1] == 0
			&& (depth_img->imageData)[depth_img->widthStep * j + i * 3 +2] == 0 ){
			;
			}else{
			depth_sum+=depthMD(i,j);
			depth_num++;
			}
			}
			}
			if(depth_num==0){
			user.center_depth = 0;
			}else if(user.center_depth - DEPTH_IMG_RANGE < depth_sum/depth_num && depth_sum/depth_num < user.center_depth + DEPTH_IMG_RANGE){
			user.center_depth = depth_sum/depth_num;
			}*/

			//�e���v���[�g�摜�X�V����
			cvSetImageROI(depth_img , cvRect(user.center_x - TEMPLATESIZE_X / 2, user.center_y - TEMPLATESIZE_Y / 2, TEMPLATESIZE_X , TEMPLATESIZE_Y)); //���S�̃e���v���[�g�̐؏o��
			cvCopy(depth_img,update_temp_img);
			cvResetImageROI(depth_img);
			int null_point_num=0;
			for(int i=0; i<update_temp_img->width ; i++){
				for(int j=0 ; j<update_temp_img->height ; j++){
					if(	depthMD(user.center_x - TEMPLATESIZE_X / 2 + i , user.center_y - TEMPLATESIZE_Y / 2 + j) > user.center_depth +  DEPTH_IMG_RANGE 
						|| depthMD(user.center_x - TEMPLATESIZE_X / 2 + i , user.center_y - TEMPLATESIZE_Y / 2 +j) < 300){
							null_point_num++;
					}
				}
			}
			if(null_point_num < MAX_NULL_POINT_NUM){
				cvCopy(update_temp_img,temp_img);
			}
			lost_count = 0;
		}else{
			lost_count++;
		}
	}else{
		lost_count++;
	}
	if(lost_count > MAX_LOST_COUNT){
		lost_flag=true;
		Talk.TalkLanguage(TALK_LOST);
		face_num = LOST_FACE;
	}
}

//X���̋���
double CKinect::Measure(int x, int y , int depth){
	XnPoint3D t1, t2;
	t1.X = x;
	t1.Y = y;
	t1.Z = depth;
	depthGenerator.ConvertProjectiveToRealWorld(1,&t1,&t2);
	return (double)(t2.X);
}

//���[�U���� DEPTHP_IMG_RANGE mm�O��̐[�x�摜
void CKinect::MakeDepthImage(void){
	double human_depth;
	cvCopy(camera,depth_img);
	cout<<user.center_depth<<endl;
	if(camera_prediction_pos_d != 0){
		cout << "�J���}���t�B���^�\���͈͌���:"<<camera_prediction_pos_d<<endl;
		human_depth = camera_prediction_pos_d;
	}else{
		human_depth = user.center_depth;
	}

	for(int j=0; j<imageMD.YRes() ; ++j){
		for(int i=0 ; i<imageMD.XRes() ; ++i){
			XnPoint3D t1, t2;
			t1.X = i;
			t1.Y = j;
			t1.Z = depthMD(i,j);
			t2.Y = 0.001763 * t1.Z *(240- t1.Y);
			t2.Z = t1.Z;

			if( t2.Z < 100 || t2.Z > human_depth + DEPTH_IMG_RANGE || t2.Y > 1000 || t2.Y <-1000){
				(depth_img->imageData)[depth_img->widthStep * j + i * 3 ] = 0;
				(depth_img->imageData)[depth_img->widthStep * j + i * 3 +1] = 0;
				(depth_img->imageData)[depth_img->widthStep * j + i * 3 +2] = 0;
			}
		}
	}

}
/*
//�{�l���ǂ����̃f�[�^���W
void CKinect::DataCollection(void){
temp_user_flag=false;
IplImage *dst_img = 0;
double  min_val =0;		// ����x�����������ʂ̃|�C���^������
CvPoint min_loc ;		// ����x���������̌��ʂ̍��W������
//�}�b�`���O�摜��ROI�Ő؂���
int roi_x = user.center_x - MATTING_X_RANGE;
if(roi_x<0)roi_x=0;
int roi_y = user.center_y - MATTING_Y_RANGE;
if(roi_y<0)roi_y=0;
int roi_range_x = MATTING_X_RANGE*2;
if( roi_range_x + roi_x >= depth_img->width)roi_range_x = depth_img->width - roi_x;
int roi_range_y = MATTING_Y_RANGE*2;
if( roi_range_y + roi_y >= depth_img->height)roi_range_y = depth_img->width - roi_y;
cvSetImageROI(depth_img , cvRect(roi_x, roi_y, MATTING_X_RANGE*2 , MATTING_Y_RANGE*2));
//�e���v���[�g�}�b�`���O
dst_img = cvCreateImage (cvSize(depth_img->roi->width - temp_img->width + 1, depth_img->roi->height - temp_img->height + 1), IPL_DEPTH_32F, 1);
cvMatchTemplate (depth_img, temp_img, dst_img, CV_TM_SQDIFF);					//SAD
cvMinMaxLoc(dst_img, &min_val, NULL , &min_loc, NULL, NULL);					//�}�b�`���O�̌���
cvResetImageROI(depth_img);														//ROI���
double  m_dissimilarity =	min_val/(temp_img->width*temp_img->height);			//�ʐςŊ���
cout << "m_dissimilarity" <<m_dissimilarity<<endl;
if(m_dissimilarity < USER_DISSIMILARITY){
temp_user_flag = true;
cvSetImageROI(depth_img , cvRect(min_loc.x + roi_x, min_loc.y + roi_y, TEMPLATESIZE_X , TEMPLATESIZE_Y));
cvCopy(depth_img,update_temp_img);
UserHightData(min_loc.x+TEMPLATESIZE_X/2+roi_x , min_loc.y+roi_y+TEMPLATESIZE_Y/2);
cvResetImageROI(depth_img);	
}
cvShowImage("Template_img",temp_img);
cvReleaseImage(&dst_img);

//���݂̃e���v���[�g�}�b�`���O�͈̔�
cvRectangle(depth_img,cvPoint(roi_x,roi_y),cvPoint(roi_x+MATTING_X_RANGE*2,roi_y+MATTING_Y_RANGE*2),CV_RGB(0,255,255),5,8,0);

//�J���}���t�B���^�ł͈̔�
double kalman_pixel_x = camera_prediction_pos_x / (0.001736 * camera_prediction_pos_d) + 320;
double kalman_pixel_rane_diff = (kalman_range_diff+200) / (0.001736 * camera_prediction_pos_d);
cvRectangle(depth_img,cvPoint(kalman_pixel_x - kalman_pixel_rane_diff,roi_y),cvPoint(kalman_pixel_x + kalman_pixel_rane_diff , roi_y+MATTING_Y_RANGE*2),CV_RGB(255,255,0),5,8,0);

}*/

//�w�i�𔲂����e���v���[�g�}�b�`���O
void CKinect::ExpectBackTemplateMatting(void){
	double human_depth;


	if(camera_prediction_pos_d != 0){
		//cout << "�J���}���t�B���^�\���͈͌���:"<<camera_prediction_pos_d<<endl;
		human_depth = camera_prediction_pos_d;
	}else{
		human_depth = user.center_depth;
	}

	temp_user_flag=false;
	IplImage *dst_img = 0;
	double  min_val = 255*255;		// ����x�����������ʂ̃|�C���^������
	CvPoint min_loc ;		// ����x���������̌��ʂ̍��W������
	//�}�b�`���O�摜��ROI�Ő؂���
	int roi_x = user.center_x - MATTING_X_RANGE;
	if(roi_x<0)roi_x=0;
	int roi_y = user.center_y - MATTING_Y_RANGE;
	if(roi_y<0)roi_y=0;
	int roi_range_x = MATTING_X_RANGE*2;
	if( roi_range_x + roi_x >= depth_img->width)roi_range_x = depth_img->width - roi_x;
	int roi_range_y = MATTING_Y_RANGE*2;
	if( roi_range_y + roi_y >= depth_img->height)roi_range_y = depth_img->height - roi_y;
	cvSetImageROI(depth_img , cvRect(roi_x, roi_y, roi_range_x , roi_range_y));
	//�e���v���[�g�}�b�`���O
	dst_img = cvCreateImage (cvSize(depth_img->roi->width - temp_img->width + 1, depth_img->roi->height - temp_img->height + 1), IPL_DEPTH_32F, 1);
	cvMatchTemplate(depth_img, temp_img, dst_img, CV_TM_SQDIFF);					//SAD
	/////////////////////////////////////////MinMaxLoc�̒��g�̕ύX//////////////////////////////////////////////////////////////////////
	/*
	for(int j = 0; j < dst_img->height ; j++){
		for(int i = 0 ; i < dst_img->width ; i++){
			if(i + TEMPLATESIZE_X / 2 < dst_img -> width &&  j + TEMPLATESIZE_Y / 2 < dst_img->height){
				if(depthMD(roi_x + i + TEMPLATESIZE_X / 2 , roi_y + j + TEMPLATESIZE_Y / 2) > human_depth + DEPTH_IMG_RANGE || depthMD(roi_x + i + TEMPLATESIZE_X / 2 , roi_y + j + TEMPLATESIZE_Y / 2) < 500){
					(dst_img->imageData)[dst_img->widthStep * j + i * dst_img-> nChannels ] =  USER_DISSIMILARITY + 1 ;
				}
			}
		}
	}
	*/
	cvMinMaxLoc(dst_img, &min_val, NULL , &min_loc, NULL, NULL);					//�}�b�`���O�̌���
	cvResetImageROI(depth_img);														//ROI���
	double  m_dissimilarity =	min_val / (temp_img->width*temp_img->height);			//�ʐςŊ���
	cout << "����x: " << m_dissimilarity << endl;
	if(m_dissimilarity < USER_DISSIMILARITY){
		temp_user_flag = true;
		UserHightData(min_loc.x+TEMPLATESIZE_X/2+roi_x , min_loc.y+roi_y+TEMPLATESIZE_Y/2);
	}
	cvShowImage("Template_img",temp_img);
	cvReleaseImage(&dst_img);

	//���݂̃e���v���[�g�}�b�`���O�͈̔�
	cvCircle(user_img,cvPoint(min_loc.x+TEMPLATESIZE_X/2+roi_x , min_loc.y+TEMPLATESIZE_Y/2+roi_y),20,CV_RGB(0,0,255),-1,8,0);
	cvRectangle(user_img,cvPoint(roi_x,roi_y),cvPoint(roi_x+roi_range_x , roi_y+roi_range_y),CV_RGB(0,255,255),5,8,0);
}
//�ŏ��̗֊s��������u���v��
CvPoint CKinect::FindContour(int point_x,int point_y){
	CvPoint point;
	point.x=point_x;
	point.y=point_y;
	int before_depth=depthMD(point.x,point.y);
	while(1){
		if(point.x >= imageMD.XRes())break;
		if(before_depth - DEPTH_HIGTH_LIMIT < depthMD(point.x+1,point.y) && depthMD(point.x+1,point.y) <before_depth + DEPTH_HIGTH_LIMIT){
			before_depth = depthMD(point.x,point.y);
			point.x++;
		}else{
			break;
		}
	}
	return point;
}

//���[�U�̂ݐg����Ԃ�(���ʂ���̐[�x����p����) �O�ڋ�`�������āA���̒��ň�ԏ�̓_��������
double CKinect::UserHightData(int point_x,int point_y){
	int x_min=imageMD.XRes();
	int x_max=0;
	int x_when_y_min;
	int y_min=imageMD.YRes();
	int y_max=0;
	//////////////
	CvPoint first_contour = FindContour(point_x,point_y);			//�ŏ��̗֊s�̓_
	if(point_x == first_contour.x){
		return 0;	//����1��f���Ȃ��Ƃ��@1��f�̎��������[�v���Ȃ��悤��
	}else{
		double before_depth = depthMD(first_contour.x,first_contour.y);	//�ЂƂO�̐[�x
		CvPoint now_point = first_contour;								//���݂̗֊s�̓_
		double search_depth = 0;										//���ׂ�_�̐[�x
		bool contour_find_flag = true;									//�֊s���܂����ׂ��邩
		int direction = D_UPPER;
		while(contour_find_flag){
			direction++;
			if(direction==5)direction=D_UPPER;
			bool direction_flag = true;									//�������߂����Ă��邩
			while(direction_flag){
				switch(direction){
				case D_UPPER:
					if(now_point.y <= 1)search_depth=0;
					else search_depth = depthMD(now_point.x,now_point.y-1);
					if(before_depth - DEPTH_HIGTH_LIMIT < search_depth && search_depth < before_depth + DEPTH_HIGTH_LIMIT){
						now_point.y--;
						direction_flag = false;
					}
					break;
				case D_RIGHT:
					if(now_point.x >= imageMD.XRes()-1 )search_depth=0;
					else search_depth = depthMD(now_point.x+1,now_point.y);
					if(before_depth - DEPTH_HIGTH_LIMIT < search_depth && search_depth < before_depth + DEPTH_HIGTH_LIMIT){
						now_point.x++;
						direction_flag = false;
					}
					break;
				case D_UNDER:
					if(now_point.y >= imageMD.YRes()-1)search_depth=0;
					else search_depth = depthMD(now_point.x,now_point.y+1);
					if(before_depth - DEPTH_HIGTH_LIMIT < search_depth && search_depth < before_depth + DEPTH_HIGTH_LIMIT){
						now_point.y++;
						direction_flag = false;
					}
					break;
				case D_LEFT:
					if(now_point.x <= 1 )search_depth=0;
					else search_depth = depthMD(now_point.x-1,now_point.y);
					if(before_depth - DEPTH_HIGTH_LIMIT < search_depth && search_depth < before_depth + DEPTH_HIGTH_LIMIT){
						now_point.x--;
						direction_flag = false;
					}
					break;
				default:
					break;
				}
				if(direction_flag){
					direction--;
					if(direction==0)direction=D_LEFT;
				}else{
					//�G���[���o�Ȃ��悤�Ɂ@�����s��
					if(now_point.x < 1 || imageMD.XRes()-1 < now_point.x|| now_point.y < 1 || imageMD.YRes()-1 < now_point.y){
						return 0;
					}
					before_depth = depthMD(now_point.x,now_point.y);
					if(y_min > now_point.y) {
						y_min=now_point.y;
						x_when_y_min=now_point.x;
					}
					if(y_max < now_point.y) y_max=now_point.y;
					if(x_min > now_point.x) x_min=now_point.x;
					if(x_max < now_point.x) x_max=now_point.x;
					//�I������
					if( first_contour.x == now_point.x && first_contour.y == now_point.y) contour_find_flag=false;
				}
			}
		}
		cvRectangle(user_img,cv::Point(x_min,y_min),cv::Point(x_max,y_max),cv::Scalar(0,255,0), 3, 4);
		update_user.upper_end = y_min;
		update_user.lower_end = y_max;
		update_user.left_end = x_min;
		update_user.right_end = x_max;
		return 0;
		//return Measure(x_when_y_min,y_min,depthMD(x_when_y_min,y_min));
	}
}

//LOST��̃��[�U�ĔF���̔���
void CKinect::UserReprobe(void){
	//���[�U�̏�����
	for(int i=0; i<MAX_USER_NUM ;i++){
		userInfo[i].hight=0;
		userInfo[i].upper_end=imageMD.YRes();
		userInfo[i].lower_end=0;
		userInfo[i].left_end=imageMD.XRes();
		userInfo[i].right_end=0;
		userInfo[i].center_x=0;
		userInfo[i].center_y=0;
		userInfo[i].pixel_num=0;
	}
	for(int j=0; j<imageMD.YRes() ; ++j){
		for(int i=0; i<imageMD.XRes() ; ++i){
			if(sceneMD(i,j)!=0){
				XnLabel label=sceneMD(i,j);
				// �㉺���E�̒[��T��
				if(j < userInfo[label].upper_end) {
					userInfo[label].upper_end = j;
				}
				if(i < userInfo[label].left_end) {
					userInfo[label].left_end = i;
				}
				if(j > userInfo[label].lower_end) {
					userInfo[label].lower_end = j;
				}
				if(i > userInfo[label].right_end) {
					userInfo[label].right_end = i;
				}
			}
		}
	}
	for (int i = 1; i < MAX_USER_NUM; i++){	
		if(userInfo[i].lower_end!=0){				//���[�U������Ƃ���
			userInfo[i].center_x = (userInfo[i].right_end + userInfo[i].left_end)/2;
			userInfo[i].center_y = (userInfo[i].upper_end + userInfo[i].lower_end)/2;
		}
	}
	int original=-1;
	double max_same=0;			//�ގ��x����ԍ����̂�
	IplImage *temp_img_front = 0,*dst_img = 0;
	double  max_val;		// �ő�̌��ʂ̃|�C���^������
	CvPoint max_loc;		// �ő�̌��ʂ̍��W������
	temp_img_front = cvLoadImage(FRONT_TEMPLATE_PATH, CV_LOAD_IMAGE_COLOR);	// �e���v���摜�����[�h
	IplImage *copy_userimg=cvCloneImage(user_img);
	for (int i = 0; i < MAX_USER_NUM; i++){	//���[�U�̐l�������J��Ԃ�
		if(userInfo[i].right_end - userInfo[i].left_end > TEMPLATESIZE_X+1 && userInfo[i].lower_end - userInfo[i].upper_end > TEMPLATESIZE_Y+1){		//�}�b�`���O�����s���Ȃ��͈�
			//ROI�Ő؂���
			cvSetImageROI(copy_userimg , cvRect(userInfo[i].left_end , userInfo[i].upper_end , userInfo[i].right_end-userInfo[i].left_end , userInfo[i].lower_end-userInfo[i].upper_end));
			//����x�}�b�v�摜�p
			dst_img = cvCreateImage( cvSize(copy_userimg->roi->width - temp_img_front->width+1 , copy_userimg->roi->height - temp_img_front->height+1), IPL_DEPTH_32F, 1);
			//�e���v���[�g�}�b�`���O���s��
			cvMatchTemplate(copy_userimg,temp_img_front,dst_img,CV_TM_CCOEFF_NORMED);			//�ő�l�̍��W���}�b�`���O�ʒu
			cvMinMaxLoc(dst_img, NULL, &max_val, NULL, &max_loc, NULL);					//�}�b�`���O�̌���
			if(max_same<max_val && max_val>TEMPLATE_SIMILARITY){										
				original=i;
				max_same=max_val;

			}
		}
	}
	//�{�l�m�� 
	if(original!=-1 && userInfo[original].lower_end - userInfo[original].upper_end > imageMD.YRes()/4 
		&& userInfo[original].right_end - userInfo[original].upper_end > imageMD.YRes()/16 )
	{
		user=userInfo[original];
		user.center_depth = depthMD(user.center_x,user.center_y);
		cvCopy(temp_img_front,temp_img);
		Talk.TalkLanguage(TALK_REDIS);
		face_num = TRACKING_FACE;
		lost_count=0;
		lost_flag = false;
		user_reprobe_flag = true;
	}
	cvReleaseImage(&copy_userimg);
	cvReleaseImage(&temp_img_front);
	cvReleaseImage(&dst_img);

}

//���{�b�g�̏������
void CKinect::MakeRobotImage(void){
	cvCvtColor(camera, robot_img, CV_BGR2GRAY);  
	cvCanny (robot_img, robot_img, 50.0, 200.0);
	cvCvtColor( robot_img, c_robot_img, CV_GRAY2BGR );
	cvLine(c_robot_img,cv::Point(imageMD.XRes()/2-ROLL_PIXEL_THRESHOLD,0),cvPoint(imageMD.XRes()/2-ROLL_PIXEL_THRESHOLD,imageMD.YRes()),CV_RGB(0,0,255),3,8);
	cvLine(c_robot_img,cv::Point(imageMD.XRes()/2+ROLL_PIXEL_THRESHOLD,0),cvPoint(imageMD.XRes()/2+ROLL_PIXEL_THRESHOLD,imageMD.YRes()),CV_RGB(0,0,255),3,8);
	cvLine(c_robot_img,cv::Point(imageMD.XRes()/2,0),cvPoint(imageMD.XRes()/2,imageMD.YRes()),CV_RGB(255,0,0),2,8);
	CvFont font;
	::cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX,1,1,20);
	std::stringstream ss;
	if(main_sign_in_flag==true){
		if(lost_flag==false ){
			ss << user.center_depth <<" mm";
			::cvPutText(c_robot_img,ss.str().c_str(),cvPoint(500,30),&font,CV_RGB(255,255,0));
			cvCircle(c_robot_img,cvPoint(user.center_x,user.center_y),3,CV_RGB(0,255,0),-1,15,0);
			cvRectangle(c_robot_img,cvPoint(user.left_end,user.upper_end),cvPoint(user.right_end,user.lower_end),CV_RGB(0,255,0),5,8,0);

		}else{
			ss << "LOST";
			::cvPutText(c_robot_img,ss.str().c_str(),cvPoint(500,30),&font,CV_RGB(255,0,0));
		}
	}
	cvShowImage("RobotImage",c_robot_img);
}

//�o�C�o�C���o
void CKinect::ByeByeSign(void){
	if(robot_stop_flag ==true){
		//�������{�b�g���~�܂��Ă�����@���@LOST���ĂȂ��Ƃ�
		//�{���摜�̐����������!
		int point_x_sum =0;
		int point_y_sum =0;
		int point_num = 0;
		cvCopy(camera,gesture_img);

		for(int j=0 ; j< imageMD.YRes() ; ++j){
			for(int i=0 ; i<imageMD.XRes() ; ++i){
				if(user.center_depth - GESTURE_DEPTH_MIN_LIMIT > depthMD(i,j) && depthMD(i,j) > user.center_depth-GESTURE_DEPTH_MAX_LIMIT && j<user.center_y && user.left_end - 50 < i&& i < user.right_end + 50 
					&& sceneMD(i,j)==sceneMD(user.center_x,user.center_y)){
						point_x_sum+= i ;
						point_y_sum+= j ;
						point_num++;
				}else{
					(gesture_img->imageData)[gesture_img->widthStep * j + i * 3 ] = 0;
					(gesture_img->imageData)[gesture_img->widthStep * j + i * 3 +1] = 0;
					(gesture_img->imageData)[gesture_img->widthStep * j + i * 3 +2] = 0;
				}
			}
		}
		if(point_num != 0){
			hand_point.x = point_x_sum /point_num;
			hand_point.y = point_y_sum /point_num;
			cvCircle(gesture_img,cvPoint(hand_point.x,hand_point.y),10,CV_RGB(255,255,255),-1,8,0);
			hand_count++;
			init_hand_count =0;
		}else{
			if(init_hand_count < 5){
				init_hand_count++;
			}else{
				//�����Ȃ��Ƃ�������
				hand_point.x =-1;
				hand_point.y =-1;
				hand_count = 0;
				init_hand_count =0;
			}
		}

		if(hand_count > 10){
			if(first_hand_point.x < 0 ){
				first_hand_point.x = hand_point.x;
				first_hand_point.y = hand_point.y;
			}else{
				if(first_hand_point.x + 20 < hand_point.x){
					right_count++;
				}else if(first_hand_point.x - 20 > hand_point.x)
					left_count++;
			}
			if((right_count > 5 && left_count>5) || right_count > 10 || left_count>10){
				byebye_flag=true;
				Talk.TalkLanguage(TALK_BYE);
				face_num = BYEBYE_FACE;
			}
			cout<<"right="<<right_count<<" left="<<left_count<<endl;
		}else{ 
			//������
			first_hand_point.x = -1;
			first_hand_point.y = -1;
			right_count=0;
			left_count=0;
		}
	}else{

		//������
		first_hand_point.x = -1;
		first_hand_point.y = -1;
		right_count=0;
		left_count=0;
	}

}
