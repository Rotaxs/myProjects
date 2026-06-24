#include "utils.h"
#include "common.h"
#include "file_io.h"
#include <conio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>

/**
 * @brief 工具函数：加密与解密
 * @param pwd 密码
 */
void cipher(char *pwd)
{
    for (int i = 0; pwd[i] != '\0'; i++)
        pwd[i] ^= ENCRYPT_KEY;
}

/**
 * @brief 工具函数：刷新缓冲区
 */
void flush_buffer()
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

/**
 * @brief 工具函数：非阻塞输入，并将输入的值返回
 * @return 键盘输入的值
 */
int get_keyboard_input()
{
    // 非阻塞输入
    int ch = getch();
    if (ch == KEY_GUIDE)
    {
        return getch();
    }
    return ch;
}

/**
 * @brief 工具函数：在输入密码时，判断输入的字符是否合法
 * @param ch 要判断的字符
 */
bool is_valid_char(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '@' || ch == '#' || ch == '_';
}

/**
 * @brief 工具函数：获取账号和密码的输入
 * @param str 输入的值会保存在 str 中
 * @param str_size str 字符串的大小
 * @param show_str 输入的时候是否明文显示输入的字符
 * @return 成功返回 true，否则返回 false
 */
bool get_input(char *str, int str_size, bool show_str)
{
    printf(SHOW_CURSOR);
    int len = 0;
    int ch;
    while (1)
    {
        ch = get_keyboard_input();
        if (ch == KEY_ENTER)
        {
            if (!show_str && len < MIN_PWD_LEN)
                return false;
            if (show_str && len < 1)
                return false;
            break;
        }
        if (ch == KEY_BACK && len > 0)
        {
            // 回退，打印空格覆盖最后一个字符，再回退
            printf("\b \b");
            len--;
        }
        else if (ch == KEY_RIGHT || ch == KEY_LEFT || ch == KEY_UP || ch == KEY_DOWN || ch == KEY_END || ch == KEY_HOME || ch == KEY_DEL || ch == KEY_PGDN || ch == KEY_PGUP)
            continue;
        else if (is_valid_char(ch))
        {
            if (show_str)
            {
                if (len < MAX_ACCOUNT_LEN && len < str_size - 1)
                {
                    printf("%c", ch);
                    str[len] = ch;
                    len++;
                }
            }
            else
            {
                if (len < MAX_PWD_LEN && len < str_size - 1)
                {
                    printf(YELLOW "*" RESET);
                    str[len] = ch;
                    len++;
                }
            }
        }
    }
    str[len] = '\0';
    return true;
}

/**
 * @brief 工具函数：获取键盘密码输入的工具函数
 * @param pwd 将键盘的合法输入存入 pwd 中
 * @param pwd_size 在传入 pwd 时给 pwd 数组开的大小
 * @return 如果密码过短，返回 false，否则返回 true
 */
bool get_password_input(char *pwd, int pwd_size)
{
    return get_input(pwd, pwd_size, false);
}

/**
 * @brief 工具函数：获取账号输入
 * @param account 账号
 * @param account_size 账号字符串的长度
 * @return 如果输入成功，返回 true；否则返回 false
 */
bool get_account_input(char *account, int account_size)
{
    return get_input(account, account_size, true);
}

/**
 * @brief 工具函数：获取试题的输入
 * @param question 试题
 * @param question_size 试题字符串的大小
 * @return 如果输入成功，返回 true，否则返回 false
 */
bool get_question_input(char *question, int question_size)
{
    char line[QLEN + 1];
    question[0] = '\0';
    // 循环读取每一行
    while (fgets(line, sizeof(line), stdin))
    {
        if (line[0] == END_FLAG && line[1] == '\n')
            break;
        if (strlen(question) + strlen(line) < question_size - 1)
            strcat(question, line);
        else
            return false;
    }
    // 处理多余的换行符
    int len = strlen(question);
    while (len > 0 && (question[len - 1] == '\n' || question[len - 1] == '\r'))
    {
        question[len - 1] = '\0';
        len--;
    }
    return true;
}

/**
 * @brief 工具函数：获取试题选项的输入
 * @param option 选项字符串
 * @param option_size 字符串的大小
 * @return 如果输入过长，返回 false，正确输入返回 true
 */
