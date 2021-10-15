
// ExamWorkerThreadDlg.h: 헤더 파일
//

#pragma once
#include <cstring>

#define MESSAGE_INCREASE_COUNT WM_USER

// CExamWorkerThreadDlg 대화 상자
class CExamWorkerThreadDlg : public CDialogEx
{
// 생성입니다.
public:
	CExamWorkerThreadDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXAMWORKERTHREAD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:
	CWinThread* m_pThread;
	CWinThread* m_pThread2;
	bool m_isWorkingThread;
	bool m_isWorkingThread2;
	int m_nCount;
	short xvel=0, yvel=0;
	CString text1;
	CString text2;
	CString text3;
	CString text4;
	CString text5;
	CString text6;
	CString text7;
	CString text8;
// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	static UINT ThreadUsing(LPVOID param);
	static UINT ThreadUsing2(LPVOID param);
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_list1;
	CRect m_image_rect;
	CImage m_image;
	CRect m_image_rect1;
	CImage m_image1;
	afx_msg void OnBnClickedButtonThread();
	afx_msg void OnBnClickedButtonThread2();
};
