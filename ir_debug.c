/*
 * Copyright (C) Mohsen Zohrevandi, 2016
 *
 * Do not share this file with anyone
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "ir_debug.h"

static void print_statements(struct StatementNode* start, struct StatementNode * end, int indent);

static void print_value_node(struct ValueNode* v)
{
    if (v != NULL)
    {
        if (strcmp(v->name, "") == 0)   // Assume the ValueNode is representing a constant
        {
            printf("%d", v->value);
        }
        else
        {
            printf("%s", v->name);
        }
        // Alternatively, you could print both:
        // printf("%s (%d)", v->name, v->value);

        // You could also print the address of the node as well:
        // printf(" @ %p", v);
    }
    else
    {
        printf("NULL");
    }
}

static void print_arithmetic_operator(int op)
{
    switch (op)
    {
        case 0:         break;
        case PLUS:      printf(" + ");  break;
        case MINUS:     printf(" - ");  break;
        case MULT:      printf(" * ");  break;
        case DIV:       printf(" / ");  break;
        default:        printf(" ? ");  break;
    }
}

static void print_conditional_operator(int op)
{
    switch (op)
    {
        case GREATER:  printf(" > ");  break;
        case LESS:     printf(" < ");  break;
        case NOTEQUAL: printf(" <> "); break;
        default:       printf(" ? ");  break;
    }
}

static void print_line_prefix(struct StatementNode* st, int indent, bool right_brace)
{
    char buf[100];
    snprintf(buf, sizeof(buf), "%p: ", st);
    if (!right_brace)
    {
        printf("%s", buf);
    }
    else
    {
        printf("%*s", (int) strlen(buf), "");
    }
    printf("%*s", indent * 4, "");
}

static void print_print(struct StatementNode* st, int indent)
{
    assert(st->print_stmt->id != NULL);

    print_line_prefix(st, indent, false);

    printf("print ");
    print_value_node(st->print_stmt->id);
    printf(";\n");
}

static void print_if(struct StatementNode* st, int indent)
{
    assert(st->if_stmt->condition_operand1 != NULL);
    assert(st->if_stmt->condition_operand2 != NULL);
    assert(st->if_stmt->true_branch != NULL);
    assert(st->if_stmt->false_branch != NULL);

    bool inverted = false;

    print_line_prefix(st, indent, false);

    if (st->if_stmt->true_branch->type == NOOP_STMT)
    {
        // This is a heuristic for SWITCH statements
        inverted = true;
    }

    if (inverted)
    {
        printf("if ! ( ");
    }
    else
    {
        printf("if ( ");
    }
    print_value_node(st->if_stmt->condition_operand1);
    print_conditional_operator(st->if_stmt->condition_op);
    print_value_node(st->if_stmt->condition_operand2);
    printf(" ) {\n");

    if (inverted)
    {
        print_statements(st->if_stmt->false_branch, st->if_stmt->true_branch, indent + 1);
    }
    else
    {
        print_statements(st->if_stmt->true_branch, st->if_stmt->false_branch, indent + 1);
    }

    print_line_prefix(st, indent, true);
    printf("}\n");
}

static void print_assignment(struct StatementNode* st, int indent)
{
    assert(st->assign_stmt->left_hand_side != NULL);
    assert(st->assign_stmt->operand1 != NULL);
    if (st->assign_stmt->op == 0)
    {
        assert(st->assign_stmt->operand2 == NULL);
    }
    else
    {
        assert(st->assign_stmt->operand2 != NULL);
    }

    print_line_prefix(st, indent, false);

    print_value_node(st->assign_stmt->left_hand_side);
    printf(" = ");
    print_value_node(st->assign_stmt->operand1);
    print_arithmetic_operator(st->assign_stmt->op);
    if (st->assign_stmt->operand2 != NULL)
    {
        print_value_node(st->assign_stmt->operand2);
    }
    printf(";\n");
}

static void print_goto(struct StatementNode* st, int indent)
{
    assert(st->goto_stmt->target != NULL);

    print_line_prefix(st, indent, false);

    printf("goto %p\n", st->goto_stmt->target);
}

static void print_noop(struct StatementNode* st, int indent)
{
    print_line_prefix(st, indent, false);

    printf("noop;\n");
}

static void print_statements(struct StatementNode* start, struct StatementNode * end, int indent)
{
    struct StatementNode * pc = start;
    while (pc != end)
    {
        switch (pc->type)
        {
            case NOOP_STMT:
                print_noop(pc, indent);
                break;

            case GOTO_STMT:
                assert(pc->goto_stmt != NULL);
                print_goto(pc, indent);
                break;

            case ASSIGN_STMT:
                assert(pc->assign_stmt != NULL);
                print_assignment(pc, indent);
                break;

            case IF_STMT:
                assert(pc->if_stmt != NULL);
                print_if(pc, indent);
                break;

            case PRINT_STMT:
                assert(pc->print_stmt != NULL);
                print_print(pc, indent);
                break;

            default:
                print_line_prefix(pc, indent, false);
                printf("Unknown Statement (type = %d)\n", pc->type);
                break;
        }
        pc = pc->next;
    }
}

void print_program(struct StatementNode* program)
{
    assert(program != NULL);
    print_statements(program, NULL, 1);
}

