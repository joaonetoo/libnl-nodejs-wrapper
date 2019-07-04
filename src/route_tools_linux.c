#define _GNU_SOURCE

#include <node_api.h>
#include <netlink/cli/utils.h>
#include <netlink/cli/route.h>
#include <netlink/cli/link.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#define IFLIST_REPLY_BUFFER	8192


int maxDstAddress = 19;
int maxGwAddress = 16;

int libnl_add_route(char *dst, char *next_hop) {
	static struct nl_cache *link_cache;
    struct nl_sock *sock;
    struct rtnl_route *route;

    sock = nl_cli_alloc_socket();
    nl_cli_connect(sock, NETLINK_ROUTE);
    route = rtnl_route_alloc();
    link_cache = nl_cli_link_alloc_cache(sock);
    nl_cli_route_parse_dst(route, dst);
    nl_cli_route_parse_nexthop(route, next_hop, link_cache);
    return rtnl_route_add(sock, route, NLM_F_EXCL);
}

int libnl_delete_route(char *dst, char *next_hop) {
	static struct nl_cache *link_cache;
    struct nl_sock *sock;
    struct rtnl_route *route;

    sock = nl_cli_alloc_socket();
    nl_cli_connect(sock, NETLINK_ROUTE);
    route = rtnl_route_alloc();
    link_cache = nl_cli_link_alloc_cache(sock);
    nl_cli_route_parse_dst(route, dst);
    nl_cli_route_parse_nexthop(route, next_hop, link_cache);
    return rtnl_route_delete(sock, route, 0);
}

void getArgvs(napi_env env, napi_callback_info info, char* dstAddress, char* gwAddress) {
    napi_status status;
    int nArgs = 2;
    size_t argc = nArgs;
    napi_value argv[nArgs];
    status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to parse arguments");
    }

    int dstAddressSize = maxDstAddress;
    size_t* lengthResult;
    status = napi_get_value_string_utf8(env, argv[0], dstAddress, dstAddressSize, &lengthResult);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Invalid argument was passed");
    }

    int gwAddressSize = maxGwAddress;
    status = napi_get_value_string_utf8(env, argv[1], gwAddress, gwAddressSize, &lengthResult);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Invalid argument was passed");
    }
}

napi_value AddRoute(napi_env env, napi_callback_info info) {
    napi_status status;

    char* dstAddress[maxDstAddress];
    char* gwAddress[maxGwAddress];
    getArgvs(env, info, &dstAddress, &gwAddress);
    
    char addRouteArg[255];
    sprintf(addRouteArg, "via=%s", gwAddress);

    int err = 1;
    napi_value result;
    if ((err = libnl_add_route(dstAddress, addRouteArg)) < 0) {
        status = napi_create_int32(env, 1, &result);
        // nl_cli_fatal(err, "Unable to add route: %s", nl_geterror(err));
    } else {
        status = napi_create_int32(env, 0, &result);
    }

  return result;
}

napi_value DeleteRoute(napi_env env, napi_callback_info info) {
    napi_status status;

    char* dstAddress[maxDstAddress];
    char* gwAddress[maxGwAddress];
    getArgvs(env, info, &dstAddress, &gwAddress);
    
    char addRouteArg[255];
    sprintf(addRouteArg, "via=%s", gwAddress);

    int err = 1;
    napi_value result;
    if ((err = libnl_delete_route(dstAddress, addRouteArg)) < 0) {
        status = napi_create_int32(env, 1, &result);
        // nl_cli_fatal(err, "Unable to add route: %s", nl_geterror(err));
    } else {
        status = napi_create_int32(env, 0, &result);
    }

  return result;
}
/* 
   define the message structure : 
     . a netlink message
     . a "general form of address family dependent" message, 
       i.e. how to tell which AF we are interested in
*/

typedef struct nl_req_s nl_req_t;  

struct nl_req_s {
  struct nlmsghdr hdr;
  struct rtgenmsg gen;
};
struct ret_info {
    char gateway[32];
    char ifName[IF_NAMESIZE];
    unsigned char route_netmask;
};

struct ret_info rtnl_print_route(struct nlmsghdr *nlh)
{
    struct  rtmsg *route_entry;  /* This struct represent a route entry \
                                    in the routing table */
    struct  rtattr *route_attribute; /* This struct contain route \
                                            attributes (route type) */
    int     route_attribute_len = 0;
    struct ret_info retInfo;
    unsigned char    route_netmask = 0;
    unsigned char    route_protocol = 0;
    char    gateway_address[32];
    char ifName[IF_NAMESIZE];

	route_entry = (struct rtmsg *) NLMSG_DATA(nlh);

	if (route_entry->rtm_table != RT_TABLE_MAIN)
		return;

	retInfo.route_netmask = route_entry->rtm_dst_len;
	route_protocol = route_entry->rtm_protocol;
    route_attribute = (struct rtattr *) RTM_RTA(route_entry);
    /* Get the route atttibutes len */
    route_attribute_len = RTM_PAYLOAD(nlh);
    /* Loop through all attributes */
    
    for ( ; RTA_OK(route_attribute, route_attribute_len); \
        route_attribute = RTA_NEXT(route_attribute, route_attribute_len))
    {
        if (route_attribute->rta_type == RTA_OIF)
        {
            if_indextoname(*(int *) RTA_DATA(route_attribute), retInfo.ifName);
        }

        /* Get the gateway (Next hop) */
        if (route_attribute->rta_type == RTA_GATEWAY)
        {
            inet_ntop(AF_INET, RTA_DATA(route_attribute), \
                    retInfo.gateway, sizeof(retInfo.gateway));
        }
    }
  strcpy(gateway_address, "0.0.0.0");
  return retInfo;
    
}

