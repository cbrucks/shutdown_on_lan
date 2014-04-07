// Guard
#ifndef __CHRIS_BRUCKS_CUSTOM_LOGGER__
#define __CHRIS_BRUCKS_CUSTOM_LOGGER__

#include "get_platform.h"

// stdio for C/C++
#include <stdio.h>

// Platform dependent includes
#if PLATFORM == PLATFORM_WINDOWS
	#include <windows.h>
#elif PLATFORM == PLATFORM_UNIX
	#include <time.h>
	#include <sys/time.h>
#endif
	

/******************************************************************************
*  Function Name: get_last_error_message
*
*  Revision log:
*                     Resp
*   Date              Pers    Description
* -----------------------------------------------------------------------------
*    23 Sep 2013       CAB     Original Release
*
*  Description:
*
*    This is a cross-platform method that returns a string representation of 
*    the last system error.
*
*  Application Functions Called: None
*
*  Inputs:
*    Globals: None
*
*    Class Variables: None
*
*    Passed: None
*
*  Outputs:
*    Globals:  None
*
*    Class Variables: None
*
*    Passed:   None
*
*    Returned Value: 
*      string						A string representation of the last error that occured.
*
******************************************************************************/
_inline void get_last_error_message(char *error_msg)
{
#if PLATFORM == PLATFORM_WINDOWS
	DWORD dwLastError = GetLastError();
	LPTSTR error;

	FormatMessage(
	   // use system message tables to retrieve error text
	   FORMAT_MESSAGE_FROM_SYSTEM
	   // allocate buffer on local heap for error text
	   |FORMAT_MESSAGE_ALLOCATE_BUFFER
	   // Important! will fail otherwise, since we're not 
	   // (and CANNOT) pass insertion parameters
	   |FORMAT_MESSAGE_IGNORE_INSERTS,
	   NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
	   dwLastError,
	   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	   (LPTSTR)&error,  // output 
	   0, // minimum size for output buffer
	   NULL);   // arguments - see note 

		error_msg = (char *)calloc(strlen(error)+1, sizeof(char));
		memcpy(error_msg, error, strlen(error));

		LocalFree(error);

#elif PLATFORM == PLATFORM_UNIX
	char buffer[ 256 ];
	char *msg = strerror_r( errno, buffer, 256 );
	error_msg.assign(msg);
#endif
}



#endif /* __CHRIS_BRUCKS_CUSTOM_LOGGER__ */