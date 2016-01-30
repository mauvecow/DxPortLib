/*
  DxPortLib - A portability library for DxLib-based software.
  Copyright (C) 2013-2015 Patrick McCarthy <mauve@sandwich.net>
  
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
  
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
    
  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required. 
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
 */

#include "DxBuildConfig.h"

#ifndef DX_NON_SOUND

/* for log10. */
#include <math.h>

#include "PLInternal.h"
#include "SDL2/PLSDL2Internal.h"

#ifndef DX_NON_OGGVORBIS
#include <vorbis/vorbisfile.h>
#endif /* #ifndef DX_NON_OGGVORBIS */

/* SDL_mixer is, unfortunately, not usable for this library,
 * so no easy shortcuts to be had there.
 * 
 * So we need three parts:
 * - The main mixer and playlist manager.
 * - AudioStream to manage streamed audio.
 * - AudioBuffer to manage whole sound buffers.
 * 
 * Unsurprisingly I'm using much of the same methodology as
 * SDL_mixer did, as its method of conversion just makes sense.
 */

/* This is the code I am least satisfied with in all of DxPortLib.
 * 
 * Namely, I do not like the list structure for buffers.
 * 
 * Other things to do include:
 *   We need to separate the actual buffering information from
 *   the sound instances, as well, to properly implement
 *   DuplicateSoundMem.
 */

typedef struct AudioStream {
    SDL_RWops *streamFile;
    
#ifndef DX_NON_OGGVORBIS
    OggVorbis_File ovfile;

    int hasLoopPoint;
    ogg_int64_t loopPoint;
    int hasLoopTarget;
    ogg_int64_t loopTarget;
#endif /* #ifndef DX_NON_OGGVORBIS */
    
    int active;
    
    SDL_AudioCVT convert;
    
    int section;
} AudioStream;

typedef struct AudioBuffer {
    unsigned char *buffer;

    unsigned int bufferSize;
    
    unsigned int length;
    
    unsigned int currentPos;
} AudioBuffer;

#define SOUNDTYPE_NONE 0
#define SOUNDTYPE_STREAM 1
#define SOUNDTYPE_BUFFER 2

typedef struct Sound {
    int soundType;
    
    int playing;
    
    int volume;
    
    int playMode;
    int nextSoundIDInSequence;
    
    AudioStream stream;
    AudioBuffer buffer;
    
    struct Sound *nextPlayingSound;
} Sound;

static int s_audioOpened = DXFALSE;
static int s_audioCannotOpen = DXFALSE;
static int s_audioOldVolumeCalcFlag = DXFALSE;
static int s_audioDataType = DX_SOUNDDATATYPE_MEMNOPRESS;

static SDL_AudioSpec s_audioSpec;

static int s_AudioStreamOpen(Sound *sound, SDL_RWops *rwops);
static void s_AudioStreamRestart(Sound *sound);
static void s_AudioStreamClose(Sound *sound);
static unsigned int s_AudioStreamPlay(Sound *sound, Uint8 *snd, unsigned int len);

static int s_AudioBufferOpen(Sound *sound, SDL_RWops *rwops);
static void s_AudioBufferRestart(Sound *sound);
static void s_AudioBufferClose(Sound *sound);
static unsigned int s_AudioBufferPlay(Sound *sound, Uint8 *snd, unsigned int len);

/* -------------------------------------------------- PLAYLIST MANAGEMENT */
static Sound *s_audioPlayingList = NULL;

static void s_StopSound(Sound *sound) {    
    SDL_LockAudio();
    
    if (sound->playing == DXTRUE) {
        Sound **listPtr = &s_audioPlayingList;
        
        while (*listPtr) {
            if ((*listPtr) == sound) {
                *listPtr = sound->nextPlayingSound;
                
                sound->nextPlayingSound = NULL;
                sound->playing = DXFALSE;
                break;
            }
            listPtr = &(*listPtr)->nextPlayingSound;
        }
    }
    
    SDL_UnlockAudio();
}

