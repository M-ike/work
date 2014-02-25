#include "ucl.h"
#include "ast.h"
#include "stmt.h"
#include "expr.h"
#include "decl.h"
#include "gen.h"

static void TranslateStatement(AstStatement stmt);

/**
 * This function decides if an expression has side effect.
 * Side effect means changes in the state of execution environment.
 * e.g. modifying an object or accessing a volatile object
 */
static int HasSideEffect(AstExpression expr)
{
	if (expr == NULL)
		return 0;

	// accessing a volatile object
	if (expr->op == OP_ID && expr->ty->qual & VOLATILE)
		return 1;

	// function call, a function may not have side effect but ucc doesn't check this
	if (expr->op == OP_CALL)
		return 1;

	// modifying an object
	if (expr->op >= OP_ASSIGN && expr->op <= OP_MOD_ASSIGN ||
	    expr->op == OP_PREINC  || expr->op == OP_PREDEC ||
	    expr->op == OP_POSTINC || expr->op == OP_POSTDEC)
		return 1;

	return HasSideEffect(expr->kids[0]) || HasSideEffect(expr->kids[1]);
}

/**
 * This function translates an expression statement
 */
static void TranslateExpressionStatement(AstStatement stmt)
{
	AstExpressionStatement exprStmt = AsExpr(stmt);

	if (exprStmt->expr != NULL)
	{
		TranslateExpression(exprStmt->expr);
	}
}

/**
 * This function translates a labeled statement
 */
static void TranslateLabelStatement(AstStatement stmt)
{
	AstLabelStatement labelStmt = AsLabel(stmt);

	// ignore unreferenced label
	if (labelStmt->label->ref > 0)
	{
		/// if the label is not associated with a basic block,
		/// create a new basic block to associate with it
		if (labelStmt->label->respBB == NULL)
		{
			labelStmt->label->respBB = CreateBBlock();
		}
		StartBBlock(labelStmt->label->respBB);
	}
	TranslateStatement(labelStmt->stmt);
}

/**
 * This function translates a case statement.
 */
static void TranslateCaseStatement(AstStatement stmt)
{
	AstCaseStatement caseStmt = AsCase(stmt);

	/// see TranslateSwitchStatement, which already creates a basic block
	/// to associate with each case statement in a switch
	StartBBlock(caseStmt->respBB);
	TranslateStatement(caseStmt->stmt);
}

/**
 * This function translates a default statement
 */
static void TranslateDefaultStatement(AstStatement stmt)
{
	AstDefaultStatement defStmt = AsDef(stmt);
	
	/// see TranslateSwitchStatement, which already creates a basic block
	/// to associate with the default statement in a switch
	StartBBlock(defStmt->respBB);
	TranslateStatement(defStmt->stmt);
}

/**
 * This function translates an if statement.
 *
 * if (expr) stmt is translated into:
 *     if ! expr goto nextBB
 * trueBB:
 *     stmt
 * nextBB:
 *     ...     
 *
 * if (expr) stmt1 else stmt2 is translated into:
 *     if ! expr goto falseBB
 * trueBB:
 *     stmt1
 *     goto nextBB
 * falseBB:
 *     stmt2
 * nextBB:
 *     ...
 */
static void TranslateIfStatement(AstStatement stmt)
{
	AstIfStatement ifStmt = AsIf(stmt);
	BBlock nextBB;
	BBlock trueBB;
	BBlock falseBB;

	nextBB = CreateBBlock();
	trueBB = CreateBBlock();

	if (ifStmt->elseStmt == NULL)
	{
		TranslateBranch(Not(ifStmt->expr), nextBB, trueBB);

		StartBBlock(trueBB);
		TranslateStatement(ifStmt->thenStmt);
	}
	else
	{
		falseBB = CreateBBlock();

		TranslateBranch(Not(ifStmt->expr), falseBB, trueBB);

		StartBBlock(trueBB);
		TranslateStatement(ifStmt->thenStmt);
		GenerateJump(nextBB);

		StartBBlock(falseBB);
		TranslateStatement(ifStmt->elseStmt);
	}

	StartBBlock(nextBB);
}

/**
 * This function translates a while statement.
 *
 * while (expr) stmt is translated into:
 * goto contBB
 * loopBB:
 *     stmt
 * contBB:
 *     if (expr) goto loopBB
 * nextBB:
 *     ...
 */
static void TranslateWhileStatement(AstStatement stmt)
{
	AstLoopStatement whileStmt = AsLoop(stmt);

	whileStmt->loopBB = CreateBBlock();
	whileStmt->contBB = CreateBBlock();
	whileStmt->nextBB = CreateBBlock();

	GenerateJump(whileStmt->contBB);

	StartBBlock(whileStmt->loopBB);
	TranslateStatement(whileStmt->stmt);

	StartBBlock(whileStmt->contBB);
	TranslateBranch(whileStmt->expr, whileStmt->loopBB, whileStmt->nextBB);

	StartBBlock(whileStmt->nextBB);
}

