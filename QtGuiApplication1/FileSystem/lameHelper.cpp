/**********************************************************************************************
	* Author:	Oso, Oluwafemi Ebenezer
	* Date:		25th of September, 2013
	* Filename:	lameHelper.cpp

	A wrapper for the LAME library
***********************************************************************************************/

#include "lameHelper.h"

settings_t::settings_t()
{
	//Setting the default values
	title = "";
	artist = "";
	album = "";
	comment = "";
	year = "";
	track = "";
	genre = "";
	albumart = NULL;
	
	channels = EC_STEREO;
	abr_bitrate = BR_128kbps;
	cbr_bitrate = BR_128kbps;
	quality = 5;
	enc_mode = EM_CBR;
	resample_frequency = SR_44khz; 
	in_samplerate = SR_44khz;
}


lameHelper::lameHelper()
{
	//Initialize to NULL, aids deletion/closing later
	for(int i = 0; i < MAX_THREAD_COUNT; i++)
	{
		hThread[i] = NULL;
		hSParam[i] = NULL;
	}
	
}

lameHelper::~lameHelper()
{
	//Destroy all declared objects
	for(int i = 0; i < MAX_THREAD_COUNT; i++)
	{
		if(hThread[i] != NULL)
			CloseHandle(hThread[i]);

		if(hSParam[i] != NULL)
			delete hSParam[i];
	}
}

int lameHelper::encode(char* pcm_in, char* mp3_out)
{
	settings_t t;//Pass the default value of settings
	return encode_x(pcm_in, mp3_out, t, NULL);
}

int lameHelper::encode(char* pcm_in, char* mp3_out, settings_t settings)
{
	return encode_x(pcm_in, mp3_out, settings, NULL);
}

int lameHelper::encode(char* pcm_in, char* mp3_out, settings_t settings, WNDPROC callback_proc)
{
	return encode_x(pcm_in, mp3_out, settings, callback_proc);
}

//The asynchronous version
void* lameHelper::encode(char* pcm_in, char* mp3_out, settings_t settings, WNDPROC callback_proc, bool async = false)
{
	if(async)
	{
		for(int i = 0; i < MAX_THREAD_COUNT; i++)//Trying to find a free thread to use
		{
			if((WaitForSingleObject(hThread[i], 0) == WAIT_FAILED && GetLastError() == ERROR_INVALID_HANDLE)/*Not used before*/ || WaitForSingleObject(hThread[i], 0) == WAIT_OBJECT_0/*Finished and ready for use again*/)
			{
				//Set the params to send to the static encode_s
				hSParam[i] = new StaticParam_t;
				StaticParam_t* hg = hSParam[i];
				hg->callback_proc = callback_proc;
				hg->lhObj = this;
				hg->mp3 = mp3_out;
				hg->pcm = pcm_in;
				hg->settings = settings;
				return hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)encode_s, (void*)hg, NULL, NULL);
			}
		}
	}
	else
	{
		return (void*)encode_x(pcm_in, mp3_out, settings, callback_proc);
	}
	return (void*)-3;//Thread as reached maximum allowed, wait for one to finish and try again
}

//The static member from which we can use for the thread
int lameHelper::encode_s(void* param)
{
	StaticParam_t* sp = (StaticParam_t*)param;
	char* pcm_in = sp->pcm;
	char* mp3_out = sp->mp3;
	settings_t settings = sp->settings;
	WNDPROC callback_proc = sp->callback_proc;
	lameHelper* lh = (lameHelper*)sp->lhObj;
    return lh->encode_x(pcm_in, mp3_out, settings, callback_proc);
}

