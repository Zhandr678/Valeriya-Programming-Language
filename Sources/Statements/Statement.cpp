
// This code was automatically built by TreeGen
// Written by Hans de Nivelle, see www.compiler-tools.eu

#include "Statement.h"

val::Statement::Statement( const Statement& from )
   : _ssss( from. _ssss )
{
   // std::cout << "Statement( const Statement& " << from. _ssss << " )";
   
   switch( from. _ssss )
   {
   case AssignmentStmt:
      tvm::init( repr. _fld00. loc, from. repr. _fld00. loc );
      repr. _fld00. heap = takeshare( from. repr. _fld00. heap );
      break;
   case BlockOfStmt:
      repr. _fld01. heap = takeshare( from. repr. _fld01. heap );
      break;
   case BreakStmt:
   case ContinueStmt:
   case EmptyStmt:
      break;
   case CaseClauseStmt:
   case MatchStmt:
   case MakeStructStmt:
   case MakePropertyStmt:
      tvm::init( repr. _fld03. loc, from. repr. _fld03. loc );
      repr. _fld03. heap = takeshare( from. repr. _fld03. heap );
      break;
   case ConditionStmt:
      repr. _fld04. heap = takeshare( from. repr. _fld04. heap );
      break;
   case WhileLoopStmt:
   case ElifConditionStmt:
      repr. _fld05. heap = takeshare( from. repr. _fld05. heap );
      break;
   case ExprCallStmt:
   case ReturnStmt:
      repr. _fld06. heap = takeshare( from. repr. _fld06. heap );
      break;
   case FnArgsStmt:
      tvm::init( get<0> ( repr. _fld07. loc ), get<0> ( from. repr. _fld07. loc ) );
      tvm::init( get<1> ( repr. _fld07. loc ), get<1> ( from. repr. _fld07. loc ) );
      tvm::init( get<2> ( repr. _fld07. loc ), get<2> ( from. repr. _fld07. loc ) );
      repr. _fld07. heap = takeshare( from. repr. _fld07. heap );
      break;
   case ForLoopStmt:
      repr. _fld08. heap = takeshare( from. repr. _fld08. heap );
      break;
   case MakeEnumStmt:
      tvm::init( repr. _fld09. loc, from. repr. _fld09. loc );
      repr. _fld09. heap = takeshare( from. repr. _fld09. heap );
      break;
   case MakeFunctionStmt:
      tvm::init( repr. _fld10. loc. first, from. repr. _fld10. loc. first );
      tvm::init( repr. _fld10. loc. second, from. repr. _fld10. loc. second );
      repr. _fld10. heap = takeshare( from. repr. _fld10. heap );
      break;
   case VarInitStmt:
      tvm::init( repr. _fld11. loc. first, from. repr. _fld11. loc. first );
      tvm::init( repr. _fld11. loc. second, from. repr. _fld11. loc. second );
      repr. _fld11. heap = takeshare( from. repr. _fld11. heap );
      break;
   }
}

