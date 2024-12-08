#include "lexer.h"
#include <cstring>
#include <algorithm>
#include "exceptions.hpp"

Lexer::Lexer(const char* fn, std::string text) : mFileName(fn), mText(std::move(text)), mPos(-1, 0, -1) {
    advance();
}

void Lexer::process() {
    while (mCurrentChar) {
        if (mCurrentChar[0] == '\t' || mCurrentChar[0] == '\n' || std::isspace(mCurrentChar[0])) {
            advance();
        } else if (!std::strncmp("print", mCurrentChar, 5)) {
            mTokens.emplace_back(TokenType::PRINT);

            for (int i = 0; i < 5; ++i) advance();
        } else if (!std::strncmp("read", mCurrentChar, 4)) {
            mTokens.emplace_back(TokenType::READ);

            for (int i = 0; i < 4; ++i) advance();
        } else if (!std::strncmp("dotimes", mCurrentChar, 7)) {
            mTokens.emplace_back(TokenType::DOTIMES);

            for (int i = 0; i < 7; ++i) advance();
        } else if (!std::strncmp("let", mCurrentChar, 3)) {
            mTokens.emplace_back(TokenType::LET);

            for (int i = 0; i < 3; ++i) advance();
        } else if (!std::strncmp("setq", mCurrentChar, 4)) {
            mTokens.emplace_back(TokenType::SETQ);

            for (int i = 0; i < 4; ++i) advance();
        } else if (std::isalnum(mCurrentChar[0])) {
            std::string token;

            while (mCurrentChar && std::isalnum(mCurrentChar[0])) {
                token += mCurrentChar[0];
                advance();
            }

            if (std::ranges::all_of(token.begin(), token.end(), [&](char c) {
                if (!std::isdigit(c)) return false;
                return true;
            })) {
                mTokens.emplace_back(TokenType::INT, token);
            } else {
                mTokens.emplace_back(TokenType::VAR, token);
            }
        } else if (mCurrentChar[0] == '+') {
            mTokens.emplace_back(TokenType::PLUS);
            advance();
        } else if (mCurrentChar[0] == '-') {
            mTokens.emplace_back(TokenType::MINUS);
            advance();
        } else if (mCurrentChar[0] == '*') {
            mTokens.emplace_back(TokenType::MUL);
            advance();
        } else if (mCurrentChar[0] == '/') {
            mTokens.emplace_back(TokenType::DIV);
            advance();
        } else if (mCurrentChar[0] == '(') {
            mTokens.emplace_back(TokenType::LPAREN);
            advance();
        } else if (mCurrentChar[0] == ')') {
            mTokens.emplace_back(TokenType::RPAREN);
            advance();
        } else {
            throw IllegalCharError(mFileName, std::string(1, mCurrentChar[0]).c_str(), mPos.lineNumber);
        }
    }

    mTokens.emplace_back(TokenType::EOF_, "EOF");
}

void Lexer::advance() {
    mPos.advance(mCurrentChar);

    if (mPos.index < mText.size()) {
        mCurrentChar = &mText[mPos.index];
    } else {
        mCurrentChar = nullptr;
    }
}