int lameHelper::encode_x(char* pcm_in, char* mp3_out, settings_t settings, WNDPROC callback_proc)
{
		lame_global_flags*	gfp = NULL;
		gfp = lame_init();
		lame_set_in_samplerate(gfp, settings.in_samplerate);
	
		//Init the id3 tag structure
		id3tag_init(gfp);
		id3tag_v2_only(gfp);
		id3tag_set_year(gfp, settings.year);
		id3tag_set_genre(gfp, settings.genre);
		id3tag_set_artist(gfp, settings.artist);
		id3tag_set_album(gfp, settings.album);
		id3tag_set_title(gfp, settings.title);
		id3tag_set_track(gfp, settings.track);
		id3tag_set_comment(gfp, settings.comment);

		set_id3_albumart(gfp, settings.albumart);

		//Setting Channels
		switch(settings.channels)
		{
		case EC_MONO:
			lame_set_mode(gfp, MONO);
			lame_set_num_channels(gfp, 1);
			break;
		case EC_STEREO:
			lame_set_mode(gfp, JOINT_STEREO);
			lame_set_num_channels(gfp, 2);
			break;
		}

		//VbrTag
		lame_set_bWriteVbrTag(gfp, 1);	
		switch(settings.enc_mode)
		{
		case EM_ABR:
			{
				int i = settings.abr_bitrate;
				if(i >= 8000)
				{
				   i = (i + 500) / 1000;
				}
				if(i > 320)
				{
					i = 320;
				}
				if(i < 8)
				{
					i = 8;
				}
				lame_set_VBR(gfp, vbr_abr);
				lame_set_VBR_mean_bitrate_kbps(gfp, i);
			}
			break;
		case EM_CBR:
			lame_set_VBR(gfp, vbr_off);
			lame_set_quality(gfp, settings.quality);
			lame_set_brate(gfp, settings.cbr_bitrate);
			break;
		case EM_VBR:
			lame_set_VBR(gfp, vbr_mtrh);
			lame_set_VBR_q(gfp, settings.quality);
			break;
		}
	
		lame_set_out_samplerate(gfp, settings.resample_frequency);

		lame_set_findReplayGain(gfp, 1);
		lame_set_write_id3tag_automatic(gfp, 0); //Dont write id3tag, will write it myself

		int lResult = 0;
		if(lame_init_params(gfp) == -1)
		{
			//lame initialization failed
			if(callback_proc != NULL)
			{
				callback_proc((HWND)GetModuleHandle(NULL), LH_ERROR, -2, NULL);
			}
			sprintf(errMsg, "FATAL ERROR: parameters failed to initialize properly in lame. Aborting!\n");
			errorHandler(errMsg);
			return -2;
		}
		else	
		{
			int read = 0, 
				write = 0;
			long PCM_total_size = 0; 
			long cumulative_read = 0;

			FILE *pcm = fopen(pcm_in, "rb");
			FILE *mp3 = fopen(mp3_out, "wb");

			if(pcm == NULL)
			{
				if(callback_proc != NULL)
				{
					callback_proc((HWND)GetModuleHandle(NULL), LH_ERROR, -1, NULL);
				}
				sprintf(errMsg, "FATAL ERROR: file '%s' can't be open for read. Aborting!\n", pcm_in);
				errorHandler(errMsg);
				return -1;
			}
			fseek(pcm, 0, SEEK_END);
			PCM_total_size = ftell(pcm);
			fseek(pcm, 0, SEEK_SET);

			if(mp3 == NULL)
			{
				if(callback_proc != NULL)
				{
					callback_proc((HWND)GetModuleHandle(NULL), LH_ERROR, -1, NULL);
				}
				sprintf(errMsg, "FATAL ERROR: file '%s' can't be open for write. Aborting!\n", mp3_out);
				errorHandler(errMsg);
				return -1;
			}

			unsigned char* buffer = new unsigned char[LAME_MAXMP3BUFFER];

			short int pcm_buffer[PCM_SIZE*2];
			unsigned char mp3_buffer[MP3_SIZE];

			if(callback_proc != NULL)
			{
				callback_proc((HWND)GetModuleHandle(NULL), LH_STARTED, NULL, NULL);
			}

			int imp3 = lame_get_id3v2_tag(gfp, buffer, LAME_MAXMP3BUFFER);
			fwrite(buffer, sizeof(char), imp3, mp3);//write the id3v2 tag
			long audio_pos=ftell(mp3); //store beginning of audio data


			do
			{
				read = fread(pcm_buffer, 2 * sizeof(short int), PCM_SIZE, pcm);
				cumulative_read += (long)read * 2 * sizeof(short int);
				if(read == 0)
				{	
					write = lame_encode_flush(gfp, mp3_buffer, MP3_SIZE);
				}
				else
				{
					write = lame_encode_buffer_interleaved(gfp, pcm_buffer, read, mp3_buffer, MP3_SIZE);
				}
				fwrite(mp3_buffer, write, sizeof(char), mp3);
		
				//Percentage done
				if(callback_proc != NULL)
				{
					int percentage = ((float)cumulative_read/PCM_total_size)*100;
					callback_proc((HWND)GetModuleHandle(NULL), LH_COMPUTED, percentage, NULL);
				}
			}while(read != 0);

			imp3 = lame_get_id3v1_tag(gfp, buffer, LAME_MAXMP3BUFFER);  
			fwrite(buffer, sizeof(char), imp3, mp3);
	
			imp3 = lame_get_lametag_frame(gfp, buffer, LAME_MAXMP3BUFFER);
			if(fseek(mp3, audio_pos, SEEK_SET) != 0) 
			{
				errorHandler("WARNING: can't seek back to update LAME-tag frame!\n");
			}
			fwrite(buffer, sizeof(char), imp3, mp3);
			delete [] buffer;

			fclose(mp3);
			fclose(pcm);
	 
			if(callback_proc != NULL)
			{
				callback_proc((HWND)GetModuleHandle(NULL), LH_DONE, NULL, NULL);
			}
		}
	
		lame_close(gfp);
		return lResult;
}

