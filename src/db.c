/* 
 * File:   db.c
 * Author: jk
 *
 * Created on June 16, 2015, 9:10 AM
 */

#include <mysql/mysql.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "db.h"
#include "log.h"
#include "macro.h"

static MYSQL *conn;

int db_initial()
{
    conn = mysql_init(NULL);
    if(!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PWD, DB_NAME, DB_PORT, NULL, 0))
    {
        LOG_ERROR("can't connect database: %s", DB_NAME);
        return -1;
    }
    return 0;
}

int db_destruct()
{
    mysql_close(conn);
    return 0;
}

//check whether the given table exists
//return 1 when exists, else 0
int db_isTableCreated(const char* imeiName)
{
    MYSQL_RES *res;

    char reg[IMEI_LENGTH *2 + 5] = "gps_";
    strncat(reg, imeiName, IMEI_LENGTH * 2);
    if((res = mysql_list_tables(conn, reg)) == NULL)
    {
        LOG_ERROR("can't judge whether tables of IMEI:%s exist", imeiName);
    }

    int rows = mysql_num_rows(res);

    mysql_free_result(res);

    if(0 == rows)
    {
        return 0;
    }
    return 1;
}

int db_createGPS(const char* tableName) //okay
{
    char query[MAX_QUERY];
    //create table gps_IMEI(timestamp INT, lat INT, lon INT, speed TINYINT, course SMALLINT)
    sprintf(query, "create table %s(timestamp INT,lat INT,lon INT,speed TINYINT,course SMALLINT,primary key(timestamp))", tableName);
    if(mysql_query(conn, query))
    {
        LOG_ERROR("can't create table: %s", tableName);
        return -1;
    }
    return 0;
}

int db_createCGI(const char* tableName)
{
    char query[MAX_QUERY];
    //create table cgi_IMEI(timestamp INT, mcc SMALLINT, mnc SMALLINT, lac SMALLINT, ci CHAR(3))
    sprintf(query, "create table %s(timestamp INT,mcc SMALLINT,mnc SMALLINT,lac SMALLINT,ci CHAR(3),primary key(timestamp))", tableName);
    if(mysql_query(conn, query))
    {
        LOG_ERROR("can't create table: %s", tableName);
        return -1;
    }
    return 0;
}

int db_saveGPS(const char *imeiName, int timestamp, int lat, int lon, char speed, short course)
{
    //timestamp INT, lat INT, lon INT, speed TINYINT, course SMALLINT
    char query[MAX_QUERY];
    sprintf(query, "insert into gps_%s(timestamp,lat,lon,speed,course) values(%d,%d,%d,%d,%d)",imeiName, timestamp, lat, lon, speed, course);
    if(mysql_query(conn, query))
    {
        LOG_ERROR("can't insert into gps_%s", imeiName);
        return -1;
    }

    return 0;
}

int db_saveCGI(const char *imeiName, int timestamp, short mcc, short mnc, short lac, char ci[])
{

    char tmp_ci[4];
    memcpy(tmp_ci, ci, 3); // the length os ci is 3...
    tmp_ci[3] = '\0';

    //timestamp INT, mcc SMALLINT, mnc SMALLINT, lac SMALLINT, ci CHAR(3)
    char query[MAX_QUERY];
    sprintf(query, "insert into cgi_%s(timestamp,mcc,mnc,lac,ci) values(%d,%d,%d,%d,\"%s\")", imeiName, timestamp, mcc, mnc, lac, tmp_ci);
    if(mysql_query(conn, query))
    {
        LOG_ERROR("can't insert into cgi_%s", imeiName);
        return -1;
    }

    return 0;
}
