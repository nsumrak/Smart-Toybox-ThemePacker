#pragma once

#ifndef WIN32
#define EMBEDDED 1
#endif

/***********************************************
STB file format:

-- Header (12b)
LONG id = 0xBABADEDA
CHAR numsounds
3 CHAR soundsize
CHAR nummusic
3 CHAR musicsize

-- SOUNDS INDEX for each numsounds
CHAR id
3 CHAR wvsize

-- MUSIC INDEX for each nummusic
CHAR type
CHAR plsize

-- SOUND DATA (soundsize) for each numsounds
CHAR wv[wvsize]

-- MUSIC DATA (musicsize) for each nummusic
CHAR playlist[plsize]

-- NAMES for each numsounds+nummusic
CHAR size
CHAR name[size]

************************************************/

#define THEME_NAME	"theme.stb"

#ifndef EMBEDDED
#include <stdio.h>
#define INVALID_FILE_HANDLE (0)
typedef FILE *filehandle;
inline filehandle open_file(const char *fn) { return fopen(fn, "rb"); }
#else
#include "platform.h"
#include <FS.h>
typedef File filehandle;
#define INVALID_FILE_HANDLE false
inline filehandle open_file(const char *fn) {
	return SPIFFS.open(fn, "r");
}

class STBFile : public File {
	File *stb;
	int pos;
	int cur;
	int size;
public:
	STBFile(File &_stb, int _pos, int _size) {
		stb = &_stb;
		_pos = pos;
		_size = size;
		cur = 0;
	}

	size_t readBytes(char *buffer, size_t length)  override {
		stb->seek(pos + cur, SeekSet);
		if (cur + length > size) length = size - cur;
		size_t readed = stb->read((uint8_t*)buffer, length);
		cur += readed;
		return readed;
	}
};

#endif

class StbReader {
protected:
	const char *themename;
	filehandle stbfilet;
	unsigned char numsounds, nummusics;
	int soundoffs, musicoffs;
	unsigned char soundheader[384], *musicheader;
#ifndef EMBEDDED
	int namesoffs;
#endif

	static int read3Blen(const unsigned char *buf)
	{
		int res = (*buf++) << 16;
		res |= (*buf++) << 8;
		res |= *buf;
		return res;
	}


public:
	enum {
		GAME_START = 0,
		GAME_WIN,
		GAME_LOSE,
		GAME_MUSIC,
		GAME_GIMME,
		GAME_CHEW,
		GAME_CHEW_GRAND,
		GAME_GULP,
		GAME_GULP_GRAND,
		GAME_COMPLAIN,
		GAME_GUARDIAN,
		GAME_GUARDIAN_OFF,
		GAME_GOOD_PROGRESS,
		GAME_BAD_PROGRESS,
		GUARDIAN,
		GUARDIAN_OFF,
	};

	StbReader()
	{
#ifndef EMBEDDED
		stbfilet = INVALID_FILE_HANDLE;
#endif
	}

	int open(const char *filename = THEME_NAME)
	{
		stbfilet = open_file(filename);
		if (INVALID_FILE_HANDLE == stbfilet) return 1;

		themename = filename;
		unsigned char buf[12];
		if (read(buf, 0, 12) < 12 || buf[0] != 0xba || buf[1] != 0xba || buf[2] != 0xde || buf[3] != 0xda) {
			close();
			return 2;
		}

		numsounds = buf[4];
		nummusics = buf[8];

		musicoffs = read3Blen(buf + 5);
	#ifndef EMBEDDED
		namesoffs = read3Blen(buf + 9);
	#endif

		int shsize = (int)numsounds << 2;
		int hdrsize = shsize + ((int)nummusics << 1);
		read(soundheader, 12, hdrsize);

		musicheader = soundheader + shsize;
		soundoffs = hdrsize + 12;
		musicoffs += soundoffs;
	#ifndef EMBEDDED
		namesoffs += musicoffs;
	#endif

		return 0;
	}

