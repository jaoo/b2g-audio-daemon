#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <pwd.h>
#include <sys/stat.h>
#include <linux/prctl.h>

#include <utils/Log.h>
#include <cutils/sockets.h>
#include <media/AudioSystem.h>
#include <telephony/ril.h>

#define AUDIOB2G_LISTEN_PORT 6666

#define AUDIO_REQUEST_FORCE_COMMUNICATION 2000
#define AUDIO_REQUEST_SPEAKER_ON_OFF  2001
#define AUDIO_REQUEST_MIC_MUTE_UNMUTE 2002
#define AUDIO_REQUEST_MODE_NORMAL 2003

using namespace android;

int onAudioRequest(int request){
  switch(request) {
  case AUDIO_REQUEST_FORCE_COMMUNICATION: 
    LOGI("AUDIO_REQUEST_FORCE_COMMUNICATION\n");
    AudioSystem::setPhoneState(AudioSystem::MODE_IN_CALL);
    AudioSystem::setForceUse(AudioSystem::FOR_COMMUNICATION, AudioSystem::FORCE_NONE);
    return 0;
  case AUDIO_REQUEST_SPEAKER_ON_OFF: 
    LOGI("AUDIO_REQUEST_SPEAKER_ON_OFF\n");
    AudioSystem::setPhoneState(AudioSystem::MODE_IN_CALL);
    AudioSystem::setForceUse(AudioSystem::FOR_COMMUNICATION,
			     AudioSystem::getForceUse(AudioSystem::FOR_COMMUNICATION) == 
			     AudioSystem::FORCE_NONE ? AudioSystem::FORCE_SPEAKER : AudioSystem::FORCE_NONE);
			return 0;
  case AUDIO_REQUEST_MIC_MUTE_UNMUTE: 
    LOGI("AUDIO_REQUEST_MIC_MUTE_UNMUTE\n");
    bool state;
    AudioSystem::isMicrophoneMuted(&state);
    state ? AudioSystem::setPhoneState(AudioSystem::MODE_IN_CALL) :
      AudioSystem::setPhoneState(AudioSystem::MODE_IN_COMMUNICATION);    
    AudioSystem::muteMicrophone(!state);
    return 0;
  case AUDIO_REQUEST_MODE_NORMAL: 
    LOGI("AUDIO_REQUEST_MODE_NORMAL\n");
    AudioSystem::setPhoneState(AudioSystem::MODE_NORMAL);
    return 0;
  default: 
    LOGW("UNKNOWN REQUEST\n");
    return 1;
  }
}

int main(int argc, char **argv){
  pid_t pid, sid;
  int audiob2g_connect;
  int audiob2g_rw;
  int retval;
  struct pollfd connect_fds;
  char data[1024];

  pid = fork();
  if(pid < 0) 
    exit(EXIT_FAILURE);
  
  if(pid > 0)
    exit(EXIT_SUCCESS);
	
  umask(0);
	
  sid = setsid();
  if (sid < 0) 
    exit(EXIT_FAILURE);

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  
  (void)setpgrp();		

  audiob2g_connect = socket_inaddr_any_server(AUDIOB2G_LISTEN_PORT, SOCK_STREAM);
  if(audiob2g_connect < 0){
    LOGE("Cannot create network socket.");
    exit(EXIT_FAILURE);
  }
  
  while(1){
    connect_fds.fd = audiob2g_connect;
    connect_fds.events = POLLIN;
    connect_fds.revents = 0;
    poll(&connect_fds, 1, -1);

    audiob2g_rw = accept(audiob2g_connect, NULL, NULL);
    memset(data, '\0', 1024);
    
    while(1){
      retval = read(audiob2g_rw, data, 1024);
      if(retval > 0){
	if(retval > 4){
	  uint32_t *prequest = reinterpret_cast<uint32_t*>(data);
	  uint32_t request = *(prequest);
	  if (request > RIL_UNSOL_RESEND_INCALL_MUTE)
	    onAudioRequest(request);
	}
      }else break;
    }
    close(audiob2g_rw);
  }

  exit(EXIT_SUCCESS);
}
