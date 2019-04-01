# Linux build script designed for jenkins
# $Id

import  sys, os, time, shutil, VistaPythonCommon

#build project in current directory                                                                                                                 
def BuildIt(strBuildType='Default', strCompiler = 'GCC_DEFAULT', strCMakeVariables = '', bDeleteCMakeCache = True, strBuildFolder='JenkinsDefault', bRunTests = False, bInstall = False, bPackage = False):
    
    #make sure we are on linux system
    if sys.platform != 'linux2':
        VistaPythonCommon.ExitError('\n\n*** ERROR *** Linux build on non Linux system\n\n',-1)
    
    sys.stdout.write('Buildtype: ' + strBuildType + '\n')
    sys.stdout.write('Compiler: ' + strCompiler + '\n')
    sys.stdout.write('CMake Definitions: ' + strCMakeVariables + '\n')
    
    iRC, strUsername = VistaPythonCommon.SysCall('whoami',ExitOnError = False)
    sys.stdout.write('Username: '+strUsername+'\n')
    iRC, strShell = VistaPythonCommon.SysCall('echo $SHELL',ExitOnError = False)
    sys.stdout.write('Shell: '+strShell+'\n')
    iRC, strHostname = VistaPythonCommon.SysCall('echo $HOSTNAME',ExitOnError = False)
    sys.stdout.write('Hostname: '+strHostname+'\n')
    iRC, strPythonVersion = VistaPythonCommon.SysCall('python --version',ExitOnError = False)
    sys.stdout.write('PythonVersion: '+strPythonVersion+'\n')    
    sys.stdout.write('\n')
    sys.stdout.flush()
    
    if True == bRunTests:
        sys.stdout.write('make tests will be executed\n')
    if True == bInstall:
        sys.stdout.write('make install will be executed\n')
    sys.stdout.flush()
    
    fStartTime=time.time()
        
 
    
    # pretty ugly we switch standard build and jenkins by the buildfolder and then ignore it ...
    # but that works for windows. the reason behind this is, that you need a folder for debug and one for release anyway
    if strBuildFolder is 'JenkinsDefault':        
        MakeJenkinsBuild(strBuildType, strCompiler, strCMakeVariables, bDeleteCMakeCache, bRunTests, bInstall, bPackage)
    else:
        MakeLinuxStandardBuild(strCompiler,bDeleteCMakeCache)
    
    sys.stdout.write("\n\nElapsed time: " + str(int(time.time()-fStartTime)) + " seconds\n")
    sys.stdout.flush()
    
    
def MakeLinuxStandardBuild(strCompiler,bDeleteCMakeCache):
    strBuildFolder='build_LINUX.X86_64' 
    strConsoleOutput=''
    if not os.path.exists(strBuildFolder):
        strCall = GetCompilerEnvCall(strCompiler, '$VISTA_CMAKE_COMMON/MakeLinuxBuildStructure.sh')
        iRC, strConsoleOutput = VistaPythonCommon.SysCall(strCall)
    else:
       if True == bDeleteCMakeCache:
            shutil.rmtree(strBuildFolder,True)#clean cmake build
            strCall = GetCompilerEnvCall(strCompiler, '$VISTA_CMAKE_COMMON/MakeLinuxBuildStructure.sh')
            iRC, strConsoleOutput = VistaPythonCommon.SysCall(strCall)
    
    sys.stdout.write(strConsoleOutput)
    sys.stdout.flush()
    os.chdir(os.path.join(os.getcwd(), strBuildFolder))
    strCall = GetCompilerEnvCall(strCompiler, 'make -j')
    iRC, strConsoleOutput = VistaPythonCommon.SysCall(strCall)        
    sys.stdout.write(strConsoleOutput)
    sys.stdout.flush()
    
