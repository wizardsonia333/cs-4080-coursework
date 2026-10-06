package com.craftinginterpreters.lox;

import java.util.List;
import java.util.Map;

class LoxClass implements LoxCallable {
  final String name;
  final LoxClass superclass;
  private final Map<String, LoxFunction> methods;

  LoxClass(String name, LoxClass superclass,
           Map<String, LoxFunction> methods) {
    this.superclass = superclass;
    this.name = name;
    this.methods = methods;
  }

  // ~~~~~~ challenge 2 start ~~~~~~
  // old:
  // LoxFunction findMethod(String name) {
  //   if (methods.containsKey(name)) {
  //     return methods.get(name);
  //   }
  //
  //   if (superclass != null) {
  //     return superclass.findMethod(name);
  //   }
  //
  //   return null;
  // }

  // looks thru the whole chain so highest method runs first
  LoxFunction findMethod(LoxInstance instance, String name) {
    LoxFunction method = null;
    LoxFunction inner = null;
    LoxClass current = this;

    while (current != null) {
      if (current.methods.containsKey(name)) {
        if (method != null) {
          inner = method.bind(instance, inner);
        }
        method = current.methods.get(name);
      }
      current = current.superclass;
    }

    if (method != null) {
      return method.bind(instance, inner);
    }
    return null;
  }
  // ~~~~~~ challenge 2 end ~~~~~~

  @Override
  public String toString() {
    return name;
  }
  @Override
  public Object call(Interpreter interpreter,
                     List<Object> arguments) {
    LoxInstance instance = new LoxInstance(this);
    // ~~~~~~ challenge 2 start ~~~~~~
    // old:
    // LoxFunction initializer = findMethod("init");
    // if (initializer != null) {
    //   initializer.bind(instance).call(interpreter, arguments);
    // }
    LoxFunction initializer = findMethod(instance, "init");
    if (initializer != null) {
      initializer.call(interpreter, arguments);
    }
    // ~~~~~~ challenge 2 end ~~~~~~

    return instance;
  }

  // old code:
  // @Override
  // public int arity() {
  //   LoxFunction initializer = findMethod("init");
  //   if (initializer == null) return 0;
  //   return initializer.arity();
  // }
  // ~~~~~~ challenge 2 start ~~~~~~
  @Override
  public int arity() {
    LoxFunction initializer = null;
    LoxClass current = this;

    // finds the highest initializer in the chain
    while (current != null) {
      if (current.methods.containsKey("init")) {
        initializer = current.methods.get("init");
      }
      current = current.superclass;
    }
    if (initializer == null) return 0;
    return initializer.arity();
  }
  // ~~~~~~ challenge 2 end ~~~~~~
}
