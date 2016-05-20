/*
 * Copyright (c) 2015 - 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <glib.h>
#include <system_info.h>
#include <system_settings.h>
#include <cynara-client.h>
#include <cynara-error.h>

#include "iotcon-types.h"
#include "ic-common.h"
#include "ic-log.h"
#include "ic-utils.h"


#ifdef TZ_VER_3
static int _ic_oic_feature_supported = -1;
#endif

// TODO: Can't access in user side daemon
#if 0
#ifdef TZ_VER_3
static const char *IC_PRIV_FILE_NETWORK_GET = "/usr/share/iotcon/iotcon-network-get";
static const char *IC_PRIV_FILE_INTERNET = "/usr/share/iotcon/iotcon-internet";
#endif
#endif

// TODO: Temporary code (need guide from security team)
#define SMACK_LABEL_LEN 255
static const char *IC_PRIVILEGE_INTERNET = "http://tizen.org/privilege/internet";
static const char *IC_PRIVILEGE_NETWORK_GET = "http://tizen.org/privilege/network.get";

static const char *IC_SYSTEM_INFO_PLATFORM_VERSION = "http://tizen.org/feature/platform.version";
static const char *IC_SYSTEM_INFO_MANUF_NAME = "http://tizen.org/system/manufacturer";
static const char *IC_SYSTEM_INFO_MODEL_NAME = "http://tizen.org/system/model_name";
static const char *IC_SYSTEM_INFO_BUILD_STRING = "http://tizen.org/system/build.string";
static const char *IC_SYSTEM_INFO_TIZEN_ID = "http://tizen.org/system/tizenid";

static pthread_mutex_t icl_utils_mutex_init = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t icl_utils_mutex_ioty = PTHREAD_MUTEX_INITIALIZER;
static __thread int icl_utils_pthread_oldstate;
static __thread int icl_utils_mutex_count;

char* ic_utils_strdup(const char *src)
{
	char *dest = NULL;

	RETV_IF(NULL == src, NULL);

	dest = strdup(src);
	if (NULL == dest) {
		ERR("strdup() Fail(%d)", errno);
		return NULL;
	}

	return dest;
}

bool ic_utils_check_permission(int permssion)
{
	// TODO: Temporary code (need guide from security team)

	static int has_network_permission = -1;
	static int has_internet_permission = -1;

	if (-1 == has_internet_permission) {
		int ret;
		char smack_label[SMACK_LABEL_LEN + 1] = {0};
		char uid[10];
		FILE *fd;
		cynara *cynara_h;

		ret = cynara_initialize(&cynara_h, NULL);
		if (CYNARA_API_SUCCESS != ret) {
			 ERR("cynara_initialize() Fail(%d)", ret);
			 return false;
		}

		fd = fopen("/proc/self/attr/current", "r");
		if (NULL == fd) {
			 ERR("fopen() Fail(%d)", errno);
			 return false;
		}

		ret = fread(smack_label, sizeof(smack_label), 1, fd);
		fclose(fd);
		if (ret < 0) {
			 ERR("fread() Fail(%d)", ret);
			 return 0;
		}

		snprintf(uid, sizeof(uid), "%d", getuid());

		ret = cynara_check(cynara_h, smack_label, "", uid, IC_PRIVILEGE_INTERNET);
		if (CYNARA_API_ACCESS_ALLOWED == ret)
			has_internet_permission = 1;
		else
			has_internet_permission = 0;

		ret = cynara_check(cynara_h, smack_label, "", uid, IC_PRIVILEGE_NETWORK_GET);
		if (CYNARA_API_ACCESS_ALLOWED == ret)
			has_network_permission = 1;
		else
			has_network_permission = 0;

		cynara_finish(cynara_h);
	}

	if ((IC_PERMISSION_NETWORK_GET & permssion) && (1 != has_network_permission)) {
		ERR("Don't have http://tizen.org/privilege/network.get");
		return false;
	}
	if ((IC_PERMISSION_INTERNET & permssion) && (1 != has_internet_permission)) {
		ERR("Don't have http://tizen.org/privilege/internet");
		return false;
	}

	return true;
}

bool ic_utils_check_oic_feature_supported()
{
#ifdef TZ_VER_3
	if (_ic_oic_feature_supported < 0) {
		bool feature_supported = false;
		system_info_get_platform_bool(IC_FEATURE_OIC, &feature_supported);
		_ic_oic_feature_supported = feature_supported ? 1 : 0;
	}
	return _ic_oic_feature_supported;
#else
	return true;
#endif
}

void ic_utils_free_platform_info(OCPlatformInfo *platform_info)
{
	RET_IF(NULL == platform_info);

	free(platform_info->manufacturerName);
	free(platform_info->manufacturerUrl);
	free(platform_info->modelNumber);
	free(platform_info->dateOfManufacture);
	free(platform_info->platformVersion);
	free(platform_info->operatingSystemVersion);
	free(platform_info->hardwareVersion);
	free(platform_info->firmwareVersion);
	free(platform_info->supportUrl);
	free(platform_info->systemTime);
}

int ic_utils_get_platform_info(OCPlatformInfo *platform_info)
{
	int ret;

	RETV_IF(NULL == platform_info, IOTCON_ERROR_INVALID_PARAMETER);

	/* Mandatory (oic.wk.p) */
	ret = system_info_get_platform_string(IC_SYSTEM_INFO_TIZEN_ID,
			&platform_info->platformID);
	if (SYSTEM_INFO_ERROR_NONE != ret) {
		ERR("system_info_get_platform_string(tizen_id) Fail(%d)", ret);
		ic_utils_free_platform_info(platform_info);
		return IOTCON_ERROR_SYSTEM;
	}

	/* Mandatory (oic.wk.p) */
	ret = system_info_get_platform_string(IC_SYSTEM_INFO_MANUF_NAME,
			&platform_info->manufacturerName);
	if (SYSTEM_INFO_ERROR_NONE != ret) {
		ERR("system_info_get_platform_string(manufacturer) Fail(%d)", ret);
		ic_utils_free_platform_info(platform_info);
		return IOTCON_ERROR_SYSTEM;
	}

	ret = system_info_get_platform_string(IC_SYSTEM_INFO_MODEL_NAME,
			&platform_info->modelNumber);
	if (SYSTEM_INFO_ERROR_NONE != ret)
		WARN("system_info_get_platform_string(model_name) Fail(%d)", ret);

	ret = system_info_get_platform_string(IC_SYSTEM_INFO_PLATFORM_VERSION,
			&platform_info->platformVersion);
	if (SYSTEM_INFO_ERROR_NONE != ret)
		WARN("system_info_get_platform_string(platform_version) Fail(%d)", ret);

	ret = system_info_get_platform_string(IC_SYSTEM_INFO_BUILD_STRING,
			&platform_info->firmwareVersion);
	if (SYSTEM_INFO_ERROR_NONE != ret)
		WARN("system_info_get_platform_string(build_string) Fail(%d)", ret);

	/* platform_info.manufacturerUrl */
	/* platform_info.dateOfManufacture */
	/* platform_info.operatingSystemVersion */
	/* platform_info.hardwareVersion */
	/* platform_info.supportUrl */
	/* platform_info.systemTime */

	return IOTCON_ERROR_NONE;
}


