#include <robutts.h>
#include <AL/al.h>
#include <AL/alut.h>

typedef struct s_audio_ctx {
	ALCcontext *context;
	ALCdevice *device;
} audio_ctx;

typedef struct s_wave_ctx {
	unsigned alSource;
	unsigned buffer;
} wave_ctx;

void checkalError() {
	ALenum e = alGetError();
	if (e) {
		puts(alGetString(e));
		abort();
	}
}

void checkalcError(void *dev) {
	ALenum e = alcGetError(dev);
	if (e) {
		puts(alcGetString(dev, e));
		abort();
	}
}

void checkalutError() {
	ALenum e = alutGetError();
	if (e != ALUT_ERROR_NO_ERROR) {
		puts(alutGetErrorString(e));
		abort();
	}
}

audio_ctx *make_audio_player() {
	audio_ctx *ctx;
	const char *defaultDeviceName;

	alutInitWithoutContext(0, 0);
	checkalutError();
	ctx = malloc(sizeof(*ctx));
	defaultDeviceName = alcGetString(0, ALC_DEFAULT_DEVICE_SPECIFIER);
	checkalcError(ctx->device);
	ctx->device = alcOpenDevice(defaultDeviceName);
	checkalcError(ctx->device);
 	ctx->context = alcCreateContext(ctx->device, 0);
	checkalcError(ctx->device);
 	alcMakeContextCurrent(ctx->context);
	checkalcError(ctx->device);
	return ctx;
}

wave_ctx *load_wav(audio_ctx *ap, const char *fn) {
	(void)ap;
	wave_ctx *wav = malloc(sizeof(*wav));
	wav->buffer = alutCreateBufferFromFile(fn);
	checkalutError();
 	alGenSources(1, &wav->alSource);
	checkalError();
	alSourcei(wav->alSource, AL_BUFFER, wav->buffer);
	checkalError();
	return wav;
}

wave_ctx *load_wav_data(audio_ctx *ap, u8 *fn, u32 size) {
	(void)ap;
	wave_ctx *wav = malloc(sizeof(*wav));
	wav->buffer = alutCreateBufferFromFileImage(fn, size);
	checkalutError();
 	alGenSources(1, &wav->alSource);
	checkalError();
	alSourcei(wav->alSource, AL_BUFFER, wav->buffer);
	checkalError();
	return wav;
}

void play_wav_async(audio_ctx *ap, wave_ctx *wav) {
	(void)ap;
	if(!ap || !wav)
		return;
	alSourcePlay(wav->alSource);
	checkalError();
}

void destroy_audio_sample(wave_ctx *wav) {
	alDeleteSources(1,&wav->alSource);
	checkalError();
	alDeleteBuffers(1,&wav->buffer);
	checkalError();
	free(wav);
}

void destroy_audio_player(audio_ctx *ctx) {
 	alcMakeContextCurrent(NULL);
	checkalcError(ctx->device);
	alcDestroyContext(ctx->context);
	checkalcError(ctx->device);
	alcCloseDevice(ctx->device);
	checkalcError(ctx->device);
	alutExit();
	free(ctx);
}
