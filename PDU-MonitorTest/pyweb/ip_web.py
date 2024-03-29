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
        self.cfgs = {'version':1, 'sw_ver': '','ip_addr': '192.168.1.163', 'ac':1, 'log_en':0, 'security':0}
        for it in items:
            self.cfgs[it[0]] = it[1]
        if int(self.cfgs['lines']) == 0:
            self.cfgs['lines'] = 1
            self.cfgs['ac'] = 0

    def verCheck(self):
        tt = self.driver.find_element_by_xpath('//div/div/div/div[last()]/span')
        try:
            try:
                jsSheet = 'if(confirm("确认网页Logo是否正确")){alert("Logo正确");}else{alert("Logo错误");}'
                self.execJs(jsSheet)
                time.sleep(1)
                while( True ):
                    alert = self.driver.switch_to_alert().text
                    if( alert == '确认网页Logo是否正确' ):
                        time.sleep(1)
                    elif( alert == 'Logo正确' ):
                        self.sendtoMainapp('Logo正确',1)
                        break
                    elif( alert == 'Logo错误' ):
                        self.sendtoMainapp('Logo错误',0)
                        break
                self.driver.switch_to.alert.accept()
            except :
                print('exception')
            finally:
                name, ver = tt.text.split(':')
        except:
            str1, str2 = tt.text.split(' ')
            name, ver = str1.split(':')
        msg = '版本号检测，V{0}'.format(ver)
        if(ver != self.cfgs['sw_ver'] ):
            self.sendtoMainapp(msg+" 错误",0)
        else:
            self.sendtoMainapp(msg, 1)

    def createAccount(self):
        self.ip_prefix = 'https://'
        ip = self.ip_prefix + self.cfgs['ip'] + '/index.html'
        user = self.cfgs['user'] = 'abcd123'
        pwd = self.cfgs['pwd'] = 'abcd123'
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
            self.driver.refresh(); time.sleep(1)
            self.setItById("name", user)
            self.setItById("psd", pwd)
            self.execJs("login()");time.sleep(3)

    def inputAccount(self):
        ip = self.ip_prefix + self.cfgs['ip'] + '/'
        user = self.cfgs['user'] = 'admin'
        pwd = self.cfgs['pwd'] = 'admin'
        self.driver.get(ip); time.sleep(2.2)
        try:
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
        if(security):
            self.createAccount()
        else:
            self.inputAccount()
        self.verCheck()

    def checkEnv(self):
        self.divClick(2)
        self.itemCheck("min7", 0, '温度最小值')
        self.itemCheck("max7", 40, '温度最大值')
        self.itemCheck("min8", 0, '湿度最小值')
        self.itemCheck("max8", 99, '湿度最大值')

    def setEle(self):
        self.checkEnv()
        self.divClick(3)
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
        security = int(self.cfgs['security'])
        self.driver.execute_script(js);time.sleep(1)
        if(security):time.sleep(1)

    def execJsAlert(self, js):
        self.execJs(js);time.sleep(0.4)
        self.driver.switch_to.alert.accept()
        time.sleep(1)

    def resetFactory(self):
        v = self.cfgs['version']
        if(3 == int(v)):
            self.divClick(10)
        else:
            self.divClick(8)
        self.setSelect("order",1)
        self.execJsAlert("setdevice();")
        self.sendtoMainapp("设备Web出厂设置成功", 1)
        time.sleep(0.5)

    def macAddrCheck(self):
        it = self.findItById('mac1')
        mac = it.get_attribute('value')
        if "2C:26:5F:" in mac:
            self.sendtoMainapp("Mac地址合法："+mac, 1)
        else:
            self.sendtoMainapp("Mac地址错误："+mac, 0)

    def itemCheck(self, ssid, value, parameter):
        it = self.findItById(ssid)
        if it != None:
            v = it.get_attribute('value')
            msg = '检测{0}，期望值{1}，实际值{2}'.format(parameter, value, v)
            if (str(value) != str(v)):
                self.sendtoMainapp('错误 '+msg, 0)
            else:
                self.sendtoMainapp(msg, 1)