/**
 * This function translates a do statement.
 *
 * do stmt while (expr) is translated into:
 * loopBB:
 *     stmt
 * contBB:
 *     if (expr) goto loopBB
 * nextBB:
 *     ...
 */
static void TranslateDoStatement(AstStatement stmt)
{
	AstLoopStatement doStmt = AsLoop(stmt);

	doStmt->loopBB = CreateBBlock();
	doStmt->contBB = CreateBBlock();
	doStmt->nextBB = CreateBBlock();

	StartBBlock(doStmt->loopBB);
	TranslateStatement(doStmt->stmt);

	StartBBlock(doStmt->contBB);
	TranslateBranch(doStmt->expr, doStmt->loopBB, doStmt->nextBB);

	StartBBlock(doStmt->nextBB);
}

/**
 * This function translates a for statement.
 *
 * for (expr1; expr2; expr3) stmt is translated into
 *     expr1
 *     goto testBB
 * loopBB:
 *     stmt
 * contBB:
 *     expr3
 * testBB:
 *     if expr2 goto loopBB (goto loopBB if expr2 is NULL)
 * nextBB:
 *     ...
 * Please pay attention to the difference between for and while
 * The continue point and loop test point is same for a while statemnt,
 * but different for a for statment.
 */
static void TranslateForStatement(AstStatement stmt)
{
	AstForStatement forStmt = AsFor(stmt);

	forStmt->loopBB = CreateBBlock();
	forStmt->contBB = CreateBBlock();
	forStmt->testBB = CreateBBlock();
	forStmt->nextBB = CreateBBlock();

	if (forStmt->initExpr)
	{
		TranslateExpression(forStmt->initExpr);
	}
	GenerateJump(forStmt->testBB);

	StartBBlock(forStmt->loopBB);
	TranslateStatement(forStmt->stmt);

	StartBBlock(forStmt->contBB);
	if (forStmt->incrExpr)
	{
		TranslateExpression(forStmt->incrExpr);
	}

	StartBBlock(forStmt->testBB);
	if (forStmt->expr)
	{
		TranslateBranch(forStmt->expr, forStmt->loopBB, forStmt->nextBB);
	}
	else
	{
		GenerateJump(forStmt->loopBB);
	}

	StartBBlock(forStmt->nextBB);
}

/**
 * This funtion translates a goto statement
 *
 * goto label is translation into:
 *     goto labelBB
 * nextBB:
 *     ...
 */
static void TranslateGotoStatement(AstStatement stmt)
{
	AstGotoStatement gotoStmt = AsGoto(stmt);

	/// if there is no basic block associated with the label, 
	/// create a basic block to associate with it
	if (gotoStmt->label->respBB == NULL)
	{
		gotoStmt->label->respBB = CreateBBlock();
	}
	GenerateJump(gotoStmt->label->respBB);
	StartBBlock(CreateBBlock());
}

/**
 * This function translates a break statement.
 * A break statement terminates the execution of associated
 * switch or loop.
 *
 * break is translated into:
 *     goto switch or loop's nextBB
 * nextBB:
 *     ...
 */
static void TranslateBreakStatement(AstStatement stmt)
{
	AstBreakStatement brkStmt = AsBreak(stmt);

	if (brkStmt->target->kind == NK_SwitchStatement)
	{
		GenerateJump(AsSwitch(brkStmt->target)->nextBB);
	}
	else
	{
		GenerateJump(AsLoop(brkStmt->target)->nextBB);
	}
	StartBBlock(CreateBBlock());
}

/**
 * This function translates a continue statement.
 * A continue statement causes next iteration of associated loop.
 *
 * continue is translate into:
 *    goto loop's contBB
 * nextBB:
 *    ...
 */
static void TranslateContinueStatement(AstStatement stmt)
{
	AstContinueStatement contStmt = AsCont(stmt);

	GenerateJump(contStmt->target->contBB);
	StartBBlock(CreateBBlock());
}

/**
 * Translates a return statement.
 * A return statement terminates execution of current function.
 */
static void TranslateReturnStatement(AstStatement stmt)
{
	AstReturnStatement retStmt = AsRet(stmt);

	if (retStmt->expr)
	{
		GenerateReturn(retStmt->expr->ty, TranslateExpression(retStmt->expr));
	}
	GenerateJump(FSYM->exitBB);
	StartBBlock(CreateBBlock());
}

/**
 * Merge the switch buckets.
 */
