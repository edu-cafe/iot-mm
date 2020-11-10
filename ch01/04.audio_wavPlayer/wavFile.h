#ifndef __WAVHEADER_H
#define __WAVHEADER_H

typedef struct wavFile_t {
	unsigned char	riffID[4];	//"RIFF" : riffID[]="RIFF"
	unsigned long	riffLen;	//Following Data Length
	unsigned char	wavID[4];	//"WAVE" : wavID[]="WAVE"
	unsigned char	fmtID[4];	//"fmt" : fmtID[]="fmt "
	unsigned long	fmtLen;		//normally 16
	unsigned short	fmtTag;		//normally 1(PCM)
	unsigned short	nChannels;	//channel number
	unsigned long	sampleRate;	//Sampling Rate
	unsigned long	avgBytesPerSec;	//Bytes/Sec
	unsigned short	nblockAlign;	//Bytes/Sample, normally 1
	unsigned short	bitsPerSample;	//Bits/Sample
	unsigned char	dataID[4];	//"data" : dataID[]="data"
	unsigned long	dataLen;	//Data Length
} WAVHEADER;

#endif
