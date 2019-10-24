import subprocess 
import os, string
available_drives = ['%s:' % d for d in string.ascii_uppercase if os.path.exists('%s:' % d)]


def findDriveByDriveLabel(driveLabel):
    for dl in available_drives:
        try:
            if (os.path.isdir(dl) != 0):
                val = subprocess.check_output(["cmd", "/c vol " + dl])
                if (driveLabel in str(val)):
                    return dl
        except:
            print("Error: findDriveByDriveLabel(): exception")
    return 0
letra=findDriveByDriveLabel("STM32")
print(letra)    

Import('env')
environ = env['ENV']
target='%s\\Microsoft\\WindowsApps' % environ['LOCALAPPDATA']
env.Replace(UPLOADCMD='copy /y $SOURCE %s\\' % letra)