
// DoorMonitorServiceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DoorMonitorService.h"
#include "DoorMonitorServiceDlg.h"
#include "afxdialogex.h"
#include "DoorMonitorServiceDlgModule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CDoorMonitorServiceDlg dialog




CDoorMonitorServiceDlg::CDoorMonitorServiceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDoorMonitorServiceDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	change_flag = false;
}

void CDoorMonitorServiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DOORS, c_list_door);
	DDX_Control(pDX, IDC_EDIT_LOG, c_edit_log);
	DDX_Control(pDX, IDC_EDIT_PORT, c_edit_port);
	DDX_Control(pDX, IDC_STATIC_TIMELONG, c_static_timelong);
}

BEGIN_MESSAGE_MAP(CDoorMonitorServiceDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CDoorMonitorServiceDlg::OnBnClickedButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CDoorMonitorServiceDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CDoorMonitorServiceDlg::OnBnClickedButtonClear)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DOORS, &CDoorMonitorServiceDlg::OnNMClickListDoors)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DOORS, &CDoorMonitorServiceDlg::OnNMRClickListDoors)
	ON_COMMAND_RANGE(ID_MENU_SELALL, ID_MENU_NAMELIST_ROW, CDoorMonitorServiceDlg::MouseRightMenuEvent)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_DOORS, &CDoorMonitorServiceDlg::OnNMCustomdrawListDoors)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDoorMonitorServiceDlg message handlers

BOOL CDoorMonitorServiceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRect rect;  
	::GetWindowRect(m_hWnd,rect);//这里m_hWnd为窗口句柄，如果不存在此变量则在该行代码前加一句：HWND h_Wnd=GetSafeHwnd( );  
	ScreenToClient(rect);  
	m_nDlgWidth = rect.right - rect.left;  
	m_nDlgHeight = rect.bottom - rect.top;  
	//Calc 分辨率  
	LONG m_nWidth = GetSystemMetrics(SM_CXSCREEN);  
	LONG m_nHeight = GetSystemMetrics(SM_CYSCREEN);  
	//计算放大倍数(要用float值，否则误差很大)
	m_Multiple_width = float(m_nWidth)/float(m_nDlgWidth);  
	m_Multiple_height = float(m_nHeight)/float(m_nDlgHeight);     
	change_flag = true;//用来判断OnSize执行时，OninitDialg是否已经执行了  
	//ShowWindow(SW_MAXIMIZE);

	//CImageList   m_l;   
	//m_l.Create(1,20,TRUE|ILC_COLOR32,1,0);   
	//c_list_door.SetImageList(&m_l,LVSIL_SMALL); 

	font_.CreatePointFont(100, "Arial");
	c_list_door.SetFont(&font_);

	DWORD dwStyle = c_list_door.GetExtendedStyle();     
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_CHECKBOXES;
	//c_list_door.SetExtendedStyle(dwStyle);
	ListView_SetExtendedListViewStyle(c_list_door,dwStyle);
	c_list_door.InsertColumn(0, _T(""), LVCFMT_LEFT, 25);
	c_list_door.InsertColumn(1, _T("机序列号"), LVCFMT_LEFT, 80);
	c_list_door.InsertColumn(2, _T("机号"), LVCFMT_LEFT, 80);
	c_list_door.InsertColumn(3, _T("设备位置"), LVCFMT_LEFT, 120);
	c_list_door.InsertColumn(4, _T("IP:端口"), LVCFMT_LEFT, 120);
	c_list_door.InsertColumn(5, _T("状态"), LVCFMT_LEFT, 80);
	c_list_door.InsertColumn(6, _T("信息"), LVCFMT_LEFT, 420);
	c_list_door.InsertColumn(7, _T(""), LVCFMT_LEFT, 150);
	currentIndex = -1;
	c_edit_port.SetWindowText("52000");

	//boost::shared_ptr<CDoorMonitorServiceDlgModule> p(new CDoorMonitorServiceDlgModule());
	module_ = CDoorMonitorServiceDlgModule::getInstance();
	module_->setDelegate(this);

	//this->SetBackgroundColor(RGB(155,170,190));
	ResizeWH(); 
	ReSizeAll();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDoorMonitorServiceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDoorMonitorServiceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDoorMonitorServiceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDoorMonitorServiceDlg::OnBnClickedButtonRun()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPort = "52000";
	c_edit_port.GetWindowTextA(strPort);
	int port = atoi(strPort);
	module_->Run(port);
}

void CDoorMonitorServiceDlg::OnBnClickedButtonStop()
{
	module_->Stop();
}


