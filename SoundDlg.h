#pragma once
#include "afxwin.h"


// CSoundDlg dialog

class CSoundDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSoundDlg)

public:
	CSoundDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSoundDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SOUND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	char m_id;
	CString m_path;
	CString m_name;
	CString m_alreadyhave;
	CComboBox m_idlist;
	void *m_wv;
	int m_wvsize;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
//	afx_msg void OnKillfocusFile();
	afx_msg void OnKillfocusFilename();
	bool TestFile();
	void CreateNameFromFileName();
	char GetUnusedID();
};
