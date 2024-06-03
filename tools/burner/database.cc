/**
 * @file database.cc
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief SQLite3 Database interface
 * @version 0.1
 * @date 2024-04-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <sqlite3.h>
#include "instance.hpp"
#include <flame/interface/log.hpp>

namespace flame_rt {

    static sqlite3* db = nullptr;

    /**
     * @brief create database table
     * 
     */
    void db_init(){
        const char* db_schema = "CREATE TABLE Config (ID INT, Name TEXT);";
        if(sqlite3_exec(db, db_schema, 0, 0, 0)!=SQLITE_OK){
            console::error("table creattion error : {}", sqlite3_errmsg(db));
        }
    }

    /**
     * @brief open database
     * 
     * @return bool true if success, else return false
     */
    bool db_open(){
        if(sqlite3_open(":memory:", &db)){
            console::error("cannot open database : {}", sqlite3_errmsg(db));
            return false;
        }
        return true;
    }

    /* close database */
    void db_close(){
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

}