bool get_option_input(char *option, int option_size)
{
    // 1. 直接读取到目标空间（或临时空间）
    if (fgets(option, option_size, stdin) == NULL)
        return false;

    // 检查是否读取到了换行符
    char *nl = strchr(option, '\n');
    if (nl)
        *nl = '\0';
    else
    {
        flush_buffer();
        return false;
    }

    // Windows/Linux 换行时可能存在 \r
    char *rl = strchr(option, '\r');
    if (rl)
        *rl = '\0';
    return true;
}

/**
 * @brief 工具函数：获取试题答案的输入
 * @param answer 答案字符指针
 * @return 如果答案输入合法，返回 true，否则返回 false
 */
bool get_answer_input(char *answer)
{
    char temp;
    if (scanf(" %c", &temp) == 1)
    {
        flush_buffer();
        if (temp >= 'a' && temp <= 'd')
            temp -= 32;
        if (temp >= 'A' && temp <= 'D')
        {
            *answer = temp;
            return true;
        }
    }
    return false;
}

/**
 * @brief 工具函数：获取试题类别的输入
 * @param category 类别字符串
 * @param category_size 类别字符串的大小
 * @return 如果类别不超过长度限制，返回 true，否则返回 false
 */
bool get_category_input(char *category, int category_size)
{
    return get_option_input(category, category_size);
}

/**
 * @brief 工具函数：获取 id 输入
 * @param id id 指针
 * @return 读取成功返回 true，否则返回 false
 */
bool get_id_input(int *id)
{
    if (scanf(" %d", id) == 1)
    {
        flush_buffer();
        return true;
    }
    flush_buffer();
    return false;
}

/**
 * @brief 工具函数：输入 y 还是 n
 * @return 如果是 y，返回 true，其他字符，返回 false
 */
bool get_y_or_n_input()
{
    char ch;
    if (scanf(" %c", &ch) != 1)
        return false;
    flush_buffer();
    if (ch == 'y' || ch == 'Y')
        return true;
    return false;
}

/**
 * @brief 工具函数：获取标题的输入
 * @param title 标题字符串
 * @param title_size 标题字符串的大小
 * @return 如果长度不超过限制，返回 true，否则返回 false
 */
bool get_title_input(char *title, int title_size)
{
    return get_option_input(title, title_size);
}

/**
 * @brief 工具函数：获取分数的输入
 * @param score 分数指针
 * @return 读取成功返回 true，否则返回 false
 */
bool get_score_input(int *score)
{
    return get_id_input(score);
}

/**
 * @brief 工具函数：获取日期的输入
 * @param date 字符串格式的日期
 * @return 如果日期格式正确，返回 true，否则返回 false
 */
bool get_date_input(char *date)
{
    if (fgets(date, TIMELEN + 1, stdin) != NULL)
    {
        date[strcspn(date, "\n")] = '\0';
        flush_buffer();
        return is_valid_date(date);
    }
    return false;
}

/**
 * @brief 工具函数：获取数字的输入
 * @param num 输入的数字的指针
 * @return 读取成功返回 true，否则返回 false
 */
bool get_num_input(int *num)
{
    return get_id_input(num);
}

/**
 * @brief 工具函数：加载动画
 * @param content 加载动画显示的内容
 * @param loading_time 加载动画持续的时间，单位为微秒（μs）
 */
void spin_loading(char *content, int loading_time)
{
    const char *spin = "|/-\\";
    for (int i = 0; i < 20; i++)
    {
        printf("\rWAITING: %s %c", content, spin[i % 4]);
        fflush(stdout);
        usleep(loading_time);
    }
}

/**
 * @brief 工具函数：获取数字的位数
 * @param num 数字
 * @return 返回数字的位数
 */
int get_digit_count(int num)
{
    int res = 0;
    while (num)
    {
        num /= 10;
        res++;
    }
    return res;
}

/**
 * @brief 获取一个字符串在控制台输出的宽度
 * @param str 要判断的字符串
 * @return 字符串输出的宽度
 */
int get_str_width(char *s)
{
    int width = 0;
    while (*s)
    {
        // ASCLL 0000 0000 -> 01111 1111
        if ((*s & 0x80) == 0)
        {
            width += 1;
            s += 1;
        }
        //  2字节字符： 0xC0 -> 0xDF
        else if ((*s & 0xE0) == 0xC0)
        {
            width += 2;
            s += 2;
        }
        // 3字节字符（汉字） 0xE0 -> 0xEF
        else if ((*s & 0xF0) == 0xE0)
        {
            width += 2;
            s += 3;
        }
        // 4字节字符（Emoji） 0xF0 -> 0xF7
        else if ((*s & 0xF8) == 0xF0)
        {
            width += 2;
            s += 4;
        }
        else
        {
            s++;
        }
    }
    return width;
}