static void s_StartSound(Sound *sound, int playMode, int fromStartFlag) {
    SDL_LockAudio();
    
    if (fromStartFlag) {
        if (sound->soundType == SOUNDTYPE_STREAM) {
            s_AudioStreamRestart(sound);
        } else if (sound->soundType == SOUNDTYPE_BUFFER) {
            s_AudioBufferRestart(sound);
        }
    }
    
    sound->playMode = playMode;
    
    if (sound->playing == DXFALSE) {
        sound->nextPlayingSound = s_audioPlayingList;
        s_audioPlayingList = sound;
        
        sound->playing = DXTRUE;
    }
    
    SDL_UnlockAudio();
}

/* ------------------------------------------------------ AUDIO STREAMING */

#ifndef DX_NON_OGGVORBIS
static int s_AudioStreamGet(Sound *sound);

/* Handles ogg streams via vorbisfile. */
static size_t s_oggRead(void *ptr, size_t size, size_t num, void *datasource) {
    SDL_RWops *rwops = (SDL_RWops *)datasource;
    return SDL_RWread(rwops, ptr, size, num);
}
static int s_oggSeek(void *datasource, ogg_int64_t offset, int whence) {
    SDL_RWops *rwops = (SDL_RWops *)datasource;
    return (int)SDL_RWseek(rwops, offset, whence);
}
static long s_oggTell(void *datasource) {
    SDL_RWops *rwops = (SDL_RWops *)datasource;
    return (long)SDL_RWtell(rwops);
}

static int s_AudioStreamOpen(Sound *sound, SDL_RWops *rwops) {
    AudioStream *stream = &sound->stream;
    ov_callbacks oggCallbacks;
    
    stream->streamFile = rwops;
    stream->section = -1;
    stream->active = 1;
    stream->hasLoopTarget = DXFALSE;
    stream->loopTarget = 0;
    stream->hasLoopPoint = DXFALSE;
    stream->loopPoint = 0;
    
    SDL_memset(&oggCallbacks, 0, sizeof(oggCallbacks));
    /* This will give conversion warnings on most platforms.
     * That is completely okay! Ignore! */
    oggCallbacks.read_func = s_oggRead;
    oggCallbacks.seek_func = s_oggSeek;
    oggCallbacks.tell_func = s_oggTell;
    
    if (ov_open_callbacks(rwops, &stream->ovfile, NULL, 0, oggCallbacks) < 0) {
        return -1;
    }
    
    sound->soundType = SOUNDTYPE_STREAM;
    
    return 0;
}

static void s_AudioStreamRestart(Sound *sound) {
    AudioStream *stream = &sound->stream;
    AudioBuffer *buffer = &sound->buffer;
    
    ov_time_seek(&stream->ovfile, 0.0f);
    
    buffer->currentPos = 0;
    buffer->length = 0;
    stream->active = 1;
    
    /* buffer now instead of in the audio thread */
    s_AudioStreamGet(sound);
}

static void s_AudioStreamClose(Sound *sound) {
    AudioStream *stream = &sound->stream;
    AudioBuffer *buffer = &sound->buffer;
    
    if (buffer->buffer) {
        DXFREE(buffer->buffer);
        buffer->buffer = NULL;
    }
    
    ov_clear(&stream->ovfile);
    
    SDL_RWclose(stream->streamFile);
}

