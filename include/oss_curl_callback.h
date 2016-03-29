#ifndef __OSS_CURL_CALLBACK_H__
#define __OSS_CURL_CALLBACK_H__

unsigned int
object_curl_operation_send_from_file_callback(
		void *ptr, unsigned int size,
		unsigned int nmemb, void *stream);
		
unsigned int
object_curl_operation_recv_to_buffer_callback(
		void *ptr, unsigned int size,
		unsigned int nmemb, void *stream);


unsigned int
object_curl_operation_header_callback(
		void *ptr, unsigned int size,
		unsigned int nmemb, void *stream);

unsigned int
object_curl_operation_recv_to_buffer_2nd_callback(
		void *ptr, unsigned int size,
		unsigned int nmemb, void *stream);
		
unsigned int
object_curl_operation_header_callback_2nd(
		void *ptr, unsigned int size,
		unsigned int nmemb, void *stream);
		
				
#endif