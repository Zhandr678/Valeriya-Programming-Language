
// This code was automatically built by TreeGen
// Written by Hans de Nivelle, see www.compiler-tools.eu

#include "TreeGen/expr.h"

mylang::expr::expr( const expr& from )
   : _ssss( from. _ssss )
{
   // std::cout << "expr( const expr& " << from. _ssss << " )";
   
   switch( from. _ssss )
   {
   case expr_add:
   case expr_div:
   case expr_sub:
   case expr_mul:
      repr. _fld00. heap = takeshare( from. repr. _fld00. heap );
      break;
   case expr_empty:
      break;
   case expr_if:
      repr. _fld02. heap = takeshare( from. repr. _fld02. heap );
      break;
   case expr_lit:
      repr. _fld03. heap = takeshare( from. repr. _fld03. heap );
      break;
   case expr_var:
      repr. _fld04. heap = takeshare( from. repr. _fld04. heap );
      break;
   }
}

mylang::expr::expr( expr&& from ) noexcept
   : _ssss( from. _ssss )
{
   // std::cout << "expr( expr&& " << from. _ssss << " )";
   
   switch( from. _ssss )
   {
   case expr_add:
   case expr_div:
   case expr_sub:
   case expr_mul:
      repr. _fld00. heap = from. repr. _fld00. heap;
      break;
   case expr_empty:
      break;
   case expr_if:
      repr. _fld02. heap = from. repr. _fld02. heap;
      break;
   case expr_lit:
      repr. _fld03. heap = from. repr. _fld03. heap;
      break;
   case expr_var:
      repr. _fld04. heap = from. repr. _fld04. heap;
      break;
   }

   // Leave from in trivial state:
   
   from. _ssss = expr_empty;
   new (&from. repr) options( );
}

// Note that the implementation of assignment is minimalistic.
// One should create special cases for when *this and from are 
// in the same state.

const mylang::expr & mylang::expr::operator = ( const expr& from )
{
   if( this == &from )
      return *this;
   
   switch( from. _ssss )
   {
   case expr_add:
   case expr_div:
   case expr_sub:
   case expr_mul:
      takeshare( from. repr. _fld00. heap );
      break;
   case expr_if:
      takeshare( from. repr. _fld02. heap );
      break;
   case expr_lit:
      takeshare( from. repr. _fld03. heap );
      break;
   case expr_var:
      takeshare( from. repr. _fld04. heap );
      break;
   }

   this -> ~expr( );
   
   _ssss = from. _ssss;
   
   switch( _ssss )
   {
   case expr_add:
   case expr_div:
   case expr_sub:
   case expr_mul:
      repr. _fld00. heap = from. repr. _fld00. heap;
      break;
   case expr_empty:
      break;
   case expr_if:
      repr. _fld02. heap = from. repr. _fld02. heap;
      break;
   case expr_lit:
      repr. _fld03. heap = from. repr. _fld03. heap;
      break;
   case expr_var:
      repr. _fld04. heap = from. repr. _fld04. heap;
      break;
   }

   return *this;
}

// We don't check self assignment in the moving case, because it is UB:

const mylang::expr & mylang::expr::operator = ( expr&& from ) noexcept
{
   if( _ssss == from. _ssss )
   {
      switch( _ssss )
      {
      case expr_add:
      case expr_div:
      case expr_sub:
      case expr_mul:
         dropshare( repr. _fld00. heap );
         repr. _fld00. heap = from. repr. _fld00. heap;
         break;
      case expr_empty:
         break;
      case expr_if:
         dropshare( repr. _fld02. heap );
         repr. _fld02. heap = from. repr. _fld02. heap;
         break;
      case expr_lit:
         dropshare( repr. _fld03. heap );
         repr. _fld03. heap = from. repr. _fld03. heap;
         break;
      case expr_var:
         dropshare( repr. _fld04. heap );
         repr. _fld04. heap = from. repr. _fld04. heap;
         break;
      }

      // Leave from in trivial state:

      from. _ssss = expr_empty;
      new (&from. repr) options( );
      return *this;
   }
   else
   {
      // I believe that this wll be safe, because we have
      // the only reference to other: 

      this -> ~expr( );

      new (this) expr( std::move( from ));
      return *this;
   }
}

mylang::expr::~expr( ) noexcept
{
   // If there are prefix fields, they will be destroyed automatically

   switch( _ssss )
   {
   case expr_add:
   case expr_div:
   case expr_sub:
   case expr_mul:
      dropshare( repr. _fld00. heap );
      break;
   case expr_empty:
      break;
   case expr_if:
      dropshare( repr. _fld02. heap );
      break;
   case expr_lit:
      dropshare( repr. _fld03. heap );
      break;
   case expr_var:
      dropshare( repr. _fld04. heap );
      break;
   }
}

bool mylang::expr::very_equal_to( const expr & other ) const
{
   if( _ssss != other. _ssss )
      return false;

   switch( _ssss )
   {
   case expr_add:
   case expr_div:
   case expr_sub:
   case expr_mul:
      if( repr. _fld00. heap != other. repr. _fld00. heap )
         return false;
      return true;
   case expr_empty:
      return true;
   case expr_if:
      if( repr. _fld02. heap != other. repr. _fld02. heap )
         return false;
      return true;
   case expr_lit:
      if( repr. _fld03. heap != other. repr. _fld03. heap )
         return false;
      return true;
   case expr_var:
      if( repr. _fld04. heap != other. repr. _fld04. heap )
         return false;
      return true;
   }
}

void mylang::expr::printstate( std::ostream& out ) const
{
   switch( _ssss )
   {
   case expr_add:
   case expr_div:
   case expr_sub:
   case expr_mul:
      tvm::printstate( repr. _fld00. heap, out );
      break;
   case expr_if:
      tvm::printstate( repr. _fld02. heap, out );
      break;
   case expr_lit:
      tvm::printstate( repr. _fld03. heap, out );
      break;
   case expr_var:
      tvm::printstate( repr. _fld04. heap, out );
      break;
   }
}

