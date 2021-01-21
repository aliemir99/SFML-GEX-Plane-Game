#include "MusicPlayer.h"

MusicPlayer::MusicPlayer()
	: music()
	, filenames()
	, volume(100.f)
{
	filenames[MusicID::MenuTheme] = "Media/Music/MenuTheme.ogg";
	filenames[MusicID::MissionTheme] = "Media/Music/MissionTheme.ogg";
}

void MusicPlayer::play(MusicID theme)
{
	auto filename = filenames[theme];

	if (!music.openFromFile(filename))
		throw std::runtime_error("Music " + filename + "could not be loaded");

	music.setVolume(volume);
	music.setLoop(true);
	music.play();
}

void MusicPlayer::stop()
{
	music.stop();
}

void MusicPlayer::setPaused(bool paused)
{
	if (paused)
		music.pause();
	else
		music.play();
}

void MusicPlayer::setVolume(float vol)
{
	volume = vol;
}
