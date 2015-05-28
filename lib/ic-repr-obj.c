/* Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <glib.h>

#include "iotcon-struct.h"
#include "iotcon-constant.h"
#include "iotcon-representation.h"
#include "ic-common.h"
#include "ic-utils.h"
#include "ic-repr-list.h"
#include "ic-repr-value.h"
#include "ic-repr.h"
#include "ic-repr-obj.h"

int ic_obj_del_value(iotcon_repr_h repr, const char *key,
		iotcon_repr_types_e value_type)
{
	gboolean ret = FALSE;
	iotcon_value_h value = NULL;

	RETV_IF(NULL == repr, false);
	RETV_IF(NULL == key, false);

	value = g_hash_table_lookup(repr->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup(%s) Fail", key);
		return IOTCON_ERROR_NO_DATA;
	}

	if (value_type != value->type) {
		ERR("Type matching Fail(input:%d, saved:%d)", value_type, value->type);
		return IOTCON_ERROR_PARAM;
	}

	ret = g_hash_table_remove(repr->hash_table, key);
	if (FALSE == ret) {
		ERR("g_hash_table_remove(%s) Fail", key);
		return IOTCON_ERROR_NO_DATA;
	}

	return IOTCON_ERROR_NONE;
}

API int iotcon_repr_get_int(iotcon_repr_h repr, const char *key)
{
	iotcon_value_h value;

	RETV_IF(NULL == repr, 0);
	RETV_IF(NULL == key, 0);

	value = g_hash_table_lookup(repr->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return 0;
	}

	ic_basic_s *real = (ic_basic_s*)value;
	RETV_IF(IOTCON_TYPE_INT != real->type, 0);

	return real->val.i;
}

API int iotcon_repr_set_int(iotcon_repr_h repr, const char *key, int val)
{
	iotcon_value_h value;

	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key, IOTCON_ERROR_PARAM);

	value = ic_value_new_int(val);
	if (NULL == value) {
		ERR("ic_value_new_int(%d) Fail", val);
		return IOTCON_ERROR_MEMORY;
	}

	g_hash_table_replace(repr->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_repr_del_int(iotcon_repr_h repr, const char *key)
{
	int ret;

	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key, IOTCON_ERROR_PARAM);

	ret = ic_obj_del_value(repr, key, IOTCON_TYPE_INT);
	if (IOTCON_ERROR_NONE != ret)
		ERR("ic_obj_del_value() Fail(%d)", ret);

	return ret;
}

API bool iotcon_repr_get_bool(iotcon_repr_h repr, const char *key)
{
	ic_basic_s *real = NULL;
	iotcon_value_h value = NULL;

	RETV_IF(NULL == repr, false);
	RETV_IF(NULL == key, false);

	value = g_hash_table_lookup(repr->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return false;
	}

	real = (ic_basic_s*)value;
	RETV_IF(IOTCON_TYPE_BOOL != real->type, false);

	return real->val.b;
}

API int iotcon_repr_set_bool(iotcon_repr_h repr, const char *key, bool val)
{
	iotcon_value_h value = NULL;

	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key, IOTCON_ERROR_PARAM);

	value = ic_value_new_bool(val);
	if (NULL == value) {
		ERR("ic_value_new_bool(%d) Fail", val);
		return IOTCON_ERROR_MEMORY;
	}

	g_hash_table_replace(repr->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_repr_del_bool(iotcon_repr_h repr, const char *key)
{
	int ret;

	ret = ic_obj_del_value(repr, key, IOTCON_TYPE_BOOL);
	if (IOTCON_ERROR_NONE != ret)
		ERR("ic_obj_del_value() Fail(%d)", ret);

	return ret;
}

API double iotcon_repr_get_double(iotcon_repr_h repr, const char *key)
{
	ic_basic_s *real = NULL;
	iotcon_value_h value = NULL;

	RETV_IF(NULL == repr, 0.0);
	RETV_IF(NULL == key, 0.0);

	value = g_hash_table_lookup(repr->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return 0;
	}

	real = (ic_basic_s*)value;
	RETV_IF(IOTCON_TYPE_DOUBLE != real->type, 0.0);

	return real->val.d;
}

API int iotcon_repr_set_double(iotcon_repr_h repr, const char *key, double val)
{
	iotcon_value_h value = NULL;

	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key, IOTCON_ERROR_PARAM);

	value = ic_value_new_double(val);
	if (NULL == value) {
		ERR("ic_value_new_double(%f) Fail", val);
		return IOTCON_ERROR_MEMORY;
	}

	g_hash_table_replace(repr->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_repr_del_double(iotcon_repr_h repr, const char *key)
{
	int ret;

	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key, IOTCON_ERROR_PARAM);

	ret = ic_obj_del_value(repr, key, IOTCON_TYPE_DOUBLE);
	if (IOTCON_ERROR_NONE != ret)
		ERR("ic_obj_del_value() Fail(%d)", ret);

	return ret;
}

API char* iotcon_repr_get_str(iotcon_repr_h repr, const char *key)
{
	ic_basic_s *real = NULL;
	iotcon_value_h value = NULL;

	RETV_IF(NULL == repr, NULL);
	RETV_IF(NULL == key, NULL);

	value = g_hash_table_lookup(repr->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return NULL;
	}

	real = (ic_basic_s*)value;
	RETV_IF(IOTCON_TYPE_STR != real->type, NULL);

	return real->val.s;
}

API int iotcon_repr_set_str(iotcon_repr_h repr, const char *key, char *val)
{
	iotcon_value_h value = NULL;

	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key, IOTCON_ERROR_PARAM);

	value = ic_value_new_str(val);
	if (NULL == value) {
		ERR("ic_value_new_str(%s) Fail", val);
		return IOTCON_ERROR_MEMORY;
	}

	g_hash_table_replace(repr->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_repr_del_str(iotcon_repr_h repr, const char *key)
{
	int ret;

	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key, IOTCON_ERROR_PARAM);

	ret = ic_obj_del_value(repr, key, IOTCON_TYPE_STR);
	if (IOTCON_ERROR_NONE != ret)
		ERR("ic_obj_del_value() Fail(%d)", ret);

	return ret;
}

API bool iotcon_repr_is_null(iotcon_repr_h repr, const char *key)
{
	ic_basic_s *real = NULL;
	iotcon_value_h value = NULL;

	RETV_IF(NULL == repr, false);
	RETV_IF(NULL == key, false);

	value = (iotcon_value_h) g_hash_table_lookup(repr->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return false;
	}

	real = (ic_basic_s*)value;

	return (IOTCON_TYPE_NULL == real->type) ? true : false;
}

API int iotcon_repr_set_null(iotcon_repr_h repr, const char *key)
{
	iotcon_value_h value = NULL;

	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key, IOTCON_ERROR_PARAM);

	value = ic_value_new_null();
	if (NULL == value) {
		ERR("ic_value_new_null() Fail");
		return IOTCON_ERROR_MEMORY;
	}

	g_hash_table_replace(repr->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_repr_del_null(iotcon_repr_h repr, const char *key)
{
	int ret;

	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key, IOTCON_ERROR_PARAM);

	ret = ic_obj_del_value(repr, key, IOTCON_TYPE_NULL);
	if (IOTCON_ERROR_NONE != ret)
		ERR("ic_obj_del_value() Fail(%d)", ret);

	return ret;
}

API iotcon_list_h iotcon_repr_get_list(iotcon_repr_h repr, const char *key)
{
	iotcon_value_h value = NULL;
	ic_val_list_s *real = NULL;

	RETV_IF(NULL == repr, NULL);
	RETV_IF(NULL == key, NULL);

	value = g_hash_table_lookup(repr->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return NULL;
	}

	real = (ic_val_list_s*)value;
	RETV_IF(IOTCON_TYPE_LIST != real->type, NULL);

	return real->list;
}

API int iotcon_repr_set_list(iotcon_repr_h repr, const char *key, iotcon_list_h list)
{
	FN_CALL;
	iotcon_value_h value = NULL;

	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key, IOTCON_ERROR_PARAM);

	value = ic_value_new_list(list);
	if (NULL == value) {
		ERR("ic_value_new_list() Fail");
		return IOTCON_ERROR_MEMORY;
	}

	g_hash_table_replace(repr->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_repr_del_list(iotcon_repr_h repr, const char *key)
{
	int ret;

	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key, IOTCON_ERROR_PARAM);

	ret = ic_obj_del_value(repr, key, IOTCON_TYPE_LIST);
	if (IOTCON_ERROR_NONE != ret)
		ERR("ic_obj_del_value() Fail(%d)", ret);

	return ret;
}

API iotcon_repr_h iotcon_repr_get_repr(iotcon_repr_h repr, const char *key)
{
	ic_val_repr_s *real = NULL;
	iotcon_value_h value = NULL;

	RETV_IF(NULL == repr, NULL);
	RETV_IF(NULL == key, NULL);

	value = g_hash_table_lookup(repr->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return NULL;
	}

	real = (ic_val_repr_s*)value;
	RETV_IF(IOTCON_TYPE_REPR != real->type, NULL);

	return real->repr;
}

API int iotcon_repr_set_repr(iotcon_repr_h repr, const char *key, iotcon_repr_h val)
{
	iotcon_value_h value = NULL;

	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == val, IOTCON_ERROR_PARAM);

	value = ic_value_new_repr(val);
	if (NULL == value) {
		ERR("ic_value_new_repr(%p) Fail", val);
		return IOTCON_ERROR_MEMORY;
	}

	g_hash_table_replace(repr->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_repr_del_repr(iotcon_repr_h repr, const char *key)
{
	int ret;

	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key, IOTCON_ERROR_PARAM);

	ret = ic_obj_del_value(repr, key, IOTCON_TYPE_REPR);
	if (IOTCON_ERROR_NONE != ret)
		ERR("ic_obj_del_value() Fail(%d)", ret);

	return ret;
}

int ic_obj_set_value(iotcon_repr_h repr, const char *key, iotcon_value_h value)
{
	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == value, IOTCON_ERROR_PARAM);

	g_hash_table_replace(repr->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

static inline int _ic_obj_to_json(GHashTable *hash, iotcon_str_list_s *key_list,
		unsigned int index, JsonObject *json_obj)
{
	FN_CALL;
	int type;
	const char *key;
	iotcon_repr_h child_repr = NULL;
	iotcon_list_h child_list = NULL;
	iotcon_value_h value = NULL;

	JsonObject *child_obj = NULL;
	JsonNode *child_node = NULL;
	JsonArray *child_array = NULL;

	RETV_IF(NULL == hash, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key_list, IOTCON_ERROR_PARAM);
	RETV_IF(index < 0, IOTCON_ERROR_PARAM);

	key = iotcon_str_list_nth_data(key_list, index);
	value = g_hash_table_lookup(hash, key);
	if (NULL == value)
		ERR("g_hash_table_lookup(%s) Fail", key);

	type = value->type;
	switch (type) {
	case IOTCON_TYPE_INT:
	case IOTCON_TYPE_BOOL:
	case IOTCON_TYPE_DOUBLE:
	case IOTCON_TYPE_STR:
	case IOTCON_TYPE_NULL:
		child_node = ic_value_to_json(value);
		if (NULL == child_node) {
			ERR("ic_value_to_json() Fail");
			return IOTCON_ERROR_PARAM;
		}
		json_object_set_member(json_obj, key, child_node);
		break;
	case IOTCON_TYPE_LIST:
		child_list = ic_value_get_list(value);
		child_array = ic_list_to_json(child_list);
		if (NULL == child_array) {
			ERR("ic_list_to_json() Fail");
			return IOTCON_ERROR_PARAM;
		}
		child_node = json_node_new(JSON_NODE_ARRAY);
		json_node_set_array(child_node, child_array);
		json_object_set_member(json_obj, key, child_node);
		break;
	case IOTCON_TYPE_REPR:
		child_repr = ic_value_get_repr(value);
		child_obj = ic_obj_to_json(child_repr);
		if (NULL == child_obj) {
			ERR("ic_obj_to_json() Fail");
			return IOTCON_ERROR_PARAM;
		}
		child_node = json_node_new(JSON_NODE_OBJECT);
		json_node_set_object(child_node, child_obj);
		json_object_set_member(json_obj, key, child_node);
		break;
	default:
		ERR("Invalid type(%d)", type);
		return IOTCON_ERROR_PARAM;
	}

	return IOTCON_ERROR_NONE;
}

/*
* A general result : {"rep":{"string":"Hello","intlist":[1,2,3]}}
*/
JsonObject* ic_obj_to_json(iotcon_repr_h repr)
{
	FN_CALL;
	int ret;
	unsigned int i = 0;
	iotcon_str_list_s *key_list = NULL;
	JsonObject *json_obj = NULL;
	JsonObject *parent_obj;

	RETV_IF(NULL == repr, NULL);

	key_list = iotcon_repr_get_key_list(repr);
	if (key_list) {
		json_obj = json_object_new();
		for (i = 0; i < iotcon_str_list_length(key_list); i++) {
			ret = _ic_obj_to_json(repr->hash_table, key_list, i, json_obj);

			if (IOTCON_ERROR_NONE != ret) {
				ERR("_ic_obj_to_json() Fail(%d)", ret);
				json_object_unref(json_obj);
				iotcon_str_list_free(key_list);
				return NULL;
			}
		}
		iotcon_str_list_free(key_list);
	}

	parent_obj = json_object_new();

	if (json_obj)
		json_object_set_object_member(parent_obj, IOTCON_KEY_REP, json_obj);

	return parent_obj;
}

