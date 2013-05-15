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

#include <net/if_dl.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <string.h>

bool getNetworkInterface(NetInterfaceInfo **pIPInfo,Int32 *pszIPInfo)
{
	struct sockaddr_in *pAddr;
	int Success = 0;
	UInt32 dwIP, dwNetmask;
	UInt8 bMAC[MAC_ADDR_SIZE];
	NetInterfaceInfo *IPInfo = NULL;
	int szIPInfo = 0;
	
	struct ifaddrs *if_addrs = NULL;
	struct ifaddrs *if_addr = NULL;
	
	void *tmp = NULL;
	char buf[INET6_ADDRSTRLEN];
	
	memset(bMAC,0,MAC_ADDR_SIZE);
	
	if (0 == getifaddrs(&if_addrs))
	{
		for (if_addr = if_addrs; if_addr != NULL; if_addr = if_addr->ifa_next)
		{
			if( if_addr->ifa_name[0] == 'l' &&
			   if_addr->ifa_name[1] == 'o' &&
			   if_addr->ifa_name[2] == '0' )
			{
				continue;
			}
			
			if (if_addr->ifa_addr->sa_family == AF_INET)
            {
				tmp = &((struct sockaddr_in *)if_addr->ifa_addr)->sin_addr;
			} else
            {
				tmp = &((struct sockaddr_in6 *)if_addr->ifa_addr)->sin6_addr;
			}
			
			pAddr = (struct sockaddr_in*)if_addr->ifa_addr;
			dwIP = pAddr->sin_addr.s_addr;
            
			if(dwIP==0)
			{
				continue;
			}
			
			if(0== inet_ntop(if_addr->ifa_addr->sa_family,tmp,buf,sizeof(buf)) )
			{
				continue;
			}
			
			dwNetmask = 0;
			
			IPInfo = realloc(IPInfo, (szIPInfo + 1) * sizeof(NetInterfaceInfo));
			memset(&IPInfo[szIPInfo], 0, sizeof(NetInterfaceInfo));
			IPInfo[szIPInfo].name = strdup(if_addr->ifa_name);
			
			IPInfo[szIPInfo].description = NULL;		// not for linux
			
			IPInfo[szIPInfo].ip = dwIP;
			
			IPInfo[szIPInfo].netmask = dwNetmask;
			
			memcpy(&IPInfo[szIPInfo].mac, bMAC, MAC_ADDR_SIZE);
			
			szIPInfo++;
			
		}
	}
	
	Success = 1;
	
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
