#pragma once
#include <iostream>
#include "sqltool"

using namespace std;
using namespace sqltool;

int main(int argc, char *argv[], char *envp[])
{
    db_info log_info("DSSW", "sa", "123456");
    cout << log_info << endl;

    SQLTOOL_STARTUP(log_info)
    SQLTOOL_BUF_LEN(128)
    auto res = SQLTOOL_GO("select * from ex1")
    SQLTOOL_SHUTDOWN

    cout << "year\t" << "sales\t" << "price\t" << "expenses\t" << "production" << endl;
    for(auto i=0; i<res.row_cnt(); ++i)
        cout << res.data_set[i]["year"] << "\t"
            << res.data_set[i]["sales"] << "\t"
            << res.data_set[i]["price"] << "\t"
            << res.data_set[i]["expenses"] << "\t"
            << res.data_set[i]["production"] << endl;
    return EXIT_SUCCESS;
}