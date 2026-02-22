#include "Semantics.h"
#include "SemanticException.h"

namespace val
{
    static int GetLine(const Statement& stmt) noexcept
    {
        switch (stmt.sel())
        {
        case selector::VarInitStmt:
            return stmt.view_VarInit().line();
        case selector::ArrayInitStmt:
			return stmt.view_ArrayInit().line();
		case selector::WhileLoopStmt:
			return stmt.view_WhileLoop().line();
		case selector::MakeStructStmt:
			return stmt.view_MakeStruct().line();
		case selector::MakeEnumStmt:
			return stmt.view_MakeEnum().line();
        case selector::MakePropertyStmt:
			return stmt.view_MakeProperty().line();
		case selector::MatchStmt:
			return stmt.view_Match().line();
		case selector::ForLoopStmt:
			return stmt.view_ForLoop().line();
		case selector::ConditionStmt:
			return stmt.view_Condition().line();
		case selector::MakeFunctionStmt:
			return stmt.view_MakeFunction().line();
		case selector::AssignmentStmt:
			return stmt.view_Assignment().line();
		case selector::BlockOfStmt:
			return stmt.view_Block().line();
		case selector::ExprCallStmt:
			return stmt.view_ExprCall().line();
        case selector::ElifConditionStmt:
			return stmt.view_ElifCondition().line();
        case selector::FnArgsStmt:
			return stmt.view_FnArgs().line();
        case selector::CaseClauseStmt:
			return stmt.view_CaseClause().line();
		case selector::ContinueStmt:
			return stmt.view_Continue().line();
		case selector::BreakStmt:
			return stmt.view_Break().line();
		case selector::ReturnStmt:
			return stmt.view_Return().line();
		case selector::EmptyStmt:
			return stmt.view_Empty().line();
        }
    }

    static void AddCommands(std::vector <Command>& commands, std::vector <Command>&& more_cmds)
    {
        for (const Command& cmd : more_cmds)
        {
            commands.push_back(cmd);
        }
    }

	Semantics::Semantics(const std::string& filename) : filename(filename)
    {}

