#include "menu.h"
#include "common.h"
#include "admin.h"
#include "user.h"
#include "file_io.h"
#include "utils.h"

/**
 * @brief 获取若干选项中最短的选项，用于对齐
 * @param options 选项数组
 * @param len 选项的个数
 */
int get_shortest_option(const Text *options, const int len)
{
    int min_id = 0;
    for (int i = 1; i < len; i++)
    {
        if (options[i].length < options[min_id].length)
            min_id = i;
    }
    return min_id;
}

/**
 * @brief 绘制菜单
 * @param title 菜单的标题
 * @param prompt 菜单下的提示，如操作提示等
 * @param selection 选项 （0 - 2）
 * @param options Text 类型的数组，包含所有选项
 * @param len 选项的个数
 */
void draw_menu(const Text title, const Text prompt,
               const int selection, const Text *options, const int len)
{
    const int total_length = MENU_WIDTH;
    const int title_length = title.length;
    const int left_interval_length = (MENU_WIDTH - 2 - title_length) / 2;
    const int right_interval_length = total_length - left_interval_length - title_length - 2;

    CURSOR_RESET;

    // 标题部分（框 + 标题）
    print_table(TABLE_UL, 1);
    print_table(TABLE_U, total_length - 2);
    print_table(TABLE_UR, 1);
    print_enter;
    print_table(TABLE_L, 1);
    print_space(left_interval_length);
    printf(BOLD MENU_TITLE_COLOR "%s" RESET, title.content);
    print_space(right_interval_length);
    print_table(TABLE_R, 1);
    print_enter;
    print_table(TABLE_BL, 1);
    print_table(TABLE_B, total_length - 2);
    print_table(TABLE_BR, 1);

    print_enter;
    print_enter;

    int shortest_option_id = get_shortest_option(options, len);
    int shortest_len = options[shortest_option_id].length;
    // 选项部分
    for (int i = 0; i < len; i++)
    {
        char *text = options[i].content;
        printf(BOLD);

        if (selection == i)
        {
            print_space((total_length - shortest_len) / 2 - 4);
            printf(GREEN BG_WHITE);
            if (selection == len - 1)
                printf(RED);
            printf(">>> %s", text);
        }
        else
        {
            print_space((total_length - shortest_len) / 2);
            printf("%s", text);
        }
        printf(RESET);
        print_enter;
    }

    // 提示部分
    print_enter;
    print_space((total_length - prompt.length) / 2);
    printf(BOLD YELLOW "%s" RESET, prompt.content);
    print_enter;
}

void main_menu(UserNode *user_head, PaperNode *paper_head, QuestionNode *question_head,
               int *user_id, int *paper_id, int *question_id)
{
    const int option_count = 3; // 选项的个数
    const int admin_option = 0, stu_option = 1, exit_option = 2;
    const Text title = {"标准化选择题考试系统 v1.0", 25};
    const Text prompt = {"使用 [↑/↓] 移动，[Enter] 确认，q/Esc 退出", 41};
    Text options[] = {
        {"1. 管理员登录", 13},
        {"2. 学生登录", 11},
        {"0. 退出系统", 11}};

    printf(CLS HIDE_CURSOR);
    int selection = 0;
    while (1)
    {
        draw_menu(title, prompt, selection, options, option_count);
        int key = get_keyboard_input();

        if (key == KEY_DOWN)
            selection = (selection + 1) % option_count;
        else if (key == KEY_UP)
            selection = (selection - 1 + option_count) % option_count;
        else if (key == 'q' || key == KEY_ESC)
            break;
        else if (key == KEY_ENTER)
        {
            if (selection == exit_option)
                break;
            else if (selection == admin_option)
            {
                if (admin_login())
                {
                    admin_menu(user_head, paper_head, question_head, user_id, paper_id, question_id);
                    printf(CLS HIDE_CURSOR);
                }
                else
                {
                    printf(CLS HIDE_CURSOR);
                    continue;
                }
            }
            else if (selection == stu_option)
            {
                UserNode *cur_stu = user_login(user_head);
                if (cur_stu != NULL)
                {
                    stu_menu(cur_stu, user_head, question_head, paper_head);
                    printf(CLS HIDE_CURSOR);
                }
                else
                {
                    printf(CLS HIDE_CURSOR);
                    continue;
                }
            }
        }
    }

    printf(SHOW_CURSOR CLS);
    // printf("您已经安全退出标准化选择题考试系统\n");
    // printf("欢迎下次使用\n");
}

