#include "StdAfx.h"
#include "Talk.h"


CTalk::CTalk(void)
{
	//���b�֘A�̏�����
	talkData = AquesTalkDa_Create();
}


CTalk::~CTalk(void)
{
	// ���b�֘A�̉��
	AquesTalkDa_Release(talkData);
}


void CTalk::TalkLanguage(int talk_number)
{
	if(AquesTalkDa_IsPlay(talkData) == 1) {
		AquesTalkDa_Stop(talkData);
	}

	switch(talk_number) {
		// �ŏ��̈��A
	case TALK_HELLO:
		AquesTalkDa_Play(talkData, "����+�ɂ���B", 100, NULL, 0, 0);
		break;
		// �ǐՊJ�n
	case TALK_START:
		AquesTalkDa_Play(talkData, "����'����/��'����+������'���B", 100, NULL, 0, 0);
		break;
		// �l�ƃ��{�b�g�̋������߂��Ƃ�
	case TALK_NEAR:
		AquesTalkDa_Play(talkData, "����'���ł��B", 100, NULL, 0, 0);
		break;
		// �ǐՂ���l�������������Ƃ�
	case TALK_QUICK:
		AquesTalkDa_Play(talkData, "�͂₢/��'���A���'����'/���邢��/������'���B", 100, NULL, 0, 0);
		break;
		// ���������Ƃ�
	case TALK_LOST:
		AquesTalkDa_Play(talkData, "����A��'����/����'���́H", 100, NULL, 0, 0);
		break;
		// ���U�����Ƃ��i�I���̍��}�j
	case TALK_BYE:
		AquesTalkDa_Play(talkData, "����[�Ȃ�A�܂�,����/����+�����B", 100, NULL, 0, 0);
		break;
		// ����������ɍĔ��������Ƃ�
	case TALK_REDIS:
		AquesTalkDa_Play(talkData, "�����͂�����B", 100, NULL, 0, 0);
		break;
		//�ً}��~
	case TALK_EMERGENCY:
		AquesTalkDa_Play(talkData, "���傤�����Ԃ���B", 100, NULL, 0, 0);
		break;
	default:
		break;
	}
}