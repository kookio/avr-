
// ExamWorkerThreadDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "Motor monitoring.h"
#include "Motor monitoringDlg.h"
#include "afxdialogex.h"
#include <mysql.h>
#include<thread>
#include "chartdir.h"
#include"Motor monitoring.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CExamWorkerThreadDlg 대화 상자



CExamWorkerThreadDlg::CExamWorkerThreadDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EXAMWORKERTHREAD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pThread = NULL;
	m_isWorkingThread = false;
	m_nCount = 0;
}

void CExamWorkerThreadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list1);
}

BEGIN_MESSAGE_MAP(CExamWorkerThreadDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_THREAD, &CExamWorkerThreadDlg::OnBnClickedButtonThread)
	ON_BN_CLICKED(IDC_BUTTON_THREAD2, &CExamWorkerThreadDlg::OnBnClickedButtonThread2)
END_MESSAGE_MAP()


// CExamWorkerThreadDlg 메시지 처리기

BOOL CExamWorkerThreadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	GetDlgItem(IDC_MYPICTURE1)->GetWindowRect(m_image_rect);
	GetDlgItem(IDC_MYPICTURE2)->GetWindowRect(m_image_rect1);
	ScreenToClient(m_image_rect);
	ScreenToClient(m_image_rect1);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CExamWorkerThreadDlg::OnPaint()
{
	CPaintDC dc(this);
	if (IsIconic())
	{							 // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		
		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
		
	}
	else
	{
		if (!m_image.IsNull()) {
			dc.SetStretchBltMode(COLORONCOLOR);
			m_image.Draw(dc, m_image_rect);
		}
		if (!m_image1.IsNull()) {
			dc.SetStretchBltMode(COLORONCOLOR);
			m_image1.Draw(dc, m_image_rect1);
		}
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CExamWorkerThreadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
UINT CExamWorkerThreadDlg::ThreadUsing(LPVOID param)
{
	CExamWorkerThreadDlg* pMain = (CExamWorkerThreadDlg*)param;
	MYSQL Conn;
	MYSQL* ConnPtr = NULL;
	MYSQL_RES* Result;
	MYSQL_ROW Row;
	int Stat;
	mysql_init(&Conn);

	WSADATA data;
	int iniResult = WSAStartup(MAKEWORD(2, 2), &data);															//소켓생성
	if (iniResult != 0)
	{
		pMain->text1.Format(L"연결중");
		pMain->SetDlgItemText(IDC_STATIC2, pMain->text1);
	}

	SOCKET client = socket(AF_INET, SOCK_STREAM, 0);

	if (client == INVALID_SOCKET)
	{
		WSACleanup();																							//소켓소멸
	}
	sockaddr_in addr = {  };

	addr.sin_family = AF_INET;																					//TCP설정
	addr.sin_addr.s_addr = inet_addr("172.31.7.11");															//IP설정
	addr.sin_port = htons(9601);																				//PORT설정

	if (connect(client, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)										//소켓통신연결 및 실패처리
	{
		pMain->text1.Format(L"OFF");
		pMain->SetDlgItemText(IDC_STATIC3, pMain->text1);
		WSACleanup();
	}
	else
	{
		pMain->text1.Format(L"ON");
		pMain->SetDlgItemText(IDC_STATIC3, pMain->text1);
	}
	ConnPtr = mysql_real_connect(&Conn, "127.0.0.1", "root", "1234", "test", 3306, (char*)NULL, 0);					//데이터베이스 연결
	if (ConnPtr == NULL)																							//데이터베이스 연결 실패처리
	{
		printf("No Connection\n");
	}
	else
	{
		pMain->text1.Format(L"ON");
		pMain->SetDlgItemText(IDC_STATIC4, pMain->text1);
	}
	short senddata[1024];																							//받아올 데이터 변수 선언
	char sql1[1024] = "";
	int i = 0;
	int sum = 1;
	double data1[10] = { 0 };
	double data2[10] = { 0 };
	char* labels[10] = { 0 };
	char a1[4],b1[4],c1[4],d1[4],e1[4],f1[4],g1[4],h1[4],i1[4],j1[4];
	
	while (pMain->m_isWorkingThread)																				//스레드가 작동중일때만 루프하는 조건
	{
			char clientsend[] = "0";																				//서버에 전송할 데이터값 선언
			send(client, clientsend, strlen(clientsend), 0);														//send를 사용하여 서버에 값 전달
			recv(client, (char*)senddata, 2, 0);																	//recv를 사용하여 서버에서 전달하는 값 senddata변수에 저장
			pMain->xvel = *senddata;																					//벨트의 위치값 
			recv(client, (char*)senddata, 2, 0);
			pMain->yvel = *senddata;																					//원판의 위치값
			recv(client, (char*)senddata, 1, 0);
			char ecerror1 = *senddata;																				//ERROR코드값
			recv(client, (char*)senddata, 1, 0);
			char ecerror2 = *senddata;
			recv(client, (char*)senddata, 1, 0);
			char ecerror3 = *senddata;
			recv(client, (char*)senddata, 1, 0);
			char ecerror4 = *senddata;
			recv(client, (char*)senddata, 1, 0);
			char ecerror5 = *senddata;
			recv(client, (char*)senddata, 1, 0);
			char ecerror6 = *senddata;
			recv(client, (char*)senddata, 1, 0);
			char ecerror7 = *senddata;
			recv(client, (char*)senddata, 1, 0);
			char ecerror8 = *senddata;
			recv(client, (char*)senddata, 1, 0);
			char mcerror1 = *senddata;
			recv(client, (char*)senddata, 1, 0);
			char mcerror2 = *senddata;
			recv(client, (char*)senddata, 1, 0);
			char mcerror3 = *senddata;
			recv(client, (char*)senddata, 1, 0);
			char mcerror4 = *senddata;
			recv(client, (char*)senddata, 1, 0);
			char mcerror5 = *senddata;
			recv(client, (char*)senddata, 1, 0);
			char mcerror6 = *senddata;
			recv(client, (char*)senddata, 1, 0);
			char mcerror7 = *senddata;
			recv(client, (char*)senddata, 1, 0);
			char mcerror8 = *senddata;

			pMain->text2.Format(L"%d", pMain->xvel);
			pMain->SetDlgItemText(IDC_STATIC5, pMain->text2);
			pMain->text3.Format(L"%d", pMain->yvel);
			pMain->SetDlgItemText(IDC_STATIC6, pMain->text3);
			if (ecerror1 != NULL && mcerror1 != NULL)
			{
				pMain->text4.Format(L"%c%c%c%c%c%c%c%c", ecerror1, ecerror2, ecerror3, ecerror4, ecerror5, ecerror6, ecerror7, ecerror8);
				pMain->SetDlgItemText(IDC_STATIC7, pMain->text4);
				pMain->text5.Format(L"%c%c%c%c%c%c%c%c", mcerror1, mcerror2, mcerror3, mcerror4, mcerror5, mcerror6, mcerror7, mcerror8);
				pMain->SetDlgItemText(IDC_STATIC8, pMain->text5);
				sprintf(sql1, "INSERT INTO abc(A,B,C,D) VALUES(%d,%d,%c%c%c%c%c%c%c%c,%c%c%c%c%c%c%c%c)", pMain->xvel, pMain->yvel, ecerror1, ecerror2, ecerror3, ecerror4, ecerror5, ecerror6, ecerror7, ecerror8, mcerror1, mcerror2, mcerror3, mcerror4, mcerror5, mcerror6, mcerror7, mcerror8);
			}
			else {
				pMain->text4.Format(L"%d", 0);
				pMain->SetDlgItemText(IDC_STATIC7, pMain->text4);
				pMain->text5.Format(L"%d", 0);
				pMain->SetDlgItemText(IDC_STATIC8, pMain->text5);
				sprintf(sql1, "INSERT INTO abc(A,B,C,D) VALUES(%d,%d,%d,%d)", pMain->xvel, pMain->yvel, 0, 0);
			}
			Stat = mysql_query(ConnPtr, sql1);																//데이터데이스에 쓰기

			const char* Query = "SELECT * from abc order by no desc limit 1 ;";
			Stat = mysql_query(ConnPtr, Query);																//데이터베이스 읽어오기
			if (Stat != 0)																					//에러처리
			{
				printf("에러발생\n");
			}
			Result = mysql_store_result(ConnPtr);
			while ((Row = mysql_fetch_row(Result)) != NULL)													//데이터 베이스 읽어온후 리스트박스에 표현
			{
				pMain->text7.Format(_T("%S  X : %S  Y: %S  ECERROR: %S  MCERROR : %S"), Row[0], Row[1], Row[2], Row[3], Row[4]);
				pMain->m_list1.AddString(pMain->text7);
				pMain->m_list1.SetCurSel(pMain->m_list1.GetCount() - 1);
			}
			if (pMain->text4 != "0"|| pMain->text5 != "0")													//PLC에서 에러발생시 에러코드값 전송후 에러처리
			{
				pMain->m_isWorkingThread = false;
				pMain->m_isWorkingThread2 = false;
				pMain->text1.Format(L"OFF");
				pMain->text2.Format(L"OFF");
				pMain->SetDlgItemText(IDC_STATIC3, pMain->text1);
				pMain->SetDlgItemText(IDC_STATIC4, pMain->text2);
				pMain->MessageBox(_T("에러가 발생하였습니다"), _T("Error"), MB_ICONERROR);
				return 0;
			}
			mysql_free_result(Result);																		
			Sleep(1000);
		}
		closesocket(client);																						//소켓연결해제
		mysql_close(ConnPtr);																						//DB연결해제
		WSACleanup();																								//소켓소멸
		return 0;
}
UINT CExamWorkerThreadDlg::ThreadUsing2(LPVOID param)
{
	CExamWorkerThreadDlg* pMain1 = (CExamWorkerThreadDlg*)param;
	int xarea = 0;
	int yarea= 1;
	double data1[10] = { 0 };
	double data2[10] = { 0 };
	char* labels[10] = { 0 };
	char a1[4], b1[4], c1[4], d1[4], e1[4], f1[4], g1[4], h1[4], i1[4], j1[4];
	while(pMain1->m_isWorkingThread2)
	{
		if (xarea < 9)																						//그래프 X,Y값 설정								
		{
			data1[xarea] = (double)pMain1->xvel;
			data2[xarea] = (double)pMain1->yvel;
			labels[0] = "1";
			labels[1] = "2";
			labels[2] = "3";
			labels[3] = "4";
			labels[4] = "5";
			labels[5] = "6";
			labels[6] = "7";
			labels[7] = "8";
			labels[8] = "9";
			labels[9] = "10";
			xarea++;
			yarea++;
		}
		if (xarea == 9)																						//그래프 X,Y값 갱신설정
		{
			xarea = 9;
			yarea++;
			for (int k = 0; k < 9; k++)
			{
				data1[k] = data1[k + 1];
				data2[k] = data2[k + 1];
			}
			data1[xarea] = (double)pMain1->xvel;
			data2[xarea] = (double)pMain1->yvel;
			sprintf(a1, "%d", yarea - 9);
			labels[0] = (char*)a1;
			sprintf(b1, "%d", yarea - 8);
			labels[1] = (char*)b1;
			sprintf(c1, "%d", yarea - 7);
			labels[2] = (char*)c1;
			sprintf(d1, "%d", yarea - 6);
			labels[3] = (char*)d1;
			sprintf(e1, "%d", yarea - 5);
			labels[4] = (char*)e1;
			sprintf(f1, "%d", yarea - 4);
			labels[5] = (char*)f1;
			sprintf(g1, "%d", yarea - 3);
			labels[6] = (char*)g1;
			sprintf(h1, "%d", yarea - 2);
			labels[7] = (char*)h1;
			sprintf(i1, "%d", yarea - 1);
			labels[8] = (char*)i1;
			sprintf(j1, "%d", yarea);
			labels[9] = (char*)j1;

		}
		const int data_size1 = (int)(sizeof(data1) / sizeof(*data1));
		const int data_size2 = (int)(sizeof(data2) / sizeof(*data2));

		XYChart* c = new XYChart(250, 250);																		//1번그래프 생성
		XYChart* d = new XYChart(250, 250);																		//2번그래프 생성	

		c->setPlotArea(30, 5, 200, 200);
		d->setPlotArea(30, 5, 200, 200);

		c->addLineLayer(DoubleArray(data1, 10));
		d->addLineLayer(DoubleArray(data2, 10));
		
		c->xAxis()->setLabels(StringArray(labels, 10));
		d->xAxis()->setLabels(StringArray(labels, 10));
		c->xAxis()->setLabelStep(1);
		d->xAxis()->setLabelStep(1);

		c->makeChart("simpleline.bmp");																			//bmp파일로 저장
		d->makeChart("simpleline1.bmp");																		//bmp파일로 저장

		if (pMain1->m_image.IsNull()) {																			//그래프를 그릴 장소에 그림이 이미 있는지 확인하는 조건
			pMain1->m_image.Load(L"simpleline.bmp");
			pMain1->InvalidateRect(pMain1->m_image_rect, false);
		}
		if (pMain1->m_image1.IsNull()) {
			pMain1->m_image1.Load(L"simpleline1.bmp");
			pMain1->InvalidateRect(pMain1->m_image_rect1, false);
		}
		Sleep(1000);
		pMain1->m_image1.Detach();
		pMain1->m_image.Detach();
	}
	return 0;
}

void CExamWorkerThreadDlg::OnBnClickedButtonThread()																//스레드작동
{
	m_isWorkingThread = true;			
	m_isWorkingThread2 = true;
	m_pThread = AfxBeginThread(ThreadUsing, this);
	m_pThread2 = AfxBeginThread(ThreadUsing2, this);
}

void CExamWorkerThreadDlg::OnBnClickedButtonThread2()																//스레드해제
{
	m_isWorkingThread = false;
	m_isWorkingThread2 = false;
	WaitForSingleObject(m_pThread->m_hThread, 1000);
	WaitForSingleObject(m_pThread2->m_hThread, 1000);
	text1.Format(L"OFF");
	SetDlgItemText(IDC_STATIC3, text1);
	text2.Format(L"OFF");
	SetDlgItemText(IDC_STATIC4, text2);
}

