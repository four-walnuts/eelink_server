/* 
 * File:   db.h
 * Author: jk
 *
 * Created on June 16, 2015, 9:10 AM
 */

#ifndef DB_H
#define	DB_H

/* database settings */
#define DB_HOST "localhost"
#define DB_USER "eelink"
#define DB_PWD  "eelink"
#define DB_PORT (3306)
#define DB_NAME "gps"

int db_initial();
int db_destruct();

int db_isTableCreated(char* tableName);
int db_create(char* tableName);
int db_saveGPS(int timestamp, int lat, int lng, char speed, short course);
int db_saveCGI(int timestamp, short mcc, short mnc, short lac, int ci);

#endif	/* DB_H */