static int s_AudioStreamGet(Sound *sound) {
    AudioStream *stream = &sound->stream;
    AudioBuffer *buffer = &sound->buffer;
    char inbuf[4096];
    int amount = 0, amount2;
    int section;
    SDL_AudioCVT *convert;
    
    if (stream->hasLoopPoint != DXFALSE) {
        vorbis_info *info = ov_info(&stream->ovfile, -1);
        ogg_int64_t bufpos = ov_pcm_tell(&stream->ovfile);
        ogg_int64_t n = (stream->loopPoint - bufpos) * (2 * info->channels);
        if (n > 0 && n < sizeof(inbuf)) {
            amount = ov_read(&stream->ovfile, inbuf, (int)n, 0, 2, 1, &section);
            ov_pcm_seek(&stream->ovfile, stream->loopTarget);
            
            if (amount < 0) {
                amount = 0;
            }
        }
    }
    
    amount2 = ov_read(&stream->ovfile, inbuf + amount, sizeof(inbuf) - amount, 0, 2, 1, &section);
    if (amount2 <= 0) {
        if (stream->hasLoopTarget != DXFALSE) {
            if (ov_pcm_seek(&stream->ovfile, stream->loopTarget) == 0) {
                return 0;
            }
        }
        s_StopSound(sound);
        
        return -1;
    }
    amount += amount2;
    
    convert = &stream->convert;
    
    if (buffer->buffer == NULL || section != stream->section) {
        /* create conversion buffer */
        vorbis_info *info;
        unsigned int bufferSize;
        
        info = ov_info(&stream->ovfile, -1);
        SDL_BuildAudioCVT(convert, AUDIO_S16,
                          (Uint8)info->channels,
                          info->rate,
                          s_audioSpec.format,
                          s_audioSpec.channels,
                          s_audioSpec.freq);
        
        bufferSize = sizeof(inbuf) * (unsigned int)convert->len_mult;
        
        if (buffer->buffer != NULL && bufferSize > buffer->bufferSize) {
            DXFREE(buffer->buffer);
            buffer->buffer = NULL;
        }
        
        if (buffer->buffer == NULL) {
            buffer->buffer = DXALLOC(bufferSize);
            buffer->bufferSize = bufferSize;
        }
        
        convert->buf = buffer->buffer;
        
        stream->section = section;
    }
    
    if (buffer->buffer) {
        SDL_memcpy(buffer->buffer, inbuf, (unsigned int) amount);
        
        if (convert->needed) {
            convert->len = amount;
            SDL_ConvertAudio(convert);
        } else {
            convert->len_cvt = amount;
        }
        buffer->length = (unsigned int)convert->len_cvt;
        buffer->currentPos = 0;
    }
    
    return 0;
}

static unsigned int s_AudioStreamPlay(Sound *sound, Uint8 *snd, unsigned int len) {
    AudioBuffer *buffer = &sound->buffer;
    
    while (len > 0 && sound->playing) {
        unsigned int amount;
        
        if (buffer->currentPos >= buffer->length) {
            if (s_AudioStreamGet(sound) < 0) {
                break;
            }
        }
        
        amount = buffer->length - buffer->currentPos;
        if (amount > len) {
            amount = len;
        }
        if (amount > 0) {
            SDL_MixAudio(snd, buffer->buffer + buffer->currentPos, amount, sound->volume);
            snd += amount;
            len -= amount;
            buffer->currentPos += amount;
        }
    }
    
    return len;
}

#else /* #ifndef DX_NON_OGGVORBIS */

static int s_AudioStreamOpen(Sound *sound, SDL_RWops *rwops) {
    return -1;
}

static void s_AudioStreamRestart(Sound *sound) {
}

static void s_AudioStreamClose(Sound *sound) {
}

static unsigned int s_AudioStreamPlay(Sound *sound, Uint8 *snd, unsigned int len) {
    sound->playing = 0;
    return len;
}

#endif /* #ifndef DX_NON_OGGVORBIS */

/* ------------------------------------------------- STATIC AUDIO BUFFERS */

