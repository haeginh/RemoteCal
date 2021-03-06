// Implementation of the ServerSocket class

#include "ServerSocket.h"
#include "SocketException.h"


ServerSocket::ServerSocket ( int port )
{
  if ( ! Socket::create() )
    {
      throw SocketException ( "Could not create server socket." );
    }

  if ( ! Socket::bind ( port ) )
    {
      throw SocketException ( "Could not bind to port." );
    }

  if ( ! Socket::listen() )
    {
      throw SocketException ( "Could not listen to socket." );
    }

  //Socket::set_non_blocking(false);
}

ServerSocket::~ServerSocket()
{
}


const ServerSocket& ServerSocket::operator << ( const std::string& s ) const
{
  if ( ! Socket::send ( s ) )
    {
      throw SocketException ( "Could not write to socket." );
    }
  else usleep(1);
  return *this;

}


const ServerSocket& ServerSocket::operator >> ( std::string& s ) const
{
  if ( ! Socket::recv ( s ) )
    {
      throw SocketException ( "Could not read from socket." );
    }

  return *this;
}

const ServerSocket& ServerSocket::RecvDoubleBuffer ( double* arr, int num ) const
{
  if ( ! Socket::recv ( arr, num ) )
    {
      throw SocketException ( "Could not read from socket." );
    }

  return *this;
}

const ServerSocket& ServerSocket::SendDoubleBuffer ( const double* buf, int num, int wait) const{
    if ( ! Socket::send ( buf, num ) )
      {
        throw SocketException ( "Could not write to socket." );
      }
    else usleep(wait);
    return *this;
}

const ServerSocket& ServerSocket::RecvIntBuffer ( int* arr, int num ) const
{
  if ( ! Socket::recv ( arr, num ) )
    {
      throw SocketException ( "Could not read from socket." );
    }

  return *this;
}

const ServerSocket& ServerSocket::SendIntBuffer ( const int* buf, int num, int wait) const{
    if ( ! Socket::send ( buf, num ) )
      {
        throw SocketException ( "Could not write to socket." );
      }
    else usleep(wait);
    return *this;
}

void ServerSocket::accept ( ServerSocket& sock )
{
  if ( ! Socket::accept ( sock ) )
    {
      throw SocketException ( "Could not accept socket." );
    }
}

void ServerSocket::close ( ServerSocket& sock )
{
  Socket::close ( sock );
}
