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
// * Neither the name of Baptiste Burles nor the
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

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x)   HeapFree(GetProcessHeap(), 0, (x))

PMIB_IPADDRTABLE pIPAddrTable;

static void freeIpTable(void)
{
	FREE(pIPAddrTable);
    pIPAddrTable = 0;
}

static uint32_t getMask(uint32_t ip)
{
	uint32_t i;

	for(i=0; i< pIPAddrTable->dwNumEntries;i++)
	{
		if( ip == (u_long) pIPAddrTable->table[i].dwAddr )
		{
			return (u_long) pIPAddrTable->table[i].dwMask;
		}
	}
	return 0;
}

static uint8_t getIpTable(void)
{
	// Variables used by GetIpAddrTable
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    // Variables used to return error message
    LPVOID lpMsgBuf;

    // Before calling AddIPAddress we use GetIpAddrTable to get
    // an adapter to which we can add the IP.
    pIPAddrTable = (MIB_IPADDRTABLE *) MALLOC(sizeof (MIB_IPADDRTABLE));

    if (pIPAddrTable)
	{
        // Make an initial call to GetIpAddrTable to get the
        // necessary size into the dwSize variable
        if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) ==
            ERROR_INSUFFICIENT_BUFFER)
		{
            FREE(pIPAddrTable);
            pIPAddrTable = (MIB_IPADDRTABLE *) MALLOC(dwSize);

        }
        if (pIPAddrTable == NULL)
		{
            LOG("Memory allocation failed for GetIpAddrTable");
            return 1;
        }
    }
    // Make a second call to GetIpAddrTable to get the
    // actual data we want
    if ( (dwRetVal = GetIpAddrTable( pIPAddrTable, &dwSize, 0 )) != NO_ERROR )
	{
        LOG("GetIpAddrTable failed with error %d", dwRetVal);
        if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),       // Default language
                          (LPTSTR) & lpMsgBuf, 0, NULL))
		{
            LOG("\tError: %s", lpMsgBuf);
            LocalFree(lpMsgBuf);
        }
        return 1;
    }

	return 0;
}

bool getNetworkInterface(NetInterfaceInfo **pIPInfo,int32_t *pszIPInfo)
{
	uint16_t i16;
	uint8_t  i8;
	uint32_t len;

	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;

	struct sockaddr_in  *ipaddr;

	ULONG outBufLen;
	DWORD dwRetVal = 0;
	NetInterfaceInfo *IPInfo = NULL;
	int szIPInfo = 0;

	ULONG family = AF_INET;
	ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

	if(getIpTable()!=0)
		return false;

	outBufLen = sizeof (IP_ADAPTER_ADDRESSES);
    pAddresses = (IP_ADAPTER_ADDRESSES *) malloc(outBufLen);
    if (pAddresses == NULL)
	{
        LOG("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
        return false;
    }

    // Make an initial call to GetAdaptersAddresses to get the
    // size needed into the outBufLen variable
    if (GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen)== ERROR_BUFFER_OVERFLOW)
	{
        free(pAddresses);
        pAddresses = (IP_ADAPTER_ADDRESSES *) malloc(outBufLen);
    }

    if (pAddresses == NULL)
	{
        LOG("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
        return false;
    }


	dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
	if(dwRetVal == NO_ERROR)
	{
		//IP_ADDR_STRING *IPList;
		//char *IP, *Netmask;


		pCurrAddresses = pAddresses;

		//TODO get netmask, name and mac addr
		while(pCurrAddresses)
		{
			//DumpAdapter(pAdapter);

			// skip non ethernet adapter
			if(pCurrAddresses->IfType  != IF_TYPE_ETHERNET_CSMACD &&
               pCurrAddresses->IfType != IF_TYPE_IEEE80211 )
			{
				pCurrAddresses = pCurrAddresses->Next;
				continue;
			}

			if (pCurrAddresses->FirstUnicastAddress != NULL)
			{
				IPInfo = (NetInterfaceInfo *)realloc(IPInfo, (szIPInfo + 1) * sizeof(NetInterfaceInfo));
				memset(&IPInfo[szIPInfo], 0, sizeof(NetInterfaceInfo));

				ipaddr = (struct sockaddr_in *)pCurrAddresses->FirstUnicastAddress->Address.lpSockaddr;

				IPInfo[szIPInfo].pkSize = pCurrAddresses->Mtu;
				IPInfo[szIPInfo].ip   = ntohl(ipaddr->sin_addr.s_addr);

				IPInfo[szIPInfo].netmask = getMask(IPInfo[szIPInfo].ip);

				if(pCurrAddresses->PhysicalAddressLength == MAC_ADDR_SIZE)
					memcpy(&IPInfo[szIPInfo].mac, pCurrAddresses->PhysicalAddress, MAC_ADDR_SIZE);

				len = wcslen( pCurrAddresses->FriendlyName);

				IPInfo[szIPInfo].name = (int8_t *)malloc(len+1);
				for(i16=0,i8=0;i16<len;i16++,i8++)
				{
					IPInfo[szIPInfo].name[i8]=(int8_t)pCurrAddresses->FriendlyName[i16];
				}
				IPInfo[szIPInfo].name[i8]=0;

				len = wcslen( pCurrAddresses->Description);

				IPInfo[szIPInfo].description = (int8_t *)malloc(len+1);
				for(i16=0,i8=0;i16<len;i16++,i8++)
				{
					IPInfo[szIPInfo].description[i8]=(int8_t)pCurrAddresses->Description[i16];
				}
				IPInfo[szIPInfo].description[i8]=0;

				szIPInfo++;
			}

			pCurrAddresses = pCurrAddresses->Next;
		}
	}
	else
	{
        return false;
	}

	free(pAddresses);
	freeIpTable();

	*pIPInfo = IPInfo;
	*pszIPInfo = szIPInfo;

	return true;
}
