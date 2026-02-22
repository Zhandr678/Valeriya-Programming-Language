
// This code was automatically built by TreeGen
// Written by Hans de Nivelle, see www.compiler-tools.eu

#include "Statement.h"

val::Statement::Statement( const Statement& from )
   : _ssss( from. _ssss )
{
   // std::cout << "Statement( const Statement& " << from. _ssss << " )";
   
   switch( from. _ssss )
   {
   case ArrayInitStmt:
      tvm::init( repr. _fld00. loc. first, from. repr. _fld00. loc. first );
      tvm::init( repr. _fld00. loc. second, from. repr. _fld00. loc. second );
      repr. _fld00. heap = takeshare( from. repr. _fld00. heap );
      break;
   case AssignmentStmt:
      tvm::init( repr. _fld01. loc, from. repr. _fld01. loc );
      repr. _fld01. heap = takeshare( from. repr. _fld01. heap );
      break;
   case BlockOfStmt:
      tvm::init( repr. _fld02. loc, from. repr. _fld02. loc );
      repr. _fld02. heap = takeshare( from. repr. _fld02. heap );
      break;
   case BreakStmt:
   case ContinueStmt:
   case EmptyStmt:
      tvm::init( repr. _fld03. loc, from. repr. _fld03. loc );
      break;
   case CaseClauseStmt:
      tvm::init( repr. _fld04. loc. first, from. repr. _fld04. loc. first );
      tvm::init( repr. _fld04. loc. second, from. repr. _fld04. loc. second );
      repr. _fld04. heap = takeshare( from. repr. _fld04. heap );
      break;
   case ConditionStmt:
      tvm::init( repr. _fld05. loc, from. repr. _fld05. loc );
      repr. _fld05. heap = takeshare( from. repr. _fld05. heap );
      break;
   case WhileLoopStmt:
   case ElifConditionStmt:
      tvm::init( repr. _fld06. loc, from. repr. _fld06. loc );
      repr. _fld06. heap = takeshare( from. repr. _fld06. heap );
      break;
   case ExprCallStmt:
   case ReturnStmt:
      tvm::init( repr. _fld07. loc, from. repr. _fld07. loc );
      repr. _fld07. heap = takeshare( from. repr. _fld07. heap );
      break;
   case FnArgsStmt:
      tvm::init( get<0> ( repr. _fld08. loc ), get<0> ( from. repr. _fld08. loc ) );
      tvm::init( get<1> ( repr. _fld08. loc ), get<1> ( from. repr. _fld08. loc ) );
      tvm::init( get<2> ( repr. _fld08. loc ), get<2> ( from. repr. _fld08. loc ) );
      tvm::init( get<3> ( repr. _fld08. loc ), get<3> ( from. repr. _fld08. loc ) );
      tvm::init( get<4> ( repr. _fld08. loc ), get<4> ( from. repr. _fld08. loc ) );
      repr. _fld08. heap = takeshare( from. repr. _fld08. heap );
      break;
   case ForLoopStmt:
      tvm::init( repr. _fld09. loc, from. repr. _fld09. loc );
      repr. _fld09. heap = takeshare( from. repr. _fld09. heap );
      break;
   case MakeEnumStmt:
      tvm::init( repr. _fld10. loc. first, from. repr. _fld10. loc. first );
      tvm::init( repr. _fld10. loc. second, from. repr. _fld10. loc. second );
      repr. _fld10. heap = takeshare( from. repr. _fld10. heap );
      break;
   case MakeFunctionStmt:
      tvm::init( get<0> ( repr. _fld11. loc ), get<0> ( from. repr. _fld11. loc ) );
      tvm::init( get<1> ( repr. _fld11. loc ), get<1> ( from. repr. _fld11. loc ) );
      tvm::init( get<2> ( repr. _fld11. loc ), get<2> ( from. repr. _fld11. loc ) );
      tvm::init( get<3> ( repr. _fld11. loc ), get<3> ( from. repr. _fld11. loc ) );
      repr. _fld11. heap = takeshare( from. repr. _fld11. heap );
      break;
   case MakePropertyStmt:
   case MakeStructStmt:
      tvm::init( repr. _fld12. loc. first, from. repr. _fld12. loc. first );
      tvm::init( repr. _fld12. loc. second, from. repr. _fld12. loc. second );
      repr. _fld12. heap = takeshare( from. repr. _fld12. heap );
      break;
   case MatchStmt:
      tvm::init( repr. _fld13. loc, from. repr. _fld13. loc );
      repr. _fld13. heap = takeshare( from. repr. _fld13. heap );
      break;
   case VarInitStmt:
      tvm::init( get<0> ( repr. _fld14. loc ), get<0> ( from. repr. _fld14. loc ) );
      tvm::init( get<1> ( repr. _fld14. loc ), get<1> ( from. repr. _fld14. loc ) );
      tvm::init( get<2> ( repr. _fld14. loc ), get<2> ( from. repr. _fld14. loc ) );
      repr. _fld14. heap = takeshare( from. repr. _fld14. heap );
      break;
   }
}

