#ifndef USER_H
#define USER_H

#include "common.h"

UserNode *user_login(UserNode *head);
void modify_password(UserNode *cur_stu);
void exercise(UserNode *user_head, QuestionNode *question_head);
void exam(UserNode *cur_user, PaperNode *paper_head, QuestionNode *question_head);

#endif