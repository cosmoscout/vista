# Windows build script designed for jenkins
# $Id

import  sys, os, time, shutil, VistaPythonCommon

#build project in current directory     
def BuildIt(strBuildType='Default', strCompiler = 'MSVC_10_64BIT', strCMakeVariables = '', bDeleteCMakeCache = True, strBuildFolder='JenkinsDefault', bRunTests = False, bInstall = False, bPackage = False):

    #make sure we are on windows system
    if sys.platform != 'win32':
        VistaPythonCommon.ExitError('\n\n*** ERROR *** Win32 build on non Windows system\n\n',-1)
        
    sys.stdout.write('Buildtype: ' + strBuildType + '\n')
    sys.stdout.write('Compiler: ' + strCompiler + '\n')
    sys.stdout.write('CMake Definitions: ' + strCMakeVariables + '\n')
   
    
    if True == bRunTests:
        sys.stdout.write('Executing tests\n')
    if True == bInstall:
        sys.stdout.write('Make install\n')
    sys.stdout.flush()
    
    fStartTime=time.time()
    strBasepath = os.getcwd()
    
        
    strVCVersion = strCompiler.split('_')[1]
    if not strVCVersion.isdigit():
        sys.stderr.write('\n\n*** ERROR *** Formt of Visual Studio version: '+ strVCVersion +' \n\n')
        ExitGently(-1)
    
    #strArch = ''
    #strMSCV = 'Visual Studio ' + strVCsVersion
    #if '64BIT' in strCompiler:
        #strArch = '-x64'
        #strMSCV += ' Win64'
    
    if strBuildFolder is 'JenkinsDefault':
        strBuildFolder='build'#.win32' + strArch + '.vc' + strVCVersion #shortening this one because of vc10 bug regarding filename length 
    
    if not os.path.exists(strBuildFolder):
        VistaPythonCommon.SysCall('mkdir ' + strBuildFolder)
    else:
       if True == bDeleteCMakeCache:
            shutil.rmtree(strBuildFolder,True)#clean cmake build
            sys.stdout.write("\nDeleting Cache\nElapsed time : " + str(int(time.time()-fStartTime)) + " seconds\n")
            VistaPythonCommon.SysCall('mkdir ' + strBuildFolder)

    os.chdir(os.path.join(strBasepath, strBuildFolder))
    
    #configure cmake
    strCMakeCmd = 'cmake.exe -version;cmake.exe -G "' + getMSVCGeneratorString(strCompiler,strVCVersion) + '" ' + strCMakeVariables + ' ' + os.path.join(strBasepath)
    iRC, strConsoleOutput = VistaPythonCommon.SysCall(strCMakeCmd)
    sys.stdout.write(strConsoleOutput)
    sys.stdout.flush()

    if VistaPythonCommon.CheckForCMakeError(strConsoleOutput):        
        VistaPythonCommon.ExitError('\n\n*** ERROR *** Cmake failed to generate configuration\n\n',-1)
        
    #make it
    if strBuildType is not 'Default':
        MSVCBuildCall(strBuildType, strVCVersion)
    else:
        MSVCBuildCall('Debug', strVCVersion)
        MSVCBuildCall('Release', strVCVersion)  
    
    #execute tests
    if True == bRunTests:
        if strBuildType is not 'Default':
            MSVCTestCall(strBuildType, strVCVersion)
        else:
            MSVCTestCall('Debug', strVCVersion)
            MSVCTestCall('Release', strVCVersion)     
        
    #install
    if True == bInstall:
        if strBuildType is not 'Default':
             MSVCCall('INSTALL', strBuildType, strVCVersion)
        else:
            MSVCCall('INSTALL', 'Debug', strVCVersion)
            MSVCCall('INSTALL', 'Release', strVCVersion) 

    #install
    if True == bPackage:
        if strBuildType is not 'Default':
             MSVCCall('PACKAGE', strBuildType, strVCVersion)
        else:
            MSVCCall('PACKAGE', 'Debug', strVCVersion)
            MSVCCall('PACKAGE', 'Release', strVCVersion)     
        
    os.chdir(os.path.join(strBasepath))
    
    if True == bDeleteCMakeCache:
        CleanWorkspace(os.path.join(strBasepath, strBuildFolder))
       
    sys.stdout.write("\n\nElapsed time: " + str(int(time.time()-fStartTime)) + " seconds\n")
    sys.stdout.flush()
    
