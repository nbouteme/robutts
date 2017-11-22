#ifndef AUDIO_H
#define AUDIO_H

/*
  Forward déclarations pour pointeurs opaques
 */
typedef struct s_audio_ctx audio_ctx;
typedef struct s_wave_ctx wave_ctx;

audio_ctx *make_audio_player();
wave_ctx *load_wav(audio_ctx *ap, const char *fn);
wave_ctx *load_wav_data(audio_ctx *ap, unsigned char *fn, unsigned size);
void play_wav_async(audio_ctx *ap, wave_ctx *wav);
void destroy_audio_sample(wave_ctx *wav);
void destroy_audio_player(audio_ctx *ctx);

#endif /* AUDIO_H */