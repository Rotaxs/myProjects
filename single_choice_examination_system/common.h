#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <windows.h>
#include <stdlib.h>

// 题目相关常量
#define QLEN 512                         // 每道题字符串的最大长度
#define OPTIONLEN 128                    // 每个选项字符串的最大长度
#define CATEGORYLEN 32                   // 标签/类别的字符串最大长度
#define QDATAPATH "./data/questions.dat" // 本地题库路径
#define MAXQ_COUNT 1000                  // 题库中题目数量的最大值
#define ONCE_COUNT 2                     // 浏览试题时一次显示的题目数量
#define END_FLAG '#'                     // 输入试题时的结束标志

// 试卷及系统常量
#define TITLELEN 100                        // 试卷标题字符串的最大长度
#define MAX_PAPER_QUESTIONS 50              // 试卷最大题目数
#define TIMELEN 16                          // 格式如 "2025-12-28 15:00"
#define MAX_PAPER_COUNT 100                 // 本地保存的试卷的最大数量
#define PAPERDATAPATH "./data/papers.dat"   // 本地保存试卷数据的位置
#define EXPORTED_PAPER_PATH "./data/papers" // 保存导出的试卷的文件夹

// 用户相关常量
#define USERDATAPATH "./data/users.dat"     // 本地存储用户信息的位置
#define EXAMRECORDPATH "./data/records.csv" // 保存用户做题记录的位置
#define MAX_USER_COUNT 1000                 // 用户的最大数量

// 菜单相关
#define MENU_WIDTH 60            // 菜单的宽度
#define MENU_BORDER_COLOR YELLOW // 菜单边框的颜色
#define MENU_TITLE_COLOR CYAN    // 菜单标题的颜色

// 账号与密码
#define ENCRYPT_KEY 66                     // 密钥
#define ADMIN_ORIGIN_PWD "123456789"       // 管理员初始密码
#define ADMIN_PWD_PATH "./data/.admin.dat" // 管理员密码保存路径
#define MAX_ACCOUNT_LEN 20                 // 账号的最大长度
#define MIN_ACCOUNT_LEN 6                  // 账号的最小长度
#define MAX_PWD_LEN 20                     // 密码的最大长度
#define MIN_PWD_LEN 6                      // 密码的最小长度
#define PWD_ATTEMP_COUNT 5                 // 密码尝试次数

#define DATA_PATH "./data"        // data 文件夹路径
#define IDS_PATH "./data/ids.dat" // 最新 id 保存路径

#define BANNER_CHAR "\u2588"
#define WAITING_TIME 1000000
#define LOADING_TIME 100000

typedef struct QuestionNode
{
    int id;                         // 题目唯一编号
    char question[QLEN + 1];        // 题干
    char option[4][OPTIONLEN + 1];  // 四个选项
    char answer;                    // 存 'A', 'B', 'C', 'D'
    char category[CATEGORYLEN + 1]; // 题目的类别/标签
    struct QuestionNode *next;
} QuestionNode;

typedef struct PaperNode
{
    int id;
    char title[TITLELEN + 1];                 // 试卷标题
    int question_ids[MAX_PAPER_QUESTIONS];    // 存储题目ID
    int question_scores[MAX_PAPER_QUESTIONS]; // 组卷时可修改单题分值
    int total_questions;                      // 总题数
    int paper_score;                          // 试卷总分
    char start_time[TIMELEN + 1];             // 开始考试的时间，时间格式 YYYY/MM/DD HH:MM
    char end_time[TIMELEN + 1];               // 考试结束的时间
    bool published;                           // 是否公布试卷
    struct PaperNode *next;
} PaperNode;

typedef struct ExamRecord
{
    int paper_id;                     // 试卷 id
    int score;                        // 用户的得分
    bool is_finished;                 // 用户是否交卷
    char start_time[TIMELEN + 1];     // 用户的起始做题时间
    char end_time[TIMELEN + 1];       // 用户交卷的时间
    int choices[MAX_PAPER_QUESTIONS]; // 用户的答案，-1 表示未作答
    struct ExamRecord *next;
} ExamRecord;

typedef struct UserNode
{
    int id;                            // 用户唯一标识
    char account[MAX_ACCOUNT_LEN + 1]; // 用户的账号
    char password[MAX_PWD_LEN + 1];    // 用户的密码
    int exercised_question_count;      // 练习题量
    ExamRecord *exam_record_head;      // 用户的考试记录数据链表的头结点
    struct UserNode *next;
} UserNode;

typedef struct Text
{
    char *content;
    int length; // 一个汉字宽为 2，一个字母宽为 1
} Text;

typedef struct Date
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
} Date;

#endif