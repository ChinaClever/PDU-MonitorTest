from monitor_ip.ip_web import  *

class IpV1(IpWeb):

    def start_fun(self):
        self.login()
        self.setEle()
        self.checkCorrect()
        self.resetFactory()
        self.driver.quit()

    def checkCorrect(self):
        cfg = self.cfgs
        security = int(self.cfgs['security'])
        ip =  self.ip_prefix + cfg['ip_addr'] + '/correct.html'
        self.driver.get(ip); time.sleep(1.1)
        if(security): time.sleep(1.2)
        self.driver.switch_to.default_content()
        #self.itemCheck("language", int(cfg['language'])+1, '语言选择')
        self.itemCheck("language", cfg['language'], '语言选择')
        self.itemCheck("modbus", cfg['modbus'], '模式选择')
        self.itemCheck("lcdled", cfg['lines'], '相数选择')
        self.itemCheck("ACDC", cfg['ac'], '交直流选择')
        self.macAddrCheck()
        self.driver.back(); time.sleep(1)
        if(security):time.sleep(1.3)







