/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "sn_devtype.h"
#include "common/json/json_build.h"
#include "common/json/json_recv.h"

Sn_DevType::Sn_DevType()
{
    sDevData *dev = sDataPacket::bulid()->getDev();
    mDt = &(dev->devType);

    QJsonObject json;
    Json_Build::bulid()->getJson(json);
    Json_Recv::bulid()->recv("PDU-CalibrateTool");
}

Sn_DevType * Sn_DevType::bulid()
{
    static  Sn_DevType* sington = nullptr;
    if(sington == nullptr)
        sington = new  Sn_DevType();
    return sington;
}

QString Sn_DevType::getDevStr(uint id)
{
    mDt->dev_type = Json_Recv::bulid()->getDevTypeByID(id);
    return mDt->dev_type;
}

int Sn_DevType::getDevType(const QString &str)
{
    int ret = 0;
    if(str.contains("ZPDU")) ret = ZPDU;
    if(str.contains("MPDU")) ret = MPDU;
    if(str.contains("RPDU")) ret = RPDU;

    if(str.contains("BM-PDU")) ret = BM_PDU;
    if(str.contains("SI-PDU")) ret = SI_PDU;
    if(str.contains("IP-PDU")) {
        ret = IP_PDU;
        if(str.contains("SNMPV3")) mDt->version = 3;
    }

    return ret;
}

int Sn_DevType::getAcType(const QString &str)
{
    int ret = AC;
    if(str.contains("直流")) ret = DC;

    return ret;
}

int Sn_DevType::getColMode(const QString &str)
{
    int ret = Transformer;
    if(str.contains("锰铜")) ret = Mn;
    if(mDt->devType > RPDU) ret = Transformer;

    return ret;
}

int Sn_DevType::getLineNum(const QString &str)
{
    int ret = 1;
    if(str.contains("三相")) ret = 3;

    return ret;
}

int Sn_DevType::getSerie(const QString &str)
{
    int ret = 0;
    if(str.contains("A系列")) ret = 1;
    if(str.contains("B系列")) ret = 2;
    if(str.contains("C系列")) ret = 3;
    if(str.contains("B/D系列")) ret = 4;

    return ret;
}

bool Sn_DevType::analysDevType(uint id)
{
    bool ret = true;
    QString str = getDevStr(id);
    if(str.size()) {
        mDt->devId = id;
        mDt->devType = getDevType(str);
        mDt->ac = getAcType(str);
        mDt->specs = getColMode(str);
        mDt->series = getSerie(str);
        mDt->lines = getLineNum(str);
    } else {
        ret = false;
    }

    return ret;
}