/**
 * @brief 交换两个数的值
 * @param a 第一个数的指针
 * @param b 第二个数的指针
 */
void swap(int *a, int *b)
{
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief 工具函数：删除数组中的元素
 * @param arr 要删除元素的数组，数组元素不能重复
 * @param len 数组长度的指针
 * @return 删除成功返回 true，否则返回 false
 */
bool delete_item_from_array(int *arr, int *len, int item)
{
    int target_index;
    for (target_index = 0; target_index < *len; target_index++)
    {
        if (arr[target_index] == item)
            break;
    }
    if (target_index == 0 && arr[0] != item)
        return false;
    (*len)--;
    for (int i = target_index; i < *len; i++)
        arr[i] = arr[i + 1];
}

/**
 * @brief 工具函数：生成一个随机整数
 * @param min 随机整数的下界
 * @param max 随机整数的上界
 */
int randint(int min, int max)
{
    int range = max - min + 1;
    return rand() % range + min;
}

/**
 * @brief 工具函数：（Fisher-Yates 随机洗牌算法）随机获取一个数组中的 n 个元素
 * @param src 原数组
 * @param len 原数组的长度
 * @param dst 随机数组
 * @param n 随机数组的长度
 */
void get_n_random_from_arr(int *src, int len, int *dst, int n)
{
    if (n > len)
        return;
    int *temp = (int *)malloc(sizeof(int) * len);
    for (int i = 0; i < len; i++)
        temp[i] = src[i];
    for (int i = 0; i < n; i++)
    {
        int j = randint(i, len - 1);
        swap(temp + i, temp + j);
        dst[i] = temp[i];
    }
    free(temp);
}

// =============================== 日期相关工具 ====================================

/**
 * @brief 工具函数：获取当前时间
 * @return 返回 Date 结构体
 */
Date get_date()
{
    Date date;
    struct tm *info;
    time_t rawtime;

    time(&rawtime);
    info = localtime(&rawtime);

    date.year = info->tm_year + 1900;
    date.month = info->tm_mon + 1;
    date.day = info->tm_mday;
    date.hour = info->tm_hour;
    date.minute = info->tm_min;

    return date;
}

/**
 * @brief 工具函数：判断一个字符是否是数字字符
 * @param d 要判断的字符
 */
bool is_digit(char d)
{
    return d >= '0' && d <= '9';
}

/**
 * @brief 检查字符串是否为规定的时间格式 YYYY/MM/DD HH:MM
 * @param str 要检查的字符串
 * @return 如果字符串符合要求，返回 true，否则返回 false
 */
bool is_valid_date(char *str)
{
    if (strlen(str) != 16)
        return false;

    // 检查年份
    for (int i = 0; i < 4; i++)
    {
        if (!is_digit(str[i]))
            return false;
    }
    int year = (str[0] - '0') * 1000 + (str[1] - '0') * 100 + (str[2] - '0') * 10 + str[3] - '0';

    // 检查分隔符
    if (str[4] != '/' || str[7] != '/' || str[10] != ' ' || str[13] != ':')
        return false;

    // 检查月
    if (!is_digit(str[5]) || !is_digit(str[6]))
        return false;
    int month = (str[5] - '0') * 10 + str[6] - '0';
    if (month <= 0 || month > 12)
        return false;

    // 检查日
    if (!is_digit(str[8]) || !is_digit(str[9]))
        return false;
    int days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year % 4 == 0 && year % 100 != 0)
        days_per_month[1] = 29;
    else if (year % 400 == 0)
        days_per_month[1] = 29;
    int day = (str[8] - '0') * 10 + str[9] - '0';
    if (day <= 0 || day > days_per_month[month - 1])
        return false;

    // 检查时
    if (!is_digit(str[11]) || !is_digit(str[12]))
        return false;
    int hour = (str[11] - '0') * 10 + str[12] - '0';
    if (hour < 0 || hour >= 24)
        return false;

    // 检查分
    if (!is_digit(str[14]) || !is_digit(str[15]))
        return false;
    int minute = (str[14] - '0') * 10 + str[15] - '0';
    if (minute < 0 || minute >= 60)
        return false;

    return true;
}

