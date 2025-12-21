

#ifndef TVM_HEXPRINT_
#define TVM_HEXPRINT_   1

#include <iostream>

namespace tvm
{

   inline 
   void hexprint( std::ostream& out, long unsigned int val, 
                  short unsigned int nrbytes = sizeof( long unsigned int ) )
   {
      short unsigned int shift = 8 * nrbytes; 
      while( nrbytes -- )  
      {
         for( short unsigned int i = 0; i != 2; ++ i )
         {
            shift -= 4; 
            unsigned int b = ( val >> shift );
            b &= 0XF;
            if( b < 10 )
               out << (char)( '0' + b );
            else
               out << (char)( 'A' + ( b - 10 ));
         }
      } 
   }
}

#endif 


