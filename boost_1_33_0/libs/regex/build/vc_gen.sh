#! /bin/bash

libname=""
src=""
header=""
all_dep=""

# current makefile:
out=""
# temporary file:
tout=""
# install target temp file:
iout=""
# debug flag:
debug="no"
# compile options:
opts=""
# main output sub-directory:
subdir=""
stlport_suffix=""
# extra debug /RTc options:
debug_extra=""

function vc6_gen_lib()
{
	all_dep="$all_dep $libname""_dir ./$subdir$stlport_suffix/$libname.lib"
	echo "	copy $subdir$stlport_suffix\\$libname.lib "'"$'"(MSVCDIR)\\lib"'"' >> $iout
	if test $debug == "yes"; then
		echo "	copy $subdir$stlport_suffix\\$libname.pdb "'"$'"(MSVCDIR)\\lib"'"' >> $iout
	fi
#
# set up section comments:
	cat >> $tout << EOF
########################################################
#
# section for $libname.lib
#
########################################################
EOF
#
#	process source files:
	all_obj=""
	for file in $src
	do
		obj=`echo "$file" | sed 's/.*src\/\(.*\)cpp/\1obj/g'`
		obj="$subdir$stlport_suffix/$libname/$obj"
		all_obj="$all_obj $obj"
		echo "$obj: $file \$(ALL_HEADER)" >> $tout
		echo "	cl \$(INCLUDES) $opts \$(CXXFLAGS) \$(ICU_COMPILE_OPTS) -Y- -Fo./$subdir$stlport_suffix/$libname/ -Fd$subdir$stlport_suffix/$libname.pdb $file" >> $tout
		echo "" >> $tout
	done
#
#	 now for the directories for this library:
	echo "$libname"_dir : >> $tout
	echo "	@if not exist \"$subdir$stlport_suffix\\$libname\\\$(NULL)\" mkdir $subdir$stlport_suffix\\$libname" >> $tout
	echo "" >> $tout
#
#	 now for the clean options for this library:
	all_clean="$all_clean $libname""_clean"
	echo "$libname"_clean : >> $tout
	echo "	del $subdir$stlport_suffix\\$libname\\"'*.obj' >> $tout
	echo "	del $subdir$stlport_suffix\\$libname\\"'*.idb' >> $tout
	echo "	del $subdir$stlport_suffix\\$libname\\"'*.exp' >> $tout
	echo "	del $subdir$stlport_suffix\\$libname\\"'*.pch' >> $tout
	echo "" >> $tout
#
#	 now for the main target for this library:
	echo ./$subdir$stlport_suffix/$libname.lib : $all_obj >> $tout
	echo "	link -lib /nologo /out:$subdir$stlport_suffix/$libname.lib \$(XSFLAGS) $all_obj" >> $tout
	echo "" >> $tout
}