/**
 * @brief 将字符串格式的日期转化为结构体
 * @param str 字符串格式的日期，格式为 YYYY/MM/DD HH:MM
 * @return 返回结构体格式的日期
 */
Date str_to_date(char *str)
{
    Date date;
    date.year = (str[0] - '0') * 1000 + (str[1] - '0') * 100 + (str[2] - '0') * 10 + str[3] - '0';
    date.month = (str[5] - '0') * 10 + str[6] - '0';
    date.day = (str[8] - '0') * 10 + str[9] - '0';
    date.hour = (str[11] - '0') * 10 + str[12] - '0';
    date.minute = (str[14] - '0') * 10 + str[15] - '0';
    return date;
}

/**
 * @brief 将日期结构体转化为字符串的格式
 * @param date 日期结构体
 * @param str 最终的字符串，要求长度为 TIMELEN
 */
void date_to_str(Date date, char *str)
{
    sprintf(str, "%d/%02d/%02d %02d:%02d", date.year, date.month, date.day, date.hour, date.minute);
}

/**
 * @brief 比较两个时间的先后
 * @param str1 第一个时间
 * @param str2 第二个时间
 * @return 如果 str1 早于 str2，返回 1，如果 str1 等于 str2，返回 0，如果 str1 晚于 str2，返回 -1，如果格式不正确，返回 2
 */
int date_compare(char *str1, char *str2)
{
    if (!is_valid_date(str1) || !is_valid_date(str2))
        return 2;
    int res = strcmp(str1, str2);
    if (res < 0)
        return 1;
    if (res > 0)
        return -1;
    return 0;
    // Date date1 = str_to_date(str1);
    // Date date2 = str_to_date(str2);
    // // 比较年份
    // if (date1.year < date2.year)
    //     return 1;
    // else if (date1.year > date2.year)
    //     return -1;
    // // 年份相等，比较月份
    // if (date1.month < date2.month)
    //     return 1;
    // else if (date1.month > date2.month)
    //     return -1;
    // // 月份也相等，比较天
    // if (date1.day < date2.day)
    //     return 1;
    // else if (date1.day > date2.day)
    //     return -1;
    // // 天相等，比较时
    // if (date1.hour < date2.hour)
    //     return 1;
    // else if (date1.hour > date2.hour)
    //     return -1;
    // // 时相等，比较分
    // if (date1.minute < date2.minute)
    //     return 1;
    // else if (date1.minute > date2.minute)
    //     return -1;
    // // 分也相等，说明时间相等
    // return 0;
}

/**
 * @brief 冒泡排序，默认升序
 * @param arr 要排序的数据
 * @param len 数组的长度
 * @param reverse 是否倒序
 */
void bubble_sort(int *arr, int len, bool reverse)
{
    bool swapped = false;
    for (int i = 0; i < len - 1; i++)
    {
        for (int j = 0; j < len - i - 1; j++)
        {
            if (!reverse && arr[j] > arr[j + 1])
            {
                swap(arr + j, arr + j + 1);
                swapped = true;
            }
            else if (reverse && arr[j] < arr[j + 1])
            {
                swap(arr + j, arr + j + 1);
                swapped = true;
            }
        }
        if (!swapped)
            break;
    }
}

// =================== 用户数据的链表操作 ====================

UserNode *list_user_create()
{
    UserNode *head = (UserNode *)malloc(sizeof(UserNode));
    head->next = NULL;
    return head;
}

bool list_user_is_reduplicate(UserNode *head, char *account)
{
    UserNode *cur_node = head->next;
    for (; cur_node != NULL; cur_node = cur_node->next)
    {
        if (strcmp(cur_node->account, account) == 0)
            return true;
    }
    return false;
}

bool list_user_add(UserNode *head, int id, char *account, char *password, int exercised_question_count)
{
    if (list_user_is_reduplicate(head, account))
        return false;
    UserNode *user_node = (UserNode *)malloc(sizeof(UserNode));
    user_node->id = id;
    strcpy(user_node->account, account);
    strcpy(user_node->password, password);
    user_node->exercised_question_count = exercised_question_count;
    user_node->exam_record_head = (ExamRecord *)malloc(sizeof(ExamRecord));
    user_node->exam_record_head->next = NULL;
    user_node->next = head->next;
    head->next = user_node;
    return true;
}

