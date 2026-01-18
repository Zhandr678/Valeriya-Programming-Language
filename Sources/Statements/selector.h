
// This code was automatically built by TreeGen
// Written by Hans de Nivelle, see www.compiler-tools.eu

#ifndef VAL_SELECTOR_
#define VAL_SELECTOR_

#include <iostream>


namespace val { 

   enum selector
   {      
      ElifConditionStmt, ContinueStmt, ArrayIndex, VarInitStmt, 
      FnCallExpr, BinaryExpr, BoolLiteralExpr, CharLiteralExpr, 
      DoubleLiteralExpr, FieldCallExpr, EmptyExpr, IntLiteralExpr, 
      StringLiteralExpr, StructInitExpr, UnaryExpr, VarNameExpr, 
      BreakStmt, AssignmentStmt, BlockOfStmt, CaseClauseStmt, 
      EmptyStmt, ForLoopStmt, MakeStructStmt, ConditionStmt, 
      MakePropertyStmt, ExprCallStmt, FnArgsStmt, MakeEnumStmt, 
      MakeFunctionStmt, MatchStmt, ReturnStmt, WhileLoopStmt
   };

   const char* getcstring( selector );

   inline std::ostream& operator << ( std::ostream& out, selector sel )
      { out << getcstring( sel ); return out; }

}


#endif


