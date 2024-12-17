#ifndef TINYSEXP_EXCEPTIONS_HPP
#define TINYSEXP_EXCEPTIONS_HPP

#include <exception>
#include <string>

class IError : public std::exception {
public:
    explicit IError(const char* msg, const char* fn, const char* detail, int ln) : msg(msg) {
        this->msg += detail;
        this->msg += "\nFile " + std::string(fn) + ", line " + std::to_string(ln);
        this->msg += '\n';
    }

    [[nodiscard]] const char* what() const noexcept override {
        return msg.c_str();
    }

private:
    std::string msg;
};

class IllegalCharError : public IError {
public:
    IllegalCharError(const char* fn, const char* detail, int ln) :
            IError("Illegal Character: ", fn, detail, ln) {}
};

class InvalidSyntaxError : public IError {
public:
    explicit InvalidSyntaxError(const char* fn, const char* detail, int ln) :
            IError("Invalid Syntax: ", fn, detail, ln) {}
};

class MultipleDeclarationError : public IError {
public:
    explicit MultipleDeclarationError(const char* fn, const char* detail, int ln) :
            IError("Multiple Declaration: ", fn, detail, ln) {}
};

class UnboundVariableError : public IError {
public:
    explicit UnboundVariableError(const char* fn, const char* detail, int ln) :
            IError("Unbound Variable: ", fn, detail, ln) {}
};

class TypeError : public IError {
public:
    explicit TypeError(const char* fn, const char* detail, int ln) :
            IError("Type Error: ", fn, detail, ln) {}
};

class ScopeError : public IError {
public:
    explicit ScopeError(const char* fn, const char* detail, int ln) :
            IError("Scope Error: ", fn, detail, ln) {}
};

#endif