function vc6_gen_dll()
{
	all_dep="$all_dep $libname""_dir ./$subdir$stlport_suffix/$libname.lib"
	echo "	copy $subdir$stlport_suffix\\$libname.lib "'"$'"(MSVCDIR)\\lib"'"' >> $iout
	echo "	copy $subdir$stlport_suffix\\$libname.dll "'"$'"(MSVCDIR)\\bin"'"' >> $iout
	if test $debug == "yes"; then
		echo "	copy $subdir$stlport_suffix\\$libname.pdb "'"$'"(MSVCDIR)\\lib"'"' >> $iout
	fi
#
# set up section comments:
	cat >> $tout << EOF
########################################################
#
# section for $libname.lib
#
########################################################
EOF
#
#	process source files:
	all_obj=""
	for file in $src
	do
		obj=`echo "$file" | sed 's/.*src\/\(.*\)cpp/\1obj/g'`
		obj="$subdir$stlport_suffix/$libname/$obj"
		all_obj="$all_obj $obj"
		echo "$obj: $file \$(ALL_HEADER)" >> $tout
		echo "	cl \$(INCLUDES) $opts \$(CXXFLAGS) \$(ICU_COMPILE_OPTS) -Y- -Fo./$subdir$stlport_suffix/$libname/ -Fd$subdir$stlport_suffix/$libname.pdb $file" >> $tout
		echo "" >> $tout
	done
#
#	 now for the directories for this library:
	echo "$libname"_dir : >> $tout
	echo "	@if not exist \"$subdir$stlport_suffix\\$libname\\\$(NULL)\" mkdir $subdir$stlport_suffix\\$libname" >> $tout
	echo "" >> $tout
#
#	 now for the clean options for this library:
	all_clean="$all_clean $libname""_clean"
	echo "$libname"_clean : >> $tout
	echo "	del $subdir$stlport_suffix\\$libname\\"'*.obj' >> $tout
	echo "	del $subdir$stlport_suffix\\$libname\\"'*.idb' >> $tout
	echo "	del $subdir$stlport_suffix\\$libname\\"'*.exp' >> $tout
	echo "	del $subdir$stlport_suffix\\$libname\\"'*.pch' >> $tout
	echo "" >> $tout
#
#	 now for the main target for this library:
	echo ./$subdir$stlport_suffix/$libname.lib : $all_obj >> $tout
	echo "	link kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:\"$subdir$stlport_suffix/$libname.pdb\" /debug /machine:I386 /out:\"$subdir$stlport_suffix/$libname.dll\" /implib:\"$subdir$stlport_suffix/$libname.lib\" /LIBPATH:\$(STLPORT_PATH)\\lib \$(XLFLAGS) \$(ICU_LINK_OPTS) $all_obj" >> $tout
	echo "" >> $tout
}

is_stlport="no"

function vc6_gen()
{
	debug="no"
	tout="temp"
	iout="temp_install"
	all_dep="main_dir"
	all_clean=""
	echo > $out
	echo > $tout
	rm -f $iout
	stlport_suffix=""
	
	if test ${subdir} != "vc8" ; then
	libname="libboost_regex-${subdir}-s-${boost_version}"
	opts='/c /nologo /ML /W3 '$EH_OPTS' /O2 '$PROC_OPTS' /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD '"$release_extra"' '
	vc6_gen_lib
	fi
	
	libname="libboost_regex-${subdir}-mt-s-${boost_version}"
	opts='/nologo /MT /W3 '$EH_OPTS' /O2 '$PROC_OPTS' /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD '"$release_extra"' /c'
	vc6_gen_lib
	
	if test ${subdir} != "vc8" ; then
	debug="yes"
	libname="libboost_regex-${subdir}-sgd-${boost_version}"
	opts='/nologo /MLd /W3 /Gm '$EH_OPTS' /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /FD '"$debug_extra"' /c '
	vc6_gen_lib
	fi
	
	libname="libboost_regex-${subdir}-mt-sgd-${boost_version}"
	opts='/nologo /MTd /W3 /Gm '$EH_OPTS' /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /FD '"$debug_extra"' /c'
	vc6_gen_lib
	
	libname="boost_regex-${subdir}-mt-gd-${boost_version}"
	opts='/nologo /MDd /W3 /Gm '$EH_OPTS' /Zi /Od /I../../../ /D_DEBUG /DBOOST_REGEX_DYN_LINK /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD '"$debug_extra"' /c'
	vc6_gen_dll
	
	debug="no"
	opts='/nologo /MD /W3 '$EH_OPTS' /O2 '$PROC_OPTS' /GF /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD '"$release_extra"' /c'
	libname="boost_regex-${subdir}-mt-${boost_version}"
	vc6_gen_dll
	
	debug="no"
	opts='/nologo /MD /W3 '$EH_OPTS' /O2 '$PROC_OPTS' /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD '"$release_extra"' /c'
	libname="libboost_regex-${subdir}-mt-${boost_version}"
	vc6_gen_lib
	
	debug="yes"
	libname="libboost_regex-${subdir}-mt-gd-${boost_version}"
	opts='/nologo /MDd /W3 /Gm '$EH_OPTS' /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD '"$debug_extra"' /c'
	vc6_gen_lib

	if test ${subdir} != "vc8" ; then
	   VC8_CHECK="MSVCDIR=\$(VS80COMNTOOLS)..\\..\\VC"
   fi
   	
	cat > $out << EOF
#
# auto generated makefile for VC6 compiler
#
# usage:
# make
#   brings libraries up to date
# make install
#   brings libraries up to date and copies binaries to your VC6 /lib and /bin directories (recomended)
#

#
# path to ICU library installation goes here:
#
ICU_PATH=
#
# Add additional compiler options here:
#
CXXFLAGS=
#
# Add additional include directories here:
#
INCLUDES=
#
# add additional linker flags here:
#
XLFLAGS=
#
# add additional static-library creation flags here:
#
XSFLAGS=

!IF "\$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

$VC8_CHECK

!IF "\$(MSVCDIR)" == ""
!ERROR Variable MSVCDIR not set.
!ENDIF

!IF "\$(ICU_PATH)" == ""
ICU_COMPILE_OPTS=
ICU_LINK_OPTS=
!MESSAGE Building Boost.Regex without ICU / Unicode support:
!MESSAGE Hint: set ICU_PATH on the nmake command line to point 
!MESSAGE to your ICU installation if you have one.
!ELSE
ICU_COMPILE_OPTS= -DBOOST_HAS_ICU=1 -I\$(ICU_PATH)\\include
ICU_LINK_OPTS= /LIBPATH:\$(ICU_PATH)\\lib icuin.lib icuuc.lib
!MESSAGE Building Boost.Regex with ICU in \$(ICU_PATH)
!ENDIF

EOF
	echo "" >> $out
	echo "ALL_HEADER=$header" >> $out
	echo "" >> $out
	echo "all : $all_dep" >> $out
	echo >> $out
	echo "clean : $all_clean" >> $out
	echo >> $out
	echo "install : all" >> $out
	cat $iout >> $out
	echo >> $out
	echo main_dir : >> $out
	echo "	@if not exist \"$subdir$stlport_suffix\\\$(NULL)\" mkdir $subdir$stlport_suffix" >> $out
	echo "" >> $out

	cat $tout >> $out
}