bool list_user_modify(UserNode *head, int target_id, char *account, char *password)
{
    UserNode *cur_node = head->next;
    for (; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->id == target_id)
        {
            strcpy(cur_node->account, account);
            strcpy(cur_node->password, password);
            return true;
        }
    }
    return false;
}

bool list_user_delete(UserNode *head, int target_id)
{
    UserNode *cur_node = head;
    for (; cur_node->next; cur_node = cur_node->next)
    {
        if (cur_node->next->id == target_id)
        {
            UserNode *temp = cur_node->next;
            cur_node->next = temp->next;
            free(temp);
            return true;
        }
    }
    return false;
}

void list_user_destroy(UserNode *head)
{
    UserNode *cur = head;
    while (cur != NULL)
    {
        UserNode *next_user = cur->next;
        // 释放该用户的做题记录记录
        ExamRecord *cur_rec = cur->exam_record_head;
        while (cur_rec != NULL)
        {
            ExamRecord *next_rec = cur_rec->next;
            free(cur_rec);
            cur_rec = next_rec;
        }
        free(cur);
        cur = next_user;
    }
}

UserNode *list_user_search(UserNode *head, char *account)
{
    UserNode *cur_node = head->next;
    for (; cur_node; cur_node = cur_node->next)
    {
        if (strcmp(cur_node->account, account) == 0)
            return cur_node;
    }
    return NULL;
}

UserNode *list_user_search_by_id(UserNode *head, int id)
{
    UserNode *cur_node = head->next;
    for (; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->id == id)
            return cur_node;
    }
    return NULL;
}

void list_user_print(UserNode *head)
{
    UserNode *ne = head->next;
    print_enter;
    for (; ne; ne = ne->next)
    {
        printf("id: %d\n", ne->id);
        printf("account: %s\n", ne->account);
        printf("password: %s\n", ne->password);
    }
}

int list_user_get_len(UserNode *head)
{
    int res = 0;
    UserNode *node = head->next;
    for (; node; node = node->next)
        res++;
    return res;
}

int list_user_get_ids(UserNode *head, int *ids)
{
    int len = 0;
    UserNode *node = head->next;
    for (; node; node = node->next)
        ids[len++] = node->id;
    return len;
}

// =================== 试题数据的链表操作 ====================

QuestionNode *list_question_create()
{
    QuestionNode *head = (QuestionNode *)malloc(sizeof(QuestionNode));
    head->next = NULL;
    return head;
}

void list_question_add(QuestionNode *head,
                       int id, char *question, char option[4][OPTIONLEN + 1], char answer, char category[CATEGORYLEN + 1])
{
    QuestionNode *new_node = (QuestionNode *)malloc(sizeof(QuestionNode));
    new_node->id = id;
    strcpy(new_node->question, question);
    for (int i = 0; i < 4; i++)
        strcpy(new_node->option[i], option[i]);
    new_node->answer = answer;
    strcpy(new_node->category, category);
    new_node->next = head->next;
    head->next = new_node;
}

bool list_question_delete(QuestionNode *head, int target_id)
{
    QuestionNode *cur_node = head;
    for (; cur_node->next; cur_node = cur_node->next)
    {
        if (cur_node->next->id == target_id)
        {
            QuestionNode *temp = cur_node->next;
            cur_node->next = temp->next;
            free(temp);
            return true;
        }
    }

    return false;
}

bool list_question_modify(QuestionNode *head,
                          int target_id, char *question, char option[4][OPTIONLEN + 1], char answer, char category[CATEGORYLEN + 1])
{
    QuestionNode *cur_node = head->next;
    for (; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->id == target_id)
        {
            strcpy(cur_node->question, question);
            for (int i = 0; i < 4; i++)
                strcpy(cur_node->option[i], option[i]);
            cur_node->answer = answer;
            strcpy(cur_node->category, category);
            return true;
        }
    }
    return false;
}

int list_question_search_by_category(QuestionNode *head, char *category, int *ids)
{
    int size = 0;
    QuestionNode *cur_node = head->next;
    for (; cur_node; cur_node = cur_node->next)
    {
        if (strcmp(cur_node->category, category) == 0)
        {
            ids[size++] = cur_node->id;
        }
    }
    return size;
}

