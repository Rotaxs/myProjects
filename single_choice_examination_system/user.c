#include "user.h"
#include "common.h"
#include "file_io.h"
#include "utils.h"

/**
 * @brief 菜单功能函数：用户登录
 * @param head 用户数据链表
 * @return 登录成功返回当前用户数据结点，否则返回空指针
 */
UserNode *user_login(UserNode *head)
{
    bool is_valid;
    char input_account[MAX_ACCOUNT_LEN + 1];
    char input_pwd[MAX_PWD_LEN + 1];
    int attempted = 0;
    UserNode *cur_user;

    // 账号检测
    while (1)
    {
        printf(CLS SHOW_CURSOR);
        printf("请输入用户名：");
        get_account_input(input_account, MAX_ACCOUNT_LEN + 1);
        print_enter;
        if (strcmp(input_account, "q") == 0)
        {
            WAITING("正在返回到用户操作界面...");
            return NULL;
        }
        cur_user = list_user_search(head, input_account);
        if (cur_user == NULL)
        {
            ERR("没有找到该用户的信息");
            usleep(WAITING_TIME);
            continue;
        }
        else
            break;
    }

    // 密码检测
    while (1)
    {
        // 保留用户名在屏幕上
        printf(CLS);
        printf("请输入用户名：%s", input_account);
        print_enter;

        printf("请输入用户密码：");
        is_valid = get_password_input(input_pwd, MAX_PWD_LEN);
        print_enter;
        if (!is_valid)
        {
            WARNING("密码过短，请重新输入");
            attempted++;
            if (attempted >= PWD_ATTEMP_COUNT)
            {
                WAITING("尝试次数过多，即将返回到主菜单...");
                return NULL;
            }
            if (attempted >= PWD_ATTEMP_COUNT - 2)
            {
                char buffer[35];
                sprintf(buffer, "你还有 %d 次的尝试机会", PWD_ATTEMP_COUNT - attempted);
                WARNING(buffer);
                usleep(500000);
            }
            usleep(WAITING_TIME);
            continue;
        }

        WAITING("正在进行登录检测...");

        // 密码正确
        if (strcmp(input_pwd, cur_user->password) == 0)
        {
            CURSOR_SHIFT_UP;
            INFO("密码正确                    ");
            WAITING("即将进入学生操作界面...");
            return cur_user;
        }
        else
        {
            CURSOR_SHIFT_UP;
            ERR("密码错误，请重新输入          ");
            attempted++;

            if (attempted >= PWD_ATTEMP_COUNT)
            {
                WAITING("尝试次数过多，即将返回到主菜单...");
                return NULL;
            }
            if (attempted >= PWD_ATTEMP_COUNT - 2)
            {
                char buffer[35];
                sprintf(buffer, "你还有 %d 次的尝试机会", PWD_ATTEMP_COUNT - attempted);
                WARNING(buffer);
                usleep(500000);
            }
            usleep(WAITING_TIME);
        }
    }
    return NULL;
}

/**
 * @brief 菜单功能函数：用户修改密码
 * @param cur_stu 当前编辑的学生结点
 */
void modify_password(UserNode *cur_stu)
{
    char input_previous_password[MAX_PWD_LEN + 1];
    char first_changed_password[MAX_PWD_LEN + 1];
    char second_changed_password[MAX_PWD_LEN + 1];
    bool is_valid;

    while (1)
    {
        printf(CLS);
        printf("请输入原密码：");
        get_password_input(input_previous_password, MAX_PWD_LEN + 1);
        print_enter;
        if (strcmp(input_previous_password, "q") == 0)
        {
            WAITING("正在返回到用户操作界面...");
            return;
        }
        if (strcmp(input_previous_password, cur_stu->password) != 0)
        {
            ERR("密码错误");
            usleep(WAITING_TIME);
            continue;
        }
        while (1)
        {
            printf(CLS);
            printf("请输入新密码：");
            is_valid = get_password_input(first_changed_password, MAX_PWD_LEN + 1);
            print_enter;
            if (!is_valid)
            {
                WARNING("密码过短，请重新输入");
                usleep(WAITING_TIME);
                continue;
            }
            printf("再次输入新密码：");
            get_password_input(second_changed_password, MAX_PWD_LEN + 1);
            print_enter;
            if (strcmp(first_changed_password, second_changed_password) == 0)
            {
                strcpy(cur_stu->password, first_changed_password);
                INFO("修改成功");
                WAITING("正在返回到学生操作界面");
                return;
            }
            else
            {
                ERR("两次密码不一致，请重新输入");
                usleep(WAITING_TIME);
            }
        }
    }
}