static int s_AudioBufferOpenRIFF(Sound *sound, SDL_RWops *rwops, Uint32 riffSize) {
    AudioBuffer *buffer = &sound->buffer;
    SDL_AudioCVT convert;
    int readFormat = 0;
    Uint32 totalSize = riffSize;
    
    /* Read chunks */
    while (riffSize > 8) {
        Uint32 ID = SDL_ReadLE32(rwops);
        Uint32 size = SDL_ReadLE32(rwops);
        riffSize -= 8;
        
        if (size > riffSize) {
            break;
        }
        riffSize -= size;
        
        if (ID == 0x20746D66 && size >= 0x10) { /* 'fmt ' */
            SDL_AudioFormat srcFormat;
            Uint16 encoding;
            Uint16 channels;
            Uint32 frequency;
            Uint16 bitspersample;
            encoding = SDL_ReadLE16(rwops);
            channels = SDL_ReadLE16(rwops);
            frequency= SDL_ReadLE32(rwops);
            /* Uint32 byterate = */ SDL_ReadLE32(rwops);
            /* Uint16 blockalign = */ SDL_ReadLE16(rwops);
            bitspersample = SDL_ReadLE16(rwops);
            
            size -= 0x10;
            if (size > 0) {
                SDL_RWseek(rwops, size, RW_SEEK_CUR);
            }
            
            if (encoding != 1) {
                /* We don't support other encodings. */
                break;
            }
            
            if (bitspersample == 8) {
                srcFormat = AUDIO_U8;
            } else if (bitspersample == 16) {
                srcFormat = AUDIO_S16;
            } else {
                break;
            }
            
            SDL_BuildAudioCVT(&convert,
                              srcFormat,
                              (Uint8)channels,
                              (int)frequency,
                              s_audioSpec.format,
                              (Uint8)s_audioSpec.channels,
                              s_audioSpec.freq);
            
            buffer->bufferSize = totalSize * (unsigned int)convert.len_mult;
            buffer->buffer = DXALLOC(buffer->bufferSize);
            buffer->length = 0;
            
            convert.buf = buffer->buffer;
            
            readFormat = 1;
        } else if (ID == 0x61746164) {/* 'data' */
            if (!readFormat) {
                break;
            }
            
            SDL_RWread(rwops, buffer->buffer + buffer->length, size, 1);
            buffer->length += size;
        } else {
            SDL_RWseek(rwops, size, RW_SEEK_CUR);
        }
    }
    
    /* Finish up */
    if (buffer->length == 0 || !readFormat) {
        if (buffer->buffer != NULL) {
            DXFREE(buffer->buffer);
            buffer->buffer = NULL;
        }
        return -1;
    }
    
    if (convert.needed) {
        convert.len = (int)(buffer->length);
        SDL_ConvertAudio(&convert);
        buffer->length = (unsigned int)(convert.len_cvt);
    }
    
    sound->soundType = SOUNDTYPE_BUFFER;
    
    SDL_RWclose(rwops);
    
    return 0;
}

static int s_AudioBufferOpen(Sound *sound, SDL_RWops *rwops) {
    Uint32 ID;
    Uint32 size;
    
    if (SDL_RWsize(rwops) < 32) {
        return -1;
    }
    
    ID = SDL_ReadLE32(rwops);
    size = SDL_ReadLE32(rwops);
    
    if (ID == 0x46464952) { /* 'RIFF' */
        ID = SDL_ReadLE32(rwops);
        if (ID == 0x45564157) { /* 'WAVE' */
            return s_AudioBufferOpenRIFF(sound, rwops, size - 4);
        }
    }
    
    return -1;
}

static void s_AudioBufferRestart(Sound *sound) {
    AudioBuffer *buffer = &sound->buffer;
    
    buffer->currentPos = 0;
}

static void s_AudioBufferClose(Sound *sound) {
    AudioBuffer *buffer = &sound->buffer;
    
    DXFREE(buffer->buffer);
    buffer->buffer = NULL;
}

static unsigned int s_AudioBufferPlay(Sound *sound, Uint8 *snd, unsigned int len) {
    AudioBuffer *buffer = &sound->buffer;
    unsigned int amount = buffer->length - buffer->currentPos;
    if (amount > len) {
        amount = len;
    }
    
    if (amount > 0) {
        SDL_MixAudio(snd, buffer->buffer + buffer->currentPos, amount, sound->volume);
        buffer->currentPos += amount;
        
        len -= amount;
    }
    
    if (buffer->currentPos >= buffer->length) {
        s_StopSound(sound);
    }
    
    return len;
}

/* ----------------------------------------------------- MAIN AUDIO MIXER */

