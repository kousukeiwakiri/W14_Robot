
// W14-I1Dlg.h : ヘッダー ファイル
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

// CW14I1Dlg ダイアログ
class CW14I1Dlg : public CDialogEx
{
	// コンストラクション
public:
	CW14I1Dlg(CWnd* pParent = NULL);	// 標準コンストラクター

	// ダイアログ データ
	enum { IDD = IDD_W14I1_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
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
	std::ofstream ofss;	//オドメトリ確認
	std::ofstream ofs;	//オドメトリ確認ロボット内部
	std::ofstream ofsh;  //追跡対象人物データ
	clock_t  time_start;	//処理時間
	clock_t  now_time ,delta_t;

	int count;//デバック用


};
