
// ThemePackerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ThemePacker.h"
#include "ThemePackerDlg.h"
#include "afxdialogex.h"
#include "SoundDlg.h"
#include "MusicDlg.h"
#include "STBwriter.h"
#include "STBreader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#pragma comment(lib, "Winmm.lib")

// CThemePackerDlg dialog



CThemePackerDlg::CThemePackerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_THEMEPACKER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	waveout = 0;
}

void CThemePackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROJECTBUTTON, m_projectbtn);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_EDIT, m_btnEdit);
	DDX_Control(pDX, IDC_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_PLAY, m_btnPlay);
	DDX_Control(pDX, IDC_ADDAUTO, m_autoaddBtn);
	DDX_Control(pDX, IDC_SAVE, m_btnSave);
}

BEGIN_MESSAGE_MAP(CThemePackerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO2, &CThemePackerDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO1, &CThemePackerDlg::OnBnClickedRadio1)
	ON_COMMAND(ID_FILE_NEW32771, &CThemePackerDlg::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN32772, &CThemePackerDlg::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE32773, &CThemePackerDlg::OnFileSave)
	ON_COMMAND(ID_FILE_EXIT, &CThemePackerDlg::OnFileExit)
	ON_BN_CLICKED(IDC_ADD, &CThemePackerDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_EDIT, &CThemePackerDlg::OnBnClickedEdit)
	ON_BN_CLICKED(IDC_DELETE, &CThemePackerDlg::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_PLAY, &CThemePackerDlg::OnBnClickedPlay)
	ON_WM_SIZE()
//	ON_NOTIFY(BCN_DROPDOWN, IDC_PROJECTBUTTON, &CThemePackerDlg::OnDropdownProjectbutton)
ON_BN_CLICKED(IDC_PROJECTBUTTON, &CThemePackerDlg::OnClickedProjectbutton)
ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CThemePackerDlg::OnDblclkList1)
ON_MESSAGE(WOM_DONE, &CThemePackerDlg::OnWomDone)
ON_COMMAND(ID_FILE_SAVEAS, &CThemePackerDlg::OnFileSaveas)
ON_BN_CLICKED(IDC_ADDAUTO, &CThemePackerDlg::OnBnClickedAddauto)
ON_WM_DROPFILES()
ON_BN_CLICKED(IDC_SAVE, &CThemePackerDlg::OnClickedSave)
END_MESSAGE_MAP()


// CThemePackerDlg message handlers

BOOL CThemePackerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	saved = true;
	GetWindowText(windowTitle);
	if (fileName.IsEmpty()) setWindowName();
	else doOpen();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	menu.LoadMenu(IDR_MENU1);
	m_projectbtn.m_hMenu = menu.GetSubMenu(0)->GetSafeHmenu();
	m_projectbtn.m_bOSMenu = FALSE;
	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);
	m_list.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 300, 0);
	m_list.InsertColumn(1, _T("ID"), LVCFMT_LEFT, 50, 1);
	OnBnClickedRadio1();

	RECT rb, rl, rw;
	m_btnAdd.GetWindowRect(&rb);
	ScreenToClient(&rb);
	m_list.GetWindowRect(&rl);
	ScreenToClient(&rl);
	GetClientRect(&rw);
	m_lbdiff = rb.top - rl.bottom;
	m_bwdiff = rw.bottom - rb.bottom;
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	DragAcceptFiles();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CThemePackerDlg::OnPaint()
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
HCURSOR CThemePackerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CThemePackerDlg::OnBnClickedRadio1()
{
	m_list.DeleteAllItems();

	LVCOLUMN lvc = { 0 };
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.cx = 50;
	lvc.pszText = _T("ID");
	m_list.SetColumn(1, &lvc);

	for (int i = 0; i < sounds.GetCount(); i++) {
		int k = m_list.InsertItem(m_list.GetItemCount(), sounds[i].name);
		m_list.SetItemText(k, 1, CString(sounds[i].id));
	}
	m_soundshown = true;
}


void CThemePackerDlg::OnBnClickedRadio2()
{
	m_list.DeleteAllItems();

	LVCOLUMN lvc = { 0 };
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.cx = 300;
	lvc.pszText = _T("Song");
	m_list.SetColumn(1, &lvc);

	for (int i = 0; i < musics.GetCount(); i++) {
		CString s;
		s.Format("(%d) %s", musics[i].type, musics[i].name);
		int k = m_list.InsertItem(m_list.GetItemCount(), s);
		m_list.SetItemText(k, 1, musics[i].playlist);
	}
	m_soundshown = false;
}


