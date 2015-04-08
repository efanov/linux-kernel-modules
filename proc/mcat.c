#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "common.h"

static int get_proc( void ) {
   char dev[ 80 ];
   int df;
//   sprintf( dev, "/proc/%s", NAME_NODE );
   sprintf( dev, "/proc/%s", "mephi" );
   if( ( df = open( dev, O_RDONLY ) ) < 0 ) {
      sprintf( dev, "/proc/%s/%s", NAME_DIR, NAME_NODE );
      if( ( df = open( dev, O_RDONLY ) ) < 0 )
         printf( "open device error: %m\n" ), exit( EXIT_FAILURE );
   }
   return df;
}

int main( int argc, char *argv[] ) {
   int df = get_proc(),
       len = ( argc > 1 && atoi( argv[ 1 ] ) > 0 ) ?
             atoi( argv[ 1 ] ) : LEN_MSG; 
   char msg[ LEN_MSG + 1 ] = "";
   char *p = msg;
   int res;
   do {
      if( ( res = read( df, p, len ) ) >= 0 ) {
         *( p += res )  = '\0';
         printf( "read + %02d bytes, input buffer: %s", res, msg );
         if( *( p - 1 ) != '\n' ) printf( "\n" );
      }
      else printf( "read device error: %m\n" );
   } while ( res > 0 );
   close( df );
   return EXIT_SUCCESS;
};
