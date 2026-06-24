#include "file_io.h"
#include "common.h"
#include "utils.h"
#include <direct.h>

/**
 * @brief 文件 I/O 函数：保存管理员密码
 * @param admin_pwd 要保存的密码
 * @return 保存成功，返回 true，否则返回 false
 */
bool save_admin_password(char *admin_pwd)
{
    FILE *fp = fopen(ADMIN_PWD_PATH, "wb");
    if (fp == NULL)
    {
        _mkdir(DATA_PATH);
        fp = fopen(ADMIN_PWD_PATH, "wb");
        if (fp == NULL)
        {
            ERR("打开管理员数据文件失败");
            usleep(WAITING_TIME);
            return false;
        }
    }
    cipher(admin_pwd);
    fwrite(admin_pwd, MAX_PWD_LEN + 1, 1, fp);
    cipher(admin_pwd);
    fclose(fp);
    return true;
}

/**
 * @brief 文件 I/O 函数：读取管理员密码
 * @param admin_pwd 读取后的密码保存的位置
 * @return 读取成功返回 true，否则返回 false
 */
bool load_admin_password(char *admin_pwd)
{
    FILE *fp = fopen(ADMIN_PWD_PATH, "rb");
    if (fp == NULL)
    {
        char origin_pwd[MAX_PWD_LEN + 1] = ADMIN_ORIGIN_PWD;
        strcpy(admin_pwd, origin_pwd);
        return save_admin_password(origin_pwd);
    }
    if (fread(admin_pwd, MAX_PWD_LEN + 1, 1, fp) != 1)
    {
        fclose(fp);
        return false;
    }
    cipher(admin_pwd);
    fclose(fp);
    return true;
}

/**
 * @brief 保存用户考试的数据
 * @param user_head 用户数据链表
 * @param paper_head 试卷数据链表
 */
bool save_exam_records(UserNode *user_head, PaperNode *paper_head)
{
    FILE *fp = fopen(EXAMRECORDPATH, "w");
    if (fp == NULL)
    {
        _mkdir(DATA_PATH);
        fp = fopen(EXAMRECORDPATH, "w");
        if (fp = NULL)
        {
            ERR("打开考试记录文件失败");
            usleep(WAITING_TIME);
            return false;
        }
    }
    UserNode *cur_stu = user_head->next;
    for (; cur_stu; cur_stu = cur_stu->next)
    {
        ExamRecord *cur_record = cur_stu->exam_record_head->next;
        for (; cur_record; cur_record = cur_record->next)
        {
            // 学生 id, 试卷 id，学生得分，是否交卷，开始考试的时间，结束考试的时间
            fprintf(fp, "%d,%d,%d,%d,%s,%s,", cur_stu->id, cur_record->paper_id,
                    cur_record->score, cur_record->is_finished, cur_record->start_time, cur_record->end_time);
            PaperNode *cur_paper = list_paper_search(paper_head, cur_record->paper_id);
            if (cur_paper != NULL)
            {
                for (int i = 0; i < cur_paper->total_questions; i++)
                {
                    fprintf(fp, "%d", cur_record->choices[i]);
                    if (i != cur_paper->total_questions - 1)
                        fprintf(fp, ";");
                }
            }
            else
            {
                fprintf(fp, "NONE");
            }
            fprintf(fp, "\n");
        }
    }
    fclose(fp);
    return true;
}

/**
 * @brief 读取用户考试数据
 * @param user_head 用户数据链表
 * @param paper_head 试卷数据链表
 */
bool load_exam_records(UserNode *user_head, PaperNode *paper_head)
{
    FILE *fp = fopen(EXAMRECORDPATH, "r");
    if (fp == NULL)
        return save_exam_records(user_head, paper_head);
    UserNode *cur_user = user_head->next;
    char line[1024];
    while (fgets(line, sizeof(line), fp))
    {
        int user_id, paper_id, score;
        int finished;
        char start_time[TIMELEN + 1], end_time[TIMELEN + 1];
        char choices[MAX_PAPER_QUESTIONS * 4];
        // 学生 id，试卷 id，学生得分，是否交卷，开始考试的时间，结束考试的时间，用户的选择
        if (sscanf(line, "%d,%d,%d,%d,%[^,],%[^,],%s", &user_id, &paper_id, &score, &finished, start_time, end_time, choices) == 7)
        {
            // 初始化结点
            ExamRecord *new_record = (ExamRecord *)malloc(sizeof(ExamRecord));
            new_record->paper_id = paper_id;
            new_record->score = score;
            new_record->is_finished = (bool)finished;
            strcpy(new_record->start_time, start_time);
            strcpy(new_record->end_time, end_time);
            new_record->next = NULL;
            PaperNode *cur_paper = list_paper_search(paper_head, paper_id);

            // 处理 choices 数组
            if (strcmp(choices, "NONE") != 0)
            {
                char *p = strtok(choices, ";");
                int i = 0;
                while (p != NULL && i < MAXQ_COUNT)
                {
                    new_record->choices[i++] = atoi(p);
                    p = strtok(NULL, ";");
                }
            }

            // 将做题记录头插至用户做题记录链表
            UserNode *target_user = list_user_search_by_id(user_head, user_id);
            if (target_user)
            {
                new_record->next = target_user->exam_record_head->next;
                target_user->exam_record_head->next = new_record;
            }
            else
            {
                ERR("读取用户做题记录时发生错误");
                usleep(WAITING_TIME);
                free(new_record);
            }
        }
    }

    fclose(fp);
    return true;
}

