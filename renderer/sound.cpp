
#include "sound.h"


Sound::Sound(std::string filename, int audiorate, Uint16 audioformat  )
{
 audio_rate = audiorate; 
 audio_format = audioformat;
 audio_channels = 2;
 audio_buffers = 4096;
 name = filename;

	//Initialize SDL_mixer with our chosen audio settings
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
  {
   //cout << "Error loading SDL_mixer\n";
	}

	//Load our WAV file from disk
	sound = Mix_LoadWAV(filename.c_str());
	if(sound == NULL) {
		//printf("Unable to load WAV file: %s\n", Mix_GetError());
	}

}

void Sound::play()
{
	//Play our sound file, and capture the channel on which it is played
	channel = Mix_PlayChannel(-1, sound, 0);
	if(channel == -1) {
		//printf("Unable to play WAV file: %s\n", Mix_GetError());
	}
}

void Sound::waitForFinish()
{
 while(Mix_Playing(channel) != 0);
}

Sound::~Sound()
{
	Mix_FreeChunk(sound);
}

