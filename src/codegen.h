#ifndef CODEGEN_H
#define CODEGEN_H

#include <any>
#include <string>
#include <unordered_map>
#include "parser.h"
#include "stack.h"
#include "register.h"

class CodeGen {
public:
    CodeGen() : currentScope("main") {
    }

    std::string emit(const ExprPtr& ast);

private:
    Register* emitAST(const ExprPtr& ast);

    Register* emitBinop(const BinOpExpr& binop);

    Register* emitDotimes(const DotimesExpr& dotimes);

    Register* emitLoop(const LoopExpr& loop);

    Register* emitLet(const LetExpr& let);

    void emitSetq(const SetqExpr& setq);

    void emitDefvar(const DefvarExpr& defvar);

    void emitDefconst(const DefconstExpr& defconst);

    void emitDefun(const DefunExpr& defun);

    Register* emitFuncCall(const FuncCallExpr& funcCall);

    Register* emitIf(const IfExpr& if_);

    Register* emitWhen(const WhenExpr& when);

    Register* emitCond(const CondExpr& cond);

    Register* emitPrimitive(const ExprPtr& prim);

    Register* emitInt(const IntExpr& int_);

    Register* emitDouble(const DoubleExpr& double_);

    Register* emitNumb(const ExprPtr& n);

    Register* emitNode(const ExprPtr& node);

    Register* emitExpr(const ExprPtr& lhs, const ExprPtr& rhs, std::pair<const char*, const char*> op);

    void emitSection(const ExprPtr& var, bool isConstant = false);

    void emitTest(const ExprPtr& test, const std::string& trueLabel, const std::string& elseLabel);

    void emitJmpTrueLabel(const Register* reg, TokenType type, const std::string& label);

    Register* emitSet(const ExprPtr& set);

    Register* emitLogOp(const BinOpExpr& binop, const char* op);

    Register* emitSetReg(const BinOpExpr& binop);

    Register* emitCmpZero(const ExprPtr& node);

    void handleAssignment(const ExprPtr& var, uint32_t size);

    void handleVariable(const VarExpr& var, uint32_t size);

    Register* emitLoadRegFromMem(const VarExpr& var, uint32_t size);

    void emitStoreMemFromReg(const std::string& varName, SymbolType stype, const Register* reg, uint32_t size);

    std::string getAddr(const std::string& varName, SymbolType stype, uint32_t size);

    uint32_t getMemSize(const ExprPtr& var);

    void pushParamToRegister(uint32_t rid, const std::any& value);

    void pushParamOntoStack(const std::string& funcName, const VarExpr& param, int& stackIdx);

    const char* getRegName(const Register* reg, uint32_t size);

    const char* getRegNameByID(uint32_t id, uint32_t size);

    std::string createLabel();

    void updateSections(const char* name, const std::pair<std::string, std::string>& data);

    static bool isPrimitive(const ExprPtr& var);

    std::string generatedCode;
    // Label
    int currentLabelCount{0};
    // Scope
    std::string currentScope;
    // Register
    RegisterAllocator registerAllocator;
    // Stack
    StackAllocator stackAllocator;
    // Sections
    std::unordered_map<std::string, std::vector<std::pair<std::string, std::string> > > sections;
    // Functions
    std::vector<std::pair<void(CodeGen::*)(const DefunExpr&), const DefunExpr&> > functions;

    static constexpr const char* memorySize[SIZE_COUNT] = {"qword", "dword", "word", "byte", "byte"};

    static constexpr const char* dataSizeInitialized[SIZE_COUNT] = {"dq", "dd", "dw", "db", "db"};

    static constexpr const char* dataSizeUninitialized[SIZE_COUNT] = {"resq", "resd", "resw", "resb", "resb"};

    static constexpr int memorySizeInBytes[SIZE_COUNT] = {8, 4, 2, 1, 1};

    static constexpr int paramRegisters[] = {RDI, RSI, RDX, RCX, R8, R9};

    static constexpr int paramRegistersSSE[] = {xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7};
};

inline bool CodeGen::isPrimitive(const ExprPtr& var) {
    return cast::toInt(var) ||
           cast::toDouble(var) ||
           cast::toNIL(var) ||
           cast::toT(var) ||
           cast::toString(var) ||
           cast::toVar(var);
}

#endif
