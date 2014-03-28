#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#define DEBUG


int main( int argc, char* argv[] )
{
    
    int sock, listener;
    struct sockaddr_in addr;
    char buf[10024];
    int bytes_read;
    int port = 10800;
    
    if( argc >= 2 ) {
        sscanf( argv[1], "%d", &port  );
    }

    listener = socket( AF_INET, SOCK_STREAM, 0 );
    if( listener < 0 ) {
        perror( "socket" );
        exit( 1 );
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons( port );
    addr.sin_addr.s_addr = INADDR_ANY;
    if( bind( listener, ( struct sockaddr * )&addr, sizeof( addr ) ) < 0 ) {
        perror( "bind" );
        exit( 2 );
    }

    listen( listener, 1 );
    
    printf("Locked and loaded!\n");
    printf("Listening %d port\n\n", port);

    while(1) {
        sock = accept( listener, NULL, NULL );
        if( sock < 0 ) {
            perror( "accept" );
            exit( 3 );
        }

        pid_t fork_ans = fork();

        switch( fork_ans ) {
        case -1:
            perror( "fork" );
            break;

        case 0:
            close( listener );
            while( 1 ) {
                bytes_read = recv( sock, buf, 10024, 0 );
                if( bytes_read <= 0 ) {
                    break;
                }
                
                write( 1, "New query\n", 10 );
                
                int fd = open( buf, O_RDONLY );
                if( fd == -1 ) {
                    perror( "open" );
                    send( sock, "404 not found", 13, 0 );
                    puts( buf );
                    break;
                }
                int cur = 0;
                while( cur = read( fd, buf, 1024 ) ) {
                    int errsv = errno;
                    if( errno == EISDIR ) {
                        send( sock, "Is a directory\n", 15, 0 );
                    }
                    if( cur == -1 ) {
                        perror( "read" );
                        break;
                    }
                    send( sock, buf, cur, MSG_MORE );
                }
                memset( buf, 0, 10024 );
                send( sock, buf, 1024, 0 );
                write( 1, "Server done\n", 12 );
                close( fd );
            }

            close( sock );
            exit( 0 );

        default:
            close( sock );
        }
    }

    close( listener );

    return 0;
}

