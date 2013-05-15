// Copyright (c) 2013, Baptiste Burles
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// * Neither the name of the University of California, Berkeley nor the
//   names of its contributors may be used to endorse or promote products
//   derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Baptiste Burles AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Baptiste Burles AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "../../include/babcode.h"

#include "../../source/networkstack-private.h"

#include <linux/sockios.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

bool getNetworkInterface(NetInterfaceInfo **pIPInfo,Int32 *pszIPInfo)
{
	int sock;
	struct ifconf IfConf;
	struct ifreq IfReq;
	struct sockaddr_in *pAddr;
	int i, Success = 0;
	UInt32 dwIP, dwNetmask;
	UInt8 bMAC[MAC_ADDR_SIZE];
	NetInterfaceInfo *IPInfo = NULL;
	int szIPInfo = 0;
    
    
	memset(&IfReq, 0, sizeof(struct ifreq));
	memset(&IfConf, 0, sizeof(struct ifconf));
    
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock== -1)
	{
		LOG_ERR1("socket");
		return false;
	}
    
	if(ioctl(sock, SIOCGIFCONF, &IfConf) == -1)
	{
		LOG_ERR1("ioctl(SIOCGIFCONF)");
		goto _end_;
	}
    
	IfConf.ifc_buf = (char*)malloc(sizeof(struct ifconf) * IfConf.ifc_len);
    
	if(ioctl(sock, SIOCGIFCONF, &IfConf) == -1)
	{
		LOG_ERR1("ioctl(SIOCGIFCONF)");
		goto _end_;
	}
    
	for(i=0;i<(IfConf.ifc_len / sizeof(struct ifreq));i++)
	{
		// ip address
		strcpy(IfReq.ifr_name, IfConf.ifc_req[i].ifr_name);
        
		if(ioctl(sock, SIOCGIFADDR, &IfReq) == -1)
		{
			LOG_ERR1("ioctl(SIOCGIFADDR)");
			continue;
		}
        
		pAddr = (struct sockaddr_in*)&IfReq.ifr_addr;
		dwIP = pAddr->sin_addr.s_addr;
		//printf("%s %s", IfReq.ifr_name, inet_ntoa(sock->sin_addr));
        
		/*
         // broadcast
         if(ioctl(Socket, SIOCGIFBRDADDR, &IfReq) == -1)
         {
         perror("ioctl");
         }
         pAddr = (struct sockaddr_in *) &IfReq.ifr_broadaddr;
         printf(" %s", inet_ntoa(sock->sin_addr));
         */
        
		// netmask
		if(ioctl(sock, SIOCGIFNETMASK, &IfReq) == -1)
		{
			LOG_ERR1("ioctl(SIOCGIFNETMASK)");
			continue;
		}
		pAddr = (struct sockaddr_in *) &IfReq.ifr_netmask;
		dwNetmask = pAddr->sin_addr.s_addr;
		//printf(" %s ", inet_ntoa(sock->sin_addr));
        
		// hw_addr
		if(ioctl(sock, SIOCGIFHWADDR, &IfReq) == -1)
		{
			LOG_ERR1("ioctl(SIOCGIFHWADDR)");
			continue;
		}
		memcpy(bMAC, IfReq.ifr_hwaddr.sa_data, MAC_ADDR_SIZE);
        
		/*
         for(j=0 ; j<MAC_ADDR_SIZE ; j++)
         printf("%2.2X:",IfReq.ifr_hwaddr.sa_data[j]& 0xFF);
         */
        
		/*
         // show tx/rx queue len
         if (ioctl(Socket, SIOCGIFTXQLEN, &IfReq) == -1)
         {
         perror("ioctl");
         }
         printf(" %d\n", IfReq.ifr_qlen);
         */
        
		IPInfo = realloc(IPInfo, (szIPInfo + 1) * sizeof(NetInterfaceInfo));
		memset(&IPInfo[szIPInfo], 0, sizeof(NetInterfaceInfo));
		IPInfo[szIPInfo].name = strdup(IfReq.ifr_name);
		IPInfo[szIPInfo].description = NULL;		// not for linux
		IPInfo[szIPInfo].ip = dwIP;
		IPInfo[szIPInfo].netmask = dwNetmask;
		memcpy(&IPInfo[szIPInfo].mac, bMAC, MAC_ADDR_SIZE);
		szIPInfo++;
	}
    
	Success = 1;
    
_end_:
	if(sock)
		close(sock);
    
	if(IfConf.ifc_buf)
		free(IfConf.ifc_buf);
    
	if(Success)
	{
		*pIPInfo = IPInfo;
		*pszIPInfo = szIPInfo;
	}
	else
	{
		return false;
	}
    
	return true;
}
