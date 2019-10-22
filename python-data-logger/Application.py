from ApplicationDelegate import *
# Necessary to make the classes known to the
# runtime
from ScanView import *
from LaptimerView import *


if __name__ == '__main__':
    from PyObjCTools import AppHelper
    AppHelper.runEventLoop()