	void close()
	{
		if (stbfilet == INVALID_FILE_HANDLE) return;
	#ifdef EMBEDDED
		stbfilet.close();
	#else
		fclose(stbfilet);
		stbfilet = INVALID_FILE_HANDLE;
	#endif
	}

	int get_sound(unsigned char c, int *pos, int *size)
	{
		if (stbfilet == INVALID_FILE_HANDLE) return 1;
		unsigned char i;
		unsigned char *sh = soundheader;
		*pos = soundoffs;

		for (i = 0; i < numsounds; i++) {
			int sz = read3Blen(sh + 1);
			if (*sh == c) {
				if (size) *size = sz;
				return 0;
			}
			*pos += sz;
			sh += 4;
		}
		return 1;
	}

#ifdef EMBEDDED
	filehandle get_sound_file(unsigned char c)
	{
		if (stbfilet == INVALID_FILE_HANDLE) return File();
		int pos, size;
		if (get_sound(c, &pos, &size)) {
			return STBFile(stbfilet, pos, size);
		}
		else return File();
	}
#endif

	int read(void *buf, int pos, int size)
	{
		if (stbfilet == INVALID_FILE_HANDLE) return 0;
	#ifdef EMBEDDED
		stbfilet.seek(pos, SeekSet);
		return stbfilet.readBytes((char*)buf, size);
	#else
		fseek(stbfilet, pos, SEEK_SET);
		return fread(buf, 1, size, stbfilet);
	#endif
	}

	int get_music_num(unsigned char type)
	{
		if (stbfilet == INVALID_FILE_HANDLE) return 0;
		unsigned char i;
		unsigned char *mh = musicheader;
		int cnt = 0;

		for (i = 0; i < nummusics; i++) {
			if (*mh == type) cnt++;
			mh += 2;
		}
		return cnt;
	}

	int get_music(char *buf, unsigned char type, int num)
	{
		if (stbfilet == INVALID_FILE_HANDLE) return 2;
		unsigned char i;
		unsigned char *mh = musicheader;
		int cnt = 0;
		int pos = musicoffs;

		for (i = 0; i < nummusics; i++) {
			if (*mh == type) {
				if (cnt == num) {
					read(buf, pos, mh[1]);
					buf[mh[1]] = 0;
					return 0;
				}
				cnt++;
			}
			pos += (int)mh[1];
			mh += 2;
		}
		buf[0] = 0;
		return 1;

	}

#ifndef EMBEDDED
	void get_sound_name(char *buf, int namenum)
	{
		int i, npos = namesoffs;
		unsigned char sz;

		for (i = 0; i < namenum; i++) {
			read(&sz, npos, 1);
			npos += (int)sz + 1;
		}
		read(&sz, npos, 1);
		if (sz)
			read(buf, npos + 1, sz);
		buf[sz] = 0;
	}

	void get_music_name(char *buf, int namenum)
	{
		get_sound_name(buf, namenum + numsounds);
	}

	int get_num_sounds()
	{
		return (int)numsounds;
	}

	int get_num_musics()
	{
		return (int)nummusics;
	}

	// get sound no# num
	void get_sound_bynum(int num, unsigned char *id, int *pos, int *size)
	{
		unsigned char i;
		unsigned char *sh = soundheader;
		*pos = soundoffs;

		for (i = 0; i <= num; i++) {
			*id = *sh;
			*size = read3Blen(sh + 1);
			if (i != num) {
				*pos += *size;
				sh += 4;
			}
		}
	}

	// get music no# num
	void get_music_bynum(int num, unsigned char *type, char *buf)
	{
		unsigned char i;
		unsigned char *mh = musicheader + 1;
		int pos = musicoffs;

		for (i = 0; i < num; i++) {
			pos += (int)*mh;
			mh += 2;
		}
		read(buf, pos, *mh);
		buf[*mh] = 0;
		*type = *(mh - 1);
	}

#endif
};
