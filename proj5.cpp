/*
Ellery Leung
1207157168
CSE 340 Project 5
*/

extern "C" {
#include "compiler.h"
}
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>

std::vector<ValueNode*> lhs;
StatementNode *parse_stmt();
StatementNode *parse_stmt_list();
StatementNode *parse_body();

std::vector<ValueNode*> buildLHS()
{
	ValueNode *newNode;
	std::vector<ValueNode*> newVec;
	ttype = getToken();
	while (ttype != SEMICOLON)	// grab all the initializations of variables
	{
		//printf("token is %s\n", token);
		newNode = new ValueNode;
		newNode->name = strdup(token);
		newNode->value = 0;
		newVec.push_back(newNode);
		ttype = getToken();			// Need to grab 2 at a time b/c of comma
		if (ttype != SEMICOLON)
			ttype = getToken();
	}
	//ungetToken();	// Grabs two at a time, so when semicolon is reached, unget the most recent.
	return newVec;
}

void printLHS()
{
	for (int i = 0; i < (int)lhs.size(); i++)
	{
		printf("LHS %d: Name = %s\n", i, lhs[i]->name);
	}
}

int searchLHS(std::string str)
{
	for (int i = 0; i < (int)lhs.size(); i++)
	{
		if (lhs[i]->name == str)
			return i;
	}
	return -1;
}

struct StatementNode* parse_stmt()
{
	StatementNode *st = (StatementNode*)malloc(sizeof(StatementNode));
	StatementNode *noop, *iter, *gt, *trueBranch, *falseBranch, *current, *end;
	IfStatement *newIf;
	PrintStatement *newPrint;
	GotoStatement *goto_node;						// No declarations inside switch
	AssignmentStatement *newAssign;
	ValueNode *newId, *oper1, *oper2;