LRESULT CDoorMonitorServiceDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	CString strmsg;
	switch (message)
	{
	case UM_END:
		strmsg = (LPCTSTR)lParam;
		module_->Disconnect(strmsg.GetBuffer());
		break;
	case UM_LISTEN:
		SetTimer(0,1000,NULL);
		break;
	case UM_STOP:
		KillTimer(0);
		break;
	default:
		break;
	}
	return CDialogEx::DefWindowProc(message, wParam, lParam);
}

void CDoorMonitorServiceDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	module_->OnTimer(nIDEvent);
	CDialogEx::OnTimer(nIDEvent);
}


void CDoorMonitorServiceDlg::OnBnClickedButtonClear()
{
	// TODO: 在此添加控件通知处理程序代码
	c_edit_log.SetWindowTextA("");
}


BOOL CDoorMonitorServiceDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	return CDialogEx::DestroyWindow();
}

void CDoorMonitorServiceDlg::OnNMClickListDoors(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int index = pNMItemActivate->iItem;
	currentIndex=index;
	if( index == -1 )
	{
		return;
	}

	int subIndex = pNMItemActivate->iSubItem;
	BOOL bUnChecked = c_list_door.GetCheck(index);
	if(subIndex!=0){
		c_list_door.SetCheck(index,!bUnChecked);
	}

	module_->SetMachineCheck(GetMserWithIndex(index),!bUnChecked);

	*pResult = 0;
}


void CDoorMonitorServiceDlg::OnNMRClickListDoors(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int index = pNMItemActivate->iItem;
	currentIndex=index;
	CMenu menu;
    menu.LoadMenu(IDR_MENU_RIGHT);
    CPoint point;
    GetCursorPos(&point);
    CMenu* pSubMenu = menu.GetSubMenu(0);
    
	if(currentIndex>-1){
		int color =c_list_door.GetItemData(index);
		if(color==0){
			//CMenu* pMenu = pSubMenu->GetSubMenu(7);
			UINT id =pSubMenu->GetMenuItemID(7);
			pSubMenu->EnableMenuItem(id,0);
			id =pSubMenu->GetMenuItemID(8);
			pSubMenu->EnableMenuItem(id,0);
			id =pSubMenu->GetMenuItemID(9);
			pSubMenu->EnableMenuItem(id,0);
		}
	}

	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

	*pResult = 0;
}

void CDoorMonitorServiceDlg::OnNMCustomdrawListDoors(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NMCUSTOMDRAW nmCustomDraw = pNMCD->nmcd;
	switch(nmCustomDraw.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
		{
			if (1 == nmCustomDraw.lItemlParam)
			{
				//pNMCD->clrTextBk = RGB(255, 0, 0);		//背景颜色
				pNMCD->clrText = RGB(255, 0, 0);		//文字颜色
			}
			else if (0 == nmCustomDraw.lItemlParam)
			{
				//pNMCD->clrTextBk = RGB(255, 255, 255);
				pNMCD->clrText = RGB(0, 100, 0);
			}
			break;
		}
	default:
		{
			break;	
		}
	}

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;		//必须有，不然就没有效果
	*pResult |= CDRF_NOTIFYITEMDRAW;		//必须有，不然就没有效果
}


void CDoorMonitorServiceDlg::MouseRightMenuEvent(UINT nID)
{
	switch(nID)
	{
	case ID_MENU_SELALL:
		//MessageBox("ID_MENU_SELALL");
		SelectAll();
		break;
	case ID_MENU_CANCLE:
		UnSelectAll();
		break;
	case ID_MENU_SETTIME_ROW:
		module_->SendCommend(GetMserWithIndex(currentIndex),0);
		break;
	case ID_MENU_PAMA_ROW:
		module_->SendCommend(GetMserWithIndex(currentIndex),1);
		break;
	case ID_MENU_NAMELIST_ROW:
		module_->SendCommend(GetMserWithIndex(currentIndex),2);
		break;
	case ID_MENU_SETTIME:
		module_->SendCommend("",3);
		break;
	case ID_MENU_PAMA:
		module_->SendCommend("",4);
		break;
	case ID_MENU_NAMELIST:
		module_->SendCommend("",5);
		break;
	default:
		break;
	}
}

std::string CDoorMonitorServiceDlg::GetMserWithIndex(int index)
{
	std::string mser;
	if(index<0)
		return mser;

	mser = c_list_door.GetItemText(index,1);
	return mser;
}

void CDoorMonitorServiceDlg::SelectAll()
{
	int count = c_list_door.GetItemCount();
	for(int i=0;i<count;++i)
	{
		c_list_door.SetCheck(i,1);
		module_->SetMachineCheck(GetMserWithIndex(i),true);
	}
}

