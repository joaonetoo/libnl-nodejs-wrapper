/* cl /D_USRDLL /D_WINDLL networkInterfaces.c /link /DLL /OUT:libnetworkInterfaces.dll */
#include <node_api.h>

#include <stdint.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#if defined(WIN32) || defined(_WIN32)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include <windows.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <ntstatus.h>
#include <iphlpapi.h>

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <objbase.h>
#include <wtypes.h>
#include <netioapi.h>
#include <ntstatus.h>
#include <winternl.h>
#include <Mstcpip.h>
#include <stdio.h>


#include "interfaces.h"

// Need to link with Iphlpapi.lib and Ws2_32.lib
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

#define MAXDSTADDRESS 19
#define MAXGWADDEESS 16


napi_value FindAdapter(napi_env env, napi_callback_info info) {
  int maxDeviceName = 19;
  napi_status status;
  int  nArgs = 1;
  size_t argc = nArgs;
  napi_value argv[1];
  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to parse arguments");
    }
    char* deviceName[19];
    int deviceNameSize = maxDeviceName;
    size_t* lengthResult;
    status = napi_get_value_string_utf8(env, argv[0], deviceName, deviceNameSize, &lengthResult);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Invalid argument was passed");
    }


  /* Some general variables */
  ULONG ulOutBufLen;
  DWORD dwRetVal;
  unsigned int i;
  napi_value result;
  int found = -1;
  status = napi_create_int32(env, found, &result);
  /* variables used for GetAdapterInfo */
  IP_ADAPTER_INFO *pAdapterInfo;
  IP_ADAPTER_INFO *pAdapter;

  pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));
  if (pAdapterInfo == NULL) {
      printf("Error allocating memory needed to call GetAdapterInfo\n");
      return result;
  }

  ulOutBufLen = sizeof (IP_ADAPTER_INFO);

  if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
      FREE(pAdapterInfo);
      pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
      if (pAdapterInfo == NULL) {
          printf("Error allocating memory needed to call GetAdapterInfo\n");
          return result;
      }
  }

  if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != NO_ERROR) {
      printf("GetAdaptersInfo failed with error %d\n", dwRetVal);
      if (pAdapterInfo)
          FREE(pAdapterInfo);
      status = napi_create_int32(env, 1, &result);
      return result;
  }

  InterfaceResult *localObj;
  localObj = (InterfaceResult *) MALLOC(sizeof (InterfaceResult));

  pAdapter = pAdapterInfo;
  
  while (pAdapter) {
      if (strstr(pAdapter->Description, deviceName) != NULL) {
          found = 1;
          localObj->ifindex = (int) pAdapter->Index;
          localObj->Adapter = pAdapter->Description;

          localObj->Gateway = pAdapter->GatewayList.IpAddress.String;
          localObj->IP = pAdapter->IpAddressList.IpAddress.String;
          break;
      }

      pAdapter = pAdapter->Next;
  }

  if (pAdapterInfo) {
    FREE(pAdapterInfo);
    pAdapterInfo = NULL;
  }
  
  if (found > 0) {
      status = napi_create_int32(env, localObj->ifindex, &result);
      return result;
  }
  napi_create_int32(env, found, &result);
  return result;
}


napi_value GetGatewayAdapter(napi_env env, napi_callback_info info) {
  napi_status status;
  int  nArgs = 1;
  size_t argc = nArgs;
  napi_value argv[1];
  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Failed to parse arguments");
  }
  int ifIndex;
  status = napi_get_value_int32(env, argv[0], &ifIndex);

  /* Some general variables */
  ULONG ulOutBufLen;
  DWORD dwRetVal;
  unsigned int i;
  napi_value result;
  int found = -1;
  status = napi_create_int32(env, found, &result);

  /* variables used for GetAdapterInfo */
  IP_ADAPTER_INFO *pAdapterInfo;
  IP_ADAPTER_INFO *pAdapter;

  pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));
  if (pAdapterInfo == NULL) {
      printf("Error allocating memory needed to call GetAdapterInfo\n");
      status = napi_create_int32(env, found, &result);
      return result;
  }

  ulOutBufLen = sizeof (IP_ADAPTER_INFO);

  if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
      FREE(pAdapterInfo);
      pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
      if (pAdapterInfo == NULL) {
          printf("Error allocating memory needed to call GetAdapterInfo\n");
          status = napi_create_int32(env, found, &result);
          return result;
      }
  }

  if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != NO_ERROR) {
      printf("GetAdaptersInfo failed with error %d\n", dwRetVal);
      if (pAdapterInfo)
          FREE(pAdapterInfo);
      status = napi_create_int32(env, 1, &result);
      return result;
  }

  InterfaceResult *localObj;
  localObj = (InterfaceResult *) MALLOC(sizeof (InterfaceResult));

  pAdapter = pAdapterInfo;
  
  while (pAdapter) {
      if ( (int)pAdapter->Index == ifIndex) {
          found = 1;
          status = napi_create_int32(env, found, &result);
          localObj->ifindex = (int) pAdapter->Index;
          localObj->Adapter = pAdapter->Description;

          localObj->Gateway = pAdapter->GatewayList.IpAddress.String;
          localObj->IP = pAdapter->IpAddressList.IpAddress.String;
          break;
      }

      pAdapter = pAdapter->Next;
  }

  if (pAdapterInfo) {
    FREE(pAdapterInfo);
    pAdapterInfo = NULL;
  }
  
  if (found > 0) {
    status = napi_create_int32(env, inet_addr(localObj->Gateway), &result);
    return result;
  }

  return result;
}


