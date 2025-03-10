/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_setdlg.h"
#include "ui_home_setdlg.h"

Home_SetDlg::Home_SetDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Home_SetDlg)
{
    ui->setupUi(this);
    groupBox_background_icon(this);
    mItem = Cfg::bulid()->item;
    QTimer::singleShot(500,this,SLOT(initFunSlot()));
}

Home_SetDlg::~Home_SetDlg()
{
    delete ui;
}

void Home_SetDlg::initFunSlot()
{
    initCntLab();
    initThresholdWid();
    int v = mItem->ip.version-1; if( v ) v = v - 1;
    ui->ipTypeBox->setCurrentIndex(v);
    ui->userEdit->setText(mItem->user);
    ui->securityBox->setCurrentIndex(mItem->ip.security);
    ui->languageBox->setCurrentIndex(mItem->ip.language);
    ui->lineBox->setCurrentIndex(mItem->ip.lines);
    ui->ipModeBox->setCurrentIndex(mItem->ip.modbus);
    ui->sBox->setCurrentIndex(mItem->ip.standard);
    ui->logBox->setCurrentIndex(mItem->ip.log);
    ui->PasswordTypeBox->setCurrentIndex(mItem->ip.ip_bytepassword);
    on_ipTypeBox_currentIndexChanged(v);
    ui->lcdBox->setCurrentIndex(mItem->ip.lcd);
    ui->protocolBox->setCurrentIndex(mItem->ip.protocol);
    ui->TransformerBox->setCurrentIndex(mItem->ip.transformer);

    ui->hwEdit->setText(mItem->hw_ver);

    ui->InFirst->setText(mItem->ip.inFirst);
    ui->InSecond->setText(mItem->ip.inSecond);
    ui->OutFirst->setText(mItem->ip.outFirst);
    ui->OutSecond->setText(mItem->ip.outSecond);
}


void Home_SetDlg::initThresholdWid()
{
    sCfgDev *cth = &(mItem->cTh);
    ui->comboBox->setCurrentIndex(cth->type);
    on_comboBox_currentIndexChanged(cth->type);
    ui->modifyCheck->setChecked(cth->enModify);
    ui->modifyCheck->setHidden(true);
}

bool Home_SetDlg::getThresholdWid()
{
    sCfgDev *cth = &(mItem->cTh);
    cth->type = ui->comboBox->currentIndex();
    cth->vol_min = ui->volMinSpin->value();
    cth->vol_max = ui->volMaxSpin->value();
    cth->cur_min = ui->curMinSpin->value()*10;
    cth->cur_max = ui->curMaxSpin->value()*10;
    cth->loopcur_min = ui->loopCurMinSpin->value()*10;
    cth->loopcur_max = ui->loopCurMaxSpin->value()*10;

    mItem->user = ui->userEdit->text();
    mItem->cnt.num = ui->numSpin->value();
    cth->enModify = ui->modifyCheck->isChecked()?1:0;

    mItem->si.led = ui->ledCheck->isChecked();
    int rtn = ui->siLineBox->currentIndex();
    if(rtn) {
        mItem->si.ac = AC;
        mItem->si.lines = rtn;
    } else {
        mItem->si.ac = DC;
        mItem->si.lines = 1;
    }

    mItem->ip.security = ui->securityBox->currentIndex();
    int v = ui->ipTypeBox->currentIndex()+1;
    if(v > 1) v = v + 1; mItem->ip.version = v;
    mItem->ip.language = ui->languageBox->currentIndex();
    mItem->ip.lines = ui->lineBox->currentIndex();
    mItem->ip.modbus = ui->ipModeBox->currentIndex();
    mItem->ip.standard = ui->sBox->currentIndex();
    mItem->ip.log = ui->logBox->currentIndex();
    mItem->sw_ver = ui->verEdit->text();
    mItem->hw_ver = ui->hwEdit->text();
    mItem->ip.inFirst = ui->InFirst->text();
    mItem->ip.inSecond = ui->InSecond->text();
    mItem->ip.outFirst = ui->OutFirst->text();
    mItem->ip.outSecond = ui->OutSecond->text();
    mItem->ip.lcd = ui->lcdBox->currentIndex();
    mItem->ip.protocol = ui->protocolBox->currentIndex();
    mItem->ip.ip_bytepassword = ui->PasswordTypeBox->currentIndex();
    mItem->ip.transformer = ui->TransformerBox->currentIndex();

    return true;
}


void Home_SetDlg::setThresholdWid()
{
    sCfgDev *cth = &(mItem->cTh);
    ui->volMinSpin->setValue(cth->vol_min);
    ui->volMaxSpin->setValue(cth->vol_max);
    ui->curMinSpin->setValue(cth->cur_min/10.0);
    ui->curMaxSpin->setValue(cth->cur_max/10.0);
    ui->loopCurMinSpin->setValue(cth->loopcur_min/10.0);
    ui->loopCurMaxSpin->setValue(cth->loopcur_max/10.0);
}

void Home_SetDlg::on_comboBox_currentIndexChanged(int index)
{
    bool en = true;
    sCfgDev *cth = &(mItem->cTh);
    cth->type = index;
    if(index) {
        cth->vol_min = 90;
        cth->vol_max = 264;
        cth->cur_min = 0;
        switch (index) {
        case 1:  cth->cur_max = 320; break;
        case 2:  cth->cur_max = 160; break;
        case 3:  cth->cur_max = 630; break;
        }
    } else {
        en = false;
    }

    setThresholdWid();
    ui->volMinSpin->setEnabled(en);
    ui->volMaxSpin->setEnabled(en);
    ui->curMinSpin->setEnabled(en);
    ui->curMaxSpin->setEnabled(en);
}

