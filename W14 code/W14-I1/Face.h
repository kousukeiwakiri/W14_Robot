#pragma once

#include<opencv/cv.h>
#include<opencv/highgui.h>

#define START_FACE 1
#define HELLO_FACE 2
#define TRACKING_FACE 3
#define LOST_FACE 4
#define BYEBYE_FACE 5
#define EMERGENCY_FACE 6
class CFace
{
private:
	IplImage* face_img;
	int old_face_num;

public:
	CFace(void);
	~CFace(void);

	void CreateFase(int face_num);
	
};

