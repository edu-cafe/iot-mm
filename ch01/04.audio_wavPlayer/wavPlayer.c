//ALSA Tutorial site :
//http://equalarea.com/paul/alsa-audio.html
//http://www.alsa-project.org/alsa-doc/alsa-lib/index.html
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <alsa/asoundlib.h>

#include "wavFile.h"

WAVHEADER wavheader;

int main(int argc, char *argv[])
{
	int fd = -1;
	int ret, buf_size, dir;
	int channels, format;
	long loops, count;
	unsigned int val;
	char * buffer;

	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	snd_pcm_uframes_t frames;

	
	if(argc < 2) {
		printf("Usage : %s filename\n", argv[0]);
		exit(-1);
	} else printf("Playing file : %s\n", argv[1]);

	if((fd = open(argv[1], O_RDONLY)) == -1) {
		fprintf(stderr, "Open Error(%s)!!\n", argv[1]);
		exit(-2);
	}

	if((count = read(fd, &wavheader, sizeof(WAVHEADER))) < 1) {
		fprintf(stderr, "Wav File Read Error!!\n");
		exit(-3);
	}

	ret = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if(ret < 0) {
		perror("Could not open output audio device");
		exit(-4);
	}
	printf("Identifier of PCM handle : %s\n", snd_pcm_name(handle));

	snd_pcm_hw_params_alloca(&params);
	snd_pcm_hw_params_any(handle, params);

	channels = wavheader.nChannels;
	printf("Wave Channel Mode : %s\n", channels ? "Stereo":"Mono");
	snd_pcm_hw_params_set_channels(handle, params, channels);

	printf("Wave Bytes : %d\n", wavheader.nblockAlign);
	switch(wavheader.nblockAlign) {
		case 1:
			format = SND_PCM_FORMAT_U8;
			break;
		case 2:
			format = (channels==1)?SND_PCM_FORMAT_S16_LE:SND_PCM_FORMAT_U8;
			break;
		case 4:
			format = SND_PCM_FORMAT_S16_LE;
			break;
		default:
			printf("Unknown byte rate for sound\n");
			break;
	}

	snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(handle, params, format);

	printf("Wave Sampling Rate : 0x%x(%d)\n", wavheader.sampleRate, wavheader.sampleRate);
	val = wavheader.sampleRate;
	if(!val) val = 44100;		//added by SOO
	snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

	snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

	ret = snd_pcm_hw_params(handle, params);
	if(ret < 0) {
		fprintf(stderr, "Parameter Set Error : %s\n", snd_strerror(ret));
		exit(-5);
	}

	snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	buf_size = frames * channels * ((format == SND_PCM_FORMAT_S16_LE) ? 2 : 1);
	printf("buf_size : %d\n", buf_size);
	buffer = (char*)malloc(buf_size);
	
	snd_pcm_hw_params_get_period_time(params, &val, &dir);
	do {
		if((count = read(fd, buffer, buf_size)) <= 0) break;
		ret = snd_pcm_writei(handle, buffer, frames);
		if(ret == -EPIPE) {
			fprintf(stderr, "Underrun Error\n");
			snd_pcm_prepare(handle);
		} else if(ret < 0) {
			fprintf(stderr, "Write Error : %s\n", snd_strerror(ret));
		} else if(ret != (int)frames) {
			fprintf(stderr, "Short Write, write %d frames\n", ret);
		}
	} while(count == buf_size);

end:
	close(fd);
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	free(buffer);
	return 0;
}


