#pragma once
#include "singleton.h"

enum LOG_LEVEL
{
    Log,     // ���� �� ������ �α׿� ���
    Warning, // ���� �� ������ �α׿� ��������� ���
    Error,   // ���� �� ������ �α׿� ���������� ���
    Fatal,   // ���� �� ������ �α׿� ���������� ���, ���α׷� ����
};

class COutputLog : public CSingleton<COutputLog>
{
public:
    SINGLE(COutputLog);

private:
    char InputBuf[256];
    ImVector<char*> Items;
    ImVector<const char*> Commands;
    ImVector<char*> History;
    int HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter Filter;
    bool AutoScroll;
    bool ScrollToBottom;

public:
    void ClearLog();
    void AddLog(LOG_LEVEL level, const char* fmt, ...);
    void ExecCommand(const char* command_line);
    int TextEditCallback(ImGuiInputTextCallbackData* data);

private:
    int Stricmp(const char* s1, const char* s2);
    int Strnicmp(const char* s1, const char* s2, int n);
    char* Strdup(const char* s);
    void Strtrim(char* s);

public:
    void init(){};
    void render(bool* open);
};

#define LOG(level, fmt, ...) COutputLog::GetInst()->AddLog(level, fmt, __VA_ARGS__);
