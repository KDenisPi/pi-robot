/*
* Receiving network interfaces information
*
* Created by Denis Kudia
* 
*/
#include <cstring>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netdb.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include "networkinfo.h"

namespace piutils {
namespace netinfo {

#define INFINITY_LIFE_TIME      0xFFFFFFFFU

void NetInfo::get_ip_list(bool is_ip4, NetInfo* net_info) {

      struct {
        struct nlmsghdr n;
        struct ifaddrmsg r;
      } req;

      int status;
      char buf[16384];

      struct rtattr *rta;
      struct nlmsghdr *nlmp;
      struct ifaddrmsg *rtmp;
      struct rtattr *rtatp;
      int rtattrlen;

      /* Internet address.
        struct in_addr {
          uint32_t       s_addr;     // address in network byte order
        };
      */
      struct in_addr *inp;

      /*
        struct in6_addr {
               unsigned char   s6_addr[16];   // IPv6 address
        };      
      */
      struct in6_addr *in6p;

      struct ifa_cacheinfo *cache_info;
      char *label;

      int fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);

      /* We use RTM_GETADDR to fetch the ip address from the kernel interface table *
         * So what we do here is pretty simple, we populate the msg structure (req)   *
         * the size of the message buffer is specified to netlink message header, and *
         * flags values are set as NLM_F_ROOT | NLM_F_REQUEST. The request flag must  *
         * be set for all messages requesting the data from kernel. The root flag is  *
         * used to notify the kernel to return the full tabel. Another flag (not used)*
         * is NLM_F_MATCH. This is used to get only speficed entried in the table.    *
         * At the time of writing this program this flag is not implemented in kernel */

      std::memset(&req, 0, sizeof(req));
      req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
      req.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT;
      req.n.nlmsg_type = RTM_GETADDR;


      /* AF_INET6 is used to signify the kernel to fetch only ipv6 entires.         *
        * Replacing this with AF_INET will fetch ipv4 address table.                 */

      req.r.ifa_family = (is_ip4 ? AF_INET : AF_INET6);

      /* Fill up all the attributes for the rtnetlink header. The code is pretty easy*
        * to understand. The lenght is very important. We use 16 to signify the ipv6  *
       * address. If the user chooses to use AF_INET (ipv4) the length has to be     *
        * RTA_LENGTH(4) */

      rta = (struct rtattr *)(((char *)&req) + NLMSG_ALIGN(req.n.nlmsg_len));
      rta->rta_len = (is_ip4 ? RTA_LENGTH(4) : RTA_LENGTH(16));

      /* Time to send and recv the message from kernel */