napi_value ListAdapterInterfaces(napi_env env, napi_callback_info info) {

  napi_value arr;
  napi_create_array(env, &arr);
  int cnt  = 0;

  int fd;
  struct sockaddr_nl local;  /* our local (user space) side of the communication */
  struct sockaddr_nl kernel; /* the remote (kernel space) side of the communication */
 
  struct msghdr rtnl_msg;    /* generic msghdr struct for use with sendmsg */
  struct iovec io;	     /* IO vector for sendmsg */

  nl_req_t req;              /* structure that describes the rtnetlink packet itself */
  char reply[IFLIST_REPLY_BUFFER]; /* a large buffer to receive lots of link information */

  pid_t pid = getpid();	     /* our process ID to build the correct netlink address */
  int end = 0;		     /* some flag to end loop parsing */

  /* 
     prepare netlink socket for kernel/userland communication 
     we are interested in the ROUTE flavor. 
     There are others like XFRM, but to deal with links, addresses and obviously routes,
     you have to use NETLINK_ROUTE.
     
   */

  fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

  memset(&local, 0, sizeof(local)); /* fill-in local address information */
  local.nl_family = AF_NETLINK;
  local.nl_pid = pid;
  local.nl_groups = 0;
  //local.nl_groups = RTMGRP_IPV4_ROUTE;

  if (bind(fd, (struct sockaddr *) &local, sizeof(local)) < 0)
    {
      perror("cannot bind, are you root ? if yes, check netlink/rtnetlink kernel support");
      return -1;
    }

  /* RTNL socket is ready for use, prepare and send request */

  memset(&rtnl_msg, 0, sizeof(rtnl_msg));
  memset(&kernel, 0, sizeof(kernel));
  memset(&req, 0, sizeof(req));

  kernel.nl_family = AF_NETLINK; /* fill-in kernel address (destination of our message) */
  kernel.nl_groups = 0;

  req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
  req.hdr.nlmsg_type = RTM_GETROUTE;
  req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP; 
  req.hdr.nlmsg_seq = 1;
  req.hdr.nlmsg_pid = pid;
  //req.gen.rtgen_family = AF_PACKET; /*  no preferred AF, we will get *all* interfaces */
  req.gen.rtgen_family = AF_INET; 

  io.iov_base = &req;
  io.iov_len = req.hdr.nlmsg_len;
  rtnl_msg.msg_iov = &io;
  rtnl_msg.msg_iovlen = 1;
  rtnl_msg.msg_name = &kernel;
  rtnl_msg.msg_namelen = sizeof(kernel);

  sendmsg(fd, (struct msghdr *) &rtnl_msg, 0);

  /* parse reply */

  while (!end)
    {
      struct ret_info retInfo;
      int len;
      struct nlmsghdr *msg_ptr;	/* pointer to current message part */

      struct msghdr rtnl_reply;	/* generic msghdr structure for use with recvmsg */
      struct iovec io_reply;

      memset(&io_reply, 0, sizeof(io_reply));
      memset(&rtnl_reply, 0, sizeof(rtnl_reply));

      io.iov_base = reply;
      io.iov_len = IFLIST_REPLY_BUFFER;
      rtnl_reply.msg_iov = &io;
      rtnl_reply.msg_iovlen = 1;
      rtnl_reply.msg_name = &kernel;
      rtnl_reply.msg_namelen = sizeof(kernel);

      len = recvmsg(fd, &rtnl_reply, 0); /* read as much data as fits in the receive buffer */
      if (len)
	{
	  for (msg_ptr = (struct nlmsghdr *) reply; NLMSG_OK(msg_ptr, len); msg_ptr = NLMSG_NEXT(msg_ptr, len))
	    {
	      switch(msg_ptr->nlmsg_type)
		{
		case 3:		/* this is the special meaning NLMSG_DONE message we asked for by using NLM_F_DUMP flag */
		  end++;
		  break;
		case 24:
            retInfo = rtnl_print_route(msg_ptr);
            if (retInfo.route_netmask == 0) {
                napi_value obj, ifName, gateway;
                napi_create_object(env, &obj);
                napi_create_string_utf8(env, retInfo.ifName, NAPI_AUTO_LENGTH, &ifName);
                napi_create_string_utf8(env, retInfo.gateway, NAPI_AUTO_LENGTH, &gateway);
                napi_set_named_property(env, obj, "ifName", ifName);
                napi_set_named_property(env, obj, "gateway", gateway);
                napi_set_element(env, arr, cnt, obj);
                cnt += 1;
            }
		  break;
		default:	/* for education only, print any message that would not be DONE or NEWLINK, 
				   which should not happen here */
		  printf("message type %d, length %d\n", msg_ptr->nlmsg_type, msg_ptr->nlmsg_len);
		  break;
		}
	    }
	}
      
    }

  /* clean up and finish properly */

  close(fd);
  return arr;
}

napi_value Init(napi_env env, napi_value exports) {
    napi_status status;
    napi_value fnAddRoute, fnDeleteRoute, fnListAdapterInterfaces;

    status = napi_create_function(env, NULL, 0, AddRoute, NULL, &fnAddRoute);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to wrap native function");
    }

    status = napi_create_function(env, NULL, 0, DeleteRoute, NULL, &fnDeleteRoute);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to wrap native function");
    }

    status = napi_create_function(env, NULL, 0, ListAdapterInterfaces, NULL, &fnListAdapterInterfaces);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to wrap native function");
    }

    status = napi_set_named_property(env, exports, "addRoute", fnAddRoute);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to populate exports");
    }

    status = napi_set_named_property(env, exports, "deleteRoute", fnDeleteRoute);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to populate exports");
    }

    status = napi_set_named_property(env, exports, "listAdapterInterfaces", fnListAdapterInterfaces);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to populate exports");
    }

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)