void Home_SetDlg::on_okBtn_clicked()
{
    bool ret = getThresholdWid();
    if(ret) {
        this->close();
        Cfg::bulid()->writeCfgDev();
    }
}

void Home_SetDlg::initCntLab()
{
    ui->verEdit->setText(mItem->sw_ver);
    ui->numSpin->setValue(mItem->cnt.num);
    QString str = tr("设备数量：%1 成功：%2 失败：%3 ")
            .arg(mItem->cnt.all).arg(mItem->cnt.ok).arg(mItem->cnt.err);
    ui->cntLab->setText(str);
}

void Home_SetDlg::on_resBtn_clicked()
{
    bool ret = MsgBox::question(this, tr("请确认，重新计数？"));
    if(ret) {
        mItem->cnt.all = 0;
        mItem->cnt.ok = 0;
        mItem->cnt.err = 0;

        initCntLab();
        Cfg::bulid()->writeCnt();
    }
}

void Home_SetDlg::on_ipTypeBox_currentIndexChanged(int index)
{
    bool res = true;
    if(index == IP_PDUV3_BYTE - 2){
        ui->label_26->show();
        ui->PasswordTypeBox->show();
    }else{
        ui->label_26->hide();
        ui->PasswordTypeBox->hide();
    }
    if( index == IP_PDUV3_BYTE - 2 || index == IP_PDUV3_SHATE - 2
            || index == IP_PDUV3_ECC - 2 || index == IP_PDUV3_DCL - 2) index = 1;
    if(index <= IP_PDUV3_EATON - 2 || index == IP_PDUV1_HUADA - 2)  res = false;
    ui->label_9->setHidden(res);
    ui->lcdBox->setHidden(res);

    res = false;
    if(index >= IP_PDUV3_C3 - 2)  res = true;
    ui->label_8->setHidden(res);
    ui->sBox->setHidden(res);
    ui->label_3->setHidden(res);
    ui->ipModeBox->setHidden(res);
    ui->label_11->setHidden(res);
    ui->logBox->setHidden(res);

//    res = false;
//    if(index == IP_PDUV6 - 2)  res = true;
//    ui->label_7->setHidden(res);
//    ui->lineBox->setHidden(res);

    res = true;
    ui->label_14->setHidden(res);
    ui->InFirst->setHidden(res);
    ui->InSecond->setHidden(res);
    ui->label_16->setHidden(res);
    ui->OutFirst->setHidden(res);
    ui->OutSecond->setHidden(res);
    if(index == IP_PDUV3 - 2 )  res = false;

    ui->sBox->setHidden(res);
    ui->logBox->setHidden(res);
    ui->label_8->setHidden(res);
    ui->label_11->setHidden(res);


    if(index == IP_PDUV3_EATON - 2){
        res = false;
        ui->label_14 ->setHidden(res);
        ui->InFirst->setHidden(res);
        ui->InSecond->setHidden(res);
        ui->label_16 ->setHidden(res);
        ui->OutFirst->setHidden(res);
        ui->OutSecond->setHidden(res);

        ui->label_8->setHidden(res);
        ui->sBox->setHidden(res);
        ui->label_3->setHidden(res);
        ui->ipModeBox->setHidden(res);
        ui->label_11->setHidden(res);
        ui->logBox->setHidden(res);
    }

    if( index == IP_PDUV1_HUADA - 2){//IP_PDUV1_HUADA
        res = false;
        ui->label_8->setHidden(res);
        ui->sBox->setHidden(res);
        ui->label_11->setHidden(res);
        ui->logBox->setHidden(res);
        ui->label_20->setText(tr("屏类型"));
        ui->securityBox->setItemText(0 , tr("液晶屏"));
        ui->securityBox->setItemText(1 , tr("段码屏"));
        ui->label_11->setText(tr("电压显示"));
        ui->logBox->setItemText(0 , tr("显示0.1"));
        ui->logBox->setItemText(1 , tr("整数"));
        ui->label_3->setHidden(res);
        ui->ipModeBox->setHidden(res);//电流小数 1 2； 屏类型 段码屏 和 液晶屏
        ui->label_20->setHidden(res);
        ui->securityBox->setHidden(res);
    }else{
        ui->label_20->setText(tr("安全版本"));
        ui->securityBox->setItemText(0 , tr("标准"));
        ui->securityBox->setItemText(1 , tr("加密"));
        ui->label_11->setText(tr("日志功能"));
        ui->logBox->setItemText(0 , tr("无日志"));
        ui->logBox->setItemText(1 , tr("带日志"));
    }
}

void Home_SetDlg::on_userEdit_textChanged(const QString &arg1)
{
    ui->userEdit->setClearButtonEnabled(1);
}

void Home_SetDlg::on_verEdit_textChanged(const QString &arg1)
{
    ui->verEdit->setClearButtonEnabled(1);
}

void Home_SetDlg::on_hwEdit_textChanged(const QString &arg1)
{
    ui->hwEdit->setClearButtonEnabled(1);
}