/**
 * @brief 菜单功能辅助函数：打印练习的题目
 * @param question_head 试题数据链表
 * @param id 要打印的题目的 id
 * @param selection 突出显示的的选项，范围为 0-3
 * @param choice 选择的选项，范围为 -1-3，-1 表示该题未做
 */
void print_exercise_question(QuestionNode *question_head, int id, int selection, int choice)
{
    QuestionNode *cur_question = list_question_search(question_head, id);

    CURSOR_RESET;

    print_table(TABLE_U, MENU_WIDTH);
    print_enter;
    print_enter;

    // 打印题目
    printf("%s", cur_question->question);
    print_enter;
    // 打印选项
    for (int i = 0; i < 4; i++)
    {
        // choice = -1 说明这题还没有做
        if (choice == -1)
        {
            if (selection == i)
                printf(BG_WHITE BLACK "> %c. %s <\n" RESET, 'A' + i, cur_question->option[i]);
            else
                printf("%c. %s    \n", 'A' + i, cur_question->option[i]);
        }
        // 做了的题要保存做题的状态
        else
        {
            // i == choice 表示之前的选择
            if (i == choice)
            {
                // 如果之前选择正确，标为白底绿字
                if (cur_question->answer - 'A' == i)
                    printf(BG_WHITE GREEN "> %c. %s <\n" RESET, 'A' + i, cur_question->option[i]);
                // 如果之前选择错误，标为白底红字
                else
                    printf(BG_WHITE RED "> %c. %s <\n" RESET, 'A' + i, cur_question->option[i]);
            }
            // 如果不是之前的选择的选项
            else
            {
                // 如果是答案，标为绿字
                if (cur_question->answer - 'A' == i)
                    printf(GREEN "%c. %s    \n" RESET, 'A' + i, cur_question->option[i]);
                else
                    printf("%c. %s    \n", 'A' + i, cur_question->option[i]);
            }
        }
    }

    print_enter;
    print_table(TABLE_B, MENU_WIDTH);
    print_enter;
}

/**
 * @brief 菜单功能函数：学生练习
 * @param cur_stu 当前学生结点
 * @param question_head 试题数据链表
 */