val::Statement::Statement( Statement&& from ) noexcept
   : _ssss( from. _ssss )
{
   // std::cout << "Statement( Statement&& " << from. _ssss << " )";
   
   switch( from. _ssss )
   {
   case ArrayInitStmt:
      tvm::init( repr. _fld00. loc. first, std::move( from. repr. _fld00. loc. first ) );
      tvm::destroy( from. repr. _fld00. loc. first );
      tvm::init( repr. _fld00. loc. second, std::move( from. repr. _fld00. loc. second ) );
      tvm::destroy( from. repr. _fld00. loc. second );
      repr. _fld00. heap = from. repr. _fld00. heap;
      break;
   case AssignmentStmt:
      tvm::init( repr. _fld01. loc, std::move( from. repr. _fld01. loc ) );
      tvm::destroy( from. repr. _fld01. loc );
      repr. _fld01. heap = from. repr. _fld01. heap;
      break;
   case BlockOfStmt:
      tvm::init( repr. _fld02. loc, std::move( from. repr. _fld02. loc ) );
      tvm::destroy( from. repr. _fld02. loc );
      repr. _fld02. heap = from. repr. _fld02. heap;
      break;
   case BreakStmt:
   case ContinueStmt:
   case EmptyStmt:
      tvm::init( repr. _fld03. loc, std::move( from. repr. _fld03. loc ) );
      tvm::destroy( from. repr. _fld03. loc );
      break;
   case CaseClauseStmt:
      tvm::init( repr. _fld04. loc. first, std::move( from. repr. _fld04. loc. first ) );
      tvm::destroy( from. repr. _fld04. loc. first );
      tvm::init( repr. _fld04. loc. second, std::move( from. repr. _fld04. loc. second ) );
      tvm::destroy( from. repr. _fld04. loc. second );
      repr. _fld04. heap = from. repr. _fld04. heap;
      break;
   case ConditionStmt:
      tvm::init( repr. _fld05. loc, std::move( from. repr. _fld05. loc ) );
      tvm::destroy( from. repr. _fld05. loc );
      repr. _fld05. heap = from. repr. _fld05. heap;
      break;
   case WhileLoopStmt:
   case ElifConditionStmt:
      tvm::init( repr. _fld06. loc, std::move( from. repr. _fld06. loc ) );
      tvm::destroy( from. repr. _fld06. loc );
      repr. _fld06. heap = from. repr. _fld06. heap;
      break;
   case ExprCallStmt:
   case ReturnStmt:
      tvm::init( repr. _fld07. loc, std::move( from. repr. _fld07. loc ) );
      tvm::destroy( from. repr. _fld07. loc );
      repr. _fld07. heap = from. repr. _fld07. heap;
      break;
   case FnArgsStmt:
      tvm::init( get<0> ( repr. _fld08. loc ), std::move( get<0> ( from. repr. _fld08. loc ) ) );
      tvm::destroy( get<0> ( from. repr. _fld08. loc ) );
      tvm::init( get<1> ( repr. _fld08. loc ), std::move( get<1> ( from. repr. _fld08. loc ) ) );
      tvm::destroy( get<1> ( from. repr. _fld08. loc ) );
      tvm::init( get<2> ( repr. _fld08. loc ), std::move( get<2> ( from. repr. _fld08. loc ) ) );
      tvm::destroy( get<2> ( from. repr. _fld08. loc ) );
      tvm::init( get<3> ( repr. _fld08. loc ), std::move( get<3> ( from. repr. _fld08. loc ) ) );
      tvm::destroy( get<3> ( from. repr. _fld08. loc ) );
      tvm::init( get<4> ( repr. _fld08. loc ), std::move( get<4> ( from. repr. _fld08. loc ) ) );
      tvm::destroy( get<4> ( from. repr. _fld08. loc ) );
      repr. _fld08. heap = from. repr. _fld08. heap;
      break;
   case ForLoopStmt:
      tvm::init( repr. _fld09. loc, std::move( from. repr. _fld09. loc ) );
      tvm::destroy( from. repr. _fld09. loc );
      repr. _fld09. heap = from. repr. _fld09. heap;
      break;
   case MakeEnumStmt:
      tvm::init( repr. _fld10. loc. first, std::move( from. repr. _fld10. loc. first ) );
      tvm::destroy( from. repr. _fld10. loc. first );
      tvm::init( repr. _fld10. loc. second, std::move( from. repr. _fld10. loc. second ) );
      tvm::destroy( from. repr. _fld10. loc. second );
      repr. _fld10. heap = from. repr. _fld10. heap;
      break;
   case MakeFunctionStmt:
      tvm::init( get<0> ( repr. _fld11. loc ), std::move( get<0> ( from. repr. _fld11. loc ) ) );
      tvm::destroy( get<0> ( from. repr. _fld11. loc ) );
      tvm::init( get<1> ( repr. _fld11. loc ), std::move( get<1> ( from. repr. _fld11. loc ) ) );
      tvm::destroy( get<1> ( from. repr. _fld11. loc ) );
      tvm::init( get<2> ( repr. _fld11. loc ), std::move( get<2> ( from. repr. _fld11. loc ) ) );
      tvm::destroy( get<2> ( from. repr. _fld11. loc ) );
      tvm::init( get<3> ( repr. _fld11. loc ), std::move( get<3> ( from. repr. _fld11. loc ) ) );
      tvm::destroy( get<3> ( from. repr. _fld11. loc ) );
      repr. _fld11. heap = from. repr. _fld11. heap;
      break;
   case MakePropertyStmt:
   case MakeStructStmt:
      tvm::init( repr. _fld12. loc. first, std::move( from. repr. _fld12. loc. first ) );
      tvm::destroy( from. repr. _fld12. loc. first );
      tvm::init( repr. _fld12. loc. second, std::move( from. repr. _fld12. loc. second ) );
      tvm::destroy( from. repr. _fld12. loc. second );
      repr. _fld12. heap = from. repr. _fld12. heap;
      break;
   case MatchStmt:
      tvm::init( repr. _fld13. loc, std::move( from. repr. _fld13. loc ) );
      tvm::destroy( from. repr. _fld13. loc );
      repr. _fld13. heap = from. repr. _fld13. heap;
      break;
   case VarInitStmt:
      tvm::init( get<0> ( repr. _fld14. loc ), std::move( get<0> ( from. repr. _fld14. loc ) ) );
      tvm::destroy( get<0> ( from. repr. _fld14. loc ) );
      tvm::init( get<1> ( repr. _fld14. loc ), std::move( get<1> ( from. repr. _fld14. loc ) ) );
      tvm::destroy( get<1> ( from. repr. _fld14. loc ) );
      tvm::init( get<2> ( repr. _fld14. loc ), std::move( get<2> ( from. repr. _fld14. loc ) ) );
      tvm::destroy( get<2> ( from. repr. _fld14. loc ) );
      repr. _fld14. heap = from. repr. _fld14. heap;
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
   case ArrayInitStmt:
      takeshare( from. repr. _fld00. heap );
      break;
   case AssignmentStmt:
      takeshare( from. repr. _fld01. heap );
      break;
   case BlockOfStmt:
      takeshare( from. repr. _fld02. heap );
      break;
   case CaseClauseStmt:
      takeshare( from. repr. _fld04. heap );
      break;
   case ConditionStmt:
      takeshare( from. repr. _fld05. heap );
      break;
   case WhileLoopStmt:
   case ElifConditionStmt:
      takeshare( from. repr. _fld06. heap );
      break;
   case ExprCallStmt:
   case ReturnStmt:
      takeshare( from. repr. _fld07. heap );
      break;
   case FnArgsStmt:
      takeshare( from. repr. _fld08. heap );
      break;
   case ForLoopStmt:
      takeshare( from. repr. _fld09. heap );
      break;
   case MakeEnumStmt:
      takeshare( from. repr. _fld10. heap );
      break;
   case MakeFunctionStmt:
      takeshare( from. repr. _fld11. heap );
      break;
   case MakePropertyStmt:
   case MakeStructStmt:
      takeshare( from. repr. _fld12. heap );
      break;
   case MatchStmt:
      takeshare( from. repr. _fld13. heap );
      break;
   case VarInitStmt:
      takeshare( from. repr. _fld14. heap );
      break;
   }

   this -> ~Statement( );
   
   _ssss = from. _ssss;
   
   switch( _ssss )
   {
   case ArrayInitStmt:
      tvm::init( repr. _fld00. loc. first, from. repr. _fld00. loc. first );
      tvm::init( repr. _fld00. loc. second, from. repr. _fld00. loc. second );
      repr. _fld00. heap = from. repr. _fld00. heap;
      break;
   case AssignmentStmt:
      tvm::init( repr. _fld01. loc, from. repr. _fld01. loc );
      repr. _fld01. heap = from. repr. _fld01. heap;
      break;
   case BlockOfStmt:
      tvm::init( repr. _fld02. loc, from. repr. _fld02. loc );
      repr. _fld02. heap = from. repr. _fld02. heap;
      break;
   case BreakStmt:
   case ContinueStmt:
   case EmptyStmt:
      tvm::init( repr. _fld03. loc, from. repr. _fld03. loc );
      break;
   case CaseClauseStmt:
      tvm::init( repr. _fld04. loc. first, from. repr. _fld04. loc. first );
      tvm::init( repr. _fld04. loc. second, from. repr. _fld04. loc. second );
      repr. _fld04. heap = from. repr. _fld04. heap;
      break;
   case ConditionStmt:
      tvm::init( repr. _fld05. loc, from. repr. _fld05. loc );
      repr. _fld05. heap = from. repr. _fld05. heap;
      break;
   case WhileLoopStmt:
   case ElifConditionStmt:
      tvm::init( repr. _fld06. loc, from. repr. _fld06. loc );
      repr. _fld06. heap = from. repr. _fld06. heap;
      break;
   case ExprCallStmt:
   case ReturnStmt:
      tvm::init( repr. _fld07. loc, from. repr. _fld07. loc );
      repr. _fld07. heap = from. repr. _fld07. heap;
      break;
   case FnArgsStmt:
      tvm::init( get<0> ( repr. _fld08. loc ), get<0> ( from. repr. _fld08. loc ) );
      tvm::init( get<1> ( repr. _fld08. loc ), get<1> ( from. repr. _fld08. loc ) );
      tvm::init( get<2> ( repr. _fld08. loc ), get<2> ( from. repr. _fld08. loc ) );
      tvm::init( get<3> ( repr. _fld08. loc ), get<3> ( from. repr. _fld08. loc ) );
      tvm::init( get<4> ( repr. _fld08. loc ), get<4> ( from. repr. _fld08. loc ) );
      repr. _fld08. heap = from. repr. _fld08. heap;
      break;
   case ForLoopStmt:
      tvm::init( repr. _fld09. loc, from. repr. _fld09. loc );
      repr. _fld09. heap = from. repr. _fld09. heap;
      break;
   case MakeEnumStmt:
      tvm::init( repr. _fld10. loc. first, from. repr. _fld10. loc. first );
      tvm::init( repr. _fld10. loc. second, from. repr. _fld10. loc. second );
      repr. _fld10. heap = from. repr. _fld10. heap;
      break;
   case MakeFunctionStmt:
      tvm::init( get<0> ( repr. _fld11. loc ), get<0> ( from. repr. _fld11. loc ) );
      tvm::init( get<1> ( repr. _fld11. loc ), get<1> ( from. repr. _fld11. loc ) );
      tvm::init( get<2> ( repr. _fld11. loc ), get<2> ( from. repr. _fld11. loc ) );
      tvm::init( get<3> ( repr. _fld11. loc ), get<3> ( from. repr. _fld11. loc ) );
      repr. _fld11. heap = from. repr. _fld11. heap;
      break;
   case MakePropertyStmt:
   case MakeStructStmt:
      tvm::init( repr. _fld12. loc. first, from. repr. _fld12. loc. first );
      tvm::init( repr. _fld12. loc. second, from. repr. _fld12. loc. second );
      repr. _fld12. heap = from. repr. _fld12. heap;
      break;
   case MatchStmt:
      tvm::init( repr. _fld13. loc, from. repr. _fld13. loc );
      repr. _fld13. heap = from. repr. _fld13. heap;
      break;
   case VarInitStmt:
      tvm::init( get<0> ( repr. _fld14. loc ), get<0> ( from. repr. _fld14. loc ) );
      tvm::init( get<1> ( repr. _fld14. loc ), get<1> ( from. repr. _fld14. loc ) );
      tvm::init( get<2> ( repr. _fld14. loc ), get<2> ( from. repr. _fld14. loc ) );
      repr. _fld14. heap = from. repr. _fld14. heap;
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
      case ArrayInitStmt:
         tvm::assign( repr. _fld00. loc. first, std::move( from. repr. _fld00. loc. first ) );
         tvm::destroy( from. repr. _fld00. loc. first );
         tvm::assign( repr. _fld00. loc. second, std::move( from. repr. _fld00. loc. second ) );
         tvm::destroy( from. repr. _fld00. loc. second );
         dropshare( repr. _fld00. heap );
         repr. _fld00. heap = from. repr. _fld00. heap;
         break;
      case AssignmentStmt:
         tvm::assign( repr. _fld01. loc, std::move( from. repr. _fld01. loc ) );
         tvm::destroy( from. repr. _fld01. loc );
         dropshare( repr. _fld01. heap );
         repr. _fld01. heap = from. repr. _fld01. heap;
         break;
      case BlockOfStmt:
         tvm::assign( repr. _fld02. loc, std::move( from. repr. _fld02. loc ) );
         tvm::destroy( from. repr. _fld02. loc );
         dropshare( repr. _fld02. heap );
         repr. _fld02. heap = from. repr. _fld02. heap;
         break;
      case BreakStmt:
      case ContinueStmt:
      case EmptyStmt:
         tvm::assign( repr. _fld03. loc, std::move( from. repr. _fld03. loc ) );
         tvm::destroy( from. repr. _fld03. loc );
         break;
      case CaseClauseStmt:
         tvm::assign( repr. _fld04. loc. first, std::move( from. repr. _fld04. loc. first ) );
         tvm::destroy( from. repr. _fld04. loc. first );
         tvm::assign( repr. _fld04. loc. second, std::move( from. repr. _fld04. loc. second ) );
         tvm::destroy( from. repr. _fld04. loc. second );
         dropshare( repr. _fld04. heap );
         repr. _fld04. heap = from. repr. _fld04. heap;
         break;
      case ConditionStmt:
         tvm::assign( repr. _fld05. loc, std::move( from. repr. _fld05. loc ) );
         tvm::destroy( from. repr. _fld05. loc );
         dropshare( repr. _fld05. heap );
         repr. _fld05. heap = from. repr. _fld05. heap;
         break;
      case WhileLoopStmt:
      case ElifConditionStmt:
         tvm::assign( repr. _fld06. loc, std::move( from. repr. _fld06. loc ) );
         tvm::destroy( from. repr. _fld06. loc );
         dropshare( repr. _fld06. heap );
         repr. _fld06. heap = from. repr. _fld06. heap;
         break;
      case ExprCallStmt:
      case ReturnStmt:
         tvm::assign( repr. _fld07. loc, std::move( from. repr. _fld07. loc ) );
         tvm::destroy( from. repr. _fld07. loc );
         dropshare( repr. _fld07. heap );
         repr. _fld07. heap = from. repr. _fld07. heap;
         break;
      case FnArgsStmt:
         tvm::assign( get<0> ( repr. _fld08. loc ), std::move( get<0> ( from. repr. _fld08. loc ) ) );
         tvm::destroy( get<0> ( from. repr. _fld08. loc ) );
         tvm::assign( get<1> ( repr. _fld08. loc ), std::move( get<1> ( from. repr. _fld08. loc ) ) );
         tvm::destroy( get<1> ( from. repr. _fld08. loc ) );
         tvm::assign( get<2> ( repr. _fld08. loc ), std::move( get<2> ( from. repr. _fld08. loc ) ) );
         tvm::destroy( get<2> ( from. repr. _fld08. loc ) );
         tvm::assign( get<3> ( repr. _fld08. loc ), std::move( get<3> ( from. repr. _fld08. loc ) ) );
         tvm::destroy( get<3> ( from. repr. _fld08. loc ) );
         tvm::assign( get<4> ( repr. _fld08. loc ), std::move( get<4> ( from. repr. _fld08. loc ) ) );
         tvm::destroy( get<4> ( from. repr. _fld08. loc ) );
         dropshare( repr. _fld08. heap );
         repr. _fld08. heap = from. repr. _fld08. heap;
         break;
      case ForLoopStmt:
         tvm::assign( repr. _fld09. loc, std::move( from. repr. _fld09. loc ) );
         tvm::destroy( from. repr. _fld09. loc );
         dropshare( repr. _fld09. heap );
         repr. _fld09. heap = from. repr. _fld09. heap;
         break;
      case MakeEnumStmt:
         tvm::assign( repr. _fld10. loc. first, std::move( from. repr. _fld10. loc. first ) );
         tvm::destroy( from. repr. _fld10. loc. first );
         tvm::assign( repr. _fld10. loc. second, std::move( from. repr. _fld10. loc. second ) );
         tvm::destroy( from. repr. _fld10. loc. second );
         dropshare( repr. _fld10. heap );
         repr. _fld10. heap = from. repr. _fld10. heap;
         break;
      case MakeFunctionStmt:
         tvm::assign( get<0> ( repr. _fld11. loc ), std::move( get<0> ( from. repr. _fld11. loc ) ) );
         tvm::destroy( get<0> ( from. repr. _fld11. loc ) );
         tvm::assign( get<1> ( repr. _fld11. loc ), std::move( get<1> ( from. repr. _fld11. loc ) ) );
         tvm::destroy( get<1> ( from. repr. _fld11. loc ) );
         tvm::assign( get<2> ( repr. _fld11. loc ), std::move( get<2> ( from. repr. _fld11. loc ) ) );
         tvm::destroy( get<2> ( from. repr. _fld11. loc ) );
         tvm::assign( get<3> ( repr. _fld11. loc ), std::move( get<3> ( from. repr. _fld11. loc ) ) );
         tvm::destroy( get<3> ( from. repr. _fld11. loc ) );
         dropshare( repr. _fld11. heap );
         repr. _fld11. heap = from. repr. _fld11. heap;
         break;
      case MakePropertyStmt:
      case MakeStructStmt:
         tvm::assign( repr. _fld12. loc. first, std::move( from. repr. _fld12. loc. first ) );
         tvm::destroy( from. repr. _fld12. loc. first );
         tvm::assign( repr. _fld12. loc. second, std::move( from. repr. _fld12. loc. second ) );
         tvm::destroy( from. repr. _fld12. loc. second );
         dropshare( repr. _fld12. heap );
         repr. _fld12. heap = from. repr. _fld12. heap;
         break;
      case MatchStmt:
         tvm::assign( repr. _fld13. loc, std::move( from. repr. _fld13. loc ) );
         tvm::destroy( from. repr. _fld13. loc );
         dropshare( repr. _fld13. heap );
         repr. _fld13. heap = from. repr. _fld13. heap;
         break;
      case VarInitStmt:
         tvm::assign( get<0> ( repr. _fld14. loc ), std::move( get<0> ( from. repr. _fld14. loc ) ) );
         tvm::destroy( get<0> ( from. repr. _fld14. loc ) );
         tvm::assign( get<1> ( repr. _fld14. loc ), std::move( get<1> ( from. repr. _fld14. loc ) ) );
         tvm::destroy( get<1> ( from. repr. _fld14. loc ) );
         tvm::assign( get<2> ( repr. _fld14. loc ), std::move( get<2> ( from. repr. _fld14. loc ) ) );
         tvm::destroy( get<2> ( from. repr. _fld14. loc ) );
         dropshare( repr. _fld14. heap );
         repr. _fld14. heap = from. repr. _fld14. heap;
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
   case ArrayInitStmt:
      tvm::destroy( repr. _fld00. loc. first );
      tvm::destroy( repr. _fld00. loc. second );
      dropshare( repr. _fld00. heap );
      break;
   case AssignmentStmt:
      tvm::destroy( repr. _fld01. loc );
      dropshare( repr. _fld01. heap );
      break;
   case BlockOfStmt:
      tvm::destroy( repr. _fld02. loc );
      dropshare( repr. _fld02. heap );
      break;
   case BreakStmt:
   case ContinueStmt:
   case EmptyStmt:
      tvm::destroy( repr. _fld03. loc );
      break;
   case CaseClauseStmt:
      tvm::destroy( repr. _fld04. loc. first );
      tvm::destroy( repr. _fld04. loc. second );
      dropshare( repr. _fld04. heap );
      break;
   case ConditionStmt:
      tvm::destroy( repr. _fld05. loc );
      dropshare( repr. _fld05. heap );
      break;
   case WhileLoopStmt:
   case ElifConditionStmt:
      tvm::destroy( repr. _fld06. loc );
      dropshare( repr. _fld06. heap );
      break;
   case ExprCallStmt:
   case ReturnStmt:
      tvm::destroy( repr. _fld07. loc );
      dropshare( repr. _fld07. heap );
      break;
   case FnArgsStmt:
      tvm::destroy( get<0> ( repr. _fld08. loc ) );
      tvm::destroy( get<1> ( repr. _fld08. loc ) );
      tvm::destroy( get<2> ( repr. _fld08. loc ) );
      tvm::destroy( get<3> ( repr. _fld08. loc ) );
      tvm::destroy( get<4> ( repr. _fld08. loc ) );
      dropshare( repr. _fld08. heap );
      break;
   case ForLoopStmt:
      tvm::destroy( repr. _fld09. loc );
      dropshare( repr. _fld09. heap );
      break;
   case MakeEnumStmt:
      tvm::destroy( repr. _fld10. loc. first );
      tvm::destroy( repr. _fld10. loc. second );
      dropshare( repr. _fld10. heap );
      break;
   case MakeFunctionStmt:
      tvm::destroy( get<0> ( repr. _fld11. loc ) );
      tvm::destroy( get<1> ( repr. _fld11. loc ) );
      tvm::destroy( get<2> ( repr. _fld11. loc ) );
      tvm::destroy( get<3> ( repr. _fld11. loc ) );
      dropshare( repr. _fld11. heap );
      break;
   case MakePropertyStmt:
   case MakeStructStmt:
      tvm::destroy( repr. _fld12. loc. first );
      tvm::destroy( repr. _fld12. loc. second );
      dropshare( repr. _fld12. heap );
      break;
   case MatchStmt:
      tvm::destroy( repr. _fld13. loc );
      dropshare( repr. _fld13. heap );
      break;
   case VarInitStmt:
      tvm::destroy( get<0> ( repr. _fld14. loc ) );
      tvm::destroy( get<1> ( repr. _fld14. loc ) );
      tvm::destroy( get<2> ( repr. _fld14. loc ) );
      dropshare( repr. _fld14. heap );
      break;
   }
}

