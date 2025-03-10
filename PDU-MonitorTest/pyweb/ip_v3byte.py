from monitor_ip.ip_web import  *
import datetime

class IpV3BYTE(IpWeb):

    def start_fun(self):
        self.ip_prefix = 'https://'
        self.login()
        self.setEle()
        self.checkIpv3ByteCur()
        self.checkCorrect()
        self.setIpDistribution()
        self.clearLogs()
        self.resetFactory()
        #self.reStart()
        self.driver.quit()
        
    def setIpDistribution(self):
        cfg = self.cfgs
        ip = int(cfg['protocol'])
        if(ip == 1):
            try:
                self.divClick(5)
                security = int(cfg['security'])
                if(security): time.sleep(1.2)
                self.setSelect("protocol", ip)
                self.execJs('setdevice(2)')
                self.sendtoMainapp('更改DHCP模式', 1)
            except:
                pass
    
    def setTime(self):
        self.divClick(8)
        self.execJsAlert("check_time()")

    def timeCheck(self):
        self.divClick(8); time.sleep(0.25)
        loctime = self.driver.find_element_by_id('loctime').text
        devtime = self.driver.find_element_by_id('devtime1').text
        if str(devtime[0:13]) in str(loctime):
            t1 = int(devtime[14:15])*60+int(devtime[17:18])
            t2 = int(loctime[14:15])*60+int(loctime[17:18])
            if( abs(t1 - t2) >= 10*60):
                msg = '设备时间错误，本地时间{0}，设备时间{1}'.format(loctime, devtime)
                self.sendtoMainapp(msg, 0)
            else:
                msg = '设备时间正常，设备时间{0}'.format(devtime)
                self.sendtoMainapp(msg, 1)
        else:
            msg = '设备日期错误，本地时间{0}，设备时间{1}'.format(loctime, devtime)
            self.sendtoMainapp(msg, 0)

    def clearLogs(self):
        en = self.cfgs['log_en']
        if(int(en) < 1):
            return
        self.timeCheck()
        self.divClick(9)
        jsSheet = "xmlset = createXmlRequest();xmlset.onreadystatechange = setdata;ajaxgets(xmlset, \"/setlclear?a=\" + {0} + \"&\");"
        for num in range(0, 2):
            self.setSelect("loglist", num)
            self.execJs(jsSheet.format(num))
            time.sleep(0.2)
        self.sendtoMainapp('清除日志', 1)

    def checkCorrect(self):
        cfg = self.cfgs
        security = int(self.cfgs['security'])
        ip =  self.ip_prefix + cfg['ip_addr'] + '/correct.html'
        self.driver.get(ip); time.sleep(1.1)
        if(security): time.sleep(1.2)
        #self.driver.switch_to.default_content()
        self.itemCheck("language", int(cfg['language']), '语言选择')
        self.itemCheck("modbus", cfg['modbus'], '模式选择')
        self.itemCheck("lcdled", cfg['lines'], '相数选择')
        self.itemCheck("ACDC", cfg['ac'], '交直流选择')
        self.itemCheck("standard", cfg['standard'], '中性/标准选择')
        self.itemCheck("Log_flag", cfg['log_en'], '日志功能设置')
        index = 0
        if(int(cfg['cur_max']) == 630 and int(cfg['lines']) != 2):
            index = 1
        self.itemCheck("choiceA", index , '32A/63A选择')
        self.macAddrCheck()
        self.driver.back(); time.sleep(1)
        if(security):time.sleep(1.3)