static int MergeSwitchBucket(SwitchBucket *pBucket)
{
	SwitchBucket bucket = *pBucket;
	int count = 0;

	while (bucket->prev)
	{
		if ((bucket->prev->ncase + bucket->ncase + 1) * 2 <= (bucket->maxVal - bucket->prev->minVal))
			break;

		bucket->prev->ncase += bucket->ncase;
		bucket->prev->maxVal = bucket->maxVal;
		*bucket->prev->tail = bucket->cases;
		bucket->prev->tail = bucket->tail;
		bucket= bucket->prev;
		count++;
	}

	*pBucket = bucket;
	return count;
}

/**
 * Generates selection and jump code for an array of switch buckets using a binary search.
 * Given the following bucket array:
 * [0, 1, 2] [9, 11] [24]
 * First select [9, 11].
 * if choice < 9, goto left half [0, 1, 2]
 * if choice > 11, goto right half [24]
 * generate indirect jump to each case statement and default label
 */
static void TranslateSwitchBuckets(SwitchBucket *bucketArray, int left, int right, 
                                   Symbol choice, BBlock currBB, BBlock defBB)
{
	int mid, len, i;
	AstCaseStatement p;
	BBlock lhalfBB, rhalfBB;
	BBlock *dstBBs;
	Symbol index;

	if (left > right)
		return;

	mid = (left + right) / 2;
	lhalfBB = (left > mid - 1) ?  defBB : CreateBBlock();
	rhalfBB = (mid + 1 > right) ? defBB : CreateBBlock();

	len = bucketArray[mid]->maxVal - bucketArray[mid]->minVal + 1;

	dstBBs = HeapAllocate(CurrentHeap, (len + 1)* sizeof(BBlock));
	for (i = 0; i < len; ++i)
		dstBBs[i] = defBB;
	dstBBs[len] = NULL;

	p = bucketArray[mid]->cases;
	while (p)
	{
		i = p->expr->val.i[0] - bucketArray[mid]->minVal;
		dstBBs[i] = p->respBB;
		p = p->nextCase;
	}

	if (currBB != NULL)
	{
		StartBBlock(currBB);
	}
	GenerateBranch(choice->ty, lhalfBB, JL, choice, IntConstant(bucketArray[mid]->minVal));

	StartBBlock(CreateBBlock());
	GenerateBranch(choice->ty, rhalfBB, JG, choice, IntConstant(bucketArray[mid]->maxVal));

	StartBBlock(CreateBBlock());

	if (len != 1)
	{
		index = CreateTemp(choice->ty);
		GenerateAssign(choice->ty, index, SUB, choice, IntConstant(bucketArray[mid]->minVal));
		GenerateIndirectJump(dstBBs, len, index);
	}
	else
	{
		GenerateJump(dstBBs[0]);
	}

	StartBBlock(CreateBBlock());

	TranslateSwitchBuckets(bucketArray, left, mid - 1, choice, lhalfBB, defBB);
	TranslateSwitchBuckets(bucketArray, mid + 1, right, choice, rhalfBB, defBB);

}

/**
 * Generate intermediate code for switch statement.
 * During semantic check, the case statements in a switch statement is already ordered in ascending order.
 * The first step of this function is to divide these case statements into switch buckets.
 * The dividing criteria is:
 * Each switch bucket holds some case statements, there must be a case statement with minimum value(minVal)
 * and a case statement with maximum value(maxVal). We define the density of the switch bucket to be:
 * density = number of case statements / (maxVal - minVal). The density of a switch bucket must be greater
 * than 1/2.
 * And when adding new case statements into a switch bucket, there is a chance that the switch bucket can be 
 * merged with previous switch buckets.
 * 
 * Given the following switch statement:
 * switch (a) { case 0: case 1: case 4: case 9: case 10: case 11: ... };
 * [0, 1, 4] will be the first bucket, since 3 / (4 - 0) > 1/2. 
 * 9 will starts a new bucket, since 4 / (9 - 0) < 1/2. But when encountering 11, 6 / (11 - 0) > 1/2
 * So the merged bucket will be [0, 1, 4, 9, 10, 11]
 *
 * The second step generates the selection and jump code to different switch buckets(See TranslateSwitchBuckets)
 * 
 * The final step generates the intermediate code for the enclosed statement.
 */
