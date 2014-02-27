#include "symboltable.h"
#include <iostream>


bool SymbolTable::registerSymbol(AST::ConstantDeclaration *constant)
{  return registerSymbol(constant->ident->value, CONSTANT, -1, constant);  }


bool SymbolTable::registerSymbol(AST::VariableDeclaration *variable)
{
   if (! registerSymbol(variable->ident->value, VARIABLE, -1, variable) ) return false;
   varOffset++;
   return true;
}


bool SymbolTable::registerSymbol(AST::ProcedureDeclaration *procedure)
{
   int variables = 0;
   if (procedure->formalParameters != NULL) variables = procedure->formalParameters->getNrParameters();
   return registerSymbol(procedure->ident->value, PROCEDURE, variables, procedure);
}


bool SymbolTable::registerSymbol(std::string name, enum SymbolType type, int nrParameters, AST::ASTTree *reference)
{
   symbol_t symbol;
   symbol.name = name;

   if (findSymbolLocal(name, nrParameters, &symbol)) return false;

   symbol.type = type;
   symbol.nrParameters = nrParameters;
   symbol.level = level;
   symbol.offset = varOffset;
   symbol.reference = reference;
   symbol.savedAtSymbolTable = this;

   ownedSymbols.push_back(symbol);
   return true;
}


bool SymbolTable::findSymbolLocal(std::string name, int nrParameters, symbol_t *symbol)
{
   if (ownedSymbols.empty()) return false;
   std::list<symbol_t>::iterator it;
   for( it = ownedSymbols.begin(); it != ownedSymbols.end(); it++ )
   {
       if (it->name.compare(name) == 0 && it->nrParameters == nrParameters) { *symbol = *it; return true; }
   }
   return false;
}


bool SymbolTable::findSymbolKnownGlobal(std::string name, int nrParameters, symbol_t *symbol)
{
   if (ownedSymbols.empty()) return false;
   std::list<symbol_t>::iterator it;
   for( it = indirectKnownSymbols.begin(); it != indirectKnownSymbols.end(); it++ )
   {
       if (it->name.compare(name) == 0 && it->nrParameters == nrParameters) { *symbol = *it; return true; }
   }
   return false;
}


bool SymbolTable::registerReferenced(std::string name, int nrParameters, enum SymbolType type)
{
   symbol_t symbol;
   symbol.name = name;

   if (findSymbolLocal(name, nrParameters, &symbol))
   { if (symbol.type == type) return true; }

   if (findSymbolKnownGlobal(name, nrParameters, &symbol))
   { if (symbol.type == type) return true; }

   if (!findSymbolGlobal(name, nrParameters, &symbol)) return false;
   if (symbol.type != type) return false;

   indirectKnownSymbols.push_back(symbol);
   return true;
}


bool SymbolTable::registerReferencedVariable(std::string name)
{  return registerReferenced(name, -1, VARIABLE); }


bool SymbolTable::registerReferencedConstant(std::string name)
{  return registerReferenced(name, -1, CONSTANT); }

bool SymbolTable::registerReferencedProcedure(std::string name, int nrParameters)
{  return registerReferenced(name, nrParameters, PROCEDURE); }


bool SymbolTable::findSymbolGlobal(std::string name, int nrParameters, symbol_t *symbol)
{
   if (parentSymbolTable == NULL) return false;
   return parentSymbolTable->findSymbol(name, nrParameters, symbol);
}



bool SymbolTable::findSymbol(std::string name, int nrParameters, symbol_t *symbol)
{
   if (findSymbolLocal(name, nrParameters, symbol)) return true;
   if (findSymbolKnownGlobal(name, nrParameters, symbol)) return true;
   if (findSymbolGlobal(name, nrParameters, symbol)) return true;
   return false;
}


bool SymbolTable::findConstant(std::string name, symbol_t *symbol)
{
   if (!findSymbol(name, -1, symbol)) return false;
   if (symbol->type != CONSTANT) return false;
   return true;
}


bool SymbolTable::findVariable(std::string name, symbol_t *symbol)
{
   if (!findSymbol(name, -1, symbol)) return false;
   if (symbol->type != VARIABLE) return false;
   return true;
}


bool SymbolTable::findProcedure(std::string name, int nrParameters, symbol_t *symbol)
{
   if (!findSymbol(name, nrParameters, symbol)) return false;
   if (symbol->type != PROCEDURE) return false;
   return true;
}
