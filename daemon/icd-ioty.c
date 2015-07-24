/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> /* for uint8_t etc */
#include <stdbool.h>
#include <errno.h>
#include <glib.h>
#include <json-glib/json-glib.h>

#include <octypes.h>
#include <ocstack.h>

#include "iotcon.h"
#include "ic-utils.h"
#include "icd.h"
#include "icd-ioty.h"
#include "icd-ioty-ocprocess.h"

static GMutex icd_csdk_mutex;

void icd_ioty_csdk_lock()
{
	g_mutex_lock(&icd_csdk_mutex);
}


void icd_ioty_csdk_unlock()
{
	g_mutex_unlock(&icd_csdk_mutex);
}


GThread* icd_ioty_init(const char *addr, unsigned short port)
{
	FN_CALL;
	GError *error;
	GThread *thread;

	OCStackResult result = OCInit(addr, port, OC_CLIENT_SERVER);
	if (OC_STACK_OK != result) {
		ERR("OCInit() Fail(%d)", result);
		return NULL;
	}

	DBG("OCInit() Success");

	thread = g_thread_try_new("packet_receive_thread", icd_ioty_ocprocess_thread,
			NULL, &error);
	if (NULL == thread) {
		ERR("g_thread_try_new() Fail(%s)", error->message);
		g_error_free(error);
		return NULL;
	}

	return thread;
}


void icd_ioty_deinit(GThread *thread)
{
	OCStackResult result;

	icd_ioty_ocprocess_stop();
	g_thread_join(thread);

	result = OCStop();
	if (OC_STACK_OK != result)
		ERR("OCStop() Fail(%d)", result);
}


OCResourceHandle icd_ioty_register_resource(const char *uri_path,
		const char* const* res_types, int ifaces, uint8_t properties)
{
	FN_CALL;
	int i;
	OCStackResult ret;
	OCResourceHandle handle;
	const char *resInterface = NULL;

	if (IOTCON_INTERFACE_DEFAULT & ifaces) {
		resInterface = IC_INTERFACE_DEFAULT;
		ifaces ^= IOTCON_INTERFACE_DEFAULT;
	} else if (IOTCON_INTERFACE_LINK & ifaces) {
		resInterface = IC_INTERFACE_LINK;
		ifaces ^= IOTCON_INTERFACE_LINK;
	} else if (IOTCON_INTERFACE_BATCH & ifaces) {
		resInterface = IC_INTERFACE_BATCH;
		ifaces ^= IOTCON_INTERFACE_BATCH;
	} else if (IOTCON_INTERFACE_GROUP & ifaces) {
		resInterface = IC_INTERFACE_GROUP;
		ifaces ^= IOTCON_INTERFACE_GROUP;
	} else {
		ERR("Invalid interface type(%d)", ifaces);
		return NULL;
	}

	icd_ioty_csdk_lock();
	ret = OCCreateResource(&handle, res_types[0], resInterface, uri_path,
			icd_ioty_ocprocess_req_handler, properties);
	icd_ioty_csdk_unlock();
	if (OC_STACK_OK != ret) {
		ERR("OCCreateResource() Fail(%d)", ret);
		return NULL;
	}

	for (i = 1; res_types[i]; i++)
		icd_ioty_bind_type(handle, res_types[i]);

	if (IOTCON_INTERFACE_DEFAULT & ifaces)
		icd_ioty_bind_interface(handle, IOTCON_INTERFACE_DEFAULT);
	if (IOTCON_INTERFACE_LINK & ifaces)
		icd_ioty_bind_interface(handle, IOTCON_INTERFACE_LINK);
	if (IOTCON_INTERFACE_BATCH & ifaces)
		icd_ioty_bind_interface(handle, IOTCON_INTERFACE_BATCH);
	if (IOTCON_INTERFACE_GROUP & ifaces)
		icd_ioty_bind_interface(handle, IOTCON_INTERFACE_GROUP);

	return handle;
}