void CThemePackerDlg::RemoveAll()
{
	for (int i = 0; i < sounds.GetCount(); i++)
		if (sounds[i].wv) free(sounds[i].wv);
	sounds.RemoveAll();
	musics.RemoveAll();
}


void CThemePackerDlg::OnFileNew()
{
	if (!saved) {
		int res = AfxMessageBox(_T("Would you like to save changes?"), MB_YESNOCANCEL);
		if (res == IDCANCEL) return;
		if (res == IDYES) OnFileSave();
	}
	m_list.DeleteAllItems();
	RemoveAll();
	saved = true;
}


void CThemePackerDlg::doOpen()
{
	if (stb.open(fileName)) {
		AfxMessageBox("Error opening file");
		return;
	}

	m_list.DeleteAllItems();
	RemoveAll();

	int numsounds = stb.get_num_sounds();
	int nummusics = stb.get_num_musics();
	char namebuf[256];

	for (int i = 0; i < numsounds; i++) {
		SoundDesc sd;
		int pos;
		stb.get_sound_bynum(i, (unsigned char *)&sd.id, &pos, &sd.size);
		sd.wv = malloc(sd.size);
		if (!sd.wv) {
			AfxMessageBox("Memory error, exiting");
			EndDialog(1);
			return;
		}
		stb.read(sd.wv, pos, sd.size);
		stb.get_sound_name(namebuf, i);
		sd.name = namebuf;
		sounds.Add(sd);

		if (m_soundshown) {
			int k = m_list.InsertItem(m_list.GetItemCount(), sd.name);
			m_list.SetItemText(k, 1, CString((TCHAR)sd.id));
			m_list.SetItemData(k, (DWORD_PTR)sd.wv);
		}
	}

	for (int i = 0; i < nummusics; i++) {
		MusicDesc md;
		stb.get_music_bynum(i, &md.type, namebuf);
		md.playlist = namebuf;
		stb.get_music_name(namebuf, i);
		md.name = namebuf;
		musics.Add(md);

		if (!m_soundshown) {
			CString s;
			s.Format("(%d) %s", md.type, md.name);
			int i = m_list.InsertItem(m_list.GetItemCount(), s);
			m_list.SetItemText(i, 1, md.playlist);
		}
	}

	saved = true;
	setWindowName();
}


