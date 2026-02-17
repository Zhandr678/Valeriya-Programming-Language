#include "Semantics.h"
#include "SemanticException.h"

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
                AnalyzeVarInit(seq.statements(i));
                break;
            case selector::ArrayInitStmt:
                AnalyzeArrayInit(seq.statements(i));
                break;
            case selector::WhileLoopStmt:
                AnalyzeWhileLoop(seq.statements(i));
                break;
            case selector::MakeStructStmt:
                AnalyzeMakeStruct(seq.statements(i));
                break;
            case selector::MakeEnumStmt:
                AnalyzeMakeEnum(seq.statements(i));
                break;
            case selector::MakePropertyStmt:
                AnalyzeMakeProperty(seq.statements(i));
                break;
            case selector::MatchStmt:
                AnalyzeMatch(seq.statements(i));
                break;
            case selector::ForLoopStmt:
                AnalyzeForLoop(seq.statements(i));
                break;
            case selector::ConditionStmt:
                AnalyzeCondition(seq.statements(i));
                break;
            case selector::MakeFunctionStmt:
                AnalyzeMakeFunction(seq.statements(i));
                break;
            case selector::AssignmentStmt:
                AnalyzeAssignment(seq.statements(i));
                break;
            case selector::BlockOfStmt:
                AnalyzeBlock(seq.statements(i));
                break;
            case selector::ExprCallStmt:
                AnalyzeExprCall(seq.statements(i));
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
        return std::string("1");
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

        std::unordered_set <std::string> has;
        for (size_t i = 0; i < view.size(); i++)
        {
            const auto& param_view = view.params(i).view_FnArgs();

            if (has.contains(param_view.var_name())) { throw; }
            has.insert(param_view.var_name());

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
                ftable.order_param.push_back(param_view.var_name());
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

    bool Semantics::NameExists(const std::string& name) const
    {
        return (symbol_table.contains(name) || type_table.contains(name) 
            || fn_table.contains(name)) && (active.contains(name));
    }

    bool Semantics::TypeExists(const std::string& name) const
    {
        return IsPrimitive(name) || type_table.contains(name);
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
        if (std::holds_alternative <ObjectKind>(from) && 
            std::get <ObjectKind>(from).type_name == "void") { return true; }

        if (to.index() != from.index()) { return false; }

        if (std::holds_alternative <ObjectKind>(to))
        {
            // std::cout << std::get <ObjectKind>(to).type_name << " " << std::get <ObjectKind>(from).type_name;
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
            if (not std::get <ArrayKind>(to).is_dynamic)
            {
                return std::stoi(std::get <ArrayKind>(to).valid_size_expression) >= std::stoi(std::get <ArrayKind>(from).valid_size_expression);
            }

            return CanBeAssigned(std::get <ArrayKind>(to).of_kind, std::get <ArrayKind>(from).of_kind);
        }

        else 
        {
            return false;
        }
    }

    // done
    VariableKind Semantics::AnalyzeVarNameExpr(const Expression& varname_expr)
    {
        if (not varname_expr.option_is_VarName()) 
        { 
            throw SemanticException("Unexpected Option " + varname_expr.sel(), filename, VarNameExpr, 0); 
        }
        const auto& view_varname = varname_expr.view_VarName();

        if (not symbol_table.contains(view_varname.name())) 
        {
            throw SemanticException("Variable " + view_varname.name() + " does not Exist", filename, VarNameExpr, 0);
        }
        return symbol_table.at(view_varname.name());
    }

    // done
    VariableKind Semantics::AnalyzeArrayCallExpr(const Expression& array_call)
    {
        if (not array_call.option_is_ArrayIndex()) 
        {
            throw SemanticException("Unexpected Option " + array_call.sel(), filename, ArrayIndexExpr, 0);
        }
        const auto& view_arr_call = array_call.view_ArrayIndex();
        
        auto at_expr = AnalyzeExpression(view_arr_call.at());
        if (std::holds_alternative <ArrayKind>(at_expr))
        {
            throw SemanticException("The Bracket Expression Must be Evaluated to int, not ArrayKind", filename, ArrayIndexExpr, 0); // at must be uint
        }

        if (not std::get <ObjectKind>(at_expr).is_primitive)
        {
            throw SemanticException("The Bracket Expression Must be Evaluated to int, not " + std::get <ObjectKind>(at_expr).type_name, filename, ArrayIndexExpr, 0); // at must be uint
        }

        if (std::get <ObjectKind>(at_expr).type_name != "uint" && std::get <ObjectKind>(at_expr).type_name != "int")
        {
            throw SemanticException("The Bracket Expression Must be Evaluated to int, not " + std::get <ObjectKind>(at_expr).type_name, filename, ArrayIndexExpr, 0); // at must be uint, or int >= 0 smh
        }

        auto arr_call = AnalyzeExpression(view_arr_call.array_expr());

        if (not std::holds_alternative <ArrayKind>(arr_call))
        {
            throw SemanticException("A Subscripted Value is not Array, but " + std::get <ObjectKind>(arr_call).type_name, filename, ArrayIndexExpr, 0);
        }

        return std::get <ArrayKind>(arr_call).of_kind;
    }

    // done
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
            return ObjectKind{ true, "string" };
        default:
            throw;
        }
    }

    // done
    VariableKind Semantics::AnalyzeStructFieldCallExpr(const Expression& field_call)
    {
        if (not field_call.option_is_FieldCall()) 
        {
            throw SemanticException("Unexpected Option " + field_call.sel(), filename, FieldCallExpr, 0);
        }

        const auto& view_fcall = field_call.view_FieldCall();
        auto kind_of_caller = AnalyzeExpression(view_fcall.caller());

        if (not view_fcall.field().option_is_VarName() && not view_fcall.field().option_is_ArrayIndex())
        {
            throw SemanticException("A Field Must be a Name", filename, FieldCallExpr, 0);
        }

        std::string name_of_field = view_fcall.field().option_is_VarName() ? 
            view_fcall.field().view_VarName().name() : view_fcall.field().view_ArrayIndex().array_expr().view_VarName().name();
        if (std::holds_alternative <ArrayKind>(kind_of_caller))
        {
            throw SemanticException("Array Fields Cannot be Accessed by Field Name", filename, FieldCallExpr, 0); // arrays cannot be accessed by field name
        }

        const auto& struct_kind = std::get <ObjectKind>(kind_of_caller);
        if (struct_kind.is_primitive) 
        { 
            throw SemanticException("Primitive Types Cannot be Accessed by Field Name", filename, FieldCallExpr, 0); // primitive type cannot be accessed by field name
        }

        if (not type_table.contains(struct_kind.type_name))
        {
            throw SemanticException("A Type " + struct_kind.type_name + " does not Exist", filename, FieldCallExpr, 0); // Type does not exists
        }

        if (not std::holds_alternative <StructType>(type_table.at(struct_kind.type_name)))
        {
            throw SemanticException("A Type " + struct_kind.type_name + " is not a Struct Type", filename, FieldCallExpr, 0); // field call on non-ADT
        }
        
        if (not std::get <StructType>(type_table.at(struct_kind.type_name)).fields.contains(name_of_field))
        {
            throw SemanticException("A Field " + name_of_field + " does not Exist in " + struct_kind.type_name, filename, FieldCallExpr, 0); // no such field
        }

        if (view_fcall.field().option_is_ArrayIndex())
        {
            return std::get <ArrayKind>(std::get <StructType>(type_table.at(struct_kind.type_name)).fields.at(name_of_field)).of_kind;
        }

        return std::get <StructType>(type_table.at(struct_kind.type_name)).fields.at(name_of_field);
    }

    // check with match
    VariableKind Semantics::AnalyzePropertyFieldCallExpr(const Expression& field_call, const std::string& active_option)
    {
        if (not field_call.option_is_FieldCall()) 
        { 
            throw SemanticException("Unexpected Option " + field_call.sel(), filename, FieldCallExpr, 0); 
        }
        const auto& view_fcall = field_call.view_FieldCall();

        if (not view_fcall.field().option_is_VarName())
        {
            throw SemanticException("A Field Must be a Name", filename, FieldCallExpr, 0); // field must be a name
        }

        std::string name_of_field = view_fcall.field().view_VarName().name();
        
        auto kind_of_caller = AnalyzeExpression(view_fcall.caller());
        if (std::holds_alternative <ArrayKind>(kind_of_caller))
        {
            throw SemanticException("Array Fields Cannot be Accessed by Field Name", filename, FieldCallExpr, 0); // arrays cannot be accessed by field name
        }

        const auto& prop_kind = std::get <ObjectKind>(kind_of_caller);
        if (prop_kind.is_primitive)
        {
            throw SemanticException("Primitive Types Cannot be Accessed by Field Name", filename, FieldCallExpr, 0); // primitive type cannot be accessed by field name
        }

        if (not std::holds_alternative <PropertyType>(type_table.at(prop_kind.type_name)))
        {
            throw SemanticException("A Type " + prop_kind.type_name + " is not a Struct Type", filename, FieldCallExpr, 0); // field call on non-ADT
        }

        if (not std::get <PropertyType>(type_table.at(prop_kind.type_name)).options.contains(active_option))
        {
            throw SemanticException("An Option " + active_option + " does not Exist in " + prop_kind.type_name, filename, FieldCallExpr, 0); // unknown option for property std::get <std::shared_ptr <ObjectKind>>(kind_of_caller)->type_name;
        }

        return std::get <PropertyType>(type_table.at(prop_kind.type_name)).options.at(active_option).fields.at(name_of_field);
    }

    // done
    VariableKind Semantics::AnalyzeFnCallExpr(const Expression& fn_call)
    {
        if (not fn_call.option_is_FnCall()) 
        { 
            throw SemanticException("Unexpected Option " + fn_call.sel(), filename, FnCallExpr, 0);
        }

        const auto& view_fncall = fn_call.view_FnCall();

        if (not fn_table.contains(view_fncall.fn_name())) 
        { 
            throw SemanticException("Function " + view_fncall.fn_name() + " does not Exist", filename, FnCallExpr, 0);
        }

        if (fn_table.at(view_fncall.fn_name()).order_param.size() != view_fncall.size())
        {
            throw SemanticException("Unexpected Number of Arguments for " + view_fncall.fn_name(), filename, FnCallExpr, 0);
        }

        for (size_t i = 0; i < view_fncall.size(); i++)
        {
            if (not CanBeAssigned( 
                fn_table.at(view_fncall.fn_name()).param_symbol_table.at(fn_table.at(view_fncall.fn_name()).order_param[i]).kind,
                AnalyzeExpression(view_fncall.args(i)))
            ) {
                throw SemanticException("Expression at position " + std::to_string(i) + " does not Match with " + view_fncall.fn_name() + "'s Arguments", filename, FnCallExpr, 0);
            }
        }

        VerifyInoutUniqueness(fn_call);
        return ObjectKind{ IsPrimitive(fn_table.at(view_fncall.fn_name()).ret_type_name), fn_table.at(view_fncall.fn_name()).ret_type_name};
    }

    static bool IsNumeric(const ObjectKind& obj_kind)
    {
        return obj_kind.type_name == "double" || obj_kind.type_name == "int" || obj_kind.type_name == "uint";
    }

    static bool IsNull(const VariableKind& kind)
    {
        return std::holds_alternative <ObjectKind>(kind) && std::get <ObjectKind>(kind).type_name == "void";
    }

    // done
    VariableKind Semantics::AnalyzeBinopExpr(const Expression& binop_expr)
    {
        if (not binop_expr.option_is_Binary()) 
        {
            throw SemanticException("Unexpected Option " + binop_expr.sel(), filename, BinaryExpr, 0);
        }

        const auto& view_binop = binop_expr.view_Binary();

        auto left_kind = AnalyzeExpression(view_binop.lhs());
        auto right_kind = AnalyzeExpression(view_binop.rhs());

        if (IsNull(left_kind) && not IsNull(right_kind))
        {
            return right_kind;
        }
        else if (not IsNull(left_kind) && IsNull(right_kind))
        {
            return left_kind;
        }
        else if (IsNull(left_kind) && IsNull(right_kind))
        {
            return left_kind;
        }

        if (left_kind.index() != right_kind.index())
        {
            if (view_binop.op() != "-")
            {
                throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
            }

            if (std::holds_alternative <ArrayKind>(left_kind) && std::holds_alternative <ObjectKind>(right_kind))
            {
                if (std::get <ObjectKind>(right_kind).type_name != "uint" && std::get <ObjectKind>(right_kind).type_name != "int")
                {
                    throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
                }

                return left_kind;
            }

            if (std::holds_alternative <ArrayKind>(right_kind) && std::holds_alternative <ObjectKind>(left_kind))
            {
                if (std::get <ObjectKind>(left_kind).type_name != "uint" && std::get <ObjectKind>(left_kind).type_name != "int")
                {
                    throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
                }

                return right_kind;
            }

            throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
        }

        if (view_binop.op() == "==" || view_binop.op() == "!=")
        {
            if (not CanBeAssigned(left_kind, right_kind))
            {
                throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
            }

            return ObjectKind{ true, "bool" };
        }

        else if (view_binop.op() == ">" || view_binop.op() == "<" || view_binop.op() == "<=" || view_binop.op() == ">=")
        {
            if (std::holds_alternative <ArrayKind>(left_kind))
            {
                throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
            }

            if (not IsNumeric(std::get <ObjectKind>(left_kind)) ||
                not IsNumeric(std::get <ObjectKind>(right_kind)))
            {
                throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
            }

            return ObjectKind{ true, "bool" };
        }

        else if (view_binop.op() == "+" || view_binop.op() == "-" || view_binop.op() == "/")
        {
            if (std::holds_alternative <ArrayKind>(left_kind))
            {
                return left_kind;
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

            throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
        }

        else if (view_binop.op() == "*" || view_binop.op() == "**")
        {
            if (std::holds_alternative <ArrayKind>(left_kind))
            {
                throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
            }

            if (IsNumeric(std::get <ObjectKind>(left_kind)) && IsNumeric(std::get <ObjectKind>(right_kind)))
            {
                return ObjectKind{ true, "double" };
            }

            throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
        }

        else if (view_binop.op() == "%")
        {
            if (std::holds_alternative <ArrayKind>(left_kind))
            {
                throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
            }

            if (std::get <ObjectKind>(left_kind).type_name == "uint" || std::get <ObjectKind>(left_kind).type_name == "int" &&
                std::get <ObjectKind>(right_kind).type_name == "uint" || std::get <ObjectKind>(right_kind).type_name == "int")
            {
                return ObjectKind{ true, "int" };
            }

            throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
        }

        else if (view_binop.op() == "&&" || view_binop.op() == "||")
        {
            if (std::holds_alternative <ArrayKind>(left_kind))
            {
                throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
            }

            if (std::get <ObjectKind>(left_kind).type_name == "bool" && std::get <ObjectKind>(right_kind).type_name == "bool")
            {
                return ObjectKind{ true, "bool" };
            }

            throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
        }

        else if (view_binop.op() == "&" || view_binop.op() == "|" || view_binop.op() == "^" || view_binop.op() == "<<" || view_binop.op() == ">>")
        {
            if (std::holds_alternative <ArrayKind>(left_kind))
            {
                throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
            }
            
            if ((std::get <ObjectKind>(left_kind).type_name == "uint" || std::get <ObjectKind>(left_kind).type_name == "int") && 
                (std::get <ObjectKind>(right_kind).type_name == "uint" || std::get <ObjectKind>(right_kind).type_name == "int"))
            {
                return ObjectKind{ true, "uint" };
            }

            throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
        }

        throw SemanticException("Unexpected Operands for Operator " + view_binop.op(), filename, BinaryExpr, 0);
    }

    // done
    VariableKind Semantics::AnalyzeUnaryExpr(const Expression& unary_expr)
    {
        if (not unary_expr.option_is_Unary()) 
        {
            throw SemanticException("Unexpected Option " + unary_expr.sel(), filename, UnaryExpr, 0);
        }

        const auto& view_unary = unary_expr.view_Unary();

        if (view_unary.op() == "-")
        {
            auto expr = AnalyzeExpression(view_unary.expr());
            if (CanBeAssigned(ObjectKind{ true, "int" }, expr))
            {
                return ObjectKind{ true, "int" };
            }
            else if (CanBeAssigned(ObjectKind{ true, "double" }, expr))
            {
                return ObjectKind{ true, "double" };
            }
            
            throw SemanticException("Cannot Have '-' on Non Numerical Types", filename, UnaryExpr, 0);
        }
        else if (view_unary.op() == "!")
        {
            if (CanBeAssigned(ObjectKind{ true, "bool" }, AnalyzeExpression(view_unary.expr())))
            {
                return ObjectKind{ true, "bool" };
            }
            
            throw SemanticException("Cannot Have '!' on Non Boolean Types", filename, UnaryExpr, 0);
        }

        throw SemanticException("Unknown Unary Operator", filename, UnaryExpr, 0);
    }

    // done
    VariableKind Semantics::AnalyzeInitListExpr(const Expression& init_list_expr)
    {
        if (not init_list_expr.option_is_InitList()) 
        { 
            throw SemanticException("Unexpected Option " + init_list_expr.sel(), filename, InitListExpr, 0);
        }

        const auto& view_inlist = init_list_expr.view_InitList();

        if (view_inlist.size() == 0) { return ObjectKind{ true, "void" }; }

        auto base_kind = AnalyzeExpression(view_inlist.exprs(0));

        for (size_t i = 1; i < view_inlist.size(); i++)
        {
            if (not CanBeAssigned(base_kind, AnalyzeExpression(view_inlist.exprs(i))))
            {
                throw SemanticException("Array Types Differ", filename, InitListExpr, 0);
            }
        }

        return ArrayKind{ std::to_string(view_inlist.size()), false, std::get <ObjectKind>(base_kind) };
    }

    // done
    VariableKind Semantics::AnalyzeStructInitExpr(const Expression& struct_init_expr)
    {
        if (not struct_init_expr.option_is_StructInit()) 
        {
            throw SemanticException("Unexpected Option " + struct_init_expr.sel(), filename, StructInitExpr, 0);
        }
        
        const auto& view = struct_init_expr.view_StructInit();
        
        if (not type_table.contains(view.struct_name()))
        {
            throw SemanticException("A Type " + view.struct_name() + " does not Exist", filename, StructInitExpr, 0);
        }
        if (not std::holds_alternative <StructType>(type_table.at(view.struct_name())))
        {
            throw SemanticException("A Type " + view.struct_name() + " is not a Struct Type", filename, StructInitExpr, 0);
        }
        
        for (size_t i = 0; i < view.size(); i++)
        {
            if (not CanBeAssigned( 
                std::get <StructType>(type_table.at(view.struct_name())).fields.at(std::get <StructType>(type_table.at(view.struct_name())).order_fields[i]),
                AnalyzeExpression(view.inits(i))))
            {
                throw SemanticException("Expression at position " + std::to_string(i) + " does not Match with Type " + std::get <StructType>(type_table.at(view.struct_name())).order_fields[i] + "'s Arguments", filename, StructInitExpr, 0);;
            }
        }

        return ObjectKind{ IsPrimitive(struct_init_expr.view_StructInit().struct_name()), struct_init_expr.view_StructInit().struct_name() };
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
        case selector::StructInitExpr:
            return AnalyzeStructInitExpr(expr);
        default:
            throw SemanticException("Unknown Expression", filename, StructInitExpr, 0);
        }
    }

    // done
    void Semantics::AnalyzeExprCall(const Statement& expr_call_stmt)
    {
        if (not expr_call_stmt.option_is_ExprCall()) 
        {
            throw SemanticException("Unexpected Option " + expr_call_stmt.sel(), filename, ExprCallStmt, 0);
        }

        AnalyzeExpression(expr_call_stmt.view_ExprCall().expr());
    }

    void Semantics::AnalyzeVarInit(const Statement& var_init_stmt)
    {
        if (not var_init_stmt.option_is_VarInit()) 
        {
            throw SemanticException("Unexpected Option " + var_init_stmt.sel(), filename, VarInitStmt, 0);
        }
        
        const auto& view = var_init_stmt.view_VarInit();
        
        if (NameExists(view.var_name()))
        {
            throw SemanticException("Variable " + view.var_name() + " Already Declared", filename, VarInitStmt, 0);
        }

        if (not NameExists(view.type_name()) && not IsPrimitive(view.type_name()))
        {
            throw SemanticException("A Type " + view.type_name() + " does not Exist", filename, VarInitStmt, 0);
        }

        if (not CanBeAssigned(ObjectKind{
            IsPrimitive(view.type_name()), view.type_name() },
            AnalyzeExpression(view.init_expr())
        ))
        {
            throw SemanticException("Expression does not Deduce to Type " + view.type_name(), filename, VarInitStmt, 0);
        }

        /* Handle Option is LVALUE */

        AddVariable(var_init_stmt);

        /* Here Construct and Add Commands */
        std::cout << "VarInit Command ready\n";
    }

    void Semantics::AnalyzeArrayInit(const Statement& array_init_stmt)
    {
        if (not array_init_stmt.option_is_ArrayInit()) { throw; }

        const auto& view = array_init_stmt.view_ArrayInit();

        if (NameExists(view.type_info().view_VarInit().var_name())) { throw; }

        if (not NameExists(view.type_info().view_VarInit().type_name()) && 
            not IsPrimitive(view.type_info().view_VarInit().type_name()))
        {
            throw;
        }

        if (not CanBeAssigned(ArrayKind{
            ExprToStr(view.alloc_size()), 
            view.is_dynamic(),
            ObjectKind{ 
                IsPrimitive(view.type_info().view_VarInit().type_name()), 
                view.type_info().view_VarInit().type_name()
                }
            },
            AnalyzeExpression(view.init_expr())
            ))
        {
            throw;
        }

        /* Handle Option is LVALUE */

        AddVariable(array_init_stmt);

        std::cout << "Array Init Ready\n";
    }

    void Semantics::AnalyzeWhileLoop(const Statement& while_loop_stmt)
    {
        if (not while_loop_stmt.option_is_WhileLoop()) { throw; }

        const auto& view = while_loop_stmt.view_WhileLoop();

        if (not CanBeAssigned(ObjectKind{ true, "bool" }, AnalyzeExpression(view.cond())))
        {
            throw;
        }

        std::cout << "WhileLoop Command ready\n";
    }

    void Semantics::AnalyzeForLoop(const Statement& for_stmt)
    {
        if (not for_stmt.option_is_ForLoop())
        {
            throw;
        }

        const auto& view = for_stmt.view_ForLoop();
    }

    void Semantics::AnalyzeFnBody(const Statement& fn_stmt)
    {
    }

    void Semantics::AnalyzeFnParams(const Statement& fn_param_stmt)
    {

    }

    void Semantics::AnalyzeMakeFunction(const Statement& make_fn_stmt)
    {
        if (not make_fn_stmt.option_is_MakeFunction()) { throw; }

        const auto& view = make_fn_stmt.view_MakeFunction();

        if (NameExists(view.fn_name())) { throw; }

        if (not TypeExists(view.ret_type_name())) { throw; }

        for (size_t i = 0; i < view.size(); i++)
        {
            AnalyzeFnParams(view.params(i));
        }

        // Deactivate All?

        AnalyzeFnBody(make_fn_stmt);

        // Activate All?

        AddFn(view.fn_name(), ConstructFn(make_fn_stmt));

        std::cout << "Make Fn Ready\n";
    }

    void Semantics::AnalyzeMakeStruct(const Statement& make_struct_stmt)
    {
        if (not make_struct_stmt.option_is_MakeStruct()) { throw; }

        const auto& view = make_struct_stmt.view_MakeStruct();

        if (NameExists(view.struct_name()))
        {
            throw;
        }

        std::unordered_set <std::string> has;
        for (size_t i = 0; i < view.size(); i++)
        {
            if (view.inits(i).option_is_VarInit()) {
                const auto& field_view = view.inits(i).view_VarInit();

                if (has.find(field_view.var_name()) != has.end()) 
                {
                    throw;
                }
                has.insert(field_view.var_name());

                if (not TypeExists(field_view.type_name()) && field_view.type_name() != view.struct_name()) { throw; }

                if (field_view.init_expr().option_is_EmptyLiteral()) { continue; }
                if (not CanBeAssigned(ObjectKind{ IsPrimitive(field_view.type_name()), field_view.type_name() }, AnalyzeExpression(field_view.init_expr())))
                {
                    throw;
                }
            }
            else {
                const auto& field_view = view.inits(i).view_ArrayInit();

                if (has.find(field_view.type_info().view_VarInit().var_name()) != has.end()) { throw; }
                has.insert(field_view.type_info().view_VarInit().var_name());

                if (not TypeExists(field_view.type_info().view_VarInit().type_name())) { throw; }

                if (field_view.init_expr().option_is_EmptyLiteral()) { continue; }
                if (not CanBeAssigned(
                    ArrayKind{ ExprToStr(field_view.alloc_size()), field_view.is_dynamic(),
                    ObjectKind{ IsPrimitive(field_view.type_info().view_VarInit().type_name()), field_view.type_info().view_VarInit().type_name() } }, 
                        AnalyzeExpression(field_view.init_expr())))
                {
                    throw;
                }
            }
        }

        AddType(view.struct_name(), std::move(ConstructStructType(make_struct_stmt)));

        std::cout << "Make Struct Command Ready\n";
    }

    void Semantics::AnalyzePropertyOption(const Statement& prop_opt_stmt, const std::string& prop_name)
    {
    }

    void Semantics::AnalyzeMakeProperty(const Statement& make_prop_stmt)
    {
        if (not make_prop_stmt.option_is_MakeProperty()) { throw; }

        const auto& view = make_prop_stmt.view_MakeProperty();

        if (NameExists(view.prop_name())) { throw; }

        for (size_t i = 0; i < view.size(); i++)
        {
            AnalyzePropertyOption(view.opts(i), view.prop_name());
        }

        AddType(view.prop_name(), std::move(ConstructPropertyType(make_prop_stmt)));

        std::cout << "Property Command Ready\n";
    }

    void Semantics::AnalyzeMakeEnum(const Statement& make_enum_stmt)
    {
        if (not make_enum_stmt.option_is_MakeEnum()) { throw; }

        const auto& view = make_enum_stmt.view_MakeEnum();

        if (NameExists(view.enum_name())) { throw; }

        std::unordered_set <std::string> has;
        for (size_t i = 0; i < view.size(); i++)
        {
            if (has.find(view.enum_variants(i)) != has.end()) { throw; }
            has.insert(view.enum_variants(i));
        }

        AddType(view.enum_name(), ConstructEnumType(make_enum_stmt));

        std::cout << "Enum Command Ready\n";
        /* Here create enum command */
    }

    void Semantics::AnalyzeCaseBody(const Statement& case_block_stmt, const std::string& active_option)
    {
        if (not case_block_stmt.option_is_Block()) { throw; }


    }

    void Semantics::AnalyzeCaseClause(const Statement& case_clause_stmt, const Expression& match_expr)
    {
        if (not case_clause_stmt.option_is_CaseClause()) { throw; }

        const auto& view = case_clause_stmt.view_CaseClause();

        if (view.is_wildcard())
        {
            AnalyzeBlock(view.case_body());
        }
        else
        {
            AnalyzeCaseBody(view.case_body(), view.case_expr().view_VarName().name());
        }

        std::cout << "Case Clause (" << view.case_expr().view_VarName().name() << ") Ready\n";
    }

    void Semantics::AnalyzeMatch(const Statement& match_stmt)
    {
        if (not match_stmt.option_is_Match()) { throw; }

        const auto& view = match_stmt.view_Match();

        auto matched_expr_kind = AnalyzeExpression(view.matched_expr());

        if (std::holds_alternative <ArrayKind>(matched_expr_kind)) { throw; }

        if (not std::holds_alternative <EnumType>(type_table.at(std::get <ObjectKind>(matched_expr_kind).type_name)) &&
            not std::holds_alternative <PropertyType>(type_table.at(std::get <ObjectKind>(matched_expr_kind).type_name)))
        {
            throw;
        }

        for (size_t i = 0; i < view.size(); i++)
        {
            AnalyzeCaseClause(view.cases(i), view.matched_expr());
        }

        std::cout << "Match Command Ready\n";
    }

    void Semantics::AnalyzeAssignment(const Statement& assign_stmt)
    {
        if (not assign_stmt.option_is_Assignment()) { throw; }

        const auto& view = assign_stmt.view_Assignment();

        if (not CanBeAssigned(AnalyzeExpression(view.dest()), AnalyzeExpression(view.expr())))
        {
            throw;
        }

        std::cout << "Assignment Command Ready\n";
    }

    void Semantics::AnalyzeElif(const Statement& elif_stmt)
    {
        if (not elif_stmt.option_is_ElifCondition()) { throw; }

        const auto& view = elif_stmt.view_ElifCondition();

        if (not CanBeAssigned(ObjectKind{ true, "bool" }, AnalyzeExpression(view.elif_cond())))
        {
            throw;
        }

        AnalyzeBlock(view.elif_body());

        std::cout << "Elif Body Ready\n";
    }

    void Semantics::AnalyzeCondition(const Statement& cond_stmt)
    {
        if (not cond_stmt.option_is_Condition()) { throw; }

        const auto& view = cond_stmt.view_Condition();

        if (not CanBeAssigned(ObjectKind{ true, "bool" }, AnalyzeExpression(view.if_cond())))
        {
            throw;
        }

        AnalyzeBlock(view.if_body());

        for (size_t i = 0; i < view.size(); i++)
        {
            AnalyzeElif(view.elif_stmt(i));
        }

        if (not view.else_body().option_is_Empty())
        {
            AnalyzeBlock(view.else_body());
        }

        std::cout << "Condition Ready\n";
    }

    void Semantics::AnalyzeBlock(const Statement& block_stmt)
    {
    }

}