	ttype = getToken();
	//printf("ttype before switch is %d\n", ttype);		// CONFIRMED
	//printf("%s", token);
	switch (ttype)
	{
		case(PRINT):
			st->type = PRINT_STMT;
			newPrint = new PrintStatement;
			st->print_stmt = newPrint;

			ttype = getToken();					// get the name of the variable to print		CONFIRMED
			if (ttype == ID)
			{
				st->print_stmt->id = lhs[searchLHS(strdup(token))];
			}
			else if (ttype == NUM)
			{
				newId = new ValueNode;
				st->print_stmt->id = newId;
				st->print_stmt->id->name = strdup("");
				st->print_stmt->id->value = atoi(token);
			}
			ttype = getToken();	// consume semicolon		CONFIRMED
			break;

		case (IF):
			st->type = IF_STMT;
			newIf = new IfStatement;
			noop = new StatementNode;
			trueBranch = new StatementNode;
			newIf->true_branch = trueBranch;
			st->if_stmt = newIf;
			noop->type = NOOP_STMT;
			st->if_stmt->false_branch = noop;					// Sets false branch to noop

			ttype = getToken();	// Grab first token - that is, first operand
			if (ttype == ID)
			{
				st->if_stmt->condition_operand1 = lhs[searchLHS(token)];
			}
			else if (ttype == NUM)
			{
				oper1 = new ValueNode;
				st->if_stmt->condition_operand1 = oper1;
				st->if_stmt->condition_operand1->value = atoi(token);
				st->if_stmt->condition_operand1->name = strdup("");
			}

			ttype = getToken();	// Grab operator - NOTEQUAL, GREATER, LESS
			st->if_stmt->condition_op = ttype;

			ttype = getToken();	// Grab second operand
			if (ttype == ID)
			{
				st->if_stmt->condition_operand2 = lhs[searchLHS(token)];
			}
			else if (ttype == NUM)
			{
				oper2 = new ValueNode;
				st->if_stmt->condition_operand2 = oper2;
				st->if_stmt->condition_operand2->value = atoi(token);
				st->if_stmt->condition_operand2->name = strdup("");
			}
		
			st->if_stmt->true_branch = parse_body();	// Develop the true branch

			iter = st->if_stmt->true_branch;
			while (iter->next != NULL)			// Loops to find end of true branch
				iter = iter->next;
			iter->next = noop;					// Sets next of true branch to noop

			st->if_stmt->false_branch = noop;	// Develop the false branch
			st->next = noop;					// Sets next of statement to noop

			break;

		case(WHILE):
			st->type = IF_STMT;
			newIf = new IfStatement;
			noop = new StatementNode;
			trueBranch = new StatementNode;
			newIf->true_branch = trueBranch;
			st->if_stmt = newIf;
			noop->type = NOOP_STMT;
			st->if_stmt->false_branch = noop;					// Sets false branch to noop

			ttype = getToken();	// Grab first token - that is, first operand
			if (ttype == ID)
			{
				st->if_stmt->condition_operand1 = lhs[searchLHS(token)];
			}
			else if (ttype == NUM)
			{
				oper1 = new ValueNode;
				st->if_stmt->condition_operand1 = oper1;
				st->if_stmt->condition_operand1->value = atoi(token);
				st->if_stmt->condition_operand1->name = strdup("");
			}

			ttype = getToken();	// Grab operator - NOTEQUAL, GREATER, LESS
			st->if_stmt->condition_op = ttype;

			ttype = getToken();	// Grab second operand
			if (ttype == ID)
			{
				st->if_stmt->condition_operand2 = lhs[searchLHS(token)];
			}
			else if (ttype == NUM)
			{
				oper2 = new ValueNode;
				st->if_stmt->condition_operand2 = oper2;
				st->if_stmt->condition_operand2->value = atoi(token);
				st->if_stmt->condition_operand2->name = strdup("");
			}

			st->if_stmt->true_branch = parse_body();	// Develop the true branch

			// Prior to this, copied from If statement - since while loop performs the same up until the end of true branch.
			// Now, to create a Gotostatement & node.
			gt = new StatementNode;
			gt->type = GOTO_STMT;
			goto_node = new GotoStatement;
			gt->goto_stmt = goto_node;
			goto_node->target = st;			// Redirect statement back to st to get to if_stmt to check again

			iter = st->if_stmt->true_branch;	// Set equal to true branch to iterate and append to end of true branch
			while (iter->next != NULL)
				iter = iter->next;
			iter->next = gt;

			end = new StatementNode;
			end->type = NOOP_STMT;
			st->if_stmt->false_branch = noop;	// Get the rest of the stmts
			st->next = noop;
			break;

		case(SWITCH):				// switch_stmt ¨ SWITCH ID LBRACE case_list RBRACE
			st->type = IF_STMT;		// switch_stmt ¨ SWITCH ID LBRACE case_list default_case RBRACE
			noop = new StatementNode;
			noop->type = NOOP_STMT;
			newIf = new IfStatement;
			trueBranch = new StatementNode;
			falseBranch = new StatementNode;
			newIf->false_branch = falseBranch;
			newIf->true_branch = trueBranch;
			st->if_stmt = newIf;
			
			st->if_stmt->condition_op = NOTEQUAL;

			ttype = getToken();	// Grab the variable the switch is using
			newId = new ValueNode;
			newId = lhs[searchLHS(token)];		// Temp ValueNode for comparisons
			//newId->value = lhs[searchLHS(token)]->value;
			//printf("newId name, value = %s, %d\n", newId->name, newId->value);


			st->if_stmt->condition_operand1 = newId;					// Value to compare everything against

			ttype = getToken();	// Grab LBRACE
			ttype = getToken();	// Grab first CASE
			ttype = getToken();	// Grab first value to be compared

			if (ttype == NUM)
			{
				oper2 = new ValueNode;
				st->if_stmt->condition_operand2 = oper2;
				st->if_stmt->condition_operand2->value = atoi(token);
				st->if_stmt->condition_operand2->name = strdup("");
			}
			if (ttype == ID)
				st->if_stmt->condition_operand2 = lhs[searchLHS(token)];	// CASE 'A' - find A, set cond_op2 to it

			ttype = getToken();	// Colon
			
			st->if_stmt->false_branch = parse_body();	// False branch because using notequal, so false is when equal
			
			iter = st->if_stmt->false_branch;
			while (iter->next != NULL)				// Append noop to end
				iter = iter->next;
			iter->next = noop;

			current = st->if_stmt->true_branch;

			ttype = getToken();	// Grab CASE
			//printf("before loop, token should be CASE: %s\n", token);		//CONFIRMED
			while (ttype == CASE)
			{
				current->type = IF_STMT;
				newIf = new IfStatement;
				trueBranch = new StatementNode;
				falseBranch = new StatementNode;
				current->if_stmt = newIf;
				current->if_stmt->true_branch = trueBranch;
				current->if_stmt->false_branch = falseBranch;
				current->if_stmt->condition_op = NOTEQUAL;
				current->if_stmt->condition_operand1 = newId;

				ttype = getToken();	// Grab comparison
				if (ttype == NUM)
				{
					oper2 = new ValueNode;
					current->if_stmt->condition_operand2 = oper2;
					current->if_stmt->condition_operand2->value = atoi(token);
					current->if_stmt->condition_operand2->name = strdup("");
				}
				if (ttype == ID)
					current->if_stmt->condition_operand2 = lhs[searchLHS(token)];	// CASE 'A' - find A, set cond_op2 to it

				ttype = getToken();	// Grab colon
				//printf("inside loop, should be colon: %d\n", ttype);		//CONFIRMED
				current->if_stmt->false_branch = parse_body();

				iter = current->if_stmt->false_branch;
				if (iter == NULL)
					iter = noop;
				else
				{
					while (iter->next != NULL)					// Append noop to end
						iter = iter->next;
					iter->next = noop;
				}

				current = current->if_stmt->true_branch;
				ttype = getToken();	// Grab CASE - when not case, the result will be either DEFAULT or RBRACE
				//printf("end of while ttype is %d\n", ttype);
			}
			if (ttype == DEFAULT)
			{
				current->type = IF_STMT;
				newIf = new IfStatement;
				trueBranch = new StatementNode;
				current->if_stmt = newIf;
				current->if_stmt->true_branch = trueBranch;
				current->if_stmt->condition_op = GREATER;
				oper1 = new ValueNode;
				oper2 = new ValueNode;
				current->if_stmt->condition_operand1 = oper1;
				current->if_stmt->condition_operand2 = oper2;
				current->if_stmt->condition_operand1->value = 1;
				current->if_stmt->condition_operand2->value = 0;

				ttype = getToken();	// Grab colon
				current->if_stmt->true_branch = parse_body();		// Grab body of default

				iter = current->if_stmt->true_branch;
				while (iter->next != NULL)
					iter = iter->next;
				iter->next = noop;

				current->if_stmt->false_branch = noop;
			}
			else
			{
				current = noop;
			}
			st->next = noop;
			//ttype = getToken();	// Grab RBRACE 	CONFIRMED
			//printf("ttype is %d\n", ttype);
			break;

		default:	// Defaults to assign stmt
			st->type = ASSIGN_STMT;
			newAssign = new AssignmentStatement;
			st->assign_stmt = newAssign;	// After working w/ newAssign, set st->assign_stmt equal to it.

			st->assign_stmt->left_hand_side = lhs[searchLHS(token)];
			//printf("%s\n", token);
			ttype = getToken();	// Grab the first char after the LHS					ex: a;

			if (ttype == SEMICOLON)	// There was no operand, initialize to 0.
			{
				oper1 = new ValueNode;
				st->assign_stmt->operand1 = oper1;
				st->assign_stmt->op = 0;
				st->assign_stmt->operand1->value = 0;
				st->assign_stmt->operand1->name = strdup("");
			}
			else																	// ex: a '=' ....
			{
				ttype = getToken();	// Grab the first operand							ex: a = b, a = 3;
				if (ttype == NUM)
				{
					oper1 = new ValueNode;
					st->assign_stmt->operand1 = oper1;
					st->assign_stmt->operand1->value = atoi(token);
					st->assign_stmt->operand1->name = strdup("");
				}
				if (ttype == ID)
				{
					st->assign_stmt->operand1 = lhs[searchLHS(token)];
				}

				ttype = getToken();	// Grab the operator - if semicolon, then done.
				if (ttype == SEMICOLON)		// If semicolon, then only operand 1 is applicable, so set op to 0.
				{
					st->assign_stmt->op = 0;
					//printf("operand name & value = %s, %d\n", st->assign_stmt->operand1->name, st->assign_stmt->operand1->value);
				}
				else 
				{
					st->assign_stmt->op = ttype;
					ttype = getToken();	// Grab the second operand
					if (ttype == NUM)
					{
						oper2 = new ValueNode;
						st->assign_stmt->operand2 = oper2;
						st->assign_stmt->operand2->value = atoi(token);
						st->assign_stmt->operand2->name = NULL;
					}
					if (ttype == ID)
					{
						st->assign_stmt->operand2 = lhs[searchLHS(token)];
					}
					ttype = getToken();	// Grab the semicolon
				}
			}
			break;
	}
	//printf("st's type = %d\n", st->type);
	return st;
}

