/***************************************************************************
                          exception.h  -  class Exception
                             -------------------
    begin                : jeu may 15 2003
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

#ifndef _LOG4CXX_HELPERS_EXCEPTION_H
#define _LOG4CXX_HELPERS_EXCEPTION_H

#include <log4cxx/helpers/tchar.h>

namespace log4cxx
{
	namespace helpers
	{
		class Exception
		{
		public:
			virtual tstring getMessage() = 0;
		}; // class Exception

		/** Thrown to indicate that a method has been passed 
		an illegal or inappropriate argument.*/
		class IllegalArgumentException : public Exception
		{
		public:
			IllegalArgumentException(const tstring& message)
			 : message(message) {}
			 
			virtual tstring getMessage()
				{ return message; }
				
		protected:
			tstring message;
		};
	}; // namespace helpers
}; // namespace log4cxx

#endif // _LOG4CXX_HELPERS_EXCEPTION_H
