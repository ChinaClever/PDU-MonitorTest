/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "test_corethread.h"

Test_CoreThread::Test_CoreThread(QObject *parent) : Test_Object(parent)
{

}


void Test_CoreThread::initFunSlot()
{
    mLogs = Test_Logs::bulid(this);
    mErr = Test_ErrRange::bulid(this);
    mRead = Test_DevRead::bulid(this);
    mCtrl = Test_DevCtrl::bulid(this);
    mNetWork = Test_NetWork::bulid(this);
}


bool Test_CoreThread::hubPort()
{
    QString str = tr("设备 LINK 级联口连接");
    bool ret = mRead->readHub();
    if(ret) str += tr("正常");
    else str += tr("错误");

    return mLogs->updatePro(str, ret);
}

bool Test_CoreThread::initDev()
{
    bool ret  = mRead->readSn();
    if(ret) {
        QString str = tr("设备 SER 级联口连接");
        ret = mRead->readDev();
        if(ret) str += tr("成功");
        else str += tr("失败");
        mLogs->updatePro(str, ret);
    }
    if(ret) ret = mRead->readNet();
    if(ret) ret = hubPort();

    return ret;
}

bool Test_CoreThread::volErrRange(int i)
{
    bool ret = true;
    for(int k=0; k<5; ++k) {
        ret = mErr->volErr(i);
        if(ret) break; else mRead->readDev();
    }

    QString str = tr("电压 L%1，期望电压=%2V，实测电压=%3V").arg(i+1)
            .arg(mSour->line.vol.value[i]).arg(mDev->line.vol.value[i]);
    if(ret) str += tr("正常");
    else str += tr("错误");

    return mLogs->updatePro(str, ret);
}

bool Test_CoreThread::curErrRange(int i)
{
    bool ret = true;
    for(int k=0; k<5; ++k) {
        ret = mErr->curErr(i);
        if(ret) break; else mRead->readDev();
    }

    QString str = tr("电流 L%1，期望电流=%2A，实测电流=%3A").arg(i+1)
            .arg(mSour->line.cur.value[i]/COM_RATE_CUR).arg(mDev->line.cur.value[i]/COM_RATE_CUR);
    if(ret) str += tr("正常");
    else {
        if(mDev->line.cur.value[i]) {
            str += tr("错误");
        } else {
            str = tr("电流 L%1，错误，请接上负载，实测电流=0A").arg(i+1);
        }
    }

    return mLogs->updatePro(str, ret);
}

bool Test_CoreThread::powErrRange(int i)
{
    bool ret = true;
    for(int k=0; k<6; ++k) {
        ret = mErr->powErr(i);
        if(ret) break; else mRead->readDev();
    }

    QString str = tr("功率 L%1，期望功率=%2kW，实测功率=%3kW").arg(i+1)
            .arg(mSour->line.pow[i]/COM_RATE_POW).arg(mDev->line.pow[i]/COM_RATE_POW);
    if(ret) str += tr("正常");
    else str += tr("错误");

    return mLogs->updatePro(str, ret);
}

bool Test_CoreThread::envErrRange()
{
    bool ret = mErr->temErr();
    QString str = tr("传感器温度，期望温度=%1，实测温度=%2")
            .arg(mSour->env.tem.value[0]).arg(mDev->env.tem.value[0]);
    if(ret) str += tr("正常");
    else {
        if(mDev->env.tem.value[0]) {
            str += tr("错误");
        } else {
            str = tr("请插入传感器，实测温度=0");
        }
    }

    ret = mLogs->updatePro(str, ret);
    if(ret) {
        str = tr("传感器湿度，期望湿度=%1，实测湿度=%2")
                .arg(mSour->env.hum.value[0]).arg(mDev->env.hum.value[0]);
        ret = mErr->humErr();
        if(ret)  str += tr("正常");
        else str += tr("错误");
        ret = mLogs->updatePro(str, ret);
    }

    return ret;
}

bool Test_CoreThread::oneLineCheck()
{
    bool ret = true;
    if(2 == mDt->lines){
        ret = mErr->oneLineCurErr();
        ushort *value = mDev->line.cur.value;
        QString str = tr("电流 L%1 ，期望电流=%2A，实测电流=%3A").arg(1)
                .arg((value[1]+value[2])/COM_RATE_CUR).arg(value[0]/COM_RATE_CUR);
        if(ret) str += tr("正常"); else str += tr("错误");
        mLogs->updatePro(str, ret);

        ret = mErr->oneLinePowErr();
        value = mDev->line.pow;
        str = tr("功率 L%1 ，期望功率=%2KW，实测功率=%3KW").arg(1)
                .arg((value[1]+value[2])/COM_RATE_POW).arg(value[0]/COM_RATE_POW);
        if(ret) str += tr("正常"); else str += tr("错误");
        mLogs->updatePro(str, ret);

        ret = mErr->oneLineVolErr();
        value = mDev->line.vol.value;
        str = tr("电压 L%1 ，期望电压=%2V，实测电压=%3V").arg(1)
                .arg((value[1]+value[2])/2/COM_RATE_VOL).arg(value[0]/COM_RATE_VOL);
        if(ret) str += tr("正常"); else str += tr("错误");
        mLogs->updatePro(str, ret);
    }

    return ret;
}