static void s_Mixer(void *udata, Uint8 *stream, int len) {
    Sound *sound;
    unsigned int ulen = (unsigned int)len;

    if (len <= 0) {
        return;
    }
    
    SDL_memset(stream, 0, (size_t)len);
    
    /* Iterate through all playing sounds. */
    sound = s_audioPlayingList;
    while (sound != NULL) {
        Sound *nextSound = sound->nextPlayingSound;
        unsigned int left = ulen;
        
        while (left > 0 && sound->playing) {
            if (sound->soundType == SOUNDTYPE_STREAM) {
                left = s_AudioStreamPlay(sound, stream + (ulen - left), left);
            } else if (sound->soundType == SOUNDTYPE_BUFFER) {
                left = s_AudioBufferPlay(sound, stream + (ulen - left), left);
            } else {
                break;
            }
            
            if (!sound->playing) {
                /* test for loops, sequences, etc here */
                int playMode = sound->playMode;
                
                if (sound->nextSoundIDInSequence >= 0) {
                    sound = (Sound *)PL_Handle_GetData(sound->nextSoundIDInSequence, DXHANDLE_SOUND);
                    if (sound == NULL) {
                        break;
                    }
                    
                    s_StartSound(sound, playMode, DXTRUE);
                } else if ((playMode & DX_PLAYTYPE_LOOP) == DX_PLAYTYPE_LOOP) {
                    s_StartSound(sound, DX_PLAYTYPE_LOOP, DXTRUE);
                }
            }
        }
        
        sound = nextSound;
    }
}

static int s_AllocateSound() {
    int soundID = PL_Handle_AcquireID(DXHANDLE_SOUND);
    Sound *sound;
    
    if (soundID < 0) {
        return -1;
    }
    
    sound = (Sound *)PL_Handle_AllocateData(soundID, sizeof(Sound));
    
    SDL_memset(sound, 0, sizeof(Sound));
    
    sound->volume = SDL_MIX_MAXVOLUME;
    sound->soundType = SOUNDTYPE_NONE;
    sound->nextSoundIDInSequence = -1;
    
    return soundID;
}

static int s_FreeSound(int soundID) {
    Sound *sound;
    
    SDL_LockAudio();
    sound = (Sound *)PL_Handle_GetData(soundID, DXHANDLE_SOUND);
    if (sound != NULL) {
        if (sound->playing) {
            s_StopSound(sound);
        }
        
        if (sound->soundType == SOUNDTYPE_BUFFER) {
            s_AudioBufferClose(sound);
        } else if (sound->soundType == SOUNDTYPE_STREAM) {
            s_AudioStreamClose(sound);
        }
        
        if (sound->buffer.buffer != NULL) {
            DXFREE(sound->buffer.buffer);
        }
    
        PL_Handle_ReleaseID(soundID, DXTRUE);
    }
    SDL_UnlockAudio();
    
    return 0;
}

static void s_AudioOpen() {
    SDL_AudioSpec audioSpec;
    
    if (s_audioOpened == DXTRUE) {
        return;
    }
    
    if (s_audioCannotOpen == DXTRUE) {
        return;
    }
    
    SDL_memset(&audioSpec, 0, sizeof(audioSpec));
    audioSpec.freq = 44100;
    audioSpec.format = AUDIO_S16SYS;
    audioSpec.channels = 2;
    audioSpec.samples = 1024; /* latency... */
    audioSpec.callback = s_Mixer;
    audioSpec.userdata = NULL;
    
    if (SDL_OpenAudio(&audioSpec, &s_audioSpec) < 0) {
        s_audioCannotOpen = DXTRUE;
        return;
    }
    
    SDL_PauseAudio(0);
    
    s_audioOpened = DXTRUE;
}

static void s_AudioClose() {
    if (s_audioOpened != DXTRUE) {
        return;
    }
    
    SDL_PauseAudio(1);
    
    SDL_LockAudio();
    
    /* - Stop all playing sounds */
    while (s_audioPlayingList != NULL) {
        s_StopSound(s_audioPlayingList);
    }
    
    /* - Free all sounds */
    PL_InitSoundMem();
    
    SDL_UnlockAudio();
    
    /* - Close up and finish. */
    SDL_CloseAudio();
    
    s_audioOpened = DXFALSE;
}

