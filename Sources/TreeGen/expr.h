
// This code was automatically built by TreeGen
// Written by Hans de Nivelle, see www.compiler-tools.eu

#ifndef MYLANG_EXPR_
#define MYLANG_EXPR_

#include <iostream>
#include <utility>
#include <tuple>
#include <initializer_list>

#include "selector.h"
#include "tvm/includes.h"

namespace mylang { 


   class expr
   {
   private:
      selector _ssss;

      using _loc00 = tvm::unit;
      using _scal00 = std::pair<expr,expr>;
      using _rep00 = tvm::unit;
      using _loc01 = tvm::unit;
      using _scal01 = tvm::unit;
      using _rep01 = tvm::unit;
      using _loc02 = tvm::unit;
      using _scal02 = std::tuple<expr,expr,expr>;
      using _rep02 = tvm::unit;
      using _loc03 = tvm::unit;
      using _scal03 = int;
      using _rep03 = tvm::unit;
      using _loc04 = tvm::unit;
      using _scal04 = std::string;
      using _rep04 = tvm::unit;

      static constexpr bool check = true;

      union options
      {
         tvm::field< _loc00, _scal00, _rep00 > _fld00;
         tvm::field< _loc01, _scal01, _rep01 > _fld01;
         tvm::field< _loc02, _scal02, _rep02 > _fld02;
         tvm::field< _loc03, _scal03, _rep03 > _fld03;
         tvm::field< _loc04, _scal04, _rep04 > _fld04;

         options( ) : _fld01( ) { }
         ~options( ) noexcept { }
      };

   private:
      options repr;

   public:
      expr( ) = delete;
      expr( const expr& );
      expr( expr&& ) noexcept;
      const expr& operator = ( const expr& );
      const expr& operator = ( expr&& ) noexcept;
      ~expr( ) noexcept;
      
      selector sel( ) const { return _ssss; }
      bool very_equal_to( const expr& ) const;
      void printstate( std::ostream& out ) const;
      
      expr( selector sel, const expr & _xx00, const expr & _xx01 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case expr_add:
            case expr_div:
            case expr_sub:
            case expr_mul:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         repr. _fld00. heap = takeshare( tvm::constr_scalar< _scal00 > ( std::pair( _xx00, _xx01 ) ));
      }

      expr( selector sel )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case expr_empty:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
      }

      expr( selector sel, const expr & _xx00, const expr & _xx01, const expr & _xx02 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case expr_if:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         repr. _fld02. heap = takeshare( tvm::constr_scalar< _scal02 > ( std::tuple( _xx00, _xx01, _xx02 ) ));
      }

      expr( selector sel, const int & _xx00 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case expr_lit:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         repr. _fld03. heap = takeshare( tvm::constr_scalar< _scal03 > ( _xx00 ));
      }

      expr( selector sel, const std::string & _xx00 )
         : _ssss( sel )
      {
         if constexpr( check )
         {
            switch( _ssss )
            {
            case expr_var:
               break;
            default:
               throw std::invalid_argument( "wrong selector for constructor" );
            }
         }
         repr. _fld04. heap = takeshare( tvm::constr_scalar< _scal04 > ( _xx00 ));
      }

      bool option_is_binop( ) const noexcept
      {
         switch( _ssss )
         {
         case expr_div:
         case expr_add:
         case expr_sub:
         case expr_mul:
            return true;
         default:
            return false;
         }
      }

      struct const_binop
      {
         const expr* _xxxx;
         const expr & operator * ( ) const { return * _xxxx; }
         const_binop( const expr* _xxxx ) : _xxxx( _xxxx ) { }

         const expr & left( ) const { return _xxxx -> repr. _fld00. heap -> scal. first; }
         const expr & right( ) const { return _xxxx -> repr. _fld00. heap -> scal. second; }
      };

