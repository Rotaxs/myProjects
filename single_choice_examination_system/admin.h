#ifndef ADMIN_H
#define ADMIN_H

#include <stdbool.h>
#include "common.h"

void modify_admin_password();
bool admin_login();
void add_stu(UserNode *head, int *cur_id);
void del_stu(UserNode *head);
void modify_stu(UserNode *head);
void add_question(QuestionNode *head, int *cur_id);
void del_question(QuestionNode *question_head, PaperNode *paper_head);
void modify_question(QuestionNode *head);
void browse_question(QuestionNode *head);
void search_question(QuestionNode *head);
void show_stu_exercise_info(UserNode *user_head);

PaperNode *init_cur_paper(char *title, int size, int *cur_id);
void add_paper(PaperNode *paper_head, PaperNode *cur_paper);
void choose_question(QuestionNode *q_head, PaperNode *cur_paper);
void modify_title(PaperNode *cur_paper);
void set_scores(QuestionNode *q_head, PaperNode *cur_paper);
void set_time(PaperNode *cur_paper);
void publish_cur_paper(PaperNode *cur_paper);
void browse_cur_paper(QuestionNode *question_head, PaperNode *cur_paper);
void browse_paper(PaperNode *paper_head, QuestionNode *question_head);
void delete_paper(PaperNode *head);
void publish_paper(PaperNode *head);
PaperNode *get_chose_paper(PaperNode *head);
void export_paper(QuestionNode *questioin_head, PaperNode *cur_paper);

#endif
