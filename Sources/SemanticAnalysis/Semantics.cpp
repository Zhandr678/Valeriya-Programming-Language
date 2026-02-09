#include "Semantics.h"

namespace val
{
    static void AddCommands(std::vector <Command>& commands, std::vector <Command>&& more_cmds)
    {
        for (const Command& cmd : more_cmds)
        {
            commands.push_back(cmd);
        }
    }

    std::vector<Command> Semantics::AnalyzePrepareCommands(const Statement& AST)
    {
        std::vector <Command> commands;

        const auto& seq = AST.view_Block();

        for (size_t i = 0; i < seq.size(); i++)
        {
            switch (seq.statements(i).sel())
            {
            case selector::VarInitStmt:
                AddCommands(commands, std::move(AnalyzeVarInit(seq.statements(i))));
                break;
            case selector::ArrayInitStmt:
                break;
            case selector::WhileLoopStmt:
                AddCommands(commands, std::move(AnalyzeWhileLoop(seq.statements(i))));
                break;
            default:
                std::cout << "Unhandled selector";
                break;
            }
        }

        return commands;
    }

    std::string ExprToStr(const Expression& expr)
    {
        return std::string();
    }

    std::string GetTypeNameFromVar(const VariableKind& arr)
    {
        if (std::holds_alternative <ObjectKind>(arr))
        {
            return std::get <ObjectKind>(arr).type_name;
        }
        else if (std::holds_alternative <ArrayKind>(arr))
        {
            return GetTypeNameFromVar(std::get <ArrayKind>(arr).of_kind);
        }
        else {
            throw;
        }
    }

    StructType Semantics::ConstructStructType(const Statement& make_struct_stmt) const noexcept
    {
        StructType stype;
        const auto& view = make_struct_stmt.view_MakeStruct();

        stype.name = view.struct_name();

        for (size_t i = 0; i < view.size(); i++)
        {
            if (view.inits(i).option_is_VarInit())
            {
                const auto& var_init = view.inits(i).view_VarInit();
                stype.order_fields.push_back(var_init.var_name());
                stype.fields.insert({ 
                    var_init.var_name(),
                    ObjectKind{ 
                        IsPrimitive(var_init.type_name()), var_init.type_name()
                    }
                });
            }
            else
            {
                const auto& arr_init = view.inits(i).view_ArrayInit();
                stype.order_fields.push_back(arr_init.type_info().view_VarInit().var_name());
                stype.fields.insert({
                    arr_init.type_info().view_VarInit().var_name(),
                    ArrayKind{
                        ExprToStr(arr_init.alloc_size()),
                        arr_init.is_dynamic(),
                        ObjectKind{ IsPrimitive(arr_init.type_info().view_VarInit().type_name()), arr_init.type_info().view_VarInit().type_name() }
                    }
                });
            }
        }

        return stype;
    }

    PropertyType Semantics::ConstructPropertyType(const Statement& make_property_stmt) const noexcept
    {
        PropertyType ptype;
        const auto& view = make_property_stmt.view_MakeProperty();

        ptype.name = view.prop_name();

        for (size_t i = 0; i < view.size(); i++)
        {
            StructType option = ConstructStructType(view.opts(i));
            ptype.options.insert({ option.name, std::move(option) });
        }

        return ptype;
    }

    EnumType Semantics::ConstructEnumType(const Statement& make_enum_stmt) const noexcept
    {
        EnumType etype;
        const auto& view = make_enum_stmt.view_MakeEnum();

        etype.name = view.enum_name();

        for (size_t i = 0; i < view.size(); i++)
        {
            etype.enum_variants.insert(view.enum_variants(i));
        }

        return etype;
    }