void exercise(UserNode *cur_stu, QuestionNode *question_head)
{
    int num;
    int key;
    int selection, index;
    int question_ids[MAXQ_COUNT];
    int *choosed_questions, *choices;
    int total_questions = list_question_get_ids(question_head, question_ids);
    printf(CLS);
    while (1)
    {
        printf("请输入要做的题数：");
        if (!get_num_input(&num))
        {
            ERR("请输入正确的数字");
            usleep(WAITING_TIME);
            continue;
        }
        if (num > total_questions)
        {
            ERR("题库中还没有这么多题");
            usleep(WAITING_TIME);
            continue;
        }
        break;
    }

    if (num <= 0)
        return;

    // 初始化练习题目和各题的用户的选择
    choosed_questions = (int *)malloc(sizeof(int) * num);
    choices = (int *)malloc(sizeof(int) * num);
    memset(choices, -1, sizeof(int) * num);
    get_n_random_from_arr(question_ids, total_questions, choosed_questions, num);
    selection = 0;
    index = 0;

    printf(HIDE_CURSOR);
    printf(CLS);
    while (1)
    {
        // printf(CLS);
        // print_space((MENU_WIDTH - 19) / 2);
        // printf(CYAN BOLD "- 当前正在做 %d/%d 题 -\n" RESET, index + 1, num);
        // 按照 choosed_questions 数组中的 id 顺序打印题目
        print_exercise_question(question_head, choosed_questions[index], selection, choices[index]);
        print_space((MENU_WIDTH - 19) / 2);
        printf(CYAN BOLD "- 当前正在做 %d/%d 题 -\n" RESET, index + 1, num);
        key = get_keyboard_input();
        if (key == 'q')
            break;
        else if (key == KEY_DOWN)
            selection = (selection + 1) % 4;
        else if (key == KEY_UP)
            selection = (selection - 1 + 4) % 4;
        else if (key == KEY_LEFT)
        {
            // 不能越界
            if (index <= 0)
                continue;
            index--;
            // 做过的题目回归之前选择的选项
            if (choices[index] != -1)
                selection = choices[index];
            // 没做过的题目跳到选项 A
            else
                selection = 0;
            printf(CLS);
        }
        else if (key == KEY_RIGHT)
        {
            if (index >= num - 1)
                continue;
            index++;
            if (choices[index] != -1)
                selection = choices[index];
            else
                selection = 0;
            printf(CLS);
        }
        else if (key == KEY_ENTER)
        {
            // 说明该题已经做过了
            if (choices[index] != -1)
                continue;
            // 标记该题用户的选择
            choices[index] = selection;
            cur_stu->exercised_question_count++;
        }
    }
    free(choices);
    free(choosed_questions);
}

/**
 * @brief 打印考试的题目（打印一题）
 * @param cur_question 当前打印的题目结点
 * @param selection 高亮的选项的索引
 * @param choice 记录中该题用户的选择，-1 表示未选
 * @param index 题目在试卷中的索引
 * @param score 当前试题在试卷中的分值
 */
void print_exam_question(QuestionNode *cur_question, int selection, int choice, int index, int score)
{
    CURSOR_RESET;
    print_table(TABLE_U, MENU_WIDTH);
    print_enter;
    print_enter;

    printf("%d.（%d 分）\n", index + 1, score);
    printf("%s", cur_question->question);
    print_enter;
    // 索引为 index 的题没做过
    if (choice == -1)
    {
        for (int i = 0; i < 4; i++)
        {
            if (selection == i)
                printf(BG_WHITE BLACK "> %c. %s <\n" RESET, 'A' + i, cur_question->option[i]);
            else
                printf("%c. %s    \n", 'A' + i, cur_question->option[i]);
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            if (i == choice)
            // 做过的题目，选择的选项标为黄色
            {
                if (selection == i)
                    printf(BG_WHITE YELLOW "> %c. %s <\n" RESET, 'A' + i, cur_question->option[i]);
                else
                    printf(YELLOW "> %c. %s <\n" RESET, 'A' + i, cur_question->option[i]);
            }
            else
            {
                if (selection == i)
                    printf(BG_WHITE BLACK "%c. %s    \n" RESET, 'A' + i, cur_question->option[i]);
                else
                    printf("%c. %s    \n", 'A' + i, cur_question->option[i]);
            }
        }
    }
    print_enter;
    print_table(TABLE_B, MENU_WIDTH);
    print_enter;
}

/**
 * @brief 打印当前试卷的状态
 * @param status 0/1/2，分别表示 未开始/考试中/已完成
 */
void print_status(int status)
{
    if (status == 0)
        printf(RED "未开始" RESET);
    else if (status == 1)
    {
        printf(YELLOW "考试中" RESET);
    }
    else
        printf(GREEN "已结束" RESET);
}

