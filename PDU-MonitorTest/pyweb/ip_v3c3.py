from monitor_ip.ip_web import  *

class IpV3C3(IpWeb):

    def start_fun(self):
        self.login()
        self.setEle()
        self.checkCorrect()
        self.clearLogs()
        self.resetFactory()
        self.driver.quit()

    def setTime(self):
        self.divClick(8)
        self.execJsAlert("check_time()")

    def timeCheck(self):
        self.divClick(8); time.sleep(0.25)
        nowTime = self.driver.find_element_by_id('loctime').text.split( )
        devTime = self.driver.find_element_by_id('devtime1').text.split( )
        if( nowTime[0] == devTime[0]):
            h1 , m1 , s1 = nowTime[1].split(':')
            t1 = int(h1)*3600 + int(m1)*60 + int(s1)
            h2 , m2 , s2 = devTime[1].split(':')
            t2 = int(h2)*3600 + int(m2)*60 + int(s2)
            if( abs( t1-t2 ) >= 10*60 ):
                self.sendtoMainapp("检查时间时分秒失败",0)
                return False
            else:
                #print(abs(t1-t2))
                self.sendtoMainapp("检查时间成功",1)
                return True
        else:
            self.sendtoMainapp("检查时间年月日失败",0)
            return False

    def clearLogs(self):
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
        ip =  self.ip_prefix + cfg['ip_addr'] + '/correct.html'
        self.driver.get(ip); time.sleep(1.1)
        self.driver.switch_to.default_content()
        self.itemCheck("language", int(cfg['language']), '语言选择')
        self.itemCheck("lcdled", cfg['lines'], '相数选择')
        choice = 0
        if(int(cfg['cur_max']) >= 500 and int(cfg['lines']) != 2):
            choice = 1
        self.itemCheck("choiceA", choice , '32A/63A选择')
        self.macAddrCheck()
        
        self.driver.back(); time.sleep(1)
