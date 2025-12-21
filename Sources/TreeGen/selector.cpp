
// This code was automatically built by TreeGen
// Written by Hans de Nivelle, see www.compiler-tools.eu

#include "TreeGen/selector.h"

const char* mylang::getcstring( selector sel )
{
   switch( sel )
   {
   case expr_div :
      return "expr_div";
   case expr_add :
      return "expr_add";
   case expr_sub :
      return "expr_sub";
   case expr_mul :
      return "expr_mul";
   case expr_empty :
      return "expr_empty";
   case expr_if :
      return "expr_if";
   case expr_lit :
      return "expr_lit";
   case expr_var :
      return "expr_var";
   }
   return "???";
}

