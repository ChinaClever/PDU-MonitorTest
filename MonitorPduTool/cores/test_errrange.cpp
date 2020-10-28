/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "test_errrange.h"

Test_ErrRange::Test_ErrRange(QObject *parent) : Test_Object(parent)
{

}

void Test_ErrRange::initFunSlot()
{
    mSourceDev = mPacket->getDev(0);
}

Test_ErrRange *Test_ErrRange::bulid(QObject *parent)
{
    static Test_ErrRange* sington = nullptr;
    if(sington == nullptr)
        sington = new Test_ErrRange(parent);
    return sington;
}

bool Test_ErrRange::volErr(int id)
{
    int pass = Test_Fail;
    int err = mItem->err.volErr;
    int value = mDev->line.vol.value[id];
    int exValue = mSourceDev->line.vol.value[id];
    bool ret = checkErrRange(exValue, value, err);
    if(ret) pass = Test_Pass;
    mDev->line.vol.status[id] = pass;

    return ret;
}

bool Test_ErrRange::curErr(int id)
{
    int pass = Test_Fail;
    int err = mItem->err.curErr;
    int value = mDev->line.cur.value[id];
    int exValue = mSourceDev->line.cur.value[id];
    bool ret = checkErrRange(exValue, value, err);
    if(ret) pass = Test_Pass;
    mDev->line.cur.status[id] = pass;

    return ret;
}

bool Test_ErrRange::powErr(int id)
{
    int pass = Test_Fail;
    int value = mDev->line.pow[id];
    int exValue = mSourceDev->line.pow[id];
    int err = exValue * mItem->err.powErr/1000.0;

    bool ret = checkErrRange(exValue, value, err);
    if(ret) pass = Test_Pass;
    mDev->line.powStatus[id] = pass;

    return ret;
}

bool Test_ErrRange::temErr()
{
    int pass = Test_Fail;
    int exValue = mSourceDev->env.tem.value[0];
    int value = mDev->env.tem.value[0];
    bool ret = checkErrRange(exValue, value, 5);
    if(ret) pass = Test_Pass;
    mDev->env.tem.status[0] = pass;

    return ret;
}

bool Test_ErrRange::humErr()
{
    int pass = Test_Fail;
    int exValue = mSourceDev->env.hum.value[0];
    int value = mDev->env.hum.value[0];
    bool ret = checkErrRange(exValue, value, 5);
    if(ret) pass = Test_Pass;
    mDev->env.hum.status[0] = pass;

    return ret;
}

bool Test_ErrRange::checkErrRange(int exValue, int value, int err)
{
    bool ret = false;
    if(qAbs(exValue-value) <= err) {
        ret = true;
    }

    return ret;
}

bool Test_ErrRange::volAlarm(int id)
{
    sCfgTh *cth = &(mItem->cTh);
    sDataUnit *unit = &(mDev->line.vol);

    bool ret = true;
    if(unit->min[id] != cth->vol_min) ret = false;
    if(unit->max[id] != cth->vol_max) ret = false;

    return ret;
}

bool Test_ErrRange::curAlarm(int id)
{
    sCfgTh *cth = &(mItem->cTh);
    sDataUnit *unit = &(mDev->line.cur);

    bool ret = true;
    if(unit->min[id] != cth->cur_min) ret = false;
    if(unit->max[id] != cth->cur_max) ret = false;

    return ret;
}