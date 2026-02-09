
// This code was automatically built by TreeGen
// Written by Hans de Nivelle, see www.compiler-tools.eu

#ifndef VAL_STATEMENT_
#define VAL_STATEMENT_

#include <iostream>
#include <utility>
#include <tuple>
#include <initializer_list>
#include "selector.h"

#include "Expression.h"

namespace val { 


   class Statement
   {
   private:
      selector _ssss;

      using _loc00 = bool;
      using _scal00 = std::tuple<Expression,Statement,Expression>;
      using _rep00 = tvm::unit;
      using _loc01 = tvm::unit;
      using _scal01 = std::pair<Expression,Expression>;
      using _rep01 = tvm::unit;
      using _loc02 = tvm::unit;
      using _scal02 = tvm::unit;
      using _rep02 = Statement;
      using _loc03 = tvm::unit;
      using _scal03 = tvm::unit;
      using _rep03 = tvm::unit;
      using _loc04 = bool;
      using _scal04 = std::pair<Statement,Expression>;
      using _rep04 = tvm::unit;
      using _loc05 = tvm::unit;
      using _scal05 = std::tuple<Expression,Statement,Statement>;
      using _rep05 = Statement;
      using _loc06 = tvm::unit;
      using _scal06 = std::pair<Expression,Statement>;
      using _rep06 = tvm::unit;
      using _loc07 = tvm::unit;
      using _scal07 = Expression;
      using _rep07 = tvm::unit;
      using _loc08 = std::tuple<std::string,bool,std::string,bool>;
      using _scal08 = Expression;
      using _rep08 = std::pair<bool, int>;
      using _loc09 = tvm::unit;
      using _scal09 = std::tuple<Statement,Expression,Statement,Statement>;
      using _rep09 = tvm::unit;
      using _loc10 = std::string;
      using _scal10 = tvm::unit;
      using _rep10 = std::string;
      using _loc11 = std::pair<std::string,std::string>;
      using _scal11 = Statement;
      using _rep11 = Statement;
      using _loc12 = std::string;
      using _scal12 = tvm::unit;
      using _rep12 = Statement;
      using _loc13 = tvm::unit;
      using _scal13 = Expression;
      using _rep13 = Statement;
      using _loc14 = std::pair<std::string,std::string>;
      using _scal14 = Expression;
      using _rep14 = tvm::unit;

      static constexpr bool check = true;

      union options
      {
         tvm::field< _loc00, _scal00, _rep00 > _fld00;
         tvm::field< _loc01, _scal01, _rep01 > _fld01;
         tvm::field< _loc02, _scal02, _rep02 > _fld02;
         tvm::field< _loc03, _scal03, _rep03 > _fld03;
         tvm::field< _loc04, _scal04, _rep04 > _fld04;
         tvm::field< _loc05, _scal05, _rep05 > _fld05;
         tvm::field< _loc06, _scal06, _rep06 > _fld06;
         tvm::field< _loc07, _scal07, _rep07 > _fld07;
         tvm::field< _loc08, _scal08, _rep08 > _fld08;
         tvm::field< _loc09, _scal09, _rep09 > _fld09;
         tvm::field< _loc10, _scal10, _rep10 > _fld10;
         tvm::field< _loc11, _scal11, _rep11 > _fld11;
         tvm::field< _loc12, _scal12, _rep12 > _fld12;
         tvm::field< _loc13, _scal13, _rep13 > _fld13;
         tvm::field< _loc14, _scal14, _rep14 > _fld14;

         options( ) : _fld03( ) { }
         ~options( ) noexcept { }
      };

   private:
      options repr;

   public:
      Statement( ) = delete;
      Statement( const Statement& );
      Statement( Statement&& ) noexcept;
      const Statement& operator = ( const Statement& );
      const Statement& operator = ( Statement&& ) noexcept;
      ~Statement( ) noexcept;
      
      selector sel( ) const { return _ssss; }
      bool very_equal_to( const Statement& ) const;
      void printstate( std::ostream& out ) const;
      
      Statement( selector sel, const Expression & _xx00, const Statement & _xx01, const Expression & _xx02, const bool & _xx03 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case ArrayInitStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld00. loc, _xx03 );
         repr. _fld00. heap = takeshare( tvm::constr_scalar< _scal00 > ( std::tuple( _xx00, _xx01, _xx02 ) ));
      }

      Statement( selector sel, const Expression & _xx00, const Expression & _xx01 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case AssignmentStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         repr. _fld01. heap = takeshare( tvm::constr_scalar< _scal01 > ( std::pair( _xx00, _xx01 ) ));
      }

