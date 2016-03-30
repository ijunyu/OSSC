###OSSC  
阿里云存储OSS的C语言SDK，只保留上传功能，可用于Windows、Mac OS 及 Linux。

###Linux下编译  
gcc main.c oss_client.c oss_common.c oss_map.c oss_object_metadata.c oss_time.c oss_auth.c oss_put_object_result.c oss_helper.c oss_curl_callback.c oss_ttxml.c include/extra/hmac-sha1.c include/extra/base64.c include/extra/sha1.c include/extra/memxor.c -lcurl  

Linux下需要libcurl动态库  
Windows下需要libcurl.dll和libcurl.lib