function vc6_stlp_gen()
{
	debug="no"
	tout="temp"
	iout="temp_install"
	all_dep="main_dir"
	all_clean=""
	echo > $out
	echo > $tout
	rm -f $iout
	stlport_suffix="-stlport"
	
	libname="libboost_regex-${subdir}-mt-sp-${boost_version}"
	opts='/nologo /MT /W3 '$EH_OPTS' /O2 '$PROC_OPTS' /GF /Gy /I$(STLPORT_PATH)\stlport /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB '"$release_extra"' /c'
	vc6_gen_lib
	
	debug="true"
	libname="libboost_regex-${subdir}-mt-sgdp-${boost_version}"
	opts='/nologo /MTd /W3 /Gm '$EH_OPTS' /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB '"$debug_extra"' /c'
	#vc6_gen_lib
	
	libname="boost_regex-${subdir}-mt-gdp-${boost_version}"
	opts='/nologo /MDd /W3 /Gm '$EH_OPTS' /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_DYN_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL '"$debug_extra"' /c'
	#vc6_gen_dll
	
	debug="no"
	opts='/nologo /MD /W3 '$EH_OPTS' /O2 '$PROC_OPTS' /GF /I$(STLPORT_PATH)\stlport /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL '"$release_extra"' /c'
	libname="boost_regex-${subdir}-mt-p-${boost_version}"
	vc6_gen_dll
	
	debug="no"
	opts='/nologo /MD /W3 '$EH_OPTS' /O2 '$PROC_OPTS' /GF /Gy /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL '"$release_extra"' /c'
	libname="libboost_regex-${subdir}-mt-p-${boost_version}"
	vc6_gen_lib
	
	debug="true"
	libname="libboost_regex-${subdir}-mt-gdp-${boost_version}"
	opts='/nologo /MDd /W3 /Gm '$EH_OPTS' /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL '"$debug_extra"' /c'
	#vc6_gen_lib

#  debug STLPort mode:
	debug="yes"
	opts='/nologo /MDd /W3 /Gm '$EH_OPTS' /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_DYN_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL '"$debug_extra"' /c'
	libname="boost_regex-${subdir}-mt-gdp-${boost_version}"
	vc6_gen_dll
	libname="libboost_regex-${subdir}-mt-sgdp-${boost_version}"
	opts='/nologo /MTd /W3 /Gm '$EH_OPTS' /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /D__STL_DEBUG /D_STLP_DEBUG /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB '"$debug_extra"' /c'
	vc6_gen_lib
	opts='/nologo /MDd /W3 /Gm '$EH_OPTS' /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL '"$debug_extra"' /c'
	libname="libboost_regex-${subdir}-mt-gdp-${boost_version}"
	vc6_gen_lib
	
	cat > $out << EOF
#
# auto generated makefile for VC6+STLPort
#
# usage:
# make
#   brings libraries up to date
# make install
#   brings libraries up to date and copies binaries to your VC6 /lib and /bin directories (recomended)
#

#
# ICU setup:
#
ICU_PATH=
#
# Add additional compiler options here:
#
CXXFLAGS=
#
# Add additional include directories here:
#
INCLUDES=
#
# add additional linker flags here:
#
XLFLAGS=
#
# add additional static-library creation flags here:
#
XSFLAGS=

!IF "\$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF "\$(MSVCDIR)" == ""
!ERROR Variable MSVCDIR not set.
!ENDIF

!IF "\$(STLPORT_PATH)" == ""
!ERROR Variable STLPORT_PATH not set.
!ENDIF

!IF "\$(ICU_PATH)" == ""
ICU_COMPILE_OPTS=
ICU_LINK_OPTS=
!MESSAGE Building Boost.Regex without ICU / Unicode support:
!MESSAGE Hint: set ICU_PATH on the nmake command line to point 
!MESSAGE to your ICU installation if you have one.
!ELSE
ICU_COMPILE_OPTS= -DBOOST_HAS_ICU=1 -I\$(ICU_PATH)\\include
ICU_LINK_OPTS= /LIBPATH:\$(ICU_PATH)\\lib icuin.lib icuuc.lib
!MESSAGE Building Boost.Regex with ICU in \$(ICU_PATH)
!ENDIF

EOF
	echo "" >> $out
	echo "ALL_HEADER=$header" >> $out
	echo "" >> $out
	echo "all : $all_dep" >> $out
	echo >> $out
	echo "clean : $all_clean" >> $out
	echo >> $out
	echo "install : stlport_check all" >> $out
	cat $iout >> $out
	echo >> $out
	echo main_dir : >> $out
	echo "	@if not exist \"$subdir$stlport_suffix\\\$(NULL)\" mkdir $subdir$stlport_suffix" >> $out
	echo "" >> $out
	echo 'stlport_check : $(STLPORT_PATH)\stlport\string' >> $out
	echo "	echo" >> $out
	echo "" >> $out

	cat $tout >> $out
}