val::Statement::Statement( Statement&& from ) noexcept
   : _ssss( from. _ssss )
{
   // std::cout << "Statement( Statement&& " << from. _ssss << " )";
   
   switch( from. _ssss )
   {
   case AssignmentStmt:
      tvm::init( repr. _fld00. loc, std::move( from. repr. _fld00. loc ) );
      tvm::destroy( from. repr. _fld00. loc );
      repr. _fld00. heap = from. repr. _fld00. heap;
      break;
   case BlockOfStmt:
      repr. _fld01. heap = from. repr. _fld01. heap;
      break;
   case BreakStmt:
   case ContinueStmt:
   case EmptyStmt:
      break;
   case CaseClauseStmt:
   case MatchStmt:
   case MakeStructStmt:
   case MakePropertyStmt:
      tvm::init( repr. _fld03. loc, std::move( from. repr. _fld03. loc ) );
      tvm::destroy( from. repr. _fld03. loc );
      repr. _fld03. heap = from. repr. _fld03. heap;
      break;
   case ConditionStmt:
      repr. _fld04. heap = from. repr. _fld04. heap;
      break;
   case WhileLoopStmt:
   case ElifConditionStmt:
      repr. _fld05. heap = from. repr. _fld05. heap;
      break;
   case ExprCallStmt:
   case ReturnStmt:
      repr. _fld06. heap = from. repr. _fld06. heap;
      break;
   case FnArgsStmt:
      tvm::init( get<0> ( repr. _fld07. loc ), std::move( get<0> ( from. repr. _fld07. loc ) ) );
      tvm::destroy( get<0> ( from. repr. _fld07. loc ) );
      tvm::init( get<1> ( repr. _fld07. loc ), std::move( get<1> ( from. repr. _fld07. loc ) ) );
      tvm::destroy( get<1> ( from. repr. _fld07. loc ) );
      tvm::init( get<2> ( repr. _fld07. loc ), std::move( get<2> ( from. repr. _fld07. loc ) ) );
      tvm::destroy( get<2> ( from. repr. _fld07. loc ) );
      repr. _fld07. heap = from. repr. _fld07. heap;
      break;
   case ForLoopStmt:
      repr. _fld08. heap = from. repr. _fld08. heap;
      break;
   case MakeEnumStmt:
      tvm::init( repr. _fld09. loc, std::move( from. repr. _fld09. loc ) );
      tvm::destroy( from. repr. _fld09. loc );
      repr. _fld09. heap = from. repr. _fld09. heap;
      break;
   case MakeFunctionStmt:
      tvm::init( repr. _fld10. loc. first, std::move( from. repr. _fld10. loc. first ) );
      tvm::destroy( from. repr. _fld10. loc. first );
      tvm::init( repr. _fld10. loc. second, std::move( from. repr. _fld10. loc. second ) );
      tvm::destroy( from. repr. _fld10. loc. second );
      repr. _fld10. heap = from. repr. _fld10. heap;
      break;
   case VarInitStmt:
      tvm::init( repr. _fld11. loc. first, std::move( from. repr. _fld11. loc. first ) );
      tvm::destroy( from. repr. _fld11. loc. first );
      tvm::init( repr. _fld11. loc. second, std::move( from. repr. _fld11. loc. second ) );
      tvm::destroy( from. repr. _fld11. loc. second );
      repr. _fld11. heap = from. repr. _fld11. heap;
      break;
   }

   // Leave from in trivial state:
   
   from. _ssss = EmptyStmt;
   new (&from. repr) options( );
}

// Note that the implementation of assignment is minimalistic.
// One should create special cases for when *this and from are 
// in the same state.

