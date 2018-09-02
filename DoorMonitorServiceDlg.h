
// DoorMonitorServiceDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"
class CDoorMonitorServiceDlgModule;

// CDoorMonitorServiceDlg dialog
class CDoorMonitorServiceDlg : public CDialogEx
{
// Construction
public:
	CDoorMonitorServiceDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DOORMONITORSERVICE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL DestroyWindow();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	bool change_flag;  
	LONG m_nDlgWidth;
	LONG m_nDlgHeight;
	float m_Multiple_height;  
	float m_Multiple_width; 

	CDoorMonitorServiceDlgModule* module_;
	int currentIndex;

	std::string GetMserWithIndex(int index);
	void SelectAll();
	void UnSelectAll();

	void ResizeWH();
	void ReSize( int nID );
	void ReSizeAll();
public:
	CListCtrl c_list_door;
	CEdit c_edit_log;
	CEdit c_edit_port;
	CStatic c_static_timelong;
	CFont font_;
public:
	void MouseRightMenuEvent(UINT nID);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnNMClickListDoors(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListDoors(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawListDoors(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
