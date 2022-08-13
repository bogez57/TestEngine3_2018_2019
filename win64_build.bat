@echo off

REM %~dp0 represents the full directory path to your batch file
set cwd=%~dp0

REM Debug/Development build
set CommonCompilerFlags=-Gm- -MP -Zi -nologo -Oi -Od -Ob1 -WX -W3 -GR -EHa- -arch:AVX2 -wd4505 -wd4101 -wd4530 -w14700 -wd4996 -wd4172
set CommonLinkerFlags=-subsystem:windows -machine:x64 -incremental:no -nologo -opt:ref -debug:FULL -ignore:4099 -STACK:2000000

set PlatformIncludePaths=-I %cwd%third_party\boagz\include -I %cwd%third_party\boagz\src -I %cwd%third_party\glew-2.1.0\include -I %cwd%third_party\stb\include
set PlatformLibraryPaths=-LIBPATH:%cwd%third_party\glew-2.1.0\lib\win64-release
set PlatformImportLibraries=user32.lib OpenGL32.lib gdi32.lib xinput.lib Winmm.lib
set PlatformStaticLibraries=glew32s.lib

set GameIncludePaths=-I %cwd%third_party\boagz\include -I %cwd%third_party\boagz\src -I %cwd%third_party\stb\include

IF NOT EXIST build mkdir bin
pushd bin

cl /c ..\source\gamecode.cpp %CommonCompilerFlags% %GameIncludePaths% -DDEVELOPMENT_BUILD=1
link gamecode.obj -dll -PDB:gamecode_%random%.pdb -export:GameUpdate %CommonLinkerFlags%

REM Build exe
cl /c ..\source\win64_test.cpp %CommonCompilerFlags% %PlatformIncludePaths% -DDEVELOPMENT_BUILD=1 -DGLEW_STATIC=1
link win64_test.obj -OUT:win64_test.exe %CommonLinkerFlags% %PlatformLibraryPaths% %PlatformImportLibraries% %PlatformStaticLibraries%

popd







