/*
 * msg_proc_tianqin.c
 *
 *  Created on: 2015年6月29日
 *      Author: jk
 */


#include <string.h>
#include <netinet/in.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

#include "msg_proc_app.h"
#include "msg_proc_tianqin.h"
#include "log.h"
#include "session.h"
#include "object.h"
#include "mqtt.h"
#include "msg_tianqin.h"
#include "db.h"

static int tianqin_login(const MSG_V1 *msg, SESSION *session);
static int tianqin_gps(const MSG_V1 *msg, SESSION *session);
static int tianqin_alarm(const MSG_V1 *msg, SESSION *session);
static int tianqin_isAlarm(const MSG_V1 *msg);
static int tianqin_isGPS(const MSG_V1 *msg);

int handle_tianqin_msg(const char* m, size_t msgLen, void* arg)
{
    MSG_V1* msg = (MSG_V1*)m;

    if (msgLen < sizeof(MSG_V1))
    {
        LOG_ERROR("message length not enough: %zu(at least(%zu)", msgLen, sizeof(MSG_HEADER));

        return -1;
    }

    if (msg->signature != START_FLAG)
    {
        LOG_ERROR("message head signature error:%d", msg->signature);
        return -1;
    }

    if(!tianqin_isAlarm(msg))    //TODO: is alarm
    {
        tianqin_gps(msg, arg);
    }
    else
    {
        tianqin_alarm(msg, arg);
    }
}


static int simcom_msg_send(void* msg, size_t len, SESSION* session)
{
    /*
    if (!session)
    {
        return -1;
    }

    SIMCOM_MSG_SEND pfn = session->pSendMsg;
    if (!pfn)
    {
        LOG_ERROR("device offline");
        return -1;
    }

    pfn(session->bev, msg, len);

    LOG_DEBUG("send msg(cmd=%d), length(%ld)", get_msg_cmd(msg), len);
    LOG_HEX(msg, len);

    free(msg);

    return 0;
    */
}

static int tianqin_login(const MSG_V1 *msg, SESSION* session)
{
    const MSG_V1* req = msg;
    const char *ei = get_EI_STRING(msg->EI);

    LOG_DEBUG("tianqin EI(%s) login", ei);

    OBJECT *obj = obj_get(ei);
    if(!obj)
    {
        LOG_INFO("the first time of tianqin EI(%s)'s login", ei);
        obj = obj_new();

        memcpy(obj->IMEI, ei, IMEI_LENGTH);
        memcpy(obj->DID, ei, IMEI_LENGTH);

        obj_add(obj);
        mqtt_subscribe(obj->IMEI);
    }
    session->obj = obj;
    session_add(session);

    if (!db_isTableCreated(obj->IMEI))
    {
        db_createCGI(obj->IMEI);
        db_createGPS(obj->IMEI);
    }

    return 0;
}

static int get_lat(const char *lat)
{
    int ret = 0;
    int i = (lat[0]>>4&0x0f) * 10 + (lat[0] & 0x0f);
    int j = (lat[1]>>4&0x0f) * 10 + (lat[1] & 0x0f);
    j = j * 100 + (lat[2]>>4&0x0f) * 10 + (lat[2] & 0x0f);
    j = j * 100 + (lat[3]>>4&0x0f) * 10 + (lat[3] & 0x0f);
    ret = (i * 600000 + j) * 3;
    return ret;
}

static int get_lon(const char *lon)
{
    int ret = 0;
    int i = (lon[0]>>4&0x0f) * 100 + (lon[0] & 0x0f) * 10 + (lon[1]>>4&0x0f);
    int j = (lon[1]&0x0f) * 100 + (lon[2]>>4&0x0f) * 10 + (lon[2]&0x0f);
    j = j * 100 + (lon[3]>>4&0x0f) * 10 + (lon[3]&0x0f);
    j = j * 10 + (lon[4]>>4&0x0f);
    ret = (i * 600000 + j) * 3;
    return ret;
}

static int tianqin_gps(const MSG_V1* msg, SESSION* session)
{
    const char *ei = get_EI_STRING(msg->EI);    //TODO: strdump
    if(session_get(ei) == NULL)
    {
        tianqin_login(msg, session);
    }

    int lat = get_lat(msg->lat);
    int lon = get_lon(msg->lon);
    static const int transGPS = 1800000;

    LOG_INFO("GPS: lat(%f), lng(%f)", (double)lat/transGPS, (double)lon/transGPS);

    OBJECT * obj = session->obj;
    
    if(obj->lon != 0)   //TODO: gps wuxiao
    {
        LOG_INFO("no gps,only send to app");
        time_t rawtime;
        time ( &rawtime );
        obj->timestamp = rawtime;
        obj->isGPSlocated = 0;
        app_sendGpsMsg2App(session);
        return 0;
    }

    if (obj->lat == lat
        && obj->lon == lon)
    {
        LOG_INFO("No need to save data to leancloud");
        app_sendGpsMsg2App(session);
        return 0;
    }

    //update local object
    obj->lat = lat;
    obj->lon = lon;

    app_sendGpsMsg2App(session);

    //yeelink_saveGPS(obj, session);

    if (tianqin_isGPS(msg)) //TODO: wuxiao
    {
        //int db_saveGPS(const char *name, int timestamp, int lat, int lon, char speed, short course)
        db_saveGPS(obj->IMEI, obj->timestamp, obj->lat, obj->lon, obj->speed, obj->course);
    }

    return 0;
}

static int tianqin_alarm(const MSG_V1 *msg, SESSION *session)
{
    return 0;
}

static int tianqin_isAlarm(const MSG_V1 *msg)
{
    //TODO:
    return 1;
}

static int tianqin_isGPS(const MSG_V1 *msg)
{
    //TODO:
    return 1;
}
