/***************************************************************************
                          inetaddress.cpp  -  class InetAddress
                             -------------------
    begin                : ven mai 9 2003
    copyright            : (C) 2003 by Michael CATANZARITI
    email                : mcatan@free.fr
 ***************************************************************************/

/***************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.      *
 *                                                                         *
 * This software is published under the terms of the Apache Software       *
 * License version 1.1, a copy of which has been included with this        *
 * distribution in the LICENSE.txt file.                                   *
 ***************************************************************************/

#include <log4cxx/helpers/inetaddress.h>
#include <log4cxx/helpers/loglog.h>

using namespace log4cxx::helpers;

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#endif
 
InetAddress::InetAddress() : address(0)
{
}
 
/** Returns the raw IP address of this InetAddress  object.
*/
int InetAddress::getAddress() const
{
	return address;
}

/** Determines all the IP addresses of a host, given the host's name.
*/
std::vector<InetAddress> InetAddress::getAllByName(const tstring& host)
{
	struct hostent * hostinfo;

	USES_CONVERSION;
	hostinfo = ::gethostbyname(T2A(host.c_str()));

	if (hostinfo == 0)
	{
		LogLog::error(_T("Cannot get information about host :") + host);
		return std::vector<InetAddress>();
	}
	else
	{
		std::vector<InetAddress> addresses;
		InetAddress address;
		char ** addrs;

		while(*addrs != 0)
		{
			address.address = ntohl(((in_addr *)*addrs)->s_addr);
			addresses.push_back(address);
		}
		
		return addresses;
	}

}

/** Determines the IP address of a host, given the host's name.
*/
InetAddress InetAddress::getByName(const tstring& host)
{
	struct hostent * hostinfo;
	InetAddress address;

	USES_CONVERSION;
	hostinfo = ::gethostbyname(T2A(host.c_str()));

	if (hostinfo == 0)
	{
		LogLog::error(_T("Cannot get information about host: ") + host);
		address.address = 0;
	}
	else
	{
		address.address = ntohl(((in_addr *)*hostinfo->h_addr_list)->s_addr);
	}

	return address;
}

/** Returns the IP address string "%d.%d.%d.%d".
*/
tstring InetAddress::getHostAddress() const
{
	USES_CONVERSION;
	in_addr addr;
	addr.s_addr = htonl(address);
	return A2T(::inet_ntoa(addr));
}

/** Gets the host name for this IP address.
*/
tstring InetAddress::getHostName() const
{
	tstring hostName;
	struct hostent * hostinfo;

	in_addr addr;
	addr.s_addr = htonl(address);
	hostinfo = ::gethostbyaddr((const char *)&addr, sizeof(addr), AF_INET);

	if (hostinfo != 0)
	{
		USES_CONVERSION;
		hostName = A2T(hostinfo->h_name);
	}
	else
	{
		tostringstream oss;
		oss << _T("Cannot get host name: ") << address;
		LogLog::error(oss.str());
	}

	return hostName;
}

/** Returns the local host.
*/
InetAddress InetAddress::getLocalHost()
{
	InetAddress address;
	address.address = ntohl(inet_addr("127.0.0.1"));
	return address;
}

/** Utility routine to check if the InetAddress is an IP multicast address.
IP multicast address is a Class D address
i.e first four bits of the address are 1110.
*/
bool InetAddress::isMulticastAddress() const
{
	return (address & 0xF000) == 0xE000;
}

/** Converts this IP address to a String.
*/
tstring InetAddress::toString() const
{
	return getHostName() + _T("/") + getHostAddress();
}
