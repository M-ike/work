#include <QWidget>
#include <QTextEdit>
#include <QHBoxLayout>
#include <iostream>

#include "grammarwindow.h"

GrammarWindow::GrammarWindow(MainWindow *parent)
   :QWidget()
{
    this->parent = parent;
    grammarWindow = new QWidget(this);
    init();
    languageNotify();
}


void GrammarWindow::init()
{
    setSizeIncrement(1, 1);
    setBaseSize(600,400);
    grammarTextEdit = new QTextEdit();
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(grammarTextEdit);
    setLayout(layout);
    QString grammar =  "Program -> Block \".\" \n\n";

    grammar.append("Block -> [Constants] [Variables] [Procedures] Statement\n\n");
    grammar.append("Constants -> \"CONST\" ConstantDeclaration (\",\" ConstantDeclaration)* \";\"\n\n");
    grammar.append("Variables -> \"VAR\" VariableDeclaration (\",\" VariableDeclaration)* \";\"\n\n");
    grammar.append("Procedures -> (ProcedureDeclaration)+\n\n");
    grammar.append("ConstantDeclaration -> Ident \"=\" Number\n\n");
    grammar.append("VariableDeclaration -> Ident\n\n");
    grammar.append("ProcedureDeclaration -> \"PROCEDURE\" Ident \"(\" [FormalParameters] \")\" Block \";\"\n\n");
    grammar.append("FormalParameters -> VariableDeclaration (\",\" VariableDeclaration)*\n\n");
    grammar.append("Statement -> AssignmentStatement | InputStatement | OutputStatement | CallStatement |\n\n");
    grammar.append("             CompoundStatement | IfStatement | WhileStatemen\n\n");
    grammar.append("AssignmentStatement -> Ident \":=\" Expression\n\n");
    grammar.append("InputStatement -> \"?\" Ident\n\n");
    grammar.append("OutputStatement -> \"!\" Expression\n\n");
    grammar.append("CallStatement -> \"CALL\" Ident \"(\" [ActualParameters] \")\"\n\n");
    grammar.append("ActualParameters -> Expression (\",\" Expression)*\n\n");
    grammar.append("CompoundStatement -> \"BEGIN\" Statement ( \";\" Statement) \"END\"\n\n");
    grammar.append("IfStatement -> \"IF\" Condition \"THEN\" Statement\n\n");
    grammar.append("WhileStatement -> \"WHILE\" Condition \"DO\" Statement\n\n");
    grammar.append("Expression -> FirstTerm [ ( \"+\" | \"-\" ) AdditionalExpression ]\n\n");
    grammar.append("FirstTerm = ( \"+\" | \"-\" | eps ) Term\n\n");
    grammar.append("AdditionalExpression -> Term [ (\"+\" | \"-\") AdditionalExpression ]\n\n");
    grammar.append("Term -> Factor [ (\"*\" | \"/\") Term ]\n\n");
    grammar.append("Factor -> Ident | Number | ParenthesisFactor\n\n");
    grammar.append("ParenthesisFactor -> \"(\" Expression \")\"\n\n");
    grammar.append("Condition -> OddCondition | CompareCondition\n\n");
    grammar.append("OddCondition -> \"ODD\" Expression\n\n");
    grammar.append("CompareCondition -> Expression (\"=\" | \"#\" | \"<\" | \"<=\" | \">\" | \">=\") Expression\n\n");
    grammar.append("\n\n");
    grammarTextEdit->setPlainText(grammar);
    //grammarTextEdit->document()->setModified(FALSE);
    grammarTextEdit->setReadOnly(TRUE);
}

void GrammarWindow::languageNotify()
{
    setWindowTitle( tr("Grammar Window") );
}

void GrammarWindow::showGrammar(bool showIt)
{
   if (showIt) show();
   else hide();
}

void GrammarWindow::setVisible(bool visible)
{
   QWidget::setVisible(visible);
   parent->grammarWindowWasSetVisible(visible);
}