    FnTable Semantics::ConstructFn(const Statement& make_fn_stmt) const noexcept
    {
        FnTable ftable;
        const auto& view = make_fn_stmt.view_MakeFunction();

        ftable.name = view.fn_name();
        ftable.ret_type_name = view.ret_type_name();

        for (size_t i = 0; i < view.size(); i++)
        {
            const auto& param_view = view.params(i).view_FnArgs();

            if (param_view.is_array())
            {
                auto arr_kind = ArrayKind{
                    std::to_string(param_view.array_info(param_view.size() - 1).second),
                    param_view.array_info(param_view.size() - 1).first,
                    ObjectKind{
                        IsPrimitive(param_view.type_name()),
                        param_view.type_name()
                    }
                };
                
                ftable.order_param.push_back(param_view.var_name());
                ftable.param_symbol_table.insert({
                    param_view.var_name(),
                    { 
                        param_view.is_inout(),
                        std::move(arr_kind) 
                    }
                });
            }
            else 
            {
                ftable.param_symbol_table.insert({
                    param_view.var_name(),
                    {
                        param_view.is_inout(),
                        ObjectKind{ IsPrimitive(param_view.type_name()), param_view.type_name() }
                    } 
                });
            }
        }

        return ftable;
    }

   /* TypeClass Semantics::IdentifyType(const std::string& var_name) const noexcept
    {
        if (std::holds_alternative <std::shared_ptr <ObjectKind>>(symbol_table.at(var_name)))
        {
            const auto& obj = std::get <std::shared_ptr <ObjectKind>>(symbol_table.at(var_name));
            if (obj->is_primitive)
            {
                return StrToPrim(obj->type_name);
            }
            else 
            {
                const auto& custom_type = type_table.at(obj->type_name);

                if (std::holds_alternative <StructType>(custom_type))
                {
                    return TypeClass{ std::get <StructType>(custom_type) };
                }
                else if (std::holds_alternative <PropertyType>(custom_type))
                {
                    return TypeClass{ std::get <PropertyType>(custom_type) };
                }
                else if (std::holds_alternative <EnumType>(custom_type))
                {
                    return TypeClass{ std::get <EnumType>(custom_type) };
                }
                else 
                {
                    return AnyType{};
                }
            }
        }
        else 
        {
            const auto& arr = std::get <std::shared_ptr <ArrayKind>>(symbol_table.at(var_name));

            return ArrayType{ arr->depth, GetTypeNameFromVar(symbol_table.at(var_name)) };
        }
    }

    size_t Semantics::FindDepth(const Expression& init_list_expr) const noexcept
    {
        if (not init_list_expr.option_is_InitList()) { return 0; }
        if (init_list_expr.view_InitList().size() == 0) { return 0; }

        return 1 + FindDepth(init_list_expr.view_InitList().exprs(0));
    }

    std::string Semantics::FindCoreTypeName(const Expression& init_list_expr) const noexcept
    {
        if (not init_list_expr.option_is_InitList())
        {
            return IdentifyType(init_list_expr);
        }
    }

    TypeClass Semantics::IdentifyType(const Expression& expr) const noexcept
    {
        switch (expr.sel())
        {
        case selector::IntLiteralExpr:
            return PrimitiveType::INT;
        case selector::DoubleLiteralExpr:
            return PrimitiveType::DOUBLE;
        case selector::StringLiteralExpr:
            return PrimitiveType::STRING;
        case selector::CharLiteralExpr:
            return PrimitiveType::CHAR;
        case selector::BoolLiteralExpr:
            return PrimitiveType::BOOL;
        case selector::VarNameExpr:
            return IdentifyType(expr.view_VarName().name());
        case selector::FieldCallExpr:
            return IdentifyType(expr.view_FieldCall().field());
        case selector::FnCallExpr:
            const std::string& ret_type = fn_table.at(expr.view_FnCall().fn_name()).ret_type;
            if (IsPrimitive(ret_type)) { return StrToPrim(ret_type); }
            return type_table.at(ret_type);
        case selector::StructInitExpr:
            return type_table.at(expr.view_StructInit().struct_name());
        case selector::ArrayIndexExpr:
            return IdentifyType(expr.view_ArrayIndex().at());
        case selector::InitListExpr:
            return ArrayType{ FindDepth(expr), FindTypeName(expr) };
        default:
            return AnyType{};
        }
    }*/

    bool Semantics::NameExists(const std::string& name) const
    {
        return (symbol_table.contains(name) || type_table.contains(name) 
            || fn_table.contains(name)) && (active.contains(name));
    }

