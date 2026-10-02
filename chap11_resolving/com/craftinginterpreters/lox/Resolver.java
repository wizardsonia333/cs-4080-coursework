package com.craftinginterpreters.lox;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Stack;

class Resolver implements Expr.Visitor<Void>, Stmt.Visitor<Void> {
  // ~~~~~~ challenge 3 start ~~~~~~
  // keeps track of where a local was declared & if it gets used
  private static class LocalVariable {
    final Token name;

    // ~~~~~~ challenge 4 start ~~~~~~
    // gives each local its spot in the env
    final int slot;
    // ~~~~~~ challenge 4 end ~~~~~~

    LocalState state;
    // ~~~~~~ challenge 4 start ~~~~~~
    // old:
    // LocalVariable(Token name, LocalState state) {
    //   this.name = name;
    //   this.state = state;
    // }
    LocalVariable(Token name, LocalState state, int slot) {
      this.name = name;
      this.state = state;
      this.slot = slot;
    }
    // ~~~~~~ challenge 4 end ~~~~~~
  }
  private enum LocalState {
    DECLARED,
    DEFINED,
    READ
  }
  // ~~~~~~ challenge 3 end ~~~~~~
  
  private final Interpreter interpreter;
  // ~~~~~~ challenge 3 start ~~~~~~
  // old: private final Stack<Map<String, Boolean>> scopes = new Stack<>();
  private final Stack<Map<String, LocalVariable>> scopes = new Stack<>();
  // ~~~~~~ challenge 3 end ~~~~~~
  private FunctionType currentFunction = FunctionType.NONE;

  Resolver(Interpreter interpreter) {
    this.interpreter = interpreter;
  }
  private enum FunctionType {
    NONE,
    FUNCTION
  }
  void resolve(List<Stmt> statements) {
    for (Stmt statement : statements) {
      resolve(statement);
    }
  }
  @Override
  public Void visitBlockStmt(Stmt.Block stmt) {
    beginScope();
    resolve(stmt.statements);
    endScope();
    return null;
  }
  @Override
  public Void visitExpressionStmt(Stmt.Expression stmt) {
    resolve(stmt.expression);
    return null;
  }
  @Override
  public Void visitFunctionStmt(Stmt.Function stmt) {
    declare(stmt.name);
    define(stmt.name);

    resolveFunction(stmt, FunctionType.FUNCTION);
    return null;
  }
  @Override
  public Void visitIfStmt(Stmt.If stmt) {
    resolve(stmt.condition);
    resolve(stmt.thenBranch);
    if (stmt.elseBranch != null) resolve(stmt.elseBranch);
    return null;
  }
  @Override
  public Void visitPrintStmt(Stmt.Print stmt) {
    resolve(stmt.expression);
    return null;
  }
  @Override
  public Void visitReturnStmt(Stmt.Return stmt) {
    if (currentFunction == FunctionType.NONE) {
      Lox.error(stmt.keyword, "Can't return from top-level code.");
    }

    if (stmt.value != null) {
      resolve(stmt.value);
    }

    return null;
  }
  @Override
  public Void visitVarStmt(Stmt.Var stmt) {
    declare(stmt.name);
    if (stmt.initializer != null) {
      resolve(stmt.initializer);
    }
    define(stmt.name);
    return null;
  }
  @Override
  public Void visitWhileStmt(Stmt.While stmt) {
    resolve(stmt.condition);
    resolve(stmt.body);
    return null;
  }
  @Override
  public Void visitAssignExpr(Expr.Assign expr) {
    resolve(expr.value);
    // ~~~~~~ challenge 3 start ~~~~~~
    // old: resolveLocal(expr, expr.name);
    resolveLocal(expr, expr.name, false);
    // ~~~~~~ challenge 3 end ~~~~~~
    return null;
  }
  @Override
  public Void visitBinaryExpr(Expr.Binary expr) {
    resolve(expr.left);
    resolve(expr.right);
    return null;
  }
  @Override
  public Void visitCallExpr(Expr.Call expr) {
    resolve(expr.callee);

    for (Expr argument : expr.arguments) {
      resolve(argument);
    }

    return null;
  }
  @Override
  public Void visitGroupingExpr(Expr.Grouping expr) {
    resolve(expr.expression);
    return null;
  }
  @Override
  public Void visitLiteralExpr(Expr.Literal expr) {
    return null;
  }
  @Override
  public Void visitLogicalExpr(Expr.Logical expr) {
    resolve(expr.left);
    resolve(expr.right);
    return null;
  }
  @Override
  public Void visitUnaryExpr(Expr.Unary expr) {
    resolve(expr.right);
    return null;
  }
  @Override
  public Void visitVariableExpr(Expr.Variable expr) {
    // ~~~~~~ challenge 3 start ~~~~~~
    // old:
    // if (!scopes.isEmpty() &&
    //     scopes.peek().get(expr.name.lexeme) == Boolean.FALSE) {
    //   Lox.error(expr.name,
    //       "Can't read local variable in its own initializer.");
    // }
    LocalVariable local = scopes.isEmpty()
        ? null
        : scopes.peek().get(expr.name.lexeme);

    if (local != null && local.state == LocalState.DECLARED) {
      Lox.error(expr.name,
          "Can't read local variable in its own initializer.");
    }
    // old: resolveLocal(expr, expr.name);
    resolveLocal(expr, expr.name, true);
    // ~~~~~~ challenge 3 end ~~~~~~
    return null;
  }