static inline int _ic_obj_from_json(JsonObject *obj, GList *key_list, unsigned int index,
		iotcon_repr_h repr)
{
	char *key;

	RETV_IF(NULL == obj, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == key_list, IOTCON_ERROR_PARAM);
	RETV_IF(index < 0, IOTCON_ERROR_PARAM);
	RETV_IF(NULL == repr, IOTCON_ERROR_PARAM);

	key = g_list_nth_data(key_list, index);

	/* search child object recursively */
	JsonNode *child_node = json_object_get_member(obj, key);
	if (NULL == child_node) {
		ERR("json_object_get_member() Fail(NULL == child_node)");
		return IOTCON_ERROR_PARAM;
	}
	if (JSON_NODE_HOLDS_NULL(child_node) || JSON_NODE_HOLDS_VALUE(child_node)) {
		iotcon_value_h value = ic_value_from_json(child_node);
		if (NULL == value) {
			ERR("ic_value_from_json() Fail(NULL == value)");
			return IOTCON_ERROR_PARAM;
		}
		ic_obj_set_value(repr, key, value);
	}
	else if (JSON_NODE_HOLDS_ARRAY(child_node)) {
		JsonArray *child_array = json_node_get_array(child_node);
		iotcon_list_h list = ic_list_from_json(child_array);
		if (NULL == list) {
			ERR("ic_list_from_json() Fail(NULL == list)");
			return IOTCON_ERROR_PARAM;
		}
		iotcon_repr_set_list(repr, key, list);
	}
	else if (JSON_NODE_HOLDS_OBJECT(child_node)) {
		JsonObject *child_obj = json_node_get_object(child_node);
		iotcon_repr_h ret_repr = ic_obj_from_json(child_obj);
		if (NULL == ret_repr) {
			ERR("ic_obj_from_json() Fail(NULL == ret_repr)");
			return IOTCON_ERROR_PARAM;
		}
		iotcon_repr_set_repr(repr, key, ret_repr);
	}
	else {
		ERR("node type(%d) Fail", json_node_get_node_type(child_node));
		return IOTCON_ERROR_PARAM;
	}

	return IOTCON_ERROR_NONE;
}

/*
* A general input : {"rep:"{"string":"Hello","intlist":[1,2,3]}}
*/
iotcon_repr_h ic_obj_from_json(JsonObject *json_repr)
{
	int ret;
	unsigned int i = 0;
	GList *key_list = NULL;
	iotcon_repr_h repr = NULL;
	JsonObject *obj = NULL;

	RETV_IF(NULL == json_repr, NULL);

	obj = json_object_get_object_member(json_repr, IOTCON_KEY_REP);

	key_list = json_object_get_members(obj);

	repr = iotcon_repr_new();
	if (key_list) {
		for (i = 0; i < g_list_length(key_list); i++) {
			ret = _ic_obj_from_json(obj, key_list, i, repr);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("_ic_obj_from_json() Fail(%d)", ret);
				iotcon_repr_free(repr);
				g_list_free(key_list);
				return NULL;
			}
		}
		g_list_free(key_list);
	}

	return repr;
}

