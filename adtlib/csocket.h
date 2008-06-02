/* csocket.h
**
**	Defines a socket stream class.
*/

#ifndef __CSOCKET_H
#define __CSOCKET_H

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>

#include "mdefs.h"

/* Set platform-independent types. Winsock should work with these */
#define SOCKET		int
#define SOCKADDR_IN	struct sockaddr_in
#define HOSTENT		struct hostent

class CSocket {
protected:
    SOCKET		m_SockId;
    SOCKADDR_IN		m_SockAddr;
    HOSTENT *		m_Host;

    int			m_SocketType;
    int			m_StreamType;
    int			m_Port;

public:
			CSocket (void);
    inline int		GetSocketType (void);
    inline int		GetStreamType (void);
    int			Read (char * buffer, int size);
    void		Read (int * buffer);
    void		Read (unsigned short int * buffer);
    void		Read (unsigned long int * buffer);
    int			Write (char * data, int size);
    void		Write (int data);
    void		Write (unsigned short int data);
    void		Write (unsigned long int data);
    int			ReadLine (char * buffer, int size, char delim = '\n');
    virtual void	Close (void);
    virtual	       ~CSocket (void);
};

class CSocketClient : public CSocket {
public:
			CSocketClient (void);
    int			Open (char * hostname, int port, 
    			      int socktype = AF_INET, 
			      int strtype = SOCK_STREAM);
    virtual	       ~CSocketClient (void);
};

class CSocketServer : public CSocket {
private:
    bool		m_IsForking;

public:
			CSocketServer (void);
    int			Open (int port, int socktype = AF_INET, 
			      int strtype = SOCK_STREAM);
    virtual void	Run (void);
    virtual void	UserServerProc (void) = 0;
    inline void		EnableForking (void);
    virtual	       ~CSocketServer (void);
};

/*--------------------------------------------------------------------------*/

inline void CSocketServer :: EnableForking (void)
{
    m_IsForking = true;
}

#endif