void CDoorMonitorServiceDlg::UnSelectAll()
{
	int count = c_list_door.GetItemCount();
	for(int i=0;i<count;++i)
	{
		c_list_door.SetCheck(i,0);
		module_->SetMachineCheck(GetMserWithIndex(i),false);
	}
}


HBRUSH CDoorMonitorServiceDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if(pWnd->GetDlgCtrlID()==IDC_EDIT_LOG)//如果是编辑框
    {
        pDC->SetTextColor(RGB(0,255,0));//设置编辑框字体的颜色
		//pDC->GetBoundsRect
        pDC->SetBkColor(RGB(0,0,0));//设置字体背景颜色
		//pDC->
        CFont font;
		LOGFONT   logFont;  
		logFont.lfHeight=14;                    //字体高度
		logFont.lfWidth=0;                      //字体平均宽度
		logFont.lfEscapement=0;                 //文本行逆时针旋转角度
		logFont.lfOrientation=0;                //字体角度
		logFont.lfWeight=FW_NORMAL;             //字体粗细程度
		logFont.lfItalic=0;                     //倾斜
		logFont.lfStrikeOut=0;                  //删除线
		logFont.lfCharSet=ANSI_CHARSET;         //
		logFont.lfOutPrecision=OUT_DEFAULT_PRECIS;  
		logFont.lfClipPrecision=OUT_DEFAULT_PRECIS;  
		logFont.lfQuality=PROOF_QUALITY;  
		logFont.lfPitchAndFamily=VARIABLE_PITCH|FF_ROMAN;  
		//	wcscpy_s(logFont.lfFaceName, _T("隶书")); 
		font.CreateFontIndirect(&logFont);  
		pWnd->SetFont(&font,FALSE);  

        //font.CreatePointFont(90,"宋体");
        pDC->SelectObject(&font);//设置字体        
        //return m_brush;
		//pDC->SetBkMode(TRANSPARENT);
		return CreateSolidBrush(RGB(0, 0,0));
    }

	return hbr;
}

BOOL CDoorMonitorServiceDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CDialogEx::OnEraseBkgnd(pDC);
}


void CDoorMonitorServiceDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (change_flag)//如果OninitDlg已经调用完毕  
	{  
		ResizeWH();
		//ReSize(IDC_LIST_DOORS);  
		ReSizeAll();
		//恢复放大倍数，并保存 (确保还原时候能够还原到原来的大小)  
		m_Multiple_width = float(1)/m_Multiple_width;  
		m_Multiple_height = float(1)/m_Multiple_height;  
	}  
}

void CDoorMonitorServiceDlg::ResizeWH()
{
	CRect rect;  
	::GetWindowRect(m_hWnd,rect);//这里m_hWnd为窗口句柄，如果不存在此变量则在该行代码前加一句：HWND h_Wnd=GetSafeHwnd( );  
	ScreenToClient(rect);  
	LONG m_nWidth = rect.right - rect.left;  
	LONG m_nHeight = rect.bottom - rect.top;  

	//计算放大倍数(要用float值，否则误差很大)
	m_Multiple_width = float(m_nWidth)/float(m_nDlgWidth);  
	m_Multiple_height = float(m_nHeight)/float(m_nDlgHeight);  

	m_nDlgWidth = m_nWidth;
	m_nDlgHeight = m_nHeight;

}


void CDoorMonitorServiceDlg::ReSize( int nID )
{
	CRect Rect;   
	GetDlgItem(nID)->GetWindowRect(Rect);   
	ScreenToClient(Rect);   
	//计算控件左上角点   
	CPoint OldTLPoint,TLPoint;   
	OldTLPoint = Rect.TopLeft();   
	TLPoint.x = long(OldTLPoint.x *m_Multiple_width);   
	TLPoint.y = long(OldTLPoint.y * m_Multiple_height );   

	//自定义
	//if (nID==IDC_NEWS_LIST)
	//{
	TLPoint.y = OldTLPoint.y;
	//}

	//计算控件右下角点   
	CPoint OldBRPoint,BRPoint; OldBRPoint = Rect.BottomRight();   
	BRPoint.x = long(OldBRPoint.x *m_Multiple_width);   
	BRPoint.y = m_nDlgHeight-50;//long(OldBRPoint.y * m_Multiple_height );   
	//移动控件到新矩形   
	Rect.SetRect(TLPoint,BRPoint);   
	GetDlgItem(nID)->MoveWindow(Rect,TRUE);  
}

