#include "parser.h"
#include "exceptions.hpp"

Parser::Parser() : mTokenIndex(-1), openParanCount(0) {}

void Parser::setTokens(std::vector<Token>& tokens) {
    mTokens = std::move(tokens);
    advance();
}

NodePtr Parser::parse() {
    return parseExpr();
}

NodePtr Parser::parseExpr() {
    NodePtr left, right;

    parseParen(TokenType::LPAREN);

    if (mCurrentToken.type == TokenType::MUL || mCurrentToken.type == TokenType::DIV ||
        mCurrentToken.type == TokenType::PLUS || mCurrentToken.type == TokenType::MINUS) {
        Token token = mCurrentToken;
        advance();
        left = parseNumber();

        if (mCurrentToken.type != TokenType::INT && mCurrentToken.type != TokenType::_EOF) {
            right = parseExpr();
        } else {
            right = parseNumber();
        }

        left = std::make_unique<BinOpNode>(left, right, token);
    } else {
        throw InvalidSyntaxError("", "Missing Operator: must be +,-,*,/", 0);
    }

    parseParen(TokenType::RPAREN);

    return left;
}

NodePtr Parser::parseNumber() {
    if (mCurrentToken.type == TokenType::INT) {
        Token token = mCurrentToken;
        advance();
        return std::make_unique<NumberNode>(std::stoi(token.value) - 48);
    }

    throw InvalidSyntaxError("", "expected INT", 0);
}

void Parser::parseParen(TokenType expected) {
    if (mCurrentToken.type != expected) {
        throw InvalidSyntaxError("", "Missing Parenthesis", 0);
    } else advance();
}

Token Parser::advance() {
    ++mTokenIndex;

    if (mTokenIndex < mTokens.size()) {
        mCurrentToken = mTokens[mTokenIndex];
    }

    return mCurrentToken;
}


