#ifndef CONFIGBASH
#define CONFIGBASH
#include "cfgcom.h"
#include "serialstatuswid.h"

enum eDevTypes {
    PDU=0,
    ZPDU,
    MPDU,
    RPDU,     // RPDU 执行板
    RPDU_Mc, // RPDU 主控
    SI_PDU,
    IP_PDU,
    BM_PDU,

    AC = 1, // 交流
    DC,     // 直流

    Sum=0,
    Transformer, // 互感器
    Mn    // 锰铜
};

enum eIpTypes {
    IP_PDUV1=1,         // IPV1标准
    IP_PDUV3=3,         // IPV3标准
    IP_PDUV3_C3,        // IP-PDU 俄罗斯C3
    IP_PDUV3_EATON,     // IP-PDU伊顿
    IP_PDUV6,           // IPV6标准
    IP_PDUV6_HUAWEI,     // IPV6华为
    IP_PDUV1_YIXIN,      // IPV1易信
    IP_PDUV1_HUADA,     // IPV1华大    9
    IP_PDUV3_BYTE,    // IPV3字节    10
    IP_PDUV3_SHATE,     // IPV3沙特  11
    IP_PDUV3_ECC,   // IPV3前海     12
    IP_PDUV3_DCL    // IPV3香港DCL  13
};

struct sIpCfg {
    QString addr;
    uchar version;
    uchar language; //
    uchar lines;
    uchar modbus; //  1 表示 Modbus版本
    uchar log;
    uchar standard;
    uchar security;
    uchar lcd;
    uchar protocol;//  0 表示静态协议 1表示DHCP
    uchar ip_bytepassword;//  0 表示字节无默认密码 1表示升点智能需要默认密码
    uchar transformer;
    QString inFirst;
    QString inSecond;
    QString outFirst;
    QString outSecond;
};

struct sSiCfg {
    uchar si_mod;
    bool led;
    uchar ac;
    uchar lines;
};

struct sCfgDev {
    uchar type; // 0 不设置， 1 32安  2 16安， 3 63安  4 自定义
    uchar enModify;
    ushort vol_min;
    ushort vol_max;
    ushort cur_min;
    ushort cur_max;
    ushort loopcur_min;
    ushort loopcur_max;
};


/**
 * RTU传输统计结构体
 */
struct sCount
{
    sCount() {all=ok=err =0;}

    int num;
    int all;
    int ok;
    int err;
};

struct sErrRange
{
    ushort volErr;
    ushort curErr;
    ushort powErr;
};

struct sSerial
{
    sSerial() {source=ser1=ser2=nullptr;}
    SerialPort *source; // 标准源
    SerialPort *ser1; // 串口对象
    SerialPort *ser2; // 串口对象
};

struct sCfgItem
{
    sErrRange err; // 电流误差
    sCfgDev cTh;
    sIpCfg ip;
    sSiCfg si;

    sCount cnt;
    int logCount;

    uchar pcNum;
    QString user; // 客户名称
    QString sw_ver;
    QString hw_ver;
    QString pn;
    sSerial coms;
    ushort currentNum;
    bool eleCheck;
    bool temCheck;
    bool vref;
    bool printer;
    bool macprinter;
    uchar macCheck;
    QString sn;//成品序列号
    int xCodeOffset;
    int yCodeOffset;
    int xMacOffset;
    int yMacOffset;
};


class Cfg
{
    Cfg();
public:
    static Cfg *bulid();

    sCfgItem *item;

    QString getSerialBr(const QString &com);
    QString getSerialName(const QString &key);
    void setSerialBr(const QString &com, const QString &br);
    void setSerialName(const QString &key, const QString &v);

    QString getLoginName();
    void setLoginName(const QString &name);

    int initAddr();
    void initPrint();
    void setPrinter();
    void writeCnt();
    void writeErrData();
    void writeCfgDev();
    void setAddr(int addr);
    void initOffset();
    void writeOffset();

    void setCurrentNum();
    void write(const QString &key, const QVariant& v, const QString &g="Cfg");
    QVariant read(const QString &key, const QVariant &v = QVariant(), const QString &g="Cfg");

protected:
    void initCnt();
    void initErrData();
    void initCfgDev();
    bool getDate();
    void setDate();
    void initCurrentNum();

private:
    CfgCom *mCfg;
};

#endif // CONFIGBASH
