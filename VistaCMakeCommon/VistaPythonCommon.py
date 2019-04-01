# $Id
# VistaPythonCommon python stuff

import sys, os, subprocess
from optparse import OptionParser
out = sys.stdout   ## use out.write('foobar') for multiplatform and python independant prints
err = sys.stderr

__optionparser = None

#flush streambuffers and quit
def ExitGently(iReturnCode = 0):
    err.flush()
    out.flush()
    os._exit(iReturnCode)

#flush streambuffers and quit with error message 
def ExitError(strErrorMessage,iReturnCode = -1):
    out.flush()
    err.write('\n'+strErrorMessage+'\n')
    err.flush()    
    os._exit(iReturnCode)

#shell or commandline call of strCmd
#    @return Errorcode and Commandoutput
def SysCall(strCmd, ExitOnError = True,Debug=False):
    iReturnCode = 0
    if True == Debug:
        out.write('\nExecuting Command:'+strCmd+'\n')
        out.flush()
    pCall = subprocess.Popen(strCmd, universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
    strOutput = pCall.communicate()[0]
    iReturnCode = int(pCall.returncode)
    if iReturnCode != 0:
        out.write(strOutput)
        err.write('Systemcall with command ' + strCmd + ' failed with return ' + str(iReturnCode) + '\n')
        if True == ExitOnError:
            ExitError('Exiting with ',iReturnCode)
    out.flush()
    err.flush()
    return iReturnCode, strOutput
    
    
#checks VISTA_CMAKE_COMMON and VISTA_EXTERNAL_LIBS
# is called upon first import 
def _CheckForVistaEnv():
    #VISTA_CMAKE_COMMON
    val = os.getenv("VISTA_CMAKE_COMMON")
    if val is None:
        ExitError("Exiting, VISTA_CMAKE_COMMON not set\n",-1)
    if not os.path.exists(val):
        ExitError("Exiting, Path of VISTA_CMAKE_COMMON ("+val+") does not exist\n",-1)
        
    #VISTA_EXTERNAL_LIBS
    val = os.getenv("VISTA_EXTERNAL_LIBS")
    if val is None:
        ExitError("Exiting, VISTA_EXTERNAL_LIBS not set\n",-1)
    # now we have the special case that VISTA_EXTERNAL_LIBS allows multiple pathes like /home/vrsw/:/home/av006de/dev
    if sys.platform == 'win32':
        strSeperator=';'
    else:
        strSeperator=':'
    strPathes=val.split(strSeperator)
    for p in strPathes:
        if not os.path.exists(p):
            ExitError("Exiting, Path of VISTA_EXTERNAL_LIBS ("+p+") does not exist\n",-1)
    
# not used right now but maybe later
def AddVistaPythonCommonArgs(parser):
    parser.add_option("-v", action="store_true", dest="verbose", default=False)
    parser.add_option("-q", action="store_false", dest="verbose")
    
    global __optionparser
    __optionparser = parser
    return parser

#CMake everytime return zero, even when errors occur 
#so this functions parses the output for errors
def CheckForCMakeError(ConsoleText):
    if 'CMake Error' in ConsoleText:
        return True
    else:
        return False
    

_CheckForVistaEnv()
