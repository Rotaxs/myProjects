#include "admin.h"
#include "common.h"
#include "utils.h"
#include "file_io.h"

/**
 * @brief 菜单功能函数：修改管理员密码
 */
void modify_admin_password()
{
    char input_previous_password[MAX_PWD_LEN + 1];
    char first_changed_password[MAX_PWD_LEN + 1];
    char second_changed_password[MAX_PWD_LEN + 1];
    char admin_pwd[MAX_PWD_LEN + 1];
    load_admin_password(admin_pwd);
    bool is_valid;

    while (1)
    {
        printf(CLS);
        printf("请输入原密码：");
        get_password_input(input_previous_password, MAX_PWD_LEN + 1);
        print_enter;
        if (strcmp(input_previous_password, "q") == 0)
        {
            WAITING("正在返回到管理员操作界面...");
            return;
        }
        if (strcmp(input_previous_password, admin_pwd) != 0)
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
                strcpy(admin_pwd, first_changed_password);
                save_admin_password(admin_pwd);
                INFO("修改成功");
                WAITING("正在返回到管理员操作界面");
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
 * @brief 菜单功能函数：管理员登录
 * @return 登录成功，返回 true，否则返回 false
 */
bool admin_login()
{
    bool is_valid;
    char input_pwd[MAX_PWD_LEN + 1];
    int attempted = 0;

    while (1)
    {
        printf(CLS);
        printf("请输入管理员密码：");

        // 判断密码是否过短
        is_valid = get_password_input(input_pwd, MAX_PWD_LEN + 1);
        print_enter;
        if (!is_valid)
        {
            ERR("密码过短，请重新输入");
            attempted++;
            if (attempted >= PWD_ATTEMP_COUNT)
            {
                WAITING("尝试次数过多，即将返回到主菜单...");
                return false;
            }
            if (attempted >= PWD_ATTEMP_COUNT - 2)
            {
                char buffer[35];
                sprintf(buffer, "你还有 %d 次的尝试机会", PWD_ATTEMP_COUNT - attempted);
                WARNING(buffer);
                usleep(500000);
            }
            sleep(1);
            continue;
        }

        WAITING("正在进行登录检测...");

        // 密码正确
        char admin_pwd[MAX_PWD_LEN + 1];
        load_admin_password(admin_pwd);
        if (strcmp(input_pwd, admin_pwd) == 0)
        {
            CURSOR_SHIFT_UP;
            INFO("密码正确                    ");
            WAITING("即将跳转至管理员操作界面...");
            return true;
        }
        else
        {
            CURSOR_SHIFT_UP;
            ERR("密码错误，请重新输入          ");
            attempted++;
            if (attempted >= PWD_ATTEMP_COUNT)
            {
                WAITING("尝试次数过多，即将返回到主菜单...");
                return false;
            }
            if (attempted >= PWD_ATTEMP_COUNT - 2)
            {
                char buffer[35];
                sprintf(buffer, "你还有 %d 次的尝试机会", PWD_ATTEMP_COUNT - attempted);
                WARNING(buffer);
                usleep(500000);
            }
            sleep(1);
        }
    }
    return false;
}

// ================================= 学生信息管理 =================================

/**
 * @brief 菜单功能函数：添加学生
 * @param head 学生数据链表
 * @param cur_id 最新 id 的指针
 */
void add_stu(UserNode *head, int *cur_id)
{
    bool is_valid;
    char account[MAX_ACCOUNT_LEN + 1];
    char password[MAX_PWD_LEN + 1];
    printf(CLS);
    while (1)
    {
        printf("请输入学生用户名：");
        get_account_input(account, MAX_ACCOUNT_LEN + 1);
        print_enter;
        if (strcmp(account, "q") == 0)
            break;
        if (list_user_is_reduplicate(head, account))
        {
            ERR("该用户名已存在");
            usleep(WAITING_TIME);
            continue;
        }
        printf("请输入密码：");
        is_valid = get_password_input(password, MAX_PWD_LEN + 1);
        print_enter;
        if (!is_valid)
        {
            ERR("密码过短，请重新输入");
            usleep(WAITING_TIME);
            continue;
        }
        bool ok = list_user_add(head, *cur_id, account, password, 0);
        if (!ok)
            ERR("添加学生失败");
        else
        {
            char buffer[45];
            sprintf(buffer, "成功添加学生信息，学生 ID 为 %d", *cur_id);
            INFO(buffer);
            (*cur_id)++;
        }
        printf("要继续添加学生信息吗？（y/n）");
        if (!get_y_or_n_input())
            break;
    }
    WAITING("正在返回到学生信息管理界面...");
}

/**
 * @brief 菜单功能函数：删除学生
 * @param head 用户数据链表
 */
void del_stu(UserNode *head)
{
    UserNode *user_node = NULL;
    bool ok;
    char input_account[MAX_ACCOUNT_LEN + 1];
    printf(CLS);
    while (1)
    {
        printf("请输入学生的用户名：");
        get_account_input(input_account, MAX_ACCOUNT_LEN + 1);
        print_enter;
        if (strcmp(input_account, "q") == 0)
            break;
        user_node = list_user_search(head, input_account);
        if (user_node == NULL)
        {
            ERR("没有找到该学生");
            usleep(WAITING_TIME);
            continue;
        }
        ok = list_user_delete(head, user_node->id);
        if (!ok)
            ERR("删除失败");
        else
            INFO("删除成功");
        printf("要继续删除学生信息吗？（y/n）");
        if (!get_y_or_n_input())
            break;
    }
    WAITING("正在返回到学生信息管理界面...");
}

/**
 * @brief 菜单功能函数：修改学生信息
 * @param head 用户数据链表
 */
void modify_stu(UserNode *head)
{
    UserNode *user_node = NULL;
    char input_account[MAX_ACCOUNT_LEN + 1];
    char changed_account[MAX_ACCOUNT_LEN + 1];
    char changed_password[MAX_PWD_LEN + 1];
    bool ok, is_valid;

    printf(CLS);
    while (1)
    {
        printf("请输入学生用户名：");
        get_account_input(input_account, MAX_ACCOUNT_LEN + 1);
        print_enter;
        if (strcmp(input_account, "q") == 0)
            break;
        user_node = list_user_search(head, input_account);
        if (user_node == NULL)
        {
            ERR("没有找到该学生");
            usleep(WAITING_TIME);
            continue;
        }

        while (1)
        {
            printf("要将用户名修改为：");
            get_account_input(changed_account, MAX_ACCOUNT_LEN + 1);
            print_enter;
            if (strcmp(input_account, changed_account) != 0 && list_user_is_reduplicate(head, changed_account))
            {
                ERR("该用户名已存在");
                usleep(WAITING_TIME);
                continue;
            }
            printf("要将密码修改为：");
            is_valid = get_password_input(changed_password, MAX_PWD_LEN + 1);
            print_enter;
            if (!is_valid)
            {
                ERR("密码过短，请重新输入");
                usleep(WAITING_TIME);
                continue;
            }
            break;
        }

        ok = list_user_modify(head, user_node->id, changed_account, changed_password);
        if (!ok)
            ERR("修改失败");
        else
            INFO("修改成功");
        printf("要继续修改学生信息吗？（y/n）");
        if (!get_y_or_n_input())
            break;
    }
    WAITING("正在返回到学生信息管理界面...");
}

void show_stu_exercise_info(UserNode *user_head)
{
    UserNode *cur_user = user_head->next;
    int ids[MAX_USER_COUNT];
    printf("%-8s          %-24s%-8s\n", "用户ID", "用户名", "练习题量");
    print_table(TABLE_U, MENU_WIDTH);
    print_enter;
    print_enter;

    int len = list_user_get_ids(user_head, ids);
    bubble_sort(ids, len, false);
    for (int i = 0; i < len; i++)
    {
        cur_user = list_user_search_by_id(user_head, ids[i]);
        printf("%-8d        %-24s%-8d\n", cur_user->id, cur_user->account, cur_user->exercised_question_count);
    }

    print_enter;
    print_table(TABLE_B, MENU_WIDTH);
    while (1)
    {
        int key = get_keyboard_input();
        if (key == 'q')
            break;
    }
}

// ================================== 试题管理 ==================================

/**
 * @brief 菜单功能函数：添加试题
 * @param head 试题数据链表
 * @param cur_id 最新试题 id 的指针
 */
void add_question(QuestionNode *head, int *cur_id)
{
    bool ok, stop;
    char question[QLEN + 1];
    char options[4][OPTIONLEN + 1];
    char answer;
    char category[CATEGORYLEN + 1];
    printf(SHOW_CURSOR);
    while (1)
    {
        while (1)
        {
            printf(CLS);
            printf("请输入试题题干（另起一行用 #\\n 作为结束）：\n");
            if (!get_question_input(question, QLEN + 1))
            {
                ERR("选项长度过长，请重新输入");
                usleep(WAITING_TIME);
                continue;
            }
            if (strlen(question) == 0)
            {
                stop = true;
                break;
            }
            break;
        }
        if (stop)
            break;
        for (int i = 0; i < 4; i++)
        {
            printf("请输入选项 %c: ", 'A' + i);
            while (!get_option_input(options[i], OPTIONLEN + 1))
            {
                ERR("选项长度过长，请重新输入");
                printf("请输入选项 %c: ", 'A' + i);
            }
        }
        printf("这道题的答案是：");
        while (!get_answer_input(&answer))
        {
            ERR("答案格式输入错误，请重新输入");
            printf("这道题的答案是：");
        }
        printf("请输入这道题的分类：");
        while (!get_category_input(category, CATEGORYLEN + 1))
        {
            ERR("分类长度过长，请重新输入");
            printf("请输入这道题的分类：");
        }

        list_question_add(head, *cur_id, question, options, answer, category);
        INFO("添加成功");
        char buffer[30];
        sprintf(buffer, "这道题目的 ID 为 %d", *cur_id);
        INFO(buffer);
        (*cur_id)++;
        printf("要继续添加试题吗？（y/n）");
        if (!get_y_or_n_input())
            break;
    }

    WAITING("正在返回到试题管理界面...");
}

/**
 * @brief 菜单功能函数：删除试题
 */
void del_question(QuestionNode *question_head, PaperNode *paper_head)
{
    int id;
    printf(CLS);
    while (1)
    {
        printf("请输入要删除的试题的 ID（输入 -1 退出）：");
        while (!get_id_input(&id))
        {
            ERR("请输入正确的 ID");
            printf("请输入要删除的试题的 ID：");
        }
        if (id == -1)
            break;
        if (id_in_paper(id, paper_head))
        {
            ERR("不能删除这道题，因为现存的试卷中收录了该题");
            usleep(WAITING_TIME);
            return;
        }
        if (list_question_delete(question_head, id))
            INFO("删除成功");
        else
            ERR("该试题不存在");
        printf("要继续删除试题吗？（y/n）");
        if (!get_y_or_n_input())
            break;
    }
    WAITING("正在返回到试题管理界面...");
}

/**
 * @brief 菜单功能辅助函数：打印试题
 * @param node 要打印的试题的结点
 */
void print_question(QuestionNode *node)
{
    printf("ID：%d\n", node->id);
    printf("%s\n", node->question);
    for (int i = 0; i < 4; i++)
    {
        printf("%c. ", 'A' + i);
        printf("%s", node->option[i]);
        print_enter;
    }
    print_enter;
    printf("答案：%c\n", node->answer);
    printf("类别：%s\n", node->category);
}

/**
 * @brief 菜单功能辅助函数：打印试题
 * @param head 试题数据链表
 * @param id 要打印的试题的 id
 */
void print_question_by_id(QuestionNode *head, int id)
{
    QuestionNode *cur_node = list_question_search(head, id);
    print_question(cur_node);
}

/**
 * @brief 菜单功能函数：修改试题
 */
void modify_question(QuestionNode *head)
{
    int id;
    bool stop = false;
    while (1)
    {
        printf(CLS);
        QuestionNode *node;
        while (1)
        {
            printf(CLS);
            printf("请输入要修改的试题的 ID（输入 -1 退出）：");
            while (!get_id_input(&id))
            {
                ERR("请输入正确的 ID");
                usleep(WAITING_TIME);
                printf("请输入要修改的试题的 ID（输入 -1 退出）：");
            }
            if (id == -1)
            {
                stop = true;
                break;
            }
            node = list_question_search(head, id);
            if (node != NULL)
                break;
            char buffer[45];
            sprintf(buffer, "ID 为 %d 的试题不存在或已被删除", id);
            ERR(buffer);
            usleep(WAITING_TIME);
        }
        if (stop)
            break;
        char changed_question[QLEN + 1];
        char changed_option[4][OPTIONLEN + 1];
        char changed_answer;
        char changed_category[CATEGORYLEN + 1];

        printf("搜索到该题如下\n");
        print_table(TABLE_U, MENU_WIDTH);
        print_enter;
        print_question_by_id(head, id);
        print_table(TABLE_B, MENU_WIDTH);
        print_enter;

        printf("要修改这道题的题干吗？（y/n）");
        if (get_y_or_n_input())
        {
            while (1)
            {
                printf("要将这题的题干修改为：\n");
                if (!get_question_input(changed_question, QLEN + 1))
                {
                    ERR("选项长度过长，请重新输入");
                    usleep(WAITING_TIME);
                    continue;
                }
                break;
            }
        }
        else
            strcpy(changed_question, node->question);

        printf("要修改这道题的选项吗？（y/n）");
        if (get_y_or_n_input())
        {
            for (int i = 0; i < 4; i++)
            {
                printf("请输入选项 %c: ", 'A' + i);
                while (!get_option_input(changed_option[i], OPTIONLEN + 1))
                {
                    ERR("选项长度过长，请重新输入");
                    printf("请输入选项 %c: ", 'A' + i);
                }
            }
        }
        else
        {
            for (int i = 0; i < 4; i++)
                strcpy(changed_option[i], node->option[i]);
        }

        printf("要修改这道题的答案吗？（y/n）");
        if (get_y_or_n_input())
        {
            printf("请输入答案：");
            while (!get_answer_input(&changed_answer))
            {
                ERR("答案格式输入错误，请重新输入");
                printf("请输入答案：");
            }
        }
        else
            changed_answer = node->answer;

        printf("要修改这道题的类别吗？（y/n）");
        if (get_y_or_n_input())
        {
            printf("请输入这道题的分类：");
            while (!get_category_input(changed_category, CATEGORYLEN + 1))
            {
                ERR("分类长度过长，请重新输入");
                printf("请输入这道题的分类：");
            }
        }
        else
            strcpy(changed_category, node->category);

        list_question_modify(head, id, changed_question, changed_option, changed_answer, changed_category);

        INFO("修改成功");

        printf("要继续修改题目吗？（y/n）");
        if (!get_y_or_n_input())
            break;
    }
    WAITING("正在返回到试题管理界面...");
}

/**
 * @brief 菜单功能辅助函数：显示 id 在 ids 中的试题
 * @param head 试题数据链表
 * @param ids 要打印的试题的 id
 * @param size ids 的大小
 */
void display_questions(QuestionNode *head, int *ids, int size)
{
    if (size <= 0)
        return;

    int i = 0;
    int key;

    while (i < size)
    {
        printf(CLS);
        print_table(TABLE_U, MENU_WIDTH);

        // 计算当前页结束的位置
        int page_end = (i + ONCE_COUNT > size) ? size : i + ONCE_COUNT;

        // 显示当前页的题目
        for (int j = i; j < page_end; j++)
        {
            print_enter;
            QuestionNode *cur_question = list_question_search(head, ids[j]);
            print_question(cur_question);
            print_enter;
            print_table(TABLE_B, MENU_WIDTH);
        }

        printf(YELLOW "\n[第 %d-%d 题 / 共 %d 题]  方向键←/→：翻页  q：返回\n" RESET, i + 1, page_end, size);

        while (1)
        {
            key = get_keyboard_input();
            if (key == 'q')
                return;
            if (key == KEY_RIGHT)
            {
                if (page_end < size)
                {
                    i += ONCE_COUNT;
                    break;
                }
            }
            else if (key == KEY_LEFT)
            {
                if (i >= ONCE_COUNT)
                {
                    i -= ONCE_COUNT;
                    break;
                }
            }
        }
    }
}

/**
 * @brief 菜单功能函数：浏览试题
 */
void browse_question(QuestionNode *head)
{
    printf(CLS);
    QuestionNode *node = head->next;
    int ids[MAXQ_COUNT];
    // int size = 0;
    // for (; node; node = node->next)
    //     ids[size++] = node->id;
    int len = list_question_get_ids(head, ids);
    if (len == 0)
    {
        printf(CLS);
        ERR("题库中还没有试题");
        usleep(WAITING_TIME);
        return;
    }
    bubble_sort(ids, len, false);
    display_questions(head, ids, len);
}

/**
 * @brief 菜单功能函数：搜索试题
 */
void search_question(QuestionNode *head)
{
    int ids[MAXQ_COUNT], size;
    char key;
    QuestionNode *node;
    char category[CATEGORYLEN + 1];
    while (1)
    {
        printf(CLS);
        printf("请输入要搜索的试题的类别：");
        while (!get_category_input(category, CATEGORYLEN + 1))
        {
            ERR("分类长度过长，请重新输入");
            printf("请输入要搜索的试题的类别：");
        }
        if (strcmp(category, "q") == 0)
        {
            break;
        }
        size = list_question_search_by_category(head, category, ids);
        bubble_sort(ids, size, false);
        if (size == 0)
        {
            ERR("没有该类别的题目");
            usleep(WAITING_TIME);
            continue;
        }
        // printf("size = %d\n", size);
        printf(HIDE_CURSOR);
        display_questions(head, ids, size);
        printf(SHOW_CURSOR);
        printf("要继续搜索题目吗？（y/n）");
        if (!get_y_or_n_input())
            break;
    }
}

// ================================== 试卷管理 ==================================

/**
 * @brief 菜单功能辅助函数：打印试题（带高亮）
 * @param q_head 试题数据链表
 * @param start_index 要打印的试题在 ids 数组中的起始索引
 * @param end_index 要打印的试题在 ids 数组中的末索引 + 1
 * @param ids 要打印的试题的 id
 * @param is_chosen 和 ids 相对应，is_chosen[i] 表示 ids[i] 是否被选中
 * @param len ids 数组的长度
 * @param selection 范围在 0~ONCE_COUNT 表示当前选中的试题
 */
void display_question_noticed(QuestionNode *q_head, int start_index, int end_index,
                              int *ids, bool *is_chosen, int len, int selection)
{
    QuestionNode *node;

    CURSOR_RESET;

    print_table(TABLE_U, MENU_WIDTH);
    print_enter;

    for (int i = start_index; i < end_index; i++)
    {
        node = list_question_search(q_head, ids[i]);

        print_enter;

        if ((i - start_index) == selection)
            printf(BG_WHITE BLACK "> ID: %d <" RESET, node->id);
        else
            printf("ID: %d    ", node->id);
        printf("\n状态：");
        if (is_chosen[i])
            printf(GREEN "选中 \n" RESET);
        else
            printf(RED "未选中\n" RESET);
        printf("%s\n", node->question);
        for (int j = 0; j < 4; j++)
        {
            printf("%c. ", 'A' + j);
            printf("%s", node->option[j]);
            print_enter;
        }
        print_enter;
        printf("答案：%c\n", node->answer);
        printf("类别：%s\n", node->category);

        print_enter;
        print_table(TABLE_B, MENU_WIDTH);
        print_enter;
    }
}

/**
 * @brief 菜单功能辅助函数：计算试卷总分
 * @param cur_paper 要计算总分的试卷
 */
void calc_paper_score(PaperNode *cur_paper)
{
    cur_paper->paper_score = 0;
    for (int i = 0; i < cur_paper->total_questions; i++)
    {
        cur_paper->paper_score += cur_paper->question_scores[i];
    }
}

/**
 * @brief 菜单功能函数：选择试题
 * @param q_head 试题数据链表
 * @param cur_paper 当前编辑的试卷结点
 */
void choose_question(QuestionNode *q_head, PaperNode *cur_paper)
{
    int selection = 0;
    int start_index = 0, end_index = ONCE_COUNT, cur_page_size;
    int len = 0;
    int key;
    bool stop = false;
    int ids[MAXQ_COUNT];
    bool is_chosen[MAXQ_COUNT] = {0};
    len = list_question_get_ids(q_head, ids);
    bubble_sort(ids, len, false);
    if (len == 0)
    {
        printf(CLS);
        ERR("现在题库中还没有题目");
        usleep(WAITING_TIME);
        return;
    }
    for (int i = 0; i < cur_paper->total_questions; i++)
    {
        for (int j = 0; j < len; j++)
        {
            if (ids[j] == cur_paper->question_ids[i])
                is_chosen[j] = true;
        }
    }

    printf(CLS);
    while (!stop)
    {
        end_index = (start_index + ONCE_COUNT > len) ? len : start_index + ONCE_COUNT;
        cur_page_size = end_index - start_index;

        // printf(CLS);
        display_question_noticed(q_head, start_index, end_index, ids, is_chosen, len, selection);
        printf(YELLOW "[第 %d-%d 题 / 共 %d 题]\n方向键←/→:翻页 ↑/↓:切换题目 Enter:选题/取消选题 q:返回\n" RESET, start_index + 1, end_index, len);

        key = get_keyboard_input();
        if (key == KEY_RIGHT)
        {
            if (start_index + ONCE_COUNT >= len)
                continue;
            start_index = end_index;
            selection = 0;
            printf(CLS);
        }
        else if (key == KEY_LEFT)
        {
            if (start_index < ONCE_COUNT)
                continue;
            start_index -= ONCE_COUNT;
            selection = 0;
            printf(CLS);
        }
        else if (key == KEY_UP)
        {
            selection = (selection - 1 + cur_page_size) % cur_page_size;
        }
        else if (key == KEY_DOWN)
        {
            selection = (selection + 1) % cur_page_size;
        }
        else if (key == KEY_ENTER)
        {
            int index = start_index + selection;
            if (index < len)
            {
                if (is_chosen[index])
                {
                    delete_item_from_array(cur_paper->question_ids, &(cur_paper->total_questions), ids[index]);
                    is_chosen[index] = false;
                }
                else
                {
                    cur_paper->question_ids[cur_paper->total_questions] = ids[index];
                    cur_paper->question_scores[cur_paper->total_questions] = 0;
                    cur_paper->total_questions++;
                    is_chosen[index] = true;
                }
            }
        }
        else if (key == 'q')
        {
            stop = true;
            break;
        }
    }
    calc_paper_score(cur_paper);
}

/**
 * @brief 菜单功能函数：修改试卷标题
 * @param cur_paper 当前编辑的试卷结点
 */
void modify_title(PaperNode *cur_paper)
{
    printf(CLS);
    printf("要将试卷的标题修改为：");
    while (!get_title_input(cur_paper->title, TITLELEN + 1))
    {
        ERR("标题长度过长，请重新输入");
        printf("要将试卷的标题修改为：");
    }
    INFO("修改成功");
    usleep(WAITING_TIME);
}

/**
 * @brief 菜单功能函数：设置试卷中每道题的分值
 * @param q_head 试题数据链表
 * @param cur_paper 当前编辑的试卷结点
 */
void set_scores(QuestionNode *q_head, PaperNode *cur_paper)
{
    if (cur_paper->total_questions == 0)
    {
        WARNING("当前试卷没有题目，请先添加试题");
        usleep(WAITING_TIME);
        return;
    }

    QuestionNode *cur_question;
    int total_sum = 0;
    int score;

    for (int i = 0; i < cur_paper->total_questions; i++)
    {
        while (1)
        {
            printf(CLS);
            printf(CYAN "--- 正在为第 %d/%d 题设置分值 ---\n" RESET, i + 1, cur_paper->total_questions);

            cur_question = list_question_search(q_head, cur_paper->question_ids[i]);
            if (!cur_question)
            {
                char buffer[45];
                sprintf(buffer, "ID 为 %d 的题目不存在或已删除", cur_paper->question_ids[i]);
                ERR(buffer);
                break;
            }

            print_table(TABLE_U, MENU_WIDTH);
            print_enter;
            print_question(cur_question);
            print_table(TABLE_B, MENU_WIDTH);
            print_enter;

            printf("\n请输入该题分值 (输入 -1 放弃组卷并退出): ");
            if (!get_score_input(&score))
            {
                ERR("请输入正确的分值！");
                usleep(WAITING_TIME);
                continue;
            }

            if (score == -1)
                return;

            cur_paper->question_scores[i] = score;
            total_sum += score;
            break;
        }
    }

    cur_paper->paper_score = total_sum;

    char buffer[50];
    sprintf(buffer, "所有题目分值设置完毕，总分为: %d", total_sum);
    INFO(buffer);
    WAITING("即将返回到组卷系统...");
}

/**
 * @brief 菜单功能函数：设置试卷的考试时间范围
 * @param cur_paper 正在编辑的试卷结点
 */
void set_time(PaperNode *cur_paper)
{
    char start_time[TIMELEN + 1], end_time[TIMELEN + 1];

    printf(CLS);
    printf("请输入开始考试的时间（格式为：YYYY/MM/DD HH:MM）");
    while (!get_date_input(start_time))
    {
        ERR("输入的日期格式错误");
        printf("请输入开始考试的时间（格式为：YYYY/MM/DD HH:MM）");
    }

    while (1)
    {
        printf("请输入结束考试的时间（格式为：YYYY/MM/DD HH:MM）");
        while (!get_date_input(end_time))
        {
            ERR("输入的日期格式错误");
            printf("请输入结束考试的时间（格式为：YYYY/MM/DD HH:MM）");
        }
        if (date_compare(start_time, end_time) <= 0)
        {
            ERR("起始时间必须早于结束时间");
            continue;
        }
        break;
    }

    strcpy(cur_paper->start_time, start_time);
    strcpy(cur_paper->end_time, end_time);
}

/**
 * @brief 菜单功能函数：发布试卷
 * @param cur_paper 正在编辑的试卷结点
 */
void publish_cur_paper(PaperNode *cur_paper)
{
    printf(CLS);
    if (cur_paper->total_questions == 0)
    {
        ERR("试卷中还没有题目");
        usleep(WAITING_TIME);
        return;
    }
    if (cur_paper->question_scores[0] == 0)
    {
        ERR("还没有为试卷中的题目设置分值");
        usleep(WAITING_TIME);
        return;
    }
    if (strcmp(cur_paper->start_time, "2000/01/01 00:00") == 0)
    {
        ERR("还没有为试卷设置考试时间");
        usleep(WAITING_TIME);
        return;
    }

    cur_paper->published = true;
    WAITING("正在发布当前试卷...");
}

PaperNode *init_cur_paper(char *title, int size, int *cur_id)
{
    PaperNode *new_node = (PaperNode *)malloc(sizeof(PaperNode));
    printf(CLS SHOW_CURSOR);
    printf("请输入试卷标题：");
    while (!get_title_input(title, size))
    {
        ERR("标题长度过长，请重新输入");
        printf("请输入试卷标题：");
    }
    strcpy(new_node->title, title);
    new_node->id = *cur_id;
    (*cur_id)++;
    new_node->total_questions = 0;
    new_node->paper_score = 0;
    memset(new_node->question_scores, 0, sizeof(new_node->question_scores));
    memset(new_node->question_ids, 0, sizeof(new_node->question_ids));

    sprintf(new_node->start_time, "%d/%02d/%02d %02d:%02d", 2000, 1, 1, 0, 0);
    sprintf(new_node->end_time, "%d/%02d/%02d %02d:%02d", 2000, 1, 1, 0, 0);

    new_node->published = false;

    return new_node;
}

/**
 * @brief 菜单功能函数：添加试卷
 * @param paper_head 试卷数据链表
 * @param cur_paper 当前处理的试卷的结点
 */
void add_paper(PaperNode *paper_head, PaperNode *cur_paper)
{
    list_paper_add(paper_head, cur_paper->id, cur_paper->title,
                   cur_paper->question_ids, cur_paper->question_scores, cur_paper->total_questions, cur_paper->paper_score,
                   cur_paper->start_time, cur_paper->end_time, cur_paper->published);
}

/**
 * @brief 菜单功能函数：浏览试卷
 * @param question_head 试题数据链表
 * @param cur_paper 要浏览的试卷的结点
 */
void browse_cur_paper(QuestionNode *question_head, PaperNode *cur_paper)
{
    printf(CLS);
    // 显示标题
    printf(CYAN BOLD "%s\n" RESET, cur_paper->title);

    // 显示考试时间
    printf(GREEN BOLD "考试时间：%s - %s\n" RESET, cur_paper->start_time, cur_paper->end_time);

    // 显示试卷分值
    printf(YELLOW BOLD "试卷总分：%d\n" RESET, cur_paper->paper_score);
    print_enter;

    // 显示题号 + 分值 + 题目
    for (int i = 0; i < cur_paper->total_questions; i++)
    {
        QuestionNode *cur_question = list_question_search(question_head, cur_paper->question_ids[i]);
        if (cur_question == NULL)
        {
            printf("第 %d 题不存在或已被删除\n\n", i + 1);
            continue;
        }
        printf("%d.（%d 分）\n", i + 1, cur_paper->question_scores[i]);
        printf("%s\n", cur_question->question);
        // 显示选项
        for (int j = 0; j < 4; j++)
            printf("%c. %s\n", 'A' + j, cur_question->option[j]);
        // 显示答案
        printf("答案：%c\n", cur_question->answer);
        print_enter;
    }

    while (1)
    {
        int key = get_keyboard_input();
        if (key == 'q')
            return;
    }
}

/**
 * @brief 菜单功能辅助函数：打印所有试卷的标题
 * @param head 试卷数据链表
 * @param selection 要突出显示的试卷的索引
 */
void draw_all_paper_title(PaperNode *paper_head, int *paper_ids, int len, int selection, bool show_published)
{
    PaperNode *cur_paper;
    for (int i = 0; i < len; i++)
    {
        cur_paper = list_paper_search(paper_head, paper_ids[i]);
        if (show_published)
        {
            if (i == selection)
            {
                if (cur_paper->published)
                {
                    printf(BG_WHITE BLACK "> %d.(id: %d " RESET, i + 1, cur_paper->id);
                    printf(BG_WHITE GREEN "已发布" RESET);
                    printf(BG_WHITE BLACK ") %s <" RESET, cur_paper->title);
                }
                else
                {
                    printf(BG_WHITE BLACK "> %d.(id: %d " RESET, i + 1, cur_paper->id);
                    printf(BG_WHITE RED "未发布" RESET);
                    printf(BG_WHITE BLACK ") %s <" RESET, cur_paper->title);
                }
            }
            else
            {
                if (cur_paper->published)
                {
                    printf("%d.(id: %d ", i + 1, cur_paper->id);
                    printf(GREEN "已发布" RESET);
                    printf(") %s", cur_paper->title);
                }
                else
                {
                    printf("%d.(id: %d ", i + 1, cur_paper->id);
                    printf(RED "未发布" RESET);
                    printf(") %s", cur_paper->title);
                }
            }
        }
        else
        {
            if (i == selection)
                printf(BG_WHITE BLACK "> %d.(id: %d) %s <" RESET, i + 1, cur_paper->id, cur_paper->title);
            else
                printf("%d.(id: %d) %s", i + 1, cur_paper->id, cur_paper->title);
        }
        print_enter;
    }
}

/**
 * @brief 菜单功能函数：浏览所有已有试卷
 * @param head 试卷数据链表
 */
void browse_paper(PaperNode *paper_head, QuestionNode *question_head)
{
    if (paper_head->next == NULL)
    {
        printf(CLS);
        ERR("还没有试卷");
        usleep(WAITING_TIME);
        return;
    }
    int selection = 0;
    int key;
    int len = 0;
    int paper_ids[MAX_PAPER_COUNT];
    len = list_paper_get_ids(paper_head, paper_ids);
    bubble_sort(paper_ids, len, false);

    while (1)
    {
        printf(CLS);
        draw_all_paper_title(paper_head, paper_ids, len, selection, false);
        key = get_keyboard_input();
        if (key == 'q')
            break;
        else if (key == KEY_DOWN)
            selection = (selection + 1) % len;
        else if (key == KEY_UP)
            selection = (selection - 1 + len) % len;
        else if (key == KEY_ENTER)
        {
            PaperNode *cur_paper = list_paper_search(paper_head, paper_ids[selection]);
            if (cur_paper != NULL)
                browse_cur_paper(question_head, cur_paper);
        }
    }
}

/**
 * @brief 菜单功能函数：删除试卷
 * @param head 试卷数据链表
 */
void delete_paper(PaperNode *head)
{
    if (head->next == NULL)
    {
        printf(CLS);
        ERR("还没有试卷");
        usleep(WAITING_TIME);
        return;
    }
    int selection = 0;
    int key;
    int len = 0;
    int paper_ids[MAX_PAPER_COUNT];

    while (1)
    {
        len = list_paper_get_ids(head, paper_ids);
        bubble_sort(paper_ids, len, false);
        printf(CLS);
        draw_all_paper_title(head, paper_ids, len, selection, false);
        key = get_keyboard_input();
        if (key == 'q')
            break;
        else if (key == KEY_DOWN)
            selection = (selection + 1) % len;
        else if (key == KEY_UP)
            selection = (selection - 1 + len) % len;
        else if (key == KEY_ENTER)
        {
            list_paper_delete(head, paper_ids[selection]);
            if (selection == len - 1 && selection >= 1)
                selection--;
        }
    }
}

/**
 * @brief 菜单功能函数：发布试卷
 * @param head 试卷数据链表
 */
void publish_paper(PaperNode *head)
{
    if (head->next == NULL)
    {
        printf(CLS);
        ERR("还没有试卷");
        usleep(WAITING_TIME);
        return;
    }
    int selection = 0;
    int key;
    int len = 0;
    int paper_ids[MAX_PAPER_COUNT];

    while (1)
    {
        len = list_paper_get_ids(head, paper_ids);
        bubble_sort(paper_ids, len, false);
        printf(CLS);
        draw_all_paper_title(head, paper_ids, len, selection, true);
        key = get_keyboard_input();
        if (key == 'q')
            break;
        else if (key == KEY_DOWN)
            selection = (selection + 1) % len;
        else if (key == KEY_UP)
            selection = (selection - 1 + len) % len;
        else if (key == KEY_ENTER)
        {
            PaperNode *cur_paper = list_paper_search(head, paper_ids[selection]);
            if (cur_paper->published)
            {
                continue;
                // publish_cur_paper(cur_paper);
            }
            else
                publish_cur_paper(cur_paper);
        }
    }
}

/**
 * @brief 菜单功能函数：获取要编辑的试卷结点
 * @param head 试卷数据链表
 * @return 返回要编辑的试卷结点
 */
PaperNode *get_chose_paper(PaperNode *head)
{
    if (head->next == NULL)
    {
        printf(CLS);
        ERR("还没有试卷");
        usleep(WAITING_TIME);
        return NULL;
    }
    int selection = 0;
    int key;
    int len = 0;
    int paper_ids[MAX_PAPER_COUNT];

    while (1)
    {
        len = list_paper_get_ids(head, paper_ids);
        bubble_sort(paper_ids, len, false);
        printf(CLS);
        draw_all_paper_title(head, paper_ids, len, selection, true);
        key = get_keyboard_input();
        if (key == 'q')
            break;
        else if (key == KEY_DOWN)
            selection = (selection + 1) % len;
        else if (key == KEY_UP)
            selection = (selection - 1 + len) % len;
        else if (key == KEY_ENTER)
        {
            PaperNode *cur_paper = list_paper_search(head, paper_ids[selection]);
            return cur_paper;
        }
    }
    return NULL;
}

/**
 * @brief 导出试卷
 * @param question_head 试题数据链表
 * @param cur_paper 要导出的试卷的结点
 */
void export_paper(QuestionNode *question_head, PaperNode *cur_paper)
{
    char file_path[TIMELEN + TITLELEN + 1];
    Date date = get_date();
    sprintf(file_path, "%s/%04d_%02d_%02d-%02d_%02d-%d.txt", EXPORTED_PAPER_PATH, date.year, date.month, date.day, date.hour, date.minute, cur_paper->id);
    printf(CLS HIDE_CURSOR);
    if (save_paper(question_head, cur_paper, file_path))
    {
        printf(HIDE_CURSOR);
        INFO("导出成功");
        usleep(WAITING_TIME);
    }
    else
    {
        printf(HIDE_CURSOR);
        ERR("导出失败");
        usleep(WAITING_TIME);
    }
}
