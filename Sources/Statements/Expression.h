
// This code was automatically built by TreeGen
// Written by Hans de Nivelle, see www.compiler-tools.eu

#ifndef VAL_EXPRESSION_
#define VAL_EXPRESSION_

#include <iostream>
#include <utility>
#include <tuple>
#include <initializer_list>
#include "selector.h"

#include "tvm/includes.h"

namespace val { 


   class Expression
   {
   private:
      selector _ssss;

      using _loc00 = std::pair<std::string,int>;
      using _scal00 = tvm::unit;
      using _rep00 = tvm::unit;
      using _loc01 = std::string;
      using _scal01 = std::pair<Expression,Expression>;
      using _rep01 = tvm::unit;
      using _loc02 = bool;
      using _scal02 = tvm::unit;
      using _rep02 = tvm::unit;
      using _loc03 = char;
      using _scal03 = tvm::unit;
      using _rep03 = tvm::unit;
      using _loc04 = double;
      using _scal04 = tvm::unit;
      using _rep04 = tvm::unit;
      using _loc05 = tvm::unit;
      using _scal05 = tvm::unit;
      using _rep05 = tvm::unit;
      using _loc06 = tvm::unit;
      using _scal06 = std::pair<Expression,Expression>;
      using _rep06 = tvm::unit;
      using _loc07 = std::string;
      using _scal07 = tvm::unit;
      using _rep07 = Expression;
      using _loc08 = int;
      using _scal08 = tvm::unit;
      using _rep08 = tvm::unit;
      using _loc09 = std::string;
      using _scal09 = tvm::unit;
      using _rep09 = tvm::unit;
      using _loc10 = std::string;
      using _scal10 = Expression;
      using _rep10 = tvm::unit;

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

         options( ) : _fld05( ) { }
         ~options( ) noexcept { }
      };

   private:
      options repr;

   public:
      Expression( ) = delete;
      Expression( const Expression& );
      Expression( Expression&& ) noexcept;
      const Expression& operator = ( const Expression& );
      const Expression& operator = ( Expression&& ) noexcept;
      ~Expression( ) noexcept;
      
      selector sel( ) const { return _ssss; }
      bool very_equal_to( const Expression& ) const;
      void printstate( std::ostream& out ) const;
      
