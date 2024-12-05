#pragma once

#include "visitor.hpp"

MAKE_VISITOR(IntEvaluator, uint8_t, MAKE_MTHD_NUMBER, MAKE_MTHD_VAR, MAKE_MTHD_READ, NULL_)

MAKE_VISITOR(StringEvaluator, std::string, MAKE_MTHD_VAR, MAKE_MTHD_STR, NULL_, NULL_)

MAKE_VISITOR(TypeEvaluator, size_t, MAKE_MTHD_NUMBER, MAKE_MTHD_DOTIMES, MAKE_MTHD_BINOP, MAKE_MTHD_PRINT)

MAKE_VISITOR(PrintEvaluator, std::string, MAKE_MTHD_NUMBER, MAKE_MTHD_BINOP, MAKE_MTHD_PRINT, MAKE_MTHD_VAR)