int lameHelper::set_id3_albumart(lame_t gfp, char const* file_name)
{
    int iResult = -1;
    FILE *fpi = 0;
    char *albumart = 0;

    if(file_name == NULL) 
	{
        return 0;
    }

    fpi = fopen(file_name, "rb");
    
	if(!fpi) 
	{
        iResult = 1;
    }
    else
	{
        size_t size;

        fseek(fpi, 0, SEEK_END);
        size = ftell(fpi);
        fseek(fpi, 0, SEEK_SET);
        albumart = (char*)malloc(size);
        if(!albumart)
		{
            iResult = 2;            
        }
        else 
		{
            if(fread(albumart, 1, size, fpi) != size)
			{
                iResult = 3;
            }
            else 
			{
                iResult = id3tag_set_albumart(gfp, albumart, size) ? 4 : 0;
            }
            free(albumart);
        }
        fclose(fpi);
    }
    switch(iResult)
	{
	case 1:
		sprintf(errMsg, "WARNING: could not find file '%s' for albumart.\n", file_name);
		errorHandler(errMsg); 
		break;
    case 2:
		errorHandler("WARNING: insufficient memory for reading the albumart.\n");
		break;
    case 3:
		sprintf(errMsg, "WARNING: read error in '%s' for albumart.\n", file_name);
		errorHandler(errMsg);
		break;
    case 4: 
		sprintf(errMsg, "WARNING: unsupported image: '%s' for albumart. Specify JPEG/PNG/GIF image\n", file_name);
		errorHandler(errMsg);
		break;
    default: 
		break;
    }
    return iResult;
}

//*****************************DECODEING*************************************************************
//****************************************************************************************************
void lameHelper::write_16_bits_low_high(FILE * fp, int val)
{
    unsigned char bytes[2];
    bytes[0] = (val & 0xff);
    bytes[1] = ((val >> 8) & 0xff);
    fwrite(bytes, 1, 2, fp);
}