    void Semantics::AddVariable(const Statement& var_init) noexcept
    {
        if (var_init.option_is_VarInit())
        {
            const auto& view = var_init.view_VarInit();

            symbol_table.insert({
                view.var_name(),
                ObjectKind{ IsPrimitive(view.type_name()), view.type_name() }
            });

            active.insert({ view.var_name(), true });
        }
        else 
        {
            const auto& view = var_init.view_ArrayInit();
            symbol_table.insert({
                view.type_info().view_VarInit().var_name(),
                ArrayKind{
                    ExprToStr(view.init_expr()),
                    view.is_dynamic(),
                    ObjectKind{ 
                        IsPrimitive(view.type_info().view_VarInit().type_name()), 
                        view.type_info().view_VarInit().type_name() 
                    }
                }
            });

            active.insert({ view.type_info().view_VarInit().var_name(), true });
        }
    }

    void Semantics::AddType(const std::string& type_name, TypeClass&& type) noexcept
    {
        type_table.insert({
            type_name,
            std::move(type)
        });

        active.insert({ type_name, true });
    }
    void Semantics::AddFn(const std::string& fn_name, FnTable&& ftable) noexcept
    {
        fn_table.insert({
            fn_name,
            std::move(ftable)
        });

        active.insert({ fn_name, true });
    }

    void Semantics::Deactivate(const std::string& name) noexcept
    {
        active[name] = false;
    }

    void Semantics::VerifyInoutUniqueness(const Expression& fn_call_expr) const
    {

    }

    static bool CanBeAssigned(const VariableKind& to, const VariableKind& from) noexcept
    {
        if (to.index() != from.index()) { return false; }

        if (std::holds_alternative <ObjectKind>(to))
        {
            return std::get <ObjectKind>(to).type_name ==
                std::get <ObjectKind>(from).type_name ||

                (std::get <ObjectKind>(to).type_name == "double"
                    && (std::get <ObjectKind>(from).type_name == "int"
                        || std::get <ObjectKind>(from).type_name == "uint")) ||

                (std::get <ObjectKind>(to).type_name == "int" &&
                    std::get <ObjectKind>(from).type_name == "uint");
        }

        else if (std::holds_alternative <ArrayKind>(to))
        {
            return (std::get <ArrayKind>(to).of_kind.type_name) == (std::get <ArrayKind>(from).of_kind.type_name);
        }

        else 
        {
            return false;
        }
    }

    VariableKind Semantics::AnalyzeVarNameExpr(const Expression& varname_expr)
    {
        if (not varname_expr.option_is_VarName()) { throw; }
        const auto& view_varname = varname_expr.view_VarName();

        if (not symbol_table.contains(view_varname.name())) { throw; }
        return symbol_table.at(view_varname.name());
    }

    VariableKind Semantics::AnalyzeArrayCallExpr(const Expression& array_call)
    {
        if (not array_call.option_is_ArrayIndex()) { throw; }
        const auto& view_arr_call = array_call.view_ArrayIndex();

        return AnalyzeExpression(view_arr_call.array_expr());
    }

    VariableKind Semantics::AnalyzeLiterals(const Expression& any_literal_expr)
    {
        switch (any_literal_expr.sel())
        {
        case selector::BoolLiteralExpr:
            return ObjectKind{ true, "bool" };
        case selector::IntLiteralExpr:
            if (any_literal_expr.view_IntLiteral().value() < 0) { return ObjectKind{ true, "int" }; }
            return ObjectKind{ true, "uint" };
        case selector::DoubleLiteralExpr:
            return ObjectKind{ true, "double" };
        case selector::CharLiteralExpr:
            return ObjectKind{ true, "char" };
        case selector::StringLiteralExpr:
            return ArrayKind{ std::to_string(any_literal_expr.view_StringLiteral().value().length()), true, ObjectKind{ true, "char" } };
        default:
            throw;
        }
    }

    VariableKind Semantics::AnalyzeStructFieldCallExpr(const Expression& field_call)
    {
        if (not field_call.option_is_FieldCall()) { throw; }
        const auto& view_fcall = field_call.view_FieldCall();

        auto kind_of_caller = AnalyzeExpression(view_fcall.caller());

        if (not view_fcall.field().option_is_VarName())
        {
            throw; // field must be a name
        }

        std::string name_of_field = view_fcall.field().view_VarName().name();
        if (std::holds_alternative <ArrayKind>(kind_of_caller))
        {
            throw; // arrays cannot be accessed by field name
        }

        const auto& struct_kind = std::get <ObjectKind>(kind_of_caller);
        if (struct_kind.is_primitive) 
        { 
            throw; // primitive type cannot be accessed by field name
        }

        if (not std::holds_alternative <StructType>(type_table[struct_kind.type_name]))
        {
            throw; // field call on non-ADT
        }

        return std::get <StructType>(type_table[struct_kind.type_name]).fields[name_of_field];
    }