      status = send(fd, &req, req.n.nlmsg_len, 0);
      if(status >= 0){

        status = recv(fd, buf, sizeof(buf), 0);
        if(status == 0){
          //no data
        }
        else if(status > 0){
      /* Typically the message is stored in buf, so we need to parse the message to *
        * get the required data for our display. */

          for(nlmp = (struct nlmsghdr *)buf; status > sizeof(*nlmp);){
            int len = nlmp->nlmsg_len;
            int req_len = len - sizeof(*nlmp);

            if (req_len<0 || len>status) {
                    //printf("error\n");
                    break;
            }

            if (!NLMSG_OK(nlmp, status)) {
                    //printf("NLMSG not OK\n");
                    break;
            }

            rtmp = (struct ifaddrmsg *)NLMSG_DATA(nlmp);
            rtatp = (struct rtattr *)IFA_RTA(rtmp);

            /* Start displaying the index of the interface */
            std::shared_ptr<NetInterface> itf = net_info->get_itf(rtmp->ifa_index);

            //printf("\n---------- Index Of Iface= %d ---------------\n",rtmp->ifa_index);

            rtattrlen = IFA_PAYLOAD(nlmp);

            for (; RTA_OK(rtatp, rtattrlen); rtatp = RTA_NEXT(rtatp, rtattrlen)) {
            
              /* Here we hit the fist chunk of the message. Time to validate the    *
                * the type. For more info on the different types see man(7) rtnetlink*
                * The table below is taken from man pages.                           *
                * Attributes                                                         *
                * rta_type        value type             description                 *
                * -------------------------------------------------------------      *
                * IFA_UNSPEC      -                      unspecified.                *
                * IFA_ADDRESS     raw protocol address   interface address           *
                * IFA_LOCAL       raw protocol address   local address               *
                * IFA_LABEL       asciiz string          name of the interface       *
                * IFA_BROADCAST   raw protocol address   broadcast address.          *
                * IFA_ANYCAST     raw protocol address   anycast address             *
                * IFA_CACHEINFO   struct ifa_cacheinfo   Address information.        */
           
                  
              if(rtatp->rta_type == IFA_CACHEINFO){
                      cache_info = (struct ifa_cacheinfo *)RTA_DATA(rtatp);
                      if (cache_info->ifa_valid == INFINITY_LIFE_TIME){
                          /* valid_lft forever */
                      }
                      else{
                          //printf("valid_lft %usec\n", cache_info->ifa_valid);
                      }

                      if (cache_info->ifa_prefered == INFINITY_LIFE_TIME){
                          /* preferred_lft forever */
                      }
                      else{
                          //printf(" preferred_lft %usec\n",cache_info->ifa_prefered);
                      }
              }

              /* NOTE: All the commented code below can be used as it is for ipv4 table */
              if(rtatp->rta_type == IFA_ADDRESS || rtatp->rta_type == IFA_LOCAL || rtatp->rta_type == IFA_BROADCAST || rtatp->rta_type == IFA_ANYCAST){
                  if(is_ip4)
                    inp = (struct in_addr *)RTA_DATA(rtatp);
                  else  
                    in6p = (struct in6_addr *)RTA_DATA(rtatp);

                  if(rtatp->rta_type == IFA_ADDRESS){
                    if(is_ip4)
                      itf->add_ip_v4(std::shared_ptr<IpAddress_V4>(new IpAddress_V4(inp, "address 0")));
                    else
                      itf->add_ip_v6(std::shared_ptr<IpAddress_V6>(new IpAddress_V6(in6p, "address 0")));
                  }
                  else if(rtatp->rta_type == IFA_LOCAL){
                    if(is_ip4)
                      itf->add_ip_v4(std::shared_ptr<IpAddress_V4>(new IpAddress_V4(inp, "address 1")));
                    else
                      itf->add_ip_v6(std::shared_ptr<IpAddress_V6>(new IpAddress_V6(in6p, "address 1")));
                  }
                  else if(rtatp->rta_type == IFA_BROADCAST){
                    if(is_ip4)
                      itf->add_ip_v4(std::shared_ptr<IpAddress_V4>(new IpAddress_V4(inp, "Broadcast address")));
                    else
                      itf->add_ip_v6(std::shared_ptr<IpAddress_V6>(new IpAddress_V6(in6p, "Broadcast address")));
                  }
                  else if(rtatp->rta_type == IFA_ANYCAST){
                    if(is_ip4)
                      itf->add_ip_v4(std::shared_ptr<IpAddress_V4>(new IpAddress_V4(inp, "Anycast address")));
                    else
                      itf->add_ip_v6(std::shared_ptr<IpAddress_V6>(new IpAddress_V6(in6p, "Anycast address")));
                  }
              }

              /* Interface label*/
              if(rtatp->rta_type == IFA_LABEL){
                label = (char*) RTA_DATA(rtatp);
                itf->set_label(label);
              }
            }
            status -= NLMSG_ALIGN(len);
            nlmp = (struct nlmsghdr*)((char*)nlmp + NLMSG_ALIGN(len));
          }
        }
      }

      close(fd);
}

} //namespace netinfo
} //namespace piutils