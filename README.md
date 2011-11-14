# B2G Audio Daemon

## Introduction

This project contains a daemon program for accepting Parcel-based commands and executing them 
on the Android audio subsystem. This daemon performs actions such as turning the speaker 
phone -or microphone- on/off during the call.

## Parcel Formats

Parcels contain information about the transaction that they are a part
of, as well as event data.

Event data will be one of the follow types:

- Void - No return
- Int List - First int is the number of ints in the list, followed by the ints
  - Byte 0x0c - uint32_t - number of ints in the packet
  - Byte 0x10 - uint32_t* - list of ints
- String - A single string (16-bit)
  - Byte 0x0c - uint32_t - length of string
  - Byte 0x10 - wchar_t* - string of length specified
- Strings - An array of strings (16-bit)
  - Byte 0x0c - uint32_t - number of strings
  - Byte 0x10 - Strings - strings in format listed for single string (size + string)
- Custom Struct - A set of different types specific to the message (SMS messages, call lists, etc...)

### JS - B2G Audio Daemon: Parcel-based command communication

Each JS socket write has the following format:

- Byte 0x00 - uint32_t - Header (Size of Following Parcel)
  - PARCEL BEGINS AFTER THIS FIELD
- Byte 0x04 - uint32_t - Event ID (See next section)
- Byte 0x08 - uint32_t - Packet Serial Number (Used to track send/receive order)
- Byte 0x0c - void* - Event Data

## Design

The daemon waits network connections on port #6666 and accepts the following parcel-based commands.

Byte 0x04 - uint32_t - Event ID := AUDIO_REQUEST_{FORCE_COMMUNICATION, SPEAKER_ON_OFF, MIC_MUTE_UNMUTE, MODE_NORMAL}

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