void CThemePackerDlg::OnFileOpen()
{
	if (!saved) {
		int res = AfxMessageBox(_T("Would you like to save changes?"), MB_YESNOCANCEL);
		if (res == IDCANCEL) return;
		if (res == IDYES) OnFileSave();
	}

	CFileDialog fd(TRUE, _T("stb"), _T("*.stb"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("Smart Toybox Theme (*.stb)|*.stb|All Files (*.*)|*.*||"));
	if (fd.DoModal() == IDOK) {
		fileName = fd.GetPathName();
		doOpen();
	}
}



void CThemePackerDlg::OnFileSaveas()
{
	CFileDialog fd(FALSE, _T("stb"), PathFindFileName(fileName), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Smart Toybox Theme (*.stb)|*.stb|All Files (*.*)|*.*||"));
	if (fd.DoModal() == IDOK) {
		fileName = fd.GetPathName();
		OnFileSave();
	}
}


void CThemePackerDlg::OnFileSave()
{
	if (fileName.IsEmpty()) {
		OnFileSaveas();
		return;
	}

	STBwriter stbw;

	if (!stbw.Open(fileName, sounds.GetCount(), musics.GetCount()))
		AfxMessageBox("Save failed");
	else {
		for (int i = 0; i < sounds.GetCount(); i++)
			stbw.WriteSound(sounds[i].name, sounds[i].id, sounds[i].wv, sounds[i].size);
		for (int i = 0; i < musics.GetCount(); i++)
			stbw.WriteMusic(musics[i].name, musics[i].type, musics[i].playlist);
		stbw.Close();
	}
	saved = true;
}


void CThemePackerDlg::OnFileExit()
{
	// if not saved...
	if(!saved) {
		int res = AfxMessageBox(_T("Would you like to save changes?"), MB_YESNOCANCEL);
		if (res == IDCANCEL) return;
		if (res == IDYES) OnFileSave();
	}
	EndDialog(0);
}


CString CThemePackerDlg::getUsedID()
{
	CString ret;
	for (int i = 0; i < sounds.GetCount(); i++)
		ret.AppendChar((TCHAR)sounds[i].id);
	return ret;
}


void CThemePackerDlg::OnBnClickedAdd()
{
	if (m_soundshown) {
		CSoundDlg dlg;
		dlg.m_alreadyhave = getUsedID();
		if (dlg.DoModal() == IDOK) {
			SoundDesc sd;
			sd.name = dlg.m_name;
			sd.id = dlg.m_id;
			sd.wv = dlg.m_wv;
			sd.size = dlg.m_wvsize;
			sounds.Add(sd);
			int i = m_list.InsertItem(m_list.GetItemCount(), sd.name);
			m_list.SetItemText(i, 1, CString((TCHAR)sd.id));
			m_list.SetItemData(i, (DWORD_PTR)sd.wv);
			m_list.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			saved = false;
		}
	}
	else {
		CMusicDlg dlg;
		if (dlg.DoModal() == IDOK) {
			MusicDesc md;
			md.name = dlg.m_name;
			md.playlist = dlg.m_music;
			md.type = dlg.m_type;
			musics.Add(md);
			CString s;
			s.Format("(%d) %s", md.type, md.name);
			int i=m_list.InsertItem(m_list.GetItemCount(), s);
			m_list.SetItemText(i, 1, md.playlist);
			m_list.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			saved = false;
		}
	}
}


void CThemePackerDlg::AddFiles(CStringArray &sa)
{
	CSoundDlg sdlg;
	sdlg.m_alreadyhave = getUsedID();
	for (int i = 0; i < sa.GetCount(); i++) {
		sdlg.m_path = sa[i];
		if (!sdlg.TestFile()) continue;
		sdlg.CreateNameFromFileName();
		SoundDesc sd;
		sd.name = sdlg.m_name;
		sd.id = sdlg.GetUnusedID();
		sd.wv = sdlg.m_wv;
		sd.size = sdlg.m_wvsize;
		sounds.Add(sd);
		sdlg.m_alreadyhave += sd.id;
		if (m_soundshown) {
			int k = m_list.InsertItem(m_list.GetItemCount(), sd.name);
			m_list.SetItemText(k, 1, CString((TCHAR)sd.id));
			m_list.SetItemData(k, (DWORD_PTR)sd.wv);
			if (!i) m_list.SetItemState(k, LVIS_SELECTED, LVIS_SELECTED);
		} else {
			CMusicDlg dlg;
			dlg.m_name = sd.name;
			dlg.m_music.AppendChar(sd.id);
			if (dlg.DoModal() == IDOK) {
				MusicDesc md;
				md.name = dlg.m_name;
				md.playlist = dlg.m_music;
				md.type = dlg.m_type;
				musics.Add(md);
				CString s;
				s.Format("(%d) %s", md.type, md.name);
				int i = m_list.InsertItem(m_list.GetItemCount(), s);
				m_list.SetItemText(i, 1, md.playlist);
				m_list.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			}
		}
		saved = false;
	}
}


void CThemePackerDlg::OnBnClickedAddauto()
{
	CFileDialog fd(TRUE, _T("wav"), _T("*.wav"), OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("Wave Audio File (*.wav)|*.wav|Wavpack Compressed Audio File (*.wv)|*.wv|All Files (*.*)|*.*||"));
	if (fd.DoModal() == IDOK) {
		char *fn = fd.GetOFN().lpstrFile;
		CString path(fn);
		CStringArray sa;
		int slen = strlen(fn) + 1;
		if (fd.GetOFN().nFileOffset < slen) {
			// single file
			sa.Add(path);
		}
		else {
			fn += slen;
			// multiple files
			while (*fn) {
				sa.Add(path + '\\' + fn);
				fn += strlen(fn) + 1;
			}
		}
		AddFiles(sa);
	}
}


void CThemePackerDlg::OnDropFiles(HDROP hDropInfo)
{
	CStringArray sa;
	char buf[MAX_PATH + 1];
	int numfiles = DragQueryFile(hDropInfo, -1, 0, 0);
	for (int i = 0; i < numfiles; i++) {
		DragQueryFile(hDropInfo, i, buf, MAX_PATH);
		sa.Add(buf);
	}
	DragFinish(hDropInfo);
	AddFiles(sa);
	//CDialogEx::OnDropFiles(hDropInfo);
}


void CThemePackerDlg::OnBnClickedEdit()
{
	if (!m_list.GetSelectedCount()) {
		AfxMessageBox(_T("No selected items..."));
		return;
	}
	POSITION p = m_list.GetFirstSelectedItemPosition();
	int item = m_list.GetNextSelectedItem(p);

	if (m_soundshown) {
		CSoundDlg dlg;
		dlg.m_alreadyhave = getUsedID();
		dlg.m_alreadyhave.Remove((TCHAR)sounds[item].id);
		dlg.m_id = sounds[item].id;
		dlg.m_name = sounds[item].name;
		if (dlg.DoModal() == IDOK) {
			if (sounds[item].wv != dlg.m_wv) {
				free(sounds[item].wv);
				sounds[item].wv = dlg.m_wv;
				sounds[item].size = dlg.m_wvsize;
				m_list.SetItemData(item, (DWORD_PTR)dlg.m_wv);
			}
			sounds[item].name = dlg.m_name;
			sounds[item].id = dlg.m_id;
			m_list.SetItemText(item, 0, dlg.m_name);
			m_list.SetItemText(item, 1, CString((TCHAR)dlg.m_id));
			saved = false;
		}
	}
	else {
		CMusicDlg dlg;
		dlg.m_name = musics[item].name;
		dlg.m_music = musics[item].playlist;
		dlg.m_type = musics[item].type;
		if (dlg.DoModal() == IDOK) {
			musics[item].name = dlg.m_name;
			musics[item].playlist = dlg.m_music;
			musics[item].type = dlg.m_type;
			CString s;
			s.Format("(%d) %s", dlg.m_type, dlg.m_name);
			m_list.SetItemText(item, 0, s);
			m_list.SetItemText(item, 1, dlg.m_music);
			saved = false;
		}
	}
}


void CThemePackerDlg::OnBnClickedDelete()
{
	if (!m_list.GetSelectedCount()) {
		AfxMessageBox(_T("No selected items..."));
		return;
	}

	POSITION p = m_list.GetFirstSelectedItemPosition();
	int item = m_list.GetNextSelectedItem(p);
	m_list.DeleteItem(item);
	if (m_soundshown) {
		if (sounds[item].wv) free(sounds[item].wv);
		sounds.RemoveAt(item);
	} 
	else musics.RemoveAt(item);
	saved = false;
}


void CThemePackerDlg::OnBnClickedPlay()
{
	if (!m_list.GetSelectedCount()) {
		AfxMessageBox(_T("No selected items..."));
		return;
	}
	POSITION p = m_list.GetFirstSelectedItemPosition();
	int item = m_list.GetNextSelectedItem(p);
	
	if (m_soundshown)
		//playBuffer((char*)sounds[item].wv, sounds[item].size);
		playSound(sounds[item].id);
	else
		playMelody(musics[item].playlist);
}


void CThemePackerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (nType != SIZE_MAXIMIZED && nType != SIZE_RESTORED) return;

	CRect rb;
	if (!m_btnAdd.m_hWnd) return;
	m_btnAdd.GetWindowRect(&rb);
	ScreenToClient(&rb);
	int diff = (cy - m_bwdiff) - rb.bottom;
	rb.OffsetRect(0, diff);
	m_btnAdd.MoveWindow(&rb);
	m_autoaddBtn.GetWindowRect(&rb);
	ScreenToClient(&rb);
	rb.OffsetRect(0, diff);
	m_autoaddBtn.MoveWindow(&rb);
	m_btnEdit.GetWindowRect(&rb);
	ScreenToClient(&rb);
	rb.OffsetRect(0, diff);
	m_btnEdit.MoveWindow(&rb);
	m_btnDelete.GetWindowRect(&rb);
	ScreenToClient(&rb);
	rb.OffsetRect(0, diff);
	m_btnDelete.MoveWindow(&rb);
	m_btnPlay.GetWindowRect(&rb);
	ScreenToClient(&rb);
	rb.OffsetRect(0, diff);
	m_btnPlay.MoveWindow(&rb);
	m_btnSave.GetWindowRect(&rb);
	ScreenToClient(&rb);
	rb.OffsetRect(0, diff);
	m_btnSave.MoveWindow(&rb);
	m_list.GetWindowRect(&rb);
	ScreenToClient(&rb);
	rb.bottom += diff;
	rb.right = cx;
	m_list.MoveWindow(&rb);
}


void CThemePackerDlg::OnClickedProjectbutton()
{
	switch (m_projectbtn.m_nMenuResult)
	{
	case ID_FILE_OPEN32772: OnFileOpen(); break;
	case ID_FILE_NEW32771: OnFileNew(); break;
	case ID_FILE_EXIT: OnFileExit(); break;
	case ID_FILE_SAVE32773: OnFileSave(); break;
	case ID_FILE_SAVEAS: OnFileSaveas(); break;
	}
}


void CThemePackerDlg::OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (m_list.GetSelectedCount()) OnBnClickedEdit();
	*pResult = 0;
}


