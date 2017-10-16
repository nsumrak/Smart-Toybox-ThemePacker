#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>


// usage
// STBwriter::Open()
// foreach(sound) STBwriter::WriteSound()
// foreach(music) STBwriter::WriteMusic()
// STBwriter::Close()


class STBwriter
{
protected:
	unsigned char *header;
	FILE *filet;
	FILE *names;
	int num_sounds, num_music;
	int cur_sound, cur_music;
	int hsize;
	int musicsize, soundsize;

	void addName(const char *name)
	{
		char nlen = (int)strlen(name);
		fwrite(&nlen, 1, 1, names);
		if(nlen)
			fwrite(name, 1, nlen, names);
	}

	void make3Blen(unsigned char *buf, unsigned size)
	{
		*buf++ = (unsigned char)(size >> 16);
		*buf++ = (unsigned char)(size >> 8);
		*buf = (unsigned char)size;
	}

public:
	STBwriter()
	{
		filet = names = 0;
		header = 0;
		cur_music = cur_sound = num_sounds = num_music = 0;
		soundsize = musicsize = 0;
	}

	~STBwriter()
	{
		if (names) fclose(names);
		if (filet) fclose(filet);
		if (header) free(header);
	}
	
	bool Open(const char *filename, int numsounds, int nummusic)
	{
		hsize = numsounds * 4 + nummusic * 2 + 12;
		header = (unsigned char *)malloc(hsize);
		filet = fopen(filename, "wb");
		names = tmpfile();
		if (!header || !names || !filet) return false;
		num_sounds = numsounds;
		num_music = nummusic;

		header[0] = 0xba;
		header[1] = 0xba;
		header[2] = 0xde;
		header[3] = 0xda;
		header[4] = (unsigned char)numsounds;
		header[8] = (unsigned char)nummusic;

		fwrite(header, 1, hsize, filet);
		return true;
	}

	void Close()
	{
		assert(cur_sound == num_sounds);
		assert(cur_music == num_music);

		int namesize = ftell(names);
		make3Blen(header + 5, soundsize);
		make3Blen(header + 9, musicsize);

		rewind(names);
		char buf[1024];
		for (int readed = 0; readed < namesize; ) {
			int res = fread(buf, 1, sizeof(buf), names);
			fwrite(buf, 1, res, filet);
			readed += res;
		}

		rewind(filet);
		fwrite(header, 1, hsize, filet);
	}

	void WriteSound(const char *name, char ID, void *wv, int wvsize)
	{
		assert(cur_sound < num_sounds);
		assert(!cur_music);

		addName(name);
		unsigned char *chdr = &header[12 + cur_sound * 4];
		*chdr++ = ID;
		make3Blen(chdr, wvsize);
		
		fwrite(wv, 1, wvsize, filet);
		soundsize += wvsize;
		cur_sound++;
	}

	void WriteMusic(const char *name, char type, const char *playlist)
	{
		assert(cur_sound == num_sounds);
		assert(cur_music < num_music);

		addName(name);
		unsigned char *chdr = &header[12 + num_sounds * 4 + cur_music * 2];
		*chdr++ = type;
		*chdr = strlen(playlist);

		fwrite(playlist, 1, *chdr, filet);
		musicsize += *chdr;
		cur_music++;
	}
};
