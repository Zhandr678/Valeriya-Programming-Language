
// This code was automatically built by TreeGen
// Written by Hans de Nivelle, see www.compiler-tools.eu

#ifndef MYLANG_SELECTOR_
#define MYLANG_SELECTOR_

#include <iostream>


namespace mylang { 

   enum selector
   {      
      expr_div, expr_add, expr_sub, expr_mul, 
      expr_empty, expr_if, expr_lit, expr_var
   };

   const char* getcstring( selector );

   inline std::ostream& operator << ( std::ostream& out, selector sel )
      { out << getcstring( sel ); return out; }

}


#endif


