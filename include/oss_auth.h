#ifndef __OSS_AUTH_H__
#define __OSS_AUTH_H__

#include "oss_map.h"

char *generate_authentication(const char *access_key, const char *method, oss_map_t *default_headers, oss_map_t *user_headers, const char *resource, unsigned int *sign_len);
		
#endif