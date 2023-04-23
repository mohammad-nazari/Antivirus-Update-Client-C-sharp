#pragma  once
#include <windows.h>
#include <vector>
#include <string>

typedef	struct _TableColumn{
    std::wstring name;
    std::wstring type;
    std::wstring defaultValue;
    BOOL isPrimaryKey;
    BOOL isAutoIncrement;
}TableColumn;

typedef struct _Table
{
    std::wstring TableName;
    std::wstring TablePath;
    std::wstring ItemName;
    std::wstring RootName;
    std::wstring Version;
    std::vector<TableColumn> columns;
}Table;

typedef struct _Record{
    std::wstring itemName;
    std::wstring rootName;
    std::wstring version;
    std::wstring dateModified;
    std::wstring createdDate;
    bool isDeleted;
    std::vector<std::pair<std::wstring,std::wstring>> data;
}Record;
