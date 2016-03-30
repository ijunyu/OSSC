#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "include/oss_client.h"
#include "include/oss_constants.h"
#include "include/oss_time.h"
#include "include/oss_auth.h"
#include "include/oss_helper.h"
#include "include/oss_curl_callback.h"
#include "include/oss_common.h"
#include "include/curl/curl.h"

oss_client_t *_client_initialize(
		const char *access_id, unsigned int access_id_len,
		const char *access_key, unsigned int access_key_len,
		const char *endpoint, unsigned int endpoint_len)
{
	if (access_id == NULL || access_key == NULL || endpoint == NULL)
		return NULL;

	unsigned int byte_of_char = sizeof(char);
	oss_client_t *client = NULL;

	client = (oss_client_t *)malloc(sizeof(oss_client_t));

	client->access_id= (char *)malloc(byte_of_char * access_id_len+ 1);
	client->access_key = (char *)malloc(byte_of_char * access_key_len + 1);
	client->endpoint = (char *)malloc(byte_of_char * endpoint_len + 1);

	strncpy(client->access_id, access_id, access_id_len);
	strncpy(client->access_key, access_key, access_key_len);
	strncpy(client->endpoint, endpoint, endpoint_len);

	(client->access_id)[access_id_len] = '\0';
	(client->access_key)[access_key_len] = '\0';
	(client->endpoint)[endpoint_len] = '\0';

	return client;
}

void client_finalize(oss_client_t *client)
{
	assert(client != NULL);

	if (client->access_id != NULL) {
		free(client->access_id);
		client->access_id = NULL;
	}

	if (client->access_key != NULL) {
		free(client->access_key);
		client->access_key = NULL;
	}

	if (client->endpoint != NULL) {
		free(client->endpoint);
		client->endpoint = NULL;
	}

	free(client);
}

oss_client_t *client_initialize_with_endpoint(const char *access_id,
		const char *access_key,
		const char *endpoint)
{
	if (access_id == NULL || access_key == NULL || endpoint == NULL)
		return NULL;

	unsigned int access_id_len = strlen(access_id);
	unsigned int access_key_len = strlen(access_key);
	unsigned int endpoint_len = strlen(endpoint); 
	return _client_initialize(access_id, access_id_len,
			access_key, access_key_len,
			endpoint, endpoint_len);
}

static oss_put_object_result_t *
construct_put_object_response(
		curl_request_param_t *user_data)
{
	const char *etag = user_data->header_buffer->ptr;
	oss_put_object_result_t *result = put_object_result_initialize();
	result->set_etag(result, etag);
	oss_free_partial_user_data_2nd(user_data);
	return result;
}

static void
object_curl_operation(const char *method,
		const char *resource,
		const char *url,
		struct curl_slist *http_headers,
		void *user_data, 
		custom_data *custom)
{
	assert(method != NULL);
	assert(resource != NULL);
	assert(http_headers != NULL);
	assert(user_data != NULL);

	CURL *curl = NULL;

	curl_request_param_t *params = (curl_request_param_t *)user_data;


	param_buffer_t *send_buffer = params->send_buffer;
	param_buffer_t *recv_buffer = params->recv_buffer;
	param_buffer_t *header_buffer = params->header_buffer;

	curl = curl_easy_init();
	if (curl != NULL) {

		char buf[1024] = { 0 };
		strncpy(buf, url, strlen(url));

		char *filename = strrchr(buf, '/');
		++filename;

		char *out = curl_easy_escape(curl, filename, strlen(filename));
		memset(filename, '\0', strlen(filename));
		strncpy(filename, out, strlen(out));

		curl_easy_setopt(curl, CURLOPT_URL, buf);
		curl_easy_setopt(curl, CURL_HTTP_VERSION_1_1, 1L);

		if (strcmp(method, OSS_HTTP_PUT) == 0) {
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
			curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
			curl_easy_setopt(curl, CURLOPT_INFILESIZE, send_buffer->allocated);
			curl_easy_setopt(curl, CURLOPT_READFUNCTION, object_curl_operation_send_from_file_callback);
			curl_easy_setopt(curl, CURLOPT_READDATA, send_buffer);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, object_curl_operation_recv_to_buffer_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, recv_buffer);
			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, object_curl_operation_header_callback);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_buffer);

			user_progress_data data = {custom->sender, curl, custom->cb};
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
			curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &data);
			curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, object_curl_progress_callback);
		} else if (strcmp(method, OSS_HTTP_GET) == 0) {
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, object_curl_operation_recv_to_buffer_2nd_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, recv_buffer);
			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, object_curl_operation_header_callback);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_buffer);
		} else if (strcmp(method, OSS_HTTP_HEAD) == 0) {
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, object_curl_operation_recv_to_buffer_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, recv_buffer);
			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, object_curl_operation_header_callback_2nd);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_buffer);	
		} else if (strcmp(method, OSS_HTTP_DELETE) == 0) {
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, object_curl_operation_recv_to_buffer_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, recv_buffer);
			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, object_curl_operation_header_callback);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_buffer);	
		} else if (strcmp(method, OSS_HTTP_POST) == 0) {
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, send_buffer->ptr);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, object_curl_operation_recv_to_buffer_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, recv_buffer);
			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, object_curl_operation_header_callback);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_buffer);
		}
		//curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_headers);
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

}