      const_binop view_binop( ) const
      {
         if constexpr( check )
         {
            if( !option_is_binop( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_binop
      {
         expr* _xxxx;
         mut_binop( expr* _xxxx ) : _xxxx( _xxxx ) { }
         const expr & operator * ( ) const { return * _xxxx; }

         const expr & left( ) const { return _xxxx -> repr. _fld00. heap -> scal. first; }
         expr extr_left( ) const {
            if( iswritable( _xxxx -> repr. _fld00. heap ))
               return std::move( _xxxx -> repr. _fld00. heap -> scal. first );
            else
               return _xxxx -> repr. _fld00. heap -> scal. first;
         }
         void update_left( const expr & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld00. heap -> scal. first, repl ))
            {
               _xxxx -> repr. _fld00. heap = takeshare( replacebywritable( _xxxx -> repr. _fld00. heap ));
               _xxxx -> repr. _fld00. heap -> scal. first = repl;
            }
         }
         const expr & right( ) const { return _xxxx -> repr. _fld00. heap -> scal. second; }
         expr extr_right( ) const {
            if( iswritable( _xxxx -> repr. _fld00. heap ))
               return std::move( _xxxx -> repr. _fld00. heap -> scal. second );
            else
               return _xxxx -> repr. _fld00. heap -> scal. second;
         }
         void update_right( const expr & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld00. heap -> scal. second, repl ))
            {
               _xxxx -> repr. _fld00. heap = takeshare( replacebywritable( _xxxx -> repr. _fld00. heap ));
               _xxxx -> repr. _fld00. heap -> scal. second = repl;
            }
         }
      };

