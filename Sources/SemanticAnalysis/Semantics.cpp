#include "Semantics.h"

namespace val
{
	Semantics::Semantics(Statement&& AST) : AST(std::move(AST))
	{
		
	}

    std::string ExprToStr(const Expression& expr)
    {
        return std::string();
    }

    std::unique_ptr<ArrayKind> BuildArrayField(const Statement& array_init_stmt)
    {
        const auto& array_init = array_init_stmt.view_ArrayInit();
        auto array = std::make_unique<ArrayKind>();
        array->is_dynamic = array_init.is_dynamic();
        array->valid_size_expression = ExprToStr(array_init.alloc_size());

        if (array_init.type_info().option_is_ArrayInit())
        {
            array->of_kind = BuildArrayField(array_init.type_info());
        }
        else
        {
            array->of_kind = std::make_unique<ObjectKind>(
                ObjectKind{ IsPrimitive(array_init.type_info().view_VarInit().type_name()), array_init.type_info().view_VarInit().type_name()
            });
        }

        return array;
    }

    std::string GetArrayName(const Statement& array_init_stmt)
    {
        if (array_init_stmt.option_is_VarInit())
        {
            return array_init_stmt.view_VarInit().var_name();
        }
        else {
            return GetArrayName(array_init_stmt.view_ArrayInit().type_info());
        }
    }

    std::string Semantics::ExprToStr(const Expression& expr)
    {
        return std::string();
    }

    StructType Semantics::ConstructStructType(const Statement& make_struct_stmt) noexcept
    {
        StructType stype;
        const auto& view = make_struct_stmt.view_MakeStruct();

        stype.name = view.struct_name();

        for (size_t i = 0; i < view.size(); i++)
        {
            if (view.inits(i).option_is_VarInit())
            {
                const auto& var_init = view.inits(i).view_VarInit();

                stype.fields.push_back({ 
                    var_init.var_name(),
                    std::make_unique<ObjectKind>(ObjectKind{ 
                        IsPrimitive(var_init.type_name()), var_init.type_name()
                    })
                });
            }
            else
            {
                stype.fields.push_back({
                    GetArrayName(view.inits(i)),
                    BuildArrayField(view.inits(i))
                });
            }
        }

        return stype;
    }
}