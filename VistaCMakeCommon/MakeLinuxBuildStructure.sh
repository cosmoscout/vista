#!/bin/bash

function usage()
{
	echo "Usage:   MakeLinuxBuildStructure.sh [options]"
	echo "   Creates cmake configs for both debug and release builds"
	echo "   and creates a Makefile script to make / run cmake on both"
	echo "   builds simultaniously"
    echo "Options"
	echo "    -h, --help"
    echo "   ( cmake | ccmake | cmake-gui ) - specify the cmake binary to use"
	echo "                                    when calling RerunCMake.sh"
	echo "                                    default: ccmake"
	echo "   -releasedir DIR                - name of the release subdirectory"
	echo "                                    default: release"
    echo "   -debugdir DIR                  - name of the debug subdirectory"
	echo "                                    default: debug"
	echo "   -topdir DIR                    - toplevel subdir for build structure"
	echo "                                    default: build - use \"\" for empty dir"
	echo "   -output TYPE                   - output of the toplevel makefile, can be"
	echo "                                      silent: no output"
	echo "                                      text: print intro/outro to configs"
	echo "                                      color: like standard, but with colors [default]"
    echo ""
}

# CUSTOMIZABLE options

# GENERAL_BUILD_DIR may be empty, the others have to be filled
GENERAL_BUILD_DIR=build_LINUX.X86_64
DEBUG_BUILD_DIR=debug
RELEASE_BUILD_DIR=release
CMAKE_BINARY=ccmake
ABSOLUTE_SOURCE_DIR=`pwd`

COLOR_BUILD='\\033[0;1;32m' # bright green
COLOR_SUCCEED='\\033[0;1;32m' # bright green
COLOR_FAILED='\\033[0;1;31m' # bright red
COLOR_RESET='\\033[0m' # reset color, only change if you want to enforce colors staying set afterwards

USE_TEXT_OUTPUT=1

while [ "$1" != "" ]; do
	case $1 in
        cmake-gui | cmake | ccmake )
            CMAKE_BINARY=$1
            ;;
		-h | --help )
            usage
			exit
            ;;
		-releasedir )
            RELEASE_BUILD_DIR=$2
			shift
            ;;
		-debugdir )
            DEBUG_BUILD_DIR=$2
			shift
            ;;
		-topdir )
            GENERAL_BUILD_DIR=$2
			shift
            ;;
		-output )
            if [ "$2" == "silent" ]; then
				USE_TEXT_OUTPUT=0
			elif [ "$2" == "text" ]; then
				COLOR_BUILD=
				COLOR_SUCCEED=
				COLOR_FAILED=
				COLOR_RESET=
			elif [ "$2" != "color" ]; then
				usage
				exit
			fi
			shift
            ;;
        *)
            echo Unexpected parameter $1
			usage
			exit
            ;;
    esac
	shift
done

ABSOLUTE_RELEASE_DIR=${ABSOLUTE_SOURCE_DIR}
ABSOLUTE_DEBUG_DIR=${ABSOLUTE_SOURCE_DIR}

if [ ! "$GENERAL_BUILD_DIR" == "" ]; then
	echo GENERAL_BUILD_DIR=$GENERAL_BUILD_DIR
	if [ ! -d $GENERAL_BUILD_DIR ]; then
		mkdir $GENERAL_BUILD_DIR
	fi
	cd $GENERAL_BUILD_DIR
	ABSOLUTE_RELEASE_DIR=$ABSOLUTE_RELEASE_DIR/$GENERAL_BUILD_DIR
	ABSOLUTE_DEBUG_DIR=$ABSOLUTE_DEBUG_DIR/$GENERAL_BUILD_DIR
fi

ABSOLUTE_RELEASE_DIR=$ABSOLUTE_RELEASE_DIR/$RELEASE_BUILD_DIR
ABSOLUTE_DEBUG_DIR=$ABSOLUTE_DEBUG_DIR/$DEBUG_BUILD_DIR

if [ ! -d $RELEASE_BUILD_DIR ]; then
	mkdir $RELEASE_BUILD_DIR
fi
if [ ! -d $DEBUG_BUILD_DIR ]; then
	mkdir $DEBUG_BUILD_DIR
fi

cd $ABSOLUTE_RELEASE_DIR

cmake -DCMAKE_BUILD_TYPE=Release $ABSOLUTE_SOURCE_DIR

cd $ABSOLUTE_DEBUG_DIR

cp -R $ABSOLUTE_RELEASE_DIR/CMakeFiles $ABSOLUTE_DEBUG_DIR
sed "s|$ABSOLUTE_RELEASE_DIR|$ABSOLUTE_DEBUG_DIR|g" $ABSOLUTE_RELEASE_DIR/CMakeCache.txt > $ABSOLUTE_DEBUG_DIR/CMakeCache.txt

cmake -DCMAKE_BUILD_TYPE=Debug $ABSOLUTE_SOURCE_DIR

cd ..

cp ${VISTA_CMAKE_COMMON}/LinuxBuildStructureMakefile .
mv LinuxBuildStructureMakefile Makefile.tmp1
sed "s|@RELEASE_DIR@|$ABSOLUTE_RELEASE_DIR|g" Makefile.tmp1 > Makefile.tmp2
sed "s|@DEBUG_DIR@|$ABSOLUTE_DEBUG_DIR|g" Makefile.tmp2 > Makefile.tmp3
if [ "$USE_TEXT_OUTPUT" == "1" ]; then
	sed "s|@COLOR_BUILD@|$COLOR_BUILD|g" Makefile.tmp3 > Makefile.tmp4
	sed "s|@COLOR_SUCCEED@|$COLOR_SUCCEED|g" Makefile.tmp4 > Makefile.tmp5
	sed "s|@COLOR_FAILED@|$COLOR_FAILED|g" Makefile.tmp5 > Makefile.tmp6
	sed "s|@COLOR_RESET@|$COLOR_RESET|g" Makefile.tmp6 > Makefile
else
	sed "s/echo.*&& exit/exit/g" Makefile.tmp3 > Makefile.tmp4
	sed "/COLOR_/d" Makefile.tmp4 > Makefile
fi
rm Makefile.tmp*

cp ${VISTA_CMAKE_COMMON}/LinuxBuildStructureRerunCMake .
mv LinuxBuildStructureRerunCMake RerunCMake.sh.tmp1
sed "s|@RELEASE_DIR@|$ABSOLUTE_RELEASE_DIR|g" RerunCMake.sh.tmp1 > RerunCMake.sh.tmp2
sed "s|@DEBUG_DIR@|$ABSOLUTE_DEBUG_DIR|g" RerunCMake.sh.tmp2 > RerunCMake.sh.tmp3
sed "s|@CMAKE_CONFIG_BINARY@|$CMAKE_BINARY|g" RerunCMake.sh.tmp3 > RerunCMake.sh.tmp4
sed "s|@TO_SOURCE_DIR@|$ABSOLUTE_SOURCE_DIR|g" RerunCMake.sh.tmp4 > RerunCMake.sh
rm RerunCMake.sh.tmp*
chmod ug+x RerunCMake.sh