
// This code was automatically built by TreeGen
// Written by Hans de Nivelle, see www.compiler-tools.eu

#include "Expression.h"

val::Expression::Expression( const Expression& from )
   : _ssss( from. _ssss )
{
   // std::cout << "Expression( const Expression& " << from. _ssss << " )";
   
   switch( from. _ssss )
   {
   case ArrayIndex:
      tvm::init( repr. _fld00. loc. first, from. repr. _fld00. loc. first );
      tvm::init( repr. _fld00. loc. second, from. repr. _fld00. loc. second );
      break;
   case BinaryExpr:
      tvm::init( repr. _fld01. loc, from. repr. _fld01. loc );
      repr. _fld01. heap = takeshare( from. repr. _fld01. heap );
      break;
   case BoolLiteralExpr:
      tvm::init( repr. _fld02. loc, from. repr. _fld02. loc );
      break;
   case CharLiteralExpr:
      tvm::init( repr. _fld03. loc, from. repr. _fld03. loc );
      break;
   case DoubleLiteralExpr:
      tvm::init( repr. _fld04. loc, from. repr. _fld04. loc );
      break;
   case EmptyExpr:
      break;
   case FieldCallExpr:
      repr. _fld06. heap = takeshare( from. repr. _fld06. heap );
      break;
   case FnCallExpr:
   case StructInitExpr:
      tvm::init( repr. _fld07. loc, from. repr. _fld07. loc );
      repr. _fld07. heap = takeshare( from. repr. _fld07. heap );
      break;
   case IntLiteralExpr:
      tvm::init( repr. _fld08. loc, from. repr. _fld08. loc );
      break;
   case StringLiteralExpr:
   case VarNameExpr:
      tvm::init( repr. _fld09. loc, from. repr. _fld09. loc );
      break;
   case UnaryExpr:
      tvm::init( repr. _fld10. loc, from. repr. _fld10. loc );
      repr. _fld10. heap = takeshare( from. repr. _fld10. heap );
      break;
   }
}

val::Expression::Expression( Expression&& from ) noexcept
   : _ssss( from. _ssss )
{
   // std::cout << "Expression( Expression&& " << from. _ssss << " )";
   
   switch( from. _ssss )
   {
   case ArrayIndex:
      tvm::init( repr. _fld00. loc. first, std::move( from. repr. _fld00. loc. first ) );
      tvm::destroy( from. repr. _fld00. loc. first );
      tvm::init( repr. _fld00. loc. second, std::move( from. repr. _fld00. loc. second ) );
      tvm::destroy( from. repr. _fld00. loc. second );
      break;
   case BinaryExpr:
      tvm::init( repr. _fld01. loc, std::move( from. repr. _fld01. loc ) );
      tvm::destroy( from. repr. _fld01. loc );
      repr. _fld01. heap = from. repr. _fld01. heap;
      break;
   case BoolLiteralExpr:
      tvm::init( repr. _fld02. loc, std::move( from. repr. _fld02. loc ) );
      tvm::destroy( from. repr. _fld02. loc );
      break;
   case CharLiteralExpr:
      tvm::init( repr. _fld03. loc, std::move( from. repr. _fld03. loc ) );
      tvm::destroy( from. repr. _fld03. loc );
      break;
   case DoubleLiteralExpr:
      tvm::init( repr. _fld04. loc, std::move( from. repr. _fld04. loc ) );
      tvm::destroy( from. repr. _fld04. loc );
      break;
   case EmptyExpr:
      break;
   case FieldCallExpr:
      repr. _fld06. heap = from. repr. _fld06. heap;
      break;
   case FnCallExpr:
   case StructInitExpr:
      tvm::init( repr. _fld07. loc, std::move( from. repr. _fld07. loc ) );
      tvm::destroy( from. repr. _fld07. loc );
      repr. _fld07. heap = from. repr. _fld07. heap;
      break;
   case IntLiteralExpr:
      tvm::init( repr. _fld08. loc, std::move( from. repr. _fld08. loc ) );
      tvm::destroy( from. repr. _fld08. loc );
      break;
   case StringLiteralExpr:
   case VarNameExpr:
      tvm::init( repr. _fld09. loc, std::move( from. repr. _fld09. loc ) );
      tvm::destroy( from. repr. _fld09. loc );
      break;
   case UnaryExpr:
      tvm::init( repr. _fld10. loc, std::move( from. repr. _fld10. loc ) );
      tvm::destroy( from. repr. _fld10. loc );
      repr. _fld10. heap = from. repr. _fld10. heap;
      break;
   }

   // Leave from in trivial state:
   
   from. _ssss = EmptyExpr;
   new (&from. repr) options( );
}

