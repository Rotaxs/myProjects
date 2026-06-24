#ifndef MENU_H
#define MENU_H

#include "common.h"

void main_menu(UserNode *user_head, PaperNode *paper_head, QuestionNode *question_head,
               int *user_id, int *paper_id, int *question_id);
void admin_menu(UserNode *user_head, PaperNode *paper_head, QuestionNode *question_head,
                int *user_id, int *paper_id, int *question_id);

void admin_manage_stu_menu(UserNode *user_head, PaperNode *paper_head, int *cur_stu_id, int *cur_paper_id, int *cur_question_id);
void admin_manage_paper_menu(PaperNode *paper_head, QuestionNode *question_head, int *cur_stu_id, int *cur_paper_id, int *cur_question_id);
void admin_assemble_paper_menu(QuestionNode *question_head, PaperNode *cur_paper);
void admin_manage_question_menu(QuestionNode *question_head, PaperNode *paper_head, int *cur_stu_id, int *cur_paper_id, int *cur_question_id);

void stu_menu(UserNode *cur_stu, UserNode *user_head, QuestionNode *question_head, PaperNode *paper_head);

#endif