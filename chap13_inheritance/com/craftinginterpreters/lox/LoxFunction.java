package com.craftinginterpreters.lox;

import java.util.List;

class LoxFunction implements LoxCallable {
  private final Stmt.Function declaration;
  private final Environment closure;

  private final boolean isInitializer;

  LoxFunction(Stmt.Function declaration, Environment closure,
              boolean isInitializer) {
    this.isInitializer = isInitializer;
    this.closure = closure;
    this.declaration = declaration;
  }

  // ~~~~~~ challenge 2 start ~~~~~~
  // old:
  // LoxFunction bind(LoxInstance instance) {
  //   Environment environment = new Environment(closure);
  //   environment.define("this", instance);
  //   return new LoxFunction(declaration, environment,
  //                          isInitializer);
  // }

  // binds both this & next method for inner
  LoxFunction bind(LoxInstance instance, LoxFunction inner) {
    Environment environment = new Environment(closure);
    environment.define("this", instance);

    // inner is bound while inheritance chain is built
    if (inner != null) {
      environment.define("inner", inner);
    } else {
      environment.define("inner", new LoxCallable() {
        @Override
        public int arity() {
          return 0;
        }
        @Override
        public Object call(Interpreter interpreter,
                          List<Object> arguments) {
          return null;
        }
      });
    }
    return new LoxFunction(declaration, environment,
                          isInitializer);
  }
  // ~~~~~~ challenge 2 end ~~~~~~

  @Override
  public String toString() {
    return "<fn " + declaration.name.lexeme + ">";
  }
  @Override
  public int arity() {
    return declaration.params.size();
  }
  @Override
  public Object call(Interpreter interpreter,
                     List<Object> arguments) {
    Environment environment = new Environment(closure);
    for (int i = 0; i < declaration.params.size(); i++) {
      environment.define(declaration.params.get(i).lexeme,
          arguments.get(i));
    }

    try {
      interpreter.executeBlock(declaration.body, environment);
    } catch (Return returnValue) {
      if (isInitializer) return closure.getAt(0, "this");

      return returnValue.value;
    }

    if (isInitializer) return closure.getAt(0, "this");
    return null;
  }
}
