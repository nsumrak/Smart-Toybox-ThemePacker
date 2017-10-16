#pragma once


// CMusicDlg dialog

class CMusicDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMusicDlg)

public:
	CMusicDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMusicDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MUSIC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_name;
	CString m_music;
	BYTE m_type;
	virtual void OnOK();
	virtual BOOL OnInitDialog();
};
