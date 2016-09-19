//sound.h
// author: Matthew B

#ifndef SOUND_H
#define SOUND_H

#include "SDL_mixer.h"

#include <string>


class Sound
{
 public:
  Sound(std::string filename, int audiorate = 11025, Uint16 audioformat = AUDIO_S16SYS );
  ~Sound();
  void play();
  void waitForFinish();

  static void Sound::killSDLMixer()
  {
   Mix_CloseAudio();
  }

 private:
  std::string name;

	int audio_rate;			//Frequency of audio playback
	Uint16 audio_format; 	//Format of the audio we're playing
	int audio_channels;			//2 channels = stereo
	int audio_buffers;		//Size of the audio buffers in memory

  Mix_Chunk *sound;		//Pointer to our sound, in memory
  int channel;				//Channel on which our sound is played
};

#endif