void lameHelper::write_32_bits_low_high(FILE * fp, int val)
{
    unsigned char bytes[4];
    bytes[0] = (val & 0xff);
    bytes[1] = ((val >> 8) & 0xff);
    bytes[2] = ((val >> 16) & 0xff);
    bytes[3] = ((val >> 24) & 0xff);
    fwrite(bytes, 1, 4, fp);
}

void lameHelper::WriteWaveHeader(FILE * const fp, int pcmbytes, int freq, int channels, int bits)
{
    int     bytes = (bits + 7) / 8;
    /* quick and dirty, but documented */
    fwrite("RIFF", 1, 4, fp); /* label */
    write_32_bits_low_high(fp, pcmbytes + 44 - 8); /* length in bytes without header */
    fwrite("WAVEfmt ", 2, 4, fp); /* 2 labels */
    write_32_bits_low_high(fp, 2 + 2 + 4 + 4 + 2 + 2); /* length of PCM format declaration area */
    write_16_bits_low_high(fp, 1); /* is PCM? */
    write_16_bits_low_high(fp, channels); /* number of channels */
    write_32_bits_low_high(fp, freq); /* sample frequency in [Hz] */
    write_32_bits_low_high(fp, freq * channels * bytes); /* bytes per second */
    write_16_bits_low_high(fp, channels * bytes); /* bytes per sample time */
    write_16_bits_low_high(fp, bits); /* bits per sample */
    fwrite("data", 1, 4, fp); /* label */
    write_32_bits_low_high(fp, pcmbytes); /* length in bytes of raw PCM data */
}

int lameHelper::decode(char* mp3_in, char* pcm_out)
{
	return decode(mp3_in, pcm_out, NULL);
}

void* lameHelper::decode(char* mp3_in, char* pcm_out, WNDPROC callback_proc, bool async)
{
	if(async)
	{
		for(int i = 0; i < MAX_THREAD_COUNT; i++)//Trying to find a free thread to use
		{
			if((WaitForSingleObject(hThread[i], 0) == WAIT_FAILED && GetLastError() == ERROR_INVALID_HANDLE)/*Not used before*/ || WaitForSingleObject(hThread[i], 0) == WAIT_OBJECT_0/*Finished and ready for use again*/)
			{
				//Set the params to send to the static encode_s
				hSParam[i] = new StaticParam_t;
				StaticParam_t* hg = hSParam[i];
				hg->callback_proc = callback_proc;
				hg->lhObj = this;
				hg->mp3 = mp3_in;
				hg->pcm = pcm_out;
				
				return hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)decode_s, (void*)hg, NULL, NULL);
			}
		}
	}
	else
	{
		return (void*)decode(mp3_in, pcm_out, callback_proc);
	}
	return (void*)-3;
}

//the static function used for the thread
int lameHelper::decode_s(void* param)
{
	StaticParam_t* sp = (StaticParam_t*)param;
	char* pcm_out = sp->pcm;
	char* mp3_in = sp->mp3;
	WNDPROC callback_proc = sp->callback_proc;

	lameHelper* lh = (lameHelper*)sp->lhObj;
	return lh->decode(mp3_in, pcm_out, callback_proc);
}