/**
 * @brief 文件 I/O 函数：保存用户链表中的数据到文件
 * @param user_head 用户链表的头指针
 * @return 如果保存成功，返回 true，否则返回 false
 */
bool save_user_data(UserNode *user_head, PaperNode *paper_head)
{
    FILE *fp = fopen(USERDATAPATH, "wb");
    if (fp == NULL)
    {
        _mkdir(DATA_PATH);
        fp = fopen(USERDATAPATH, "wb");
        if (fp == NULL)
        {
            ERR("打开用户数据文件失败");
            usleep(WAITING_TIME);
            return false;
        }
    }

    UserNode *cur_node = user_head->next;
    int data_size = sizeof(UserNode) - sizeof(UserNode *) - sizeof(ExamRecord *);
    for (; cur_node; cur_node = cur_node->next)
    {
        cipher(cur_node->password);
        int ok = fwrite(cur_node, data_size, 1, fp);
        cipher(cur_node->password);
        if (ok != 1)
        {
            printf(RED BOLD "Error: 写入用户数据文件时错误" RESET);
            fclose(fp);
            return false;
        }
    }
    save_exam_records(user_head, paper_head);

    fclose(fp);
    return true;
}

/**
 * @brief 文件 I/O 函数：读取文件中的用户数据
 * @param user_head 用户链表的头指针
 * @return 读取成功返回 true，否则返回 false
 */
bool load_user_data(UserNode *user_head, PaperNode *paper_head)
{
    // 后面释放内存时不加这行会有段错误
    user_head->exam_record_head = NULL;
    FILE *fp = fopen(USERDATAPATH, "rb");
    if (fp == NULL)
        return save_user_data(user_head, paper_head);
    UserNode user_data;
    int data_size = sizeof(UserNode) - sizeof(UserNode *) - sizeof(ExamRecord *);
    while (fread(&user_data, data_size, 1, fp))
    {
        cipher(user_data.password);
        list_user_add(user_head, user_data.id, user_data.account, user_data.password, user_data.exercised_question_count);
    }
    load_exam_records(user_head, paper_head);
    fclose(fp);
    return true;
}

/**
 * @brief 文件 I/O 函数：保存试题链表中的数据到文件
 * @param head 试题数据链表
 * @return 读取成功返回 true，否则返回 false
 */
bool save_question_data(QuestionNode *head)
{
    FILE *fp = fopen(QDATAPATH, "wb");
    if (fp == NULL)
    {
        _mkdir(DATA_PATH);
        fp = fopen(QDATAPATH, "wb");
        if (fp == NULL)
        {
            ERR("打开试题数据文件时出错");
            usleep(WAITING_TIME);
            return false;
        }
    }
    int data_size = sizeof(QuestionNode) - sizeof(QuestionNode *);
    QuestionNode *cur_node = head->next;

    for (; cur_node; cur_node = cur_node->next)
    {
        int ok = fwrite(cur_node, data_size, 1, fp);
        if (ok != 1)
        {
            printf(RED BOLD "Error: 写入试题数据文件时错误" RESET);
            fclose(fp);
            return false;
        }
    }

    fclose(fp);
    return true;
}

/**
 * @brief 文件 I/O 函数：读取试题文件中的数据
 * @param head 试题数据链表
 * @return 读取成功返回 true，否则返回 false
 */
bool load_question_data(QuestionNode *head)
{
    FILE *fp = fopen(QDATAPATH, "rb");
    if (fp == NULL)
        return save_question_data(head);

    QuestionNode question_data;
    int data_size = sizeof(QuestionNode) - sizeof(QuestionNode *);

    while (fread(&question_data, data_size, 1, fp))
    {
        list_question_add(head, question_data.id, question_data.question,
                          question_data.option, question_data.answer, question_data.category);
    }

    fclose(fp);
    return true;
}

/**
 * @brief 文件 I/O 函数：保存试卷链表中的数据到文件
 * @param head 试卷数据链表
 * @return 读取成功返回 true，否则返回 false
 */