// Note that the implementation of assignment is minimalistic.
// One should create special cases for when *this and from are 
// in the same state.

const val::Expression & val::Expression::operator = ( const Expression& from )
{
   if( this == &from )
      return *this;
   
   switch( from. _ssss )
   {
   case BinaryExpr:
      takeshare( from. repr. _fld01. heap );
      break;
   case FieldCallExpr:
      takeshare( from. repr. _fld06. heap );
      break;
   case FnCallExpr:
   case StructInitExpr:
      takeshare( from. repr. _fld07. heap );
      break;
   case UnaryExpr:
      takeshare( from. repr. _fld10. heap );
      break;
   }

   this -> ~Expression( );
   
   _ssss = from. _ssss;
   
   switch( _ssss )
   {
   case ArrayIndex:
      tvm::init( repr. _fld00. loc. first, from. repr. _fld00. loc. first );
      tvm::init( repr. _fld00. loc. second, from. repr. _fld00. loc. second );
      break;
   case BinaryExpr:
      tvm::init( repr. _fld01. loc, from. repr. _fld01. loc );
      repr. _fld01. heap = from. repr. _fld01. heap;
      break;
   case BoolLiteralExpr:
      tvm::init( repr. _fld02. loc, from. repr. _fld02. loc );
      break;
   case CharLiteralExpr:
      tvm::init( repr. _fld03. loc, from. repr. _fld03. loc );
      break;
   case DoubleLiteralExpr:
      tvm::init( repr. _fld04. loc, from. repr. _fld04. loc );
      break;
   case EmptyExpr:
      break;
   case FieldCallExpr:
      repr. _fld06. heap = from. repr. _fld06. heap;
      break;
   case FnCallExpr:
   case StructInitExpr:
      tvm::init( repr. _fld07. loc, from. repr. _fld07. loc );
      repr. _fld07. heap = from. repr. _fld07. heap;
      break;
   case IntLiteralExpr:
      tvm::init( repr. _fld08. loc, from. repr. _fld08. loc );
      break;
   case StringLiteralExpr:
   case VarNameExpr:
      tvm::init( repr. _fld09. loc, from. repr. _fld09. loc );
      break;
   case UnaryExpr:
      tvm::init( repr. _fld10. loc, from. repr. _fld10. loc );
      repr. _fld10. heap = from. repr. _fld10. heap;
      break;
   }

   return *this;
}

// We don't check self assignment in the moving case, because it is UB:

const val::Expression & val::Expression::operator = ( Expression&& from ) noexcept
{
   if( _ssss == from. _ssss )
   {
      switch( _ssss )
      {
      case ArrayIndex:
         tvm::assign( repr. _fld00. loc. first, std::move( from. repr. _fld00. loc. first ) );
         tvm::destroy( from. repr. _fld00. loc. first );
         tvm::assign( repr. _fld00. loc. second, std::move( from. repr. _fld00. loc. second ) );
         tvm::destroy( from. repr. _fld00. loc. second );
         break;
      case BinaryExpr:
         tvm::assign( repr. _fld01. loc, std::move( from. repr. _fld01. loc ) );
         tvm::destroy( from. repr. _fld01. loc );
         dropshare( repr. _fld01. heap );
         repr. _fld01. heap = from. repr. _fld01. heap;
         break;
      case BoolLiteralExpr:
         tvm::assign( repr. _fld02. loc, std::move( from. repr. _fld02. loc ) );
         tvm::destroy( from. repr. _fld02. loc );
         break;
      case CharLiteralExpr:
         tvm::assign( repr. _fld03. loc, std::move( from. repr. _fld03. loc ) );
         tvm::destroy( from. repr. _fld03. loc );
         break;
      case DoubleLiteralExpr:
         tvm::assign( repr. _fld04. loc, std::move( from. repr. _fld04. loc ) );
         tvm::destroy( from. repr. _fld04. loc );
         break;
      case EmptyExpr:
         break;
      case FieldCallExpr:
         dropshare( repr. _fld06. heap );
         repr. _fld06. heap = from. repr. _fld06. heap;
         break;
      case FnCallExpr:
      case StructInitExpr:
         tvm::assign( repr. _fld07. loc, std::move( from. repr. _fld07. loc ) );
         tvm::destroy( from. repr. _fld07. loc );
         dropshare( repr. _fld07. heap );
         repr. _fld07. heap = from. repr. _fld07. heap;
         break;
      case IntLiteralExpr:
         tvm::assign( repr. _fld08. loc, std::move( from. repr. _fld08. loc ) );
         tvm::destroy( from. repr. _fld08. loc );
         break;
      case StringLiteralExpr:
      case VarNameExpr:
         tvm::assign( repr. _fld09. loc, std::move( from. repr. _fld09. loc ) );
         tvm::destroy( from. repr. _fld09. loc );
         break;
      case UnaryExpr:
         tvm::assign( repr. _fld10. loc, std::move( from. repr. _fld10. loc ) );
         tvm::destroy( from. repr. _fld10. loc );
         dropshare( repr. _fld10. heap );
         repr. _fld10. heap = from. repr. _fld10. heap;
         break;
      }

      // Leave from in trivial state:

      from. _ssss = EmptyExpr;
      new (&from. repr) options( );
      return *this;
   }
   else
   {
      // I believe that this wll be safe, because we have
      // the only reference to other: 

      this -> ~Expression( );

      new (this) Expression( std::move( from ));
      return *this;
   }
}

