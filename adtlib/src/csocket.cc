/* csocket.cc
**
**	Implementation of the socket class.
*/

#include <sys/param.h>
#include <string.h>
#include <csocket.h>

CSocket :: CSocket (void)
{
    m_SockId = 0;
    memset (&m_SockAddr, 0, sizeof(SOCKADDR_IN));

    // This depends on whether it is a client or server
    m_Host = NULL;

    m_SocketType = AF_INET;
    m_StreamType = SOCK_STREAM;
    m_Port = 0;
}

int CSocket :: Read (char * buffer, int size)
{
int bcount = 0, br = 0;

    memset (buffer, 0, size);
    while (bcount < size) {
    	br = read (m_SockId, (void*) &buffer[bcount], size - bcount);
	if (br < 0) {
	    perror ("CSocket");
	    return (bcount);
	}
	else if (br == 0)
	    break;
	else
	    bcount += br;
    }
    return (bcount);
}

int CSocket :: ReadLine (char * buffer, int size, char delim)
{
int bcount = 0, br = 0;
char c = '\x0';

    memset (buffer, 0, size);
    while (bcount < size && c != delim) {
    	br = read (m_SockId, (void*) &c, sizeof(char));
	if (br != sizeof(char)) {
	    perror ("CSocket");
	    return (bcount);
	}
	buffer[bcount] = c;
	bcount += br;
    }
    return (bcount);
}

void CSocket :: Read (int * buffer)
{
unsigned long int tempbuf;
    read (m_SockId, (void*) &tempbuf, sizeof(unsigned long int));
    *buffer = (int) ntohl (tempbuf);
}

void CSocket :: Read (unsigned short int * buffer)
{
    read (m_SockId, (void*) buffer, sizeof(unsigned short int));
    *buffer = ntohs (*buffer);
}

void CSocket :: Read (unsigned long int * buffer)
{
    read (m_SockId, (void*) buffer, sizeof(unsigned long int));
    *buffer = ntohl (*buffer);
}

int CSocket :: Write (char * data, int size)
{
int bcount = 0, br = 0;

    while (bcount < size) {
    	br = write (m_SockId, &data[bcount], size - bcount);
	if (br < 0) {
	    perror ("CSocket");
	    return (bcount);
	}
	else if (br == 0)
	    break;
	else
	    bcount += br;
    }
    return (bcount);
}

void CSocket :: Write (int data)
{
unsigned long int tempbuf;
    tempbuf = htonl ((unsigned long int) data);
    read (m_SockId, (void*) &tempbuf, sizeof(unsigned long int));
}

void CSocket :: Write (unsigned short int data)
{
    data = htons (data);
    write (m_SockId, &data, sizeof(unsigned short int));
}

void CSocket :: Write (unsigned long int data)
{
    data = htonl (data);
    write (m_SockId, &data, sizeof(unsigned long int));
}

void CSocket :: Close (void)
{
    close (m_SockId);
}

CSocket :: ~CSocket (void)
{
}

CSocketClient :: CSocketClient (void)
{
}

int CSocketClient :: Open (char * hostname, int port, 
			    int socktype, int strtype)
{
    m_Port = port;
    m_SocketType = socktype;
    m_StreamType = strtype;
    
    if ((m_Host = gethostbyname (hostname)) == NULL) {
	errno = ECONNREFUSED;
	return (-1);
    }

    memset (&m_SockAddr, 0, sizeof(SOCKADDR_IN));
    memcpy (&m_SockAddr.sin_addr, m_Host->h_addr, m_Host->h_length);
    m_SockAddr.sin_family = m_Host->h_addrtype;
    m_SockAddr.sin_port = htons (m_Port);

    if ((m_SockId = socket (m_Host->h_addrtype, m_StreamType, 0)) < 0)
	return (-2);
    if (connect (m_SockId, (struct sockaddr*) &m_SockAddr, 
		 sizeof (m_SockAddr)) < 0) 
    {
	close (m_SockId);
	return (-3);
    }
    return (0);
}

CSocketClient :: ~CSocketClient (void)
{
}

CSocketServer :: CSocketServer (void)
{
    m_IsForking = FALSE;
}

int CSocketServer :: Open (int port, int socktype, int strtype)
{
char myname [MAXHOSTNAMELEN+1];

    m_Port = port;
    m_SocketType = socktype;
    m_StreamType = strtype;

    memset (&m_SockAddr, 0, sizeof(SOCKADDR_IN));
    gethostname (myname, MAXHOSTNAMELEN);

    if ((m_Host = gethostbyname (myname)) == NULL)
	return (-1);

    m_SockAddr.sin_family = m_Host->h_addrtype;
    m_SockAddr.sin_port = htons (m_Port);

    if ((m_SockId = socket (m_SocketType, m_StreamType, 0)) < 0)
       	return (-2);

    if (bind (m_SockId, (struct sockaddr*) &m_SockAddr, 
	      sizeof(SOCKADDR_IN)) < 0) 
    {
       	close (m_SockId);
       	return (-3);
    }

    return (0);
}

static void Fireman (int child)
{
    child = -1;
    while (waitpid (-1, NULL, WNOHANG));
}

void CSocketServer :: Run (void)
{
SOCKET t, os;

    if (m_IsForking) {
	/* Tell fireman to watch for falling children */
	signal (SIGCHLD, Fireman);	
    }

    /* Say we are ready */
    listen (m_SockId, 5);

    /* Loop forever waiting for connections */
    while (TRUE) { 
	if ((t = accept (m_SockId, NULL, NULL)) < 0) {
	    if (errno == EINTR)
		continue; 
	    perror ("CSocketServer accept");
	    return;
	} 

	if (m_IsForking) { 
	    switch (fork()) {
		case -1:
		    perror ("fork");
		    close (m_SockId);
		    close (t);
		    return;
		case 0:
		    close (m_SockId);
		    // No need to restore it, since this process will exit
		    m_SockId = t;
		    UserServerProc();
		    close (t);
		    exit (0);
		default:
		    close (t);
		    continue;
	    }
	}
	else {
	    // Here, on the other hand, we do have to restore it
	    os = m_SockId;
	    m_SockId = t;
	    UserServerProc();
	    m_SockId = os;
	    close (t);
	}
    }
}

CSocketServer :: ~CSocketServer (void)
{
}

