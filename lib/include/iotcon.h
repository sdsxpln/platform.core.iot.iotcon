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
#ifndef __IOT_CONNECTIVITY_MANAGER_H__
#define __IOT_CONNECTIVITY_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <iotcon-errors.h>
#include <iotcon-struct.h>
#include <iotcon-constant.h>
#include <iotcon-representation.h>

/**
 * @file iotcon.h
 */

/**
 *
 * @addtogroup CAPI_IOT_CONNECTIVITY_MODULE
 *
 * @{
 */

/**
 * @brief Opens Iotcon.
 * @details Call this function to start Iotcon.
 *
 * @since_tizen 3.0
 *
 * @remarks You must free all resources of the Iotcon by calling iotcon_close()
 * if Iotcon API is no longer needed.
 *
 * @return  0 on success, otherwise a negative error value.
 * @retval  #IOTCON_ERROR_NONE Successful
 * @retval  #IOTCON_ERROR_DBUS Dbus error
 *
 * @see iotcon_close()
 */
int iotcon_open(void);

/**
 * @brief Closes Iotcon.
 * @details Frees the resources allocated to Iotcon.
 *
 * @since_tizen 3.0
 *
 * @remarks This function must be called if Iotcon API is no longer needed.
 *
 * @return void
 *
 * @see iotcon_open()
 */
void iotcon_close(void);


/**
 * @brief Set timeout of asynchronize APIs.
 * @details Default timeout is 10 seconds.
 * Maximum timeout is 60 seconds.
 *
 * @since_tizen 3.0
 *
 * @param[in] timeout_secods Seconds for timeout
 *
 * @return  0 on success, otherwise a negative error value.
 * @retval  #IOTCON_ERROR_NONE Successful
 * @retval  #IOTCON_ERROR_DBUS Dbus error
 *
 * @pre iotcon_open() should be called to open a connection to the iotcon.
 *
 * @see iotcon_get_device_info()
 * @see iotcon_get_platform_info()
 * @see iotcon_get_tizen_info()
 * @see iotcon_find_resource()
 * @see iotcon_remote_resource_get()
 * @see iotcon_remote_resource_put()
 * @see iotcon_remote_resource_post()
 * @see iotcon_remote_resource_delete()
 */
int iotcon_set_timeout(int timeout_seconds);

/**
 * @brief Specifies the type of function passed to iotcon_add_connection_changed_cb() and
 * iotcon_remove_connection_changed_cb().
 *
 * @since_tizen 3.0
 *
 * @param[in] is_connected The status of connection
 * @param[in] user_data The user data to pass to the function
 *
 * @pre The callback must be registered using iotcon_add_connection_changed_cb()\n
 * The callback must be unregistered using iotcon_remove_connection_changed_cb()\n
 *
 * @see iotcon_add_connection_changed_cb()
 * @see iotcon_remove_connection_changed_cb()
 */
typedef void (*iotcon_connection_changed_cb)(bool is_connected, void *user_data);

/**
 * @brief Adds a callback to Iotcon
 * @details When Iotcon connection status is changed, registered callbacks will be called in turn.
 *
 * @since_tizen 3.0
 *
 * @param[in] cb The callback function to add into callback list
 * @param[in] user_data The user data to pass to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_ALREADY  Already done
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 *
 * @see iotcon_remove_connection_changed_cb()
 * @see iotcon_connection_changed_cb()
 */
int iotcon_add_connection_changed_cb(iotcon_connection_changed_cb cb, void *user_data);

/**
 * @brief Removes the callback from the callback list.
 * @details Finds out the callback passing to parameter from registered callbacks, then remove it.
 *
 * @since_tizen 3.0
 *
 * @param[in] cb The callback function to remove from callback list
 * @param[in] user_data The user data to pass to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_add_connection_changed_cb()
 * @see iotcon_connection_changed_cb()
 */
int iotcon_remove_connection_changed_cb(iotcon_connection_changed_cb cb, void *user_data);

/**
 * @brief Specifies the type of function passed to iotcon_resource_create() and
 * iotcon_resource_set_request_handler()
 * @details Called when server receive request from the client.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The resource requested
 * @param[in] request The request from client
 * @param[in] user_data The user data to pass to the function
 *
 * @pre The callback must be registered using iotcon_resource_create()
 *
 * @see iotcon_resource_create()
 * @see iotcon_resource_set_request_handler()
 */
