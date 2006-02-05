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
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "NptConfig.h"
#include "NptTypes.h"
#include "NptStreams.h"
#include "NptThreads.h"
#include "NptNetwork.h"
#include "NptUtils.h"
#include "NptConstants.h"

/*----------------------------------------------------------------------
|       NPT_NetworkInterface::GetNetworkInterfaces
+---------------------------------------------------------------------*/
NPT_Result
NPT_NetworkInterface::GetNetworkInterfaces(NPT_List<NPT_NetworkInterface*>& interfaces)
{
    int net = socket(AF_INET, SOCK_DGRAM, 0);
    
    // Try to get the config until we have enough memory for it
    // According to "Unix Network Programming", some implementations
    // do not return an error when the supplied buffer is too small
    // so we need to try, increasing the buffer size every time, 
    // until we get the same size twice. We cannot assume success when
    // the returned size is smaller than the supplied buffer, because
    // some implementations can return less that the buffer size if
    // another structure does not fit.
    unsigned int buffer_size = 4096; // initial guess
    unsigned int last_size = 0;
    struct ifconf config;
    unsigned char* buffer;
    for (;buffer_size < 16384;) {
        buffer = new unsigned char[buffer_size];
        config.ifc_len = buffer_size;
        config.ifc_buf = (char*)buffer;
        if (ioctl(net, SIOCGIFCONF, &config) < 0) {
            if (errno != EINVAL || last_size != 0) {
                return NPT_FAILURE;
            }
        } else {
            if ((unsigned int)config.ifc_len == last_size) {
                // same size, we can use the buffer
                break;
            }
            // different size, we need to reallocate
            last_size = config.ifc_len;
        } 
        
        // supply 256 more bytes more next time around
        buffer_size += 256;
        delete[] buffer;
    }
    
    unsigned char *entries;
    for (entries = buffer; entries < buffer+config.ifc_len;) {
        struct ifreq* entry = (struct ifreq*)entries;
        // get the size of the addresses
        unsigned int address_length;
#if defined(NPT_CONFIG_HAVE_SOCKADDR_SA_LEN)
        address_length = sizeof(struct sockaddr) > entry->ifr_addr.sa_len ?
            sizeof(sockaddr) : entry->ifr_addr.sa_len;
#else
        switch (entry->ifr_addr.sa_family) {
            case AF_INET6:
                address_length = sizeof(struct sockaddr_in6);
                break;
                
            default:
                address_length = sizeof(struct sockaddr);
                break;
        }
#endif
                
        // point to the next entry
        entries += address_length + sizeof(entry->ifr_name);
        
        // ignore anything except AF_INET addresses
        if (entry->ifr_addr.sa_family != AF_INET) {
            continue;
        }
        
        // get detailed info about the interface
        struct ifreq query = *entry;
        if (ioctl(net, SIOCGIFFLAGS, &query) < 0) continue;
        
        // process the flags
        NPT_Flags flags = 0;
        if ((query.ifr_flags & IFF_UP) == 0) {
            // the interface is not up, ignore it
            continue;
        }
        if (query.ifr_flags & IFF_BROADCAST) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_BROADCAST;
        }
        if (query.ifr_flags & IFF_LOOPBACK) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_LOOPBACK;
        }
        if (query.ifr_flags & IFF_POINTOPOINT) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_POINT_TO_POINT;
        }
        if (query.ifr_flags & IFF_PROMISC) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_PROMISCUOUS;
        }
        if (query.ifr_flags & IFF_MULTICAST) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_MULTICAST;
        }
        
        // get the mac address        
        NPT_MacAddress mac;
#if defined(SIOCGIFHWADDR)
        if (ioctl(net, SIOCGIFHWADDR, &query) == 0) {
            NPT_MacAddress::Type type;
            unsigned int length = 6; // default to 48 bits
            //printf("***** type = %d\n", query.ifr_addr.sa_family);
            switch (query.ifr_addr.sa_family) {
                case ARPHRD_ETHER:
                    type = NPT_MacAddress::TYPE_ETHERNET;
                    break;
                    
                case ARPHRD_LOOPBACK:
                    type = NPT_MacAddress::TYPE_LOOPBACK;
                    length = 0;
                    break;
                          
                case ARPHRD_PPP:
                    type = NPT_MacAddress::TYPE_PPP;
                    length = 0;
                    break;
                    
                case ARPHRD_IEEE80211:
                    type = NPT_MacAddress::TYPE_IEEE_802_11;
                    break;
                                   
                default:
                    type = NPT_MacAddress::TYPE_UNKNOWN;
                    break;
            }
                
            mac.SetAddress(type, (const unsigned char*)query.ifr_addr.sa_data, length);
        }
#endif

        // create an interface object
        NPT_NetworkInterface* interface = new NPT_NetworkInterface(entry->ifr_name, mac, flags);

        // primary address
        NPT_IpAddress primary_address(ntohl(((struct sockaddr_in*)&entry->ifr_addr)->sin_addr.s_addr));

        // broadcast address
        NPT_IpAddress broadcast_address;
#if defined(SIOCGIFBRDADDR)
        if (flags & NPT_NETWORK_INTERFACE_FLAG_BROADCAST) {
            if (ioctl(net, SIOCGIFBRDADDR, &query) == 0) {
                broadcast_address.Set(ntohl(((struct sockaddr_in*)&query.ifr_addr)->sin_addr.s_addr));
            }
        }
#endif

        // point to point address
        NPT_IpAddress destination_address;
#if defined(SIOCGIFDSTADDR)
        if (flags & NPT_NETWORK_INTERFACE_FLAG_POINT_TO_POINT) {
            if (ioctl(net, SIOCGIFDSTADDR, &query) == 0) {
                destination_address.Set(ntohl(((struct sockaddr_in*)&query.ifr_addr)->sin_addr.s_addr));
            }
        }
#endif

        // netmask
        NPT_IpAddress netmask(0xFFFFFFFF);
#if defined(SIOCGIFNETMASK)
        if (ioctl(net, SIOCGIFNETMASK, &query) == 0) {
            netmask.Set(ntohl(((struct sockaddr_in*)&query.ifr_addr)->sin_addr.s_addr));
        }
#endif

        // create the interface object
        NPT_NetworkInterfaceAddress iface_address(
            primary_address,
            broadcast_address,
            destination_address,
            netmask);
        interface->AddAddress(iface_address);  
         
        // add the interface to the list
        interfaces.Add(interface);   
    }

    // free resources
    delete[] buffer;
    close(net);
    
    return NPT_SUCCESS;
}