bool save_paper_data(PaperNode *head)
{
    FILE *fp = fopen(PAPERDATAPATH, "wb");
    if (fp == NULL)
    {
        _mkdir(DATA_PATH);
        fp = fopen(PAPERDATAPATH, "wb");
        if (fp == NULL)
        {
            ERR("打开试卷数据文件时出错");
            usleep(WAITING_TIME);
            return false;
        }
    }

    int data_size = sizeof(PaperNode) - sizeof(PaperNode *);
    PaperNode *cur_node = head->next;

    for (; cur_node; cur_node = cur_node->next)
    {
        int ok = fwrite(cur_node, data_size, 1, fp);
        if (ok != 1)
        {
            printf(RED BOLD "Error: 写入试卷数据文件时错误" RESET);
            fclose(fp);
            return false;
        }
    }
    fclose(fp);
    return true;
}

/**
 * @brief 文件 I/O 函数：读取试卷文件中的数据
 * @param head 试卷数据链表
 * @return 读取成功返回 true，否则返回 false
 */
bool load_paper_data(PaperNode *head)
{
    FILE *fp = fopen(PAPERDATAPATH, "rb");
    if (fp == NULL)
        return save_paper_data(head);

    PaperNode paper_data;
    int data_size = sizeof(PaperNode) - sizeof(PaperNode *);

    while (fread(&paper_data, data_size, 1, fp))
    {
        list_paper_add(head, paper_data.id, paper_data.title,
                       paper_data.question_ids, paper_data.question_scores, paper_data.total_questions,
                       paper_data.paper_score, paper_data.start_time, paper_data.end_time, paper_data.published);
    }

    fclose(fp);
    return true;
}

/**
 * @brief 读取最新用户/试卷/试题 id
 */
void load_ids(int *user_id, int *paper_id, int *question_id)
{
    FILE *fp = fopen(IDS_PATH, "rb");
    if (fp == NULL)
    {
        *user_id = 1;
        *paper_id = 1;
        *question_id = 1;
        save_ids(*user_id, *paper_id, *question_id);
        return;
    }

    if (fread(user_id, sizeof(int), 1, fp) != 1)
        *user_id = 1;
    if (fread(paper_id, sizeof(int), 1, fp) != 1)
        *paper_id = 1;
    if (fread(question_id, sizeof(int), 1, fp) != 1)
        *question_id = 1;

    fclose(fp);
}

/**
 * @brief 保存用户/试卷/试题 id 到文件
 * @return 保存成功返回 true，否则返回 false
 */
bool save_ids(int user_id, int paper_id, int question_id)
{
    FILE *fp = fopen(IDS_PATH, "wb");
    if (fp == NULL)
    {
        _mkdir(DATA_PATH);
        fp = fopen(IDS_PATH, "wb");
        if (fp == NULL)
        {
            ERR("打开ID文件时发生错误");
            usleep(WAITING_TIME);
            return false;
        }
    }

    fwrite(&user_id, sizeof(user_id), 1, fp);
    fwrite(&paper_id, sizeof(paper_id), 1, fp);
    fwrite(&question_id, sizeof(question_id), 1, fp);

    fclose(fp);
    return true;
}

/**
 * @brief 将试卷以文本形式导出
 * @param cur_paper 要导出的试卷的试卷结点
 * @return 导出成功，返回 true，否则返回 false
 */
bool save_paper(QuestionNode *question_head, PaperNode *cur_paper, char *file_path)
{
    FILE *fp = fopen(file_path, "w");
    if (fp == NULL)
    {
        _mkdir(EXPORTED_PAPER_PATH);
        fp = fopen(file_path, "w");
        if (fp == NULL)
        {
            printf(RED "打开文件 %s 时发生错误\n" RESET, file_path);
            usleep(WAITING_TIME);
            return false;
        }
    }

    fprintf(fp, "试卷标题：%s\n", cur_paper->title);
    fprintf(fp, "考试时间：%s - %s\n", cur_paper->start_time, cur_paper->end_time);
    fprintf(fp, "试卷总分：%d\n", cur_paper->paper_score);
    fprintf(fp, "\n");

    QuestionNode *cur_question;
    for (int i = 0; i < cur_paper->total_questions; i++)
    {
        cur_question = list_question_search(question_head, cur_paper->question_ids[i]);
        fprintf(fp, "%d. （%d 分）\n", i + 1, cur_paper->question_scores[i]);
        // 输出题目
        fprintf(fp, "%s\n", cur_question->question);
        // 输出选项
        for (int j = 0; j < 4; j++)
        {
            fprintf(fp, "%c. %s\n", 'A' + j, cur_question->option[j]);
        }
        // 输出答案
        fprintf(fp, "答案：%c\n", cur_question->answer);
        fprintf(fp, "\n");
    }

    fclose(fp);
    return true;
}