typedef void (*iotcon_request_handler_cb)(iotcon_resource_h resource,
		iotcon_request_h request, void *user_data);

/**
 * @brief Create a resource handle and registers the resource in server
 * @details Registers a resource specified by @a uri_path, @a res_types, @a ifaces which have
 * @a properties in Iotcon server.\n
 * When client find the registered resource, iotcon_request_handler_cb() will be called automatically.\n
 * @a uri_path format would be relative URI path like '/a/light'\n
 * @a res_types is a list of resource types. Create a iotcon_resource_types_h handle and
 * add types string to it.\n
 * @a ifaces can contain multiple interfaces like
 * IOTCON_INTERFACE_LINK | IOTCON_INTERFACE_BATCH.\n
 * @a properties also can contain multiple properties like
 * IOTCON_ACTIVE | IOTCON_DISCOVERABLE.\n
 * iotcon_request_handler_cb() will be called when receive CRUD request to the registered
 * resource.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @remarks @a uri_path length must be less than or equal 36.\n
 * You must unregister resource by calling iotcon_resource_destroy()
 * if resource is no longer needed.
 *
 * @param[in] uri_path The URI path of the resource.
 * @param[in] res_types The list of type of the resource.
 * @param[in] ifaces The interfaces of the resource.
 * @param[in] properties The property of the resource.
 * @param[in] cb The request handler callback function
 * @param[in] user_data The user data to pass to the callback function
 * @param[out] resource_handle The handle of the resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_IOTIVITY  Iotivity errors
 * @retval #IOTCON_ERROR_DBUS  Dbus errors
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @post When the resource receive CRUD request, iotcon_request_handler_cb() will be called.
 *
 * @see iotcon_resource_destroy()
 * @see iotcon_resource_bind_interface()
 * @see iotcon_resource_bind_type()
 * @see iotcon_resource_set_request_handler()
 * @see iotcon_resource_bind_child_resource()
 * @see iotcon_resource_unbind_child_resource()
 * @see iotcon_request_handler_cb()
 */
int iotcon_resource_create(const char *uri_path,
		iotcon_resource_types_h res_types,
		int ifaces,
		int properties,
		iotcon_request_handler_cb cb,
		void *user_data,
		iotcon_resource_h *resource_handle);

/**
 * @brief Destroy the resource and releases its data.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @remarks When a normal variable is used, there are only dbus error and permission\n
 * denied error. If the errors of this API are not handled, then you must check\n
 * whether dbus is running and an application have the privileges for the API.
 *
 * @param[in] resource_handle The handle of the resource to be unregistered
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @see iotcon_resource_create()
 * @see iotcon_resource_bind_interface()
 * @see iotcon_resource_bind_type()
 * @see iotcon_resource_set_request_handler()
 * @see iotcon_resource_bind_child_resource()
 * @see iotcon_resource_unbind_child_resource()
 * @see iotcon_request_handler_cb()
 */
int iotcon_resource_destroy(iotcon_resource_h resource_handle);

/**
 * @brief Binds an interface to the resource
 *
 * @details The @a action could be one of #iotcon_interface_e.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @remarks Set only one interface to @a iface. If not, @a iface will be ignored.
 *
 * @param[in] resource The handle of the resource
 * @param[in] iface The interface to be bound to the resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @see iotcon_resource_create()
 * @see iotcon_resource_destroy()
 * @see iotcon_resource_bind_type()
 * @see iotcon_resource_set_request_handler()
 * @see iotcon_resource_bind_child_resource()
 * @see iotcon_resource_unbind_child_resource()
 * @see iotcon_request_handler_cb()
 */
int iotcon_resource_bind_interface(iotcon_resource_h resource, iotcon_interface_e iface);

/**
 * @brief Binds a type to the resource
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource_handle The handle of the resource
 * @param[in] resource_type The type to be bound to the resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @see iotcon_resource_create()
 * @see iotcon_resource_destroy()
 * @see iotcon_resource_bind_interface()
 * @see iotcon_resource_set_request_handler()
 * @see iotcon_resource_bind_child_resource()
 * @see iotcon_resource_unbind_child_resource()
 * @see iotcon_request_handler_cb()
 */
