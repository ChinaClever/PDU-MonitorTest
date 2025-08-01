/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "test_corethread.h"
#include "printer_bartender.h"

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
    connect(mNetWork , SIGNAL(sendMACSig(QString)) , this , SLOT(getMacSlot(QString)));
    Printer_BarTender::bulid(this);
//    mSendUdp = new UdpSendSocket(this);
//    mSendUdp->initSocket(47755);
}

void Test_CoreThread::getMacSlot(QString str)
{
    if( str.size() >= 17 ){
        this->mMacStr = str.right(17);
        mPro->macAddress = this->mMacStr;
    }
}

bool Test_CoreThread::hubPort()
{
    QString str = tr("设备 SER 级联口");
    bool ret = mItem->coms.ser1->isOpened();
    if(ret) {
        ret = mRead->readHub();
        if(ret) str += tr("正常");
        else str += tr("错误");
    } else {
        ret = true;
        str += tr("跳过");
    }

    return mLogs->updatePro(str, ret);
}

bool Test_CoreThread::initDev()
{
    mLogs->updatePro(tr("即将开始"));
    bool ret  = false;
    if((IP_PDUV3_SHATE == mItem->ip.version)||(IP_PDUV6 == mItem->ip.version))
        ret = true;
    else{
        ret = mRead->readSn();
        if(ret) {
            mPro->productSN = mDt->sn;
            mPro->productType = mDt->dev_type;
            mPro->clientName.clear();
            mPro->clientName = mItem->user;
            mPro->softwareVersion = mItem->sw_ver;
            ret = mRead->readDev();
            QString str = tr("设备 LINK 级联口连接");
            if(ret) str += tr("成功"); else str += tr("失败");
            mLogs->updatePro(str, ret);
        }
        if(ret) ret = mRead->readNet();
        if(ret) ret = hubPort();
    }

    return ret;
}

