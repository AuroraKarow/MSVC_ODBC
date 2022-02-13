# MSVC_ODBC
ODBC C++
这是一个ODBC C++项目。使用C++去操作SQL Server数据库。<br>This is a ODBC C++ project. Use C++ to operate SQL Server database.<br>
关于SQL Server配置，参见pdf文档C++ODBC SQL Server Database Configuration进行数据配置。<br>Use the pdf file C++ODBC SQL Server Database Configuration to config <br>

## 警告 Warning
命令`SQLTOOL_STARTUP`必须先于`SQLTOOL_SHUTDOWN`且成对出现，不可重复。<br>
Command `SQLTOOL_STARTUP` should be front of command `SQLTOOL_SHUTDOWN`, unduplicable.<br>
<br>
命令`SQLTOOL_BUF_LEN`必须先于`SQLTOOL_GO`且两者必须位于命令`SQLTOOL_STARTUP`和`SQLTOOL_SHUTDOWN`之间，可重复。<br>
Command `SQLTOOL_BUF_LEN` should be front of command `SQLTOOL_GO` and between command `SQLTOOL_STARTUP` and `SQLTOOL_SHUTDOWN`, duplicable.