int icd_ioty_unregister_resource(OCResourceHandle resource_handle)
{
	OCStackResult ret;

	icd_ioty_csdk_lock();
	ret = OCDeleteResource(resource_handle);
	icd_ioty_csdk_unlock();
	if (OC_STACK_OK != ret) {
		ERR("OCDeleteResource() Fail(%d)", ret);
		return IOTCON_ERROR_IOTIVITY;
	}

	return IOTCON_ERROR_NONE;
}


int icd_ioty_bind_interface(OCResourceHandle resourceHandle, iotcon_interface_e iface)
{
	int ret;
	OCStackResult result;
	char *resource_interface;

	ret = ic_utils_convert_interface_flag(iface, &resource_interface);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("ic_utils_convert_interface_flag(%d) Fail(%d)", iface, ret);
		return ret;
	}

	icd_ioty_csdk_lock();
	result = OCBindResourceInterfaceToResource(resourceHandle, resource_interface);
	icd_ioty_csdk_unlock();
	if (OC_STACK_OK != result) {
		ERR("OCBindResourceInterfaceToResource() Fail(%d)", result);
		return IOTCON_ERROR_IOTIVITY;
	}

	return IOTCON_ERROR_NONE;
}


int icd_ioty_bind_type(OCResourceHandle resource_handle, const char *resource_type)
{
	OCStackResult ret;

	icd_ioty_csdk_lock();
	ret = OCBindResourceTypeToResource(resource_handle, resource_type);
	icd_ioty_csdk_unlock();
	if (OC_STACK_OK != ret) {
		ERR("OCBindResourceTypeToResource() Fail(%d)", ret);
		return IOTCON_ERROR_IOTIVITY;
	}

	return IOTCON_ERROR_NONE;
}


int icd_ioty_bind_resource(OCResourceHandle parent, OCResourceHandle child)
{
	OCStackResult ret;

	icd_ioty_csdk_lock();
	ret = OCBindResource(parent, child);
	icd_ioty_csdk_unlock();
	if (OC_STACK_OK != ret) {
		ERR("OCBindResource() Fail(%d)", ret);
		return IOTCON_ERROR_IOTIVITY;
	}

	return IOTCON_ERROR_NONE;
}


int icd_ioty_unbind_resource(OCResourceHandle parent, OCResourceHandle child)
{
	OCStackResult ret;

	icd_ioty_csdk_lock();
	ret = OCUnBindResource(parent, child);
	icd_ioty_csdk_unlock();
	if (OC_STACK_OK != ret) {
		ERR("OCUnBindResource() Fail(%d)", ret);
		return IOTCON_ERROR_IOTIVITY;
	}

	return IOTCON_ERROR_NONE;
}


int icd_ioty_notify_list_of_observers(void *handle, GVariant *msg, GVariant *observers)
{
	int i, error_code, obs_length;
	char *repr_json = NULL;
	GVariantIter obs_iter, msg_iter;
	OCStackResult ret;

	g_variant_iter_init(&obs_iter, observers);
	obs_length = g_variant_iter_n_children(&obs_iter);

	/* Variable-length Array */
	OCObservationId obs_ids[obs_length];

	for (i = 0; i < obs_length; i++)
		g_variant_iter_loop(&obs_iter, "i", &obs_ids[i]);

	g_variant_iter_init(&msg_iter, msg);
	g_variant_iter_loop(&msg_iter, "(i&s)", &error_code, &repr_json);
	/* TODO : How to use error_code. */

	icd_ioty_csdk_lock();
	/* TODO : QoS is come from lib. And user can set QoS to client structure.  */
	ret = OCNotifyListOfObservers(handle, obs_ids, obs_length, repr_json, OC_HIGH_QOS);
	icd_ioty_csdk_unlock();

	if (OC_STACK_NO_OBSERVERS == ret) {
		WARN("No Observers. Stop Notifying");
		return IOTCON_ERROR_NONE;
	} else if (OC_STACK_OK != ret) {
		ERR("OCNotifyListOfObservers() Fail(%d)", ret);
		return IOTCON_ERROR_IOTIVITY;
	}

	return IOTCON_ERROR_NONE;
}


