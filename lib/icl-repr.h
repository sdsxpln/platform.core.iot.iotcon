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
#ifndef __IOT_CONNECTIVITY_MANAGER_LIBRARY_REPRESENTATION_H__
#define __IOT_CONNECTIVITY_MANAGER_LIBRARY_REPRESENTATION_H__

#include <stdbool.h>
#include <glib.h>
#include <json-glib/json-glib.h>

#include "iotcon-struct.h"

struct icl_repr_s {
	char *uri_path;
	int ref_count;
	int interfaces;
	GHashTable *hash_table;
	GList *children;
	iotcon_resource_types_h res_types;
};

/**
 * @ingroup CAPI_IOT_CONNECTIVITY_MODULE
 * @brief Generates JSON string from Representation.
 * @since_tizen 3.0
 * @remarks The returned string must be released with free() by you.
 *
 * @param[in] repr The handle to the Representation
 * @param[in] set_pretty Whether the generated JSON string should be pretty printed
 *
 * @return Generated JSON string, otherwise a null pointer if a parse error
 */
char* icl_repr_generate_json(iotcon_repr_h repr, bool set_pretty);

char* icl_repr_json_get_uri_path(const char *json_string);

iotcon_repr_h icl_repr_create_repr(const char *json_string);
iotcon_repr_h icl_repr_parse_json(const char *json_string);

void icl_repr_inc_ref_count(iotcon_repr_h val);

int icl_repr_parse_resource_property(JsonObject *prop_obj,
		iotcon_resource_types_h *types, int *ifaces);

#endif /* __IOT_CONNECTIVITY_MANAGER_LIBRARY_REPRESENTATION_H__ */
