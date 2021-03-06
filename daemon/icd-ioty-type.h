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
#ifndef __IOT_CONNECTIVITY_MANAGER_DAEMON_IOTIVITY_TYPE_H__
#define __IOT_CONNECTIVITY_MANAGER_DAEMON_IOTIVITY_TYPE_H__

#include <octypes.h>

OCConnectivityType icd_ioty_conn_type_to_oic_conn_type(int conn_type);

int icd_ioty_transport_flag_to_conn_type(OCTransportAdapter adapter,
		OCTransportFlags flag);

int icd_ioty_get_dev_addr(const char *host_address, int conn_type, OCDevAddr *dev_addr);

int icd_ioty_get_host_address(OCDevAddr *dev_addr, char **host_address, int *conn_type);

int icd_ioty_oic_properties_to_properties(int oic_properties);

#endif /*__IOT_CONNECTIVITY_MANAGER_DAEMON_IOTIVITY_TYPE_H__*/