bool Test_CoreThread::checkErrRange()
{
    int i = 0;
    bool res = true, ret = true;
    if(2 == mDt->lines) i=1;
    for(; i<mDev->line.size; ++i) {
        ret = volErrRange(i); if(!ret) res = false;
        ret = curErrRange(i); if(!ret) res = false;
        if(ret){ret = powErrRange(i); if(!ret) res = false;}
    }
    if(res) res = oneLineCheck();
    if(res) res = envErrRange();

    return res;
}

bool Test_CoreThread::volAlarmErr(int i)
{
    QString str = tr("电压报警阈值 L%1 ").arg(i+1);
    bool ret = mErr->volAlarm(i);
    if(ret) str += tr("正常");
    else str += tr("错误");

    return mLogs->updatePro(str, ret);
}

bool Test_CoreThread::curAlarmErr(int i)
{
    QString str = tr("电流报警阈值 L%1 ").arg(i+1);
    bool ret = mErr->curAlarm(i);
    if(ret) str += tr("正常");
    else str += tr("错误");

    return mLogs->updatePro(str, ret);
}

bool Test_CoreThread::envAlarmErr()
{
    bool res = true;
    if(SI_PDU == mDt->devType) {
        QString str = tr("温度报警阈值 ");
        bool ret = mErr->temAlarm();
        if(ret) str += tr("正常"); else {str += tr("错误"); res = false;}
        mLogs->updatePro(str, ret);

        str = tr("湿度报警阈值 ");
        ret = mErr->humAlarm();
        if(ret) str += tr("正常"); else {str += tr("错误"); res = false;}
        mLogs->updatePro(str, ret);
    }

    return res;
}

bool Test_CoreThread::checkAlarmErr()
{
    bool res = true, ret = true;
    sCfgDev *cth = &(mItem->cTh);
    if(cth->type) {
        for(int i=0; i<mDev->line.size; ++i) {
            ret = volAlarmErr(i); if(!ret) res = false;
            ret = curAlarmErr(i); if(!ret) res = false;
        }
        if(res) res = envAlarmErr();
    }

    return res;
}

bool Test_CoreThread::curAlarmWrite(int i)
{
    QString str = tr("电流报警阈值 L%1 写入").arg(i+1);
    bool ret = mCtrl->setCurTh(i);
    if(ret) str += tr("正常");
    else str += tr("错误");

    return mLogs->updatePro(str, ret);
}

bool Test_CoreThread::volAlarmWrite(int i)
{
    QString str = tr("电压报警阈值 L%1 写入").arg(i+1);
    bool ret = mCtrl->setVolTh(i);
    if(ret) str += tr("正常");
    else str += tr("错误");

    return mLogs->updatePro(str, ret);
}

bool Test_CoreThread::writeAlarmTh()
{
    bool ret = true;
    sCfgDev *cth = &(mItem->cTh);
    if(cth->type) {
        for(int i=0; i<mDev->line.size; ++i) {
            ret = curAlarmWrite(i); if(!ret) break;
            ret = volAlarmWrite(i); if(!ret) break;
        }
    }

    return ret;
}

bool Test_CoreThread::factorySet()
{
    bool ret = true;
    if(SI_PDU == mDt->devType) {
        ret = mCtrl->factorySet();
        QString str = tr("恢复出厂设置");
        if(ret) str += tr("成功");
        else str += tr("失败");
        mLogs->updatePro(str, ret);
    } else {
        Ctrl_IpRtu::bulid(this)->wait();
    }

    return ret;
}

void Test_CoreThread::workResult(bool)
{
    mLogs->saveLogs();
    mLogs->updatePro(tr("测试结束"));
    mPro->step = Test_Over;
}

void Test_CoreThread::workDown()
{
    mPro->step = Test_Start;
    bool ret = initDev();
    if(ret) {
        ret = checkErrRange();
        if(mItem->cTh.enModify) {
            if(ret) ret = writeAlarmTh();
        } else {
            ret = checkAlarmErr();
        }
        ret = factorySet();
    }
    workResult(ret);
}

void Test_CoreThread::run()
{
    if(isRun) return;
    isRun = true;

    workDown();
    isRun = false;
}