struct StatementNode* parse_stmt_list()
{
	struct StatementNode* st;   // statement
	struct StatementNode* stl;  // statement list

	st = parse_stmt();
	//printf("parsing statement: %d\n", st->type);
	//printf("ttype : %d\n", ttype);
	ttype = getToken();
	//printf("after grabbing ttype : %d\n", ttype);
	//printf("ttype at parse stl %d\n", ttype);
	if (ttype == ID || ttype == IF || ttype == WHILE || ttype == PRINT || ttype == SWITCH)
	{
		ungetToken();
		stl = parse_stmt_list();

		if (st->type == IF_STMT)	// Append stl to noop		st -> noop -> stl
		{
			//printf("made it through\n");
			struct StatementNode *iter = st;
			while (iter->type != NOOP_STMT)
				iter = iter->next;
			iter->next = stl;
		}
		else			// Append stl to st;		st -> stl
		{
			st->next = stl;
		}
		return st;
	}
	else
	{
		ungetToken();
		return st;
	}
}

struct StatementNode* parse_body()	// Parses the body structure of each body
{
	struct StatementNode* stl;
	ttype = getToken();	// match LBRACE
	stl = parse_stmt_list();
	ttype = getToken(); // match RBRACE
	return stl;
}

struct StatementNode* parse_generate_intermediate_representation()
{
	StatementNode *newNode;

	lhs = buildLHS();
	newNode = parse_body();
	
	return newNode;
}
