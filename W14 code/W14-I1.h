
// W14-I1.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��


// CW14I1App:
// ���̃N���X�̎����ɂ��ẮAW14-I1.cpp ���Q�Ƃ��Ă��������B
//

class CW14I1App : public CWinApp
{
public:
	CW14I1App();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CW14I1App theApp;