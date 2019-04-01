###################### preparation
import sys, os, time
sys.path.append(os.getenv("VISTA_CMAKE_COMMON"))
from VistaPythonCommon import *
import VistaSVN
import VistaBuild

###################### settings
# set default compiler
if sys.platform == 'linux2':
    strCompiler='GCC_DEFAULT'        
elif sys.platform == 'win32':
    strCompiler='MSVC_10'  # MSVC_10_64

bDelCMakeCache = True   # if set to true the cmake cache will be wiped and regenerated
strSVNBasePath='https://svn.rwth-aachen.de/repos/vrgroup-svn/projects/'

# the following three lines set the basedir to ../ 
#BASEDIR=os.getcwd()+os.sep+'..'
#os.chdir(BASEDIR)

###################### definition of projects

lstPROJECTS=list()
lstPROJECTS.append(dict(name='VistaCoreLibs.trunk',svnpath=strSVNBasePath+'Vista/trunk/VistaCoreLibs/',revision=None,CMakeVariables='-DVISTACORELIBS_BUILD_TESTS=OFF'))

#Inside dependencies
lstPROJECTS.append(dict(name='VistaFlowLib.trunk',svnpath=strSVNBasePath+'VistaFlowLib/trunk/',revision=None,CMakeVariables=None))

lstPROJECTS.append(dict(name='VistaCollisionDetection.trunk',svnpath=strSVNBasePath+'VistaAddonLibs/VistaCollisionDetection/trunk/',revision=None,CMakeVariables=None))
lstPROJECTS.append(dict(name='VistaCollisionInterface.trunk',svnpath=strSVNBasePath+'VistaAddonLibs/VistaCollisionInterface/trunk/',revision=None,CMakeVariables=None))
lstPROJECTS.append(dict(name='VistaPhysicsInterface.trunk',svnpath=strSVNBasePath+'VistaAddonLibs/VistaPhysicsInterface/trunk',revision=None,CMakeVariables=None))
lstPROJECTS.append(dict(name='VistaMedia.NEWAUDIOINTERFACE',svnpath=strSVNBasePath+'VistaAddonLibs/VistaMedia/branches/NEWAUDIOINTERFACE',revision=None,CMakeVariables=None))
lstPROJECTS.append(dict(name='LuaBridge.CLEANUP',svnpath=strSVNBasePath+'LuaBridge/branches/CLEANUP',revision=None,CMakeVariables=None))
lstPROJECTS.append(dict(name='VistaSketchInterface.trunk',svnpath=strSVNBasePath+'VistaAddonLibs/VistaSketchInterface/trunk',revision=None,CMakeVariables=None))

#Inside 
lstPROJECTS.append(dict(name='Inside.trunk',svnpath=strSVNBasePath+'Inside/trunk',revision=None))

###################### deploy and build
# no need to change below here, but also not really rocket science

VistaSVN.update(os.getenv("VISTA_CMAKE_COMMON"))


BASEDIR=os.getcwd()
globstarttime=time.time()
for i in range(0,len(lstPROJECTS)):
    if None==lstPROJECTS[i]['revision']:
        path=BASEDIR+os.sep+lstPROJECTS[i]['name']
    else:
        path=BASEDIR+os.sep+lstPROJECTS[i]['name']+'.'+lstPROJECTS[i]['revision']
    
    out.write('\n##################################################\n')
    out.write('\nStarting Project: '+lstPROJECTS[i]['name']+'\n\n')
    out.write('\n##################################################\n')
    projstarttime=time.time()
    if os.path.exists(path):
        out.write('\nperforming svn update')
        out.flush()
        VistaSVN.update(path)
    else:
        out.write('\nperforming svn checkout')
        out.flush()
        if None==lstPROJECTS[i]['revision']:
            VistaSVN.checkout(lstPROJECTS[i]['svnpath'],path)
        else:
            VistaSVN.checkout(lstPROJECTS[i]['svnpath'],path,lstPROJECTS[i]['revision'])
            
    os.chdir(path)   
    
    CMakeVariables=''
    if None!=lstPROJECTS[i]['CMakeVariables']:
        CMakeVariables = lstPROJECTS[i]['CMakeVariables']
            
    #see VistaBuild.py
    VistaBuild.BuildIt(strBuildType='Default',strCompiler=strCompiler, bDeleteCMakeCache=bDelCMakeCache ,strBuildFolder='build32',strCMakeVariables=CMakeVariables)
    out.write("\n\nProject "+lstPROJECTS[i]['name']+" finished after: "+str(int(time.time()-projstarttime))+" seconds\n")
out.write('\ndone. Totaltime: '+str(int(time.time()-globstarttime))+" seconds\n")
ExitGently()


