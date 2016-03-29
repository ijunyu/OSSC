#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>
#include "include/oss_curl_callback.h"
#include "include/oss_client.h"

unsigned int
object_curl_operation_send_from_file_callback(
		void *ptr, unsigned int size,
		unsigned int nmemb, void *stream)
{
	param_buffer_t *send_buffer = (param_buffer_t *)stream;
	unsigned int r = fread(ptr, size, nmemb, send_buffer->fp);
	return r;
}

unsigned int
object_curl_operation_recv_to_buffer_callback(
		void *ptr, unsigned int size,
		unsigned int nmemb, void *stream)
{
	param_buffer_t *recv_buffer = (param_buffer_t *)stream;
	unsigned int bytes_per_recv = size * nmemb;
	if ((int)(recv_buffer->left) > 0) {
		unsigned int offset = recv_buffer->allocated - recv_buffer->left;
		if (recv_buffer->left > bytes_per_recv) {
			strncpy(recv_buffer->ptr + offset, ptr, size * nmemb);
			recv_buffer->left -= bytes_per_recv;
			return bytes_per_recv;
		} else {
			strncpy(recv_buffer->ptr + offset, ptr, recv_buffer->left);
			unsigned int last_recv_bytes = recv_buffer->left;
			recv_buffer->left -= bytes_per_recv;
			return last_recv_bytes;
		}
	} else {
		fprintf(stderr, "%s\n", "Receive buffer overflow!");
		return 0;
	}
}

unsigned int
object_curl_operation_header_callback(
		void *ptr, unsigned int size,
		unsigned int nmemb, void *stream)
{
	param_buffer_t *header_buffer = (param_buffer_t *)stream;
	char etag[48] = {0};
	int rcode = 0;
	int retag = 0;
	unsigned int code = 0;

	rcode = sscanf(ptr, "HTTP/1.1 %u\n", &code);
	if (rcode != 0) {
		header_buffer->code= code;
	}

	retag = sscanf(ptr, "ETag: %s", etag);
	if (retag != 0) {
		memset(header_buffer->ptr, 0, header_buffer->allocated);
		strncpy(header_buffer->ptr, etag, 48);
	}
	return size * nmemb;
}

unsigned int
object_curl_operation_recv_to_buffer_2nd_callback(
		void *ptr, unsigned int size,
		unsigned int nmemb, void *stream)
{

  unsigned int recv_size = size * nmemb;
  param_buffer_t *recv_buffer = (param_buffer_t *)stream;
 
  recv_buffer->ptr = realloc(recv_buffer->ptr, recv_buffer->allocated + recv_size + 1);
  if (recv_buffer->ptr == NULL) {
    /* out of memory! */ 
    exit(EXIT_FAILURE);
  }
  memcpy(&(recv_buffer->ptr[recv_buffer->allocated]), ptr, recv_size);
  recv_buffer->allocated += recv_size;
  recv_buffer->ptr[recv_buffer->allocated] = 0;
  return recv_size;
}

unsigned int
object_curl_operation_header_callback_2nd(
		void *ptr, unsigned int size,
		unsigned int nmemb, void *stream)
{
	param_buffer_t *header_buffer = (param_buffer_t *)stream;
	char etag[48] = {0};
	char type[64] = {0};
	char length[16] = {0};
	char week[5] = {0};
	char day[3] = {0};
	char mon[4] = {0};
	char year[5] = {0};
	char time[9] = {0};
	char gmt[4] = {0};
	int rcode = 0;
	int retag = 0;
	int rtype = 0;
	int rlength = 0;
	int rlastmodified = 0;
	unsigned int code = 0;

	rcode = sscanf(ptr, "HTTP/1.1 %u\n", &code);
	if (rcode != 0) {
		header_buffer->code= code;
	}

	retag = sscanf(ptr, "ETag: %s", etag);
	if (retag != 0) {
		unsigned int offset = header_buffer->allocated - header_buffer->left;
		retag = sprintf(header_buffer->ptr + offset, "Content-Md5#%s#", etag);
		header_buffer->left -= retag;
	}
	
	rtype = sscanf(ptr, "Content-Type: %s", type);
	if (rtype != 0) {
		unsigned int offset = header_buffer->allocated - header_buffer->left;
		rtype = sprintf(header_buffer->ptr + offset, "Content-Type#%s#", type);
		header_buffer->left -= rtype;
	}
	
	rlength = sscanf(ptr, "Content-Length: %s", length);
	if (rlength != 0) {
		unsigned int offset = header_buffer->allocated - header_buffer->left;
		rlength = sprintf(header_buffer->ptr + offset, "Content-Length#%s#", length);
		header_buffer->left -= rlength;
	}

	rlastmodified = sscanf(ptr, "Last-Modified: %s %s %s %s %s %s", week, day, mon, year, time, gmt);
	if (rlastmodified != 0) {
		unsigned int offset = header_buffer->allocated - header_buffer->left;
		rlastmodified = sprintf(header_buffer->ptr + offset, "Last-Modified#%s %s %s %s %s %s#", week,
				day, mon, year, time, gmt);
		header_buffer->left -= rlastmodified;
	}
	return size * nmemb;
}