int lameHelper::decode(char* mp3_in, char* pcm_out, WNDPROC callback_proc)
{
	int read, i, samples;
	long wavsize = 0; // use to count the number of mp3 byte read, this is used to write the length of the wave file
	long cumulative_read = 0;

	short int pcm_l[PCM_SIZE], pcm_r[PCM_SIZE];
	unsigned char mp3_buffer[MP3_SIZE];

	FILE* mp3 = fopen(mp3_in, "rb");
	if(mp3 == NULL)
	{
		if(callback_proc != NULL)
		{
			callback_proc((HWND)GetModuleHandle(NULL), LH_ERROR, -1, NULL);
		}
		sprintf(errMsg, "FATAL ERROR: file '%s' can't be open for read. Aborting!\n", mp3_in);
		errorHandler(errMsg);
		return -1;
	}
	fseek(mp3, 0, SEEK_END);
    long MP3_total_size = ftell(mp3);
    fseek(mp3, 0, SEEK_SET);

	
	FILE* pcm = fopen(pcm_out, "wb");
	if(pcm == NULL)
	{
		if(callback_proc != NULL)
		{
			callback_proc((HWND)GetModuleHandle(NULL), LH_ERROR, -1, NULL);
		}
		sprintf(errMsg, "FATAL ERROR: file '%s' can't be open for write. Aborting!\n", pcm_out);
		errorHandler(errMsg);
		return -1;
	}

	
	lame_t lame = lame_init();
	lame_set_decode_only(lame, 1);
	if(lame_init_params(lame) == -1)
	{
		if(callback_proc != NULL)
		{
			callback_proc((HWND)GetModuleHandle(NULL), LH_ERROR, -2, NULL);
		}
		sprintf(errMsg, "FATAL ERROR: parameters failed to initialize properly in lame. Aborting!\n", pcm_out);
		errorHandler(errMsg);
		return -2;
	}

	hip_t hip = hip_decode_init();
	
	mp3data_struct mp3data;
	memset(&mp3data, 0, sizeof(mp3data));
	
	int nChannels = -1;
	int nSampleRate = -1;
	int mp3_len;

	if(callback_proc != NULL)
	{
		callback_proc((HWND)GetModuleHandle(NULL), LH_STARTED, NULL, NULL);
	}

	while((read = fread(mp3_buffer, sizeof(char), MP3_SIZE, mp3)) > 0)
	{
		mp3_len = read;
		cumulative_read += read * sizeof(char);
		do
		{
			samples = hip_decode1_headers(hip, mp3_buffer, mp3_len, pcm_l, pcm_r, &mp3data);
			wavsize += samples;

			if(mp3data.header_parsed == 1)//header is gotten
			{
				if(nChannels < 0)//reading for the first time
				{
					//Write the header
					WriteWaveHeader(pcm, 0x7FFFFFFF, mp3data.samplerate, mp3data.stereo, 16); //unknown size, so write maximum 32 bit signed value
				}
				nChannels = mp3data.stereo;
				nSampleRate = mp3data.samplerate;
			}

			if(samples > 0 && mp3data.header_parsed != 1)
			{
				errorHandler("WARNING: lame decode error occured!");
				break;
			}

			if(samples > 0)
			{
				for(i = 0 ; i < samples; i++)
				{
					fwrite((char*)&pcm_l[i], sizeof(char), sizeof(pcm_l[i]), pcm);
					if(nChannels == 2)
					{
						fwrite((char*)&pcm_r[i], sizeof(char), sizeof(pcm_r[i]), pcm);
					}
				}
			}
			mp3_len = 0;

			if(callback_proc != NULL)
			{
				int percentage = ((float)cumulative_read/MP3_total_size)*100;
				callback_proc((HWND)GetModuleHandle(NULL), LH_COMPUTED, percentage, NULL);
			}
		}while(samples>0);
	}

	i = (16 / 8) * mp3data.stereo;
    if (wavsize <= 0) 
	{
       wavsize = 0;
    }
    else if (wavsize > 0xFFFFFFD0 / i) 
	{
        wavsize = 0xFFFFFFD0;
    }
    else 
	{
        wavsize *= i;
    }
	
    if(!fseek(pcm, 0l, SEEK_SET))//seek back and adjust length
        WriteWaveHeader(pcm, (int) wavsize, mp3data.samplerate, mp3data.stereo, 16);
	else
		errorHandler("WARNING: can't seek back to adjust length in wave header!");

	hip_decode_exit(hip);
	lame_close(lame);
	fclose(mp3);
	fclose(pcm);

	if(callback_proc != NULL)
	{
		callback_proc((HWND)GetModuleHandle(NULL), LH_DONE, NULL, NULL);
	}
	return 0;
}

void lameHelper::errorHandler(char* msg)
{
	printf("%s\n", msg);
}