// MusicDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ThemePacker.h"
#include "MusicDlg.h"
#include "afxdialogex.h"


// CMusicDlg dialog

IMPLEMENT_DYNAMIC(CMusicDlg, CDialogEx)

CMusicDlg::CMusicDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MUSIC, pParent)
	, m_name(_T(""))
	, m_music(_T(""))
	, m_type(0)
{

}

CMusicDlg::~CMusicDlg()
{
}

void CMusicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_name);
	DDX_Text(pDX, IDC_EDIT2, m_music);
	DDX_Text(pDX, IDC_EDIT3, m_type);
	DDV_MinMaxByte(pDX, m_type, 0, 255);
}


BEGIN_MESSAGE_MAP(CMusicDlg, CDialogEx)
END_MESSAGE_MAP()


// CMusicDlg message handlers


void CMusicDlg::OnOK()
{
	UpdateData();
	if (m_music.IsEmpty()) {
		AfxMessageBox("You should set music you want to add");
		return;
	}
	CDialogEx::OnOK();
}


BOOL CMusicDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_EDIT3)->ModifyStyle(0, ES_NUMBER);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
