SQLTOOL_BEGIN

struct result_set final
{
    SQLTOOL_SEQ<SQLTOOL_MAP<std::string, std::string>> data_set;
    uint64_t row_cnt() {return data_set.size();}
    uint64_t col_cnt() {return data_set[SQLTOOL_AXIS].size();}
};

struct db_info final
{
    std::string user = "",
        password = "",
        data_src = "";
    db_info() {}
    db_info(std::wstring _data_src, std::wstring _user, std::wstring _password) :
        user(SQLTOOL_CHARSET(_user)), password(SQLTOOL_CHARSET(_password)), data_src(SQLTOOL_CHARSET(_data_src)) {}
    db_info(std::string _data_src, std::string _user, std::string _password) :
        user(_user), password(_password), data_src(_data_src) {}
    db_info(db_info &src) : user(src.user), password(src.password), data_src(src.data_src) {}
    db_info(db_info &&src) : user(std::move(src.user)), password(std::move(src.password)), data_src(std::move(src.data_src)) {}
    friend std::ostream &operator<<(std::ostream &output, db_info &src)
    {
        output << "[Data Source][" << src.data_src << ']' <<  std::endl;
        output << "[User][" << src.user << ']' <<  std::endl;
        output << "[Password][" << src.password << ']' <<  std::endl;
        return output;
    }
    bool operator==(db_info &src) {return (user==src.user && password==src.password && data_src==src.data_src);}
    bool operator!=(db_info &src) {return !(*this == src);}
    void operator=(db_info &src) {new (this)db_info(src);}
    void operator=(db_info &&src) {new (this)db_info(std::move(src));}
    ~db_info(){};
};

bool sqltool_startup(db_info &log_info)
{
    auto ret_alloc_env = SQLAllocEnv(&SQLTOOL_H_ENV);
    auto ret_attr_env = SQLSetEnvAttr(SQLTOOL_H_ENV, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
    auto ret_alloc_conn = SQLAllocConnect(SQLTOOL_H_ENV, &SQLTOOL_H_DBC);
    auto ret_conn = SQLConnectW(SQLTOOL_H_DBC, SQLTOOL_WCHAR(SQLTOOL_CHARSET(log_info.data_src).c_str()), SQL_NTS, SQLTOOL_WCHAR(SQLTOOL_CHARSET(log_info.user).c_str()), SQL_NTS, SQLTOOL_WCHAR(SQLTOOL_CHARSET(log_info.password).c_str()), SQL_NTS);
    SQLSetConnectAttr(SQLTOOL_H_DBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
    auto ret_alloc_stmt = SQLAllocStmt(SQLTOOL_H_DBC, &SQLTOOL_H_STMT);
    SQLSetStmtAttr(SQLTOOL_H_STMT, SQL_ATTR_CURSOR_SCROLLABLE, (SQLPOINTER)SQL_SCROLLABLE, SQL_NTS);
    return (ret_alloc_env==SQL_SUCCESS||ret_alloc_env==SQL_SUCCESS_WITH_INFO) &&
        (ret_attr_env==SQL_SUCCESS||ret_attr_env==SQL_SUCCESS_WITH_INFO) &&
        (ret_alloc_conn==SQL_SUCCESS||ret_alloc_conn==SQL_SUCCESS_WITH_INFO) &&
        (ret_conn==SQL_SUCCESS||ret_conn==SQL_SUCCESS_WITH_INFO) &&
        (ret_alloc_stmt==SQL_SUCCESS||ret_alloc_stmt==SQL_SUCCESS_WITH_INFO);
}

result_set sqltool_go(std::string sql_inst, uint64_t buf_len = 128)
{
    result_set rs;
    auto ret_exec = SQLExecDirectW(SQLTOOL_H_STMT, SQLTOOL_WCHAR(SQLTOOL_CHARSET(sql_inst).c_str()), SQL_NTS);
    SQLSMALLINT col_cnt = 0;
    SQLNumResultCols(SQLTOOL_H_STMT, &col_cnt);
    if(ret_exec==SQL_SUCCESS || ret_exec==SQL_SUCCESS_WITH_INFO) if(col_cnt) while (SQLFetch(SQLTOOL_H_STMT)!=SQL_NO_DATA)
    {
        SQLTOOL_MAP<std::string, std::string> row_data(async::capsulate_function<uint64_t, std::string>(bagrt::string_hash));
        SQLULEN col_size = 0;
        SQLSMALLINT dec = 0;
        SQLSMALLINT db_type = 0;
        for(auto i=0; i<col_cnt; ++i)
        {
            auto buf_key = new SQLWCHAR[buf_len];
            SQLSMALLINT key_len = 0;
            SQLDescribeColW(SQLTOOL_H_STMT, i + 1, buf_key, buf_len, &key_len, &db_type, &col_size, &dec, SQL_NO_NULLS);
            std::wstring str_key = buf_key;
            delete [] buf_key;
            buf_key = nullptr;
            auto buf_data = new SQLWCHAR[buf_len];
            SQLLEN value_len = 0;
            SQLGetData(SQLTOOL_H_STMT, i + 1, SQL_C_WCHAR, buf_data, buf_len, &value_len);
            std::wstring str_val = buf_data;
            delete [] buf_data;
            buf_data = nullptr;
            row_data.insert(SQLTOOL_CHARSET(str_key), SQLTOOL_CHARSET(str_val));
        }
        rs.data_set.emplace_back(row_data);
    }
    return rs;
}

result_set sqltool_go(std::wstring sql_inst, uint64_t buf_len = 128) {return sqltool_go(SQLTOOL_CHARSET(sql_inst), buf_len);}

bool sqltool_shutdown()
{
    auto ret_free_stmt = SQLFreeStmt(SQLTOOL_H_STMT, SQL_CLOSE);
    auto ret_disconn = SQLDisconnect(SQLTOOL_H_DBC);
    auto ret_free_conn = SQLFreeConnect(SQLTOOL_H_DBC);
    auto ret_free_env = SQLFreeEnv(SQLTOOL_H_ENV);
    return (ret_free_stmt==SQL_SUCCESS||ret_free_stmt==SQL_SUCCESS_WITH_INFO) &&
        (ret_disconn==SQL_SUCCESS||ret_disconn==SQL_SUCCESS_WITH_INFO) &&
        (ret_free_conn==SQL_SUCCESS||ret_free_conn==SQL_SUCCESS_WITH_INFO) &&
        (ret_free_env==SQL_SUCCESS||ret_free_env==SQL_SUCCESS_WITH_INFO);
}

SQLTOOL_END