void admin_menu(UserNode *user_head, PaperNode *paper_head, QuestionNode *question_head,
                int *user_id, int *paper_id, int *question_id)
{
    const int option_count = 5;
    const int question_option = 0, user_option = 1, paper_option = 2, password_option = 3, exit_option = 4;
    const Text title = {"管理员操作界面", 14};
    const Text prompt = {"使用 [↑/↓] 移动，[Enter] 确认，q/Esc 退出", 41};
    Text options[] = {
        {"1. 试题管理", 11},
        {"2. 用户管理", 11},
        {"3. 试卷管理", 11},
        {"4. 修改密码", 11},
        {"0. 返回主菜单", 13}};

    printf(HIDE_CURSOR CLS);
    int selection = 0;

    while (1)
    {
        draw_menu(title, prompt, selection, options, option_count);
        int key = get_keyboard_input();

        if (key == KEY_DOWN)
            selection = (selection + 1) % option_count;
        else if (key == KEY_UP)
            selection = (selection - 1 + option_count) % option_count;
        else if (key == 'q' || key == KEY_ESC)
            break;
        else if (key == KEY_ENTER)
        {
            if (selection == exit_option)
                break;
            else if (selection == user_option)
            {
                admin_manage_stu_menu(user_head, paper_head, user_id, paper_id, question_id);
            }
            else if (selection == paper_option)
            {
                admin_manage_paper_menu(paper_head, question_head, user_id, paper_id, question_id);
            }
            else if (selection == question_option)
            {
                admin_manage_question_menu(question_head, paper_head, user_id, paper_id, question_id);
            }
            else if (selection == password_option)
            {
                printf(SHOW_CURSOR);
                modify_admin_password();
            }
            printf(CLS HIDE_CURSOR);
        }
    }
}

void admin_manage_stu_menu(UserNode *user_head, PaperNode *paper_head, int *cur_stu_id, int *cur_paper_id, int *cur_question_id)
{
    const int option_count = 5;
    const int add_option = 0, del_option = 1, modify_option = 2, search_option = 3, exit_option = 4;
    const Text title = {"学生信息管理", 12};
    char buffer[40];
    Text prompt;
    int user_count;

    Text options[] = {
        {"1. 添加用户", 11},
        {"2. 删除用户", 11},
        {"3. 修改用户信息", 15},
        {"4. 浏览用户做题信息", 19},
        {"0. 返回上一级", 13}};

    printf(HIDE_CURSOR CLS);
    int selection = 0;

    while (1)
    {
        user_count = list_user_get_len(user_head);
        sprintf(buffer, "系统中有 %d 位学生的信息", user_count);
        prompt.content = buffer;
        prompt.length = 22 + get_digit_count(user_count);
        draw_menu(title, prompt, selection, options, option_count);
        int key = get_keyboard_input();

        if (key == KEY_DOWN)
            selection = (selection + 1) % option_count;
        else if (key == KEY_UP)
            selection = (selection - 1 + option_count) % option_count;
        else if (key == 'q' || key == KEY_ESC)
            break;
        else if (key == KEY_ENTER)
        {
            if (selection == exit_option)
                break;
            if (selection == add_option)
            {
                add_stu(user_head, cur_stu_id);
                save_user_data(user_head, paper_head);
                save_ids(*cur_stu_id, *cur_paper_id, *cur_question_id);
            }
            else if (selection == del_option)
            {
                del_stu(user_head);
                save_user_data(user_head, paper_head);
            }
            else if (selection == modify_option)
            {
                modify_stu(user_head);
                save_user_data(user_head, paper_head);
            }
            else if (selection == search_option)
            {
                printf(CLS);
                show_stu_exercise_info(user_head);
            }
            printf(CLS HIDE_CURSOR);
        }
    }
}