    VariableKind Semantics::AnalyzePropertyFieldCallExpr(const Expression& field_call, const std::string& active_option)
    {
        if (not field_call.option_is_FieldCall()) { throw; }
        const auto& view_fcall = field_call.view_FieldCall();

        auto kind_of_caller = AnalyzeExpression(view_fcall.caller());

        if (not view_fcall.field().option_is_VarName())
        {
            throw; // field must be a name
        }

        std::string name_of_field = view_fcall.field().view_VarName().name();
        if (std::holds_alternative <ArrayKind>(kind_of_caller))
        {
            throw; // arrays cannot be accessed by field name
        }

        const auto& prop_kind = std::get <ObjectKind>(kind_of_caller);
        if (prop_kind.is_primitive)
        {
            throw; // primitive type cannot be accessed by field name
        }

        if (not std::holds_alternative <PropertyType>(type_table[prop_kind.type_name]))
        {
            throw; // field call on non-ADT
        }

        if (not std::get <PropertyType>(type_table[prop_kind.type_name]).options.contains(active_option))
        {
            throw; // unknown option for property std::get <std::shared_ptr <ObjectKind>>(kind_of_caller)->type_name;
        }

        return std::get <PropertyType>(type_table[prop_kind.type_name]).options[active_option].fields[name_of_field];
    }

    VariableKind Semantics::AnalyzeFnCallExpr(const Expression& fn_call)
    {
        if (not fn_call.option_is_FnCall()) { throw; }

        const auto& view_fncall = fn_call.view_FnCall();

        if (not fn_table.contains(view_fncall.fn_name())) { throw; }

        for (size_t i = 0; i < view_fncall.size(); i++)
        {
            if (not CanBeAssigned(
                AnalyzeExpression(view_fncall.args(i)), 
                fn_table[view_fncall.fn_name()].param_symbol_table[fn_table[view_fncall.fn_name()].order_param[i]].kind)
            ) {
                throw;
            }
        }

        VerifyInoutUniqueness(fn_call);
        return ObjectKind{ false, fn_table[view_fncall.fn_name()].ret_type_name };
    }

    static bool IsNumeric(const ObjectKind& obj_kind)
    {
        return obj_kind.type_name == "double" || obj_kind.type_name == "int" || obj_kind.type_name == "uint";
    }