void CDoorMonitorServiceDlg::ReSizeAll()
{
	CRect Rect,TempRect,frameRect;   
	GetDlgItem(IDC_LIST_DOORS)->GetWindowRect(Rect);   
	ScreenToClient(Rect);   
	//计算控件左上角点   
	CPoint OldTLPoint,TLPoint;   
	OldTLPoint = Rect.TopLeft();   
	TLPoint.x = 10;//long(OldTLPoint.x *m_Multiple_width);   
	TLPoint.y = long(OldTLPoint.y * m_Multiple_height );   

	TLPoint.y = OldTLPoint.y;

	//计算控件右下角点   
	CPoint OldBRPoint,BRPoint; OldBRPoint = Rect.BottomRight();   
	BRPoint.x = m_nDlgWidth-25;//long(OldBRPoint.x *m_Multiple_width);   
	BRPoint.y = m_nDlgHeight*0.550f;//long(OldBRPoint.y * m_Multiple_height );   
	//移动控件到新矩形   
	Rect.SetRect(TLPoint,BRPoint);   
	GetDlgItem(IDC_LIST_DOORS)->MoveWindow(Rect,TRUE);  

	GetDlgItem(IDC_STATIC_TXT1)->GetWindowRect(Rect);   
	ScreenToClient(Rect); 
	Rect.MoveToX(TLPoint.x);
	GetDlgItem(IDC_STATIC_TXT1)->MoveWindow(Rect,TRUE);  
	//#####################################################
	GetDlgItem(IDC_STATIC_TXT2)->GetWindowRect(Rect);   
	ScreenToClient(Rect); 
	TLPoint.y = BRPoint.y+7;
	Rect.MoveToXY(TLPoint.x,TLPoint.y);
	GetDlgItem(IDC_STATIC_TXT2)->MoveWindow(Rect,TRUE);  

	GetDlgItem(IDC_EDIT_LOG)->GetWindowRect(Rect);   
	ScreenToClient(Rect);   
	//计算控件左上角点    
	TLPoint.y = TLPoint.y+15;   

	//计算控件右下角点   
	BRPoint.x = m_nDlgWidth-25;   
	BRPoint.y = m_nDlgHeight-70;
	//移动控件到新矩形   
	Rect.SetRect(TLPoint,BRPoint);   
	GetDlgItem(IDC_EDIT_LOG)->MoveWindow(Rect,TRUE);  
	//#####################################################
	GetDlgItem(IDC_STATIC_TIMELONG)->GetWindowRect(Rect);   
	ScreenToClient(Rect); 
	TLPoint.y = BRPoint.y+5;
	Rect.MoveToXY(TLPoint.x,TLPoint.y);
	GetDlgItem(IDC_STATIC_TIMELONG)->MoveWindow(Rect,TRUE);  
	//#####################################################
	GetDlgItem(IDC_BUTTON_CLEAR)->GetWindowRect(Rect);   
	ScreenToClient(Rect); 
	BRPoint.x = m_nDlgWidth-25;
	TLPoint.x = BRPoint.x-Rect.Width();
	Rect.MoveToX(TLPoint.x);
	GetDlgItem(IDC_BUTTON_CLEAR)->MoveWindow(Rect,TRUE);  

	GetDlgItem(IDC_BUTTON_STOP)->GetWindowRect(Rect);   
	ScreenToClient(Rect); 
	TLPoint.x = TLPoint.x-Rect.Width()-5;
	Rect.MoveToX(TLPoint.x);
	GetDlgItem(IDC_BUTTON_STOP)->MoveWindow(Rect,TRUE);  

	GetDlgItem(IDC_BUTTON_RUN)->GetWindowRect(Rect);   
	ScreenToClient(Rect); 
	TLPoint.x = TLPoint.x-Rect.Width()-5;
	Rect.MoveToX(TLPoint.x);
	GetDlgItem(IDC_BUTTON_RUN)->MoveWindow(Rect,TRUE);

	GetDlgItem(IDC_EDIT_PORT)->GetWindowRect(Rect);   
	ScreenToClient(Rect); 
	TLPoint.x = TLPoint.x-Rect.Width()-5;
	Rect.MoveToX(TLPoint.x);
	GetDlgItem(IDC_EDIT_PORT)->MoveWindow(Rect,TRUE);

	GetDlgItem(IDC_STATIC_TXT3)->GetWindowRect(Rect);   
	ScreenToClient(Rect); 
	TLPoint.x = TLPoint.x-Rect.Width()-5;
	Rect.MoveToX(TLPoint.x);
	GetDlgItem(IDC_STATIC_TXT3)->MoveWindow(Rect,TRUE);

}