def CleanWorkspace(strdirpath):
    sys.stdout.write("\nCleaning *.obj files from "+strdirpath+"\n")
    for (dirpath, dirnames, filenames) in os.walk(strdirpath):        
        for filename in filenames:
            temp, fileExtension = os.path.splitext(filename)
            if fileExtension == '.obj': 
                try:
                    os.remove(os.sep.join([dirpath,filename]))
                except:
                    sys.stderr.out("Error while deleting "+os.sep.join([dirpath,filename]))
    sys.stdout.flush()

def MSVCBuildCall(strBuildType, strVCVersion):
        sys.stdout.write('\nStarting to build '+strBuildType+ '\n')
        strVC = getVCvarsall( strVCVersion )
        strVC += ' & msbuild ALL_BUILD.'+getProjextFileEnding( strVCVersion )+' /property:configuration=' + strBuildType
        strVC += ' /maxcpucount /clp:WarningsOnly;ErrorsOnly;ForceNoAlign '
        iRC, strConsoleOutput = VistaPythonCommon.SysCall(strVC)
        sys.stdout.write(strConsoleOutput)
        sys.stdout.flush()
        
def MSVCTestCall(strBuildType, strVCVersion):
        sys.stdout.write('\nStarting to build Tests \n')
        strVC = getVCvarsall( strVCVersion )
        strVC += ' & msbuild RUN_TESTS.'+getProjextFileEnding( strVCVersion )+' /property:configuration=' + strBuildType
        iRC, strConsoleOutput = VistaPythonCommon.SysCall(strVC)
#        if 0 != iRC:
#            strVC = getVCvarsall( strVCVersion )
#            strVC += ' & msbuild RUN_TESTS_VERBOSE.'+getProjextFileEnding( strVCVersion )
#            iRC, strConsoleOutput = VistaPythonCommon.SysCall(strVC,ExitOnError = True)
        sys.stdout.write(strConsoleOutput)
        sys.stdout.flush()
       
def MSVCCall(strTarget, strBuildType, strVCVersion):
        sys.stdout.write('\nStarting to build '+strTarget+ '\n')
        strVC = getVCvarsall( strVCVersion )
        strVC += ' & msbuild '+strTarget+'.'+getProjextFileEnding( strVCVersion )+' /property:configuration=' + strBuildType
        strVC += ' /maxcpucount /clp:WarningsOnly;ErrorsOnly;ForceNoAlign '
        iRC, strConsoleOutput = VistaPythonCommon.SysCall(strVC)
        sys.stdout.write(strConsoleOutput)
        sys.stdout.flush()
        
def getVCvarsall( strVCVersion ): #  or 11
    strProgramPath=os.environ['ProgramFiles(x86)']
    
    if "09" == strVCVersion:
        return 'call "'+strProgramPath+'\\Microsoft Visual Studio 9.0\\VC\\vcvarsall.bat" x86'
    elif int(strVCVersion) > 9:
        return 'call "'+strProgramPath+'\\Microsoft Visual Studio '+strVCVersion+'.0\\VC\\vcvarsall.bat" x86'
    else:
        sys.stderr.write('\n\n*** ERROR *** Unsupported MSVC Version: '+strVCVersion)
        ExitGently(-1)
        
def getMSVCGeneratorString(strCompiler, strVCVersion):
    strMSVCGenerator='Visual Studio '
    strArch=''
    strVersion=''
    
    if '64BIT' in strCompiler:
        strArch += ' Win64'    
    
    if "09" == strVCVersion:
        strVersion='9 2008'
    elif "08" == strVCVersion:
        strVersion='8 2005'
    elif int(strVCVersion) > 9:
        strVersion=strVCVersion
    else:
        sys.stderr.write('\n\n*** ERROR *** Unsupported MSVC Version: '+strVCVersion)
        ExitGently(-1)
    return strMSVCGenerator+strVersion+strArch

def getProjextFileEnding( strVCVersion ):
    if "09" == strVCVersion:
        return 'vcproj'
    else:
        return 'vcxproj'

    