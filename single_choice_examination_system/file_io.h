#ifndef FILE_IO_H
#define FILE_IO_H

// #include <stdbool.h>
#include "common.h"
bool save_admin_password(char *admin_pwd);
bool load_admin_password(char *admin_pwd);
bool save_user_data(UserNode *user_head, PaperNode *paper_head);
bool load_user_data(UserNode *user_head, PaperNode *paper_head);
bool save_question_data(QuestionNode *head);
bool load_question_data(QuestionNode *head);
bool save_paper_data(PaperNode *head);
bool load_paper_data(PaperNode *head);
void load_ids(int *user_id, int *paper_id, int *quetion_id);
bool save_ids(int user_id, int paper_id, int question_id);
bool save_paper(QuestionNode *question_head, PaperNode *cur_paper, char *file_path);

#endif