int iotcon_resource_bind_type(iotcon_resource_h resource_handle,
		const char *resource_type);

/**
 * @brief Binds a request handler to the resource
 * @details When the resource receive CRUD request, iotcon_request_handler_cb() will be
 * called.
 *
 * @since_tizen 3.0
 *
 * @remarks Registered callback function will be replaced with the new @a cb.\n
 *
 * @param[in] resource The handle of the resource
 * @param[in] cb The request handler to be bound to the resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_resource_create()
 * @see iotcon_resource_destroy()
 * @see iotcon_resource_bind_interface()
 * @see iotcon_resource_bind_type()
 * @see iotcon_resource_bind_child_resource()
 * @see iotcon_resource_unbind_child_resource()
 * @see iotcon_request_handler_cb()
 */
int iotcon_resource_set_request_handler(iotcon_resource_h resource,
		iotcon_request_handler_cb cb);

/**
 * @brief Binds a child resource into the parent resource.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] parent The handle of the parent resource
 * @param[in] child The handle of the child resource to be added to the parent resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_ALREADY  Already done
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @see iotcon_resource_create()
 * @see iotcon_resource_destroy()
 * @see iotcon_resource_bind_interface()
 * @see iotcon_resource_bind_type()
 * @see iotcon_resource_set_request_handler()
 * @see iotcon_resource_unbind_child_resource()
 * @see iotcon_request_handler_cb()
 */
int iotcon_resource_bind_child_resource(iotcon_resource_h parent,
		iotcon_resource_h child);

/**
 * @brief Unbinds a child resource from the parent resource.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] parent The handle of the parent resource
 * @param[in] child The handle of the child resource to be unbound from the parent resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @see iotcon_resource_create()
 * @see iotcon_resource_destroy()
 * @see iotcon_resource_bind_interface()
 * @see iotcon_resource_bind_type()
 * @see iotcon_resource_set_request_handler()
 * @see iotcon_resource_bind_child_resource()
 * @see iotcon_request_handler_cb()
 */
int iotcon_resource_unbind_child_resource(iotcon_resource_h parent,
		iotcon_resource_h child);

/**
 * @brief Specifies the type of function passed to iotcon_get_device_info().
 *
 * @since_tizen 3.0
 *
 * @param[in] device_info the device information from remote server.
 * @param[in] result The result code (0 on success, other wise a negative error value)
 * @param[in] user_data The user data to pass to the function
 *
 * @pre iotcon_get_device_info() will invoke this callback function.
 *
 * @see iotcon_get_device_info()
 * @see iotcon_device_info_get_property()
 */
typedef void (*iotcon_device_info_cb)(iotcon_device_info_h device_info, int result,
		void *user_data);

/**
 * @brief Calls a function for device information of remote server.
 * @details Request device information to server and pass the information by calling
 * iotcon_device_info_cb().\n
 * iotcon_device_info_cb() will be called when success on getting device information.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] host_address The host address of remote server
 * @param[in] connectivity_type The connectivity type
 * @param[in] cb The callback function to invoke
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @post iotcon_device_info_cb() will be called when success on getting device information.
 *
 * @see iotcon_device_info_cb()
 * @see iotcon_device_info_get_property()
 */
int iotcon_get_device_info(const char *host_address,
		iotcon_connectivity_type_e connectivity_type,
		iotcon_device_info_cb cb,
		void *user_data);

/**
 * @brief Specifies the type of function passed to iotcon_get_platform_info().
 *
 * @since_tizen 3.0
 *
 * @param[in] platform_info The platform information from remote server.
 * @param[in] result The result code (0 on success, other wise a negative error value)
 * @param[in] user_data The user data to pass to the function
 *
 * @pre iotcon_get_platform_info() will invoke this callback function.
 *
 * @see iotcon_get_platform_info()
 * @see iotcon_platform_info_get_property()
 */
typedef void (*iotcon_platform_info_cb)(iotcon_platform_info_h platform_info, int result,
		void *user_data);

