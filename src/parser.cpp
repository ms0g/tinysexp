#include "parser.h"
#include "exceptions.hpp"

Parser::Parser(const char* fn) : mFileName(fn), mTokenIndex(-1) {}

void Parser::setTokens(std::vector<Token>& tokens) {
    mTokens = std::move(tokens);
    advance();
}

ExprPtr Parser::parse() {
    return parseExpr();
}

Token Parser::advance() {
    ++mTokenIndex;

    if (mTokenIndex < mTokens.size()) {
        mCurrentToken = mTokens[mTokenIndex];
    }

    return mCurrentToken;
}

ExprPtr Parser::parseExpr() {
    ExprPtr left, right;

    consume(TokenType::LPAREN);

    if (mCurrentToken.type == TokenType::MUL || mCurrentToken.type == TokenType::DIV ||
        mCurrentToken.type == TokenType::PLUS || mCurrentToken.type == TokenType::MINUS) {
        left = parseSExpr();
    } else if (mCurrentToken.type == TokenType::PRINT) {
        left = parsePrint();
    } else if (mCurrentToken.type == TokenType::DOTIMES) {
        left = parseDotimes();
    } else {
        throw InvalidSyntaxError(mFileName, mCurrentToken.value.c_str(), 0);
    }

    consume(TokenType::RPAREN);

    return left;
}

ExprPtr Parser::parseSExpr() {
    ExprPtr left, right;

    Token token = mCurrentToken;
    advance();
    left = parseNumber();

    if (mCurrentToken.type == TokenType::LPAREN) {
        consume(TokenType::LPAREN);
        right = parseSExpr();
        consume(TokenType::RPAREN);
    } else {
        right = parseAtom();
    }

    return std::make_unique<BinOpExpr>(left, right, token);
}

ExprPtr Parser::parsePrint() {
    ExprPtr left, right;

    Token token = mCurrentToken;
    advance();
    left = std::make_unique<PrintExpr>();

    if (mCurrentToken.type == TokenType::LPAREN) {
        consume(TokenType::LPAREN);
        right = parseSExpr();
        consume(TokenType::RPAREN);
    } else {
        right = parseAtom();
    }

    return std::make_unique<BinOpExpr>(left, right, token);
}

ExprPtr Parser::parseDotimes() {
    ExprPtr left, statement, iterationCount;
    Token token = mCurrentToken;
    advance();

    consume(TokenType::LPAREN);
    iterationCount = parseAtom(); // consume variable name
    iterationCount = parseAtom(); // get the actual number
    consume(TokenType::RPAREN);

    if (mCurrentToken.type != TokenType::RPAREN)
        statement = parseExpr();

    left = std::make_unique<DotimesExpr>(iterationCount);
    return std::make_unique<BinOpExpr>(left, statement, token);
}

ExprPtr Parser::parseAtom() {
    if (mCurrentToken.type == TokenType::VAR) {
        advance();
        return std::make_unique<VarExpr>();
    } else if (mCurrentToken.type == TokenType::INT) {
        ExprPtr num = parseNumber();
        return num;
    }
}

ExprPtr Parser::parseNumber() {
    if (mCurrentToken.type == TokenType::INT) {
        Token token = mCurrentToken;
        advance();
        return std::make_unique<NumberExpr>(std::stoi(token.value));
    }

    throw InvalidSyntaxError(mFileName, "Expected INT", 0);
}

void Parser::consume(TokenType expected) {
    if (mCurrentToken.type != expected) {
        throw InvalidSyntaxError(mFileName, "Missing Parenthesis", 0);
    } else advance();
}