void draw_chose_paper(PaperNode *paper_head, int *paper_ids, int *status, int len, int selection)
{
    CURSOR_RESET;
    for (int i = 0; i < len; i++)
    {
        PaperNode *cur_paper = list_paper_search(paper_head, paper_ids[i]);
        // 打印序号
        printf("%d. ", i + 1);
        // 如果是选中的试卷
        if (selection == i)
        {
            // 打印 id 状态 标题
            printf(BG_WHITE BLACK "> (ID: %d " RESET, paper_ids[i]);
            printf(BG_WHITE);
            print_status(status[i]);
            printf(BG_WHITE);
            printf(") %s <", cur_paper->title);
            printf(RESET);
        }
        else
        {
            printf("(ID: %d ", paper_ids[i]);
            print_status(status[i]);
            printf(") %s", cur_paper->title);
        }
        print_enter;
    }
}

/**
 * @brief 菜单功能辅助函数：获取试卷的开考状态
 * @param paper_head 试卷数据链表
 * @param status 试卷状态数组
 * @param ids 要检测的试卷 id 数组
 * @param len 数组的长度
 */
void get_status(PaperNode *paper_head, int *status, int *ids, int len)
{
    for (int i = 0; i < len; i++)
    {
        PaperNode *cur_paper = list_paper_search(paper_head, ids[i]);
        char str_cur_date[TIMELEN + 1];
        Date cur_date = get_date();
        date_to_str(cur_date, str_cur_date);
        // 如果当前时间早于考试开始时间
        if (date_compare(str_cur_date, cur_paper->start_time) == 1)
            status[i] = 0;
        else if (date_compare(str_cur_date, cur_paper->end_time) == -1)
            status[i] = 2;
        else
            status[i] = 1;
    }
}

/**
 * @brief 选择考试的试卷并返回
 * @param paper_head 试卷数据链表
 * @return 返回选择的试卷的结点
 */
PaperNode *choose_paper(PaperNode *paper_head)
{
    int len;
    int key;
    int selection = 0;
    int ids[MAX_PAPER_COUNT];
    int status[MAX_PAPER_COUNT];

    len = list_paper_get_published(paper_head, ids, MAX_PAPER_COUNT);
    if (len == 0)
    {
        printf(CLS);
        ERR("现在还没有发布的试卷");
        usleep(WAITING_TIME);
        return NULL;
    }
    bubble_sort(ids, len, false);
    get_status(paper_head, status, ids, len);

    while (1)
    {
        printf(CLS);
        draw_chose_paper(paper_head, ids, status, len, selection);
        key = get_keyboard_input();
        if (key == 'q')
            return NULL;
        else if (key == KEY_UP)
            selection = (selection - 1 + len) % len;
        else if (key == KEY_DOWN)
            selection = (selection + 1) % len;
        else if (key == KEY_ENTER)
        {
            // 如果是进行中的考试和结束的考试，就可以进入试卷界面
            if (status[selection] >= 1)
                return list_paper_search(paper_head, ids[selection]);
            else
            {
                printf(CLS);
                ERR("考试还没有开始");
                usleep(WAITING_TIME);
            }
        }
    }
}

/**
 * @brief 检查 cur_paper 在不在 cur_user 的做题记录中
 * @param cur_user 要检查的用户
 * @param cur_paper 要检查的试卷
 * @return 如果存在，返回相应的记录结点，否则返回 NULL
 */
ExamRecord *check_record(UserNode *cur_user, PaperNode *cur_paper)
{
    ExamRecord *cur_record = cur_user->exam_record_head->next;
    for (; cur_record; cur_record = cur_record->next)
    {
        if (cur_record->paper_id == cur_paper->id)
            return cur_record;
    }
    return NULL;
}

/**
 * @brief 计算得分，并将得分写入记录
 * @param cur_paper 正在写的试卷
 * @param question_head 试题数据链表
 * @param cur_record 当前的记录
 */