. common.sh

#
# options that change with compiler version:
#
EH_OPTS="/GX"
PROC_OPTS="/GB"

#
# generate vc6 makefile:
debug_extra="$EH_OPTS"
out="vc6.mak"
subdir="vc6"
vc6_gen
#
# generate vc6-stlport makefile:
is_stlport="yes"
out="vc6-stlport.mak"
no_single="yes"
subdir="vc6"
vc6_stlp_gen
#
# generate vc7 makefile:
debug_extra="$EH_OPTS /RTC1 /Zc:wchar_t"
release_extra="/Zc:wchar_t"
is_stlport="no"
out="vc7.mak"
no_single="no"
subdir="vc7"
vc6_gen
#
# generate vc7-stlport makefile:
is_stlport="yes"
out="vc7-stlport.mak"
no_single="yes"
subdir="vc7"
vc6_stlp_gen
#
# generate vc71 makefile:
is_stlport="no"
out="vc71.mak"
no_single="no"
subdir="vc71"
vc6_gen
#
# generate vc71-stlport makefile:
is_stlport="yes"
out="vc71-stlport.mak"
no_single="yes"
subdir="vc71"
vc6_stlp_gen
#
# generate vc8 makefile:
EH_OPTS="/EHsc"
PROC_OPTS=""
debug_extra="$EH_OPTS"
is_stlport="no"
out="vc8.mak"
no_single="no"
subdir="vc80"
vc6_gen


#
# remove tmep files;
rm -f $tout $iout








