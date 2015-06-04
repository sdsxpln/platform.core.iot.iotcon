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
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib.h>
#include <glib-object.h>

#include "iotcon.h"
#include "icl-utils.h"
#include "icl-resource-types.h"
#include "icl-ioty.h"
#include "icl-repr.h"
#include "icl.h"

/**
 * @brief global context
 */
static GHashTable *ic_request_cb_hash;
static bool ic_is_init = false;

static void _free_resource(gpointer data)
{
	int ret;
	iotcon_resource_h resource = data;

	RET_IF(NULL == data);

	ret = ic_ioty_unregister_res(resource->handle);
	if (IOTCON_ERROR_NONE != ret)
		ERR("ic_ioty_unregister_res() Fail(%d)", ret);

	free(resource);
}

/* Host address should begin with "coap://" */
API void iotcon_initialize(const char *addr, unsigned short port)
{
	FN_CALL;

	RETM_IF(true == ic_is_init, "already initialized");
	RET_IF(NULL == addr);

	ic_ioty_config(addr, port);
	ic_request_cb_hash = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL,
			_free_resource);

#if !GLIB_CHECK_VERSION(2,35,0)
	g_type_init();
#endif
	ic_is_init = true;
}


API void iotcon_deinitialize()
{
	FN_CALL;

	RETM_IF(false == ic_is_init, "Not initialized");

	g_hash_table_destroy(ic_request_cb_hash);
	ic_request_cb_hash = NULL;

	ic_is_init = false;
}


static gboolean _find_valid_resource(gpointer key, gpointer value, gpointer user_data)
{
	return (key == user_data);
}


iotcon_resource_h ic_get_resource_handler_data(void *handle)
{
	return g_hash_table_find(ic_request_cb_hash, _find_valid_resource, handle);
}


/* The length of uri should be less than or equal to 36. */
API iotcon_resource_h iotcon_register_resource(const char *uri,
		iotcon_resource_types_h res_types,
		int ifaces,
		uint8_t properties,
		iotcon_request_handler_cb cb,
		void *user_data)
{
	FN_CALL;
	iotcon_resource_h resource;

	RETV_IF(NULL == uri, NULL);
	RETVM_IF(IOTCON_URI_LENGTH_MAX < strlen(uri), NULL,
			"The length of uri(%s) is invalid", uri);
	RETV_IF(NULL == res_types, NULL);
	RETV_IF(NULL == cb, NULL);

	resource = calloc(1, sizeof(struct ic_resource));
	if (NULL == resource) {
		ERR("calloc Fail(%d)", errno);
		return NULL;
	}

	resource->handle = ic_ioty_register_res(uri, res_types, ifaces, properties);
	if (NULL == resource->handle) {
		ERR("ic_ioty_register_res() Fail");
		free(resource);
		return NULL;
	}

	resource->cb = cb;
	resource->user_data = user_data;

	resource->uri = ic_utils_strdup(uri);
	resource->types = ic_resource_types_ref(res_types);
	resource->ifaces = ifaces;
	resource->is_observable = properties & IOTCON_OBSERVABLE;

	g_hash_table_insert(ic_request_cb_hash, resource->handle, resource);

	return resource;
}


API void iotcon_unregister_resource(iotcon_resource_h resource)
{
	FN_CALL;

	RET_IF(NULL == resource);

	g_hash_table_remove(ic_request_cb_hash, resource->handle);
}


API int iotcon_bind_interface(iotcon_resource_h resource, iotcon_interface_e iface)
{
	FN_CALL;
	int ret;

	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);

	ret = ic_ioty_bind_iface_to_res(resource->handle, iface);
	if (IOTCON_ERROR_NONE != ret)
		ERR("ic_ioty_bind_iface_to_res() Fail(%d)", ret);

	return ret;
}


API int iotcon_bind_type(iotcon_resource_h resource, const char *resource_type)
{
	FN_CALL;
	int ret;

	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == resource_type, IOTCON_ERROR_INVALID_PARAMETER);
	if (IOTCON_RESOURCE_TYPE_LENGTH_MAX < strlen(resource_type)) {
		ERR("The length of resource_type(%s) is invalid", resource_type);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	ret = ic_ioty_bind_type_to_res(resource->handle, resource_type);
	if (IOTCON_ERROR_NONE != ret)
		ERR("ic_ioty_bind_type_to_res() Fail(%d)", ret);

	return ret;
}


API int iotcon_bind_request_handler(iotcon_resource_h resource,
		iotcon_request_handler_cb cb)
{
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	WARN("Request handler is changed");
	resource->cb = cb;

	return IOTCON_ERROR_NONE;
}


API int iotcon_bind_resource(iotcon_resource_h parent, iotcon_resource_h child)
{
	FN_CALL;
	int ret;
	int i;

	RETV_IF(NULL == parent, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == child, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(parent == child, IOTCON_ERROR_INVALID_PARAMETER);

	for (i = 0; i < IOTCON_CONTAINED_RESOURCES_MAX; i++) {
		if (child == parent->children[i]) {
			ERR("Child resource was already bound to parent resource.");
			return IOTCON_ERROR_ALREADY;
		}
		if (NULL == parent->children[i]) {
			ret = ic_ioty_bind_res(parent->handle, child->handle);
			if (IOTCON_ERROR_NONE == ret)
				parent->children[i] = child;
			else
				ERR("ic_ioty_bind_res() Fail(%d)", ret);

			return ret;
		}
	}

	ERR("There is no slot to bind a child resource");
	return IOTCON_ERROR_OUT_OF_MEMORY;
}


API int iotcon_unbind_resource(iotcon_resource_h parent, iotcon_resource_h child)
{
	int ret;
	int i;

	RETV_IF(NULL == parent, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == child, IOTCON_ERROR_INVALID_PARAMETER);

	ret = ic_ioty_unbind_res(parent->handle, child->handle);
	if (IOTCON_ERROR_NONE == ret) {
		for (i = 0; i < IOTCON_CONTAINED_RESOURCES_MAX; i++) {
			if (child == parent->children[i])
				parent->children[i] = NULL;
		}
	}
	else
		ERR("ic_ioty_unbind_res() Fail(%d)", ret);

	return ret;
}


API int iotcon_resource_get_number_of_children(iotcon_resource_h resource, int *number)
{
	int i;

	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == number, IOTCON_ERROR_INVALID_PARAMETER);

	*number = 0;
	for (i = 0; i < IOTCON_CONTAINED_RESOURCES_MAX; i++) {
		if (resource->children[i])
			*number += 1;
	}

	return IOTCON_ERROR_NONE;
}