    VariableKind Semantics::AnalyzeBinopExpr(const Expression& binop_expr)
    {
        if (not binop_expr.option_is_Binary()) { throw; }

        const auto& view_binop = binop_expr.view_Binary();

        auto left_kind = AnalyzeExpression(view_binop.lhs());
        auto right_kind = AnalyzeExpression(view_binop.rhs());

        if (left_kind.index() != right_kind.index()) { throw; }

        if (view_binop.op() == "==" || view_binop.op() == "!=")
        {
            if (not CanBeAssigned(left_kind, right_kind))
            {
                throw;
            }

            return ObjectKind{ true, "bool" };
        }

        else if (view_binop.op() == ">" || view_binop.op() == "<" || view_binop.op() == "<=" || view_binop.op() == ">=")
        {
            if (std::holds_alternative <ArrayKind>(left_kind))
            {
                throw;
            }

            if (not IsNumeric(std::get <ObjectKind>(left_kind)) ||
                not IsNumeric(std::get <ObjectKind>(right_kind)))
            {
                throw;
            }

            return ObjectKind{ true, "bool" };
        }

        else if (view_binop.op() == "+" || view_binop.op() == "-" || view_binop.op() == "/")
        {
            if (std::holds_alternative <ArrayKind>(left_kind))
            {
                throw;
            }

            if ((std::get <ObjectKind>(left_kind).type_name == "uint" || std::get <ObjectKind>(left_kind).type_name == "int") &&
                (std::get <ObjectKind>(right_kind).type_name == "uint" || std::get <ObjectKind>(right_kind).type_name == "int"))
            {
                return ObjectKind{ true, "int" };
            }

            if ((std::get <ObjectKind>(left_kind).type_name == "double" && 
                IsNumeric(std::get <ObjectKind>(right_kind))) ||
                (IsNumeric(std::get <ObjectKind>(left_kind)) &&
                std::get <ObjectKind>(right_kind).type_name == "double"))
            {
                return ObjectKind{ true, "double" };
            }

            throw;
        }

        else if (view_binop.op() == "*" || view_binop.op() == "**")
        {
            if (std::holds_alternative <ArrayKind>(left_kind))
            {
                throw;
            }

            if (IsNumeric(std::get <ObjectKind>(left_kind)) && IsNumeric(std::get <ObjectKind>(right_kind)))
            {
                return ObjectKind{ true, "double" };
            }

            throw;
        }

        else if (view_binop.op() == "%")
        {
            if (std::holds_alternative <ArrayKind>(left_kind))
            {
                throw;
            }

            if (std::get <ObjectKind>(left_kind).type_name == "uint" || std::get <ObjectKind>(left_kind).type_name == "int" &&
                std::get <ObjectKind>(right_kind).type_name == "uint" || std::get <ObjectKind>(right_kind).type_name == "int")
            {
                return ObjectKind{ true, "int" };
            }

            throw;
        }

        else if (view_binop.op() == "&&" || view_binop.op() == "||")
        {
            if (std::holds_alternative <ArrayKind>(left_kind))
            {
                throw;
            }

            if (std::get <ObjectKind>(left_kind).type_name == "bool" && std::get <ObjectKind>(right_kind).type_name == "bool")
            {
                return ObjectKind{ true, "bool" };
            }

            throw;
        }

        else if (view_binop.op() == "&" || view_binop.op() == "|" || view_binop.op() == "^" || view_binop.op() == "<<" || view_binop.op() == ">>")
        {
            if (std::holds_alternative <ArrayKind>(left_kind))
            {
                throw;
            }
            
            if ((std::get <ObjectKind>(left_kind).type_name == "uint" || std::get <ObjectKind>(left_kind).type_name == "int") && 
                (std::get <ObjectKind>(right_kind).type_name == "uint" || std::get <ObjectKind>(right_kind).type_name == "int"))
            {
                return ObjectKind{ true, "uint" };
            }

            throw;
        }

        throw;
    }

    VariableKind Semantics::AnalyzeUnaryExpr(const Expression& unary_expr)
    {
        if (not unary_expr.option_is_Unary()) { throw; }

        const auto& view_unary = unary_expr.view_Unary();

        if (view_unary.op() == "-")
        {
            if (CanBeAssigned(ObjectKind{ true, "int" }, AnalyzeExpression(view_unary.expr())))
            {
                return ObjectKind{ true, "int" };
            }
            else if (CanBeAssigned(ObjectKind{ true, "double" }, AnalyzeExpression(view_unary.expr())))
            {
                return ObjectKind{ true, "double" };
            }
            
            throw;
        }
        else if (view_unary.op() == "!")
        {
            if (CanBeAssigned(ObjectKind{ true, "bool" }, AnalyzeExpression(view_unary.expr())))
            {
                return ObjectKind{ true, "bool" };
            }
            
            throw;
        }

        throw;
    }

    VariableKind Semantics::AnalyzeInitListExpr(const Expression& init_list_expr)
    {
        if (not init_list_expr.option_is_InitList()) { throw; }

        const auto& view_inlist = init_list_expr.view_InitList();

        if (view_inlist.size() == 0) { return ObjectKind{ true, "void" }; }

        auto base_kind = AnalyzeExpression(view_inlist.exprs(0));

        for (size_t i = 1; i < view_inlist.size(); i++)
        {
            if (not CanBeAssigned(base_kind, AnalyzeExpression(view_inlist.exprs(i))))
            {
                throw;
            }
        }

        return ArrayKind{ std::to_string(view_inlist.size()), false, std::get <ObjectKind>(base_kind) };
    }