/* ------------------------------------------------------ DXLIB INTERFACE */

static int s_LoadSound(const char *filename) {
    SDL_RWops *rwops;
    char buf[4];
    int soundID;
    Sound *sound;
    
    s_AudioOpen();
    
    if (s_audioOpened == DXFALSE) {
        return -1;
    }
    
    rwops = PLSDL2_FileToRWops(PL_File_OpenRead(filename));
    if (rwops == NULL) {
        return -1;
    }
    
    if (SDL_RWread(rwops, buf, 4, 1) >= 0) {
        SDL_RWseek(rwops, 0, RW_SEEK_SET);
        
        do {
            soundID = s_AllocateSound();
            if (soundID < 0) {
                break;
            }
            
            sound = (Sound *)PL_Handle_GetData(soundID, DXHANDLE_SOUND);
            
            if (SDL_memcmp(buf, "RIFF", 4) == 0) {
                /* buffer */
                if (s_AudioBufferOpen(sound, rwops) < 0) {
                    break;
                }
            } else if (SDL_memcmp(buf, "OggS", 4) == 0) {
                /* stream */
                if (s_AudioStreamOpen(sound, rwops) < 0) {
                    break;
                }
            } else {
                break;
            }
            
            return soundID;
        } while (0);
    }
    
    SDL_RWclose(rwops);
    if (soundID >= 0) {
        s_FreeSound(soundID);
    }
    return -1;
}

static void s_RestartSound(Sound *sound) {
    if (sound->soundType == SOUNDTYPE_STREAM) {
        s_AudioStreamRestart(sound);
    } else if (sound->soundType == SOUNDTYPE_BUFFER) {
        s_AudioBufferRestart(sound);
    }
}

static int s_WaitForSound(int soundID) {
    int playing = 1;
    while (playing > 0) {
        Sound *sound;
        
        SDL_LockAudio();
        sound = (Sound *)PL_Handle_GetData(soundID, DXHANDLE_SOUND);
        if (sound == NULL) {
            playing = -1;
        } else {
            playing = sound->playing;
            
            if (playing == 0 && sound->nextSoundIDInSequence >= 0) {
                soundID = sound->nextSoundIDInSequence;
                playing = 1;
            }
        }
        SDL_UnlockAudio();
        
        if (PL_Window_ProcessMessages() < 0) {
            return -1;
        }
        
        PL_Platform_Wait(1);
    }
    
    return playing;
}

int PL_LoadSoundMem(const char *filename) {
    return s_LoadSound(filename);
}

int PL_LoadSoundMem2(const char *filenameA, const char *filenameB) {
    int soundIDA = s_LoadSound(filenameA);
    
    if (soundIDA >= 0) {
        int soundIDB = s_LoadSound(filenameB);
        if (soundIDB >= 0) {
            Sound *sound = (Sound *)PL_Handle_GetData(soundIDA, DXHANDLE_SOUND);
            sound->nextSoundIDInSequence = soundIDB;
        }
    }
    
    return soundIDA;
}

int PL_DeleteSoundMem(int soundID) {
    Sound *sound;
    
    SDL_LockAudio();
    sound = (Sound *)PL_Handle_GetData(soundID, DXHANDLE_SOUND);
    if (sound != NULL) {
        int nextID = sound->nextSoundIDInSequence;
        
        s_FreeSound(soundID);
        
        if (nextID >= 0) {
            PL_DeleteSoundMem(nextID);
        }
    }
    SDL_UnlockAudio();
    return 0; 
}