      template< tvm::const_iterator< _rep02 > It >
      Statement( selector sel, It begin, It end )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case BlockOfStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         repr. _fld02. heap = takeshare( tvm::constr_scalar_repeated< _scal02, _rep02 > ( tvm::unit( ), begin, end ));
      }

      Statement( selector sel, std::initializer_list< _rep02 > repeated )
         : Statement( sel, repeated. begin( ), repeated. end( ) )
      { }

      Statement( selector sel )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case BreakStmt:
            case ContinueStmt:
            case EmptyStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
      }

      Statement( selector sel, const Statement & _xx00, const Expression & _xx01, const bool & _xx02 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case CaseClauseStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld04. loc, _xx02 );
         repr. _fld04. heap = takeshare( tvm::constr_scalar< _scal04 > ( std::pair( _xx00, _xx01 ) ));
      }

      template< tvm::const_iterator< _rep05 > It >
      Statement( selector sel, const Expression & _xx00, const Statement & _xx01, const Statement & _xx02, It begin, It end )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case ConditionStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         repr. _fld05. heap = takeshare( tvm::constr_scalar_repeated< _scal05, _rep05 > ( std::tuple( _xx00, _xx01, _xx02 ), begin, end ));
      }

      Statement( selector sel, const Expression & _xx00, const Statement & _xx01, const Statement & _xx02, std::initializer_list< _rep05 > repeated )
         : Statement( sel, _xx00, _xx01, _xx02, repeated. begin( ), repeated. end( ) )
      { }

      Statement( selector sel, const Expression & _xx00, const Statement & _xx01 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case WhileLoopStmt:
            case ElifConditionStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         repr. _fld06. heap = takeshare( tvm::constr_scalar< _scal06 > ( std::pair( _xx00, _xx01 ) ));
      }

      Statement( selector sel, const Expression & _xx00 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case ExprCallStmt:
            case ReturnStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         repr. _fld07. heap = takeshare( tvm::constr_scalar< _scal07 > ( _xx00 ));
      }

      template< tvm::const_iterator< _rep08 > It >
      Statement( selector sel, const Expression & _xx00, It begin, It end, const std::string & _xx03, const bool & _xx04, const std::string & _xx05, const bool & _xx06 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case FnArgsStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( get<0> ( repr. _fld08. loc ), _xx03 );
         tvm::init( get<1> ( repr. _fld08. loc ), _xx04 );
         tvm::init( get<2> ( repr. _fld08. loc ), _xx05 );
         tvm::init( get<3> ( repr. _fld08. loc ), _xx06 );
         repr. _fld08. heap = takeshare( tvm::constr_scalar_repeated< _scal08, _rep08 > ( _xx00, begin, end ));
      }

      Statement( selector sel, const Expression & _xx00, std::initializer_list< _rep08 > repeated, const std::string & _xx02, const bool & _xx03, const std::string & _xx04, const bool & _xx05 )
         : Statement( sel, _xx00, repeated. begin( ), repeated. end( ), _xx02, _xx03, _xx04, _xx05 )
      { }

      Statement( selector sel, const Statement & _xx00, const Expression & _xx01, const Statement & _xx02, const Statement & _xx03 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case ForLoopStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         repr. _fld09. heap = takeshare( tvm::constr_scalar< _scal09 > ( std::tuple( _xx00, _xx01, _xx02, _xx03 ) ));
      }

      template< tvm::const_iterator< _rep10 > It >
      Statement( selector sel, It begin, It end, const std::string & _xx02 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case MakeEnumStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld10. loc, _xx02 );
         repr. _fld10. heap = takeshare( tvm::constr_scalar_repeated< _scal10, _rep10 > ( tvm::unit( ), begin, end ));
      }

      Statement( selector sel, std::initializer_list< _rep10 > repeated, const std::string & _xx01 )
         : Statement( sel, repeated. begin( ), repeated. end( ), _xx01 )
      { }

      template< tvm::const_iterator< _rep11 > It >
      Statement( selector sel, const Statement & _xx00, It begin, It end, const std::string & _xx03, const std::string & _xx04 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case MakeFunctionStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld11. loc. first, _xx03 );
         tvm::init( repr. _fld11. loc. second, _xx04 );
         repr. _fld11. heap = takeshare( tvm::constr_scalar_repeated< _scal11, _rep11 > ( _xx00, begin, end ));
      }

      Statement( selector sel, const Statement & _xx00, std::initializer_list< _rep11 > repeated, const std::string & _xx02, const std::string & _xx03 )
         : Statement( sel, _xx00, repeated. begin( ), repeated. end( ), _xx02, _xx03 )
      { }

      template< tvm::const_iterator< _rep12 > It >
      Statement( selector sel, It begin, It end, const std::string & _xx02 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case MakePropertyStmt:
            case MakeStructStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld12. loc, _xx02 );
         repr. _fld12. heap = takeshare( tvm::constr_scalar_repeated< _scal12, _rep12 > ( tvm::unit( ), begin, end ));
      }

      Statement( selector sel, std::initializer_list< _rep12 > repeated, const std::string & _xx01 )
         : Statement( sel, repeated. begin( ), repeated. end( ), _xx01 )
      { }

      template< tvm::const_iterator< _rep13 > It >
      Statement( selector sel, const Expression & _xx00, It begin, It end )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case MatchStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         repr. _fld13. heap = takeshare( tvm::constr_scalar_repeated< _scal13, _rep13 > ( _xx00, begin, end ));
      }

      Statement( selector sel, const Expression & _xx00, std::initializer_list< _rep13 > repeated )
         : Statement( sel, _xx00, repeated. begin( ), repeated. end( ) )
      { }

      Statement( selector sel, const Expression & _xx00, const std::string & _xx01, const std::string & _xx02 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case VarInitStmt:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld14. loc. first, _xx01 );
         tvm::init( repr. _fld14. loc. second, _xx02 );
         repr. _fld14. heap = takeshare( tvm::constr_scalar< _scal14 > ( _xx00 ));
      }

      bool option_is_ArrayInit( ) const noexcept
      {
         switch( _ssss )
         {
         case ArrayInitStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_ArrayInit
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_ArrayInit( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const bool & is_dynamic( ) const { return _xxxx -> repr. _fld00. loc; }

         const Expression & init_expr( ) const { return get<0> ( _xxxx -> repr. _fld00. heap -> scal ); }
         const Statement & type_info( ) const { return get<1> ( _xxxx -> repr. _fld00. heap -> scal ); }
         const Expression & alloc_size( ) const { return get<2> ( _xxxx -> repr. _fld00. heap -> scal ); }
      };

      const_ArrayInit view_ArrayInit( ) const
      {
         if constexpr( check )
         {
            if( !option_is_ArrayInit( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_ArrayInit
      {
         Statement* _xxxx;
         mut_ArrayInit( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         bool & is_dynamic( ) const { return _xxxx -> repr. _fld00. loc; }
         bool extr_is_dynamic( ) const { return std::move( _xxxx -> repr. _fld00. loc ); }
         void update_is_dynamic( const bool & from ) const { _xxxx -> repr. _fld00. loc = from; }

         const Expression & init_expr( ) const { return get<0> ( _xxxx -> repr. _fld00. heap -> scal ); }
         Expression extr_init_expr( ) const {
            if( iswritable( _xxxx -> repr. _fld00. heap ))
               return std::move( get<0> ( _xxxx -> repr. _fld00. heap -> scal ) );
            else
               return get<0> ( _xxxx -> repr. _fld00. heap -> scal );
         }
         void update_init_expr( const Expression & repl ) const
         {
            if( tvm::distinct( get<0> ( _xxxx -> repr. _fld00. heap -> scal ), repl ))
            {
               _xxxx -> repr. _fld00. heap = takeshare( replacebywritable( _xxxx -> repr. _fld00. heap ));
               get<0> ( _xxxx -> repr. _fld00. heap -> scal ) = repl;
            }
         }
         const Statement & type_info( ) const { return get<1> ( _xxxx -> repr. _fld00. heap -> scal ); }
         Statement extr_type_info( ) const {
            if( iswritable( _xxxx -> repr. _fld00. heap ))
               return std::move( get<1> ( _xxxx -> repr. _fld00. heap -> scal ) );
            else
               return get<1> ( _xxxx -> repr. _fld00. heap -> scal );
         }
         void update_type_info( const Statement & repl ) const
         {
            if( tvm::distinct( get<1> ( _xxxx -> repr. _fld00. heap -> scal ), repl ))
            {
               _xxxx -> repr. _fld00. heap = takeshare( replacebywritable( _xxxx -> repr. _fld00. heap ));
               get<1> ( _xxxx -> repr. _fld00. heap -> scal ) = repl;
            }
         }
         const Expression & alloc_size( ) const { return get<2> ( _xxxx -> repr. _fld00. heap -> scal ); }
         Expression extr_alloc_size( ) const {
            if( iswritable( _xxxx -> repr. _fld00. heap ))
               return std::move( get<2> ( _xxxx -> repr. _fld00. heap -> scal ) );
            else
               return get<2> ( _xxxx -> repr. _fld00. heap -> scal );
         }
         void update_alloc_size( const Expression & repl ) const
         {
            if( tvm::distinct( get<2> ( _xxxx -> repr. _fld00. heap -> scal ), repl ))
            {
               _xxxx -> repr. _fld00. heap = takeshare( replacebywritable( _xxxx -> repr. _fld00. heap ));
               get<2> ( _xxxx -> repr. _fld00. heap -> scal ) = repl;
            }
         }
      };

      mut_ArrayInit view_ArrayInit( )
      {
         if constexpr( check )
         {
            if( !option_is_ArrayInit( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_Assignment( ) const noexcept
      {
         switch( _ssss )
         {
         case AssignmentStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_Assignment
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_Assignment( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const Expression & expr( ) const { return _xxxx -> repr. _fld01. heap -> scal. first; }
         const Expression & dest( ) const { return _xxxx -> repr. _fld01. heap -> scal. second; }
      };

      const_Assignment view_Assignment( ) const
      {
         if constexpr( check )
         {
            if( !option_is_Assignment( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_Assignment
      {
         Statement* _xxxx;
         mut_Assignment( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         const Expression & expr( ) const { return _xxxx -> repr. _fld01. heap -> scal. first; }
         Expression extr_expr( ) const {
            if( iswritable( _xxxx -> repr. _fld01. heap ))
               return std::move( _xxxx -> repr. _fld01. heap -> scal. first );
            else
               return _xxxx -> repr. _fld01. heap -> scal. first;
         }
         void update_expr( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld01. heap -> scal. first, repl ))
            {
               _xxxx -> repr. _fld01. heap = takeshare( replacebywritable( _xxxx -> repr. _fld01. heap ));
               _xxxx -> repr. _fld01. heap -> scal. first = repl;
            }
         }
         const Expression & dest( ) const { return _xxxx -> repr. _fld01. heap -> scal. second; }
         Expression extr_dest( ) const {
            if( iswritable( _xxxx -> repr. _fld01. heap ))
               return std::move( _xxxx -> repr. _fld01. heap -> scal. second );
            else
               return _xxxx -> repr. _fld01. heap -> scal. second;
         }
         void update_dest( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld01. heap -> scal. second, repl ))
            {
               _xxxx -> repr. _fld01. heap = takeshare( replacebywritable( _xxxx -> repr. _fld01. heap ));
               _xxxx -> repr. _fld01. heap -> scal. second = repl;
            }
         }
      };

      mut_Assignment view_Assignment( )
      {
         if constexpr( check )
         {
            if( !option_is_Assignment( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_Block( ) const noexcept
      {
         switch( _ssss )
         {
         case BlockOfStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_Block
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_Block( const Statement* _xxxx ) : _xxxx( _xxxx ) { }
         size_t size( ) const { return _xxxx -> repr. _fld02. heap -> size( ); }
         const Statement & statements( size_t _iiii ) const
            { return _xxxx -> repr. _fld02. heap -> begin( ) [ _iiii ]; }
      };

      const_Block view_Block( ) const
      {
         if constexpr( check )
         {
            if( !option_is_Block( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_Block
      {
         Statement* _xxxx;
         mut_Block( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         size_t size( ) const { return _xxxx -> repr. _fld02. heap -> size( ); }
         void push_back( const Statement & xx00 ) const
         {
            _xxxx -> repr. _fld02. heap = tvm::push_back( _xxxx -> repr. _fld02. heap, xx00 );
         }
         void pop_back( ) const { _xxxx -> repr. _fld02. heap = tvm::pop_back( _xxxx -> repr. _fld02. heap ); }
         const Statement& statements( size_t _iiii ) const
            { return _xxxx -> repr. _fld02. heap -> begin( ) [ _iiii ]; }
         Statement extr_statements( size_t _iiii ) const
         {
            if( iswritable( _xxxx -> repr. _fld02. heap ))
               return std::move( _xxxx -> repr. _fld02. heap -> begin( ) [ _iiii ] );
            else
               return _xxxx -> repr. _fld02. heap -> begin( ) [ _iiii ];
         }
         void update_statements( size_t _iiii, const Statement & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld02. heap -> begin( ) [ _iiii ], repl ))
            {
               _xxxx -> repr. _fld02. heap = takeshare( replacebywritable( _xxxx -> repr. _fld02. heap ));
               _xxxx -> repr. _fld02. heap -> begin( ) [ _iiii ] = repl;
            }
         }
      };

      mut_Block view_Block( )
      {
         if constexpr( check )
         {
            if( !option_is_Block( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_Break( ) const noexcept
      {
         switch( _ssss )
         {
         case BreakStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_Break
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_Break( const Statement* _xxxx ) : _xxxx( _xxxx ) { }
      };

      const_Break view_Break( ) const
      {
         if constexpr( check )
         {
            if( !option_is_Break( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_Break
      {
         Statement* _xxxx;
         mut_Break( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }
      };

      mut_Break view_Break( )
      {
         if constexpr( check )
         {
            if( !option_is_Break( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_CaseClause( ) const noexcept
      {
         switch( _ssss )
         {
         case CaseClauseStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_CaseClause
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_CaseClause( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const bool & is_wildcard( ) const { return _xxxx -> repr. _fld04. loc; }

         const Statement & case_body( ) const { return _xxxx -> repr. _fld04. heap -> scal. first; }
         const Expression & case_expr( ) const { return _xxxx -> repr. _fld04. heap -> scal. second; }
      };

      const_CaseClause view_CaseClause( ) const
      {
         if constexpr( check )
         {
            if( !option_is_CaseClause( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_CaseClause
      {
         Statement* _xxxx;
         mut_CaseClause( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         bool & is_wildcard( ) const { return _xxxx -> repr. _fld04. loc; }
         bool extr_is_wildcard( ) const { return std::move( _xxxx -> repr. _fld04. loc ); }
         void update_is_wildcard( const bool & from ) const { _xxxx -> repr. _fld04. loc = from; }

         const Statement & case_body( ) const { return _xxxx -> repr. _fld04. heap -> scal. first; }
         Statement extr_case_body( ) const {
            if( iswritable( _xxxx -> repr. _fld04. heap ))
               return std::move( _xxxx -> repr. _fld04. heap -> scal. first );
            else
               return _xxxx -> repr. _fld04. heap -> scal. first;
         }
         void update_case_body( const Statement & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld04. heap -> scal. first, repl ))
            {
               _xxxx -> repr. _fld04. heap = takeshare( replacebywritable( _xxxx -> repr. _fld04. heap ));
               _xxxx -> repr. _fld04. heap -> scal. first = repl;
            }
         }
         const Expression & case_expr( ) const { return _xxxx -> repr. _fld04. heap -> scal. second; }
         Expression extr_case_expr( ) const {
            if( iswritable( _xxxx -> repr. _fld04. heap ))
               return std::move( _xxxx -> repr. _fld04. heap -> scal. second );
            else
               return _xxxx -> repr. _fld04. heap -> scal. second;
         }
         void update_case_expr( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld04. heap -> scal. second, repl ))
            {
               _xxxx -> repr. _fld04. heap = takeshare( replacebywritable( _xxxx -> repr. _fld04. heap ));
               _xxxx -> repr. _fld04. heap -> scal. second = repl;
            }
         }
      };

      mut_CaseClause view_CaseClause( )
      {
         if constexpr( check )
         {
            if( !option_is_CaseClause( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_Condition( ) const noexcept
      {
         switch( _ssss )
         {
         case ConditionStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_Condition
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_Condition( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const Expression & if_cond( ) const { return get<0> ( _xxxx -> repr. _fld05. heap -> scal ); }
         const Statement & if_body( ) const { return get<1> ( _xxxx -> repr. _fld05. heap -> scal ); }
         const Statement & else_body( ) const { return get<2> ( _xxxx -> repr. _fld05. heap -> scal ); }
         size_t size( ) const { return _xxxx -> repr. _fld05. heap -> size( ); }
         const Statement & elif_stmt( size_t _iiii ) const
            { return _xxxx -> repr. _fld05. heap -> begin( ) [ _iiii ]; }
      };

      const_Condition view_Condition( ) const
      {
         if constexpr( check )
         {
            if( !option_is_Condition( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_Condition
      {
         Statement* _xxxx;
         mut_Condition( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         const Expression & if_cond( ) const { return get<0> ( _xxxx -> repr. _fld05. heap -> scal ); }
         Expression extr_if_cond( ) const {
            if( iswritable( _xxxx -> repr. _fld05. heap ))
               return std::move( get<0> ( _xxxx -> repr. _fld05. heap -> scal ) );
            else
               return get<0> ( _xxxx -> repr. _fld05. heap -> scal );
         }
         void update_if_cond( const Expression & repl ) const
         {
            if( tvm::distinct( get<0> ( _xxxx -> repr. _fld05. heap -> scal ), repl ))
            {
               _xxxx -> repr. _fld05. heap = takeshare( replacebywritable( _xxxx -> repr. _fld05. heap ));
               get<0> ( _xxxx -> repr. _fld05. heap -> scal ) = repl;
            }
         }
         const Statement & if_body( ) const { return get<1> ( _xxxx -> repr. _fld05. heap -> scal ); }
         Statement extr_if_body( ) const {
            if( iswritable( _xxxx -> repr. _fld05. heap ))
               return std::move( get<1> ( _xxxx -> repr. _fld05. heap -> scal ) );
            else
               return get<1> ( _xxxx -> repr. _fld05. heap -> scal );
         }
         void update_if_body( const Statement & repl ) const
         {
            if( tvm::distinct( get<1> ( _xxxx -> repr. _fld05. heap -> scal ), repl ))
            {
               _xxxx -> repr. _fld05. heap = takeshare( replacebywritable( _xxxx -> repr. _fld05. heap ));
               get<1> ( _xxxx -> repr. _fld05. heap -> scal ) = repl;
            }
         }
         const Statement & else_body( ) const { return get<2> ( _xxxx -> repr. _fld05. heap -> scal ); }
         Statement extr_else_body( ) const {
            if( iswritable( _xxxx -> repr. _fld05. heap ))
               return std::move( get<2> ( _xxxx -> repr. _fld05. heap -> scal ) );
            else
               return get<2> ( _xxxx -> repr. _fld05. heap -> scal );
         }
         void update_else_body( const Statement & repl ) const
         {
            if( tvm::distinct( get<2> ( _xxxx -> repr. _fld05. heap -> scal ), repl ))
            {
               _xxxx -> repr. _fld05. heap = takeshare( replacebywritable( _xxxx -> repr. _fld05. heap ));
               get<2> ( _xxxx -> repr. _fld05. heap -> scal ) = repl;
            }
         }

         size_t size( ) const { return _xxxx -> repr. _fld05. heap -> size( ); }
         void push_back( const Statement & xx00 ) const
         {
            _xxxx -> repr. _fld05. heap = tvm::push_back( _xxxx -> repr. _fld05. heap, xx00 );
         }
         void pop_back( ) const { _xxxx -> repr. _fld05. heap = tvm::pop_back( _xxxx -> repr. _fld05. heap ); }
         const Statement& elif_stmt( size_t _iiii ) const
            { return _xxxx -> repr. _fld05. heap -> begin( ) [ _iiii ]; }
         Statement extr_elif_stmt( size_t _iiii ) const
         {
            if( iswritable( _xxxx -> repr. _fld05. heap ))
               return std::move( _xxxx -> repr. _fld05. heap -> begin( ) [ _iiii ] );
            else
               return _xxxx -> repr. _fld05. heap -> begin( ) [ _iiii ];
         }
         void update_elif_stmt( size_t _iiii, const Statement & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld05. heap -> begin( ) [ _iiii ], repl ))
            {
               _xxxx -> repr. _fld05. heap = takeshare( replacebywritable( _xxxx -> repr. _fld05. heap ));
               _xxxx -> repr. _fld05. heap -> begin( ) [ _iiii ] = repl;
            }
         }
      };

      mut_Condition view_Condition( )
      {
         if constexpr( check )
         {
            if( !option_is_Condition( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_Continue( ) const noexcept
      {
         switch( _ssss )
         {
         case ContinueStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_Continue
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_Continue( const Statement* _xxxx ) : _xxxx( _xxxx ) { }
      };

      const_Continue view_Continue( ) const
      {
         if constexpr( check )
         {
            if( !option_is_Continue( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_Continue
      {
         Statement* _xxxx;
         mut_Continue( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }
      };

      mut_Continue view_Continue( )
      {
         if constexpr( check )
         {
            if( !option_is_Continue( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_ElifCondition( ) const noexcept
      {
         switch( _ssss )
         {
         case ElifConditionStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_ElifCondition
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_ElifCondition( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const Expression & elif_cond( ) const { return _xxxx -> repr. _fld06. heap -> scal. first; }
         const Statement & elif_body( ) const { return _xxxx -> repr. _fld06. heap -> scal. second; }
      };

      const_ElifCondition view_ElifCondition( ) const
      {
         if constexpr( check )
         {
            if( !option_is_ElifCondition( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_ElifCondition
      {
         Statement* _xxxx;
         mut_ElifCondition( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         const Expression & elif_cond( ) const { return _xxxx -> repr. _fld06. heap -> scal. first; }
         Expression extr_elif_cond( ) const {
            if( iswritable( _xxxx -> repr. _fld06. heap ))
               return std::move( _xxxx -> repr. _fld06. heap -> scal. first );
            else
               return _xxxx -> repr. _fld06. heap -> scal. first;
         }
         void update_elif_cond( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld06. heap -> scal. first, repl ))
            {
               _xxxx -> repr. _fld06. heap = takeshare( replacebywritable( _xxxx -> repr. _fld06. heap ));
               _xxxx -> repr. _fld06. heap -> scal. first = repl;
            }
         }
         const Statement & elif_body( ) const { return _xxxx -> repr. _fld06. heap -> scal. second; }
         Statement extr_elif_body( ) const {
            if( iswritable( _xxxx -> repr. _fld06. heap ))
               return std::move( _xxxx -> repr. _fld06. heap -> scal. second );
            else
               return _xxxx -> repr. _fld06. heap -> scal. second;
         }
         void update_elif_body( const Statement & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld06. heap -> scal. second, repl ))
            {
               _xxxx -> repr. _fld06. heap = takeshare( replacebywritable( _xxxx -> repr. _fld06. heap ));
               _xxxx -> repr. _fld06. heap -> scal. second = repl;
            }
         }
      };

      mut_ElifCondition view_ElifCondition( )
      {
         if constexpr( check )
         {
            if( !option_is_ElifCondition( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_Empty( ) const noexcept
      {
         switch( _ssss )
         {
         case EmptyStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_Empty
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_Empty( const Statement* _xxxx ) : _xxxx( _xxxx ) { }
      };

      const_Empty view_Empty( ) const
      {
         if constexpr( check )
         {
            if( !option_is_Empty( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_Empty
      {
         Statement* _xxxx;
         mut_Empty( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }
      };

      mut_Empty view_Empty( )
      {
         if constexpr( check )
         {
            if( !option_is_Empty( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_ExprCall( ) const noexcept
      {
         switch( _ssss )
         {
         case ExprCallStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_ExprCall
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_ExprCall( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const Expression & expr( ) const { return _xxxx -> repr. _fld07. heap -> scal; }
      };

      const_ExprCall view_ExprCall( ) const
      {
         if constexpr( check )
         {
            if( !option_is_ExprCall( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_ExprCall
      {
         Statement* _xxxx;
         mut_ExprCall( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         const Expression & expr( ) const { return _xxxx -> repr. _fld07. heap -> scal; }
         Expression extr_expr( ) const {
            if( iswritable( _xxxx -> repr. _fld07. heap ))
               return std::move( _xxxx -> repr. _fld07. heap -> scal );
            else
               return _xxxx -> repr. _fld07. heap -> scal;
         }
         void update_expr( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld07. heap -> scal, repl ))
            {
               _xxxx -> repr. _fld07. heap = takeshare( replacebywritable( _xxxx -> repr. _fld07. heap ));
               _xxxx -> repr. _fld07. heap -> scal = repl;
            }
         }
      };

      mut_ExprCall view_ExprCall( )
      {
         if constexpr( check )
         {
            if( !option_is_ExprCall( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_FnArgs( ) const noexcept
      {
         switch( _ssss )
         {
         case FnArgsStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_FnArgs
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_FnArgs( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const std::string & type_name( ) const { return get<0> ( _xxxx -> repr. _fld08. loc ); }
         const bool & is_inout( ) const { return get<1> ( _xxxx -> repr. _fld08. loc ); }
         const std::string & var_name( ) const { return get<2> ( _xxxx -> repr. _fld08. loc ); }
         const bool & is_array( ) const { return get<3> ( _xxxx -> repr. _fld08. loc ); }

         const Expression & default_expr( ) const { return _xxxx -> repr. _fld08. heap -> scal; }
         size_t size( ) const { return _xxxx -> repr. _fld08. heap -> size( ); }
         const std::pair<bool, int> & array_info( size_t _iiii ) const
            { return _xxxx -> repr. _fld08. heap -> begin( ) [ _iiii ]; }
      };

      const_FnArgs view_FnArgs( ) const
      {
         if constexpr( check )
         {
            if( !option_is_FnArgs( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_FnArgs
      {
         Statement* _xxxx;
         mut_FnArgs( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         std::string & type_name( ) const { return get<0> ( _xxxx -> repr. _fld08. loc ); }
         std::string extr_type_name( ) const { return std::move( get<0> ( _xxxx -> repr. _fld08. loc ) ); }
         void update_type_name( const std::string & from ) const { get<0> ( _xxxx -> repr. _fld08. loc ) = from; }
         bool & is_inout( ) const { return get<1> ( _xxxx -> repr. _fld08. loc ); }
         bool extr_is_inout( ) const { return std::move( get<1> ( _xxxx -> repr. _fld08. loc ) ); }
         void update_is_inout( const bool & from ) const { get<1> ( _xxxx -> repr. _fld08. loc ) = from; }
         std::string & var_name( ) const { return get<2> ( _xxxx -> repr. _fld08. loc ); }
         std::string extr_var_name( ) const { return std::move( get<2> ( _xxxx -> repr. _fld08. loc ) ); }
         void update_var_name( const std::string & from ) const { get<2> ( _xxxx -> repr. _fld08. loc ) = from; }
         bool & is_array( ) const { return get<3> ( _xxxx -> repr. _fld08. loc ); }
         bool extr_is_array( ) const { return std::move( get<3> ( _xxxx -> repr. _fld08. loc ) ); }
         void update_is_array( const bool & from ) const { get<3> ( _xxxx -> repr. _fld08. loc ) = from; }

         const Expression & default_expr( ) const { return _xxxx -> repr. _fld08. heap -> scal; }
         Expression extr_default_expr( ) const {
            if( iswritable( _xxxx -> repr. _fld08. heap ))
               return std::move( _xxxx -> repr. _fld08. heap -> scal );
            else
               return _xxxx -> repr. _fld08. heap -> scal;
         }
         void update_default_expr( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld08. heap -> scal, repl ))
            {
               _xxxx -> repr. _fld08. heap = takeshare( replacebywritable( _xxxx -> repr. _fld08. heap ));
               _xxxx -> repr. _fld08. heap -> scal = repl;
            }
         }

         size_t size( ) const { return _xxxx -> repr. _fld08. heap -> size( ); }
         void push_back( const std::pair<bool, int> & xx00 ) const
         {
            _xxxx -> repr. _fld08. heap = tvm::push_back( _xxxx -> repr. _fld08. heap, xx00 );
         }
         void pop_back( ) const { _xxxx -> repr. _fld08. heap = tvm::pop_back( _xxxx -> repr. _fld08. heap ); }
         const std::pair<bool, int>& array_info( size_t _iiii ) const
            { return _xxxx -> repr. _fld08. heap -> begin( ) [ _iiii ]; }
         std::pair<bool, int> extr_array_info( size_t _iiii ) const
         {
            if( iswritable( _xxxx -> repr. _fld08. heap ))
               return std::move( _xxxx -> repr. _fld08. heap -> begin( ) [ _iiii ] );
            else
               return _xxxx -> repr. _fld08. heap -> begin( ) [ _iiii ];
         }
         void update_array_info( size_t _iiii, const std::pair<bool, int> & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld08. heap -> begin( ) [ _iiii ], repl ))
            {
               _xxxx -> repr. _fld08. heap = takeshare( replacebywritable( _xxxx -> repr. _fld08. heap ));
               _xxxx -> repr. _fld08. heap -> begin( ) [ _iiii ] = repl;
            }
         }
      };

      mut_FnArgs view_FnArgs( )
      {
         if constexpr( check )
         {
            if( !option_is_FnArgs( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_ForLoop( ) const noexcept
      {
         switch( _ssss )
         {
         case ForLoopStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_ForLoop
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_ForLoop( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const Statement & init_part( ) const { return get<0> ( _xxxx -> repr. _fld09. heap -> scal ); }
         const Expression & check( ) const { return get<1> ( _xxxx -> repr. _fld09. heap -> scal ); }
         const Statement & final_expr( ) const { return get<2> ( _xxxx -> repr. _fld09. heap -> scal ); }
         const Statement & forloop_body( ) const { return get<3> ( _xxxx -> repr. _fld09. heap -> scal ); }
      };

      const_ForLoop view_ForLoop( ) const
      {
         if constexpr( check )
         {
            if( !option_is_ForLoop( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_ForLoop
      {
         Statement* _xxxx;
         mut_ForLoop( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         const Statement & init_part( ) const { return get<0> ( _xxxx -> repr. _fld09. heap -> scal ); }
         Statement extr_init_part( ) const {
            if( iswritable( _xxxx -> repr. _fld09. heap ))
               return std::move( get<0> ( _xxxx -> repr. _fld09. heap -> scal ) );
            else
               return get<0> ( _xxxx -> repr. _fld09. heap -> scal );
         }
         void update_init_part( const Statement & repl ) const
         {
            if( tvm::distinct( get<0> ( _xxxx -> repr. _fld09. heap -> scal ), repl ))
            {
               _xxxx -> repr. _fld09. heap = takeshare( replacebywritable( _xxxx -> repr. _fld09. heap ));
               get<0> ( _xxxx -> repr. _fld09. heap -> scal ) = repl;
            }
         }
         const Expression & check( ) const { return get<1> ( _xxxx -> repr. _fld09. heap -> scal ); }
         Expression extr_check( ) const {
            if( iswritable( _xxxx -> repr. _fld09. heap ))
               return std::move( get<1> ( _xxxx -> repr. _fld09. heap -> scal ) );
            else
               return get<1> ( _xxxx -> repr. _fld09. heap -> scal );
         }
         void update_check( const Expression & repl ) const
         {
            if( tvm::distinct( get<1> ( _xxxx -> repr. _fld09. heap -> scal ), repl ))
            {
               _xxxx -> repr. _fld09. heap = takeshare( replacebywritable( _xxxx -> repr. _fld09. heap ));
               get<1> ( _xxxx -> repr. _fld09. heap -> scal ) = repl;
            }
         }
         const Statement & final_expr( ) const { return get<2> ( _xxxx -> repr. _fld09. heap -> scal ); }
         Statement extr_final_expr( ) const {
            if( iswritable( _xxxx -> repr. _fld09. heap ))
               return std::move( get<2> ( _xxxx -> repr. _fld09. heap -> scal ) );
            else
               return get<2> ( _xxxx -> repr. _fld09. heap -> scal );
         }
         void update_final_expr( const Statement & repl ) const
         {
            if( tvm::distinct( get<2> ( _xxxx -> repr. _fld09. heap -> scal ), repl ))
            {
               _xxxx -> repr. _fld09. heap = takeshare( replacebywritable( _xxxx -> repr. _fld09. heap ));
               get<2> ( _xxxx -> repr. _fld09. heap -> scal ) = repl;
            }
         }
         const Statement & forloop_body( ) const { return get<3> ( _xxxx -> repr. _fld09. heap -> scal ); }
         Statement extr_forloop_body( ) const {
            if( iswritable( _xxxx -> repr. _fld09. heap ))
               return std::move( get<3> ( _xxxx -> repr. _fld09. heap -> scal ) );
            else
               return get<3> ( _xxxx -> repr. _fld09. heap -> scal );
         }
         void update_forloop_body( const Statement & repl ) const
         {
            if( tvm::distinct( get<3> ( _xxxx -> repr. _fld09. heap -> scal ), repl ))
            {
               _xxxx -> repr. _fld09. heap = takeshare( replacebywritable( _xxxx -> repr. _fld09. heap ));
               get<3> ( _xxxx -> repr. _fld09. heap -> scal ) = repl;
            }
         }
      };

      mut_ForLoop view_ForLoop( )
      {
         if constexpr( check )
         {
            if( !option_is_ForLoop( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_MakeEnum( ) const noexcept
      {
         switch( _ssss )
         {
         case MakeEnumStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_MakeEnum
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_MakeEnum( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const std::string & enum_name( ) const { return _xxxx -> repr. _fld10. loc; }
         size_t size( ) const { return _xxxx -> repr. _fld10. heap -> size( ); }
         const std::string & enum_variants( size_t _iiii ) const
            { return _xxxx -> repr. _fld10. heap -> begin( ) [ _iiii ]; }
      };

      const_MakeEnum view_MakeEnum( ) const
      {
         if constexpr( check )
         {
            if( !option_is_MakeEnum( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_MakeEnum
      {
         Statement* _xxxx;
         mut_MakeEnum( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         std::string & enum_name( ) const { return _xxxx -> repr. _fld10. loc; }
         std::string extr_enum_name( ) const { return std::move( _xxxx -> repr. _fld10. loc ); }
         void update_enum_name( const std::string & from ) const { _xxxx -> repr. _fld10. loc = from; }

         size_t size( ) const { return _xxxx -> repr. _fld10. heap -> size( ); }
         void push_back( const std::string & xx00 ) const
         {
            _xxxx -> repr. _fld10. heap = tvm::push_back( _xxxx -> repr. _fld10. heap, xx00 );
         }
         void pop_back( ) const { _xxxx -> repr. _fld10. heap = tvm::pop_back( _xxxx -> repr. _fld10. heap ); }
         const std::string& enum_variants( size_t _iiii ) const
            { return _xxxx -> repr. _fld10. heap -> begin( ) [ _iiii ]; }
         std::string extr_enum_variants( size_t _iiii ) const
         {
            if( iswritable( _xxxx -> repr. _fld10. heap ))
               return std::move( _xxxx -> repr. _fld10. heap -> begin( ) [ _iiii ] );
            else
               return _xxxx -> repr. _fld10. heap -> begin( ) [ _iiii ];
         }
         void update_enum_variants( size_t _iiii, const std::string & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld10. heap -> begin( ) [ _iiii ], repl ))
            {
               _xxxx -> repr. _fld10. heap = takeshare( replacebywritable( _xxxx -> repr. _fld10. heap ));
               _xxxx -> repr. _fld10. heap -> begin( ) [ _iiii ] = repl;
            }
         }
      };

      mut_MakeEnum view_MakeEnum( )
      {
         if constexpr( check )
         {
            if( !option_is_MakeEnum( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_MakeFunction( ) const noexcept
      {
         switch( _ssss )
         {
         case MakeFunctionStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_MakeFunction
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_MakeFunction( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const std::string & fn_name( ) const { return _xxxx -> repr. _fld11. loc. first; }
         const std::string & ret_type_name( ) const { return _xxxx -> repr. _fld11. loc. second; }

         const Statement & fn_body( ) const { return _xxxx -> repr. _fld11. heap -> scal; }
         size_t size( ) const { return _xxxx -> repr. _fld11. heap -> size( ); }
         const Statement & params( size_t _iiii ) const
            { return _xxxx -> repr. _fld11. heap -> begin( ) [ _iiii ]; }
      };

      const_MakeFunction view_MakeFunction( ) const
      {
         if constexpr( check )
         {
            if( !option_is_MakeFunction( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_MakeFunction
      {
         Statement* _xxxx;
         mut_MakeFunction( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         std::string & fn_name( ) const { return _xxxx -> repr. _fld11. loc. first; }
         std::string extr_fn_name( ) const { return std::move( _xxxx -> repr. _fld11. loc. first ); }
         void update_fn_name( const std::string & from ) const { _xxxx -> repr. _fld11. loc. first = from; }
         std::string & ret_type_name( ) const { return _xxxx -> repr. _fld11. loc. second; }
         std::string extr_ret_type_name( ) const { return std::move( _xxxx -> repr. _fld11. loc. second ); }
         void update_ret_type_name( const std::string & from ) const { _xxxx -> repr. _fld11. loc. second = from; }

         const Statement & fn_body( ) const { return _xxxx -> repr. _fld11. heap -> scal; }
         Statement extr_fn_body( ) const {
            if( iswritable( _xxxx -> repr. _fld11. heap ))
               return std::move( _xxxx -> repr. _fld11. heap -> scal );
            else
               return _xxxx -> repr. _fld11. heap -> scal;
         }
         void update_fn_body( const Statement & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld11. heap -> scal, repl ))
            {
               _xxxx -> repr. _fld11. heap = takeshare( replacebywritable( _xxxx -> repr. _fld11. heap ));
               _xxxx -> repr. _fld11. heap -> scal = repl;
            }
         }

         size_t size( ) const { return _xxxx -> repr. _fld11. heap -> size( ); }
         void push_back( const Statement & xx00 ) const
         {
            _xxxx -> repr. _fld11. heap = tvm::push_back( _xxxx -> repr. _fld11. heap, xx00 );
         }
         void pop_back( ) const { _xxxx -> repr. _fld11. heap = tvm::pop_back( _xxxx -> repr. _fld11. heap ); }
         const Statement& params( size_t _iiii ) const
            { return _xxxx -> repr. _fld11. heap -> begin( ) [ _iiii ]; }
         Statement extr_params( size_t _iiii ) const
         {
            if( iswritable( _xxxx -> repr. _fld11. heap ))
               return std::move( _xxxx -> repr. _fld11. heap -> begin( ) [ _iiii ] );
            else
               return _xxxx -> repr. _fld11. heap -> begin( ) [ _iiii ];
         }
         void update_params( size_t _iiii, const Statement & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld11. heap -> begin( ) [ _iiii ], repl ))
            {
               _xxxx -> repr. _fld11. heap = takeshare( replacebywritable( _xxxx -> repr. _fld11. heap ));
               _xxxx -> repr. _fld11. heap -> begin( ) [ _iiii ] = repl;
            }
         }
      };

      mut_MakeFunction view_MakeFunction( )
      {
         if constexpr( check )
         {
            if( !option_is_MakeFunction( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_MakeProperty( ) const noexcept
      {
         switch( _ssss )
         {
         case MakePropertyStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_MakeProperty
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_MakeProperty( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const std::string & prop_name( ) const { return _xxxx -> repr. _fld12. loc; }
         size_t size( ) const { return _xxxx -> repr. _fld12. heap -> size( ); }
         const Statement & opts( size_t _iiii ) const
            { return _xxxx -> repr. _fld12. heap -> begin( ) [ _iiii ]; }
      };

      const_MakeProperty view_MakeProperty( ) const
      {
         if constexpr( check )
         {
            if( !option_is_MakeProperty( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_MakeProperty
      {
         Statement* _xxxx;
         mut_MakeProperty( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         std::string & prop_name( ) const { return _xxxx -> repr. _fld12. loc; }
         std::string extr_prop_name( ) const { return std::move( _xxxx -> repr. _fld12. loc ); }
         void update_prop_name( const std::string & from ) const { _xxxx -> repr. _fld12. loc = from; }

         size_t size( ) const { return _xxxx -> repr. _fld12. heap -> size( ); }
         void push_back( const Statement & xx00 ) const
         {
            _xxxx -> repr. _fld12. heap = tvm::push_back( _xxxx -> repr. _fld12. heap, xx00 );
         }
         void pop_back( ) const { _xxxx -> repr. _fld12. heap = tvm::pop_back( _xxxx -> repr. _fld12. heap ); }
         const Statement& opts( size_t _iiii ) const
            { return _xxxx -> repr. _fld12. heap -> begin( ) [ _iiii ]; }
         Statement extr_opts( size_t _iiii ) const
         {
            if( iswritable( _xxxx -> repr. _fld12. heap ))
               return std::move( _xxxx -> repr. _fld12. heap -> begin( ) [ _iiii ] );
            else
               return _xxxx -> repr. _fld12. heap -> begin( ) [ _iiii ];
         }
         void update_opts( size_t _iiii, const Statement & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld12. heap -> begin( ) [ _iiii ], repl ))
            {
               _xxxx -> repr. _fld12. heap = takeshare( replacebywritable( _xxxx -> repr. _fld12. heap ));
               _xxxx -> repr. _fld12. heap -> begin( ) [ _iiii ] = repl;
            }
         }
      };

      mut_MakeProperty view_MakeProperty( )
      {
         if constexpr( check )
         {
            if( !option_is_MakeProperty( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_MakeStruct( ) const noexcept
      {
         switch( _ssss )
         {
         case MakeStructStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_MakeStruct
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_MakeStruct( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const std::string & struct_name( ) const { return _xxxx -> repr. _fld12. loc; }
         size_t size( ) const { return _xxxx -> repr. _fld12. heap -> size( ); }
         const Statement & inits( size_t _iiii ) const
            { return _xxxx -> repr. _fld12. heap -> begin( ) [ _iiii ]; }
      };

      const_MakeStruct view_MakeStruct( ) const
      {
         if constexpr( check )
         {
            if( !option_is_MakeStruct( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_MakeStruct
      {
         Statement* _xxxx;
         mut_MakeStruct( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         std::string & struct_name( ) const { return _xxxx -> repr. _fld12. loc; }
         std::string extr_struct_name( ) const { return std::move( _xxxx -> repr. _fld12. loc ); }
         void update_struct_name( const std::string & from ) const { _xxxx -> repr. _fld12. loc = from; }

         size_t size( ) const { return _xxxx -> repr. _fld12. heap -> size( ); }
         void push_back( const Statement & xx00 ) const
         {
            _xxxx -> repr. _fld12. heap = tvm::push_back( _xxxx -> repr. _fld12. heap, xx00 );
         }
         void pop_back( ) const { _xxxx -> repr. _fld12. heap = tvm::pop_back( _xxxx -> repr. _fld12. heap ); }
         const Statement& inits( size_t _iiii ) const
            { return _xxxx -> repr. _fld12. heap -> begin( ) [ _iiii ]; }
         Statement extr_inits( size_t _iiii ) const
         {
            if( iswritable( _xxxx -> repr. _fld12. heap ))
               return std::move( _xxxx -> repr. _fld12. heap -> begin( ) [ _iiii ] );
            else
               return _xxxx -> repr. _fld12. heap -> begin( ) [ _iiii ];
         }
         void update_inits( size_t _iiii, const Statement & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld12. heap -> begin( ) [ _iiii ], repl ))
            {
               _xxxx -> repr. _fld12. heap = takeshare( replacebywritable( _xxxx -> repr. _fld12. heap ));
               _xxxx -> repr. _fld12. heap -> begin( ) [ _iiii ] = repl;
            }
         }
      };

      mut_MakeStruct view_MakeStruct( )
      {
         if constexpr( check )
         {
            if( !option_is_MakeStruct( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_Match( ) const noexcept
      {
         switch( _ssss )
         {
         case MatchStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_Match
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_Match( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const Expression & matched_expr( ) const { return _xxxx -> repr. _fld13. heap -> scal; }
         size_t size( ) const { return _xxxx -> repr. _fld13. heap -> size( ); }
         const Statement & cases( size_t _iiii ) const
            { return _xxxx -> repr. _fld13. heap -> begin( ) [ _iiii ]; }
      };

      const_Match view_Match( ) const
      {
         if constexpr( check )
         {
            if( !option_is_Match( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_Match
      {
         Statement* _xxxx;
         mut_Match( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         const Expression & matched_expr( ) const { return _xxxx -> repr. _fld13. heap -> scal; }
         Expression extr_matched_expr( ) const {
            if( iswritable( _xxxx -> repr. _fld13. heap ))
               return std::move( _xxxx -> repr. _fld13. heap -> scal );
            else
               return _xxxx -> repr. _fld13. heap -> scal;
         }
         void update_matched_expr( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld13. heap -> scal, repl ))
            {
               _xxxx -> repr. _fld13. heap = takeshare( replacebywritable( _xxxx -> repr. _fld13. heap ));
               _xxxx -> repr. _fld13. heap -> scal = repl;
            }
         }

         size_t size( ) const { return _xxxx -> repr. _fld13. heap -> size( ); }
         void push_back( const Statement & xx00 ) const
         {
            _xxxx -> repr. _fld13. heap = tvm::push_back( _xxxx -> repr. _fld13. heap, xx00 );
         }
         void pop_back( ) const { _xxxx -> repr. _fld13. heap = tvm::pop_back( _xxxx -> repr. _fld13. heap ); }
         const Statement& cases( size_t _iiii ) const
            { return _xxxx -> repr. _fld13. heap -> begin( ) [ _iiii ]; }
         Statement extr_cases( size_t _iiii ) const
         {
            if( iswritable( _xxxx -> repr. _fld13. heap ))
               return std::move( _xxxx -> repr. _fld13. heap -> begin( ) [ _iiii ] );
            else
               return _xxxx -> repr. _fld13. heap -> begin( ) [ _iiii ];
         }
         void update_cases( size_t _iiii, const Statement & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld13. heap -> begin( ) [ _iiii ], repl ))
            {
               _xxxx -> repr. _fld13. heap = takeshare( replacebywritable( _xxxx -> repr. _fld13. heap ));
               _xxxx -> repr. _fld13. heap -> begin( ) [ _iiii ] = repl;
            }
         }
      };

      mut_Match view_Match( )
      {
         if constexpr( check )
         {
            if( !option_is_Match( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_Return( ) const noexcept
      {
         switch( _ssss )
         {
         case ReturnStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_Return
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_Return( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const Expression & return_expr( ) const { return _xxxx -> repr. _fld07. heap -> scal; }
      };

      const_Return view_Return( ) const
      {
         if constexpr( check )
         {
            if( !option_is_Return( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_Return
      {
         Statement* _xxxx;
         mut_Return( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         const Expression & return_expr( ) const { return _xxxx -> repr. _fld07. heap -> scal; }
         Expression extr_return_expr( ) const {
            if( iswritable( _xxxx -> repr. _fld07. heap ))
               return std::move( _xxxx -> repr. _fld07. heap -> scal );
            else
               return _xxxx -> repr. _fld07. heap -> scal;
         }
         void update_return_expr( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld07. heap -> scal, repl ))
            {
               _xxxx -> repr. _fld07. heap = takeshare( replacebywritable( _xxxx -> repr. _fld07. heap ));
               _xxxx -> repr. _fld07. heap -> scal = repl;
            }
         }
      };

      mut_Return view_Return( )
      {
         if constexpr( check )
         {
            if( !option_is_Return( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_VarInit( ) const noexcept
      {
         switch( _ssss )
         {
         case VarInitStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_VarInit
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_VarInit( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const std::string & var_name( ) const { return _xxxx -> repr. _fld14. loc. first; }
         const std::string & type_name( ) const { return _xxxx -> repr. _fld14. loc. second; }

         const Expression & init_expr( ) const { return _xxxx -> repr. _fld14. heap -> scal; }
      };

      const_VarInit view_VarInit( ) const
      {
         if constexpr( check )
         {
            if( !option_is_VarInit( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_VarInit
      {
         Statement* _xxxx;
         mut_VarInit( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         std::string & var_name( ) const { return _xxxx -> repr. _fld14. loc. first; }
         std::string extr_var_name( ) const { return std::move( _xxxx -> repr. _fld14. loc. first ); }
         void update_var_name( const std::string & from ) const { _xxxx -> repr. _fld14. loc. first = from; }
         std::string & type_name( ) const { return _xxxx -> repr. _fld14. loc. second; }
         std::string extr_type_name( ) const { return std::move( _xxxx -> repr. _fld14. loc. second ); }
         void update_type_name( const std::string & from ) const { _xxxx -> repr. _fld14. loc. second = from; }

         const Expression & init_expr( ) const { return _xxxx -> repr. _fld14. heap -> scal; }
         Expression extr_init_expr( ) const {
            if( iswritable( _xxxx -> repr. _fld14. heap ))
               return std::move( _xxxx -> repr. _fld14. heap -> scal );
            else
               return _xxxx -> repr. _fld14. heap -> scal;
         }
         void update_init_expr( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld14. heap -> scal, repl ))
            {
               _xxxx -> repr. _fld14. heap = takeshare( replacebywritable( _xxxx -> repr. _fld14. heap ));
               _xxxx -> repr. _fld14. heap -> scal = repl;
            }
         }
      };

      mut_VarInit view_VarInit( )
      {
         if constexpr( check )
         {
            if( !option_is_VarInit( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_WhileLoop( ) const noexcept
      {
         switch( _ssss )
         {
         case WhileLoopStmt:
            return true;
         default:
            return false;
         }
      }

      struct const_WhileLoop
      {
         const Statement* _xxxx;
         const Statement & operator * ( ) const { return * _xxxx; }
         const_WhileLoop( const Statement* _xxxx ) : _xxxx( _xxxx ) { }

         const Expression & cond( ) const { return _xxxx -> repr. _fld06. heap -> scal. first; }
         const Statement & whileloop_body( ) const { return _xxxx -> repr. _fld06. heap -> scal. second; }
      };

      const_WhileLoop view_WhileLoop( ) const
      {
         if constexpr( check )
         {
            if( !option_is_WhileLoop( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_WhileLoop
      {
         Statement* _xxxx;
         mut_WhileLoop( Statement* _xxxx ) : _xxxx( _xxxx ) { }
         const Statement & operator * ( ) const { return * _xxxx; }

         const Expression & cond( ) const { return _xxxx -> repr. _fld06. heap -> scal. first; }
         Expression extr_cond( ) const {
            if( iswritable( _xxxx -> repr. _fld06. heap ))
               return std::move( _xxxx -> repr. _fld06. heap -> scal. first );
            else
               return _xxxx -> repr. _fld06. heap -> scal. first;
         }
         void update_cond( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld06. heap -> scal. first, repl ))
            {
               _xxxx -> repr. _fld06. heap = takeshare( replacebywritable( _xxxx -> repr. _fld06. heap ));
               _xxxx -> repr. _fld06. heap -> scal. first = repl;
            }
         }
         const Statement & whileloop_body( ) const { return _xxxx -> repr. _fld06. heap -> scal. second; }
         Statement extr_whileloop_body( ) const {
            if( iswritable( _xxxx -> repr. _fld06. heap ))
               return std::move( _xxxx -> repr. _fld06. heap -> scal. second );
            else
               return _xxxx -> repr. _fld06. heap -> scal. second;
         }
         void update_whileloop_body( const Statement & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld06. heap -> scal. second, repl ))
            {
               _xxxx -> repr. _fld06. heap = takeshare( replacebywritable( _xxxx -> repr. _fld06. heap ));
               _xxxx -> repr. _fld06. heap -> scal. second = repl;
            }
         }
      };

      mut_WhileLoop view_WhileLoop( )
      {
         if constexpr( check )
         {
            if( !option_is_WhileLoop( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
   };
}

#endif

