#pragma once

void publish_all();
void *app_free(char *buffer, uint32_t *cbuffer, char *param);
void *app_hello(char *buffer, uint32_t *cbuffer, char *param);
void *app_poweroff(char *buffer, uint32_t *cbuffer, char *param);
void *app_reboot(char *buffer, uint32_t *cbuffer, char *param);
void *app_window(char *buffer, uint32_t *cbuffer, char *param);
void *app_tasklist(char *buffer, uint32_t *cbuffer, char *param);
void *app_nyancat(char *buffer, uint32_t *cbuffer, char *param);
void *app_crash1(char *buffer, uint32_t *cbuffer, char *param);
void *app_crash2(char *buffer, uint32_t *cbuffer, char *param);
void *app_crash3(char *buffer, uint32_t *cbuffer, char *param);
void *app_time(char *buffer, uint32_t *cbuffer, char *param);
void *app_help(char *buffer, uint32_t *cbuffer, char *param);
void *app_binaryclock(char *buffer, uint32_t *cbuffer, char *param);
