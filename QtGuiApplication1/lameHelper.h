/**********************************************************************************************
	* Author:	Oso, Oluwafemi Ebenezer
	* Date:		25th of September, 2013
	* Filename:	lameHelper.h

	A wrapper for the LAME library
***********************************************************************************************/
#include <windows.h>
#include "lame.h"

#define LH_STARTED WM_USER+1
#define LH_COMPUTED WM_USER+2
#define LH_DONE WM_USER+3
#define LH_ERROR WM_USER+4

#define MAX_THREAD_COUNT 5

	enum encode_mode_e
	{
		EM_ABR,
		EM_CBR,
		EM_VBR
	};

	enum encode_channel_e
	{
		EC_MONO,
		EC_STEREO
	};

	enum bitrate_e
	{
		BR_8kbps = 8,
		BR_16kbps = 16,
		BR_24kbps = 24,
		BR_32kbps = 32,
		BR_40kbps = 40,
		BR_48kbps = 48,
		BR_56kbps = 56,
		BR_64kbps = 64,
		BR_80kbps = 80,
		BR_96kbps = 96,
		BR_112kbps = 112,
		BR_128kbps = 128,
		BR_144kbps = 144,
		BR_160kbps = 160,
		BR_192kbps = 192,
		BR_224kbps = 224,
		BR_256kbps = 256,
		BR_320kbps = 320
	};

	enum samplerate_e
	{
		SR_8khz = 8000,
		SR_11khz = 11025,
		SR_12khz = 12000,
		SR_16khz = 16000,
		SR_22khz = 22050,
		SR_24khz = 24000,
		SR_32khz = 32000,
		SR_44khz = 44100,
		SR_48khz = 48000
	};

	struct settings_t
	{
		char* title;
		char* artist;
		char* album;
		char* comment;
		char* year;
		char* track;
		char* genre;
		char* albumart;

		encode_channel_e channels;
		bitrate_e abr_bitrate;
		bitrate_e cbr_bitrate;
		int quality;
		encode_mode_e enc_mode;
		samplerate_e resample_frequency;
		samplerate_e in_samplerate;

		//The constructor; used to set default values
		settings_t();
	};

	class lameHelper; //lameHelper prototype, needed because of struct StaticParam_t

	//Use to hold parameters for the thread function
	struct StaticParam_t
	{
		char* pcm;
		char* mp3;
		settings_t settings;
		WNDPROC callback_proc;
		lameHelper* lhObj;
	};


	class lameHelper
	{
	private:
		static const int PCM_SIZE = 4096;
		static const int MP3_SIZE = 4096;

		HANDLE hThread[MAX_THREAD_COUNT];
		StaticParam_t* hSParam[MAX_THREAD_COUNT];

		int encode_x(char*, char*, settings_t, WNDPROC);

		//The static member from which we can start the thread
		static int encode_s(void* param);
		static int decode_s(void* param);

		void WriteWaveHeader(FILE * const, int, int, int, int);
		void write_32_bits_low_high(FILE*, int);
		void write_16_bits_low_high(FILE*, int);

		int set_id3_albumart(lame_t gfp, char const* file_name);
		void errorHandler(char*);
		char errMsg[1000];
	public:
		lameHelper();
		~lameHelper();

		//Encode a pcm file to mp3
		int encode(char* pcm_in, char* mp3_out);
		int encode(char* pcm_in, char* mp3_out, settings_t settings);
		int encode(char* pcm_in, char* mp3_out, settings_t settings, WNDPROC callback_proc);
		void* encode(char* pcm_in, char* mp3_out, settings_t settings, WNDPROC callback_proc, bool async);

		//Decode a mp3 to pcm
		int decode(char* mp3_in, char* pcm_out);
		int decode(char* mp3_in, char* pcm_out, WNDPROC callback_proc);
		void* decode(char* mp3_in, char* pcm_out, WNDPROC callback_proc, bool async);
	};