QuestionNode *list_question_search(QuestionNode *head, int id)
{
    QuestionNode *cur_node = head->next;
    for (; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->id == id)
            return cur_node;
    }
    return NULL;
}

void list_question_destroy(QuestionNode *head)
{
    QuestionNode *cur = head;
    while (cur != NULL)
    {
        QuestionNode *next = cur->next;
        free(cur);
        cur = next;
    }
}

int list_question_get_len(QuestionNode *head)
{
    int total = 0;
    QuestionNode *node = head->next;
    for (; node; node = node->next)
        total++;
    return total;
}

int list_question_get_ids(QuestionNode *head, int *ids)
{
    int len = 0;
    QuestionNode *node = head->next;
    for (; node; node = node->next)
        ids[len++] = node->id;
    return len;
}

/**
 * @brief 查找 id 为 id 的题目是否已经出现在试卷库中
 * @param id 要查找的题目的 id
 * @param head 试卷数据链表
 * @return 如果存在，返回 true，否则返回 false
 */
bool id_in_paper(int id, PaperNode *head)
{
    PaperNode *cur_paper = head->next;
    for (; cur_paper; cur_paper = cur_paper->next)
    {
        for (int i = 0; i < cur_paper->total_questions; i++)
        {
            if (cur_paper->question_ids[i] == id)
                return true;
        }
    }
    return false;
}

// =================== 试卷数据的链表操作 ====================

PaperNode *list_paper_create()
{
    PaperNode *head = (PaperNode *)malloc(sizeof(PaperNode));
    head->next = NULL;
    return head;
}

void list_paper_add(PaperNode *head, int id, char *title,
                    int *question_ids, int *question_scores,
                    int total_question, int paper_score,
                    char *start_time, char *end_time, bool published)
{
    PaperNode *new_node = (PaperNode *)malloc(sizeof(PaperNode));

    new_node->id = id;
    strcpy(new_node->title, title);
    for (int i = 0; i < total_question; i++)
    {
        new_node->question_ids[i] = question_ids[i];
        new_node->question_scores[i] = question_scores[i];
    }
    new_node->total_questions = total_question;
    new_node->paper_score = paper_score;
    strcpy(new_node->start_time, start_time);
    strcpy(new_node->end_time, end_time);
    new_node->published = published;

    new_node->next = head->next;
    head->next = new_node;
}

bool list_paper_delete(PaperNode *head, int target_id)
{
    PaperNode *cur_node = head;
    for (; cur_node->next; cur_node = cur_node->next)
    {
        if (cur_node->next->id == target_id)
        {
            PaperNode *temp = cur_node->next;
            cur_node->next = temp->next;
            free(temp);
            return true;
        }
    }
    return false;
}

bool list_paper_modify(PaperNode *head, int target_id, char *title,
                       int *question_ids, int *question_scores,
                       int total_question, int paper_score,
                       char *start_time, char *end_time)
{
    PaperNode *cur_node = head->next;
    for (; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->id == target_id)
        {
            strcpy(cur_node->title, title);
            memset(cur_node->question_ids, 0, sizeof(int) * MAX_PAPER_QUESTIONS);
            memset(cur_node->question_scores, 0, sizeof(int) * MAX_PAPER_QUESTIONS);
            for (int i = 0; i < total_question; i++)
            {
                cur_node->question_ids[i] = question_ids[i];
                cur_node->question_scores[i] = question_scores[i];
            }
            cur_node->total_questions = total_question;
            cur_node->paper_score = paper_score;
            strcpy(cur_node->start_time, start_time);
            strcpy(cur_node->end_time, end_time);
            return true;
        }
    }
    return false;
}

PaperNode *list_paper_search(PaperNode *head, int id)
{
    PaperNode *cur_paper = head->next;
    for (; cur_paper; cur_paper = cur_paper->next)
    {
        if (cur_paper->id == id)
            return cur_paper;
    }
    return NULL;
}

void list_paper_destroy(PaperNode *head)
{
    PaperNode *cur = head;
    while (cur != NULL)
    {
        PaperNode *next = cur->next;
        free(cur);
        cur = next;
    }
}

int list_paper_get_len(PaperNode *head)
{
    PaperNode *node = head->next;
    int len = 0;
    for (; node; node = node->next)
        len++;
    return len;
}

