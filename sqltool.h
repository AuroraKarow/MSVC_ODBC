SQLTOOL_BEGIN

struct result_set final
{
    SQLTOOL_SEQ<SQLTOOL_MAP<std::string, std::string>> data_set;
    uint64_t row_cnt();
    uint64_t col_cnt();
};

struct db_info final
{
    std::string user = "", password = "", data_src = "";
    db_info();
    db_info(std::wstring _data_src, std::wstring _user, std::wstring _password);
    db_info(std::string _data_src, std::string _user, std::string _password);
    db_info(db_info &src);
    db_info(db_info &&src);
    bool operator==(db_info &src);
    bool operator!=(db_info &src);
    void operator=(db_info &src);
    void operator=(db_info &&src);
    friend std::ostream &operator<<(std::ostream &output, db_info &src)
    {
        output << "[Data Source][" << src.data_src << ']' <<  std::endl;
        output << "[User][" << src.user << ']' <<  std::endl;
        output << "[Password][" << src.password << ']' <<  std::endl;
        return output;
    }
};

bool sqltool_startup(db_info &log_info);
bool sqltool_shutdown();

result_set sqltool_go(std::string sql_inst, uint64_t buf_len = 128);
result_set sqltool_go(std::wstring sql_inst, uint64_t buf_len);

SQLTOOL_END

#include "sqltool.hpp"