const val::Statement & val::Statement::operator = ( const Statement& from )
{
   if( this == &from )
      return *this;
   
   switch( from. _ssss )
   {
   case AssignmentStmt:
      takeshare( from. repr. _fld00. heap );
      break;
   case BlockOfStmt:
      takeshare( from. repr. _fld01. heap );
      break;
   case CaseClauseStmt:
   case MatchStmt:
   case MakeStructStmt:
   case MakePropertyStmt:
      takeshare( from. repr. _fld03. heap );
      break;
   case ConditionStmt:
      takeshare( from. repr. _fld04. heap );
      break;
   case WhileLoopStmt:
   case ElifConditionStmt:
      takeshare( from. repr. _fld05. heap );
      break;
   case ExprCallStmt:
   case ReturnStmt:
      takeshare( from. repr. _fld06. heap );
      break;
   case FnArgsStmt:
      takeshare( from. repr. _fld07. heap );
      break;
   case ForLoopStmt:
      takeshare( from. repr. _fld08. heap );
      break;
   case MakeEnumStmt:
      takeshare( from. repr. _fld09. heap );
      break;
   case MakeFunctionStmt:
      takeshare( from. repr. _fld10. heap );
      break;
   case VarInitStmt:
      takeshare( from. repr. _fld11. heap );
      break;
   }

   this -> ~Statement( );
   
   _ssss = from. _ssss;
   
   switch( _ssss )
   {
   case AssignmentStmt:
      tvm::init( repr. _fld00. loc, from. repr. _fld00. loc );
      repr. _fld00. heap = from. repr. _fld00. heap;
      break;
   case BlockOfStmt:
      repr. _fld01. heap = from. repr. _fld01. heap;
      break;
   case BreakStmt:
   case ContinueStmt:
   case EmptyStmt:
      break;
   case CaseClauseStmt:
   case MatchStmt:
   case MakeStructStmt:
   case MakePropertyStmt:
      tvm::init( repr. _fld03. loc, from. repr. _fld03. loc );
      repr. _fld03. heap = from. repr. _fld03. heap;
      break;
   case ConditionStmt:
      repr. _fld04. heap = from. repr. _fld04. heap;
      break;
   case WhileLoopStmt:
   case ElifConditionStmt:
      repr. _fld05. heap = from. repr. _fld05. heap;
      break;
   case ExprCallStmt:
   case ReturnStmt:
      repr. _fld06. heap = from. repr. _fld06. heap;
      break;
   case FnArgsStmt:
      tvm::init( get<0> ( repr. _fld07. loc ), get<0> ( from. repr. _fld07. loc ) );
      tvm::init( get<1> ( repr. _fld07. loc ), get<1> ( from. repr. _fld07. loc ) );
      tvm::init( get<2> ( repr. _fld07. loc ), get<2> ( from. repr. _fld07. loc ) );
      repr. _fld07. heap = from. repr. _fld07. heap;
      break;
   case ForLoopStmt:
      repr. _fld08. heap = from. repr. _fld08. heap;
      break;
   case MakeEnumStmt:
      tvm::init( repr. _fld09. loc, from. repr. _fld09. loc );
      repr. _fld09. heap = from. repr. _fld09. heap;
      break;
   case MakeFunctionStmt:
      tvm::init( repr. _fld10. loc. first, from. repr. _fld10. loc. first );
      tvm::init( repr. _fld10. loc. second, from. repr. _fld10. loc. second );
      repr. _fld10. heap = from. repr. _fld10. heap;
      break;
   case VarInitStmt:
      tvm::init( repr. _fld11. loc. first, from. repr. _fld11. loc. first );
      tvm::init( repr. _fld11. loc. second, from. repr. _fld11. loc. second );
      repr. _fld11. heap = from. repr. _fld11. heap;
      break;
   }

   return *this;
}

// We don't check self assignment in the moving case, because it is UB:

const val::Statement & val::Statement::operator = ( Statement&& from ) noexcept
{
   if( _ssss == from. _ssss )
   {
      switch( _ssss )
      {
      case AssignmentStmt:
         tvm::assign( repr. _fld00. loc, std::move( from. repr. _fld00. loc ) );
         tvm::destroy( from. repr. _fld00. loc );
         dropshare( repr. _fld00. heap );
         repr. _fld00. heap = from. repr. _fld00. heap;
         break;
      case BlockOfStmt:
         dropshare( repr. _fld01. heap );
         repr. _fld01. heap = from. repr. _fld01. heap;
         break;
      case BreakStmt:
      case ContinueStmt:
      case EmptyStmt:
         break;
      case CaseClauseStmt:
      case MatchStmt:
      case MakeStructStmt:
      case MakePropertyStmt:
         tvm::assign( repr. _fld03. loc, std::move( from. repr. _fld03. loc ) );
         tvm::destroy( from. repr. _fld03. loc );
         dropshare( repr. _fld03. heap );
         repr. _fld03. heap = from. repr. _fld03. heap;
         break;
      case ConditionStmt:
         dropshare( repr. _fld04. heap );
         repr. _fld04. heap = from. repr. _fld04. heap;
         break;
      case WhileLoopStmt:
      case ElifConditionStmt:
         dropshare( repr. _fld05. heap );
         repr. _fld05. heap = from. repr. _fld05. heap;
         break;
      case ExprCallStmt:
      case ReturnStmt:
         dropshare( repr. _fld06. heap );
         repr. _fld06. heap = from. repr. _fld06. heap;
         break;
      case FnArgsStmt:
         tvm::assign( get<0> ( repr. _fld07. loc ), std::move( get<0> ( from. repr. _fld07. loc ) ) );
         tvm::destroy( get<0> ( from. repr. _fld07. loc ) );
         tvm::assign( get<1> ( repr. _fld07. loc ), std::move( get<1> ( from. repr. _fld07. loc ) ) );
         tvm::destroy( get<1> ( from. repr. _fld07. loc ) );
         tvm::assign( get<2> ( repr. _fld07. loc ), std::move( get<2> ( from. repr. _fld07. loc ) ) );
         tvm::destroy( get<2> ( from. repr. _fld07. loc ) );
         dropshare( repr. _fld07. heap );
         repr. _fld07. heap = from. repr. _fld07. heap;
         break;
      case ForLoopStmt:
         dropshare( repr. _fld08. heap );
         repr. _fld08. heap = from. repr. _fld08. heap;
         break;
      case MakeEnumStmt:
         tvm::assign( repr. _fld09. loc, std::move( from. repr. _fld09. loc ) );
         tvm::destroy( from. repr. _fld09. loc );
         dropshare( repr. _fld09. heap );
         repr. _fld09. heap = from. repr. _fld09. heap;
         break;
      case MakeFunctionStmt:
         tvm::assign( repr. _fld10. loc. first, std::move( from. repr. _fld10. loc. first ) );
         tvm::destroy( from. repr. _fld10. loc. first );
         tvm::assign( repr. _fld10. loc. second, std::move( from. repr. _fld10. loc. second ) );
         tvm::destroy( from. repr. _fld10. loc. second );
         dropshare( repr. _fld10. heap );
         repr. _fld10. heap = from. repr. _fld10. heap;
         break;
      case VarInitStmt:
         tvm::assign( repr. _fld11. loc. first, std::move( from. repr. _fld11. loc. first ) );
         tvm::destroy( from. repr. _fld11. loc. first );
         tvm::assign( repr. _fld11. loc. second, std::move( from. repr. _fld11. loc. second ) );
         tvm::destroy( from. repr. _fld11. loc. second );
         dropshare( repr. _fld11. heap );
         repr. _fld11. heap = from. repr. _fld11. heap;
         break;
      }

      // Leave from in trivial state:

      from. _ssss = EmptyStmt;
      new (&from. repr) options( );
      return *this;
   }
   else
   {
      // I believe that this wll be safe, because we have
      // the only reference to other: 

      this -> ~Statement( );

      new (this) Statement( std::move( from ));
      return *this;
   }
}