void admin_assemble_paper_menu(QuestionNode *question_head, PaperNode *cur_paper)
{
    const int option_count = 8;
    const int assemble_option = 0, title_option = 1, score_option = 2, time_option = 3, browse_option = 4, publish_option = 5, export_option = 6, exit_option = 7;
    char buffer[100];
    const Text title = {"组卷系统", 8};
    Text prompt;
    Text options[] = {
        {"1. 添加试题", 11},
        {"2. 修改试卷标题", 15},
        {"3. 设置试卷分值", 15},
        {"4. 设置考试时间", 15},
        {"5. 浏览当前试卷", 15},
        {"6. 发布当前试卷", 15},
        {"7. 导出当前试卷", 15},
        {"0. 保存并退出", 13}};

    printf(HIDE_CURSOR CLS);
    int selection = 0;

    while (1)
    {
        sprintf(buffer, "正在编辑试卷 %s，试卷 ID 为 %d", cur_paper->title, cur_paper->id);
        prompt.content = buffer;
        prompt.length = 26 + get_digit_count(cur_paper->id) + get_str_width(cur_paper->title);
        draw_menu(title, prompt, selection, options, option_count);
        int key = get_keyboard_input();

        if (key == KEY_DOWN)
            selection = (selection + 1) % option_count;
        else if (key == KEY_UP)
            selection = (selection - 1 + option_count) % option_count;
        else if (key == 'q' || key == KEY_ESC)
            break;
        else if (key == KEY_ENTER)
        {
            if (selection == exit_option)
                break;
            else if (selection == assemble_option)
            {
                choose_question(question_head, cur_paper);
            }
            else if (selection == title_option)
            {
                printf(SHOW_CURSOR);
                modify_title(cur_paper);
                printf(HIDE_CURSOR);
            }
            else if (selection == score_option)
            {
                printf(SHOW_CURSOR);
                set_scores(question_head, cur_paper);
                printf(HIDE_CURSOR);
            }
            else if (selection == time_option)
            {
                printf(SHOW_CURSOR);
                set_time(cur_paper);
                printf(HIDE_CURSOR);
            }
            else if (selection == publish_option)
            {
                printf(HIDE_CURSOR);
                publish_cur_paper(cur_paper);
            }
            else if (selection == browse_option)
            {
                printf(HIDE_CURSOR);
                browse_cur_paper(question_head, cur_paper);
            }
            else if (selection == export_option)
            {
                export_paper(question_head, cur_paper);
            }
            printf(HIDE_CURSOR CLS);
        }
    }
}

void admin_manage_paper_menu(PaperNode *paper_head, QuestionNode *question_head, int *cur_stu_id, int *cur_paper_id, int *cur_question_id)
{
    const int option_count = 7;
    const int assemble_option = 0, browse_option = 1, delete_option = 2, edit_option = 3, publish_option = 4, export_option = 5, exit_option = 6;
    const Text title = {"试卷管理", 8};
    char buffer[40];
    int len;
    Text prompt;
    Text options[] = {
        {"1. 开始组卷", 11},
        {"2. 浏览试卷", 11},
        {"3. 删除试卷", 11},
        {"4. 编辑试卷", 11},
        {"5. 发布试卷", 11},
        {"6. 导出试卷", 11},
        {"0. 返回上一级", 13}};

    printf(HIDE_CURSOR CLS);
    int selection = 0;

    while (1)
    {
        len = list_paper_get_len(paper_head);
        sprintf(buffer, "当前共有 %d 张试卷", len);
        prompt.content = buffer;
        prompt.length = 16 + get_digit_count(len);
        draw_menu(title, prompt, selection, options, option_count);
        int key = get_keyboard_input();

        if (key == KEY_DOWN)
            selection = (selection + 1) % option_count;
        else if (key == KEY_UP)
            selection = (selection - 1 + option_count) % option_count;
        else if (key == 'q' || key == KEY_ESC)
            break;
        else if (key == KEY_ENTER)
        {
            if (selection == exit_option)
                break;
            else if (selection == assemble_option)
            {
                char title[TITLELEN + 1];
                PaperNode *cur_paper = init_cur_paper(title, TITLELEN + 1, cur_paper_id);
                printf(HIDE_CURSOR);
                admin_assemble_paper_menu(question_head, cur_paper);

                add_paper(paper_head, cur_paper);
                save_paper_data(paper_head);
                save_ids(*cur_stu_id, *cur_paper_id, *cur_question_id);
            }
            else if (selection == browse_option)
            {
                printf(HIDE_CURSOR);
                browse_paper(paper_head, question_head);
            }
            else if (selection == delete_option)
            {
                printf(HIDE_CURSOR);
                delete_paper(paper_head);
                save_paper_data(paper_head);
            }
            else if (selection == publish_option)
            {
                printf(HIDE_CURSOR);
                publish_paper(paper_head);
                save_paper_data(paper_head);
            }
            else if (selection == edit_option)
            {
                PaperNode *cur_paper = get_chose_paper(paper_head);
                if (cur_paper != NULL)
                {
                    admin_assemble_paper_menu(question_head, cur_paper);
                    save_paper_data(paper_head);
                }
            }
            else if (selection == export_option)
            {
                PaperNode *cur_paper = get_chose_paper(paper_head);
                if (cur_paper != NULL)
                    export_paper(question_head, cur_paper);
            }
            printf(CLS HIDE_CURSOR);
        }
    }
}

