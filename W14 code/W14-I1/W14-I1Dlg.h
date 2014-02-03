
// W14-I1Dlg.h : �w�b�_�[ �t�@�C��
//

#pragma once

#include "Kinect.h"
#include "Robot.h"
#include "Kalman.h"
#include "Face.h"

#define ROBOT_TIMER_ID 10
#define ROBOT_TIMER_MS 100
#define KINECT_TIMER_ID 20
#define KINECT_TIMER_MS 33

// CW14I1Dlg �_�C�A���O
class CW14I1Dlg : public CDialogEx
{
	// �R���X�g���N�V����
public:
	CW14I1Dlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^�[

	// �_�C�A���O �f�[�^
	enum { IDD = IDD_W14I1_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g


// ����
protected:
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CKinect kinect;
	CRobot robot;
	CKalman kalman;
	CFace face;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	////////////////////////////////////////////////////
	std::ofstream ofss;	//�I�h���g���m�F
	std::ofstream ofs;	//�I�h���g���m�F���{�b�g����
	std::ofstream ofsh;  //�ǐՑΏېl���f�[�^
	clock_t  time_start;	//��������
	clock_t  now_time ,delta_t;

	int count;//�f�o�b�N�p


};