bool val::Statement::very_equal_to( const Statement & other ) const
{
   if( _ssss != other. _ssss )
      return false;

   switch( _ssss )
   {
   case ArrayInitStmt:
      if( tvm::distinct( repr. _fld00. loc. first, other. repr. _fld00. loc. first ))
         return false;
      if( tvm::distinct( repr. _fld00. loc. second, other. repr. _fld00. loc. second ))
         return false;
      if( repr. _fld00. heap != other. repr. _fld00. heap )
         return false;
      return true;
   case AssignmentStmt:
      if( tvm::distinct( repr. _fld01. loc, other. repr. _fld01. loc ))
         return false;
      if( repr. _fld01. heap != other. repr. _fld01. heap )
         return false;
      return true;
   case BlockOfStmt:
      if( tvm::distinct( repr. _fld02. loc, other. repr. _fld02. loc ))
         return false;
      if( repr. _fld02. heap != other. repr. _fld02. heap )
         return false;
      return true;
   case BreakStmt:
   case ContinueStmt:
   case EmptyStmt:
      if( tvm::distinct( repr. _fld03. loc, other. repr. _fld03. loc ))
         return false;
      return true;
   case CaseClauseStmt:
      if( tvm::distinct( repr. _fld04. loc. first, other. repr. _fld04. loc. first ))
         return false;
      if( tvm::distinct( repr. _fld04. loc. second, other. repr. _fld04. loc. second ))
         return false;
      if( repr. _fld04. heap != other. repr. _fld04. heap )
         return false;
      return true;
   case ConditionStmt:
      if( tvm::distinct( repr. _fld05. loc, other. repr. _fld05. loc ))
         return false;
      if( repr. _fld05. heap != other. repr. _fld05. heap )
         return false;
      return true;
   case WhileLoopStmt:
   case ElifConditionStmt:
      if( tvm::distinct( repr. _fld06. loc, other. repr. _fld06. loc ))
         return false;
      if( repr. _fld06. heap != other. repr. _fld06. heap )
         return false;
      return true;
   case ExprCallStmt:
   case ReturnStmt:
      if( tvm::distinct( repr. _fld07. loc, other. repr. _fld07. loc ))
         return false;
      if( repr. _fld07. heap != other. repr. _fld07. heap )
         return false;
      return true;
   case FnArgsStmt:
      if( tvm::distinct( get<0> ( repr. _fld08. loc ), get<0> ( other. repr. _fld08. loc ) ))
         return false;
      if( tvm::distinct( get<1> ( repr. _fld08. loc ), get<1> ( other. repr. _fld08. loc ) ))
         return false;
      if( tvm::distinct( get<2> ( repr. _fld08. loc ), get<2> ( other. repr. _fld08. loc ) ))
         return false;
      if( tvm::distinct( get<3> ( repr. _fld08. loc ), get<3> ( other. repr. _fld08. loc ) ))
         return false;
      if( tvm::distinct( get<4> ( repr. _fld08. loc ), get<4> ( other. repr. _fld08. loc ) ))
         return false;
      if( repr. _fld08. heap != other. repr. _fld08. heap )
         return false;
      return true;
   case ForLoopStmt:
      if( tvm::distinct( repr. _fld09. loc, other. repr. _fld09. loc ))
         return false;
      if( repr. _fld09. heap != other. repr. _fld09. heap )
         return false;
      return true;
   case MakeEnumStmt:
      if( tvm::distinct( repr. _fld10. loc. first, other. repr. _fld10. loc. first ))
         return false;
      if( tvm::distinct( repr. _fld10. loc. second, other. repr. _fld10. loc. second ))
         return false;
      if( repr. _fld10. heap != other. repr. _fld10. heap )
         return false;
      return true;
   case MakeFunctionStmt:
      if( tvm::distinct( get<0> ( repr. _fld11. loc ), get<0> ( other. repr. _fld11. loc ) ))
         return false;
      if( tvm::distinct( get<1> ( repr. _fld11. loc ), get<1> ( other. repr. _fld11. loc ) ))
         return false;
      if( tvm::distinct( get<2> ( repr. _fld11. loc ), get<2> ( other. repr. _fld11. loc ) ))
         return false;
      if( tvm::distinct( get<3> ( repr. _fld11. loc ), get<3> ( other. repr. _fld11. loc ) ))
         return false;
      if( repr. _fld11. heap != other. repr. _fld11. heap )
         return false;
      return true;
   case MakePropertyStmt:
   case MakeStructStmt:
      if( tvm::distinct( repr. _fld12. loc. first, other. repr. _fld12. loc. first ))
         return false;
      if( tvm::distinct( repr. _fld12. loc. second, other. repr. _fld12. loc. second ))
         return false;
      if( repr. _fld12. heap != other. repr. _fld12. heap )
         return false;
      return true;
   case MatchStmt:
      if( tvm::distinct( repr. _fld13. loc, other. repr. _fld13. loc ))
         return false;
      if( repr. _fld13. heap != other. repr. _fld13. heap )
         return false;
      return true;
   case VarInitStmt:
      if( tvm::distinct( get<0> ( repr. _fld14. loc ), get<0> ( other. repr. _fld14. loc ) ))
         return false;
      if( tvm::distinct( get<1> ( repr. _fld14. loc ), get<1> ( other. repr. _fld14. loc ) ))
         return false;
      if( tvm::distinct( get<2> ( repr. _fld14. loc ), get<2> ( other. repr. _fld14. loc ) ))
         return false;
      if( repr. _fld14. heap != other. repr. _fld14. heap )
         return false;
      return true;
   }
}