/**
 * @brief Calls a function for platform information of remote server.
 * @details Request platform information to server and pass the information by calling
 * iotcon_platform_info_cb().\n
 * iotcon_platform_info_cb() will be called when success on getting device information.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] host_address The host address of remote server
 * @param[in] connectivity_type The connectivity type
 * @param[in] cb The callback function to invoke
 * @param[in] user_data The user data to pass to the function
 *
 * @return  0 on success, otherwise a negative error value.
 *
 * @retval #IOTCON_ERROR_NONE Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @post iotcon_platform_info_cb() will be called when success on getting device information.
 *
 * @see iotcon_platform_info_cb()
 * @see iotcon_platform_info_get_property()
 */
int iotcon_get_platform_info(const char *host_address,
		iotcon_connectivity_type_e connectivity_type,
		iotcon_platform_info_cb cb,
		void *user_data);

/**
 * @brief Specifies the type of function passed to iotcon_get_tizen_info().
 *
 * @since_tizen 3.0
 *
 * @param[in] tizen_info The information of tizen device from remote server.
 * @param[in] response_result The response result code (Lesser than 0 on fail, otherwise a response result value)
 * @param[in] user_data The user data to pass to the function
 *
 * @pre iotcon_get_tizen_info() will invoke this callback function.
 *
 * @see iotcon_get_tizen_info()
 * @see iotcon_tizen_info_get_property()
 */
typedef void (*iotcon_tizen_info_cb)(iotcon_tizen_info_h tizen_info,
		int response_result, void *user_data);

/**
 * @brief Calls a function for tizen device information of remote server.
 * @details Request tizen device information to server and pass the information by calling
 * iotcon_tizen_info_cb().\n
 * iotcon_tizen_info_cb() will be called when success on getting tizen device information.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] host_address The host address of remote server
 * @param[in] connectivity_type The connectivity type
 * @param[in] cb The callback function to invoke
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @post iotcon_tizen_info_cb() will be called when success on getting tizen device information.
 *
 * @see iotcon_tizen_info_cb()
 * @see iotcon_tizen_info_get_property()
 */
int iotcon_get_tizen_info(const char *host_address,
		iotcon_connectivity_type_e connectivity_type,
		iotcon_tizen_info_cb cb,
		void *user_data);

/**
 * @brief Specifies the type of function passed to iotcon_subscribe_presence().
 * @details Called when client receive presence events from the server.
 *
 * @since_tizen 3.0
 *
 * @param[in] result The result code of server's presence
 * @param[in] nonce Current nonce of server's presence
 * @param[in] host_address The address or addressable name of server
 * @param[in] user_data The user data to pass to the function
 *
 * @pre The callback must be registered using iotcon_subscribe_presence()
 *
 * @see iotcon_subscribe_presence()
 */
typedef void (*iotcon_presence_cb)(int result, unsigned int nonce,
		const char *host_address, void *user_data);

/**
 * @brief Subscribes to a server to receive presence events.
 * @details Request to receive presence to an interested server's resource with @a resource_type.\n
 * If succeed to subscribe, iotcon_presence_cb() will be invoked when the server sends presence\n
 * A server sends presence events when adds/removes/alters a resource or start/stop presence.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] host_address The address or addressable name of the server
 * @param[in] connectivity_type The connectivity type
 * @param[in] resource_type A resource type that a client has interested in
 * @param[in] cb The callback function to invoke
 * @param[in] user_data The user data to pass to the function
 * @param[out] presence_handle The generated presence handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_IOTIVITY  Iotivity errors
 * @retval #IOTCON_ERROR_DBUS  Dbus errors
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @post When the resource receive presence, iotcon_presence_cb() will be called.
 *
 * @see iotcon_start_presence()
 * @see iotcon_stop_presence()
 * @see iotcon_unsubscribe_presence()
 * @see iotcon_presence_cb()
 */
int iotcon_subscribe_presence(const char *host_address,
		iotcon_connectivity_type_e connectivity_type,
		const char *resource_type,
		iotcon_presence_cb cb,
		void *user_data,
		iotcon_presence_h *presence_handle);

/**
 * @brief Unsubscribes to a server's presence events.
 * @details Request not to receive server's presence any more.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] presence_handle The presence handle to be unsubscribed
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @see iotcon_start_presence()
 * @see iotcon_stop_presence()
 * @see iotcon_subscribe_presence()
 * @see iotcon_presence_cb()
 */
