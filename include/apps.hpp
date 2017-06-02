#pragma once

void publish_all();
void *app_free(char *buffer, uint32_t *cbuffer, char *param);
void *app_hello(char *buffer, uint32_t *cbuffer, char *param);
void *app_poweroff(char *buffer, uint32_t *cbuffer, char *param);
void *app_reboot(char *buffer, uint32_t *cbuffer, char *param);
void *app_window(char *buffer, uint32_t *cbuffer, char *param);
void *app_tasklist(char *buffer, uint32_t *cbuffer, char *param);
