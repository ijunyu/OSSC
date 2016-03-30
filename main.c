#include <stdio.h>
#include "include/oss_common.h"
#include "include/oss_client.h"
#include "include/oss_object_metadata.h"
#include "include/oss_constants.h"
#include "include/oss_helper.h"
#include "include/curl/curl.h"

static const char *access_id = "your id";   //设置用户帐号
static const char *access_key = "your key";  //设置用户密码
static const char *endpoint = "oss-cn-shenzhen.aliyuncs.com";    //设置hostname
static const char *bucket_name = "your bucket name";       //设置bucket_name

/**
* sender:来自custom_data的sender
* speed:上传速度，默认bytes/second
* remaining_time:剩余时间
* progress_percentage:上传进度百分比
*/
void progress_callback(void *sender, double speed, double remaining_time, double progress_percentage)
{
	//printf("%f %f %f\n", speed, remaining_time, progress_percentage);
}


int main()
{
	unsigned short retcode = -1;			//保存服务器http返回码的解析结果;
	const char *retinfo = NULL;            //保存通过retcode获得的错误信息

	const char *key         = "test@qq.com/Personal/ossc.txt";      //设置上传key
	const char *local_file  = "ossc.txt";         //设置需要上传的文件

	// or _wfopen
	FILE *fp = fopen(local_file, "rb");
	if (fp == NULL) {
		fprintf(stderr, "error in opening file %s\n", local_file);
		return -1;
	}
	
	//unsigned int file_len = oss_get_file_size(fp);
	//printf("file size:%d\n", file_len);
	
	oss_client_t *client = client_initialize_with_endpoint(access_id, access_key, endpoint);

	custom_data custom = {NULL, progress_callback, "Sat, 07 Nov 2015 11:12:14 GMT", "Vhptq4eFBIbOzKoPen0NDNaTVkw="};

	oss_put_object_result_t *result = client_put_object_from_file(client, bucket_name, key, fp, NULL, &retcode, &custom);

	if (retcode == OK) {
		printf("Put object from file successfully.\n");
	}
	else {
		retinfo = oss_get_error_message_from_retcode(retcode);
		printf("%s\n", retinfo);
	}

	if (result != NULL) put_object_result_finalize(result);
	client_finalize(client);
	fclose(fp);
	fp = NULL;

	return 0;
}