API int iotcon_resource_get_nth_child(iotcon_resource_h parent, int index,
		iotcon_resource_h *child)
{
	RETV_IF(NULL == parent, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == child, IOTCON_ERROR_INVALID_PARAMETER);
	if ((index < 0) || (IOTCON_CONTAINED_RESOURCES_MAX <= index)) {
		ERR("Invalid index(%d)", index);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	*child = parent->children[index];

	return IOTCON_ERROR_NONE;
}


/* The content of the resource should not be freed by user. */
API int iotcon_resource_get_uri(iotcon_resource_h resource, char **uri)
{
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == uri, IOTCON_ERROR_INVALID_PARAMETER);

	*uri = resource->uri;

	return IOTCON_ERROR_NONE;
}


/* The content of the resource should not be freed by user. */
API int iotcon_resource_get_host(iotcon_resource_h resource, char **host)
{
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == host, IOTCON_ERROR_INVALID_PARAMETER);

	*host = resource->host;

	return IOTCON_ERROR_NONE;
}


/* The content of the resource should not be freed by user. */
API int iotcon_resource_get_types(iotcon_resource_h resource, iotcon_resource_types_h *types)
{
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == types, IOTCON_ERROR_INVALID_PARAMETER);

	*types = resource->types;

	return IOTCON_ERROR_NONE;
}


API int iotcon_resource_get_interfaces(iotcon_resource_h resource, int *ifaces)
{
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == ifaces, IOTCON_ERROR_INVALID_PARAMETER);

	*ifaces = resource->ifaces;

	return IOTCON_ERROR_NONE;
}


API int iotcon_resource_is_observable(iotcon_resource_h resource, bool *observable)
{
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == observable, IOTCON_ERROR_INVALID_PARAMETER);

	*observable = resource->is_observable;

	return IOTCON_ERROR_NONE;
}


API int iotcon_start_presence(unsigned int time_to_live)
{
	FN_CALL;
	int ret;

	ret = ic_ioty_start_presence(time_to_live);
	if (IOTCON_ERROR_NONE != ret)
		ERR("ic_ioty_start_presence() Fail(%d)", ret);

	return ret;
}


API int iotcon_stop_presence()
{
	FN_CALL;
	int ret;

	ret = ic_ioty_stop_presence();
	if (IOTCON_ERROR_NONE != ret)
		ERR("ic_ioty_stop_presence() Fail(%d)", ret);

	return ret;
}


/* The length of resource_type should be less than or equal to 61. */
API iotcon_presence_h iotcon_subscribe_presence(const char *host_address,
		const char *resource_type, iotcon_presence_cb cb, void *user_data)
{
	iotcon_presence_h handle;

	RETV_IF(NULL == host_address, NULL);
	RETV_IF(NULL == cb, NULL);
	if (resource_type &&(IOTCON_RESOURCE_TYPE_LENGTH_MAX < strlen(resource_type))) {
		ERR("The length of resource_type(%s) is invalid", resource_type);
		return NULL;
	}

	if (NULL == resource_type)
		resource_type = "";

	handle = ic_ioty_subscribe_presence(host_address, resource_type, cb, user_data);
	if (NULL == handle)
		ERR("ic_ioty_subscribe_presence() Fail");

	return handle;
}


API int iotcon_unsubscribe_presence(iotcon_presence_h handle)
{
	FN_CALL;
	int ret;

	RETV_IF(NULL == handle, IOTCON_ERROR_INVALID_PARAMETER);

	ret = ic_ioty_unsubscribe_presence(handle);
	if (IOTCON_ERROR_NONE != ret)
		ERR("ic_ioty_unsubscribe_presence() Fail(%d)", ret);

	return ret;
}


API iotcon_notimsg_h iotcon_notimsg_new(iotcon_repr_h repr, iotcon_interface_e iface)
{
	iotcon_notimsg_h msg;

	RETV_IF(NULL == repr, NULL);

	msg = calloc(1, sizeof(struct ic_notify_msg));
	if (NULL == msg) {
		ERR("calloc() Fail(%d)", errno);
		return NULL;
	}

	msg->repr = repr;
	msg->iface = iface;
	msg->error_code = 200;

	return msg;
}


API void iotcon_notimsg_free(iotcon_notimsg_h msg)
{
	RET_IF(NULL == msg);

	iotcon_repr_free(msg->repr);
	free(msg);
}


API int iotcon_notify(iotcon_resource_h resource, iotcon_notimsg_h msg,
		iotcon_observers_h observers)
{
	int ret;

	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == observers, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == msg, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == msg->repr, IOTCON_ERROR_INVALID_PARAMETER);

	ret = ic_ioty_send_notify(resource->handle, msg, observers);
	if (IOTCON_ERROR_NONE != ret)
		ERR("ic_ioty_send_notify() Fail(%d)", ret);

	return ret;
}