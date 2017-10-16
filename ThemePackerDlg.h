
// ThemePackerDlg.h : header file
//

#pragma once
#include "afxmenubutton.h"
#include "afxcmn.h"
#include "afxwin.h"
#include <Mmsystem.h>
#include "STBreader.h"

// CThemePackerDlg dialog
class CThemePackerDlg : public CDialogEx
{
	// Construction
public:
	CThemePackerDlg(CWnd* pParent = NULL);	// standard constructor
	~CThemePackerDlg()
	{
		RemoveAll();
	}

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_THEMEPACKER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	CMenu menu;
	StbReader stb;
	bool saved;
	CString windowTitle, fileName;

	struct SoundDesc {
		CString name;
		char id;
		void *wv;
		int size;
	};
	struct MusicDesc {
		unsigned char type;
		CString name, playlist;
	};
	CArray<SoundDesc> sounds;
	CArray<MusicDesc> musics;

	void RemoveAll();

	int m_lbdiff;
	int m_bwdiff;
	bool m_soundshown;

	HWAVEOUT waveout;
	WAVEHDR wihdr;
	void stopPlay();
	void nextSong();
	void playMelody(const char *melody);
	void playSound(char c) { char s[2] = { c, 0 }; playMelody(s); }
	bool playBuffer(char *buf, int size);
	short rec_data_buf[320000]; // 10 sec buffer * 16KHz * 16bit (*2 for compression)

	void doOpen();
	CString getUsedID();

	void setWindowName()
	{
		if (fileName.IsEmpty()) SetWindowText(_T("Untitled - " + windowTitle));
		else SetWindowText(PathFindFileName(fileName) + CString(_T(" - ")) + windowTitle);
	}
	void AddFiles(CStringArray &sa);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CMFCMenuButton m_projectbtn;
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio1();
	CListCtrl m_list;
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileExit();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_btnAdd;
	CButton m_btnEdit;
	CButton m_btnDelete;
	CButton m_btnPlay;
//	afx_msg void OnDropdownProjectbutton(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickedProjectbutton();
	afx_msg void OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	afx_msg LRESULT OnWomDone(WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
public:
	afx_msg void OnFileSaveas();
	afx_msg void OnBnClickedAddauto();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CButton m_autoaddBtn;
	CButton m_btnSave;
	afx_msg void OnClickedSave();
};
