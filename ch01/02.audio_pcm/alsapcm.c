//ALSA Tutorial site :
//http://equalarea.com/paul/alsa-audio.html
//http://www.alsa-project.org/alsa-doc/alsa-lib/index.html
//
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <sys/ioctl.h>
#include <alsa/asoundlib.h>

#define	BITS		2			//bit rate : 2ytes

#if 1
#define	FRAGMENT	8
#else
#define	FRAGMENT	2
#endif

#define	DURATION	5.0			//sec
#define	MODE		1			//채널수:mono
#define	FREQ		44100		//sampling rate
#define	BUFSIZE		(int)(BITS*FREQ*DURATION*MODE)

int setupDSP(snd_pcm_t *dev, int buf_size, int format, int sampleRate, int channels);

int main(int argc, char *argv[])
{
	snd_pcm_t *playback_handle;
	double total = DURATION, t;
	
#if 1
	int freq = 440;						//재생음의 주파수:440Hz
#else
	int freq = 8000;					//재생음의 주파수:8KHz
#endif

	int i, frames, count=1;
	char *snd_dev_out = "plughw:0,0";	//오디오 디바이스의 이름
	short buf[BUFSIZE];					//입출력 오디오를 위한 버퍼
	
	//오디오 디바이스 열기
	if(snd_pcm_open(&playback_handle, snd_dev_out, SND_PCM_STREAM_PLAYBACK, 0) < 0) {
		perror("Could not open output audio device");
		return -1;
	}
	printf("Identifier of PCM handle : %s\n", snd_pcm_name(playback_handle));

	//오디오 디바이스 설정
	if(setupDSP(playback_handle, BUFSIZE, SND_PCM_FORMAT_S16_LE, FREQ, MODE) < 0) {
		perror("Could not setup output audio device");
		return -2;
	}
	
	//정현파 데이터 생성
	printf("Make Sine Wave!!\n");
	for(i=0; i<BUFSIZE; i++) {
		t = (total/BUFSIZE) * i;
		buf[i] = SHRT_MAX * sin((int)(2.0 * M_PI * freq * t));
	}
	
	frames = BUFSIZE / (MODE * BITS);
	
	while(count--) {
		//오디오 출력을 위한 준비
		snd_pcm_prepare(playback_handle);
		
		//오디오 데이터를 디바이스로 출력(쓰기)
		snd_pcm_writei(playback_handle, buf, frames);	//i:interleaved, n:non-interleaved
	}
	
	//오디오 버퍼 버리기
	snd_pcm_drop(playback_handle);

	
	//오디오 디바이스 닫기
	snd_pcm_close(playback_handle);
	
	return 0;	
}

int setupDSP(snd_pcm_t *dev, int buf_size, int format, int sampleRate, int channels)
{
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_uframes_t frames;
	int fragments = FRAGMENT;
	int bits = (format == SND_PCM_FORMAT_S16_LE)?2:1;
	
	//오디오 디바이스의 매개변수 구조를 위한 메모리 할당
	if(snd_pcm_hw_params_malloc(&hw_params) < 0) {
		perror("Could not allocate parameters");
		return -1;
	}
	
	//오디오 디바이스의 매개변수들의 초기화
	if(snd_pcm_hw_params_any(dev, hw_params) < 0) {
		perror("Could not initialize parameters");
		return -2;
	}	
	
	//오디오 데이터 접근(access) 타입(인터리브, 넌인터리브)을 설정함
	if(snd_pcm_hw_params_set_access(dev, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
		perror("Could not set access type");
		return -3;
	}
	
	//샘플링 레이트를 설정함 : 부호있는 16비트 리틀엔디안
	if(snd_pcm_hw_params_set_format(dev, hw_params, format) < 0) {
		perror("Could not set sample format");
		return -4;
	}	
	
	//샘플의 포맷을 설정함 : 44.1KHz(CD 수준의 품질)
	if(snd_pcm_hw_params_set_rate_near(dev, hw_params, &sampleRate, 0) < 0) {
		perror("Could not set sample rate");
		return -5;
	}
	//snd_pcm_hw_params_get_rate()
	
	//채널 설정 : MONO(1)
	if(snd_pcm_hw_params_set_channels(dev, hw_params, channels) < 0) {
		perror("Could not set channel count");
		return -6;
	}
	
	//프레임 주기 설정
	if(snd_pcm_hw_params_set_periods_near(dev, hw_params, &fragments, 0) < 0) {
		perror("Could not set fragments");
		return -7;
	}
	//snd_pcm_hw_params_set_periods_xxx(), xxx:near,first,last,min,max,minmax,integer
	
	//버퍼의 크기 설정
	frames = (buf_size * fragments) / (channels * bits);
	if(snd_pcm_hw_params_set_buffer_size_near(dev, hw_params, &frames) < 0) {
		perror("Could not set buffer_size");
		return -8;
	}	
	
	//앞에서 설정한 오디오 디바이스의 매개변수를 ALSA 시스템에 적용
	if(snd_pcm_hw_params(dev, hw_params) < 0) {
		perror("Could not set HW params");
		return -9;
	}	
	
	return 0;
}





