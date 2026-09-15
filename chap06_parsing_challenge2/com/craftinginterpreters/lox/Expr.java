//> Appendix II expr
package com.craftinginterpreters.lox;

import java.util.List;

abstract class Expr {
  interface Visitor<R> {
    R visitBinaryExpr(Binary expr);
    // ~~~~start of challenge 2~~~~

    // new code for challenge 2:
    // lets visitors handle our new Conditional expression
    R visitConditionalExpr(Conditional expr);

    // ~~~~end of challenge 2~~~~
    R visitGroupingExpr(Grouping expr);
    R visitLiteralExpr(Literal expr);
    R visitUnaryExpr(Unary expr);
  }

  // Nested Expr classes here...
//> expr-binary
  static class Binary extends Expr {
    Binary(Expr left, Token operator, Expr right) {
      this.left = left;
      this.operator = operator;
      this.right = right;
    }

    @Override
    <R> R accept(Visitor<R> visitor) {
      return visitor.visitBinaryExpr(this);
    }

    final Expr left;
    final Token operator;
    final Expr right;
  }

  // ~~~~start of challenge 2~~~~

  // new code for challenge 2:
  // creates a new AST node for ternary expressions
  // unlike Binary this needs three expression parts
  // condition ? thenBranch : elseBranch
  static class Conditional extends Expr {
    Conditional(Expr condition, Expr thenBranch, Expr elseBranch) {
      this.condition = condition;
      this.thenBranch = thenBranch;
      this.elseBranch = elseBranch;
    }

    // lets a visitor know it is visiting a Conditional expression
    @Override
    <R> R accept(Visitor<R> visitor) {
      return visitor.visitConditionalExpr(this);
    }

    // stores the condition before the ?
    final Expr condition;
    // stores the expression used if the condition is true
    final Expr thenBranch;
    // stores the expression used if the condition is false
    final Expr elseBranch;
  }

  // ~~~~end of challenge 2~~~~

//< expr-binary
//> expr-grouping
  static class Grouping extends Expr {
    Grouping(Expr expression) {
      this.expression = expression;
    }

    @Override
    <R> R accept(Visitor<R> visitor) {
      return visitor.visitGroupingExpr(this);
    }

    final Expr expression;
  }
//< expr-grouping
//> expr-literal
  static class Literal extends Expr {
    Literal(Object value) {
      this.value = value;
    }

    @Override
    <R> R accept(Visitor<R> visitor) {
      return visitor.visitLiteralExpr(this);
    }

    final Object value;
  }
//< expr-literal
//> expr-unary
  static class Unary extends Expr {
    Unary(Token operator, Expr right) {
      this.operator = operator;
      this.right = right;
    }

    @Override
    <R> R accept(Visitor<R> visitor) {
      return visitor.visitUnaryExpr(this);
    }

    final Token operator;
    final Expr right;
  }
//< expr-unary

  abstract <R> R accept(Visitor<R> visitor);
}
//< Appendix II expr