int iotcon_unsubscribe_presence(iotcon_presence_h presence_handle);

/**
 * @brief Specifies the type of function passed to iotcon_find_resource().
 * @details Called when a resource is found from the remote server.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of resource which is found
 * @param[in] result The result code (Lesser than 0 on fail, otherwise a response result value)
 * @param[in] user_data The user data to pass to the function
 *
 * @pre The callback must be registered using iotcon_find_resource()
 *
 * @see iotcon_find_resource()
 */
typedef void (*iotcon_found_resource_cb)(iotcon_remote_resource_h resource, int result,
		void *user_data);

/**
 * @brief Finds resources.
 * @details Request to find a resource of @a host_address server with @a resource_type.\n
 * If succeed to find the resource, iotcon_found_resource_cb() will be invoked with
 * information of the resource.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] host_address The address or addressable name of server
 * @param[in] connectivity_type The connectivity type
 * @param[in] resource_type The resource type specified as a filter for the resource
 * @param[in] cb The callback function to invoke
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @post When the resource is found, iotcon_found_resource_cb() will be called.
 *
 * @see iotcon_found_resource_cb()
 */
int iotcon_find_resource(const char *host_address,
		iotcon_connectivity_type_e connectivity_type,
		const char *resource_type,
		iotcon_found_resource_cb cb,
		void *user_data);

/**
 * @brief Creates a new resource handle.
 * @details Creates a resource proxy object so that get/put/observe functionality can be used
 * without discovering the object in advance.\n
 * To use this API, you should provide all of the details required to correctly contact and
 * observe the object.\n
 * If not, you should discover the resource object manually.
 *
 * @since_tizen 3.0
 *
 * @param[in] host The host address of the resource
 * @param[in] connectivity_type The connectivity type
 * @param[in] uri_path The URI path of the resource.
 * @param[in] is_observable Allow observation
 * @param[in] resource_types The resource type of the resource. For example, "core.light"
 * @param[in] resource_interfaces The resource interfaces (whether it is collection etc)
 * @param[out] client_handle Generated resource handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_IOTIVITY  Iotivity errors
 * @retval #IOTCON_ERROR_DBUS  Dbus errors
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 *
 * @see iotcon_remote_resource_destroy()
 * @see iotcon_remote_resource_clone()
 */
int iotcon_remote_resource_create(const char *host,
		iotcon_connectivity_type_e connectivity_type,
		const char *uri_path,
		bool is_observable,
		iotcon_resource_types_h resource_types,
		int resource_ifs,
		iotcon_remote_resource_h *client_handle);

/**
 * @brief Releases a resource handle.
 * @details Decrements reference count of the source resource.\n
 * If the reference count drops to 0, releases a resource handle.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the resource
 *
 * @return void
 *
 * @see iotcon_remote_resource_create()
 * @see iotcon_remote_resource_clone()
 */
void iotcon_remote_resource_destroy(iotcon_remote_resource_h resource);

/**
 * @brief Makes a clone of a remote resource.
 *
 * @since_tizen 3.0
 *
 * @param[in] src The Source of resource
 * @param[out] dest The cloned resource handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_remote_resource_create()
 * @see iotcon_remote_resource_destroy()
 */
int iotcon_remote_resource_clone(iotcon_remote_resource_h src,
		iotcon_remote_resource_h *dest);

/**
 * @brief Specifies the type of function passed to iotcon_remote_resource_observer_start().
 * @details Called when a client receive notifications from a server. The @a response_result could be one of #iotcon_response_result_e.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the resource
 * @param[in] repr The handle of the representation
 * @param[in] options The handle of the header options
 * @param[in] response_result The response result code
 * @param[in] sequence_number The sequence of notifications from server.
 * @param[in] user_data The user data passed from the callback registration function
 *
 * @pre The callback must be registered using iotcon_remote_resource_observer_start()
 *
 * @see iotcon_remote_resource_observer_start()
 */
typedef void (*iotcon_remote_resource_observe_cb)(iotcon_remote_resource_h resource,
		iotcon_representation_h repr,
		iotcon_options_h options,
		int response_result,
		int sequence_number,
		void *user_data);

