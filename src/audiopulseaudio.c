#define _GNU_SOURCE
#include <robutts.h>
#include <pulse/pulseaudio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

struct s_fmt {
	char Subchunk1ID[4];
	unsigned Subchunk1Size;
	unsigned short AudioFormat;
	unsigned short NumChannels;
	unsigned SampleRate;
	unsigned ByteRate;
	unsigned short BlockAlign;
	unsigned short BitsPerSample;
	unsigned short Extra[];
} __attribute__((packed));

struct s_data {
	char Subchunk2ID[4];
	unsigned Subchunk2Size;
	unsigned char data[];
} __attribute__((packed));

struct s_wav {
	char ChunkID[4];
	unsigned ChunkSize;
	char Format[4];
	struct s_fmt *fmt_chunk;
	struct s_data *data_chunk;
} __attribute__((packed));

typedef struct s_wav wav_t;

static void get_spec(wav_t *wav, pa_sample_spec *pss, pa_channel_map *map);

typedef struct s_audio_ctx {
	pa_threaded_mainloop *mainloop;
	pa_context *ctx;
	int ready;
} audio_ctx;

typedef struct s_wave_ctx {
	audio_ctx *ctx;
	pa_stream *sample;
	char *sample_name;
} wave_ctx;

typedef struct {
	wav_t *file;
	size_t offset;
} wav_write_wrap_t;

static wav_t *readwav(void *fdata) {
	wav_t *ret = malloc(sizeof(*ret));
	char *data = fdata;

	memcpy(ret, data, sizeof(*ret));
	ret->fmt_chunk = (void*)&((struct s_wav*)data)->fmt_chunk;
	ret->data_chunk = (void*)(data + sizeof(struct s_fmt) + 12);

	/* TODO: Add checks */
	/* On veut seulement little endian 16 bits et entre 1 et 2 channels */
	
	// data = (char*)ret.fmt_chunk - sizeof(struct s_fmt) - 12;
	return ret;
}

static void get_spec(wav_t *wav, pa_sample_spec *pss, pa_channel_map *map) {
	pss->format = PA_SAMPLE_S16LE;
	pss->rate = wav->fmt_chunk->SampleRate;
	pss->channels = wav->fmt_chunk->NumChannels;
	map->channels = wav->fmt_chunk->NumChannels;
	pa_channel_position_t channels[] = {PA_CHANNEL_POSITION_MONO,
										PA_CHANNEL_POSITION_FRONT_LEFT,
										PA_CHANNEL_POSITION_FRONT_RIGHT};
	memcpy(map->map, channels + map->channels - 1, sizeof(pa_channel_position_t) * map->channels);
}


static void stream_state_callback(pa_stream *s, void *userdata) {
	int *r = userdata;
    switch (pa_stream_get_state(s)) {
	case PA_STREAM_UNCONNECTED:
	case PA_STREAM_CREATING:
		break;
	case PA_STREAM_READY:
		*r = 1;
		break;
	case PA_STREAM_FAILED:
	case PA_STREAM_TERMINATED:
		break;
    }
}

static void context_state_callback(pa_context *ctx, void *userdata) {
	audio_ctx *c = userdata;
    switch (pa_context_get_state(ctx)) {
	case PA_CONTEXT_UNCONNECTED:
	case PA_CONTEXT_CONNECTING:
	case PA_CONTEXT_AUTHORIZING:
	case PA_CONTEXT_SETTING_NAME:
		break;
	case PA_CONTEXT_READY: {
		c->ready = 1;
		break;
	}
	case PA_CONTEXT_FAILED:
	case PA_CONTEXT_TERMINATED:
		break;
    }
}

audio_ctx *make_audio_player() {
	audio_ctx *ctx = malloc(sizeof(*ctx));

	ctx->ready = 0;
	ctx->mainloop = pa_threaded_mainloop_new();
	ctx->ctx = pa_context_new(pa_threaded_mainloop_get_api(ctx->mainloop), "Robutts");
	pa_context_set_state_callback(ctx->ctx, context_state_callback, ctx);
	pa_context_connect(ctx->ctx, 0, 0, 0);
	pa_threaded_mainloop_start(ctx->mainloop);
	while (!ctx->ready);
	return ctx;
}

static void write_data(pa_stream *stream, size_t n, void *userptr) {
	void *ssbuffer;
	size_t available;

	wav_write_wrap_t *data = userptr;

	size_t left = data->file->data_chunk->Subchunk2Size - data->offset;
	do {
		available = n;
		pa_stream_begin_write(stream, &ssbuffer, &available);
		if (left <= available)
			available = left;
		memcpy(ssbuffer, data->file->data_chunk->data + data->offset, available);
		pa_stream_write(stream, ssbuffer, available, 0, 0, PA_SEEK_RELATIVE);
		n -= available;
		left -= available;
		data->offset += available;
	} while (n);
	if (data->offset >= data->file->data_chunk->Subchunk2Size) {
		pa_stream_set_write_callback(stream, 0, 0);
		pa_stream_finish_upload(stream);
		free(data);
	}
}

wave_ctx *load_wav_data(audio_ctx *ap, u8 *data, u32 size) {
	(void)size;
	static int sample_number = 0;
	if (!ap)
		return 0;
	sample_number++;
	wave_ctx *wav = malloc(sizeof(*wav));
	wav->ctx = ap;
	asprintf(&wav->sample_name, "robutt_sample_%d", sample_number);
	wav_t *f = readwav(data);
	wav_write_wrap_t *wrap = calloc(1, sizeof(*wrap));
	wrap->file = f;
	pa_sample_spec pss;
	pa_channel_map map;
	get_spec(f, &pss, &map);

	int ready = 0;
	pa_threaded_mainloop_lock(ap->mainloop);
	{
		wav->sample = pa_stream_new(ap->ctx, wav->sample_name, &pss, &map);
		pa_stream_set_state_callback(wav->sample, stream_state_callback, &ready);
		pa_stream_set_write_callback(wav->sample, write_data, wrap);
		pa_stream_connect_upload(wav->sample, f->data_chunk->Subchunk2Size);
	}
	pa_threaded_mainloop_unlock(ap->mainloop);
	while(!ready);
	return wav;
}

wave_ctx *load_wav(audio_ctx *ap, const char *fn) {
	unsigned s = 0;
	void *d = readfile(fn, &s);
	return load_wav_data(ap, d, s);
}

void play_wav_async(audio_ctx *ap, wave_ctx *wav) {
	if (!wav || !ap)
		return;
	pa_threaded_mainloop_lock(ap->mainloop);
	{
		pa_operation_unref(pa_context_play_sample(ap->ctx, wav->sample_name, 0, PA_VOLUME_NORM, 0, 0));
	}
	pa_threaded_mainloop_unlock(ap->mainloop);
}

void destroy_audio_sample(wave_ctx *wav) {
	if (!wav)
		return;
	pa_threaded_mainloop_lock(wav->ctx->mainloop);
	{
		pa_context_remove_sample(wav->ctx->ctx, wav->sample_name, 0, 0);
		pa_stream_unref(wav->sample);
	}
	pa_threaded_mainloop_unlock(wav->ctx->mainloop);
	free(wav->sample_name);
	free(wav);
}

void destroy_audio_player(audio_ctx *ctx) {
	pa_context_disconnect(ctx->ctx);
	pa_context_unref(ctx->ctx);
	pa_threaded_mainloop_free(ctx->mainloop);
	free(ctx);
}
