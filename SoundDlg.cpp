// SoundDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ThemePacker.h"
#include "SoundDlg.h"
#include "afxdialogex.h"
#include "WavFile.h"

// CSoundDlg dialog

IMPLEMENT_DYNAMIC(CSoundDlg, CDialogEx)

CSoundDlg::CSoundDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SOUND, pParent)
	, m_path(_T(""))
	, m_name(_T(""))
{
	m_id = 0;
	m_alreadyhave.Empty();
	m_wv = 0;
	m_wvsize = 0;
}

CSoundDlg::~CSoundDlg()
{
}

void CSoundDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MFCEDITBROWSE1, m_path);
	DDX_Text(pDX, IDC_EDIT1, m_name);
	DDX_Control(pDX, IDC_COMBO1, m_idlist);
}


BEGIN_MESSAGE_MAP(CSoundDlg, CDialogEx)
//	ON_EN_KILLFOCUS(IDC_MFCEDITBROWSE1, &CSoundDlg::OnKillfocusFile)
ON_EN_KILLFOCUS(IDC_MFCEDITBROWSE1, &CSoundDlg::OnKillfocusFilename)
END_MESSAGE_MAP()


// CSoundDlg message handlers

static const char * const ids = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

char CSoundDlg::GetUnusedID()
{
	for (const char *a = ids; *a; a++) {
		if (m_alreadyhave.Find(*a) < 0)
			return *a;
	}
	return 0;
}

BOOL CSoundDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	TCHAR s[2];
	s[1] = 0;
	for (const char *a = ids; *a; a++) {
		s[0] = (TCHAR)*a;
		if (m_alreadyhave.Find(s[0]) >= 0) continue;
		m_idlist.AddString(s);
	}
	if (m_id) {
		s[0] = m_id;
		m_idlist.SelectString(0, s);
	}
	else {
		CString ss;
		m_idlist.GetLBText(0, ss);
		m_idlist.SelectString(0, ss);
	}
	GetDlgItem(IDC_MFCEDITBROWSE1)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

extern "C" {
#include "wavpack\bits.c"
#include "wavpack\pack.c"
#include "wavpack\tinypack.c"
#include "wavpack\words.c"
#include "wavpack\wputils.c"
int test_wv_file(char *, int);
};


bool CSoundDlg::TestFile()
{
	CFileStatus fs;
	if (!CFile::GetStatus(m_path, fs)) {
		AfxMessageBox(_T("File not found"));
		return false;
	}

	if (fs.m_attribute == CFile::Attribute::directory) {
		AfxMessageBox(_T("File is a directory"));
		return false;
	}

	if (m_path.Right(3) == ".wv") {
		m_wv = malloc((m_wvsize = (int)fs.m_size));
		if (!m_wv) {
			AfxMessageBox(_T("Memory error :*"));
			return false;
		}

		FILE *f = fopen(m_path, "rb");
		if (!f) {
			free(m_wv);
			m_wv = 0;
			AfxMessageBox("file read error");
			return false;
		}
		fread(m_wv, 1, m_wvsize, f);
		fclose(f);

		if (test_wv_file((char*)m_wv, m_wvsize)) {
			free(m_wv);
			m_wv = 0;
			AfxMessageBox(_T("Wv file must be 16KHz, 16bit, mono, up to 10sec"));
			return false;
		}
	}
	else {
		WavInFile filet(m_path);

		if (filet.getBytesPerSample() != 2 || filet.getSampleRate() != 16000 || filet.getNumChannels() != 1 || filet.getNumSamples()>160000) {
			AfxMessageBox(_T("Wav file must be 16KHz, 16bit, mono, up to 10sec"));
			return false;
		}

		static short buff[320000];
		filet.read((short*)buff, filet.getNumSamples());
		m_wvsize = pack_buffer((char*)buff, filet.getNumSamples());

		//FILE *f;
		//f = fopen("c:\\w\\out.wv", "wb");
		//fwrite(buff, 1, m_wvsize, f);
		//fclose(f);

		m_wv = malloc(m_wvsize);
		if (!m_wv) {
			AfxMessageBox(_T("Memory error :*"));
			return false;
		}
		memcpy(m_wv, buff, m_wvsize);
	}
	return true;
}


void CSoundDlg::OnOK()
{
	UpdateData();

	if (!TestFile()) {
		GetDlgItem(IDC_MFCEDITBROWSE1)->SetFocus();
		return;
	}
	CString s;
	m_idlist.GetLBText(m_idlist.GetCurSel(), s);
	m_id = s[0];

	CDialogEx::OnOK();
}

void CSoundDlg::CreateNameFromFileName()
{
	m_name = PathFindFileName(m_path);
	if (m_name.Right(4).MakeLower() == ".wav") m_name = m_name.Left(m_name.GetLength() - 4);
	else if (m_name.Right(3).MakeLower() == ".wv") m_name = m_name.Left(m_name.GetLength() - 3);
}

void CSoundDlg::OnKillfocusFilename()
{
	UpdateData();
	if (m_name.IsEmpty()) {
		UpdateData(0);
		CreateNameFromFileName();
	}
	GetDlgItem(IDC_EDIT1)->SetFocus();
}
