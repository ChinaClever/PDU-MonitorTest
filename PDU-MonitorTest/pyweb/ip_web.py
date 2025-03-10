from selenium import webdriver
from selenium.webdriver.support.select import Select
from selenium.common.exceptions import NoSuchElementException
import configparser
import socket
import time
import os

class IpWeb:

    def __init__(self):
        self.ip_prefix = "http://"
        self.initCfg()
        self.initDriver()
        self.initNetWork()

    def initDriver(self):
        try:
            self.driver = webdriver.Firefox(executable_path="geckodriver.exe")
        except ValueError:
            self.driver = webdriver.Chrome(executable_path="chromedriver.exe")
        #finally:
            #self.driver.minimize_window()

    def initNetWork(self):
        hostname = socket.gethostname()  # 获取计算机名称
        self.dest_ip = socket.gethostbyname(hostname)  # 获取本机IP
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        if "192.168.1." in self.dest_ip:
            return True
        else:
            self.dest_ip = '127.0.0.1'
            # self.sendtoMainapp("Mac地址错误：" + mac, 0)

    def sendtoMainapp(self, parameter, res):
        message = parameter + ";" + str(res)
        self.sock.sendto(message.encode('utf-8-sig'), (self.dest_ip, 10010))

    @staticmethod
    def getCfg():
        cf = configparser.ConfigParser()
        fn = os.path.expanduser('~') + "/.PDU-MonitorTest/cfg.ini"
        cf.read(fn, 'utf-8-sig')  # 读取配置文件，如果写文件的绝对路径，就可以不用os模块
        return cf

    def initCfg(self):
        items = IpWeb.getCfg().items("ipCfg")  # 获取section名为Mysql-Database所对应的全部键值对
        self.cfgs = {'version':1, 'sw_ver': '','ip_addr': '192.168.1.163', 'ac':1, 'log_en':0, 'security':0,'lcd':0,'ip_bytepassword': 0 ,'ip_infirst': 'IEC309', 'ip_insecond': '18A', 'ip_outfirst': '20xC13', 'ip_outsecond': '4xC19', 'protocol':0 , 'lcd_switch':1,'maccheck':1,'language':1,'transformer':1}
        for it in items:
            self.cfgs[it[0]] = it[1]
            
        cfgitems = IpWeb.getCfg().items("Cfg")  # 
        for it in cfgitems:
            self.cfgs[it[0]] = it[1]
        if int(self.cfgs['lines']) == 0:
            self.cfgs['lines'] = 1
            self.cfgs['ac'] = 0

    def verCheck(self):
        tt = self.driver.find_element_by_xpath('//div/div/div/div[last()]/span')
        try:
            #try:
            #    jsSheet = 'if(confirm("确认网页Logo是否正确")){alert("Logo正确");}else{alert("Logo错误");}'
            #    self.execJs(jsSheet)
            #    time.sleep(1)
            #    while( True ):
            #        alert = self.driver.switch_to_alert().text
            #        if( alert == '确认网页Logo是否正确' ):
            #            time.sleep(1)
            #        elif( alert == 'Logo正确' ):
            #            self.sendtoMainapp('Logo正确',1)
            #            break
            #        elif( alert == 'Logo错误' ):
            #            self.sendtoMainapp('Logo错误',0)
            #            break
            #    self.driver.switch_to.alert.accept()
            #except :
            #    print('exception')
            #finally:
            #    name, ver = tt.text.split(':')
            name, ver = tt.text.split(':')
        except:
            str1, str2 = tt.text.split(' ')
            name, ver = str1.split(':')
        msg = '版本号检测，V{0}'.format(ver)
        if(self.cfgs['sw_ver'] in ver):
            self.sendtoMainapp(msg, 1)
        else:
            self.sendtoMainapp(msg+" 错误",0)

    def createAccount(self):
        self.ip_prefix = 'https://'
        ip = self.ip_prefix + self.cfgs['ip'] + '/index.html'
        user = self.cfgs['user'] = 'abcd123'
        pwd = self.cfgs['pwd'] = 'abcd123'
        bytepassword = int(self.cfgs['ip_bytepassword'])
        ret = False
        ret1 = False
        ret2 = False
        ret3 = False
        if(int(self.cfgs['version'])==10):
            user = self.cfgs['user'] = 'admin123'
            pwd = self.cfgs['pwd'] = 'admin123'
        self.driver.get(ip); time.sleep(3)
        try:
            self.setItById('old_pwd' , user)
            self.setItById('sign_pwd' , pwd)
            self.setItById('sign_repwd' , pwd)
            self.execJs('changePwd()'); time.sleep(1.2)
            self.sendtoMainapp("创建测试账号成功", 1)
        except:
            self.sendtoMainapp("创建测试账号失败", 1)
        finally:
            try:
                self.driver.refresh(); time.sleep(3)
                self.setItById("name", user)
                self.setItById("psd", pwd)
                self.execJs("login()");time.sleep(2)
                ret3 = True#能正常打开网页
                self.driver.switch_to.alert.accept()
                ret1 = True#将进入admin
            except:
                if(ret3 == True and ret1 == False):
                    ret = True#正常
                    self.sendtoMainapp("网页登陆成功", 1)
                if(ret3 == True and ret1 == True):
                    ret = False#密码异常
                if(ret3 == False and ret1 == False):
                    self.sendtoMainapp("网页登陆失败", 0)#找不到控件
            finally:
                try:
                    if(ret3 == True and ret1 == True):
                        time.sleep(0.5)
                        user = self.cfgs['user'] = 'admin'
                        pwd = self.cfgs['pwd'] = 'admin'
                        self.driver.refresh(); time.sleep(3)
                        self.setItById("name", user)
                        self.setItById("psd", pwd)
                        self.execJs("login()");time.sleep(2)
                        time.sleep(1.2)
                        ret2 = True#正常
                        self.sendtoMainapp("网页登陆成功", 1)
                except:
                    if(ret2 == False):
                        self.sendtoMainapp("网页登陆失败", 0)
                finally:
                    time.sleep(1.2)

    def inputAccount(self):
        
        user = self.cfgs['user'] = 'admin'
        pwd = self.cfgs['pwd'] = 'admin'
        if(int(self.cfgs['version'])==5):
            user = self.cfgs['user'] = 'abcd123'
            pwd = self.cfgs['pwd'] = 'abcd123'
        elif(int(self.cfgs['version'])==7):
            user = self.cfgs['user'] = 'huawei'
            pwd = self.cfgs['pwd'] = 'huawei'
        elif(int(self.cfgs['version'])==10):
            self.ip_prefix = 'https://'
            user = self.cfgs['user'] = 'admin123'
            pwd = self.cfgs['pwd'] = 'admin123'
        elif(int(self.cfgs['version'])==11):
            self.ip_prefix = 'https://'
            user = self.cfgs['user'] = 'admin'
            pwd = self.cfgs['pwd'] = 'admin'
        
        try:
            ip = self.ip_prefix + self.cfgs['ip'] + '/'
            self.driver.get(ip); time.sleep(2.2)
            self.setItById("name", user)
            self.setItById("psd", pwd)
            self.execJs("login()")
            self.sendtoMainapp("网页登陆成功", 1)
        except:
            self.sendtoMainapp("网页登陆失败", 0)
        finally:
            time.sleep(1.2)

    def login(self):
        self.cfgs['ip'] = self.cfgs['ip_addr']
        security = int(self.cfgs['security'])
        v = int(self.cfgs['version'])
        flag = int(self.cfgs['ip_bytepassword'])
        password = True
        if( v == 10 and flag == 0):
            password = False
        if( v == 11):
            password = False
        
        if(security and v != 9 and password):
            self.createAccount()
        else:
            self.inputAccount()
        time.sleep(2)
        self.verCheck()

    def checkEnv(self):
        self.divClick(2)
        self.itemCheck("min7", 0, '温度最小值')
        self.itemCheck("max7", 40, '温度最大值')
        self.itemCheck("min8", 0, '湿度最小值')
        self.itemCheck("max8", 99, '湿度最大值')
        
    def checkEnvIP6(self):
        self.divClick(2)
        ret = 1
        tt = self.driver.find_element_by_id('tem').text
        if(int(tt) == 0):
                ret = 0
        self.sendtoMainapp('检查温度当前值：'+tt+'℃', ret)
        ret = 1
        tt = self.driver.find_element_by_id('hum').text
        if(int(tt) == 0):
                ret = 0
        self.sendtoMainapp('检查湿度当前值：'+tt+'%', ret)
        self.itemCheck("min7", 0, '温度最小值')
        self.itemCheck("max7", 40, '温度最大值')
        self.itemCheck("min8", 0, '湿度最小值')
        self.itemCheck("max8", 99, '湿度最大值')

    def setSelectLcd(self, id, v):
        try:
            time.sleep(0.15)
            it = self.driver.find_element_by_id(id)
        except NoSuchElementException:
            msg = '网页上找不到{0}'.format(id)
        else:
            if it.is_displayed():
                self.itemCheck(id, v, 'LCD方向')
                #Select(it).select_by_index(v); time.sleep(1)
                #self.execJs("setdevice()"); time.sleep(1)
                #self.driver.switch_to.alert.accept()

    def checkLcdDir(self):
        dir = self.cfgs['lcd']
        self.setSelectLcd("dir", dir)
        if(int(self.cfgs['version']) == 8):
            self.setSelectLcd("direction", dir)
        
    def setEle(self):
        if(int(self.cfgs['version']) <= 5 or int(self.cfgs['version']) == 10):
            self.checkEnv()
        self.divClick(3)
        self.checkLcdDir()
        jsSheet = " claerset = createXmlRequest();claerset.onreadystatechange = clearrec;ajaxget(claerset, \"/energyzero?a=\" + {0}+\"&\");"
        for num in range(0, 4):
            self.execJs(jsSheet.format(num))
        self.sendtoMainapp("设备电能清除成功", 1)

    def setSelect(self, id, v):
        it = self.findItById(id)
        Select(it).select_by_index(v)
        time.sleep(0.7)

    def findItById(self, id):
        try:
            time.sleep(0.15)
            it = self.driver.find_element_by_id(id)
        except NoSuchElementException:
            msg = '网页上找不到{0}'.format(id)
            #self.sendtoMainapp(msg, 1)
            return None
        else:
            return it

    def setItById(self, id, v):
        it = self.findItById(id)
        if it != None:
            it.clear()
            it.send_keys(str(v))

    def btnClick(self, id):
        self.driver.find_element_by_id(id).click()
        time.sleep(0.8)

    def alertClick(self, id):
        self.btnClick(id)
        self.driver.switch_to.alert.accept()
        time.sleep(0.8)

    def divClick(self, id):
        security = int(self.cfgs['security'])
        self.driver.switch_to.default_content()
        self.execJs("clk({0})".format(id))
        self.driver.switch_to.frame('ifrm')
        if(security):time.sleep(1.2)
        time.sleep(0.8)

    def execJs(self, js):
        try:
            security = int(self.cfgs['security'])
            self.driver.execute_script(js);time.sleep(1)
            if(security):time.sleep(1.5)
        except:
            msg = '网页上找不到此方法{0}'.format(js)

    def execJsAlert(self, js):
        self.execJs(js);time.sleep(0.4)
        self.driver.switch_to.alert.accept()
        time.sleep(1)

    def resetFactory(self):
        v = self.cfgs['version']
        if( int(v) > 2 ):
            self.divClick(10)
        else:
            self.divClick(8)
        self.setSelect("order",1)
        self.execJsAlert("setdevice();")
        self.sendtoMainapp("设备Web出厂设置成功", 1)
        time.sleep(0.5)
    
    def reStart(self):
        v = self.cfgs['version']
        if( int(v) > 2 ):
            self.divClick(10)
        else:
            self.divClick(8)
        self.setSelect("order",0)
        self.execJsAlert("setdevice();")
        self.sendtoMainapp("设备重启设置成功", 1)
        time.sleep(0.5)

    def macAddrCheck(self):
        it = self.findItById('mac1')
        mac = it.get_attribute('value')
        if(int(self.cfgs['maccheck']) == 1):
            if "2C:26:5F:" in mac:
                self.sendtoMainapp("Mac地址合法："+mac, 1)
            else:
                self.sendtoMainapp("Mac地址错误："+mac, 0)
        else:
            self.sendtoMainapp("Mac地址："+mac, 1)

    def itemCheck(self, ssid, value, parameter):
        try:
            it = self.findItById(ssid)
            if it != None:
                v = it.get_attribute('value')
                msg = '检测{0}，期望值{1}，实际值{2}'.format(parameter, value, v)
                if (str(value) != str(v)):
                    self.sendtoMainapp('错误 '+msg, 0)
                else:
                    self.sendtoMainapp(msg, 1)
        except:
            pass

    def checkIpv6Threshold(self):
        curMin = self.cfgs['cur_min']
        curMax = self.cfgs['cur_max']
        volMin = self.cfgs['vol_min']
        volMax = self.cfgs['vol_max']
        line = 3
        self.checkCur(line, curMin, curMax)
        self.checkVol(line, volMin, volMax)
        self.checkEnvIP6()
        self.sendtoMainapp("检查设备报警阈值成功", 1)
        
    def checkCur(self, lines, min, max):
        p = '电流阈值'
        for num in range(1, lines + 1):
            self.itemCheck("min" + str(num), int(min), p)
            self.itemCheck("lower_L" + str(num), int(min), p)
            self.itemCheck("Upper_L" + str(num), int(int(max)/10), p)
            self.itemCheck("max" + str(num), int(int(max)/10), p)

    def checkVol(self, lines, min, max):
        p = '电压阈值'
        for num in range(4, lines + 4):
            self.itemCheck("min" + str(num), min, p)
            self.itemCheck("max" + str(num), max, p)
            
    def checkIpv6Env(self):

        p = '温度阈值'
        self.itemCheck("min7", 0, p)
        self.itemCheck("max7", 40, p)

        p = '湿度阈值'
        self.itemCheck("min8", 0, p)
        self.itemCheck("max8", 99, p)
        
    def checkIpv3ByteCur(self):
        p = '电流预警值'
        min = int(self.cfgs['cur_min'])
        max = int(self.cfgs['cur_max'])
        lines = int(self.cfgs['lines'])
        self.divClick(2)
        size = lines
        if (size == 2):
            lines = 3
        for num in range(1, lines + 1):
            if (size == 2 and num == 2):
                max = int((int(max) + 1) // 2)
            self.itemCheck("Lmax" + str(num), round(int(max)/10*0.8,1), p)
        time.sleep(2.2)
        
    def checkIpv3ECCCur(self):
        p = '电流预警值'
        min = int(self.cfgs['cur_min'])
        max = int(self.cfgs['cur_max'])
        size = int(self.cfgs['lines'])
        lines = 1
        self.divClick(2)
        if (size == 2):
            lines = 3
        for num in range(1, lines + 1):
            if (size == 2 and num == 2):
                max = ((max + 10) // 2)
            self.itemCheck("min" + str(num), min/10, p)
            self.itemCheck("Lmax" + str(num), round(max/10*0.8,1), p)
            self.itemCheck("max" + str(num), max/10, p)
        time.sleep(2.2)