/**
 * @brief Sets observation on the resource
 * @details When server sends notification message, iotcon_remote_resource_observe_cb() will be called.
 * The @a observe_type could be one of #iotcon_observe_type_e.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the resource
 * @param[in] observe_type The type to specify how client wants to observe.
 * @param[in] query The query to send to server
 * @param[in] cb The callback function to get notifications from server
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_IOTIVITY  Iotivity errors
 * @retval #IOTCON_ERROR_DBUS  Dbus errors
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @post When the @a resource receive notification message, iotcon_remote_resource_observe_cb() will be called.
 *
 * @see iotcon_remote_resource_observe_cb()
 * @see iotcon_remote_resource_observer_stop()
 * @see iotcon_notimsg_create()
 * @see iotcon_resource_notify()
 */
int iotcon_remote_resource_observer_start(iotcon_remote_resource_h resource,
		iotcon_observe_type_e observe_type,
		iotcon_query_h query,
		iotcon_remote_resource_observe_cb cb,
		void *user_data);

/**
 * @brief Cancels the observation on the resource
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @see iotcon_remote_resource_observe_cb()
 * @see iotcon_remote_resource_observer_start()
 * @see iotcon_notimsg_create()
 * @see iotcon_resource_notify()
 */
int iotcon_remote_resource_observer_stop(iotcon_remote_resource_h resource);

/**
 * @brief Send response for incoming request.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resp The handle of the response to send
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 */
int iotcon_response_send(iotcon_response_h resp);

/**
 * @brief Creates a new notifications message handle.
 * @details @a iface could be one of #iotcon_interface_e.
 *
 * @since_tizen 3.0
 *
 * @param[in] repr The handle of the representation
 * @param[in] iface The resource interface
 * @param[out] notimsg_handle The generated notifications message handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 *
 * @see iotcon_remote_resource_observe_cb()
 * @see iotcon_remote_resource_observer_start()
 * @see iotcon_remote_resource_observer_stop()
 * @see iotcon_resource_notify()
 */
int iotcon_notimsg_create(iotcon_representation_h repr, iotcon_interface_e iface,
		iotcon_notimsg_h *notimsg_handle);

/**
 * @brief Releases a notifications message handle.
 *
 * @since_tizen 3.0
 *
 * @param[in] msg The handle of the notifications message
 *
 * @return void
 *
 * @see iotcon_notimsg_create()
 */
void iotcon_notimsg_destroy(iotcon_notimsg_h msg);

/**
 * @brief Notifies specific clients that resource's attributes have changed.
 * @details If @a observers is @c NULL, the @a msg will notify to all observers.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the resource
 * @param[in] msg The handle of the notifications message
 * @param[in] observers The handle of the observers.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_REPRESENTATION  Representation error
 * @retval #IOTCON_ERROR_SYSTEM  System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @see iotcon_remote_resource_observe_cb()
 * @see iotcon_remote_resource_observer_start()
 * @see iotcon_remote_resource_observer_stop()
 * @see iotcon_notimsg_create()
 * @see iotcon_notimsg_destroy()
 * @see iotcon_observers_create()
 * @see iotcon_observers_destroy()
 * @see iotcon_observers_add()
 * @see iotcon_observers_remove()
 */
int iotcon_resource_notify(iotcon_resource_h resource, iotcon_notimsg_h msg,
		iotcon_observers_h observers);

/**
 * @brief Specifies the type of function passed to iotcon_remote_resource_get(), iotcon_remote_resource_put(), iotcon_remote_resource_post()
 * @details The @a response_result could be one of #iotcon_response_result_e.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the resource
 * @param[in] repr The handle of the representation
 * @param[in] options The handle of the header options
 * @param[in] response_result The response result code (Lesser than 0 on fail, otherwise a response result value)
 * @param[in] user_data The user data to pass to the function
 *
 * @pre The callback must be registered using iotcon_remote_resource_get(), iotcon_remote_resource_put(), iotcon_remote_resource_post()
 *
 * @see iotcon_remote_resource_get()
 * @see iotcon_remote_resource_put()
 * @see iotcon_remote_resource_post()
 */
typedef void (*iotcon_remote_resource_cru_cb)(iotcon_remote_resource_h resource,
		iotcon_representation_h repr,
		iotcon_options_h options,
		int response_result,
		void *user_data);

