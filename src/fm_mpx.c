/*
    PiFmExciter - Advanced FM exciter for the Raspberry Pi
    Copyright (C) 2018 LOUDRR
    Based on original works by Miegl (https://github.com/Miegl/PiFmAdv)

    See https://github.com/LoudRR/PiFmExciter
*/

#include <sndfile.h>
#include <stdlib.h>
#include <strings.h>

size_t length;

int channels;

double downsample_factor;

double *audio_buffer;
int audio_index = 0;
int audio_len = 0;
double audio_pos;

SNDFILE *inf;

double *alloc_empty_buffer(size_t length) {
    double *p = malloc(length * sizeof(double));
    if(p == NULL) return NULL;

    bzero(p, length * sizeof(double));

    return p;
}



int fm_mpx_open(char *filename, size_t len) {
	length = len;

	if(filename != NULL) {
		// Open the input file
		SF_INFO sfinfo;

		// stdin or file on the filesystem?
		if(filename[0] == '-') {
			if(!(inf = sf_open_fd(fileno(stdin), SFM_READ, &sfinfo, 0))) {
				fprintf(stderr, "Error: could not open stdin for audio input.\n");
				return -1;
			} else {
				printf("Using stdin for audio input.\n");
			}
		} else {
			if(!(inf = sf_open(filename, SFM_READ, &sfinfo))) {
				fprintf(stderr, "Error: could not open input file %s.\n", filename);
				return -1;
			} else {
				printf("Using audio file: %s\n", filename);
			}
		}

		int in_samplerate = sfinfo.samplerate;
		channels = sfinfo.channels;

		if (in_samplerate != 192000) {
			fprintf(stderr, "Error: sample rate must be 192000Hz.\n");
			return -1;
		}

		if (channels != 1) {
			fprintf(stderr, "Error: only mono files are supported.\n");
			return -1;
		}


		downsample_factor = 192000. / in_samplerate;

		printf("Input: %d Hz, upsampling factor: %.2f\n", in_samplerate, downsample_factor);


		audio_pos = downsample_factor;
		audio_buffer = alloc_empty_buffer(length * channels);
		if(audio_buffer == NULL) return -1;
	}
	else {
		inf = NULL;
	}

	return 0;
}


// samples provided by this function are in 0..10: they need to be divided by
// 10 after.
int fm_mpx_get_samples(double *mpx_buffer, float mpx, int wait) {

	for(int i=0; i<length; i++) {
		if(audio_pos >= downsample_factor) {
			audio_pos -= downsample_factor;

			if(audio_len <= channels) {
				for(int j=0; j<2; j++) { // one retry
					audio_len = sf_read_double(inf, audio_buffer, length);
					if (audio_len < 0) {
						fprintf(stderr, "Error reading audio\n");
						return -1;
					}
					if(audio_len == 0) {
						if( sf_seek(inf, 0, SEEK_SET) < 0 ) {
							if(wait) {
								return 0;
							} else {
								fprintf(stderr, "Could not rewind in audio file, terminating\n");
                                                        	return -1;
							}
						}
					} else {
						break;
					}
				}
				audio_index = 0;
			} else {
				audio_index += channels;
				audio_len -= channels;
			}
		}

		mpx_buffer[i] = audio_buffer[audio_index] * mpx;
		audio_pos++;
	}

	return 0;
}


int fm_mpx_close() {
	if(sf_close(inf) ) {
		fprintf(stderr, "Error closing audio file");
	}

	if(audio_buffer != NULL) free(audio_buffer);

	return 0;
}