oss_put_object_result_t *
client_put_object_from_file(oss_client_t *client,
		const char *bucket_name,
		const char *key,
		void *input, /**< 文件指针*/
		oss_object_metadata_t *metadata,
		unsigned short *retcode, 
		custom_data *custom)
{
	if (client == NULL || bucket_name == NULL || key == NULL || input == NULL)
		return NULL;

	curl_request_param_t *user_data = 
		(curl_request_param_t *)malloc(sizeof(curl_request_param_t));

	user_data->send_buffer = (param_buffer_t *)malloc(sizeof(param_buffer_t));
	user_data->send_buffer->ptr = NULL;
	user_data->send_buffer->fp = (FILE *)input; /**< 从文件中读取数据  */
	if (metadata != NULL) {
		user_data->send_buffer->left = metadata->get_content_length(metadata);
		user_data->send_buffer->allocated = metadata->get_content_length(metadata);
	} else {
		user_data->send_buffer->left = oss_get_file_size((FILE *)input);
		user_data->send_buffer->allocated = oss_get_file_size((FILE *)input);
	}

	user_data->recv_buffer = (param_buffer_t *)malloc(sizeof(param_buffer_t));
	user_data->recv_buffer->ptr = (char *)malloc(sizeof(char) * MAX_RECV_BUFFER_SIZE);
	user_data->recv_buffer->fp = NULL;
	user_data->recv_buffer->left = MAX_RECV_BUFFER_SIZE;
	user_data->recv_buffer->allocated = MAX_RECV_BUFFER_SIZE;
	memset(user_data->recv_buffer->ptr, 0, MAX_RECV_BUFFER_SIZE);

	user_data->header_buffer = (param_buffer_t *)malloc(sizeof(param_buffer_t));
	user_data->header_buffer->ptr = (char *)malloc(sizeof(char) * MAX_HEADER_BUFFER_SIZE);
	user_data->header_buffer->fp = NULL;
	user_data->header_buffer->left = MAX_HEADER_BUFFER_SIZE;
	user_data->header_buffer->allocated = MAX_HEADER_BUFFER_SIZE;
	memset(user_data->header_buffer->ptr, 0, MAX_HEADER_BUFFER_SIZE);

	unsigned int bucket_name_len = strlen(bucket_name);
	unsigned int key_len = strlen(key);
	char *resource = (char *)malloc(sizeof(char) * (bucket_name_len + key_len + 16));
	char *url = (char *)malloc(sizeof(char) * 
			(bucket_name_len + key_len + strlen(client->endpoint) + 8 ));

	char *now;  /**< Fri, 24 Feb 2012 02:58:28 GMT */

	char header_host[256]  = {0};
	char header_date[48]  = {0};
	char header_auth[128] = {0};
	char header_content_type[64] = {0}; /**< 64字节足够容纳MIME标准类型 */
	char header_cache_control[48] = {0};
	char header_expires[64] = {0};
	char header_content_encoding[64] = {0};
	char header_content_disposition[256] = {0};

	unsigned int sign_len = 0;

	oss_map_t *default_headers = oss_map_new(16);
	oss_map_t *user_headers = NULL;

	/**
	 * 构造参数，resource,url 赋值
	 * */
	sprintf(resource, "/%s/%s", bucket_name, key);
	sprintf(url, "%s/%s/%s", client->endpoint, bucket_name, key);
	now = (char *)oss_get_gmt_time();

	/** 构造请求头部 */
	sprintf(header_host, "Host: %s", client->endpoint);
	sprintf(header_date, "Date: %s", now);

	/* Content-Type is a must */
	if (metadata != NULL) {
		sprintf(header_content_type, "Content-Type: %s",
				metadata->get_content_type(metadata));
		oss_map_put(default_headers, OSS_CONTENT_TYPE,
				metadata->get_content_type(metadata));
		user_headers = metadata->get_user_metadata(metadata);
	} else {
		sprintf(header_content_type, "Content-Type: application/octet-stream");
		oss_map_put(default_headers, OSS_CONTENT_TYPE,
				"application/octet-stream");

	}
	
	oss_map_put(default_headers, OSS_DATE, now);
	// oss_map_put(default_headers, OSS_DATE, custom->gmtdate);

	/**
	 * 生成签名值
	 */
	char *sign = (char *)generate_authentication(client->access_key, OSS_HTTP_PUT,
			default_headers, user_headers, resource, &sign_len);

	sprintf(header_auth, "Authorization: OSS %s:%s", client->access_id, sign);
	// sprintf(header_auth, "Authorization: OSS %s:%s", client->access_id, custom->sign);

	/**
	 * 自定义 HTTP 请求头部
	 */
	struct curl_slist *http_headers = NULL;
	
	if (metadata != NULL) {
		if (metadata->get_cache_control(metadata) != NULL) {
			sprintf(header_cache_control, "Cache-Control: %s", metadata->get_cache_control(metadata));
			http_headers = curl_slist_append(http_headers, header_cache_control);
		}
		if (metadata->get_expiration_time(metadata) != NULL) {
			sprintf(header_expires, "Expires: %s", metadata->get_expiration_time(metadata));
			http_headers = curl_slist_append(http_headers, header_expires);
		}
		if (metadata->get_content_encoding(metadata) != NULL) {
			sprintf(header_content_encoding, "Content-Encoding: %s", metadata->get_content_encoding(metadata));
			http_headers = curl_slist_append(http_headers, header_content_encoding);
		}
		if (metadata->get_content_disposition(metadata) != NULL) {
			sprintf(header_content_disposition, "Content-Disposition: %s", metadata->get_content_disposition(metadata));
			http_headers = curl_slist_append(http_headers, header_content_disposition);
		}
	}

	http_headers = curl_slist_append(http_headers, header_content_type);
	http_headers = curl_slist_append(http_headers, header_host);
	http_headers = curl_slist_append(http_headers, header_date);
	http_headers = curl_slist_append(http_headers, header_auth);

	/**
	 * 发送请求
	 */
	object_curl_operation(OSS_HTTP_PUT, resource, url, http_headers, user_data);

	/**
	 * 释放资源
	 */
	oss_map_delete(default_headers);
	if(now != NULL) {
		free(now);
		now = NULL;
	}
	if(sign != NULL) {
		free(sign);
		sign = NULL;
	}
	if(resource != NULL) {
		free(resource);
		resource = NULL;
	}
	if(url != NULL) {
		free(url);
		url = NULL;
	}
	curl_slist_free_all(http_headers);
	if (user_data->header_buffer->code == 200) {
		if (retcode != NULL) *retcode = 0;
		return construct_put_object_response(user_data);
	} else {
		if (retcode != NULL) 
			*retcode = oss_get_retcode_from_response(user_data->recv_buffer->ptr);
		oss_free_user_data(user_data);
	}
	return NULL;
}