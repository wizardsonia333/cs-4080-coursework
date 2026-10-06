package com.craftinginterpreters.lox;

import java.util.List;
import java.util.Map;

// ~~~~~~ challenge 1 start ~~~~~~
// old: class LoxClass implements LoxCallable {
class LoxClass extends LoxInstance implements LoxCallable {
// ~~~~~~ challenge 1 end ~~~~~~
  final String name;
  private final Map<String, LoxFunction> methods;

  // ~~~~~~ challenge 1 start ~~~~~~
  // old:
  // LoxClass(String name, Map<String, LoxFunction> methods) {
  //   this.name = name;
  //   this.methods = methods;
  // }

  LoxClass(String name, LoxClass metaclass,
          Map<String, LoxFunction> methods) {
    super(metaclass);
    this.name = name;
    this.methods = methods;
  }
  // ~~~~~~ challenge 1 end ~~~~~~

  LoxFunction findMethod(String name) {
    if (methods.containsKey(name)) {
      return methods.get(name);
    }

    return null;
  }

  @Override
  public String toString() {
    return name;
  }
  @Override
  public Object call(Interpreter interpreter,
                     List<Object> arguments) {
    LoxInstance instance = new LoxInstance(this);
    LoxFunction initializer = findMethod("init");
    if (initializer != null) {
      initializer.bind(instance).call(interpreter, arguments);
    }

    return instance;
  }

  @Override
  public int arity() {
    LoxFunction initializer = findMethod("init");
    if (initializer == null) return 0;
    return initializer.arity();
  }
}
