#pragma once

#include "./AquesTalk/AquesTalkDa.h"
#pragma comment(lib, "AquesTalkDa.lib")

#define TALK_HELLO	1
#define TALK_START	2
#define TALK_NEAR	3
#define TALK_QUICK	4
#define TALK_LOST	5
#define TALK_BYE	6
#define TALK_REDIS	7
#define TALK_EMERGENCY 8

class CTalk
{
public:
	CTalk(void);
	~CTalk(void);
	void TalkLanguage(int talk_number);
private:
	H_AQTKDA talkData;
};

