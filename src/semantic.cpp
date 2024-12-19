#include "semantic.h"
#include <format>
#include "exceptions.hpp"

SemanticAnalyzer::SemanticAnalyzer(const char* fn): mFileName(fn) {}

void SemanticAnalyzer::analyze(ExprPtr& ast) {
    auto next = ast;

    scopeEnter();
    while (next != nullptr) {
        exprResolve(next);
        next = next->child;
    }
    scopeExit();
}

void SemanticAnalyzer::exprResolve(const ExprPtr& ast) {
    if (auto binop = cast::toBinop(ast)) {
        binopResolve(*binop);
    } else if (auto dotimes = cast::toDotimes(ast)) {
        dotimesResolve(*dotimes);
    } else if (auto let = cast::toLet(ast)) {
        letResolve(*let);
    } else if (auto setq = cast::toSetq(ast)) {
        setqResolve(*setq);
    } else if (auto defvar = cast::toDefvar(ast)) {
        defvarResolve(*defvar);
    } else if (auto defconst = cast::toDefconstant(ast)) {
        defconstResolve(*defconst);
    } else if (auto defun = cast::toDefun(ast)) {
        defunResolve(*defun);
    } else if (auto funcCall = cast::toFuncCall(ast)) {
        funcCallResolve(*funcCall);
    } else if (auto if_ = cast::toIf(ast)) {
        ifResolve(*if_);
    } else if (auto when = cast::toWhen(ast)) {
        whenResolve(*when);
    } else if (auto cond = cast::toCond(ast)) {
        condResolve(*cond);
    }
}

void SemanticAnalyzer::binopResolve(BinOpExpr& binop) {
    varResolve(binop.lhs);
    varResolve(binop.rhs);
}

void SemanticAnalyzer::varResolve(ExprPtr& var) {
    auto isT = cast::toT(var);
    auto isNil = cast::toNIL(var);

    if (isT || isNil) {
        if (isT) {
            throw TypeError(mFileName, T_VAR_ERROR, 0);
        }

        if (isNil) {
            throw TypeError(mFileName, NIL_VAR_ERROR, 0);
        }
    }

    if (!cast::toInt(var) && !cast::toDouble(var)) {
        const auto name = cast::toString(var)->data;
        Symbol sym = scopeLookup(name);

        if (!sym.value) {
            throw UnboundVariableError(mFileName, ERROR(UNBOUND_VAR_ERROR, name), 0);
        }

        var = sym.value;
    }
}

void SemanticAnalyzer::dotimesResolve(const DotimesExpr& dotimes) {
    scopeEnter();
    const auto iterVarName = cast::toString(dotimes.iterationCount)->data;

    Symbol sym = scopeLookup(iterVarName);
    if (sym.isConstant) {
        throw TypeError(mFileName, ERROR(CONSTANT_VAR_ERROR, iterVarName), 0);
    }
    scopeExit();
}

void SemanticAnalyzer::letResolve(const LetExpr& let) {
    scopeEnter();
    for (auto& var: let.bindings) {
        const auto varExpr = cast::toVar(var);
        auto varName = cast::toString(varExpr->name)->data;

        Symbol sym = scopeLookupCurrent(varName);
        if (sym.value) {
            throw MultipleDeclarationError(mFileName, ERROR(MULTIPLE_DECL_ERROR, varName + " in LET"), 0);
        }

        scopeBind(varName, {varName, var, SymbolType::LOCAL});
    }

    for (auto& statement: let.body) {
        exprResolve(statement);
    }
    scopeExit();
}

void SemanticAnalyzer::setqResolve(const SetqExpr& setq) {
    const auto var = cast::toVar(setq.var);
    const auto varName = cast::toString(var->name)->data;

    Symbol sym = scopeLookup(varName);

    if (!sym.value) {
        throw UnboundVariableError(mFileName, ERROR(UNBOUND_VAR_ERROR, varName), 0);
    } else {
        if (sym.isConstant) {
            throw TypeError(mFileName, ERROR(CONSTANT_VAR_ERROR, varName), 0);
        }
    }
}

void SemanticAnalyzer::defvarResolve(const DefvarExpr& defvar) {
    const auto var = cast::toVar(defvar.var);
    const auto varName = cast::toString(var->name)->data;

    if (scopeLevel() > 1) {
        throw ScopeError(mFileName, ERROR(GLOBAL_VAR_DECL_ERROR, varName), 0);
    }

    scopeBind(varName, {varName, defvar.var, SymbolType::GLOBAL});
}

void SemanticAnalyzer::defconstResolve(const DefconstExpr& defconst) {
    const auto var = cast::toVar(defconst.var);
    const auto varName = cast::toString(var->name)->data;

    if (scopeLevel() > 1) {
        throw ScopeError(mFileName, ERROR(CONSTANT_VAR_DECL_ERROR, varName), 0);
    }

    scopeBind(varName, {varName, defconst.var, SymbolType::GLOBAL, true});
}

void SemanticAnalyzer::defunResolve(const DefunExpr& defun) {
    const auto funcName = cast::toString(defun.name)->data;

    if (scopeLevel() > 1) {
        throw ScopeError(mFileName, ERROR(FUNC_DEF_ERROR, funcName), 0);
    }

    ExprPtr func = std::make_shared<DefunExpr>(defun);

    scopeBind(funcName, {funcName, func, SymbolType::GLOBAL});

    scopeEnter();
    for (auto& param: defun.params) {

    }

    for (auto& statement: defun.body) {

    }
    scopeExit();
}

void SemanticAnalyzer::funcCallResolve(const FuncCallExpr& funcCall) {

}

void SemanticAnalyzer::ifResolve(const IfExpr& ifExpr) {}

void SemanticAnalyzer::whenResolve(const WhenExpr& when) {}

void SemanticAnalyzer::condResolve(const CondExpr& cond) {}

void SemanticAnalyzer::scopeEnter() {
    std::unordered_map<std::string, Symbol> scope;
    mSymbolTable.push(scope);
}

void SemanticAnalyzer::scopeExit() {
    mSymbolTable.pop();
}

size_t SemanticAnalyzer::scopeLevel() {
    return mSymbolTable.size();
}

void SemanticAnalyzer::scopeBind(const std::string& name, const Symbol& symbol) {
    auto scope = mSymbolTable.top();
    mSymbolTable.pop();
    scope.emplace(name, symbol);
    mSymbolTable.push(scope);
}

Symbol SemanticAnalyzer::scopeLookup(const std::string& name) {
    Symbol sym;
    std::stack<ScopeType> scopes;
    size_t level = mSymbolTable.size();

    for (int i = 0; i < level; ++i) {
        ScopeType scope = mSymbolTable.top();
        mSymbolTable.pop();
        scopes.push(scope);

        if (auto elem = scope.find(name);elem != scope.end()) {
            sym = elem->second;
            break;
        }
    }

    // reconstruct the scopes
    size_t currentLevel = scopes.size();
    for (int i = 0; i < currentLevel; ++i) {
        ScopeType scope = scopes.top();
        scopes.pop();
        mSymbolTable.push(scope);
    }

    return sym;
}

Symbol SemanticAnalyzer::scopeLookupCurrent(const std::string& name) {
    ScopeType currentScope = mSymbolTable.top();

    if (auto elem = currentScope.find(name);elem != currentScope.end()) {
        return elem->second;
    }

    return {};
}