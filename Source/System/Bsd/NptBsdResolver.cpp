/*****************************************************************
|
|      Neptune - Network :: BSD Implementation
|
|      (c) 2001-2005 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#include "NptConfig.h"
#include "NptTypes.h"
#include "NptNetwork.h"
#include "NptUtils.h"
#include "NptConstants.h"
#include "NptResults.h"
#include "NptSockets.h"

#if defined(NPT_CONFIG_HAVE_GETADDRINFO) 
/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
const unsigned int NPT_BSD_NETWORK_MAX_ADDR_LIST_LENGTH = 1024;

/*----------------------------------------------------------------------
|   MapGetAddrInfoErrorCode
+---------------------------------------------------------------------*/
static NPT_Result
MapGetAddrInfoErrorCode(int error_code)
{
    switch (error_code) {
        case EAI_NONAME:
            return NPT_ERROR_HOST_UNKNOWN;
            
        case EAI_AGAIN:
            return NPT_ERROR_TIMEOUT;
            
        default: 
            return NPT_FAILURE;
    }
}

/*----------------------------------------------------------------------
|   NPT_NetworkNameResolver::Resolve
+---------------------------------------------------------------------*/
NPT_Result
NPT_NetworkNameResolver::Resolve(const char*              name, 
                                 NPT_List<NPT_IpAddress>& addresses,
                                 NPT_Timeout              /*timeout*/)
{
    // empty the list first
    addresses.Clear();
    
    // get the addr list
    struct addrinfo *infos = NULL;
    int result = getaddrinfo(name,  /* hostname */
                             NULL,  /* servname */
                             NULL,  /* hints    */
                             &infos /* res      */);
    if (result != 0) {
        return MapGetAddrInfoErrorCode(result);
    }
    
    for (struct addrinfo* info = infos; 
         info && addresses.GetItemCount() < NPT_BSD_NETWORK_MAX_ADDR_LIST_LENGTH; 
         info = info->ai_next) {
        if (info->ai_family != AF_INET) continue;
        if (info->ai_addrlen != sizeof(struct sockaddr_in)) continue;
        if (info->ai_protocol != IPPROTO_TCP) continue; 
        struct sockaddr_in* inet_addr = (struct sockaddr_in*)info->ai_addr;
        NPT_IpAddress address(ntohl(inet_addr->sin_addr.s_addr));
        addresses.Add(address);
    }
    freeaddrinfo(infos);
    
    return NPT_SUCCESS;
}
#endif