  private void resolve(Stmt stmt) {
    stmt.accept(this);
  }
  private void resolve(Expr expr) {
    expr.accept(this);
  }
  private void resolveFunction(
      Stmt.Function function, FunctionType type) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();
    for (Token param : function.params) {
      declare(param);
      define(param);
    }
    resolve(function.body);
    endScope();
    currentFunction = enclosingFunction;
  }
  private void beginScope() {
    // ~~~~~~ challenge 3 start ~~~~~~
    // old: scopes.push(new HashMap<String, Boolean>());
    scopes.push(new HashMap<String, LocalVariable>());
    // ~~~~~~ challenge 3 end ~~~~~~
  }
  private void endScope() {
    // ~~~~~~ challenge 3 start ~~~~~~
    // old: scopes.pop();
    Map<String, LocalVariable> finishedScope = scopes.pop();

    for (LocalVariable local : finishedScope.values()) {
      if (local.state == LocalState.DEFINED) {
        Lox.error(local.name, "Local variable is never used.");
      }
    }
    // ~~~~~~ challenge 3 end ~~~~~~
  }

  private void declare(Token name) {
    if (scopes.isEmpty()) return;

    // ~~~~~~ challenge 3 start ~~~~~~
    // old: Map<String, Boolean> scope = scopes.peek();
    Map<String, LocalVariable> scope = scopes.peek();
    // ~~~~~~ challenge 3 end ~~~~~~

    if (scope.containsKey(name.lexeme)) {
      Lox.error(name,
          "Already a variable with this name in this scope.");
    }

    // ~~~~~~ challenge 3 start ~~~~~~
    // old: scope.put(name.lexeme, false);
    
    // ~~~~~~ challenge 4 start ~~~~~~
    // old:
    // scope.put(name.lexeme,
    //     new LocalVariable(name, LocalState.DECLARED));

    // uses the next spot in this scope for the var
    scope.put(name.lexeme,
        new LocalVariable(name, LocalState.DECLARED, scope.size()));
    // ~~~~~~ challenge 4 end ~~~~~~

    // ~~~~~~ challenge 3 end ~~~~~~
  }

  private void define(Token name) {
    if (scopes.isEmpty()) return;
    // ~~~~~~ challenge 3 start ~~~~~~
    // old: scopes.peek().put(name.lexeme, true);
    scopes.peek().get(name.lexeme).state = LocalState.DEFINED;
    // ~~~~~~ challenge 3 end ~~~~~~
  }
  // ~~~~~~ challenge 3 start ~~~~~~
  // old method:
  // private void resolveLocal(Expr expr, Token name) {
  //   for (int i = scopes.size() - 1; i >= 0; i--) {
  //     if (scopes.get(i).containsKey(name.lexeme)) {
  //       interpreter.resolve(expr, scopes.size() - 1 - i);
  //       return;
  //     }
  //   }
  // }
  private void resolveLocal(Expr expr, Token name, boolean markUsed) {
    for (int i = scopes.size() - 1; i >= 0; i--) {
      Map<String, LocalVariable> scope = scopes.get(i);

      if (scope.containsKey(name.lexeme)) {
        // ~~~~~~ challenge 4 start ~~~~~~
        LocalVariable local = scope.get(name.lexeme);

        // old:
        // interpreter.resolve(expr, scopes.size() - 1 - i);
        interpreter.resolve(expr, scopes.size() - 1 - i, local.slot);
        // ~~~~~~ challenge 4 end ~~~~~~

        if (markUsed) {
          local.state = LocalState.READ;
        }
        return;
      }
    }
  }
  // ~~~~~~ challenge 3 end ~~~~~~
}