static inline pthread_mutex_t* _utils_mutex_get(int type)
{
	pthread_mutex_t *mutex;

	switch (type) {
	case IC_UTILS_MUTEX_INIT:
		mutex = &icl_utils_mutex_init;
		break;
	case IC_UTILS_MUTEX_IOTY:
		mutex = &icl_utils_mutex_ioty;
		break;
	default:
		ERR("Invalid type(%d)", type);
		mutex = NULL;
	}

	return mutex;
}

void ic_utils_mutex_lock(int type)
{
	int ret;

	icl_utils_mutex_count++;
	if (1 == icl_utils_mutex_count) {
		ret = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &icl_utils_pthread_oldstate);
		WARN_IF(0 != ret, "pthread_setcancelstate() Fail(%d)", ret);
	}

	ret = pthread_mutex_lock(_utils_mutex_get(type));
	WARN_IF(0 != ret, "pthread_mutex_lock() Fail(%d)", ret);
}

void ic_utils_mutex_unlock(int type)
{
	int ret;

	icl_utils_mutex_count--;
	if (0 == icl_utils_mutex_count) {
		ret = pthread_setcancelstate(icl_utils_pthread_oldstate, NULL);
		WARN_IF(0 != ret, "pthread_setcancelstate() Fail(%d)", ret);
	}

	ret = pthread_mutex_unlock(_utils_mutex_get(type));
	WARN_IF(0 != ret, "pthread_mutex_unlock() Fail(%d)", ret);
}