int list_paper_get_ids(PaperNode *head, int *ids)
{
    PaperNode *node = head->next;
    int len = 0;
    for (; node; node = node->next)
        ids[len++] = node->id;
    return len;
}

int list_paper_get_published(PaperNode *head, int *ids, int ids_len)
{
    PaperNode *cur_paper = head->next;
    int len = 0;
    for (; cur_paper; cur_paper = cur_paper->next)
    {
        if (cur_paper->published)
        {
            if (len < ids_len)
                ids[len++] = cur_paper->id;
        }
    }
    return len;
}

// ===================== 用户考试记录链表操作 ======================

ExamRecord *list_examRecord_add(ExamRecord *head, int paper_id, int score, bool is_finished, char *start_time, char *end_time, int *choices)
{
    ExamRecord *new_node = (ExamRecord *)malloc(sizeof(ExamRecord));
    new_node->paper_id = paper_id;
    new_node->score = score;
    new_node->is_finished = is_finished;
    strcpy(new_node->start_time, start_time);
    strcpy(new_node->end_time, end_time);
    memcpy(new_node->choices, choices, sizeof(int) * MAX_PAPER_QUESTIONS);
    new_node->next = head->next;
    head->next = new_node;
    return new_node;
}

/**
 * @brief 打印字符画（字母）
 * @param text 要打印的文本，可以有空格
 * @param pre_space 字符画前面的空格数
 */
void print_text(char *text, int pre_space)
{
    // 使用 7x5 矩阵存储 26 个字母的点阵数据
    // 1 代表显示块，0 代表空格
    int alpha_data[26][7][5] = {
        // A
        {{0, 0, 1, 0, 0}, {0, 1, 0, 1, 0}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}},
        // B
        {{1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 0}},
        // C
        {{0, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {0, 1, 1, 1, 1}},
        // D
        {{1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 0}},
        // E
        {{1, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}},
        // F
        {{1, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}},
        // G
        {{0, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 1}},
        // H
        {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}},
        // I
        {{1, 1, 1, 1, 1}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {1, 1, 1, 1, 1}},
        // J
        {{0, 0, 1, 1, 1}, {0, 0, 0, 1, 0}, {0, 0, 0, 1, 0}, {0, 0, 0, 1, 0}, {0, 0, 0, 1, 0}, {1, 0, 0, 1, 0}, {0, 1, 1, 0, 0}},
        // K
        {{1, 0, 0, 0, 1}, {1, 0, 0, 1, 0}, {1, 0, 1, 0, 0}, {1, 1, 0, 0, 0}, {1, 0, 1, 0, 0}, {1, 0, 0, 1, 0}, {1, 0, 0, 0, 1}},
        // L
        {{1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}},
        // M
        {{1, 0, 0, 0, 1}, {1, 1, 0, 1, 1}, {1, 0, 1, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}},
        // N
        {{1, 0, 0, 0, 1}, {1, 1, 0, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 0, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}},
        // O
        {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}},
        // P
        {{1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}},
        // Q
        {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 0, 1, 0}, {0, 1, 1, 0, 1}},
        // R
        {{1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 0}, {1, 0, 1, 0, 0}, {1, 0, 0, 1, 0}, {1, 0, 0, 0, 1}},
        // S
        {{0, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {0, 1, 1, 1, 0}, {0, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {1, 1, 1, 1, 0}},
        // T
        {{1, 1, 1, 1, 1}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}},
        // U
        {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}},
        // V
        {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 0, 1, 0, 0}},
        // W
        {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 1, 0, 1, 1}, {1, 0, 0, 0, 1}},
        // X
        {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 1, 0, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}},
        // Y
        {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}},
        // Z
        {{1, 1, 1, 1, 1}, {0, 0, 0, 0, 1}, {0, 0, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 1, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}}};

    for (int row = 0; row < 7; row++)
    {
        print_space(pre_space);
        for (int i = 0; text[i] != '\0'; i++)
        {
            char c = toupper(text[i]);
            if (c == ' ')
            {
                printf("    ");
                continue;
            }

            if (c >= 'A' && c <= 'Z')
            {
                int idx = c - 'A';
                for (int col = 0; col < 5; col++)
                {
                    if (alpha_data[idx][row][col])
                        printf(BANNER_CHAR);
                    else
                        printf(" ");
                }
                printf("  ");
            }
        }
        printf("\n");
    }
}
