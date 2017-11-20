#include <robutts.h>

audio_ctx *make_audio_player() {
	return 0;
}

wave_ctx *load_wav(audio_ctx *ap, const char *fn) {
	(void)fn;
	(void)ap;
	return 0;
}

wave_ctx *load_wav_data(audio_ctx *ap, u8 *fn, u32 size) {
	(void)fn;
	(void)ap;
	(void)size;
	return 0;
}

void play_wav_async(audio_ctx *ap, wave_ctx *wav) {
	(void)ap;
	(void)wav;
}

void destroy_audio_sample(wave_ctx *wav) {
	(void)wav;
}

void destroy_audio_player(audio_ctx *ctx) {
	(void)ctx;
}
