#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char message[] = "/home/michael/Projects/ifmo/os/hw1/c/tester.c";
char* msg;
char buf[10024];

int main( int argc, char* argv[] ) {
    int sock;
    struct sockaddr_in addr;
    
    if( argc < 2 ) {
        msg = message;
    } else {
        msg = argv[1];
    }
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if( sock < 0 ) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(10800);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if( connect( sock, ( struct sockaddr * )&addr, sizeof( addr ) ) < 0 ) {
        perror( "connect" );
        exit(2);
    }

    int result = send( sock, msg, strlen( msg ) * sizeof( msg[0] ), 0 );
    if( result == -1 ) {
        perror("send");
        close( sock );
        puts("");
        exit(3);
    }
    int sz;

    while( sz = recv( sock, buf, 1024, 0 ) ) {
        if( sz == -1 ) {
            perror( "recv" );
            break;
        }
        if( !buf[0] ) {
            break;
        }
        write(1, buf, sz);
        memset( buf, 0, sz );
    }
    close( sock );
    puts("");
    return 0;
}

