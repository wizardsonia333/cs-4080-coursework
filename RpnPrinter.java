package com.craftinginterpreters.lox;
// ~~~~~ challenge 3 start ~~~~~
// visitor that walks through an expression ast & converts it into rpn

class RpnPrinter implements Expr.Visitor<String> {
  // starts the visitor on the expression & returns the finished rpn string
  String print(Expr expr) {
    return expr.accept(this);
  }

  // binary expressions put the left & right operands before the operator
  @Override
  public String visitBinaryExpr(Expr.Binary expr) {
    String left = expr.left.accept(this);
    String right = expr.right.accept(this);

    return left + " " + right + " " + expr.operator.lexeme;
  }

  // rpn doesn't need grouping parentheses so just visit the expression inside
  @Override
  public String visitGroupingExpr(Expr.Grouping expr) {
    return expr.expression.accept(this);
  }

  // literals can be returned as their actual value
  @Override
  public String visitLiteralExpr(Expr.Literal expr) {
    if (expr.value == null) return "nil";
    return expr.value.toString();
  }

  // unary operators also come after their operand in rpn
  @Override
  public String visitUnaryExpr(Expr.Unary expr) {
    String operator = expr.operator.lexeme;

    // gives unary minus its own name so it is not confused w/ subtraction
    if (expr.operator.type == TokenType.MINUS) {
      operator = "neg";
    }
    return expr.right.accept(this) + " " + operator;
  }

  // small test just to make sure the visitor produces the expected rpn
  // manually builds the ast for (1 + 2) * (4 - 3) for ex
  public static void main(String[] args) {
    Expr expression = new Expr.Binary(
        new Expr.Grouping(
            new Expr.Binary(
                new Expr.Literal(1.0),
                new Token(TokenType.PLUS, "+", null, 1),
                new Expr.Literal(2.0))),
        new Token(TokenType.STAR, "*", null, 1),
        new Expr.Grouping(
            new Expr.Binary(
                new Expr.Literal(4.0),
                new Token(TokenType.MINUS, "-", null, 1),
                new Expr.Literal(3.0))));

    // expected output is 1.0 2.0 + 4.0 3.0 - *
    System.out.println(new RpnPrinter().print(expression));
  }
}
// ~~~~~ challenge 3 end ~~~~~