#include <iostream>
#include <QSplitter>
#include <QScrollBar>
#include <QListWidget>
#include <QListWidgetItem>

#include "codewindow.h"
#include <QAbstractScrollArea>

CodeWindow::CodeWindow(MainWindow *parent)
  : QDockWidget(parent)
{
   this->parent = parent;
   LanguageManager::getInstance()->registerWidget(this);

   labelTextEdit = new QListWidget();
   labelTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

   codeTextEdit = new QListWidget();

   QWidget *labelWidget = new QWidget();
   QWidget *codeWidget = new QWidget();
   leftLabel = new QLabel();
   rightLabel = new QLabel();
   QVBoxLayout *labelLayout = new QVBoxLayout();
   QVBoxLayout *codeLayout = new QVBoxLayout();
   labelLayout->setContentsMargins(0,2,0,0);
   codeLayout->setContentsMargins(0,2,0,0);

   labelLayout->addWidget(leftLabel);
   labelLayout->addWidget(labelTextEdit);
   labelWidget->setLayout(labelLayout);

   codeLayout->addWidget(rightLabel);
   codeLayout->addWidget(codeTextEdit);
   codeWidget->setLayout(codeLayout);


   changeAction = new QAction(QString(), this);
   connect(codeTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(changeCodeScrollbarAction(int)));
   connect(codeTextEdit, SIGNAL(currentRowChanged(int)), this, SLOT(changeCodeWindowCurrentRow(int)));
   connect(labelTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(changeLabelScrollbarAction(int)));
   connect(labelTextEdit, SIGNAL(currentRowChanged(int)), this, SLOT(changeLabelWindowCurrentRow(int)));

   QSplitter *splitter = new QSplitter;
   splitter->addWidget(labelWidget);
   splitter->addWidget(codeWidget);
   splitter->setStretchFactor(0, 1);
   splitter->setStretchFactor(1, 1);
   splitter->setCollapsible (0, false);
   splitter->setCollapsible (1, false);

   setWidget(splitter);

   languageNotify();
}


void CodeWindow::languageNotify()
{
    setWindowTitle( tr("Code") );

    leftLabel->setText(tr("Label"));
    rightLabel->setText(tr("Operation"));
}


void CodeWindow::showAssemblerCode(std::list<instruction_t> instructions)
{
    if (instructions.empty()) return;

    QString labels;
    QListWidgetItem *labelItem;
    QListWidgetItem *codeItem;
    QString asmInstructions;

    std::list<instruction_t>::iterator it;
    for( it = instructions.begin(); it != instructions.end(); it++ )
    {
       labels.append(QString::fromStdString(it->label)); labels.append("\n");
       labelItem = new QListWidgetItem();
       labelItem->setTextAlignment(Qt::AlignRight);
       labelItem->setText(QString::fromStdString(it->label));
       labelTextEdit->addItem(labelItem);

       codeItem = new QListWidgetItem();
       codeItem->setText(convertInstructionToQString(*it));
       codeTextEdit->addItem(codeItem);
      }
}


QString CodeWindow::convertInstructionToQString(instruction_t instruction)
{
   QString instr;

   switch (instruction.operation.opcode)
   {
       case LIT:   {  instr.append("LIT "); instr.append(QString::fromStdString(instruction.operation.operand)); break; }
       case LOD:   {  instr.append("LOD "); instr.append(QString::fromStdString(instruction.operation.operand)); break; }
       case STO:   {  instr.append("STO "); instr.append(QString::fromStdString(instruction.operation.operand)); break; }
       case CALL:   {  instr.append("CAL "); instr.append(QString::fromStdString(instruction.operation.operand)); break; }
       case INTT:   {  instr.append("INT "); instr.append(QString::fromStdString(instruction.operation.operand)); break; }
       case JMP:   {  instr.append("JMP "); instr.append(QString::fromStdString(instruction.operation.operand)); break; }
       case JPC:   {  instr.append("JPC "); instr.append(QString::fromStdString(instruction.operation.operand)); break; }
       case OPR:
       {
           instr.append("OPR ");
           switch (instruction.operation.identity)
           {
                case ADD: { instr.append("ADD"); break; }
                case SUB: { instr.append("SUB"); break; }
                case MULT: { instr.append("MULT"); break; }
                case DIV: { instr.append("DIV"); break; }
                case RET: { instr.append("RET"); break; }
                case HALT: { instr.append("HALT"); break; }
                case NOP: { instr.append("NOP"); break; }
                case NOT: { instr.append("NOT"); break; }
                case ODD: { instr.append("ODD"); break; }
                case EQUAL: { instr.append("EQUAL"); break; }
                case NEQUAL: { instr.append("NEQUAL"); break; }
                case GREATER: { instr.append("GREATER"); break; }
                case GEQUAL: { instr.append("GEQUAL"); break; }
                case LESS: { instr.append("LESS"); break; }
                case LEQUAL: { instr.append("LEQUAL"); break; }
                case STDIN: { instr.append("STDIN"); break; }
                case STDOUT: { instr.append("STDOUT"); break; }
                default: { instr.append("<IDENTITY>"); break; }
           }
           break;
       }
       default: { instr.append("<OPCODE>"); break; }
   }

   return instr;
}

void CodeWindow::changeCodeScrollbarAction(int pos)
{
    QScrollBar *sbl = labelTextEdit->verticalScrollBar();
    sbl->setValue( pos );
}

void CodeWindow::changeLabelScrollbarAction(int pos)
{
    QScrollBar *sbr = codeTextEdit->verticalScrollBar();
    sbr->setValue( pos );
}

void CodeWindow::clearListWidget()
{
   labelTextEdit->clear();
   codeTextEdit->clear();
}

void CodeWindow::changeCodeWindowCurrentRow(int row)
{
   labelTextEdit->setCurrentRow(row);
}

void CodeWindow::changeLabelWindowCurrentRow(int row)
{
   codeTextEdit->setCurrentRow(row);
}

