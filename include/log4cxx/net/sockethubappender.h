/***************************************************************************
                          sokecthubappender.h  -  class SocketHubAppender
                             -------------------
    begin                : jeu mai 8 2003
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

#ifndef _LOG4CXX_NET_SOCKET_HUB_APPENDER_H
#define _LOG4CXX_NET_SOCKET_HUB_APPENDER_H
 
#include <log4cxx/appenderskeleton.h>
#include <log4cxx/helpers/socket.h>
#include <log4cxx/helpers/thread.h>
#include <vector>
#include <log4cxx/helpers/socket.h>

namespace log4cxx
{
	namespace helpers
	{
		class SocketOutputStream;
		typedef helpers::ObjectPtr<SocketOutputStream> SocketOutputStreamPtr;
	};

	namespace net
	{
		/**
		Sends {@link spi::LoggingEvent LoggingEvent} objects to a set of remote log
		servers, usually a SocketNode.

		<p>Acts just like SocketAppender except that instead of
		connecting to a given remote log server,
		<code>SocketHubAppender</code> accepts connections from the remote
		log servers as clients.  It can accept more than one connection.
		When a log event is received, the event is sent to the set of
		currently connected remote log servers. Implemented this way it does
		not require any update to the configuration file to send data to
		another remote log server. The remote log server simply connects to
		the host and port the <code>SocketHubAppender</code> is running on.

		<p>The <code>SocketHubAppender</code> does not store events such
		that the remote side will events that arrived after the
		establishment of its connection. Once connected, events arrive in
		order as guaranteed by the TCP protocol.

		<p>This implementation borrows heavily from the SocketAppender.

		<p>The SocketHubAppender has the following characteristics:

		<ul>

		<p><li>If sent to a SocketNode, logging is non-intrusive as
		far as the log event is concerned. In other words, the event will be
		logged with the same time stamp, NDC,
		location info as if it were logged locally.

		<p><li><code>SocketHubAppender</code> does not use a layout. It
		ships a serialized spi::LoggingEvent object to the remote side.

		<p><li><code>SocketHubAppender</code> relies on the TCP
		protocol. Consequently, if the remote side is reachable, then log
		events will eventually arrive at remote client.

		<p><li>If no remote clients are attached, the logging requests are
		simply dropped.

		<p><li>Logging events are automatically <em>buffered</em> by the
		native TCP implementation. This means that if the link to remote
		client is slow but still faster than the rate of (log) event
		production, the application will not be affected by the slow network
		connection. However, if the network connection is slower then the
		rate of event production, then the local application can only
		progress at the network rate. In particular, if the network link to
		the the remote client is down, the application will be blocked.

		<p>On the other hand, if the network link is up, but the remote
		client is down, the client will not be blocked when making log
		requests but the log events will be lost due to client
		unavailability. 

		<p>The single remote client case extends to multiple clients
		connections. The rate of logging will be determined by the slowest


		link.

		<p><li>If the application hosting the <code>SocketHubAppender</code> 
		exits before the <code>SocketHubAppender</code> is closed either
		explicitly or subsequent to garbage collection, then there might
		be untransmitted data in the pipe which might be lost. This is a
		common problem on Windows based systems.

		<p>To avoid lost data, it is usually sufficient to #close
		the <code>SocketHubAppender</code> either explicitly or by calling
		the LogManager#shutdown method before
		exiting the application.

		</ul>
		*/

		class SocketHubAppender : public AppenderSkeleton
		{
		private:
			/**
			The default port number of the ServerSocket will be created on.
			*/
			static int DEFAULT_PORT;
			
			int port;
			std::vector<helpers::SocketOutputStreamPtr> oosList;
			bool locationInfo;
			
		public:
			SocketHubAppender();
			~SocketHubAppender();
			
			/**
			Connects to remote server at <code>address</code> and <code>port</code>.
			*/
			SocketHubAppender(int port) ;
			
			/**
			Set up the socket server on the specified port.
			*/
			virtual void activateOptions();
			
		    /**
		    Set options
		    */
			virtual void setOption(const std::string& option, const std::string& value);

			/**
			Close this appender. 
			<p>This will mark the appender as closed and
			call then #cleanUp method.
			*/
			virtual void close();
			
			/**
			Release the underlying ServerMonitor thread, and drop the connections
			to all connected remote servers. */
			void cleanUp();
			
			/**
			Append an event to all of current connections. */
			virtual void append(const spi::LoggingEvent& event);
			
			/**
			The SocketHubAppender does not use a layout. Hence, this method returns
			<code>false</code>. */
			virtual bool requiresLayout()
				{ return false; }
			
			/**
			The <b>Port</b> option takes a positive integer representing
			the port where the server is waiting for connections. */
			inline void setPort(int port)
				{ this->port = port; }
			
			/**
			Returns value of the <b>Port</b> option. */
			inline int getPort() const
				{ return port; }
			
			/**
			The <b>LocationInfo</b> option takes a boolean value. If true,
			the information sent to the remote host will include location
			information. By default no location information is sent to the server. */
			inline void setLocationInfo(bool locationInfo)
				{  this->locationInfo = locationInfo; }
			
			/**
			Returns value of the <b>LocationInfo</b> option. */
			inline bool getLocationInfo() const
				{ return locationInfo; }
			
			/**
			Start the ServerMonitor thread. */
		private:
			void startServer();
			
			/**
			This class is used internally to monitor a ServerSocket
			and register new connections in a vector passed in the
			constructor. */
			class ServerMonitor : 
				public helpers::Runnable,
					public helpers::ObjectImpl
			{
			private:
				int port;
				std::vector<helpers::SocketOutputStreamPtr> oosList;
				bool keepRunning;
				helpers::Thread * monitorThread;
				
			public:
				/**
				Create a thread and start the monitor. */
				ServerMonitor(int port, const std::vector<helpers::SocketOutputStreamPtr>& oosList);
			
				/**
				Stops the monitor. This method will not return until
				the thread has finished executing. */
				void stopMonitor();
				
				/**
				Method that runs, monitoring the ServerSocket and adding connections as
				they connect to the socket. */
				void run();
			}; // class ServerMonitor

			typedef helpers::ObjectPtr<ServerMonitor> ServerMonitorPtr;
			ServerMonitorPtr serverMonitor;
		}; // class SocketHubAppender
	}; // namespace net
}; // namespace log4cxx

#endif // _LOG4CXX_NET_SOCKET_HUB_APPENDER_H