      Expression( selector sel, const std::string & _xx00, const int & _xx01 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case ArrayIndex:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld00. loc. first, _xx00 );
         tvm::init( repr. _fld00. loc. second, _xx01 );
      }

      Expression( selector sel, const Expression & _xx00, const Expression & _xx01, const std::string & _xx02 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case BinaryExpr:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld01. loc, _xx02 );
         repr. _fld01. heap = takeshare( tvm::constr_scalar< _scal01 > ( std::pair( _xx00, _xx01 ) ));
      }

      Expression( selector sel, const bool & _xx00 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case BoolLiteralExpr:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld02. loc, _xx00 );
      }

      Expression( selector sel, const char & _xx00 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case CharLiteralExpr:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld03. loc, _xx00 );
      }

      Expression( selector sel, const double & _xx00 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case DoubleLiteralExpr:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld04. loc, _xx00 );
      }

      Expression( selector sel )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case EmptyExpr:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
      }

      Expression( selector sel, const Expression & _xx00, const Expression & _xx01 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case FieldCallExpr:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         repr. _fld06. heap = takeshare( tvm::constr_scalar< _scal06 > ( std::pair( _xx00, _xx01 ) ));
      }

      template< tvm::const_iterator< _rep07 > It >
      Expression( selector sel, It begin, It end, const std::string & _xx02 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case FnCallExpr:
            case StructInitExpr:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld07. loc, _xx02 );
         repr. _fld07. heap = takeshare( tvm::constr_scalar_repeated< _scal07, _rep07 > ( tvm::unit( ), begin, end ));
      }

      Expression( selector sel, std::initializer_list< _rep07 > repeated, const std::string & _xx01 )
         : Expression( sel, repeated. begin( ), repeated. end( ), _xx01 )
      { }

      Expression( selector sel, const int & _xx00 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case IntLiteralExpr:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld08. loc, _xx00 );
      }

      Expression( selector sel, const std::string & _xx00 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case StringLiteralExpr:
            case VarNameExpr:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld09. loc, _xx00 );
      }

      Expression( selector sel, const Expression & _xx00, const std::string & _xx01 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case UnaryExpr:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         tvm::init( repr. _fld10. loc, _xx01 );
         repr. _fld10. heap = takeshare( tvm::constr_scalar< _scal10 > ( _xx00 ));
      }

      bool option_is_ArrayIndex( ) const noexcept
      {
         switch( _ssss )
         {
         case ArrayIndex:
            return true;
         default:
            return false;
         }
      }

      struct const_ArrayIndex
      {
         const Expression* _xxxx;
         const Expression & operator * ( ) const { return * _xxxx; }
         const_ArrayIndex( const Expression* _xxxx ) : _xxxx( _xxxx ) { }

         const std::string & var_name( ) const { return _xxxx -> repr. _fld00. loc. first; }
         const int & at( ) const { return _xxxx -> repr. _fld00. loc. second; }
      };

      const_ArrayIndex view_ArrayIndex( ) const
      {
         if constexpr( check )
         {
            if( !option_is_ArrayIndex( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_ArrayIndex
      {
         Expression* _xxxx;
         mut_ArrayIndex( Expression* _xxxx ) : _xxxx( _xxxx ) { }
         const Expression & operator * ( ) const { return * _xxxx; }

         std::string & var_name( ) const { return _xxxx -> repr. _fld00. loc. first; }
         std::string extr_var_name( ) const { return std::move( _xxxx -> repr. _fld00. loc. first ); }
         void update_var_name( const std::string & from ) const { _xxxx -> repr. _fld00. loc. first = from; }
         int & at( ) const { return _xxxx -> repr. _fld00. loc. second; }
         int extr_at( ) const { return std::move( _xxxx -> repr. _fld00. loc. second ); }
         void update_at( const int & from ) const { _xxxx -> repr. _fld00. loc. second = from; }
      };

      mut_ArrayIndex view_ArrayIndex( )
      {
         if constexpr( check )
         {
            if( !option_is_ArrayIndex( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_Binary( ) const noexcept
      {
         switch( _ssss )
         {
         case BinaryExpr:
            return true;
         default:
            return false;
         }
      }

      struct const_Binary
      {
         const Expression* _xxxx;
         const Expression & operator * ( ) const { return * _xxxx; }
         const_Binary( const Expression* _xxxx ) : _xxxx( _xxxx ) { }

         const std::string & op( ) const { return _xxxx -> repr. _fld01. loc; }

         const Expression & lhs( ) const { return _xxxx -> repr. _fld01. heap -> scal. first; }
         const Expression & rhs( ) const { return _xxxx -> repr. _fld01. heap -> scal. second; }
      };

      const_Binary view_Binary( ) const
      {
         if constexpr( check )
         {
            if( !option_is_Binary( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_Binary
      {
         Expression* _xxxx;
         mut_Binary( Expression* _xxxx ) : _xxxx( _xxxx ) { }
         const Expression & operator * ( ) const { return * _xxxx; }

         std::string & op( ) const { return _xxxx -> repr. _fld01. loc; }
         std::string extr_op( ) const { return std::move( _xxxx -> repr. _fld01. loc ); }
         void update_op( const std::string & from ) const { _xxxx -> repr. _fld01. loc = from; }

         const Expression & lhs( ) const { return _xxxx -> repr. _fld01. heap -> scal. first; }
         Expression extr_lhs( ) const {
            if( iswritable( _xxxx -> repr. _fld01. heap ))
               return std::move( _xxxx -> repr. _fld01. heap -> scal. first );
            else
               return _xxxx -> repr. _fld01. heap -> scal. first;
         }
         void update_lhs( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld01. heap -> scal. first, repl ))
            {
               _xxxx -> repr. _fld01. heap = takeshare( replacebywritable( _xxxx -> repr. _fld01. heap ));
               _xxxx -> repr. _fld01. heap -> scal. first = repl;
            }
         }
         const Expression & rhs( ) const { return _xxxx -> repr. _fld01. heap -> scal. second; }
         Expression extr_rhs( ) const {
            if( iswritable( _xxxx -> repr. _fld01. heap ))
               return std::move( _xxxx -> repr. _fld01. heap -> scal. second );
            else
               return _xxxx -> repr. _fld01. heap -> scal. second;
         }
         void update_rhs( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld01. heap -> scal. second, repl ))
            {
               _xxxx -> repr. _fld01. heap = takeshare( replacebywritable( _xxxx -> repr. _fld01. heap ));
               _xxxx -> repr. _fld01. heap -> scal. second = repl;
            }
         }
      };

      mut_Binary view_Binary( )
      {
         if constexpr( check )
         {
            if( !option_is_Binary( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_BoolLiteral( ) const noexcept
      {
         switch( _ssss )
         {
         case BoolLiteralExpr:
            return true;
         default:
            return false;
         }
      }

      struct const_BoolLiteral
      {
         const Expression* _xxxx;
         const Expression & operator * ( ) const { return * _xxxx; }
         const_BoolLiteral( const Expression* _xxxx ) : _xxxx( _xxxx ) { }

         const bool & value( ) const { return _xxxx -> repr. _fld02. loc; }
      };

      const_BoolLiteral view_BoolLiteral( ) const
      {
         if constexpr( check )
         {
            if( !option_is_BoolLiteral( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_BoolLiteral
      {
         Expression* _xxxx;
         mut_BoolLiteral( Expression* _xxxx ) : _xxxx( _xxxx ) { }
         const Expression & operator * ( ) const { return * _xxxx; }

         bool & value( ) const { return _xxxx -> repr. _fld02. loc; }
         bool extr_value( ) const { return std::move( _xxxx -> repr. _fld02. loc ); }
         void update_value( const bool & from ) const { _xxxx -> repr. _fld02. loc = from; }
      };

      mut_BoolLiteral view_BoolLiteral( )
      {
         if constexpr( check )
         {
            if( !option_is_BoolLiteral( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_CharLiteral( ) const noexcept
      {
         switch( _ssss )
         {
         case CharLiteralExpr:
            return true;
         default:
            return false;
         }
      }

      struct const_CharLiteral
      {
         const Expression* _xxxx;
         const Expression & operator * ( ) const { return * _xxxx; }
         const_CharLiteral( const Expression* _xxxx ) : _xxxx( _xxxx ) { }

         const char & value( ) const { return _xxxx -> repr. _fld03. loc; }
      };

      const_CharLiteral view_CharLiteral( ) const
      {
         if constexpr( check )
         {
            if( !option_is_CharLiteral( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_CharLiteral
      {
         Expression* _xxxx;
         mut_CharLiteral( Expression* _xxxx ) : _xxxx( _xxxx ) { }
         const Expression & operator * ( ) const { return * _xxxx; }

         char & value( ) const { return _xxxx -> repr. _fld03. loc; }
         char extr_value( ) const { return std::move( _xxxx -> repr. _fld03. loc ); }
         void update_value( const char & from ) const { _xxxx -> repr. _fld03. loc = from; }
      };

      mut_CharLiteral view_CharLiteral( )
      {
         if constexpr( check )
         {
            if( !option_is_CharLiteral( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_DoubleLiteral( ) const noexcept
      {
         switch( _ssss )
         {
         case DoubleLiteralExpr:
            return true;
         default:
            return false;
         }
      }

      struct const_DoubleLiteral
      {
         const Expression* _xxxx;
         const Expression & operator * ( ) const { return * _xxxx; }
         const_DoubleLiteral( const Expression* _xxxx ) : _xxxx( _xxxx ) { }

         const double & value( ) const { return _xxxx -> repr. _fld04. loc; }
      };

      const_DoubleLiteral view_DoubleLiteral( ) const
      {
         if constexpr( check )
         {
            if( !option_is_DoubleLiteral( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_DoubleLiteral
      {
         Expression* _xxxx;
         mut_DoubleLiteral( Expression* _xxxx ) : _xxxx( _xxxx ) { }
         const Expression & operator * ( ) const { return * _xxxx; }

         double & value( ) const { return _xxxx -> repr. _fld04. loc; }
         double extr_value( ) const { return std::move( _xxxx -> repr. _fld04. loc ); }
         void update_value( const double & from ) const { _xxxx -> repr. _fld04. loc = from; }
      };

      mut_DoubleLiteral view_DoubleLiteral( )
      {
         if constexpr( check )
         {
            if( !option_is_DoubleLiteral( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_EmptyLiteral( ) const noexcept
      {
         switch( _ssss )
         {
         case EmptyExpr:
            return true;
         default:
            return false;
         }
      }

      struct const_EmptyLiteral
      {
         const Expression* _xxxx;
         const Expression & operator * ( ) const { return * _xxxx; }
         const_EmptyLiteral( const Expression* _xxxx ) : _xxxx( _xxxx ) { }
      };

      const_EmptyLiteral view_EmptyLiteral( ) const
      {
         if constexpr( check )
         {
            if( !option_is_EmptyLiteral( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_EmptyLiteral
      {
         Expression* _xxxx;
         mut_EmptyLiteral( Expression* _xxxx ) : _xxxx( _xxxx ) { }
         const Expression & operator * ( ) const { return * _xxxx; }
      };

      mut_EmptyLiteral view_EmptyLiteral( )
      {
         if constexpr( check )
         {
            if( !option_is_EmptyLiteral( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_FieldCall( ) const noexcept
      {
         switch( _ssss )
         {
         case FieldCallExpr:
            return true;
         default:
            return false;
         }
      }

      struct const_FieldCall
      {
         const Expression* _xxxx;
         const Expression & operator * ( ) const { return * _xxxx; }
         const_FieldCall( const Expression* _xxxx ) : _xxxx( _xxxx ) { }

         const Expression & caller( ) const { return _xxxx -> repr. _fld06. heap -> scal. first; }
         const Expression & field( ) const { return _xxxx -> repr. _fld06. heap -> scal. second; }
      };

      const_FieldCall view_FieldCall( ) const
      {
         if constexpr( check )
         {
            if( !option_is_FieldCall( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_FieldCall
      {
         Expression* _xxxx;
         mut_FieldCall( Expression* _xxxx ) : _xxxx( _xxxx ) { }
         const Expression & operator * ( ) const { return * _xxxx; }

         const Expression & caller( ) const { return _xxxx -> repr. _fld06. heap -> scal. first; }
         Expression extr_caller( ) const {
            if( iswritable( _xxxx -> repr. _fld06. heap ))
               return std::move( _xxxx -> repr. _fld06. heap -> scal. first );
            else
               return _xxxx -> repr. _fld06. heap -> scal. first;
         }
         void update_caller( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld06. heap -> scal. first, repl ))
            {
               _xxxx -> repr. _fld06. heap = takeshare( replacebywritable( _xxxx -> repr. _fld06. heap ));
               _xxxx -> repr. _fld06. heap -> scal. first = repl;
            }
         }
         const Expression & field( ) const { return _xxxx -> repr. _fld06. heap -> scal. second; }
         Expression extr_field( ) const {
            if( iswritable( _xxxx -> repr. _fld06. heap ))
               return std::move( _xxxx -> repr. _fld06. heap -> scal. second );
            else
               return _xxxx -> repr. _fld06. heap -> scal. second;
         }
         void update_field( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld06. heap -> scal. second, repl ))
            {
               _xxxx -> repr. _fld06. heap = takeshare( replacebywritable( _xxxx -> repr. _fld06. heap ));
               _xxxx -> repr. _fld06. heap -> scal. second = repl;
            }
         }
      };

      mut_FieldCall view_FieldCall( )
      {
         if constexpr( check )
         {
            if( !option_is_FieldCall( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_FnCall( ) const noexcept
      {
         switch( _ssss )
         {
         case FnCallExpr:
            return true;
         default:
            return false;
         }
      }

      struct const_FnCall
      {
         const Expression* _xxxx;
         const Expression & operator * ( ) const { return * _xxxx; }
         const_FnCall( const Expression* _xxxx ) : _xxxx( _xxxx ) { }

         const std::string & fn_name( ) const { return _xxxx -> repr. _fld07. loc; }
         size_t size( ) const { return _xxxx -> repr. _fld07. heap -> size( ); }
         const Expression & args( size_t _iiii ) const
            { return _xxxx -> repr. _fld07. heap -> begin( ) [ _iiii ]; }
      };

      const_FnCall view_FnCall( ) const
      {
         if constexpr( check )
         {
            if( !option_is_FnCall( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_FnCall
      {
         Expression* _xxxx;
         mut_FnCall( Expression* _xxxx ) : _xxxx( _xxxx ) { }
         const Expression & operator * ( ) const { return * _xxxx; }

         std::string & fn_name( ) const { return _xxxx -> repr. _fld07. loc; }
         std::string extr_fn_name( ) const { return std::move( _xxxx -> repr. _fld07. loc ); }
         void update_fn_name( const std::string & from ) const { _xxxx -> repr. _fld07. loc = from; }

         size_t size( ) const { return _xxxx -> repr. _fld07. heap -> size( ); }
         void push_back( const Expression & xx00 ) const
         {
            _xxxx -> repr. _fld07. heap = tvm::push_back( _xxxx -> repr. _fld07. heap, xx00 );
         }
         void pop_back( ) const { _xxxx -> repr. _fld07. heap = tvm::pop_back( _xxxx -> repr. _fld07. heap ); }
         const Expression& args( size_t _iiii ) const
            { return _xxxx -> repr. _fld07. heap -> begin( ) [ _iiii ]; }
         Expression extr_args( size_t _iiii ) const
         {
            if( iswritable( _xxxx -> repr. _fld07. heap ))
               return std::move( _xxxx -> repr. _fld07. heap -> begin( ) [ _iiii ] );
            else
               return _xxxx -> repr. _fld07. heap -> begin( ) [ _iiii ];
         }
         void update_args( size_t _iiii, const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld07. heap -> begin( ) [ _iiii ], repl ))
            {
               _xxxx -> repr. _fld07. heap = takeshare( replacebywritable( _xxxx -> repr. _fld07. heap ));
               _xxxx -> repr. _fld07. heap -> begin( ) [ _iiii ] = repl;
            }
         }
      };

      mut_FnCall view_FnCall( )
      {
         if constexpr( check )
         {
            if( !option_is_FnCall( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_IntLiteral( ) const noexcept
      {
         switch( _ssss )
         {
         case IntLiteralExpr:
            return true;
         default:
            return false;
         }
      }

      struct const_IntLiteral
      {
         const Expression* _xxxx;
         const Expression & operator * ( ) const { return * _xxxx; }
         const_IntLiteral( const Expression* _xxxx ) : _xxxx( _xxxx ) { }

         const int & value( ) const { return _xxxx -> repr. _fld08. loc; }
      };

      const_IntLiteral view_IntLiteral( ) const
      {
         if constexpr( check )
         {
            if( !option_is_IntLiteral( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_IntLiteral
      {
         Expression* _xxxx;
         mut_IntLiteral( Expression* _xxxx ) : _xxxx( _xxxx ) { }
         const Expression & operator * ( ) const { return * _xxxx; }

         int & value( ) const { return _xxxx -> repr. _fld08. loc; }
         int extr_value( ) const { return std::move( _xxxx -> repr. _fld08. loc ); }
         void update_value( const int & from ) const { _xxxx -> repr. _fld08. loc = from; }
      };

      mut_IntLiteral view_IntLiteral( )
      {
         if constexpr( check )
         {
            if( !option_is_IntLiteral( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_StringLiteral( ) const noexcept
      {
         switch( _ssss )
         {
         case StringLiteralExpr:
            return true;
         default:
            return false;
         }
      }

      struct const_StringLiteral
      {
         const Expression* _xxxx;
         const Expression & operator * ( ) const { return * _xxxx; }
         const_StringLiteral( const Expression* _xxxx ) : _xxxx( _xxxx ) { }

         const std::string & value( ) const { return _xxxx -> repr. _fld09. loc; }
      };

      const_StringLiteral view_StringLiteral( ) const
      {
         if constexpr( check )
         {
            if( !option_is_StringLiteral( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_StringLiteral
      {
         Expression* _xxxx;
         mut_StringLiteral( Expression* _xxxx ) : _xxxx( _xxxx ) { }
         const Expression & operator * ( ) const { return * _xxxx; }

         std::string & value( ) const { return _xxxx -> repr. _fld09. loc; }
         std::string extr_value( ) const { return std::move( _xxxx -> repr. _fld09. loc ); }
         void update_value( const std::string & from ) const { _xxxx -> repr. _fld09. loc = from; }
      };

      mut_StringLiteral view_StringLiteral( )
      {
         if constexpr( check )
         {
            if( !option_is_StringLiteral( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_StructInit( ) const noexcept
      {
         switch( _ssss )
         {
         case StructInitExpr:
            return true;
         default:
            return false;
         }
      }

      struct const_StructInit
      {
         const Expression* _xxxx;
         const Expression & operator * ( ) const { return * _xxxx; }
         const_StructInit( const Expression* _xxxx ) : _xxxx( _xxxx ) { }

         const std::string & struct_name( ) const { return _xxxx -> repr. _fld07. loc; }
         size_t size( ) const { return _xxxx -> repr. _fld07. heap -> size( ); }
         const Expression & inits( size_t _iiii ) const
            { return _xxxx -> repr. _fld07. heap -> begin( ) [ _iiii ]; }
      };

      const_StructInit view_StructInit( ) const
      {
         if constexpr( check )
         {
            if( !option_is_StructInit( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_StructInit
      {
         Expression* _xxxx;
         mut_StructInit( Expression* _xxxx ) : _xxxx( _xxxx ) { }
         const Expression & operator * ( ) const { return * _xxxx; }

         std::string & struct_name( ) const { return _xxxx -> repr. _fld07. loc; }
         std::string extr_struct_name( ) const { return std::move( _xxxx -> repr. _fld07. loc ); }
         void update_struct_name( const std::string & from ) const { _xxxx -> repr. _fld07. loc = from; }

         size_t size( ) const { return _xxxx -> repr. _fld07. heap -> size( ); }
         void push_back( const Expression & xx00 ) const
         {
            _xxxx -> repr. _fld07. heap = tvm::push_back( _xxxx -> repr. _fld07. heap, xx00 );
         }
         void pop_back( ) const { _xxxx -> repr. _fld07. heap = tvm::pop_back( _xxxx -> repr. _fld07. heap ); }
         const Expression& inits( size_t _iiii ) const
            { return _xxxx -> repr. _fld07. heap -> begin( ) [ _iiii ]; }
         Expression extr_inits( size_t _iiii ) const
         {
            if( iswritable( _xxxx -> repr. _fld07. heap ))
               return std::move( _xxxx -> repr. _fld07. heap -> begin( ) [ _iiii ] );
            else
               return _xxxx -> repr. _fld07. heap -> begin( ) [ _iiii ];
         }
         void update_inits( size_t _iiii, const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld07. heap -> begin( ) [ _iiii ], repl ))
            {
               _xxxx -> repr. _fld07. heap = takeshare( replacebywritable( _xxxx -> repr. _fld07. heap ));
               _xxxx -> repr. _fld07. heap -> begin( ) [ _iiii ] = repl;
            }
         }
      };

      mut_StructInit view_StructInit( )
      {
         if constexpr( check )
         {
            if( !option_is_StructInit( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_Unary( ) const noexcept
      {
         switch( _ssss )
         {
         case UnaryExpr:
            return true;
         default:
            return false;
         }
      }

      struct const_Unary
      {
         const Expression* _xxxx;
         const Expression & operator * ( ) const { return * _xxxx; }
         const_Unary( const Expression* _xxxx ) : _xxxx( _xxxx ) { }

         const std::string & op( ) const { return _xxxx -> repr. _fld10. loc; }

         const Expression & expr( ) const { return _xxxx -> repr. _fld10. heap -> scal; }
      };

      const_Unary view_Unary( ) const
      {
         if constexpr( check )
         {
            if( !option_is_Unary( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_Unary
      {
         Expression* _xxxx;
         mut_Unary( Expression* _xxxx ) : _xxxx( _xxxx ) { }
         const Expression & operator * ( ) const { return * _xxxx; }

         std::string & op( ) const { return _xxxx -> repr. _fld10. loc; }
         std::string extr_op( ) const { return std::move( _xxxx -> repr. _fld10. loc ); }
         void update_op( const std::string & from ) const { _xxxx -> repr. _fld10. loc = from; }

         const Expression & expr( ) const { return _xxxx -> repr. _fld10. heap -> scal; }
         Expression extr_expr( ) const {
            if( iswritable( _xxxx -> repr. _fld10. heap ))
               return std::move( _xxxx -> repr. _fld10. heap -> scal );
            else
               return _xxxx -> repr. _fld10. heap -> scal;
         }
         void update_expr( const Expression & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld10. heap -> scal, repl ))
            {
               _xxxx -> repr. _fld10. heap = takeshare( replacebywritable( _xxxx -> repr. _fld10. heap ));
               _xxxx -> repr. _fld10. heap -> scal = repl;
            }
         }
      };

      mut_Unary view_Unary( )
      {
         if constexpr( check )
         {
            if( !option_is_Unary( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_VarName( ) const noexcept
      {
         switch( _ssss )
         {
         case VarNameExpr:
            return true;
         default:
            return false;
         }
      }

      struct const_VarName
      {
         const Expression* _xxxx;
         const Expression & operator * ( ) const { return * _xxxx; }
         const_VarName( const Expression* _xxxx ) : _xxxx( _xxxx ) { }

         const std::string & name( ) const { return _xxxx -> repr. _fld09. loc; }
      };

      const_VarName view_VarName( ) const
      {
         if constexpr( check )
         {
            if( !option_is_VarName( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_VarName
      {
         Expression* _xxxx;
         mut_VarName( Expression* _xxxx ) : _xxxx( _xxxx ) { }
         const Expression & operator * ( ) const { return * _xxxx; }

         std::string & name( ) const { return _xxxx -> repr. _fld09. loc; }
         std::string extr_name( ) const { return std::move( _xxxx -> repr. _fld09. loc ); }
         void update_name( const std::string & from ) const { _xxxx -> repr. _fld09. loc = from; }
      };

      mut_VarName view_VarName( )
      {
         if constexpr( check )
         {
            if( !option_is_VarName( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
   };
}

#endif