void admin_manage_question_menu(QuestionNode *question_head, PaperNode *paper_head, int *cur_stu_id, int *cur_paper_id, int *cur_question_id)
{
    const int option_count = 6;
    const int add_option = 0, del_option = 1, modify_option = 2, search_option = 3, browse_option = 4, exit_option = 5;
    int question_count;
    char buffer[35];
    const Text title = {"试题管理", 8};
    Text prompt;
    Text options[] = {
        {"1. 添加试题", 11},
        {"2. 删除试题", 11},
        {"3. 修改试题", 11},
        {"4. 搜索试题", 11},
        {"5. 浏览试题", 11},
        {"0. 返回主菜单", 13}};

    printf(HIDE_CURSOR CLS);
    int selection = 0;

    while (1)
    {
        question_count = list_question_get_len(question_head);
        sprintf(buffer, "题库目前共有 %d 道试题", question_count);
        prompt.content = buffer;
        prompt.length = 20 + get_digit_count(question_count);
        draw_menu(title, prompt, selection, options, option_count);
        int key = get_keyboard_input();

        if (key == KEY_DOWN)
            selection = (selection + 1) % option_count;
        else if (key == KEY_UP)
            selection = (selection - 1 + option_count) % option_count;
        else if (key == 'q' || key == KEY_ESC)
            break;
        else if (key == KEY_ENTER)
        {
            if (selection == exit_option)
                break;
            else if (selection == add_option)
            {
                printf(SHOW_CURSOR);
                add_question(question_head, cur_question_id);
                save_question_data(question_head);
                save_ids(*cur_stu_id, *cur_paper_id, *cur_question_id);
                printf(HIDE_CURSOR);
            }
            else if (selection == del_option)
            {
                printf(SHOW_CURSOR);
                del_question(question_head, paper_head);
                save_question_data(question_head);
                printf(HIDE_CURSOR);
            }
            else if (selection == modify_option)
            {
                printf(SHOW_CURSOR);
                modify_question(question_head);
                save_question_data(question_head);
                printf(HIDE_CURSOR);
            }
            else if (selection == search_option)
            {
                printf(SHOW_CURSOR);
                search_question(question_head);
                printf(HIDE_CURSOR);
            }
            else if (selection == browse_option)
            {
                printf(HIDE_CURSOR);
                browse_question(question_head);
            }
            printf(CLS HIDE_CURSOR);
        }
    }
}

void stu_menu(UserNode *cur_stu, UserNode *user_head, QuestionNode *question_head, PaperNode *paper_head)
{
    const int option_count = 4;
    const int exercise_option = 0, exam_option = 1, password_option = 2, exit_option = 3;
    const Text title = {"学生操作界面", 12};
    const Text prompt = {"使用 [↑/↓] 移动，[Enter] 确认，q/Esc 退出", 41};
    Text options[] = {
        {"1. 练习", 7},
        {"2. 考试", 7},
        {"3. 修改密码", 11},
        {"0. 返回主菜单", 13}};

    printf(CLS HIDE_CURSOR);
    int selection = 0;

    while (1)
    {
        draw_menu(title, prompt, selection, options, option_count);
        int key = get_keyboard_input();

        if (key == KEY_DOWN)
            selection = (selection + 1) % option_count;
        else if (key == KEY_UP)
            selection = (selection - 1 + option_count) % option_count;
        else if (key == 'q' || key == KEY_ESC)
            break;
        else if (key == KEY_ENTER)
        {
            if (selection == exit_option)
                break;
            else if (selection == password_option)
            {
                printf(SHOW_CURSOR);
                modify_password(cur_stu);
                printf(HIDE_CURSOR CLS);
            }
            else if (selection == exercise_option)
            {
                printf(SHOW_CURSOR);
                exercise(cur_stu, question_head);
                save_user_data(user_head, paper_head);
                printf(HIDE_CURSOR CLS);
            }
            else if (selection == exam_option)
            {
                printf(HIDE_CURSOR);
                exam(cur_stu, paper_head, question_head);
                save_user_data(user_head, paper_head);
                printf(HIDE_CURSOR CLS);
            }
        }
    }
}
