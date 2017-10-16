#include "wavpack.h"

static char *in_buff;
static int inbuff_readed, inbuff_size;

static int32_t read_stream_fn(void *buff, int32_t bcount)
{
	int toread = inbuff_size - inbuff_readed;
	if (toread > bcount) toread = bcount;
	memcpy(buff, in_buff+inbuff_readed, toread);
	inbuff_readed += toread;
	return toread;
}

int unpack_buffer(char *input_buffer, int *out_buff, int byte_size)
{
	WavpackContext wpc;
	int32_t samples_unpacked = 0, su;
	in_buff = input_buffer;
	inbuff_readed = 0;
	inbuff_size = byte_size;

	if (WavpackOpenFileInput(&wpc, read_stream_fn)) return 0;
	do {
		su = WavpackUnpackSamples(&wpc, out_buff + samples_unpacked, 1024);
		samples_unpacked += su;
	} while (su);

	uchar *output = (uchar *)out_buff;
	for (int scount = samples_unpacked; scount; scount--)
	{
		int32_t temp = *(out_buff++);
		*(output++) = (uchar)temp;
		*(output++) = (uchar)(temp >> 8);
	}
	return samples_unpacked;
}

int test_wv_file(char *buff, int size)
{
	WavpackContext wpc;

	in_buff = buff;
	inbuff_readed = 0;
	inbuff_size = size;

	if (WavpackOpenFileInput(&wpc, read_stream_fn)) return 0;
	if (WavpackGetBitsPerSample(&wpc) != 16 || WavpackGetNumChannels(&wpc) != 1 || WavpackGetNumSamples(&wpc) > 160000 || WavpackGetSampleRate(&wpc) != 16000)
		return 1;
	return 0;
}