bool Test_CoreThread::volErrRange(int i)
{
    bool ret = true;
    for(int k=0; k<5; ++k) {
        ret = mErr->volErr(i);
        if(ret) break; else mRead->readDev();
    }

    int crate = 1;
    if(mDev->devType.screen == 1) crate = 10;
    if(mDev->devType.screen == 3 && mItem->ip.log  == 0 && mItem->ip.security  == 1) crate = 10;
    QString str = tr("电压 L%1，期望电压=%2V，实测电压=%3V").arg(i+1)
            .arg(mSour->line.vol.value[i]).arg(mDev->line.vol.value[i]/(crate*1.0));
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

    int crate = 1;
    if(mDev->devType.screen == 1) crate = 10;
    if(mDev->devType.screen == 3 && mItem->ip.log  == 0 && mItem->ip.security  == 1) crate = 10;
    QString str = tr("电流 L%1，期望电流=%2A，实测电流=%3A").arg(i+1)
            .arg(mSour->line.cur.value[i]/COM_RATE_CUR).arg(mDev->line.cur.value[i]/COM_RATE_CUR/crate);
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


bool Test_CoreThread::eleErrRange(int i)
{
    bool ret = mItem->eleCheck;
    QString str = tr("电能 L%1，实测电能=%2Kwh").arg(i+1).arg(mDev->line.ele[i]/COM_RATE_ELE);
    if(ret) {
        if(1 != mDev->line.ele[i]) {
            str += tr("错误");
            ret = false;
        } else {
            str += tr("正常");
            ret = true;
        }
    } else {
        str += tr("跳过");
        ret = true;
    }

    return mLogs->updatePro(str, ret);
}

bool Test_CoreThread::eleErrRange2(int i)
{
    bool ret = mItem->eleCheck;
    QString str = tr("电能 L%1，实测电能=%2Kwh").arg(i+1).arg(mDev->line.ele[i]/COM_RATE_ELE);
    if(ret) {
        if(2 != mDev->line.ele[i]) {
            str += tr("错误");
            ret = false;
        } else {
            str += tr("正常");
            ret = true;
        }
    } else {
        str += tr("跳过");
        ret = true;
    }

    return mLogs->updatePro(str, ret);
}

bool Test_CoreThread::eleErrRange0(int i)
{
    QString str = tr("电能 L%1，实测电能=%2Kwh").arg(i+1).arg(mDev->line.ele[i]/COM_RATE_ELE);
    bool ret = false;
    if(0 != mDev->line.ele[i]) {
        str += tr("错误");
        ret = false;
    } else {
        str += tr("正常");
        ret = true;
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
    bool ch = mItem->temCheck;
    bool ret = true;
    if(ch){
        ret = mErr->temErr();
        QString str = tr("传感器温度，温度=%1").arg(mDev->env.tem.value[0]);
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
            str = tr("传感器湿度，湿度=%2").arg(mDev->env.hum.value[0]);
            ret = mErr->humErr();
            if(ret) str += tr("正常");
            else str += tr("错误");
            ret = mLogs->updatePro(str, ret);
        }
    }
    return ret;
}

bool Test_CoreThread::oneLineCheck()
{
    bool ret = true;
    if(2 == mDt->lines){
        int crate = 1;
        if(mDev->devType.screen == 1) crate = 10;
        ret = mErr->oneLineCurErr();
        ushort *value = mDev->line.cur.value;
        QString str = tr("电流 L%1 ，期望电流=%2A，实测电流=%3A").arg(1)
                .arg((value[1]+value[2])/COM_RATE_CUR/crate).arg(value[0]/COM_RATE_CUR/crate);
        if(ret) str += tr("正常"); else str += tr("错误");
        mLogs->updatePro(str, ret);

        ret = mErr->oneLinePowErr();
        value = mDev->line.pow;
        if( crate == 10 )
        {
            if(SI_PDU == mDev->devType.devType)
                str = tr("功率 L%1 ，期望功率=%2kW，实测功率=%3kW").arg(1)
                        .arg((value[1]+value[2])/COM_RATE_PF).arg(value[0]/COM_RATE_PF);
            else
                str = tr("功率 L%1 ，期望功率=%2kW，实测功率=%3kW").arg(1)
                        .arg((value[1]+value[2])/COM_RATE_POW).arg(value[0]/COM_RATE_POW);
        }
        else
            str = tr("功率 L%1 ，期望功率=%2kW，实测功率=%3kW").arg(1)
                    .arg((value[1]+value[2])/COM_RATE_POW).arg(value[0]/COM_RATE_POW);
        if(ret) str += tr("正常"); else str += tr("错误");
        mLogs->updatePro(str, ret);

        ret = mErr->oneLineVolErr();
        value = mDev->line.vol.value;
        str = tr("电压 L%1 ，期望电压=%2V，实测电压=%3V").arg(1)
                .arg((value[1]+value[2])/2/COM_RATE_VOL/crate).arg(value[0]/COM_RATE_VOL/crate);
        if(ret) str += tr("正常"); else str += tr("错误");
        mLogs->updatePro(str, ret);
        ret = eleErrRange2(0);
    }

    return ret;
}

bool Test_CoreThread::oneLineECCCheck()
{
    bool ret = true;
    if(2 == mDt->lines){
        int crate = 1;
        if(mDev->devType.screen == 1) crate = 10;
        ret = mErr->oneLineECCCurErr();
        ushort *value = mDev->line.cur.value;
        ushort *value1 = mSour->line.cur.value;
        QString str = tr("电流 L%1 ，期望电流=%2A，实测电流=%3A").arg(1)
                .arg((value1[1]+value1[2])/COM_RATE_CUR/crate).arg(value[0]/COM_RATE_CUR/crate);
        if(ret) str += tr("正常"); else str += tr("错误");
        mLogs->updatePro(str, ret);

        ret = mErr->oneLineECCPowErr();
        value = mDev->line.pow;
        value1 = mSour->line.pow;
        if( crate == 10 )
        {
            if(SI_PDU == mDev->devType.devType)
                str = tr("功率 L%1 ，期望功率=%2kW，实测功率=%3kW").arg(1)
                        .arg((value1[1]+value1[2])/COM_RATE_PF).arg(value[0]/COM_RATE_PF);
            else
                str = tr("功率 L%1 ，期望功率=%2kW，实测功率=%3kW").arg(1)
                        .arg((value1[1]+value1[2])/COM_RATE_POW).arg(value[0]/COM_RATE_POW);
        }
        else
            str = tr("功率 L%1 ，期望功率=%2kW，实测功率=%3kW").arg(1)
                    .arg((value1[1]+value1[2])/COM_RATE_POW).arg(value[0]/COM_RATE_POW);
        if(ret) str += tr("正常"); else str += tr("错误");
        mLogs->updatePro(str, ret);

        ret = mErr->oneLineECCVolErr();
        value = mDev->line.vol.value;
        value1 = mSour->line.vol.value;
        str = tr("电压 L%1 ，期望电压=%2V，实测电压=%3V").arg(1)
                .arg((value1[1]+value1[2])/2/COM_RATE_VOL/crate).arg(value[0]/COM_RATE_VOL/crate);
        if(ret) str += tr("正常"); else str += tr("错误");
        mLogs->updatePro(str, ret);

        ret = eleErrRange2(0);
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
        ret = eleErrRange(i); if(!ret) res = false;
        if(ret){ret = powErrRange(i); if(!ret) res = false;}
    }
    if(IP_PDUV3_ECC == mDt->version){
        if(res) res = oneLineECCCheck();
    }
    else{
        if(res) res = oneLineCheck();
    }
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
    bool ret = true , res = true;
    if(SI_PDU == mDt->devType) {
        ret = mCtrl->eleClean();
        mRead->readDev();
        int i = 0;
        if(2 == mDt->lines) i=1;
        Sleep(2);
        for( ; i<mDev->line.size; ++i) {
            ret = eleErrRange0(i); if(!ret) res = false;
        }
        QString str = tr("清除电能");
        if(res) str += tr("成功"); else str += tr("L%1 失败").arg(i);
        mLogs->updatePro(str, res);

        if(1 != mDev->id) {
            ret = mCtrl->factorySet();
            str = tr("设备通讯地址");
            if(ret) str += tr("成功"); else str += tr("失败");
            mLogs->updatePro(str, ret);
        }
    } else {
        ret = mRead->checkNet(); if(!ret) return ret;
        mLogs->updatePro(tr("启动Web，开始网页检查"));
        Ctrl_IpRtu::bulid(this)->start();
        Ctrl_IpRtu::bulid(this)->wait();
        mLogs->updatePro(tr("退出Web，网页检查结束"));
    }

    return ret;
}

void Test_CoreThread::workResult(bool)
{
    bool res = false;
    mLogs->saveLogs();
    mLogs->updatePro(tr("测试结束"));
    QString str = tr("最终结果 ");
    if(mPro->result != Test_Fail) {
        str += tr("通过");
        res = true;
        mPro->uploadPassResult = 1;
    } else {
        res = false;
        str += tr("失败");
        mPro->uploadPassResult = 0;
    }
    mPacket->updatePro(str, res);

//    sleep(2);
//    Json_Pack::bulid()->http_post("testdata/add","192.168.1.12");//全流程才发送记录(http)

    mPro->step = Test_Over;
}

void Test_CoreThread::workDown()
{
    mPro->step = Test_Start;
    bool ret = false;
    ret = initDev();
    if(ret) {
        if((IP_PDUV3_SHATE != mItem->ip.version)&&(IP_PDUV6 != mItem->ip.version)){
            ret = checkErrRange();
            if(mItem->cTh.enModify) {
                if(ret) ret = writeAlarmTh();
            } else {
                ret = checkAlarmErr();
            }
        }
        if(ret) ret = factorySet();
        if(ret){
            if(mItem->macprinter){
                if( !this->mMacStr.isEmpty() && (IP_PDU == mDt->devType || IP_PDUV3_SHATE == mItem->ip.version)){
                    ret = Printer_BarTender::bulid()->printMAC(this->mMacStr, mItem->xCodeOffset , mItem->yCodeOffset, mItem->xMacOffset , mItem->yMacOffset);
                    if(!ret) ret = Printer_BarTender::bulid()->printMAC(this->mMacStr, mItem->xCodeOffset , mItem->yCodeOffset, mItem->xMacOffset , mItem->yMacOffset);
                    if(ret) mLogs->updatePro(tr("MAC标签打印成功"), ret);
                    else mLogs->updatePro(tr("MAC标签打印失败"), ret);
                }
            }
            if(mItem->printer){
                sBarTend it;
                it.fw = mItem->sw_ver;it.hw = mItem->hw_ver;it.pn = mItem->pn;

                ret = Printer_BarTender::bulid()->printerInfo(it);
                if(!ret) ret = Printer_BarTender::bulid()->printerInfo(it);
                if(ret) mLogs->updatePro(tr("标签打印成功"), ret);
                else mLogs->updatePro(tr("标签打印失败"), ret);
            }
        }else mLogs->updatePro(tr("因测试未通过，标签未打印"), ret);
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
