#include "videoplayer.h"
#include <QMessageBox>

//播放器全局对象
VideoPlayer* videoPlayer = new VideoPlayer();
bool VideoPlayer::setVolume(int volume) {
	Volume = volume;
	if (m != NULL && mp != NULL && ml != NULL && mlp != NULL) {
		libvlc_audio_set_volume(mp, Volume);
	}
	return true;
}

bool VideoPlayer::releasePlayer() {
	if (m != NULL) {
		libvlc_media_release(m);
		m = NULL;
	}
	if (ml != NULL) {
		libvlc_media_list_release(ml);
		ml = NULL;
	}
	if (mlp != NULL) {
		libvlc_media_list_player_release(mlp);
		mlp = NULL;
	}
	if (mp != NULL) {
		libvlc_media_player_release(mp);
		mp = NULL;
	}
	if (inst != NULL) {
		libvlc_release(inst);
		inst = NULL;
	}
	return true;
}

bool VideoPlayer::loadPlayer(const char* videoPath) {
	if (m != NULL && mp != NULL && ml != NULL && mlp != NULL) {
		releasePlayer();
	}
	inst = libvlc_new(0, NULL);
	m = libvlc_media_new_path(inst, videoPath);
	ml = libvlc_media_list_new(inst);
	mp = libvlc_media_player_new_from_media(m);
	mlp = libvlc_media_list_player_new(inst);
	libvlc_media_list_add_media(ml, m);
	libvlc_media_list_player_set_media_list(mlp, ml);
	libvlc_media_list_player_set_media_player(mlp, mp);
	//libvlc_media_list_player_set_playback_mode(mlp, libvlc_playback_mode_loop);
	libvlc_media_player_set_hwnd(mp, workerw);
	libvlc_audio_set_volume(mp, Volume);
	libvlc_media_list_player_play(mlp);
	return true;
}

float VideoPlayer::get_position()
{
	float temp = 0;
	if (mp != NULL) {
		temp = libvlc_media_player_get_position(mp);
	}

	return temp;
}

float VideoPlayer::get_length()
{
	float temp = 0;
	if (mp != NULL) {
		temp = libvlc_media_player_get_length(mp);
	}
	return temp;
}

bool VideoPlayer::set_position(float position)
{
	if (mp != NULL) {
		libvlc_media_player_set_position(mp, position);
	}
	return true;
}

bool VideoPlayer::set_stop() {
	if (mp != NULL) {
		libvlc_media_player_pause(mp);
	}
	return true;
}

bool VideoPlayer::set_play() {
	if (mp != NULL) {
		libvlc_media_player_play(mp);
	}
	return true;
}