napi_value AddRoute(napi_env env, napi_callback_info info) {
    napi_status status;
    int nArgs = 4;
    size_t argc = nArgs;
    napi_value argv[4];
    status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to parse arguments");
    }

    int ifIndex;
    int destMask;
    char destinationPrefix[MAXDSTADDRESS];
    char nextHop[MAXGWADDEESS];
    status = napi_get_value_int32(env, argv[0], &ifIndex);

    int destinationPrefixSize = MAXDSTADDRESS;
    size_t* lengthResult;
    status = napi_get_value_string_utf8(env, argv[1], destinationPrefix, destinationPrefixSize, &lengthResult);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Invalid argument was passed");
    }

    status = napi_get_value_int32(env, argv[2], &destMask);

    int nextHopSize = MAXGWADDEESS;
    status = napi_get_value_string_utf8(env, argv[3], nextHop, nextHopSize, &lengthResult);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Invalid argument was passed");
    }


    DWORD retval;
    MIB_IPFORWARD_TABLE2 *routes = NULL;
    MIB_IPFORWARD_ROW2 *route;
    int idx;
    DWORD dwStatus = 0;
    napi_value result;
    status = napi_create_int32(env, 1, &result); //failure

    retval = GetIpForwardTable2(AF_INET, &routes);
    if (retval != ERROR_SUCCESS)
    {
        printf("Error! InterfaceIndex: %lu\n", retval);
        return 1;
    }

    for (idx = 0; idx < routes->NumEntries; idx++)
    {   
        route = routes->Table + idx;
        if (route->InterfaceIndex == ifIndex) { 
            route->DestinationPrefix.PrefixLength = destMask;
            route->DestinationPrefix.Prefix.Ipv4.sin_addr.S_un.S_addr = inet_addr(destinationPrefix);
            route->NextHop.Ipv4.sin_addr.S_un.S_addr = inet_addr(nextHop);
            
            NTSTATUS r = CreateIpForwardEntry2(route);
            if (r == NO_ERROR) {
                printf("Success! \n");
                status = napi_create_int32(env, 0, &result); //failure
                return result;
            } else if (r == ERROR_OBJECT_ALREADY_EXISTS) {
                printf("ERROR_OBJECT_ALREADY_EXISTS \n");
                if (DeleteIpForwardEntry2(route) == NO_ERROR) {
                    r = CreateIpForwardEntry2(route);
                    if (r == NO_ERROR) {
                        printf("Success! \n"); 
                    } else {
                        status = napi_create_int32(env, 1, &result); //failure
                        return result;
                    }
                }
                status = napi_create_int32(env, 0, &result); //failure
                return result;
            } else {
                printf("Unknow! Permission?\n");
                status = napi_create_int32(env, 1, &result); //failure
                return result;
            }
        }
    }
    return result;
}

napi_value DeleteRoute(napi_env env, napi_callback_info info) {
    napi_status status;
    int nArgs = 2;
    size_t argc = nArgs;
    napi_value argv[2];
    status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to parse arguments");
    }
    int ifIndex;
    status = napi_get_value_int32(env, argv[0], &ifIndex);
    char destinationPrefix[MAXDSTADDRESS];
    int destinationPrefixSize = MAXDSTADDRESS;
    size_t* lengthResult;
    status = napi_get_value_string_utf8(env, argv[1], destinationPrefix, destinationPrefixSize, &lengthResult);

    DWORD retval;
    MIB_IPFORWARD_TABLE2 *routes = NULL;
    MIB_IPFORWARD_ROW2 *route;
    int idx;
    DWORD dwStatus = 0;
    napi_value result;
    status = napi_create_int32(env, 1, &result); //failure

    retval = GetIpForwardTable2(AF_INET, &routes);
    if (retval != ERROR_SUCCESS)
    {
        printf("InterfaceIndex:\t %lu\n", retval);
        return result;
    }

    for (idx = 0; idx < routes->NumEntries; idx++)
    {   
        route = routes->Table + idx;
        if (route->InterfaceIndex == ifIndex && 
                route->DestinationPrefix.Prefix.Ipv4.sin_addr.S_un.S_addr == inet_addr(destinationPrefix)) { 
           
            if (DeleteIpForwardEntry2(route) == NO_ERROR) {
                printf("Deleted! \n");
                status = napi_create_int32(env, 0, &result); //failure
                return result;
            } else {
                printf("Unknow! \n");
                status = napi_create_int32(env, 1, &result); //failure
                return result;
            }
        }
    }
    return result;
}

napi_value Init(napi_env env, napi_value exports) {
    napi_status status;
    napi_value fnFindAdapter, fnGetGatewayAdapter, fnAddRoute, fnDeleteRoute;

    status = napi_create_function(env, NULL, 0, FindAdapter, NULL, &fnFindAdapter);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to wrap native function");
    }
    status = napi_create_function(env, NULL, 0, GetGatewayAdapter, NULL, &fnGetGatewayAdapter);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to wrap native function");
    }
    status = napi_create_function(env, NULL, 0, AddRoute, NULL, &fnAddRoute);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to wrap native function");
    }
    status = napi_create_function(env, NULL, 0, DeleteRoute, NULL, &fnDeleteRoute);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to wrap native function");
    }
    status = napi_set_named_property(env, exports, "findAdapter", fnFindAdapter);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to populate exports");
    }
    status = napi_set_named_property(env, exports, "getGatewayAdapter", fnGetGatewayAdapter);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to populate exports");
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