/**
 * @brief Gets the attributes of a resource.
 * @details When server sends response on get request, iotcon_remote_resource_cru_cb() will be called.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the resource
 * @param[in] query The query to send to server
 * @param[in] cb The callback function
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus errors
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @post When the client receive get response, iotcon_remote_resource_cru_cb() will be called.
 *
 * @see iotcon_remote_resource_cru_cb()
 * @see iotcon_remote_resource_put()
 * @see iotcon_remote_resource_post()
 */
int iotcon_remote_resource_get(iotcon_remote_resource_h resource, iotcon_query_h query,
		iotcon_remote_resource_cru_cb cb, void *user_data);

/**
 * @brief Sets the representation of a resource (via PUT)
 * @details When server sends response on put request, iotcon_remote_resource_cru_cb() will be called.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the resource
 * @param[in] repr The handle of the representation
 * @param[in] query The query to send to server
 * @param[in] cb The callback function
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus errors
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @post When the client receive put response, iotcon_remote_resource_cru_cb() will be called.
 *
 * @see iotcon_remote_resource_cru_cb()
 * @see iotcon_remote_resource_get()
 * @see iotcon_remote_resource_post()
 */
int iotcon_remote_resource_put(iotcon_remote_resource_h resource,
		iotcon_representation_h repr,
		iotcon_query_h query,
		iotcon_remote_resource_cru_cb cb,
		void *user_data);

/**
 * @brief Posts on a resource
 * @details When server sends response on post request, iotcon_remote_resource_cru_cb() will be called.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the resource
 * @param[in] repr The handle of the representation
 * @param[in] query The query to send to server
 * @param[in] cb The callback function
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus errors
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @post When the client receive post response, iotcon_remote_resource_cru_cb() will be called.
 *
 * @see iotcon_remote_resource_cru_cb()
 * @see iotcon_remote_resource_get()
 * @see iotcon_remote_resource_put()
 */
int iotcon_remote_resource_post(iotcon_remote_resource_h resource,
		iotcon_representation_h repr,
		iotcon_query_h query,
		iotcon_remote_resource_cru_cb cb,
		void *user_data);

/**
 * @brief Specifies the type of function passed to iotcon_remote_resource_delete()
 * @details The @a response_result could be one of #iotcon_response_result_e.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the resource
 * @param[in] options The handle of the header options
 * @param[in] response_result The response result code
 * @param[in] user_data The user data to pass to the function
 *
 * @pre The callback must be registered using iotcon_remote_resource_delete()
 *
 * @see iotcon_remote_resource_delete()
 */
typedef void (*iotcon_remote_resource_delete_cb)(iotcon_remote_resource_h resource,
		iotcon_options_h options,
		int response_result,
		void *user_data);

/**
 * @brief Deletes a resource.
 * @details When server sends response on delete request, iotcon_remote_resource_delete_cb() will be called.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the resource
 * @param[in] cb The callback function
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus errors
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @post When the client receive delete response, iotcon_remote_resource_delete_cb() will be called.
 *
 * @see iotcon_remote_resource_delete_cb()
 */
int iotcon_remote_resource_delete(iotcon_remote_resource_h resource,
		iotcon_remote_resource_delete_cb cb, void *user_data);

/**
 * @brief Create a lite resource handle and registers the resource in server
 * @details Registers a resource specified by @a uri_path, @a res_types, @a state which have
 * @a properties in Iotcon server.\n
 * When client requests some operations, it send a response to client, automatically.\n
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @remarks @a uri_path length must be less than or equal 36.\n
 * You must unregister resource by calling iotcon_resource_destroy()
 * if resource is no longer needed.
 *
 * @param[in] uri_path The URI path of the resource.
 * @param[in] res_types The list of type of the resource.
 * @param[in] properties The property of the resource.
 * @param[in] state The state handle to set.
 * @param[out] resource_handle The handle of the resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_IOTIVITY  Iotivity errors
 * @retval #IOTCON_ERROR_DBUS  Dbus errors
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @post When the resource receive CRUD request, iotcon_request_handler_cb() will be called.
 *
 * @see iotcon_lite_resource_destroy()
 */
