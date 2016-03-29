#ifndef __OSS_HELPER_H__
#define __OSS_HELPER_H__

#include "oss_client.h"

void oss_free_partial_user_data_2nd(curl_request_param_t *user_data);

unsigned short oss_get_retcode_from_response(const char *response);

void oss_free_user_data(curl_request_param_t *user_data);

unsigned short oss_get_retcode_from_retinfo(const char *retinfo);

const char *oss_get_error_message_from_retcode(unsigned short retcode);

#endif