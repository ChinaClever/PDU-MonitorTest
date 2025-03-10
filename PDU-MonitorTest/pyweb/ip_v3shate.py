from monitor_ip.ip_web import  *

class IpV3SHATE(IpWeb):

    def start_fun(self):
        self.login()
        self.checkCorrect()
        self.setEle()
        self.checkValue()
        self.checkEngID()
        self.clearLogs()
        self.resetFactory()
        self.driver.quit()

    def setTime(self):
        self.divClick(8)
        self.execJsAlert("check_time()")
        self.sendtoMainapp("设置设备时间", 1)
    
    def checkValue(self):
        self.divClick(2)
        time.sleep(3)
        self.checkVol()
        self.checkCur()
        self.checkLoopCur()
        self.checkTemHum()
        
    def checkVol(self):
        cfg = self.cfgs
        for num in range(1, 4):
            idmin = 'min'+str(num)
            textmin = 'L'+str(num)+'相电压最小值'
            idmax = 'max'+str(num)
            textmax = 'L'+str(num)+'相电压最大值'
            self.itemCheck(idmin, int(cfg['vol_min']), textmin)
            self.itemCheck(idmax, int(cfg['vol_max']), textmax)
    
    def checkCur(self):
        cfg = self.cfgs
        for num in range(16, 19):
            idmin = 'min'+str(num)
            textmin = 'L'+str(num-15)+'相电流最小值'
            idmax = 'max'+str(num)
            textmax = 'L'+str(num-15)+'相电流最大值'
            self.itemCheck(idmin, int(cfg['cur_min'])//10.0, textmin)
            self.itemCheck(idmax, int(cfg['cur_max'])//10.0,textmax)
            
    def checkLoopCur(self):
        cfg = self.cfgs
        index = 31
        if(int(cfg['cur_max']) == 320):
            index = 25
        for num in range(19, index):
            idmin = 'min'+str(num)
            textmin = 'C'+str(num-18)+'回路电流最小值'
            idmax = 'max'+str(num)
            textmax = 'C'+str(num-18)+'回路电流最大值'
            self.itemCheck(idmin, int(cfg['loopcur_min'])//10.0, textmin)
            self.itemCheck(idmax, int(cfg['loopcur_max'])//10.0,textmax)
    
    def checkTemHum(self):
        for num in range(31, 33):
            v , id , text , value , ret ='℃', 'tem' , '温度' ,40 , 1
            if(num==32):
                v , id , text , value= '%','hum' , '湿度' , 99
            idmin = 'min'+str(num)
            textmin = text+'最小值'
            idmax = 'max'+str(num)
            textmax = text+'最大值'
            tt = self.driver.find_element_by_id(id).text
            if(int(tt) == 0):
                ret = 0
            self.sendtoMainapp('检查'+text+'当前值：'+tt+v, ret)
            self.itemCheck(idmin, 0 , textmin)
            self.itemCheck(idmax, value ,textmax)
    
    def checkEngID(self):
        self.divClick(6)
        engid = self.driver.find_element_by_id('engID').text
        #print('engid %s'%(engid))
        mac = self.driver.find_element_by_id('mac').text
        #print('mac %s'%(mac))
        #print('engid[0:2] %s mac[0:2] %s'%(engid[0:2] , mac[0:2]))
        #print(engid[0:2]==mac[0:2])
        #print('engid[3:5] %s mac[-2:] %s'%(engid[3:5] , mac[-2:]))
        #print(engid[3:5]==mac[-2:])
        #print('engid[6:8] %s \'01\' %s'%(engid[6:8] , '01'))
        #print(engid[6:8]=='01')
        #print('engid[9:17] %s mac[3:11] %s'%(engid[9:17] , mac[3:11]))
        #print(engid[9:17]==mac[3:11])
        #print('engid[-5:-3] %s \'A3\' %s'%(engid[-5:-3] , 'A3'))
        #print(engid[-5:-3]=='A3')
        #print('engid[-2:] %s mac[-5:-3] %s'%(engid[-2:] , mac[-5:-3]))
        #print(engid[-2:]==mac[-5:-3])
        if(engid[0:2]==mac[0:2] and 
        engid[3:5]==mac[-2:] and 
        engid[6:8]=='01' and
        engid[9:17]==mac[3:11] and
        engid[-5:-3]=='A3' and
        engid[-2:]==mac[-5:-3]):
            self.sendtoMainapp("检查engID成功,engid:%s,mac:%s"%(engid , mac), 1)
        else:
            self.sendtoMainapp("检查engID失败,engid:%s,mac:%s"%(engid , mac), 0)
    
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
        time.sleep(3)
        jsSheet = "xmlset = createXmlRequest();xmlset.onreadystatechange = setdata;ajaxgets(xmlset, \"/setlclear?a=\" + {0} + \"&\");"
        for num in range(0, 2):
            self.setSelect("loglist", num)
            self.execJs(jsSheet.format(num))
            time.sleep(6)
        self.sendtoMainapp("设备日志清除成功", 1)

    def checkCorrect(self):
        cfg = self.cfgs
        security = int(self.cfgs['security'])
        if(security): time.sleep(1.2)
        ip = self.ip_prefix + cfg['ip'] + '/correct.html'
        self.driver.get(ip); time.sleep(1.2)
        if(security): time.sleep(1.2)
        self.driver.switch_to.default_content()
        self.itemCheck("language", cfg['language'], '设备语言')
        self.itemCheck("modbus", cfg['modbus'], '设备模式')
        self.itemCheck("Log_flag", cfg['log_en'], '日志功能')
        index = 1
        if(int(cfg['cur_max']) == 630):
            index = 2
        self.itemCheck("circuit", index, '回路数选择')
        self.macAddrCheck()
        if(security):time.sleep(2.3)
        self.checkCorrect1(index)
        self.sendtoMainapp("设备后台网页配置成功", 1)
        self.driver.back(); time.sleep(1)
        if(security):time.sleep(1.3)
    
    def checkCorrect1(self,index):
        cfg = self.cfgs
        self.itemCheck("min1", int(cfg['vol_min']), '相电压最小值')
        self.itemCheck("max1", int(cfg['vol_max']), '相电压最大值')
        self.itemCheck("min3", int(cfg['cur_min'])//10.0, '相电流最小值')
        self.itemCheck("max3", int(cfg['cur_max'])//10.0, '相电流最大值')
        self.itemCheck("min4", int(cfg['loopcur_min'])//10.0, '回路电流最小值')
        self.itemCheck("max4", int(cfg['loopcur_max'])//10.0, '回路电流最大值')
        self.itemCheck("min5", 0, '温度最小值')
        self.itemCheck("max5", 40, '温度最大值')
        self.itemCheck("min6", 0, '湿度最小值')
        self.itemCheck("max6", 99, '湿度最大值')
            

    





