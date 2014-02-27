#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "ast_classes.h"

#include "ast.h"
#include <string>
#include <list>

/*
  something like a distributed nameserver
  all symbol tables are in a single linked list
  the symbol visibility is local and global in all parent tables
*/
class SymbolTable
{
   public:
       enum SymbolType
       {
          CONSTANT,
          VARIABLE,
          PROCEDURE
       };


       typedef struct symbol
       {
             std::string name;
             enum SymbolType type;
             int nrParameters;  //only relevant for procedures
             int level;
             int offset; //only relevant for variables
             AST::ASTTree *reference;
             SymbolTable *savedAtSymbolTable;
       } symbol_t;

      SymbolTable(SymbolTable *parent, int level): parentSymbolTable(parent), level(level), varOffset(0) {}

      //register symbols while definition
      bool registerSymbol(AST::ConstantDeclaration *constant);
      bool registerSymbol(AST::VariableDeclaration *variable);
      bool registerSymbol(AST::ProcedureDeclaration *procedure);

      //register symbols while use/reference
      bool registerReferencedVariable(std::string name);
      bool registerReferencedConstant(std::string name);
      bool registerReferencedProcedure(std::string name, int nrParameters);

      //search and find symbols
      bool findConstant(std::string name, symbol_t *symbolReference);
      bool findVariable(std::string name, symbol_t *symbolReference);
      bool findProcedure(std::string name, int nrParameters, symbol_t *symbolReference);

      std::list<symbol_t> getDirectOwnedSymbols() { return ownedSymbols; }
      std::list<symbol_t> getIndirectKnownSymbols() { return indirectKnownSymbols; }

      std::string name() { return _name; }
      void setName(std::string name) { _name = name; }

      SymbolTable *getParentSymbolTable() { return parentSymbolTable; }

      int getLevel() { return level; }

   private:
      SymbolTable *parentSymbolTable;
      int level;
      int varOffset;

      std::list<symbol_t> ownedSymbols;
      std::list<symbol_t> indirectKnownSymbols;

      std::string _name;

      bool registerSymbol(std::string name, enum SymbolType type, int nrParameters, AST::ASTTree *reference);
      bool registerReferenced(std::string name, int nrParameters, enum SymbolType type);

      bool findSymbol            (std::string name, int nrParameters, symbol_t *symbolReference);
      bool findSymbolLocal       (std::string name, int nrParameters, symbol_t *symbolReference);
      bool findSymbolKnownGlobal (std::string name, int nrParameters, symbol_t *symbolReference);
      bool findSymbolGlobal      (std::string name, int nrParameters, symbol_t *symbolReference);
};

#endif
