#include <node_api.h>
#include <netlink/cli/utils.h>
#include <netlink/cli/route.h>
#include <netlink/cli/link.h>

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

napi_value Init(napi_env env, napi_value exports) {
    napi_status status;
    napi_value fnAddRoute, fnDeleteRoute;

    status = napi_create_function(env, NULL, 0, AddRoute, NULL, &fnAddRoute);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to wrap native function");
    }

    status = napi_create_function(env, NULL, 0, DeleteRoute, NULL, &fnDeleteRoute);
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

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)