int PL_PlaySoundMem(int soundID, int playType, int startPositionFlag) {
    Sound *sound;
    int retval = -1;
    
    SDL_LockAudio();
    
    sound = (Sound *)PL_Handle_GetData(soundID, DXHANDLE_SOUND);
    if (sound != NULL) {
        retval = 0;
        
        s_StartSound(sound, playType, startPositionFlag);
        
        if (sound->nextSoundIDInSequence) {
            Sound *nextSound = (Sound *)PL_Handle_GetData(sound->nextSoundIDInSequence, DXHANDLE_SOUND);
            if (nextSound != NULL) {
                s_RestartSound(nextSound);
            }
        }
    }
    
    SDL_UnlockAudio();
    
    if (playType == DX_PLAYTYPE_NORMAL) {
        s_WaitForSound(soundID);
    }
    
    return retval;
}

int PL_StopSoundMem(int soundID) {
    Sound *sound;
    int retval = -1;
    
    SDL_LockAudio();
    
    sound = (Sound *)PL_Handle_GetData(soundID, DXHANDLE_SOUND);
    if (sound != NULL) {
        retval = 0;
        if (sound->playing) {
            s_StopSound(sound);
        }
        if (sound->nextSoundIDInSequence) {
            PL_StopSoundMem(sound->nextSoundIDInSequence);
        }
    }
   
    SDL_UnlockAudio();
    
    return retval;
}

int PL_CheckSoundMem(int soundID) {
    Sound *sound;
    int retval = -1;
    
    SDL_LockAudio();
    
    sound = (Sound *)PL_Handle_GetData(soundID, DXHANDLE_SOUND);
    if (sound != NULL) {
        retval = sound->playing;
    }
   
    SDL_UnlockAudio();
    
    return retval;
}

int PL_Audio_SetLoopSamples(int soundID,
                            uint64_t loopTarget,
                            uint64_t loopPoint) {
    Sound *sound;
    int retval = -1;
    
    SDL_LockAudio();
    
    sound = (Sound *)PL_Handle_GetData(soundID, DXHANDLE_SOUND);
    if (sound != NULL) {
        sound->stream.hasLoopTarget = DXTRUE;
        sound->stream.loopTarget = loopTarget;
        if (loopPoint > 0.0) {
            sound->stream.hasLoopPoint = DXTRUE;
            sound->stream.loopPoint = loopPoint;
        } else {
            sound->stream.hasLoopPoint = DXFALSE;
        }
        retval = 0;
    }
   
    SDL_UnlockAudio();
    
    return retval;
}
int PL_Audio_SetLoopTimes(int soundID,
                          double loopTarget,
                          double loopPoint) {
    Sound *sound;
    int retval = -1;
    
    SDL_LockAudio();
    
    sound = (Sound *)PL_Handle_GetData(soundID, DXHANDLE_SOUND);
    if (sound != NULL) {
#ifndef DX_NON_OGGVORBIS
        vorbis_info *info = ov_info(&sound->stream.ovfile, -1);
        
        sound->stream.hasLoopTarget = DXTRUE;
        sound->stream.loopTarget = (ogg_int64_t)(loopTarget * info->rate);
        if (loopPoint > 0.0) {
            sound->stream.hasLoopPoint = DXTRUE;
            sound->stream.loopPoint = (ogg_int64_t)(loopPoint * info->rate);
            if (sound->stream.loopPoint > ov_pcm_total(&sound->stream.ovfile, -1)) {
                sound->stream.loopPoint = ov_pcm_total(&sound->stream.ovfile, -1);
            }
        } else {
            sound->stream.hasLoopPoint = DXFALSE;
        }
#endif
        retval = 0;
    }
   
    SDL_UnlockAudio();
    
    return retval;
}

int PL_Audio_RemoveLoopPoints(int soundID) {
    Sound *sound;
    int retval = -1;
    
    SDL_LockAudio();
    
    sound = (Sound *)PL_Handle_GetData(soundID, DXHANDLE_SOUND);
    if (sound != NULL) {
        sound->stream.hasLoopPoint = DXFALSE;
        sound->stream.hasLoopTarget = DXFALSE;
        retval = 0;
    }
   
    SDL_UnlockAudio();
    
    return retval;
}

