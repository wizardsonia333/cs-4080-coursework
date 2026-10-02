package com.craftinginterpreters.lox;

// ~~~~~~ challenge 4 start ~~~~~~
// old:
// import java.util.HashMap;
// import java.util.Map;

import java.util.ArrayList;
import java.util.List;
// ~~~~~~ challenge 4 end ~~~~~~

class Environment {
  final Environment enclosing;
  // ~~~~~~ challenge 4 start ~~~~~~
  // old: private final Map<String, Object> values = new HashMap<>();

  // locals are stored by their slot instead of their name
  private final List<Object> values = new ArrayList<>();
  // ~~~~~~ challenge 4 end ~~~~~~
  Environment() {
    enclosing = null;
  }

  Environment(Environment enclosing) {
    this.enclosing = enclosing;
  }

  // ~~~~~~ challenge 4 start ~~~~~~
  // old lookup, not needed anymore
  // Object get(Token name) {
  //   if (values.containsKey(name.lexeme)) {
  //     return values.get(name.lexeme);
  //   }

  //   if (enclosing != null) return enclosing.get(name);

  //   throw new RuntimeError(name,
  //       "Undefined variable '" + name.lexeme + "'.");
  // }

  // void assign(Token name, Object value) {
  //   if (values.containsKey(name.lexeme)) {
  //     values.put(name.lexeme, value);
  //     return;
  //   }

  //   if (enclosing != null) {
  //     enclosing.assign(name, value);
  //     return;
  //   }

  //   throw new RuntimeError(name,
  //       "Undefined variable '" + name.lexeme + "'.");
  // }
  // ~~~~~~ challenge 4 end ~~~~~~

  // ~~~~~~ challenge 4 start ~~~~~~
  // old:
  // void define(String name, Object value) {
  //   values.put(name, value);
  // }

  // adds the value into its next local slot
  void define(Object value) {
    values.add(value);
  }
  // ~~~~~~ challenge 4 end ~~~~~~

  Environment ancestor(int distance) {
    Environment environment = this;
    for (int i = 0; i < distance; i++) {
      environment = environment.enclosing; // [coupled]
    }

    return environment;
  }
  
  // ~~~~~~ challenge 4 start ~~~~~~
  // old:
  // Object getAt(int distance, String name) {
  //   return ancestor(distance).values.get(name);
  // }

  // gets local directly from its numbered spot
  Object getAt(int distance, int slot) {
    return ancestor(distance).values.get(slot);
  }
  // ~~~~~~ challenge 4 end ~~~~~~

  // ~~~~~~ challenge 4 start ~~~~~~
  // old:
  // void assignAt(int distance, Token name, Object value) {
  //   ancestor(distance).values.put(name.lexeme, value);
  // }

  // changes the value already sitting in that slot
  void assignAt(int distance, int slot, Object value) {
    ancestor(distance).values.set(slot, value);
  }
  // ~~~~~~ challenge 4 end ~~~~~~

  @Override
  public String toString() {
    String result = values.toString();
    if (enclosing != null) {
      result += " -> " + enclosing.toString();
    }

    return result;
  }
}