void CThemePackerDlg::OnCancel()
{
	OnFileExit();
}


void CThemePackerDlg::OnClickedSave()
{
	if (!m_list.GetSelectedCount()) {
		AfxMessageBox(_T("No selected items..."));
		return;
	}
	if (!m_soundshown) {
		AfxMessageBox(_T("Please select sound..."));
		return;
	}
	POSITION p = m_list.GetFirstSelectedItemPosition();
	int item = m_list.GetNextSelectedItem(p);

	if (!sounds[item].wv || !sounds[item].size) {
		AfxMessageBox(_T("No sound found..."));
		return;
	}

	CFileDialog fd(FALSE, _T("wv"), _T("*.wv"), OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, _T("Wavpack Compressed Audio File (*.wv)|*.wv|All Files (*.*)|*.*||"));
	if (fd.DoModal() == IDOK) {
		FILE *f = fopen(fd.GetOFN().lpstrFile, "wb");
		if (f) {
			fwrite(sounds[item].wv, 1, sounds[item].size, f);
			fclose(f);
		}
		else {
			AfxMessageBox(_T("Failed to save file"));
			return;
		}
	}
}


/////// PLAY


extern "C" {
#include "wp\float.c"
#include "wp\metadata.c"
#include "wp\bits.c"
#include "wp\unpack.c"
#include "wp\words.c"
#include "wp\wputils.c"
#include "wp\tinyunpack.c"
};

