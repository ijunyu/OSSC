#ifndef __OSS_PUT_OBJECT_RESULT_H__
#define __OSS_PUT_OBJECT_RESULT_H__

typedef struct oss_put_object_result_s oss_put_object_result_t;
struct oss_put_object_result_s {
	char *etag; /**< 新创建的OSSObject的ETag值*/

	/**
	 * 获得新创建的OSSObject的ETag值
	 * @param result [in] 标识一个oss_put_object_result_t结构指针
	 * @return 新创建的OSSObject的ETag值
	 * @retval const char *
	 */
	const char * (*get_etag)(oss_put_object_result_t *result);

	/**
	 * 设置新创建的OSSObject的ETag值
	 * @param result [in] 标识一个oss_put_object_result_t结构指针
	 * @param etag [in] 新创建的OSSObject的ETag值
	 * @retval void
	 */
	void (*set_etag)(oss_put_object_result_t *result, const char *etag);
};

oss_put_object_result_t *put_object_result_initialize();

/**
* oss_put_object_result_t 析构函数
* @param  result [in] 标识一个oss_put_object_result_t结构指针
* @retval void
* @pre result 必须使用put_object_result_initialize的返回值
*/
void put_object_result_finalize(oss_put_object_result_t *result);
		
#endif