static void TranslateSwitchStatement(AstStatement stmt)
{
	AstSwitchStatement swtchStmt = AsSwitch(stmt);
	AstCaseStatement p, q;
	SwitchBucket bucket, b;
	SwitchBucket *bucketArray;
	int i, val;
	Symbol sym;

	sym = TranslateExpression(swtchStmt->expr);

	bucket = b = NULL;
	p = swtchStmt->cases;
	while (p)
	{
		q = p;
		p = p->nextCase;

		q->respBB = CreateBBlock();
		val = q->expr->val.i[0];
		if (bucket && (bucket->ncase + 1) * 2 > (val - bucket->minVal))
		{
			bucket->ncase++;
			bucket->maxVal = val;
			*bucket->tail = q;
			bucket->tail = &(q->nextCase);
			swtchStmt->nbucket -= MergeSwitchBucket(&bucket);
		}
		else
		{
			ALLOC(b);

			b->cases = q;
			b->ncase = 1;
			b->minVal = b->maxVal = val;
			b->tail = &(q->nextCase);
			b->prev = bucket;
			bucket = b;
			swtchStmt->nbucket++;
		}
	}
	swtchStmt->buckets = bucket;

	bucketArray = HeapAllocate(CurrentHeap, swtchStmt->nbucket * sizeof(SwitchBucket));

	for (i = swtchStmt->nbucket - 1; i >= 0; i--)
	{
		bucketArray[i] = bucket;
		*bucket->tail = NULL;
		bucket = bucket->prev;
	}

	swtchStmt->defBB = CreateBBlock();
	if (swtchStmt->defStmt)
	{
		swtchStmt->defStmt->respBB = swtchStmt->defBB;
		swtchStmt->nextBB = CreateBBlock();
	}
	else
	{
		swtchStmt->nextBB = swtchStmt->defBB;
	}
	TranslateSwitchBuckets(bucketArray, 0, swtchStmt->nbucket - 1, sym, NULL, swtchStmt->defBB);
	TranslateStatement(swtchStmt->stmt);
	StartBBlock(swtchStmt->nextBB);
}

static void TranslateCompoundStatement(AstStatement stmt)
{
	AstCompoundStatement compStmt = AsComp(stmt);
	AstNode p = compStmt->stmts;
	Vector ilocals = compStmt->ilocals;
	Symbol v;
	int i;

	for (i = 0; i < LEN(ilocals); ++i)
	{
		InitData initd;
		Type ty;
		Symbol dst, src;
		int size;

		v = GET_ITEM(ilocals, i);
		initd = AsVar(v)->idata;
		size = 0;
		while (initd != NULL)
		{
			if (initd->offset != size)
			{
				dst = CreateOffset(T(UCHAR), v, size);
				GenerateClear(dst, initd->offset - size);
			}
			ty = initd->expr->ty;
			if (initd->expr->op == OP_STR)
			{
				String str = initd->expr->val.p;

				src = AddString(ArrayOf(str->len + 1, ty->bty), str);
			}
			else
			{
				src = TranslateExpression(initd->expr);
			}
			dst = CreateOffset(ty, v, initd->offset);
			GenerateMove(ty, dst, src);

			size = initd->offset + ty->size;
			initd = initd->next;
		}
		if (size < v->ty->size)
		{
			dst = CreateOffset(T(UCHAR), v, size);
			GenerateClear(dst, v->ty->size - size);
		}
	}

	while (p)
	{
		TranslateStatement((AstStatement)p);
		p = p->next;
	}

}

static void (* StmtTrans[])(AstStatement) = 
{
	TranslateExpressionStatement,
	TranslateLabelStatement,
	TranslateCaseStatement,
	TranslateDefaultStatement,
	TranslateIfStatement,
	TranslateSwitchStatement,
	TranslateWhileStatement,
	TranslateDoStatement,
	TranslateForStatement,
	TranslateGotoStatement,
	TranslateBreakStatement,
	TranslateContinueStatement,
	TranslateReturnStatement,
	TranslateCompoundStatement
};

static void TranslateStatement(AstStatement stmt)
{
	(* StmtTrans[stmt->kind - NK_ExpressionStatement])(stmt);
}

static void TranslateFunction(AstFunction func)
{
	BBlock bb;

	FSYM = func->fsym;
	if (! FSYM->defined)
		return;

	TempNum = 0;
	FSYM->entryBB = CreateBBlock();
	FSYM->exitBB = CreateBBlock();

	CurrentBB = FSYM->entryBB;
	TranslateStatement(func->stmt);
	StartBBlock(FSYM->exitBB);

	Optimize(FSYM);

	bb = FSYM->entryBB;
	while (bb != NULL)
	{
		if (bb->ref != 0)
		{
			bb->sym = CreateLabel();
		}
		bb = bb->next;
	}
}

void Translate(AstTranslationUnit transUnit)
{
	AstNode p = transUnit->extDecls;

	while (p)
	{
		if (p->kind == NK_Function && ((AstFunction)p)->stmt)
		{
			TranslateFunction((AstFunction)p);
		}
		p = p->next;
	}
}