void val::Statement::printstate( std::ostream& out ) const
{
   switch( _ssss )
   {
   case ArrayInitStmt:
      tvm::printstate( repr. _fld00. heap, out );
      break;
   case AssignmentStmt:
      tvm::printstate( repr. _fld01. heap, out );
      break;
   case BlockOfStmt:
      tvm::printstate( repr. _fld02. heap, out );
      break;
   case CaseClauseStmt:
      tvm::printstate( repr. _fld04. heap, out );
      break;
   case ConditionStmt:
      tvm::printstate( repr. _fld05. heap, out );
      break;
   case WhileLoopStmt:
   case ElifConditionStmt:
      tvm::printstate( repr. _fld06. heap, out );
      break;
   case ExprCallStmt:
   case ReturnStmt:
      tvm::printstate( repr. _fld07. heap, out );
      break;
   case FnArgsStmt:
      tvm::printstate( repr. _fld08. heap, out );
      break;
   case ForLoopStmt:
      tvm::printstate( repr. _fld09. heap, out );
      break;
   case MakeEnumStmt:
      tvm::printstate( repr. _fld10. heap, out );
      break;
   case MakeFunctionStmt:
      tvm::printstate( repr. _fld11. heap, out );
      break;
   case MakePropertyStmt:
   case MakeStructStmt:
      tvm::printstate( repr. _fld12. heap, out );
      break;
   case MatchStmt:
      tvm::printstate( repr. _fld13. heap, out );
      break;
   case VarInitStmt:
      tvm::printstate( repr. _fld14. heap, out );
      break;
   }
}

