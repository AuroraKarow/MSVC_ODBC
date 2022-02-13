# MSVC_ODBC
ODBC C++
这是一个ODBC C++项目。使用C++去操作SQL Server数据库<br>This is a ODBC C++ project. Use C++ to operate SQL Server database.

## 警告 Warning
*命令`SQLTOOL_STARTUP`必须必须先于`SQLTOOL_SHUTDOWN`且成对出现，不可重复。
*命令`SQLTOOL_BUF_LEN`必须先于`SQLTOOL_GO`且两者必须位于命令`SQLTOOL_STARTUP` `SQLTOOL_SHUTDOWN`之间，可重复。
