#ifndef __OSS_CLIENT_H__
#define __OSS_CLIENT_H__

#include <stdio.h>
#include "oss_object_metadata.h"
#include "oss_put_object_result.h"

typedef struct oss_client_s {
	char *endpoint;       /**< hostname */
	char *access_id;      /**< 用户的OSS服务用户名 */
	char *access_key;     /**< 用户的OSS服务密码 */
}oss_client_t;

struct param_buffer_s {
	char *ptr; /**< 缓冲区首指针 */
	FILE *fp; /**< 文件指针 */
	unsigned int left; /** 缓冲区剩余大小 */
	unsigned int allocated; /** 缓冲区总大小 */
	unsigned short code; /**返回码 */
};
typedef struct param_buffer_s param_buffer_t;

struct curl_request_param_s {
	param_buffer_t *send_buffer; /**< send buffer */
	param_buffer_t *recv_buffer; /**< receive buffer */
	param_buffer_t *header_buffer; /**< header buffer */
};
typedef struct curl_request_param_s curl_request_param_t;

/**
* oss_client_t 带endpoint参数的构造函数
* @param access_id [in] 用户的OSS服务用户名
* @param access_key [in] 用户的OSS服务密码
* @param endpoint [in] 标识hostname
* @return 返回一个标识该用户身份的结构指针
* @retval 非空 表示成功
* @retval NULL 表示失败
*/
oss_client_t *client_initialize_with_endpoint(const char *access_id, const char *access_key, const char *endpoint);

/**
* oss_client_t 析构函数
* @param client [in] 标识一个oss_client_t的结构指针
* @retval void
* @pre client 必须使用client_initialize的返回值
*/
void client_finalize(oss_client_t *client);

oss_put_object_result_t *client_put_object_from_file(oss_client_t *client, const char *bucket_name, const char *key, void *input, oss_object_metadata_t *metadata, unsigned short *retcode);
#endif