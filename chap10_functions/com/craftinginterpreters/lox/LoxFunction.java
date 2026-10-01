package com.craftinginterpreters.lox;

import java.util.List;

class LoxFunction implements LoxCallable {
  private final Stmt.Function declaration;

  // ~~~~~~ challenge 2 start ~~~~~~
  private final Expr.Function anonymous;
  // ~~~~~~ challenge 2 end ~~~~~~

  private final Environment closure;

  LoxFunction(Stmt.Function declaration, Environment closure) {
    this.closure = closure;
    this.declaration = declaration;

    // ~~~~~~ challenge 2 start ~~~~~~
    this.anonymous = null;
    // ~~~~~~ challenge 2 end ~~~~~~
  }

  // ~~~~~~ challenge 2 start ~~~~~~
  // creates anonymous function
  LoxFunction(Expr.Function anonymous, Environment closure) {
    this.closure = closure;
    this.declaration = null;
    this.anonymous = anonymous;
  }
  // ~~~~~~ challenge 2 end ~~~~~~

  @Override
  public String toString() {
    // ~~~~~~ challenge 2 start ~~~~~~
    // old: return "<fn " + declaration.name.lexeme + ">";

    if (declaration == null) return "<fn>";
    return "<fn " + declaration.name.lexeme + ">";
    // ~~~~~~ challenge 2 end ~~~~~~
  }
  @Override
  public int arity() {
    // ~~~~~~ challenge 2 start ~~~~~~
    // old: return declaration.params.size();

    if (declaration != null) return declaration.params.size();
    return anonymous.params.size();
    // ~~~~~~ challenge 2 end ~~~~~~
  }
  @Override
  public Object call(Interpreter interpreter,
                     List<Object> arguments) {
    Environment environment = new Environment(closure);
    // old: for (int i = 0; i < declaration.params.size(); i++) {
    //   environment.define(declaration.params.get(i).lexeme,
    //       arguments.get(i));
    // }
    // ~~~~~~ challenge 2 start ~~~~~~
    List<Token> params;
    List<Stmt> body;

    if (declaration != null) {
      params = declaration.params;
      body = declaration.body;
    } else {
      params = anonymous.params;
      body = anonymous.body;
    }

    for (int i = 0; i < params.size(); i++) {
      environment.define(params.get(i).lexeme, arguments.get(i));
    }
    // ~~~~~~ challenge 2 end ~~~~~~

    try {
      // ~~~~~~ challenge 2 start ~~~~~~
      // old: interpreter.executeBlock(declaration.body, environment);
      interpreter.executeBlock(body, environment);
      // ~~~~~~ challenge 2 end ~~~~~~
    } catch (Return returnValue) {
      return returnValue.value;
    }
    return null;
  }
}
