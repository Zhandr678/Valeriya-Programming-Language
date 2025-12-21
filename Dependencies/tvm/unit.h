
// Written by Hans de Nivelle, October 2022.

#ifndef TVM_UNIT_
#define TVM_UNIT_  

#include <iostream>

namespace tvm
{
   struct unit
   {
      unit( ) noexcept = default;

      bool operator == ( unit ) const { return true; }
      bool operator != ( unit ) const { return false; }
      bool operator < ( unit ) const { return false; }
      bool operator > ( unit ) const { return false; }
      bool operator <= ( unit ) const { return true; }
      bool operator >= ( unit ) const { return true; }

      struct hash
      {
         bool operator( ) ( unit u ) const 
            { return 0; }
      };

      struct equal_to
      {
         bool operator( ) ( unit u1, unit u2 ) const 
            { return true; }
      };
   };

   inline std::ostream& operator << ( std::ostream& out, unit )
   {
      out << "unit";
      return out;
   }

}

#endif