void calc_getted_scores(PaperNode *cur_paper, QuestionNode *question_head, ExamRecord *cur_record)
{
    int sum = 0;
    for (int i = 0; i < cur_paper->total_questions; i++)
    {
        // 获取试卷索引为 i 的题目
        QuestionNode *cur_question = list_question_search(question_head, cur_paper->question_ids[i]);
        if (cur_record->choices[i] != -1)
        {
            if (cur_record->choices[i] == cur_question->answer - 'A')
            {
                // 如果记录中当前题目用户的选择和题目的答案相等，那么 sum 加上这道题的分值
                sum += cur_paper->question_scores[i];
            }
        }
    }
    cur_record->score = sum;
}

/**
 * @brief 菜单功能辅助函数：开始考试
 * @param cur_paper 考试的试卷
 * @param question_head 题目数据的链表
 * @param cur_record 要编辑的记录
 * @param index 初始的题目索引
 */
void begin_exam(PaperNode *cur_paper, QuestionNode *question_head, ExamRecord *cur_record, int index)
{
    int key;
    int selection = 0;

    printf(CLS HIDE_CURSOR);
    while (1)
    {
        // printf(CLS);
        QuestionNode *cur_question = list_question_search(question_head, cur_paper->question_ids[index]);
        print_exam_question(cur_question, selection, cur_record->choices[index], index, cur_paper->question_scores[index]);

        key = get_keyboard_input();
        if (key == 'q')
        {
            calc_getted_scores(cur_paper, question_head, cur_record);
            return;
        }
        else if (key == KEY_UP)
            selection = (selection - 1 + 4) % 4;
        else if (key == KEY_DOWN)
            selection = (selection + 1) % 4;
        else if (key == KEY_LEFT)
        {
            if (index <= 0)
                continue;
            index--;
            printf(CLS);
        }
        else if (key == KEY_RIGHT)
        {
            if (index > cur_paper->total_questions - 1)
                continue;
            if (index == cur_paper->total_questions - 1)
            {
                printf(SHOW_CURSOR);
                printf("是否交卷？（y/n）");
                if (get_y_or_n_input())
                {
                    calc_getted_scores(cur_paper, question_head, cur_record);
                    cur_record->is_finished = true;
                    Date date = get_date();
                    char end_time[TIMELEN + 1];
                    date_to_str(date, end_time);
                    strcpy(cur_record->end_time, end_time);
                    return;
                }
                else
                    index--;
            }
            index++;
            printf(CLS HIDE_CURSOR);
        }
        else if (key == KEY_ENTER)
        {
            cur_record->choices[index] = selection;
            Date date = get_date();
            char cur_date[TIMELEN + 1];
            date_to_str(date, cur_date);
            if (date_compare(cur_date, cur_paper->end_time) <= 0)
            {
                WARNING("考试结束时间已到，请停止作答");
                WAITING("正在退出考试界面...");
                return;
            }
        }
    }
}

/**
 * @brief 浏览完成或结束的试卷
 * @param cur_paper 当前考试的试卷
 * @param cur_record 当前学生的考试记录
 * @param question_head 试题数据链表
 */
void browse_exam_paper(PaperNode *cur_paper, ExamRecord *cur_record, QuestionNode *question_head)
{
    QuestionNode *cur_question;
    int key;

    printf(HIDE_CURSOR);
    printf(CYAN BOLD "%s\n" RESET, cur_paper->title);
    printf(YELLOW BOLD "答卷时间：%s\n" RESET, cur_record->start_time);
    printf(YELLOW BOLD "交卷时间：" RESET);
    // if (date_compare(cur_record->start_time, cur_record->end_time) == 0)
    // printf(RED "未交卷\n" RESET);
    if (!cur_paper->published)
        printf(RED "未交卷\n" RESET);
    else
        printf(YELLOW BOLD "%s\n" RESET, cur_record->end_time);
    printf(GREEN BOLD "试卷得分：%d/%d\n" RESET, cur_record->score, cur_paper->paper_score);
    print_enter;

    for (int i = 0; i < cur_paper->total_questions; i++)
    {
        cur_question = list_question_search(question_head, cur_paper->question_ids[i]);
        printf("%d.（", i + 1);
        if (cur_record->choices[i] + 'A' == cur_question->answer)
            printf(GREEN "%d" RESET, cur_paper->question_scores[i]);
        else
            printf(RED "%d" RESET, 0);
        printf(" 分 / ");
        printf(GREEN "%d" RESET, cur_paper->question_scores[i]);
        printf(" 分）\n");

        printf("%s", cur_question->question);
        print_enter;

        for (int j = 0; j < 4; j++)
            printf("%c. %s\n", 'A' + j, cur_question->option[j]);

        printf("本题答案：");
        printf(GREEN "%c\n" RESET, cur_question->answer);
        printf("你的答案：");
        if (cur_record->choices[i] == -1)
            printf(RED "未作答\n" RESET);
        else if (cur_record->choices[i] + 'A' == cur_question->answer)
            printf(GREEN "%c\n" RESET, cur_record->choices[i] + 'A');
        else
            printf(RED "%c\n" RESET, cur_record->choices[i] + 'A');

        print_enter;
    }
    while (1)
    {
        key = get_keyboard_input();
        if (key == 'q')
            break;
    }
}