    VariableKind Semantics::AnalyzeStructInitExpr(const Expression& struct_init_expr)
    {
        if (not struct_init_expr.option_is_StructInit()) { throw; }
        
        const auto& view = struct_init_expr.view_StructInit();
        
        if (not type_table.contains(view.struct_name()))
        {
            throw;
        }
        if (not std::holds_alternative <StructType>(type_table[view.struct_name()]))
        {
            throw;
        }
        
        for (size_t i = 0; i < view.size(); i++)
        {
            if (not CanBeAssigned(AnalyzeExpression(view.inits(i)), 
                std::get <StructType>(type_table[view.struct_name()]).fields[std::get <StructType>(type_table[view.struct_name()]).order_fields[i]]))
            {
                throw;
            }
        }

        return ObjectKind{ false, struct_init_expr.view_StructInit().struct_name() };
    }

    VariableKind Semantics::AnalyzeExpression(const Expression& expr)
    {
        switch (expr.sel())
        {
        case selector::EmptyExpr:
            return ObjectKind{ true, "void" };
        case selector::VarNameExpr:
            return AnalyzeVarNameExpr(expr);
        case selector::ArrayIndexExpr:
            return AnalyzeArrayCallExpr(expr);
        case selector::BoolLiteralExpr: case selector::CharLiteralExpr: case selector::DoubleLiteralExpr:
        case selector::IntLiteralExpr: case selector::StringLiteralExpr:
            return AnalyzeLiterals(expr);
        case selector::FieldCallExpr:
            return AnalyzeStructFieldCallExpr(expr);
        case selector::FnCallExpr:
            return AnalyzeFnCallExpr(expr);
        case selector::BinaryExpr:
            return AnalyzeBinopExpr(expr);
        case selector::UnaryExpr:
            return AnalyzeUnaryExpr(expr);
        case selector::InitListExpr:
            return AnalyzeInitListExpr(expr);
        default:
            throw;
        }
    }

    std::vector<Command> Semantics::AnalyzeVarInit(const Statement& var_init_stmt)
    {
        if (not var_init_stmt.option_is_VarInit()) { throw; }
        
        const auto& view = var_init_stmt.view_VarInit();
        
        if (NameExists(view.var_name()))
        {
            throw;
        }

        if (not NameExists(view.type_name()) && not IsPrimitive(view.type_name()))
        {
            throw;
        }

        if (not CanBeAssigned(ObjectKind{
            IsPrimitive(view.type_name()), view.type_name() },
            AnalyzeExpression(view.init_expr())
        ))
        {
            throw;
        }

        if (view.init_expr().option_is_VarName())
        {
            std::cout << "Took reference\n";
            // dsu_COW.Add(view.var_name());
            // dsu_COW.Union(view.var_name(), view.init_expr().view_VarName().name());
        }

        std::vector <Command> cmds;

        AddVariable(var_init_stmt);

        /* Here Construct and Add Commands */
        std::cout << "VarInit Command ready\n";

        return cmds;
    }

    std::vector<Command> Semantics::AnalyzeArrayInit(const Statement& array_init_stmt)
    {
        if (not array_init_stmt.option_is_ArrayInit()) { throw; }

        const auto& view = array_init_stmt.view_ArrayInit();
        return {};
    }

    std::vector<Command> Semantics::AnalyzeWhileLoop(const Statement& while_loop_stmt)
    {
        if (not while_loop_stmt.option_is_WhileLoop()) { throw; }

        const auto& view = while_loop_stmt.view_WhileLoop();

        if (not CanBeAssigned(ObjectKind{ true, "bool" }, AnalyzeExpression(view.cond())))
        {
            throw;
        }

        std::vector <Command> cmds;

        AddCommands(cmds, std::move(AnalyzeBlock(view.whileloop_body())));

        std::cout << "WhileLoop Command ready\n";

        return cmds;
    }

    std::vector<Command> Semantics::AnalyzeForLoop(const Statement& for_stmt)
    {
        if (not for_stmt.option_is_ForLoop())
        {
            throw;
        }

        const auto& view = for_stmt.view_ForLoop();

        std::vector <Command> cmds;

        AddCommands(cmds, std::move(AnalyzeVarInit(view.init_part())));
        // Add While Loop Command;
    }

    std::vector<Command> Semantics::AnalyzeBlock(const Statement& block_stmt)
    {
        return std::vector<Command>();
    }

}