val::Expression::~Expression( ) noexcept
{
   // If there are prefix fields, they will be destroyed automatically

   switch( _ssss )
   {
   case ArrayIndex:
      tvm::destroy( repr. _fld00. loc. first );
      tvm::destroy( repr. _fld00. loc. second );
      break;
   case BinaryExpr:
      tvm::destroy( repr. _fld01. loc );
      dropshare( repr. _fld01. heap );
      break;
   case BoolLiteralExpr:
      tvm::destroy( repr. _fld02. loc );
      break;
   case CharLiteralExpr:
      tvm::destroy( repr. _fld03. loc );
      break;
   case DoubleLiteralExpr:
      tvm::destroy( repr. _fld04. loc );
      break;
   case EmptyExpr:
      break;
   case FieldCallExpr:
      dropshare( repr. _fld06. heap );
      break;
   case FnCallExpr:
   case StructInitExpr:
      tvm::destroy( repr. _fld07. loc );
      dropshare( repr. _fld07. heap );
      break;
   case IntLiteralExpr:
      tvm::destroy( repr. _fld08. loc );
      break;
   case StringLiteralExpr:
   case VarNameExpr:
      tvm::destroy( repr. _fld09. loc );
      break;
   case UnaryExpr:
      tvm::destroy( repr. _fld10. loc );
      dropshare( repr. _fld10. heap );
      break;
   }
}

bool val::Expression::very_equal_to( const Expression & other ) const
{
   if( _ssss != other. _ssss )
      return false;

   switch( _ssss )
   {
   case ArrayIndex:
      if( tvm::distinct( repr. _fld00. loc. first, other. repr. _fld00. loc. first ))
         return false;
      if( tvm::distinct( repr. _fld00. loc. second, other. repr. _fld00. loc. second ))
         return false;
      return true;
   case BinaryExpr:
      if( tvm::distinct( repr. _fld01. loc, other. repr. _fld01. loc ))
         return false;
      if( repr. _fld01. heap != other. repr. _fld01. heap )
         return false;
      return true;
   case BoolLiteralExpr:
      if( tvm::distinct( repr. _fld02. loc, other. repr. _fld02. loc ))
         return false;
      return true;
   case CharLiteralExpr:
      if( tvm::distinct( repr. _fld03. loc, other. repr. _fld03. loc ))
         return false;
      return true;
   case DoubleLiteralExpr:
      if( tvm::distinct( repr. _fld04. loc, other. repr. _fld04. loc ))
         return false;
      return true;
   case EmptyExpr:
      return true;
   case FieldCallExpr:
      if( repr. _fld06. heap != other. repr. _fld06. heap )
         return false;
      return true;
   case FnCallExpr:
   case StructInitExpr:
      if( tvm::distinct( repr. _fld07. loc, other. repr. _fld07. loc ))
         return false;
      if( repr. _fld07. heap != other. repr. _fld07. heap )
         return false;
      return true;
   case IntLiteralExpr:
      if( tvm::distinct( repr. _fld08. loc, other. repr. _fld08. loc ))
         return false;
      return true;
   case StringLiteralExpr:
   case VarNameExpr:
      if( tvm::distinct( repr. _fld09. loc, other. repr. _fld09. loc ))
         return false;
      return true;
   case UnaryExpr:
      if( tvm::distinct( repr. _fld10. loc, other. repr. _fld10. loc ))
         return false;
      if( repr. _fld10. heap != other. repr. _fld10. heap )
         return false;
      return true;
   }
}

void val::Expression::printstate( std::ostream& out ) const
{
   switch( _ssss )
   {
   case BinaryExpr:
      tvm::printstate( repr. _fld01. heap, out );
      break;
   case FieldCallExpr:
      tvm::printstate( repr. _fld06. heap, out );
      break;
   case FnCallExpr:
   case StructInitExpr:
      tvm::printstate( repr. _fld07. heap, out );
      break;
   case UnaryExpr:
      tvm::printstate( repr. _fld10. heap, out );
      break;
   }
}