/**
 * @brief 菜单功能函数：考试
 * @param cur_user 考试的学生
 * @param paper_head 试卷数据链表
 * @param question_head 试题数据链表
 */
void exam(UserNode *cur_user, PaperNode *paper_head, QuestionNode *question_head)
{
    int key, index;
    char start_time[TIMELEN + 1];
    bool jump = false;
    Date date;
    char cur_date[TIMELEN + 1];

    while (1)
    {
        // 选择试卷
        printf(HIDE_CURSOR);
        PaperNode *cur_paper = choose_paper(paper_head);
        if (cur_paper == NULL)
            return;

        ExamRecord *cur_record = check_record(cur_user, cur_paper);
        // 如果没有做过，就添加记录，并打印考生须知
        if (!cur_record)
        {
            int *choices = (int *)malloc(sizeof(int) * cur_paper->total_questions);
            memset(choices, -1, sizeof(int) * cur_paper->total_questions);

            // 考试结束的试卷不用打印考生须知
            date = get_date();
            date_to_str(date, cur_date);
            if (date_compare(cur_date, cur_paper->end_time) == 1)
            {
                printf(CLS HIDE_CURSOR);
                printf(BOLD CYAN "考生须知：\n" RESET);
                printf(YELLOW BOLD "考试时间：%s - %s\n" RESET, cur_paper->start_time, cur_paper->end_time);
                printf(YELLOW BOLD "试卷总分：%d\n" RESET, cur_paper->paper_score);
                printf(YELLOW BOLD "试卷题量：%d 题\n" RESET, cur_paper->total_questions);
                print_enter;
                INFO("按下空格键开始考试");
                while (1)
                {
                    key = get_keyboard_input();
                    if (key == KEY_SPACE)
                        break;
                    // 如果按下 q，要回到选择试卷的界面
                    else if (key == 'q')
                    {
                        jump = true;
                        break;
                    }
                }
                if (jump)
                {
                    jump = false;
                    continue;
                }
            }
            date = get_date();
            date_to_str(date, start_time);
            cur_record = list_examRecord_add(cur_user->exam_record_head, cur_paper->id, 0, false, start_time, start_time, choices);
        }

        // 如果记录显示当前试卷已经完成，或者考试已经结束，就浏览试卷
        date = get_date();
        date_to_str(date, cur_date);
        if (cur_record->is_finished || date_compare(cur_date, cur_paper->end_time) == -1)
        {
            printf(CLS);
            browse_exam_paper(cur_paper, cur_record, question_head);
        }
        // 如果记录显示当前试卷没有完成，就考试
        else
        {
            // 查找第一道没做的题目
            for (index = 0; index < cur_paper->total_questions; index++)
            {
                if (cur_record->choices[index] == -1)
                    break;
            }
            // 考试
            begin_exam(cur_paper, question_head, cur_record, index);
        }
    }
}