int PL_SetVolumeSoundMemDirect(int volume, int soundID) {
    Sound *sound;
    int retval = -1;
    
    SDL_LockAudio();
    
    sound = (Sound *)PL_Handle_GetData(soundID, DXHANDLE_SOUND);
    if (sound != NULL) {
        int adjustedVol;
        retval = 0;
        
        if (volume < 0) {
            volume = 0;
        } else if (volume > 255) {
            volume = 255;
        }
        
        adjustedVol = (int)(volume * SDL_MIX_MAXVOLUME / 255);
        
        if (adjustedVol < 0) {
            adjustedVol = 0;
        } else if (adjustedVol > SDL_MIX_MAXVOLUME) {
            adjustedVol = SDL_MIX_MAXVOLUME;
        }
        
        sound->volume = adjustedVol;
        
        if (sound->nextSoundIDInSequence) {
            PL_SetVolumeSoundMem(volume, sound->nextSoundIDInSequence);
        }
    }
    
    SDL_UnlockAudio();
    
    return retval;
}

int PL_SetVolumeSoundMem(int volume, int soundID) {
    Sound *sound;
    int retval = -1;
    
    SDL_LockAudio();
    
    sound = (Sound *)PL_Handle_GetData(soundID, DXHANDLE_SOUND);
    if (sound != NULL) {
        int adjustedVol;
        retval = 0;
        
        if (volume < -10000) {
            volume = -10000;
        } else if (volume > 0) {
            volume = 0;
        }
        
        adjustedVol = (int)(SDL_pow(2, volume / 600.0) * SDL_MIX_MAXVOLUME);
        
        if (adjustedVol < 0) {
            adjustedVol = 0;
        } else if (adjustedVol > SDL_MIX_MAXVOLUME) {
            adjustedVol = SDL_MIX_MAXVOLUME;
        }
        
        sound->volume = adjustedVol;
        
        if (sound->nextSoundIDInSequence) {
            PL_SetVolumeSoundMem(volume, sound->nextSoundIDInSequence);
        }
    }
    
    SDL_UnlockAudio();
    
    return retval;
}

int PL_ChangeVolumeSoundMem(int volume, int soundID) {
    double mul;
    
    if (volume > 255) {
        volume = 255;
    }
    if (volume < 0) {
        volume = 0;
    }
    
    /* DxLib itself converts to decibels to send to
     * DirectSound's SetVolume call, which of course
     * converts it back to later.
     * 
     * Unfortunately, it does it incorrectly, so we can't
     * just map the value directly to a multiplier.
     */
    
    if (s_audioOldVolumeCalcFlag) {
        mul = 1000.0;
    } else {
        mul = 5000.0;
    }
    
    return PL_SetVolumeSoundMem((int)(log10(volume / 255.0) * mul), soundID);
}

int PL_SetUseOldVolumeCalcFlag(int volumeFlag) {
    s_audioOldVolumeCalcFlag = volumeFlag;
    
    return 0;
}

int PL_SetCreateSoundDataType(int soundDataType) {
    if (soundDataType != s_audioDataType) {
        s_audioDataType = soundDataType;
    }
    
    return 0;
}
int PL_GetCreateSoundDataType() {
    return 0;
}

int PL_InitSoundMem() {
    int handle;
    if (s_audioOpened == DXFALSE) {
        return 0;
    }
    
    SDL_LockAudio();
    
    while ((handle = PL_Handle_GetFirstIDOf(DXHANDLE_SOUND)) >= 0) {
        PL_DeleteSoundMem(handle);
    }
    
    SDL_UnlockAudio();
    
    return 0;
}

int PL_Audio_ResetSettings() {
    if (s_audioOpened == DXTRUE) {
        return -1;
    }
    
    s_audioOldVolumeCalcFlag = DXFALSE;
    s_audioDataType = DX_SOUNDDATATYPE_MEMNOPRESS;
    
    return 0;
}

int PL_Audio_Init() {
    return 0;
}

int PL_Audio_End() {
    s_AudioClose();
    
    return 0;
}

#endif /* #ifndef DX_NON_SOUND */