def MakeJenkinsBuild(strBuildType, strCompiler, strCMakeVariables, bDeleteCMakeCache, bRunTests, bInstall, bPackage):
    strSysName = os.uname()[0].upper()
    strMachine = os.uname()[4].upper()
    strBuildFolder = 'build.' + strSysName + '.' + strMachine + '.' + strCompiler + '.' + strBuildType
    if strBuildType is 'Default':
        VistaPythonCommon.ExitError('right now BuildType Default is not supported for Jenkins and Linux',-1)
        
    if not os.path.exists(strBuildFolder):
        VistaPythonCommon.SysCall('mkdir ' + strBuildFolder)
    else:
       if True == bDeleteCMakeCache:
            shutil.rmtree(strBuildFolder)#clean cmake build
            VistaPythonCommon.SysCall('mkdir ' + strBuildFolder)

    strBasePath = os.getcwd()
    os.chdir(os.path.join(strBasePath, strBuildFolder))
            
    #check compiler and cmake if we are on gpucluster
    strCompilerEnv = GetCompilerEnvCall(strCompiler,'')
    sys.stdout.write('Compiler Environment:\n ' + strCompilerEnv + '\n\nExecution cmake:\n')
    strCMakeEnv = GetCMakeEnvCall()
    VistaPythonCommon.SysCall(strCMakeEnv + 'cmake -version',ExitOnError = False)
    sys.stdout.flush()
    
    #configure cmake
    strCMakeCmd = GetCompilerEnvCall(strCompiler, strCMakeEnv + 'cmake -DCMAKE_BUILD_TYPE=' + strBuildType + ' ' + strCMakeVariables + ' ' + os.path.join(strBasePath))
    iRC, strConsoleOutput = VistaPythonCommon.SysCall(strCMakeCmd)
    sys.stdout.write(strConsoleOutput)
    sys.stdout.flush()

    if VistaPythonCommon.CheckForCMakeError(strConsoleOutput):        
        VistaPythonCommon.ExitError('\n\n*** ERROR *** Cmake failed to generate configuration\n\n',-1)

    #log gcc version
    strCXXVersion = 'g++ -v'
    if (0 == os.uname()[1].find('linuxgpu')):
        strCXXVersion = '$CXX -v'
    strCompilerCheck = GetCompilerEnvCall(strCompiler, strCXXVersion)
    iRC, strConsoleOutput = VistaPythonCommon.SysCall(strCompilerCheck)
    sys.stdout.write(strConsoleOutput)
    sys.stdout.flush()
        
    #make it
    strMakeCmd = GetCompilerEnvCall(strCompiler, 'make -j')
    iRC, strConsoleOutput = VistaPythonCommon.SysCall('who | wc -l')
    if int(strConsoleOutput) > 1:
        strMakeCmd = GetCompilerEnvCall(strCompiler, 'make -j2')
    
    iRC, strConsoleOutput = VistaPythonCommon.SysCall(strMakeCmd)
    sys.stdout.write(strConsoleOutput)
    sys.stdout.flush()
    
    #execute tests
    if True == bRunTests:
        strTestCmd = GetCompilerEnvCall(strCompiler, 'make test_outputonfailure')
        iRC, strConsoleOutput = VistaPythonCommon.SysCall(strTestCmd)
        #if 0 != iRC:
        #    iRC, strConsoleOutput = VistaPythonCommon.SysCall(strCompilerEnv + 'make test_verbose',ExitOnError = True)
        sys.stdout.write(strConsoleOutput)
        sys.stdout.flush()
        
    #install
    if True == bInstall:
        strInstallCall = GetCompilerEnvCall(strCompiler, 'make install')
        iRC, strConsoleOutput = VistaPythonCommon.SysCall(strInstallCall)
        sys.stdout.write(strConsoleOutput)
        sys.stdout.flush()

    # create package
    if True == bPackage:
        strPackageCall = GetCompilerEnvCall(strCompiler, 'make package')
        iRC, strConsoleOutput = VistaPythonCommon.SysCall(strPackageCall)
        sys.stdout.write(strConsoleOutput)
        sys.stdout.flush()
        
        
        
#since every syscall opens a new shell we have to set environment each time :(
def GetCompilerEnvCall(strCompiler, strCommand):
    if (0 == os.uname()[1].find('linuxgpu')):
        strDefaultModules = 'module unload gcc;module unload intel;'
        liCompilerDef = strCompiler.split('_', 1)
        if (len(liCompilerDef) != 0):
            if 'INTEL' in liCompilerDef[0]:
                return strDefaultModules+'module load intel;' + strCommand
            elif 'GCC' in liCompilerDef[0]:
                if (len(liCompilerDef) > 1):
                    if 'DEFAULT' in liCompilerDef[1]:
                        return strDefaultModules+'module load gcc;' + strCommand
                    else:
                        return strDefaultModules+'module load gcc/' + liCompilerDef[1] + ';' +strCommand
            else:
                sys.stderr.write('unsupported compiler-version: ' + strCompiler)
                VistaPythonCommon.ExitGently(-1)
        else:
            return strCommand
    else:
        liCompilerDef = strCompiler.split('_', 1)
        if (len(liCompilerDef) != 0):
            if 'GCC' in liCompilerDef[0] and '4.8' in liCompilerDef[1]:
                iRC, strConsoleOutput = VistaPythonCommon.SysCall('scl -l',ExitOnError = False)
                if (0 == strConsoleOutput.find('devtoolset-2')):
                    return "scl enable devtoolset-2 '" + strCommand + "'"
                else:
                    sys.stderr.write('devtoolset for gcc-4.8 not installed.')
                    VistaPythonCommon.ExitGently(-1)
            else:
                return strCommand
        else:
            return strCommand

#since every syscall opens a new shell we have to set environment each time :(
def GetCMakeEnvCall():
    if (0 == os.uname()[1].find('linuxgpu')):
        return 'module load cmake;'
    else:
        return ''
    
