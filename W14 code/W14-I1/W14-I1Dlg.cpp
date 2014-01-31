
// W14-I1Dlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "W14-I1.h"
#include "W14-I1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CW14I1Dlg �_�C�A���O




CW14I1Dlg::CW14I1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CW14I1Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CW14I1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CW14I1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CW14I1Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CW14I1Dlg::OnBnClickedButton2)
	ON_WM_DESTROY()
	ON_WM_TIMER()

END_MESSAGE_MAP()


// CW14I1Dlg ���b�Z�[�W �n���h���[

BOOL CW14I1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
	kinect.Init();
	SetTimer(KINECT_TIMER_ID,KINECT_TIMER_MS,NULL);
	SetTimer(ROBOT_TIMER_ID,ROBOT_TIMER_MS,NULL);
	AllocConsole();					//�R���\�[���E�B���h�E�o�͐ݒ�
	freopen("con","w", stdout);
	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

void CW14I1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CW14I1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CW14I1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//conect
void CW14I1Dlg::OnBnClickedButton1()
{
	if(robot.connect_flag==false){
		robot.Init();
		robot.connect_flag=true;
	}
}

//disconnect
void CW14I1Dlg::OnBnClickedButton2()
{
	if(robot.connect_flag==true){
		robot.Close();
		robot.connect_flag=false;
	}
}


void CW14I1Dlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if(robot.connect_flag==true){
		robot.Close();
		robot.connect_flag=false;
	}
}

//Timer
void CW14I1Dlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case KINECT_TIMER_ID:
		if(kinect.byebye_flag == false){
			kinect.Main();

		}else if(kinect.byebye_flag ==true){
			robot.Close();
		}
		break;
	case ROBOT_TIMER_ID:

		if(robot.connect_flag == true){ 
			robot.Sonar();
			robot.EmergencyStop();
			if(kinect.main_sign_in_flag==true ){
				if(kinect.user.center_x!=0 && kinect.lost_flag==false && robot.emergency_stop_flag==false){					//���[�U�����o�ł��Ă��
					robot.user_data.center_x = kinect.user.center_x;
					robot.user_data.center_depth = kinect.user.center_depth;
					//robot.Main();

					kalman.robot_x = robot.myRobot->getX();
					kalman.robot_y = robot.myRobot->getY();
					kalman.robot_w = robot.myRobot->getTh();
					kalman.human_d = kinect.user.center_depth;
					kalman.human_x = kinect.user_x;
					if(kinect.user_reprobe_flag == true)kalman.kalman_init_flag =true;
					kalman.main();
					kinect.camera_prediction_pos_x = kalman.camera_prediction_pos_x;
					kinect.camera_prediction_pos_d = kalman.camera_prediction_pos_d;
					kinect.kalman_range_diff = kalman.range_diff;
					kinect.user_reprobe_flag = false;

				}else{
					robot.myRobot->stop();
				}
			}
			//�\�i�[�̒l�\��
			GetDlgItem(IDC_EDIT_SONAR0)->SetWindowText(robot.sonar_level[0]);
			GetDlgItem(IDC_EDIT_SONAR1)->SetWindowText(robot.sonar_level[1]);
			GetDlgItem(IDC_EDIT_SONAR2)->SetWindowText(robot.sonar_level[2]);
			GetDlgItem(IDC_EDIT_SONAR3)->SetWindowText(robot.sonar_level[3]);
			GetDlgItem(IDC_EDIT_SONAR4)->SetWindowText(robot.sonar_level[4]);
			GetDlgItem(IDC_EDIT_SONAR5)->SetWindowText(robot.sonar_level[5]);
			GetDlgItem(IDC_EDIT_SONAR6)->SetWindowText(robot.sonar_level[6]);
			GetDlgItem(IDC_EDIT_SONAR7)->SetWindowText(robot.sonar_level[7]);
		}
		break;

	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}