int iotcon_lite_resource_create(const char *uri_path,
		iotcon_resource_types_h res_types,
		int properties,
		iotcon_state_h state,
		iotcon_lite_resource_h *resource_handle);

/**
 * @brief Destroy the resource and releases its data.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @remarks When a normal variable is used, there are only dbus error and permission\n
 * denied error. If the errors of this API are not handled, then you must check\n
 * whether dbus is running and an application have the privileges for the API.
 *
 * @param[in] resource The handle of the lite resource to be unregistered
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @see iotcon_lite_resource_create()
 */
int iotcon_lite_resource_destroy(iotcon_lite_resource_h resource);

/**
 * @brief Specifies the type of function passed to iotcon_remote_resource_start_caching().
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the remote resource
 * @param[in] representation The handle of the representation
 * @param[in] user_data The user data to pass to the function
 *
 * @pre The callback must be registered using iotcon_remote_resource_start_caching()\n
 *
 * @see iotcon_remote_resource_start_caching()
 * @see iotcon_remote_resource_stop_caching()
 */
typedef void (*iotcon_remote_resource_cached_representation_changed_cb)(
		iotcon_remote_resource_h resource,
		iotcon_representation_h representation,
		void *user_data);

/**
 * @brief Start caching of a remote resource.
 * @details Use this function to start caching the resource's attribute data (state).\n
 * Default caching time interval is 10 seconds.
 * Internally, it operates GET method, periodically, and it observes the remote resource.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the remote resource to be cached
 * @param[in] caching_interval Seconds for caching time interval.\n
 * If value is 0, then it sets 10 seconds(default caching time).
 * @param[in] cb The callback function to add into callback list
 * @param[in] user_data The user data to pass to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_ALREADY Already done
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 *
 * @see iotcon_remote_resource_stop_caching()
 * @see iotcon_remote_resource_cached_representation_changed_cb()
 */
int iotcon_remote_resource_start_caching(iotcon_remote_resource_h resource,
		int caching_interval,
		iotcon_remote_resource_cached_representation_changed_cb cb,
		void *user_data);

/**
 * @brief Stop caching of a remote resource.
 * @details Use this function to stop caching the resource's attribute data (state).\n
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the remote resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @see iotcon_remote_resource_start_caching()
 * @see iotcon_remote_resource_cached_representation_changed_cb()
 */
int iotcon_remote_resource_stop_caching(iotcon_remote_resource_h resource);

/**
 * @brief Specifies the type of function passed to iotcon_remote_resource_start_monitoring().
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the remote resource
 * @param[in] state The state of the remote resource
 * @param[in] user_data The user data to pass to the function
 *
 * @pre The callback must be registered using iotcon_remote_resource_start_monitoring()\n
 *
 * @see iotcon_remote_resource_start_monitoring()
 * @see iotcon_remote_resource_stop_monitoring()
 */
typedef void (*iotcon_remote_resource_state_changed_cb)(iotcon_remote_resource_h resource,
		iotcon_remote_resource_state_e state, void *user_data);

/**
 * @brief Start monitoring of a remote resource.
 * @details When remote resource's state are changed, registered callbacks will be called\n
 * in turn. Default monitoring time interval is 10 seconds.
 * Internally, it operates GET method, periodically, and it subscribes the devices's presence.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the remote resource
 * @param[in] monitoring_interval Seconds for monitoring time interval.\n
 * If value is 0, then it sets 10 seconds(default monitoring time).
 * @param[in] cb The callback function to add into callback list
 * @param[in] user_data The user data to pass to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_ALREADY  Already done
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 *
 * @see iotcon_remote_resource_stop_monitoring()
 * @see iotcon_remote_resource_state_changed_cb()
 */
int iotcon_remote_resource_start_monitoring(iotcon_remote_resource_h resource,
		int monitoring_interval,
		iotcon_remote_resource_state_changed_cb cb,
		void *user_data);

/**
 * @brief Stop monitoring.
 * @details Use this function to stop monitoring the remote resource.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the remote resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_DBUS  Dbus error
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @see iotcon_remote_resource_start_monitoring()
 * @see iotcon_remote_resource_state_changed_cb()
 */
int iotcon_remote_resource_stop_monitoring(iotcon_remote_resource_h resource);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __IOT_CONNECTIVITY_MANAGER_H__ */
