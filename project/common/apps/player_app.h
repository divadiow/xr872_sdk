/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _APPS_PLAYER_APP_H_
#define _APPS_PLAYER_APP_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum aplayer_states {
	APLAYER_STATES_PAUSE,
	APLAYER_STATES_PLAYING,
	APLAYER_STATES_PREPARING,
	APLAYER_STATES_PREPARED,
	APLAYER_STATES_STOPPED,
	APLAYER_STATES_RESUME,
	APLAYER_STATES_INIT,
	APLAYER_STATES_NONE,
	APLAYER_STATES_COMPLETED,
	APLAYER_STATES_ERROR,
} aplayer_states;

typedef enum player_events {
	PLAYER_EVENTS_NONE                      = APLAYER_STATES_NONE,
	PLAYER_EVENTS_MEDIA_PREPARED            = APLAYER_STATES_PLAYING,
	PLAYER_EVENTS_MEDIA_START_PLAY          = APLAYER_STATES_PLAYING,
	PLAYER_EVENTS_MEDIA_STOPPED             = APLAYER_STATES_STOPPED,
	PLAYER_EVENTS_MEDIA_ERROR               = APLAYER_STATES_ERROR,
	PLAYER_EVENTS_MEDIA_PLAYBACK_COMPLETE   = APLAYER_STATES_COMPLETED,
} player_events;

typedef enum player_cmd {
	PLAYER_CMD_SET_OUTPUT_CONFIG,
	PLAYER_CMD_ADD_OUTPUT_CONFIG,
	PLAYER_CMD_CLEAR_OUTPUT_CONFIG,
	PLAYER_CMD_SET_EQ_MODE,
	PLAYER_CMD_CLEAR_EQ_MODE,
	PLAYER_CMD_SET_DRC_MODE,
	PLAYER_CMD_CLEAR_DRC_MODE,
} player_cmd;

typedef void (*app_player_callback)(player_events event, void *data, void *arg);

typedef struct player_base {
	int (*play)(struct player_base *base, const char *url);
	int (*stop)(struct player_base *base);
	int (*pause)(struct player_base *base);
	int (*resume)(struct player_base *base);
	int (*seek)(struct player_base *base, int ms);
	int (*tell)(struct player_base *base);
	int (*size)(struct player_base *base);
	int (*setvol)(struct player_base *base, int vol);
	int (*getvol)(struct player_base *base);
	int (*mute)(struct player_base *base, bool is_mute);
	int (*is_mute)(struct player_base *base);
	int (*control)(struct player_base *base, player_cmd command, void *data);
	void (*set_callback)(struct player_base *base, app_player_callback cb, void *arg); /* callback can't use player interface */
	aplayer_states (*get_status)(struct player_base *base);
} player_base;

player_base *player_create();
void player_destroy(player_base *base);

#ifdef __cplusplus
}
#endif

#endif /* _APPS_PLAYER_APP_H_ */
