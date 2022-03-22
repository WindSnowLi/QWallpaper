#ifndef VIDEOPLAYER_H_
#define VIDEOPLAYER_H_
#define ssize_t SSIZE_T
#include <windows.h>
#include <WinUser.h>
#include <vlc/vlc.h>
#pragma comment(lib, "libvlc.lib")
#pragma comment(lib, "libvlccore.lib")
class VideoPlayer
{
private:
	int Volume = 30;
	libvlc_instance_t* inst = NULL;
	libvlc_media_t* m = NULL;
	libvlc_media_list_t* ml = NULL;
	libvlc_media_player_t* mp = NULL;
	libvlc_media_list_player_t* mlp = NULL;
public:
	bool setVolume(int volume);
	bool releasePlayer();
	bool loadPlayer(const char* videoPath);
	float get_position();
	float get_length();
	bool set_position(float position);
	bool set_stop();
	bool set_play();
	//获取播放器是否存在
	bool getVideoStatus();
	//获取播放状态
	bool getPlayingStatus();
	HWND workerw;
};
//播放器对象
extern VideoPlayer* videoPlayer;
#endif // !VIDEOPLAYER_H_