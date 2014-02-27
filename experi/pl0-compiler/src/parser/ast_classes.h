namespace AST
{
     class ASTTree;

     class Program;

     class Block;

     class Constants;
     class ConstantDeclaration;
     class Variables;
     class VariableDeclaration;
     class Procedures;
     class ProcedureDeclaration;
     class FormalParameters;

     class Statement;
     class AssignmentStatement;
     class CallStatement;
     class ActualParameters;
     class CompoundStatement;
     class IfStatement;
     class InputStatement;
     class OutputStatement;
     class WhileStatement;

     class Condition;
     class OddCondition;
     class CompareCondition;
     class EqualCondition;
     class NotEqualCondition;
     class GreaterCondition;
     class GreaterEqualCondition;
     class LessCondition;
     class LessEqualCondition;

     class Expression;
     class PlusExpression;
     class MinusExpression;
     class FirstTermExpression;

     class AdditionalExpression;
     class MinusAdditionalExpression;
     class PlusAdditionalExpression;
     class TermAdditionalExpression;

     class FirstTerm;
     class PlusFirstTerm;
     class MinusFirstTerm;
     class SignlessFirstTerm;

     class Term;
     class MultTerm;
     class DivTerm;
     class FactorTerm;

     class Factor;
     class Ident;
     class Number;
     class ParenthesisFactor;
}