int icd_ioty_notify_all(void *handle)
{
	OCStackResult ret;

	icd_ioty_csdk_lock();
	/* TODO : QoS is come from lib. And user can set QoS to client structure.  */
	ret = OCNotifyAllObservers(handle, OC_HIGH_QOS);
	icd_ioty_csdk_unlock();

	if (OC_STACK_NO_OBSERVERS == ret) {
		WARN("No Observers. Stop Notifying");
		return IOTCON_ERROR_NONE;
	} else if (OC_STACK_OK != ret) {
		ERR("OCNotifyAllObservers() Fail(%d)", ret);
		return IOTCON_ERROR_IOTIVITY;
	}

	return IOTCON_ERROR_NONE;
}


static int _ioty_get_header_options(GVariantIter *src, int src_size,
		OCHeaderOption dest[], int dest_size)
{
	int i = 0;
	char *option_data;
	unsigned short option_id;

	RETV_IF(NULL == dest, IOTCON_ERROR_INVALID_PARAMETER);

	if (dest_size < src_size) {
		ERR("Exceed Size(%d)", src_size);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	while (g_variant_iter_loop(src, "(q&s)", &option_id, &option_data)) {
		dest[i].protocolID = OC_COAP_ID;
		dest[i].optionID = option_id;
		dest[i].optionLength = strlen(option_data) + 1;
		memcpy(dest[i].optionData, option_data, dest[i].optionLength);
		i++;
	}

	return IOTCON_ERROR_NONE;
}


int icd_ioty_send_response(GVariant *resp)
{
	int result, error_code, options_size;
	int request_handle, resource_handle;
	char *new_uri_path, *repr_json;
	GVariantIter *options;
	OCStackResult ret;
	OCEntityHandlerResponse response = {0};

	g_variant_get(resp, "(&sia(qs)i&sii)",
			&new_uri_path,
			&error_code,
			&options,
			&result,
			&repr_json,
			&request_handle,
			&resource_handle);

	response.requestHandle = GINT_TO_POINTER(request_handle);
	response.resourceHandle = GINT_TO_POINTER(resource_handle);
	response.ehResult = (OCEntityHandlerResult)result;

	if (OC_EH_RESOURCE_CREATED == response.ehResult)
		snprintf(response.resourceUri, sizeof(response.resourceUri), "%s", new_uri_path);

	options_size = g_variant_iter_n_children(options);
	response.numSendVendorSpecificHeaderOptions = options_size;

	if (0 != options_size) {
		int ret= _ioty_get_header_options(options,
				response.numSendVendorSpecificHeaderOptions,
				response.sendVendorSpecificHeaderOptions,
				sizeof(response.sendVendorSpecificHeaderOptions)
				/ sizeof(response.sendVendorSpecificHeaderOptions[0]));

		if (IOTCON_ERROR_NONE != ret)
			ERR("_ioty_get_header_options() Fail(%d)", ret);
	}
	g_variant_iter_free(options);

	response.payload = repr_json;
	response.payloadSize = strlen(response.payload) + 1;

	/* related to block transfer */
	response.persistentBufferFlag = 0;

	icd_ioty_csdk_lock();
	ret = OCDoResponse(&response);
	icd_ioty_csdk_unlock();

	if (OC_STACK_OK != ret) {
		ERR("OCDoResponse() Fail(%d)", ret);
		return IOTCON_ERROR_IOTIVITY;
	}

	return IOTCON_ERROR_NONE;
}


static void _ioty_free_signal_context(void *data)
{
	icd_sig_ctx_s *context = data;
	free(context->sender);
	free(context);
}

int icd_ioty_find_resource(const char *host_address, const char *resource_type,
		unsigned int signum, const char *sender)
{
	int len;
	OCStackResult result;
	OCCallbackData cbdata = {0};
	icd_sig_ctx_s *context;
	char uri[PATH_MAX] = {0};
	iotcon_connectivity_type_e conn_type = IOTCON_CONNECTIVITY_IPV4;

	if (IC_STR_EQUAL == strcmp(IOTCON_MULTICAST_ADDRESS, host_address)) {
		len = snprintf(uri, sizeof(uri), "%s", OC_MULTICAST_DISCOVERY_URI);
		conn_type = IOTCON_CONNECTIVITY_ALL;
	} else {
		len = snprintf(uri, sizeof(uri), ICD_IOTY_COAP"%s%s", host_address,
				OC_MULTICAST_DISCOVERY_URI);
	}
	if (len <= 0 || sizeof(uri) <= len) {
		ERR("snprintf() Fail(%d)", len);
		return IOTCON_ERROR_UNKNOWN;
	}

	if (IC_STR_EQUAL != strcmp(IC_STR_NULL, resource_type))
		snprintf(uri + len, sizeof(uri), "?rt=%s", resource_type);

	context = calloc(1, sizeof(icd_sig_ctx_s));
	if (NULL == context) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	context->sender = ic_utils_strdup(sender);
	context->signum = signum;

	cbdata.context = context;
	cbdata.cb = icd_ioty_ocprocess_find_cb;
	cbdata.cd = _ioty_free_signal_context;

	icd_ioty_csdk_lock();
	result = OCDoResource(NULL, OC_REST_GET, uri, NULL, NULL, conn_type, OC_LOW_QOS,
			&cbdata, NULL, 0);
	icd_ioty_csdk_unlock();

	if (OC_STACK_OK != result) {
		ERR("OCDoResource() Fail(%d)", result);
		free(context->sender);
		free(context);
		return IOTCON_ERROR_IOTIVITY;
	}

	return IOTCON_ERROR_NONE;
}


/*
 * returned string SHOULD be released by you
 */
static char* _icd_ioty_resource_generate_uri(char *host, char *uri_path, GVariant *query)
{
	int len;
	bool loop_first = true;
	char *key, *value;
	GVariantIter *queryIter;
	char uri_buf[PATH_MAX] = {0};

	len = snprintf(uri_buf, sizeof(uri_buf), "%s%s", host, uri_path);

	/* remove suffix '/' */
	if ('/' == uri_buf[strlen(uri_buf) - 1]) {
		uri_buf[strlen(uri_buf) - 1] = '\0';
		len--;
	}

	g_variant_get(query, "a(ss)", &queryIter);

	while (g_variant_iter_loop(queryIter, "(&s&s)", &key, &value)) {
		int query_len;

		DBG("query exist. key(%s), value(%s)", key, value);

		if (true == loop_first) {
			query_len = snprintf(uri_buf + len, sizeof(uri_buf), "?%s=%s", key, value);
			loop_first = false;
		} else {
			query_len = snprintf(uri_buf + len, sizeof(uri_buf), "&%s=%s", key, value);
		}

		len += query_len;
	}
	g_variant_iter_free(queryIter);

	return strdup(uri_buf);
}


int icd_ioty_get(GVariant *resource, GVariant *query, unsigned int signal_number,
		const char *sender)
{
	FN_CALL;
	int conn_type, options_size;
	char *uri_path, *host, *uri;
	OCStackResult result;
	GVariantIter *options;
	OCCallbackData cbdata = {0};
	icd_sig_ctx_s *context;
	int is_observable, ifaces, observe_handle;
	OCHeaderOption oic_options[MAX_HEADER_OPTIONS];

	g_variant_get(resource, "(&s&sba(qs)iii)", &uri_path, &host, &is_observable, &options,
			&ifaces, &observe_handle, &conn_type);

	uri = _icd_ioty_resource_generate_uri(host, uri_path, query);
	if (NULL == uri) {
		ERR("_icd_ioty_resource_generate_uri() Fail");
		g_variant_iter_free(options);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	context = calloc(1, sizeof(icd_sig_ctx_s));
	if (NULL == context) {
		ERR("calloc() Fail(%d)", errno);
		g_variant_iter_free(options);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}
	context->sender = ic_utils_strdup(sender);
	context->signum = signal_number;

	cbdata.context = context;
	cbdata.cb = icd_ioty_ocprocess_get_cb;
	cbdata.cd = _ioty_free_signal_context;

	options_size = g_variant_iter_n_children(options);
	if (0 != options_size) {
		int ret = _ioty_get_header_options(options, options_size, oic_options,
				sizeof(oic_options) / sizeof(oic_options[0]));
		if (IOTCON_ERROR_NONE != ret) {
			ERR("_ioty_get_header_options() Fail(%d)", ret);
			free(context->sender);
			free(context);
			free(uri);
			g_variant_iter_free(options);
			return ret;
		}
	}
	g_variant_iter_free(options);

	icd_ioty_csdk_lock();
	/* TODO : QoS is come from lib. And user can set QoS to client structure.  */
	result = OCDoResource(NULL, OC_REST_GET, uri, NULL, NULL, conn_type, OC_HIGH_QOS,
			&cbdata, options_size?oic_options:NULL, options_size);
	icd_ioty_csdk_unlock();

	free(uri);

	if (OC_STACK_OK != result) {
		ERR("OCDoResource() Fail(%d)", result);
		free(context->sender);
		free(context);
		return IOTCON_ERROR_IOTIVITY;
	}

	return IOTCON_ERROR_NONE;
}


int icd_ioty_put(GVariant *resource, const char *repr, GVariant *query,
		unsigned int signal_number, const char *sender)
{
	// TODO : To be implemented
	return IOTCON_ERROR_NONE;
}


int icd_ioty_post(GVariant *resource, const char *repr, GVariant *query,
		unsigned int signal_number, const char *sender)
{
	// TODO : To be implemented
	return IOTCON_ERROR_NONE;
}


int icd_ioty_delete(GVariant *resource, unsigned int signal_number,
		const char *sender)
{
	// TODO : To be implemented
	return IOTCON_ERROR_NONE;
}


int icd_ioty_observer_start(GVariant *resource, int observe_type,
		GVariant *query, unsigned int signal_number, const char *sender, int *observe_h)
{
	// TODO : To be implemented
	return IOTCON_ERROR_NONE;
}


int icd_ioty_observer_stop(void *observe_h)
{
	// TODO : To be implemented
	return IOTCON_ERROR_NONE;
}


#ifdef DEVICE_INFO_IMPL /* not implemented in iotivity 0.9.1 */
int icd_ioty_register_device_info(GVariant *value)
{
	// TODO : To be implemented
	return IOTCON_ERROR_NONE;
}


int icd_ioty_get_device_info(const char *host_address,
		unsigned int signal_number, const char *sender)
{
	// TODO : To be implemented
	return IOTCON_ERROR_NONE;
}
#endif


int icd_ioty_register_platform_info(GVariant *value)
{
	// TODO : To be implemented
	return IOTCON_ERROR_NONE;
}


int icd_ioty_get_platform_info(const char *host_address, unsigned int signal_number,
		const char *sender)
{
	// TODO : To be implemented
	return IOTCON_ERROR_NONE;
}


OCDoHandle icd_ioty_subscribe_presence(const char *host_address,
		const char *resource_type, unsigned int signal_number, const char *sender)
{
	// TODO : To be implemented
	return NULL;
}


int icd_ioty_unsubscribe_presence(OCDoHandle presence_handle)
{
	// TODO : To be implemented
	return IOTCON_ERROR_NONE;
}


int icd_ioty_start_presence(unsigned int time_to_live)
{
	// TODO : To be implemented
	return IOTCON_ERROR_NONE;
}


int icd_ioty_stop_presence()
{
	// TODO : To be implemented
	return IOTCON_ERROR_NONE;
}