val::Statement::~Statement( ) noexcept
{
   // If there are prefix fields, they will be destroyed automatically

   switch( _ssss )
   {
   case AssignmentStmt:
      tvm::destroy( repr. _fld00. loc );
      dropshare( repr. _fld00. heap );
      break;
   case BlockOfStmt:
      dropshare( repr. _fld01. heap );
      break;
   case BreakStmt:
   case ContinueStmt:
   case EmptyStmt:
      break;
   case CaseClauseStmt:
   case MatchStmt:
   case MakeStructStmt:
   case MakePropertyStmt:
      tvm::destroy( repr. _fld03. loc );
      dropshare( repr. _fld03. heap );
      break;
   case ConditionStmt:
      dropshare( repr. _fld04. heap );
      break;
   case WhileLoopStmt:
   case ElifConditionStmt:
      dropshare( repr. _fld05. heap );
      break;
   case ExprCallStmt:
   case ReturnStmt:
      dropshare( repr. _fld06. heap );
      break;
   case FnArgsStmt:
      tvm::destroy( get<0> ( repr. _fld07. loc ) );
      tvm::destroy( get<1> ( repr. _fld07. loc ) );
      tvm::destroy( get<2> ( repr. _fld07. loc ) );
      dropshare( repr. _fld07. heap );
      break;
   case ForLoopStmt:
      dropshare( repr. _fld08. heap );
      break;
   case MakeEnumStmt:
      tvm::destroy( repr. _fld09. loc );
      dropshare( repr. _fld09. heap );
      break;
   case MakeFunctionStmt:
      tvm::destroy( repr. _fld10. loc. first );
      tvm::destroy( repr. _fld10. loc. second );
      dropshare( repr. _fld10. heap );
      break;
   case VarInitStmt:
      tvm::destroy( repr. _fld11. loc. first );
      tvm::destroy( repr. _fld11. loc. second );
      dropshare( repr. _fld11. heap );
      break;
   }
}

