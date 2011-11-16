# B2G Audio Daemon

## Introduction

This project contains a daemon program for accepting Parcel-based commands and executing them 
on the Android audio subsystem. This daemon performs actions such as turning the speaker 
phone -or microphone- on/off during the call.


## Design

The daemon waits network connections on port #6666 and accepts the following parcel-based commands.

Byte 0x00 - uint32_t - Event ID := AUDIO_REQUEST_{FORCE_COMMUNICATION, SPEAKER_ON_OFF, MIC_MUTE_UNMUTE, MODE_NORMAL}

### AUDIO_REQUEST_FORCE_COMMUNICATION

Event ID value which indicates to set the phone in IN_CALL mode and to force use as FOR_COMMUNICATION usage.
 
### AUDIO_REQUEST_SPEAKER_ON_OFF 

Event ID value which indicates to set the phone in IN_CALL mode and to force use as FORCE_SPEAKER usage. It turns the phone speaker on or off -set force use as FORCE_NONE usage- depending on the previous usage.

### AUDIO_REQUEST_MIC_MUTE_UNMUTE 

Event ID value which indicates to mute or unmute the microphone denpending the previous state. 

### AUDIO_REQUEST_MODE_NORMAL

Event ID value which indicates to set the phone in IN_NORMAL mode when the call has finnished.

## Workflow

Once the outgoing call is initiated the daemon has to receive the AUDIO_REQUEST_FORCE_COMMUNICATION parcel-based command. This command set the phone in IN_CALL mode routing the audio to headset and capturing audio from micrphone. From this state we could turn the speaker on or off and mute or unmute the microphone. Once the call is finnished we must to set the phone in IN_NORMAL mode.
