from monitor_ip.ip_web import  *

class IpV1YIXIN(IpWeb):

    def start_fun(self):
        self.login()
        self.setEle()
        self.checkCorrect()
        self.checkValue()
        self.resetFactory()
        self.driver.quit()

    def checkCorrect(self):
        cfg = self.cfgs
        ip =  self.ip_prefix + cfg['ip_addr'] + '/correct.html'
        self.driver.get(ip); time.sleep(1.1)
        self.driver.switch_to.default_content()
        self.itemCheck("language", int(cfg['language'])+1, '语言选择')
        choice = 0
        if(int(cfg['cur_max']) >= 500 and int(cfg['lines']) != 2):
            choice = 1
        self.itemCheck("choiceA", choice , '32A/63A选择')
        self.macAddrCheck()
        self.driver.back(); time.sleep(1)
        
    def login(self):
        self.cfgs['ip'] = self.cfgs['ip_addr']
        self.inputAccount()
        self.verCheck()
        
    def checkValue(self):
        curMin = self.cfgs['cur_min']
        curMax = self.cfgs['cur_max']
        volMin = self.cfgs['vol_min']
        volMax = self.cfgs['vol_max']
        line = 1
        self.divClick(2)
        self.checkCur(line, curMin, curMax)
        self.checkVol(line, volMin, volMax)
        self.checkEnvValue()
        self.checkOtherValue()
        self.sendtoMainapp("设备报警阈值设置成功", 1)
        
    def checkEnvValue(self):    
        temMin = 0
        temMax = 40
        humMin = 0
        humMax = 99
        
        self.itemCheck("min7", temMin, '温度1最小值')
        self.itemCheck("max7", temMax, '温度1最大值')
        self.itemCheck("min8", humMin, '湿度1最小值')
        self.itemCheck("max8", humMax, '湿度1最大值')

        self.itemCheck("min9", temMin, '温度2最小值')
        self.itemCheck("max9", temMax, '温度2最大值')
        self.itemCheck("min10", humMin, '湿度2最小值')
        self.itemCheck("max10", humMax, '湿度2最大值')
    
    def checkOtherValue(self): 
        fanMin = 20
        fanMax = 60
        pwmMax = 60
        
        self.itemCheck("min11", fanMin, '风扇曲线温度最小值')
        self.itemCheck("max11", fanMax, '风扇曲线温度最大值')
        self.itemCheck("max12", pwmMax, 'PWM最大值')
        
    def resetFactory(self):
        self.divClick(8)
        self.setSelect("order",1)
        self.execJsAlert("setdevice()")
        self.sendtoMainapp("设备Web出厂设置成功", 1)
        time.sleep(0.5)