bool val::Statement::very_equal_to( const Statement & other ) const
{
   if( _ssss != other. _ssss )
      return false;

   switch( _ssss )
   {
   case AssignmentStmt:
      if( tvm::distinct( repr. _fld00. loc, other. repr. _fld00. loc ))
         return false;
      if( repr. _fld00. heap != other. repr. _fld00. heap )
         return false;
      return true;
   case BlockOfStmt:
      if( repr. _fld01. heap != other. repr. _fld01. heap )
         return false;
      return true;
   case BreakStmt:
   case ContinueStmt:
   case EmptyStmt:
      return true;
   case CaseClauseStmt:
   case MatchStmt:
   case MakeStructStmt:
   case MakePropertyStmt:
      if( tvm::distinct( repr. _fld03. loc, other. repr. _fld03. loc ))
         return false;
      if( repr. _fld03. heap != other. repr. _fld03. heap )
         return false;
      return true;
   case ConditionStmt:
      if( repr. _fld04. heap != other. repr. _fld04. heap )
         return false;
      return true;
   case WhileLoopStmt:
   case ElifConditionStmt:
      if( repr. _fld05. heap != other. repr. _fld05. heap )
         return false;
      return true;
   case ExprCallStmt:
   case ReturnStmt:
      if( repr. _fld06. heap != other. repr. _fld06. heap )
         return false;
      return true;
   case FnArgsStmt:
      if( tvm::distinct( get<0> ( repr. _fld07. loc ), get<0> ( other. repr. _fld07. loc ) ))
         return false;
      if( tvm::distinct( get<1> ( repr. _fld07. loc ), get<1> ( other. repr. _fld07. loc ) ))
         return false;
      if( tvm::distinct( get<2> ( repr. _fld07. loc ), get<2> ( other. repr. _fld07. loc ) ))
         return false;
      if( repr. _fld07. heap != other. repr. _fld07. heap )
         return false;
      return true;
   case ForLoopStmt:
      if( repr. _fld08. heap != other. repr. _fld08. heap )
         return false;
      return true;
   case MakeEnumStmt:
      if( tvm::distinct( repr. _fld09. loc, other. repr. _fld09. loc ))
         return false;
      if( repr. _fld09. heap != other. repr. _fld09. heap )
         return false;
      return true;
   case MakeFunctionStmt:
      if( tvm::distinct( repr. _fld10. loc. first, other. repr. _fld10. loc. first ))
         return false;
      if( tvm::distinct( repr. _fld10. loc. second, other. repr. _fld10. loc. second ))
         return false;
      if( repr. _fld10. heap != other. repr. _fld10. heap )
         return false;
      return true;
   case VarInitStmt:
      if( tvm::distinct( repr. _fld11. loc. first, other. repr. _fld11. loc. first ))
         return false;
      if( tvm::distinct( repr. _fld11. loc. second, other. repr. _fld11. loc. second ))
         return false;
      if( repr. _fld11. heap != other. repr. _fld11. heap )
         return false;
      return true;
   }
}

void val::Statement::printstate( std::ostream& out ) const
{
   switch( _ssss )
   {
   case AssignmentStmt:
      tvm::printstate( repr. _fld00. heap, out );
      break;
   case BlockOfStmt:
      tvm::printstate( repr. _fld01. heap, out );
      break;
   case CaseClauseStmt:
   case MatchStmt:
   case MakeStructStmt:
   case MakePropertyStmt:
      tvm::printstate( repr. _fld03. heap, out );
      break;
   case ConditionStmt:
      tvm::printstate( repr. _fld04. heap, out );
      break;
   case WhileLoopStmt:
   case ElifConditionStmt:
      tvm::printstate( repr. _fld05. heap, out );
      break;
   case ExprCallStmt:
   case ReturnStmt:
      tvm::printstate( repr. _fld06. heap, out );
      break;
   case FnArgsStmt:
      tvm::printstate( repr. _fld07. heap, out );
      break;
   case ForLoopStmt:
      tvm::printstate( repr. _fld08. heap, out );
      break;
   case MakeEnumStmt:
      tvm::printstate( repr. _fld09. heap, out );
      break;
   case MakeFunctionStmt:
      tvm::printstate( repr. _fld10. heap, out );
      break;
   case VarInitStmt:
      tvm::printstate( repr. _fld11. heap, out );
      break;
   }
}

