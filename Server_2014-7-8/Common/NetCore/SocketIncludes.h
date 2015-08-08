#pragma once

#ifdef __native_client__
#define _PP_Instance_ PP_Instance
#else
#define _PP_Instance_ int
#endif

#if   defined(_WIN32)
typedef int socklen_t;

#include <winsock2.h>
#else
#define closesocket close
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef __native_client__
#include "ppapi/cpp/private/net_address_private.h"
#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_errors.h"
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/instance_handle.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/module_impl.h"
#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/pp_var.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/c/ppb_messaging.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/c/ppp.h"
#include "ppapi/c/ppb_core.h"
#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_messaging.h"
#include "ppapi/c/pp_input_event.h"
#include "ppapi/c/pp_completion_callback.h"
#include "ppapi/c/private/ppb_udp_socket_private.h"
#include "ppapi/cpp/private/net_address_private.h"

namespace VENet
{
struct SocketImpl;
}

typedef VENet::SocketImpl *SOCKET;
#define INVALID_SOCKET NULL

#else
typedef int SOCKET;
#define INVALID_SOCKET -1
#endif

#endif