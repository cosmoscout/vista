import  sys, os
from VistaPythonCommon import *

def checkout(url,path,revision='HEAD'):
    out.write('\n[VistaSVN] Warning password input currently not working :(\n')
    out.flush()
    rc, output=SysCall('svn co '+url+' '+path+' -r '+revision,ExitOnError=True)
    out.write("RC: "+str(rc)+'\n')
    out.write(output)
    
def update(path):    
    rc, output=SysCall('svn update '+path,ExitOnError=True)
    out.write("RC: "+str(rc)+'\n')
    out.write(output)
        
def _checks():
    SysCall('svn help',ExitOnError=True)
    
_checks()



#syscall svn update C:\ah548455\MA_AH\VistaCoreLibs.HEAD
#RC: 0Updating 'VistaCoreLibs.HEAD':
#At revision 36694.