#include "sqltool"

using namespace std;
using namespace sqltool;

int main(int argc, char *argv[], char *envp[])
{
    // 初始化登录令牌
    // ID card initialization
    /* 数据源 Data source - DSSW
     * 数据库用户名 Database user name - sa
     * 数据库用户密码 Database user password - 123456
     */
    db_info log_info("DSSW", "sa", "123456");
    cout << log_info << endl;
    
    // 启动数据库环境，使用令牌变量log_info
    // Database environment startup with ID card variable log_info
    SQLTOOL_STARTUP(log_info)
    // 设定缓冲长度为128
    // Set buffer length as 128
    SQLTOOL_BUF_LEN(128)
    // 执行SQL命令并返回结果到res中
    // Execute SQL instruction and return the result to res.
    SQLTOOL_GO("insert into ex1 values ('2011', '259.982208', '43.4160', '108.291', '152.7')")
    auto res = SQLTOOL_GO("select * from ex1")
    // 关闭数据库环境
    // Database environment shutdown
    SQLTOOL_SHUTDOWN
    
    // 打印表头
    // Print columns' name
    cout << "year\t" << "sales\t" << "price\t" << "expenses\t" << "production" << endl;
    for(auto i=0; i<res.row_cnt(); ++i)
        // 使用函数SQLTOOL_CHARSET转换宽窄字符串，打印第i行的对应字段名称的字段值
        // Use function SQLTOOL_CHARSET to transform the wide or ANSI character string, print the field value by coresponding field name at line i
        cout << SQLTOOL_CHARSET(res.data_set[i][L"year"]) << "\t"
            << SQLTOOL_CHARSET(res.data_set[i][L"sales"]) << "\t"
            << SQLTOOL_CHARSET(res.data_set[i][L"price"]) << "\t"
            << SQLTOOL_CHARSET(res.data_set[i][L"expenses"]) << "\t"
            << SQLTOOL_CHARSET(res.data_set[i][L"production"]) << endl;

    return EXIT_SUCCESS;
}
