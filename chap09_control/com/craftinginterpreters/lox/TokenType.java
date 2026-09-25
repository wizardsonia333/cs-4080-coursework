package com.craftinginterpreters.lox;

enum TokenType {
  // Single-character tokens.
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

  // One or two character tokens.
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,

  // Literals.
  IDENTIFIER, STRING, NUMBER,

  // Keywords.
  // ~~~~~~ challenge 3 start ~~~~~~
  AND, BREAK, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  // ~~~~~~ challenge 3 end ~~~~~~
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

  EOF
}