    std::vector<Command> Semantics::AnalyzePrepareCommands(const Statement& AST)
    {
        if (not AST.option_is_Block()) 
        {
            throw SemanticException("Unexpected Option " + AST.sel(), filename, BlockOfStmt, GetLine(AST));
        }

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
                AnalyzeWhileLoop(seq.statements(i), false, {});
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
                AnalyzeMatch(seq.statements(i), false, {});
                break;
            case selector::ForLoopStmt:
                AnalyzeForLoop(seq.statements(i), false, {});
                break;
            case selector::ConditionStmt:
                AnalyzeCondition(seq.statements(i), false, {});
                break;
            case selector::MakeFunctionStmt:
                AnalyzeMakeFunction(seq.statements(i));
                break;
            case selector::AssignmentStmt:
                AnalyzeAssignment(seq.statements(i));
                break;
            case selector::BlockOfStmt:
                AnalyzeBlock(seq.statements(i), false, {});
                break;
            case selector::ExprCallStmt:
                AnalyzeExprCall(seq.statements(i));
                break;
            default:
                throw SemanticException("Unexpected Statement", filename, BlockOfStmt, GetLine(seq.statements(i)));
            }
        }

        return commands;
    }

    std::string ExprToStr(const Expression& expr)
    {
        switch (expr.sel())
        {
        case selector::EmptyExpr:
            return "null";
        case selector::VarNameExpr:
            return expr.view_VarName().name();
        case selector::ArrayIndexExpr:
            return ExprToStr(expr.view_ArrayIndex().array_expr()) + "[" + ExprToStr(expr.view_ArrayIndex().at()) + "]";
        case selector::BoolLiteralExpr: 
            return expr.view_BoolLiteral().value() ? "true" : "false";
        case selector::CharLiteralExpr:
            return "'" + std::string(1, expr.view_CharLiteral().value()) + "'";
        case selector::DoubleLiteralExpr:
            return std::to_string(expr.view_DoubleLiteral().value());
        case selector::IntLiteralExpr: 
            return std::to_string(expr.view_IntLiteral().value());
        case selector::StringLiteralExpr:
            return expr.view_StringLiteral().value();
        case selector::FieldCallExpr:
            return ExprToStr(expr.view_FieldCall().caller()) + ExprToStr(expr.view_FieldCall().field());
        case selector::FnCallExpr:
        {
            std::string fn_call_str = expr.view_FnCall().fn_name() + "(";
            for (size_t i = 0; i < expr.view_FnCall().size(); i++)
            {
                fn_call_str += ExprToStr(expr.view_FnCall().args(i));
            }
            return fn_call_str + ")";
        }
        case selector::BinaryExpr:
            return ExprToStr(expr.view_Binary().lhs()) + expr.view_Binary().op() + ExprToStr(expr.view_Binary().rhs());
        case selector::UnaryExpr:
            return expr.view_Unary().op() + ExprToStr(expr.view_Unary().expr());
        case selector::InitListExpr:
        {
            std::string init_list_str = "{ ";
            for (size_t i = 0; i < expr.view_InitList().size(); i++)
            {
                init_list_str += ExprToStr(expr.view_InitList().exprs(i)) + ", ";
            }
            return init_list_str + "}";
        }
        case selector::StructInitExpr:
        {
            std::string struct_init_str = expr.view_StructInit().struct_name() + "{ ";
            for (size_t i = 0; i < expr.view_StructInit().size(); i++)
            {
                struct_init_str += (ExprToStr(expr.view_StructInit().inits(i))) + ", ";
            }
            return struct_init_str + "}";
        }
        default:
            return "null";
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

        if (view.is_array())
        {
            ftable.ret_kind = ArrayKind{ "", true, ObjectKind{ IsPrimitive(view.ret_type_name()), view.ret_type_name() }};
        }
        else {
            ftable.ret_kind = ObjectKind{ IsPrimitive(view.ret_type_name()), view.ret_type_name() };
        }

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
                ftable.param_symbol_table[param_view.var_name()] = 
                    { 
                        param_view.is_inout(),
                        std::move(arr_kind) 
                    };
            }
            else 
            {
                ftable.order_param.push_back(param_view.var_name());
                ftable.param_symbol_table[param_view.var_name()] = 
                    {
                        param_view.is_inout(),
                        ObjectKind{ IsPrimitive(param_view.type_name()), param_view.type_name() }
                    };
            }
        }

        return ftable;
    }

    bool Semantics::NameExists(const std::string& name) const noexcept
    {
        return (symbol_table.contains(name) || type_table.contains(name)
            || fn_table.contains(name)) && (active.contains(name) && (active.at(name)));
    }

    bool Semantics::TypeExists(const std::string& name) const noexcept
    {
        return IsPrimitive(name) || (type_table.contains(name) && (active.contains(name) && (active.at(name))));
    }

    void Semantics::AddVariable(const Statement& var_init) noexcept
    {
        if (var_init.option_is_VarInit())
        {
            const auto& view = var_init.view_VarInit();

            symbol_table[view.var_name()] = ObjectKind{ IsPrimitive(view.type_name()), view.type_name() };

            Activate(view.var_name());
        }
        else 
        {
            const auto& view = var_init.view_ArrayInit();
            symbol_table[view.type_info().view_VarInit().var_name()] = 
                ArrayKind{
                    ExprToStr(view.init_expr()),
                    view.is_dynamic(),
                    ObjectKind{ 
                        IsPrimitive(view.type_info().view_VarInit().type_name()), 
                        view.type_info().view_VarInit().type_name() 
                    }
                };

            Activate(view.type_info().view_VarInit().var_name());
        }
    }

    void Semantics::AddType(const std::string& type_name, TypeClass&& type) noexcept
    {
        type_table.insert({
            type_name,
            std::move(type)
        });

        Activate(type_name);
    }

    void Semantics::AddFn(const std::string& fn_name, FnTable&& ftable) noexcept
    {
        fn_table[fn_name] = std::move(ftable);

        Activate(fn_name);
    }

    void Semantics::Activate(const std::string& name) noexcept
    {
		active[name] = true;
    }

    void Semantics::Deactivate(const std::string& name) noexcept
    {
        active[name] = false;
    }

    bool Semantics::CanBeAssigned(const VariableKind& to, const VariableKind& from) noexcept
    {
        if (std::holds_alternative <ObjectKind>(from) && 
            std::get <ObjectKind>(from).type_name == "void") { return true; }

        if (to.index() != from.index()) { return false; }

        if (std::holds_alternative <ObjectKind>(to))
        {
            if (type_table.contains(std::get <ObjectKind>(from).type_name) &&
				type_table.contains(std::get <ObjectKind>(to).type_name) &&
                std::holds_alternative <StructType>(type_table.at(std::get <ObjectKind>(from).type_name)) &&
                std::holds_alternative <PropertyType> (type_table.at(std::get <ObjectKind>(to).type_name)) &&
                option_to_prop.contains(std::get <ObjectKind>(from).type_name) && 
                option_to_prop.at(std::get <ObjectKind>(from).type_name) == std::get <ObjectKind>(to).type_name
            )
            {
                return true;
            }

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

        if (not symbol_table.contains(view_varname.name()) || not active[view_varname.name()])
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
        auto if_property = ExprToStr(view_fcall.caller());
        auto kind_of_caller = (symbol_table.contains(if_property) && active[if_property]) ? symbol_table.at(if_property) : AnalyzeExpression(view_fcall.caller());

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

        if (not type_table.contains(struct_kind.type_name) || not active[struct_kind.type_name])
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

        return fn_table.at(view_fncall.fn_name()).ret_kind;
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
		if (view.size() != std::get <StructType>(type_table.at(view.struct_name())).order_fields.size())
        {
            throw SemanticException("Unexpected Number of Arguments for " + view.struct_name(), filename, StructInitExpr, 0);
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

    // done
    void Semantics::AnalyzeVarInit(const Statement& var_init_stmt)
    {
        if (not var_init_stmt.option_is_VarInit()) 
        {
            throw SemanticException("Unexpected Option " + var_init_stmt.sel(), filename, VarInitStmt, GetLine(var_init_stmt));
        }
        
        const auto& view = var_init_stmt.view_VarInit();
        
        if (NameExists(view.var_name()))
        {
            throw SemanticException("Variable " + view.var_name() + " Already Declared", filename, VarInitStmt, GetLine(var_init_stmt));
        }

        if (not NameExists(view.type_name()) && not IsPrimitive(view.type_name()))
        {
            throw SemanticException("A Type " + view.type_name() + " does not Exist", filename, VarInitStmt, GetLine(var_init_stmt));
        }

        if (not CanBeAssigned(ObjectKind{
            IsPrimitive(view.type_name()), view.type_name() },
            AnalyzeExpression(view.init_expr())
        ))
        {
            throw SemanticException("Expression does not Deduce to Type " + view.type_name(), filename, VarInitStmt, GetLine(var_init_stmt));
        }

        AddVariable(var_init_stmt);

        std::cout << "VarInit Command ready\n";
    }

    // done 
    void Semantics::AnalyzeArrayInit(const Statement& array_init_stmt)
    {
        if (not array_init_stmt.option_is_ArrayInit()) 
        {
            throw SemanticException("Unexpected Option " + array_init_stmt.sel(), filename, ArrayInitStmt, GetLine(array_init_stmt));
        }

        const auto& view = array_init_stmt.view_ArrayInit();

        if (NameExists(view.type_info().view_VarInit().var_name())) 
        {
			throw SemanticException("Variable " + view.type_info().view_VarInit().var_name() + " Already Declared", filename, ArrayInitStmt, GetLine(array_init_stmt));
        }

        if ((not NameExists(view.type_info().view_VarInit().type_name()) && 
            not IsPrimitive(view.type_info().view_VarInit().type_name())))
        {
			throw SemanticException("A Type " + view.type_info().view_VarInit().type_name() + " does not Exist", filename, ArrayInitStmt, GetLine(array_init_stmt));
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
			throw SemanticException("Expression does not Deduce to Array of Type " + view.type_info().view_VarInit().type_name(), filename, ArrayInitStmt, GetLine(array_init_stmt));
        }

        AddVariable(array_init_stmt);

        std::cout << "Array Init Ready\n";
    }

    bool Semantics::AnalyzeLoopBody(const Statement& block_stmt, bool should_return, const VariableKind& ret)
    {
        if (not block_stmt.option_is_Block())
        {
            throw SemanticException("Unexpected Option " + block_stmt.sel(), filename, BlockOfStmt, GetLine(block_stmt));
        }

        const auto& seq = block_stmt.view_Block();

        bool returns = false;

        for (size_t i = 0; i < seq.size(); i++)
        {
            switch (seq.statements(i).sel())
            {
            case selector::VarInitStmt:
                AnalyzeVarInit(seq.statements(i));
                block_allocated.push_back(seq.statements(i).view_VarInit().var_name());
                break;
            case selector::ArrayInitStmt:
                AnalyzeArrayInit(seq.statements(i));
                block_allocated.push_back(seq.statements(i).view_ArrayInit().type_info().view_VarInit().var_name());
                break;
            case selector::WhileLoopStmt:
                AnalyzeWhileLoop(seq.statements(i), should_return, ret);
                break;
            case selector::MakeStructStmt:
                throw SemanticException("MakeStruct is not Allowed Inside a Block", filename, MakeStructStmt, 0);
            case selector::MakeEnumStmt:
                throw SemanticException("MakeEnum is not Allowed Inside a Block", filename, MakeEnumStmt, 0);
            case selector::MakePropertyStmt:
                throw SemanticException("MakeProperty is not Allowed Inside a Block", filename, MakePropertyStmt, 0);
            case selector::MatchStmt:
            {
                bool res = AnalyzeMatch(seq.statements(i), should_return, ret);

                if (should_return)
                {
                    returns = res;
                }
                break;
            }
            case selector::ForLoopStmt:
                AnalyzeForLoop(seq.statements(i), should_return, ret);
                break;
            case selector::ConditionStmt:
            {
                bool res = AnalyzeCondition(seq.statements(i), should_return, ret);

                if (should_return)
                {
                    returns = res;
                }
                break;
            }
            case selector::MakeFunctionStmt:
                throw SemanticException("MakeFunction is not Allowed Inside a Block", filename, MakeFunctionStmt, GetLine(seq.statements(i)));
            case selector::AssignmentStmt:
                AnalyzeAssignment(seq.statements(i));
                break;
            case selector::BlockOfStmt:
            {
                bool res = AnalyzeBlock(seq.statements(i), should_return, ret);

                if (should_return)
                {
                    returns = res;
                }

                break;
            }
            case selector::ExprCallStmt:
                AnalyzeExprCall(seq.statements(i));
                break;
            case selector::ReturnStmt:
            {
                if (not should_return)
                {
                    throw SemanticException("Return Statement Outside Function Body", filename, ReturnStmt, GetLine(seq.statements(i)));
                }

                else if (not CanBeAssigned(ret, AnalyzeExpression(seq.statements(i).view_Return().return_expr())))
                {
                    throw SemanticException("Cannot Deduce Return Type to Function's", filename, ReturnStmt, GetLine(seq.statements(i)));
                }

                returns = true;
                break;
            }
            case selector::BreakStmt:
                break;
            case selector::ContinueStmt:
                break;
            default:
                throw SemanticException("Unexpected Statement", filename, BlockOfStmt, GetLine(block_stmt));
                break;
            }
        }

        for (const auto& var : block_allocated)
        {
            Deactivate(var);
        }
        block_allocated.clear();

        std::cout << "Block is ready\n";

        return returns;
    }

    void Semantics::AnalyzeWhileLoop(const Statement& while_loop_stmt, bool should_return, const VariableKind& ret)
    {
        if (not while_loop_stmt.option_is_WhileLoop()) 
        {
			throw SemanticException("Unexpected Option " + while_loop_stmt.sel(), filename, WhileLoopStmt, GetLine(while_loop_stmt));
        }

        const auto& view = while_loop_stmt.view_WhileLoop();

        if (not CanBeAssigned(ObjectKind{ true, "bool" }, AnalyzeExpression(view.cond())))
        {
			throw SemanticException("While Loop Condition Must be of Type bool", filename, WhileLoopStmt, GetLine(while_loop_stmt));
        }

        std::cout << "WhileLoop Command ready\n";

		AnalyzeLoopBody(view.whileloop_body(), should_return, ret);
    }

    void Semantics::AnalyzeFinalForLoopStmt(const Statement& final_for_stmt)
    {
        if (not final_for_stmt.option_is_Block())
        {
            throw SemanticException("Unexpected Option " + final_for_stmt.sel(), filename, ForLoopStmt, GetLine(final_for_stmt));
        }

        for (size_t i = 0; i < final_for_stmt.view_Block().size(); i++)
        {
            switch (final_for_stmt.view_Block().statements(i).sel())
            {
            case selector::AssignmentStmt:
                AnalyzeAssignment(final_for_stmt.view_Block().statements(i));
                break;
            case selector::ExprCallStmt:
                AnalyzeExprCall(final_for_stmt.view_Block().statements(i));
                break;
            default:
                throw SemanticException("Statement is not Allowed in For Loop", filename, ForLoopStmt, GetLine(final_for_stmt.view_Block().statements(i)));
            }
        }
    }

    void Semantics::AnalyzeForLoop(const Statement& for_stmt, bool should_return, const VariableKind& ret)
    {
        if (not for_stmt.option_is_ForLoop())
        {
            throw SemanticException("Unexpected Option " + for_stmt.sel(), filename, ForLoopStmt, GetLine(for_stmt));
        }

        const auto& view = for_stmt.view_ForLoop();

        if (view.init_part().option_is_Block())
        {
            for (size_t i = 0; i < view.init_part().view_Block().size(); i++)
            {
                if (view.init_part().view_Block().statements(i).option_is_VarInit())
                {
                    AnalyzeVarInit(view.init_part().view_Block().statements(i));
                    block_allocated.push_back(view.init_part().view_Block().statements(i).view_VarInit().var_name());
                }
                else if (view.init_part().view_Block().statements(i).option_is_ArrayInit())
                {
                    AnalyzeArrayInit(view.init_part().view_Block().statements(i));
                    block_allocated.push_back(view.init_part().view_Block().statements(i).view_ArrayInit().type_info().view_VarInit().var_name());
                }
            }
        }

        if (not view.check().option_is_EmptyLiteral() &&
            not CanBeAssigned(ObjectKind{true,"bool"}, AnalyzeExpression(view.check())))
        {
            throw SemanticException("For Loop's Check MUST Deduce to bool", filename, ForLoopStmt, GetLine(for_stmt));
        }

        if (view.final_expr().option_is_Block())
        {
            AnalyzeFinalForLoopStmt(view.final_expr());
        }

        AnalyzeLoopBody(view.forloop_body(), should_return, ret);
    }

    void Semantics::AnalyzeMakeFunction(const Statement& make_fn_stmt)
    {
        if (not make_fn_stmt.option_is_MakeFunction()) 
        {
			throw SemanticException("Unexpected Option " + make_fn_stmt.sel(), filename, MakeFunctionStmt, GetLine(make_fn_stmt));
        }

        const auto& view = make_fn_stmt.view_MakeFunction();

        if (NameExists(view.fn_name())) 
        {
			throw SemanticException(view.fn_name() + " Already Exists", filename, MakeFunctionStmt, GetLine(make_fn_stmt));
        }

        if (not TypeExists(view.ret_type_name())) 
        {
			throw SemanticException("A Type " + view.ret_type_name() + " does not Exist", filename, MakeFunctionStmt, GetLine(make_fn_stmt));
        }

		std::unordered_set <std::string> has;
        auto temp_st = std::move(symbol_table);
        std::vector <std::string> to_deactivate;
        symbol_table.clear();
        for (size_t i = 0; i < view.size(); i++)
        {
            if (not NameExists(view.params(i).view_FnArgs().type_name()) && not IsPrimitive(view.params(i).view_FnArgs().type_name()))
            {
				throw SemanticException("A Type " + view.params(i).view_FnArgs().type_name() + " does not Exist", filename, MakeFunctionStmt, GetLine(view.params(i)));
            }

            if (has.contains(view.params(i).view_FnArgs().var_name()))
            {
				throw SemanticException("Parameter " + view.params(i).view_FnArgs().var_name() + " Already Exists", filename, MakeFunctionStmt, GetLine(view.params(i)));
            }
			has.insert(view.params(i).view_FnArgs().var_name());

            if (view.params(i).view_FnArgs().is_array())
            {
                symbol_table[view.params(i).view_FnArgs().var_name()] =
                    ArrayKind{
                        std::to_string(view.params(i).view_FnArgs().array_info(0).second),
                        view.params(i).view_FnArgs().array_info(0).first,
                        ObjectKind{
                            IsPrimitive(view.params(i).view_FnArgs().type_name()),
                            view.params(i).view_FnArgs().type_name()
                        }
                    };
                active[view.params(i).view_FnArgs().var_name()] = true;
				to_deactivate.push_back(view.params(i).view_FnArgs().var_name());
            }
            else {
                symbol_table[view.params(i).view_FnArgs().var_name()] =
                    ObjectKind{
                            IsPrimitive(view.params(i).view_FnArgs().type_name()),
                            view.params(i).view_FnArgs().type_name()
                    };
                active[view.params(i).view_FnArgs().var_name()] = true;
				to_deactivate.push_back(view.params(i).view_FnArgs().var_name());
            }
        }

        bool returns;
        bool should_return;
        if (view.is_array())
        {
            should_return = true;
            returns = AnalyzeBlock(view.fn_body(), true, ArrayKind{ "", true, ObjectKind{ IsPrimitive(view.ret_type_name()), view.ret_type_name()}});
        }
        else if (view.ret_type_name() == "void") 
        {
            should_return = false;
            returns = AnalyzeBlock(view.fn_body(), false, ObjectKind{ true, "void" });
        }
        else {
            should_return = true;
            returns = AnalyzeBlock(view.fn_body(), true, ObjectKind{ IsPrimitive(view.ret_type_name()), view.ret_type_name() });
        }

        if (should_return && not returns)
        {
            throw SemanticException("Function " + view.fn_name() + " May not Return in All Paths", filename, MakeFunctionStmt, GetLine(make_fn_stmt));
        }

        for (const auto& str : to_deactivate)
        {
			active[str] = false;
        }

        symbol_table.clear();
        symbol_table = std::move(temp_st);
        AddFn(view.fn_name(), ConstructFn(make_fn_stmt));

        std::cout << "Make Fn Ready\n";
    }

    void Semantics::AnalyzeMakeStruct(const Statement& make_struct_stmt)
    {
        if (not make_struct_stmt.option_is_MakeStruct()) 
        {
			throw SemanticException("Unexpected Option " + make_struct_stmt.sel(), filename, MakeStructStmt, GetLine(make_struct_stmt));
        }

        const auto& view = make_struct_stmt.view_MakeStruct();

        if (NameExists(view.struct_name()))
        {
			throw SemanticException(view.struct_name() + " Already Exists", filename, MakeStructStmt, GetLine(make_struct_stmt));
        }

        std::unordered_set <std::string> has;
        for (size_t i = 0; i < view.size(); i++)
        {
            if (view.inits(i).option_is_VarInit()) {
                const auto& field_view = view.inits(i).view_VarInit();

                if (has.find(field_view.var_name()) != has.end()) 
                {
					throw SemanticException("Field " + field_view.var_name() + " Already Exists", filename, MakeStructStmt, GetLine(view.inits(i)));
                }
                has.insert(field_view.var_name());

                if (not TypeExists(field_view.type_name()) && field_view.type_name() != view.struct_name()) 
                {
					throw SemanticException("A Type " + field_view.type_name() + " does not Exist", filename, MakeStructStmt, GetLine(view.inits(i)));
                }

                if (field_view.init_expr().option_is_EmptyLiteral()) { continue; }
                if (not CanBeAssigned(ObjectKind{ IsPrimitive(field_view.type_name()), field_view.type_name() }, AnalyzeExpression(field_view.init_expr())))
                {
					throw SemanticException("Expression does not Deduce to Type " + field_view.type_name(), filename, MakeStructStmt, GetLine(view.inits(i)));
                }
            }
            else {
                const auto& field_view = view.inits(i).view_ArrayInit();

                if (has.find(field_view.type_info().view_VarInit().var_name()) != has.end()) 
                {
					throw SemanticException("Field " + field_view.type_info().view_VarInit().var_name() + " Already Exists", filename, MakeStructStmt, GetLine(view.inits(i)));
                }
                has.insert(field_view.type_info().view_VarInit().var_name());

                if (not TypeExists(field_view.type_info().view_VarInit().type_name())) 
                {
					throw SemanticException("A Type " + field_view.type_info().view_VarInit().type_name() + " does not Exist", filename, MakeStructStmt, GetLine(view.inits(i)));
                }

                if (field_view.init_expr().option_is_EmptyLiteral()) { continue; }
                if (not CanBeAssigned(
                    ArrayKind{ ExprToStr(field_view.alloc_size()), field_view.is_dynamic(),
                    ObjectKind{ IsPrimitive(field_view.type_info().view_VarInit().type_name()), field_view.type_info().view_VarInit().type_name() } }, 
                        AnalyzeExpression(field_view.init_expr())))
                {
					throw SemanticException("Expression does not Deduce to Array of Type " + field_view.type_info().view_VarInit().type_name(), filename, MakeStructStmt, GetLine(view.inits(i)));
                }
            }
        }

        AddType(view.struct_name(), std::move(ConstructStructType(make_struct_stmt)));

        std::cout << "Make Struct Command Ready\n";
    }

    void Semantics::AnalyzePropertyOption(const Statement& prop_opt_stmt, const std::string& prop_name)
    {
        if (not prop_opt_stmt.option_is_MakeStruct())
        {
            throw SemanticException("Unexpected Option " + prop_opt_stmt.sel(), filename, MakePropertyStmt, GetLine(prop_opt_stmt));
        }

        const auto& view = prop_opt_stmt.view_MakeStruct();

        std::unordered_set <std::string> has;
        for (size_t i = 0; i < view.size(); i++)
        {
            if (view.inits(i).option_is_VarInit()) {
                const auto& field_view = view.inits(i).view_VarInit();
                if (has.contains(field_view.var_name()))
                {
                    throw SemanticException("Field " + field_view.var_name() + " Already Exists in Option " + view.struct_name(), filename, MakePropertyStmt, GetLine(view.inits(i)));
                }
                has.insert(field_view.var_name());
                if (not TypeExists(field_view.type_name()) && field_view.type_name() != prop_name)
                {
                    throw SemanticException("A Type " + field_view.type_name() + " does not Exist", filename, MakePropertyStmt, GetLine(view.inits(i)));
                }
                if (field_view.init_expr().option_is_EmptyLiteral()) { continue; }
                if (not CanBeAssigned(ObjectKind{ IsPrimitive(field_view.type_name()), field_view.type_name() }, AnalyzeExpression(field_view.init_expr())))
                {
                    throw SemanticException("Expression does not Deduce to Type " + field_view.type_name(), filename, MakePropertyStmt, GetLine(view.inits(i)));
                }
            }
            else {
                const auto& field_view = view.inits(i).view_ArrayInit();
                if (has.contains(field_view.type_info().view_VarInit().var_name()))
                {
                    throw SemanticException("Field " + field_view.type_info().view_VarInit().var_name() + " Already Exists in Option " + view.struct_name(), filename, MakePropertyStmt, GetLine(view.inits(i)));
                }
                has.insert(field_view.type_info().view_VarInit().var_name());
                if (not TypeExists(field_view.type_info().view_VarInit().type_name()) && field_view.type_info().view_VarInit().type_name() != prop_name)
                {
                    throw SemanticException("A Type " + field_view.type_info().view_VarInit().type_name() + " does not Exist", filename, MakePropertyStmt, GetLine(view.inits(i)));
                }
                if (field_view.init_expr().option_is_EmptyLiteral()) { continue; }
                if (not CanBeAssigned(
                    ArrayKind{ ExprToStr(field_view.alloc_size()), field_view.is_dynamic(),
                    ObjectKind{ IsPrimitive(field_view.type_info().view_VarInit().type_name()), field_view.type_info().view_VarInit().type_name() } },
                    AnalyzeExpression(field_view.init_expr())))
                {
                    throw SemanticException("Expression does not Deduce to Array of Type " + field_view.type_info().view_VarInit().type_name(), filename, MakePropertyStmt, GetLine(view.inits(i)));
                }
            }

        }
        std::cout << "Property Option " << view.struct_name() << " Ready\n";
    }

    void Semantics::AnalyzeMakeProperty(const Statement& make_prop_stmt)
    {
        if (not make_prop_stmt.option_is_MakeProperty()) 
        {
			throw SemanticException("Unexpected Option " + make_prop_stmt.sel(), filename, MakePropertyStmt, GetLine(make_prop_stmt));
        }

        const auto& view = make_prop_stmt.view_MakeProperty();

        if (NameExists(view.prop_name())) 
        {
			throw SemanticException(view.prop_name() + " Already Exists", filename, MakePropertyStmt, GetLine(make_prop_stmt));
        }

		std::unordered_set <std::string> has;
        for (size_t i = 0; i < view.size(); i++)
        {
            if (has.contains(view.opts(i).view_MakeStruct().struct_name()))
            {
                throw SemanticException("Option " + view.opts(i).view_MakeStruct().struct_name() + " Already Exists in Property " + view.prop_name(), filename, MakePropertyStmt, GetLine(view.opts(i)));
			}

			has.insert(view.opts(i).view_MakeStruct().struct_name());
            AnalyzePropertyOption(view.opts(i), view.prop_name());
			AddType(view.opts(i).view_MakeStruct().struct_name(), ConstructStructType(view.opts(i)));
			option_to_prop.insert({ view.opts(i).view_MakeStruct().struct_name(), view.prop_name() });
        }

        AddType(view.prop_name(), std::move(ConstructPropertyType(make_prop_stmt)));

        std::cout << "Property Command Ready\n";
    }

    void Semantics::AnalyzeMakeEnum(const Statement& make_enum_stmt)
    {
        if (not make_enum_stmt.option_is_MakeEnum()) 
        {
			throw SemanticException("Unexpected Option " + make_enum_stmt.sel(), filename, MakeEnumStmt, GetLine(make_enum_stmt));
        }

        const auto& view = make_enum_stmt.view_MakeEnum();

        if (NameExists(view.enum_name())) 
        {
			throw SemanticException(view.enum_name() + " Already Exists", filename, MakeEnumStmt, GetLine(make_enum_stmt));
        }

        std::unordered_set <std::string> has;
        for (size_t i = 0; i < view.size(); i++)
        {
            if (has.contains(view.enum_variants(i))) 
            {
				throw SemanticException("Variant " + view.enum_variants(i) + " Already Exists in Enum " + view.enum_name(), filename, MakeEnumStmt, GetLine(make_enum_stmt));
            }
            has.insert(view.enum_variants(i));
            symbol_table.insert({ view.enum_variants(i), ObjectKind{ false, view.enum_name() }});
            Activate(view.enum_variants(i));
        }

        AddType(view.enum_name(), ConstructEnumType(make_enum_stmt));

        std::cout << "Enum Command Ready\n";
    }

    bool Semantics::AnalyzeMatch(const Statement& match_stmt, bool should_return, const VariableKind& ret)
    {
        if (not match_stmt.option_is_Match()) 
        {
			throw SemanticException("Unexpected Option " + match_stmt.sel(), filename, MatchStmt, GetLine(match_stmt));
        }

        const auto& view = match_stmt.view_Match();

        auto matched_expr_kind = AnalyzeExpression(view.matched_expr());

        if (std::holds_alternative <ArrayKind>(matched_expr_kind)) 
        {
			throw SemanticException("Cannot Match on Array Types", filename, MatchStmt, GetLine(match_stmt));
        }

        if (not std::holds_alternative <EnumType>(type_table.at(std::get <ObjectKind>(matched_expr_kind).type_name)) &&
            not std::holds_alternative <PropertyType>(type_table.at(std::get <ObjectKind>(matched_expr_kind).type_name)))
        {
			throw SemanticException("Can Only Match on Enum Types and Property Types", filename, MatchStmt, GetLine(match_stmt));
        }

        bool returns = true, changed = false;
        auto matched_type_name = ExprToStr(view.matched_expr());
        for (size_t i = 0; i < view.size(); i++)
        {
            if (not view.cases(i).view_CaseClause().case_expr().option_is_VarName()
                && not view.cases(i).view_CaseClause().is_wildcard())
            {
				throw SemanticException("Case Clause Must be of the Form 'case <variant/option>:'", filename, CaseClauseStmt, GetLine(view.cases(i)));
            }

            // std::cout << matched_type_name;
            VariableKind temp;

            if (not view.cases(i).view_CaseClause().is_wildcard())
            {
                auto option_name = view.cases(i).view_CaseClause().case_expr().view_VarName().name();
                if ((not type_table.contains(option_name) || not active[option_name]) && 
                    (not symbol_table.contains(option_name) || not active[option_name]))
                {
                    throw SemanticException("Variant/Option " + option_name + " does not Exist", filename, CaseClauseStmt, GetLine(view.cases(i)));
                }
                temp = symbol_table[matched_type_name];
                symbol_table[matched_type_name] = ObjectKind{ false, option_name };
                Activate(matched_type_name);
            }

            returns = AnalyzeBlock(view.cases(i).view_CaseClause().case_body(), should_return, ret) && returns;
            changed = true;
			symbol_table[matched_type_name] = temp;

            if (not view.cases(i).view_CaseClause().is_wildcard())
            {
                Deactivate(matched_type_name);
            }
        }

        std::cout << "Match Command Ready\n";

        return changed ? returns : false;
    }

    void Semantics::AnalyzeAssignment(const Statement& assign_stmt)
    {
        if (not assign_stmt.option_is_Assignment()) 
        {
			throw SemanticException("Unexpected Option " + assign_stmt.sel(), filename, AssignmentStmt, GetLine(assign_stmt));
        }

        const auto& view = assign_stmt.view_Assignment();

        if (not CanBeAssigned(AnalyzeExpression(view.dest()), AnalyzeExpression(view.expr())))
        {
			throw SemanticException("Cannot Assign Expression", filename, AssignmentStmt, GetLine(assign_stmt));
        }

        std::cout << "Assignment Command Ready\n";
    }

    bool Semantics::AnalyzeElif(const Statement& elif_stmt, bool should_return, const VariableKind& ret)
    {
        if (not elif_stmt.option_is_ElifCondition()) 
        {
			throw SemanticException("Unexpected Option " + elif_stmt.sel(), filename, ElifConditionStmt, GetLine(elif_stmt));
        }

        const auto& view = elif_stmt.view_ElifCondition();

        if (not CanBeAssigned(ObjectKind{ true, "bool" }, AnalyzeExpression(view.elif_cond())))
        {
			throw SemanticException("Elif Condition Must be of Type bool", filename, ElifConditionStmt, GetLine(elif_stmt));
        }

        std::cout << "Elif Body Ready\n";
        return AnalyzeBlock(view.elif_body(), should_return, ret);
    }

    bool Semantics::AnalyzeCondition(const Statement& cond_stmt, bool should_return, const VariableKind& ret)
    {
        if (not cond_stmt.option_is_Condition()) 
        {
			throw SemanticException("Unexpected Option " + cond_stmt.sel(), filename, ConditionStmt, GetLine(cond_stmt));
        }

        const auto& view = cond_stmt.view_Condition();

        if (not CanBeAssigned(ObjectKind{ true, "bool" }, AnalyzeExpression(view.if_cond())))
        {
			throw SemanticException("If Condition Must be of Type bool", filename, ConditionStmt, GetLine(cond_stmt));
        }


        bool if_returns = AnalyzeBlock(view.if_body(), should_return, ret);

        bool elif_returns = true;
        bool changed = false;
        for (size_t i = 0; i < view.size(); i++)
        {
            elif_returns = elif_returns && AnalyzeElif(view.elif_stmt(i), should_return, ret);
            changed = true;
        }

        bool else_returns = false;
        if (not view.else_body().option_is_Empty())
        {
            else_returns = AnalyzeBlock(view.else_body(), should_return, ret);
        }

        std::cout << "Condition Ready\n";

        return else_returns ? if_returns && ((elif_returns && changed) || (not changed)) : false;
    }

    bool Semantics::AnalyzeBlock(const Statement& block_stmt, bool should_return, const VariableKind& ret)
    {
        if (not block_stmt.option_is_Block())
        {
            throw SemanticException("Unexpected Option " + block_stmt.sel(), filename, BlockOfStmt, GetLine(block_stmt));
        }

        const auto& seq = block_stmt.view_Block();

        bool returns = false;

        for (size_t i = 0; i < seq.size(); i++)
        {
            switch (seq.statements(i).sel())
            {
            case selector::VarInitStmt:
                AnalyzeVarInit(seq.statements(i));
				block_allocated.push_back(seq.statements(i).view_VarInit().var_name());
                break;
            case selector::ArrayInitStmt:
                AnalyzeArrayInit(seq.statements(i));
                block_allocated.push_back(seq.statements(i).view_ArrayInit().type_info().view_VarInit().var_name());
                break;
            case selector::WhileLoopStmt:
                AnalyzeWhileLoop(seq.statements(i), should_return, ret);
                break;
            case selector::MakeStructStmt:
				throw SemanticException("MakeStruct is not Allowed Inside a Block", filename, MakeStructStmt, 0);
            case selector::MakeEnumStmt:
				throw SemanticException("MakeEnum is not Allowed Inside a Block", filename, MakeEnumStmt, 0);
            case selector::MakePropertyStmt:
				throw SemanticException("MakeProperty is not Allowed Inside a Block", filename, MakePropertyStmt, 0);
            case selector::MatchStmt:
            {
                bool res = AnalyzeMatch(seq.statements(i), should_return, ret);

                if (should_return)
                {
                    returns = res;
                }
                break;
            }
            case selector::ForLoopStmt:
                AnalyzeForLoop(seq.statements(i), should_return, ret);
                break;
            case selector::ConditionStmt:
            {
                bool res = AnalyzeCondition(seq.statements(i), should_return, ret);

                if (should_return)
                {
                    returns = res;
                }
                break;
            }
            case selector::MakeFunctionStmt:
				throw SemanticException("MakeFunction is not Allowed Inside a Block", filename, MakeFunctionStmt, GetLine(seq.statements(i)));
            case selector::AssignmentStmt:
                AnalyzeAssignment(seq.statements(i));
                break;
            case selector::BlockOfStmt:
            {
                bool res = AnalyzeBlock(seq.statements(i), should_return, ret);

                if (should_return)
                {
                    returns = res;
                }

                break;
            }
            case selector::ExprCallStmt:
                AnalyzeExprCall(seq.statements(i));
                break;
            case selector::ReturnStmt:
            {
                if (not should_return && (not std::holds_alternative <ObjectKind>(ret) && std::get <ObjectKind>(ret).type_name != "void"))
                {
                    throw SemanticException("Return Statement Outside Function Body", filename, ReturnStmt, GetLine(seq.statements(i)));
                }

                else if (not CanBeAssigned(ret, AnalyzeExpression(seq.statements(i).view_Return().return_expr())))
                {
                    throw SemanticException("Cannot Deduce Return Type to Function's", filename, ReturnStmt, GetLine(seq.statements(i)));
                }

                returns = true;
                break;
            }
            default:
				throw SemanticException("Unexpected Statement", filename, BlockOfStmt, GetLine(seq.statements(i)));
            }
        }

        for (const auto& var : block_allocated)
        {
            Deactivate(var);
        }
		block_allocated.clear();

        std::cout << "Block is ready\n";

        return returns;
    }

}