static bool justreset = false;
static char playmelody[256], *playpos;

void CThemePackerDlg::stopPlay()
{
	if (!waveout) return;
	waveOutReset(waveout);
	waveOutUnprepareHeader(waveout, &wihdr, sizeof(wihdr));
	waveOutClose(waveout);
	waveout = 0;
}


void CThemePackerDlg::nextSong()
{
	do {
		char c = *playpos++;
		if (!c) {
			stopPlay();
			return;
		}
		for (int i = 0; i < sounds.GetCount(); i++)
			if (sounds[i].id == c) {
				playBuffer((char*)sounds[i].wv, sounds[i].size);
				return;
			}
	} while (1);
}


void CThemePackerDlg::playMelody(const char *melody)
{
	strcpy(playmelody, melody);
	playpos = playmelody;
	if (waveout) {
		waveOutReset(waveout);
		waveOutUnprepareHeader(waveout, &wihdr, sizeof(wihdr)); // will call nextsong from callback
	} else
		nextSong();
}


bool CThemePackerDlg::playBuffer(char *buf, int insize)
{
	if (!waveout) {
	//	justreset = true;
	//	waveOutReset(waveout);
	//	waveOutUnprepareHeader(waveout, &wihdr, sizeof(wihdr));
	//}
	//else {
		WAVEFORMATEX wf = { 0 };
		wf.cbSize = sizeof(wf);
		wf.nChannels = 1;
		wf.nSamplesPerSec = 16000;
		wf.wFormatTag = WAVE_FORMAT_PCM;
		wf.wBitsPerSample = 16;
		wf.nBlockAlign = 2;
		wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

		if (waveOutOpen(&waveout, WAVE_MAPPER, &wf, (DWORD_PTR)GetSafeHwnd(), 0, CALLBACK_WINDOW | WAVE_ALLOWSYNC) != MMSYSERR_NOERROR)
			return false;
	}

	int size = unpack_buffer(buf, (int*)rec_data_buf, insize) * 2; // *2 = 16bit

	wihdr.dwFlags = 0;
	wihdr.lpData = (LPSTR)rec_data_buf;
	wihdr.dwBufferLength = size;

	waveOutPrepareHeader(waveout, &wihdr, sizeof(wihdr));
	waveOutWrite(waveout, &wihdr, sizeof(wihdr));
	return true;
}


afx_msg LRESULT CThemePackerDlg::OnWomDone(WPARAM wParam, LPARAM lParam)
{
	if(!justreset)
		nextSong();
	else
		justreset = false;

	return 0;
}