      mut_binop view_binop( )
      {
         if constexpr( check )
         {
            if( !option_is_binop( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_empty( ) const noexcept
      {
         switch( _ssss )
         {
         case expr_empty:
            return true;
         default:
            return false;
         }
      }

      struct const_empty
      {
         const expr* _xxxx;
         const expr & operator * ( ) const { return * _xxxx; }
         const_empty( const expr* _xxxx ) : _xxxx( _xxxx ) { }
      };

      const_empty view_empty( ) const
      {
         if constexpr( check )
         {
            if( !option_is_empty( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_empty
      {
         expr* _xxxx;
         mut_empty( expr* _xxxx ) : _xxxx( _xxxx ) { }
         const expr & operator * ( ) const { return * _xxxx; }
      };

      mut_empty view_empty( )
      {
         if constexpr( check )
         {
            if( !option_is_empty( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_ifexpr( ) const noexcept
      {
         switch( _ssss )
         {
         case expr_if:
            return true;
         default:
            return false;
         }
      }

      struct const_ifexpr
      {
         const expr* _xxxx;
         const expr & operator * ( ) const { return * _xxxx; }
         const_ifexpr( const expr* _xxxx ) : _xxxx( _xxxx ) { }

         const expr & cond( ) const { return get<0> ( _xxxx -> repr. _fld02. heap -> scal ); }
         const expr & then_branch( ) const { return get<1> ( _xxxx -> repr. _fld02. heap -> scal ); }
         const expr & else_branch( ) const { return get<2> ( _xxxx -> repr. _fld02. heap -> scal ); }
      };

      const_ifexpr view_ifexpr( ) const
      {
         if constexpr( check )
         {
            if( !option_is_ifexpr( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_ifexpr
      {
         expr* _xxxx;
         mut_ifexpr( expr* _xxxx ) : _xxxx( _xxxx ) { }
         const expr & operator * ( ) const { return * _xxxx; }

         const expr & cond( ) const { return get<0> ( _xxxx -> repr. _fld02. heap -> scal ); }
         expr extr_cond( ) const {
            if( iswritable( _xxxx -> repr. _fld02. heap ))
               return std::move( get<0> ( _xxxx -> repr. _fld02. heap -> scal ) );
            else
               return get<0> ( _xxxx -> repr. _fld02. heap -> scal );
         }
         void update_cond( const expr & repl ) const
         {
            if( tvm::distinct( get<0> ( _xxxx -> repr. _fld02. heap -> scal ), repl ))
            {
               _xxxx -> repr. _fld02. heap = takeshare( replacebywritable( _xxxx -> repr. _fld02. heap ));
               get<0> ( _xxxx -> repr. _fld02. heap -> scal ) = repl;
            }
         }
         const expr & then_branch( ) const { return get<1> ( _xxxx -> repr. _fld02. heap -> scal ); }
         expr extr_then_branch( ) const {
            if( iswritable( _xxxx -> repr. _fld02. heap ))
               return std::move( get<1> ( _xxxx -> repr. _fld02. heap -> scal ) );
            else
               return get<1> ( _xxxx -> repr. _fld02. heap -> scal );
         }
         void update_then_branch( const expr & repl ) const
         {
            if( tvm::distinct( get<1> ( _xxxx -> repr. _fld02. heap -> scal ), repl ))
            {
               _xxxx -> repr. _fld02. heap = takeshare( replacebywritable( _xxxx -> repr. _fld02. heap ));
               get<1> ( _xxxx -> repr. _fld02. heap -> scal ) = repl;
            }
         }
         const expr & else_branch( ) const { return get<2> ( _xxxx -> repr. _fld02. heap -> scal ); }
         expr extr_else_branch( ) const {
            if( iswritable( _xxxx -> repr. _fld02. heap ))
               return std::move( get<2> ( _xxxx -> repr. _fld02. heap -> scal ) );
            else
               return get<2> ( _xxxx -> repr. _fld02. heap -> scal );
         }
         void update_else_branch( const expr & repl ) const
         {
            if( tvm::distinct( get<2> ( _xxxx -> repr. _fld02. heap -> scal ), repl ))
            {
               _xxxx -> repr. _fld02. heap = takeshare( replacebywritable( _xxxx -> repr. _fld02. heap ));
               get<2> ( _xxxx -> repr. _fld02. heap -> scal ) = repl;
            }
         }
      };

      mut_ifexpr view_ifexpr( )
      {
         if constexpr( check )
         {
            if( !option_is_ifexpr( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_literal( ) const noexcept
      {
         switch( _ssss )
         {
         case expr_lit:
            return true;
         default:
            return false;
         }
      }

      struct const_literal
      {
         const expr* _xxxx;
         const expr & operator * ( ) const { return * _xxxx; }
         const_literal( const expr* _xxxx ) : _xxxx( _xxxx ) { }

         const int & value( ) const { return _xxxx -> repr. _fld03. heap -> scal; }
      };

      const_literal view_literal( ) const
      {
         if constexpr( check )
         {
            if( !option_is_literal( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_literal
      {
         expr* _xxxx;
         mut_literal( expr* _xxxx ) : _xxxx( _xxxx ) { }
         const expr & operator * ( ) const { return * _xxxx; }

         const int & value( ) const { return _xxxx -> repr. _fld03. heap -> scal; }
         int extr_value( ) const {
            if( iswritable( _xxxx -> repr. _fld03. heap ))
               return std::move( _xxxx -> repr. _fld03. heap -> scal );
            else
               return _xxxx -> repr. _fld03. heap -> scal;
         }
         void update_value( const int & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld03. heap -> scal, repl ))
            {
               _xxxx -> repr. _fld03. heap = takeshare( replacebywritable( _xxxx -> repr. _fld03. heap ));
               _xxxx -> repr. _fld03. heap -> scal = repl;
            }
         }
      };

      mut_literal view_literal( )
      {
         if constexpr( check )
         {
            if( !option_is_literal( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
      bool option_is_var( ) const noexcept
      {
         switch( _ssss )
         {
         case expr_var:
            return true;
         default:
            return false;
         }
      }

      struct const_var
      {
         const expr* _xxxx;
         const expr & operator * ( ) const { return * _xxxx; }
         const_var( const expr* _xxxx ) : _xxxx( _xxxx ) { }

         const std::string & name( ) const { return _xxxx -> repr. _fld04. heap -> scal; }
      };

      const_var view_var( ) const
      {
         if constexpr( check )
         {
            if( !option_is_var( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }

      struct mut_var
      {
         expr* _xxxx;
         mut_var( expr* _xxxx ) : _xxxx( _xxxx ) { }
         const expr & operator * ( ) const { return * _xxxx; }

         const std::string & name( ) const { return _xxxx -> repr. _fld04. heap -> scal; }
         std::string extr_name( ) const {
            if( iswritable( _xxxx -> repr. _fld04. heap ))
               return std::move( _xxxx -> repr. _fld04. heap -> scal );
            else
               return _xxxx -> repr. _fld04. heap -> scal;
         }
         void update_name( const std::string & repl ) const
         {
            if( tvm::distinct( _xxxx -> repr. _fld04. heap -> scal, repl ))
            {
               _xxxx -> repr. _fld04. heap = takeshare( replacebywritable( _xxxx -> repr. _fld04. heap ));
               _xxxx -> repr. _fld04. heap -> scal = repl;
            }
         }
      };

      mut_var view_var( )
      {
         if constexpr( check )
         {
            if( !option_is_var( ))
               throw std::invalid_argument( "wrong selector for view" );
         }
         return this;
      }
      
   };
}

#endif

