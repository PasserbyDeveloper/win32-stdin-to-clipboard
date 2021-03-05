@echo off
title Setting environment
SET "ENVSCRIPT=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%ENVSCRIPT%" (
	echo Error: Missing Microsoft Visual Studio 2019 enviroment 'vcvars64' script
	echo You may try to compile it but 'cl.exe' might fail
	pause
) else (
	call "%ENVSCRIPT%"
)
WHERE cl
IF %ERRORLEVEL% NEQ 0 (
	echo Error: Could not find 'cl.exe', the Microsoft C/C++ Optimizing Compiler used by this script to compile
	echo Ensure it is installed and available at path
	pause
	exit
)
start cmd /c code .
:start
title Compiling and Executing
cl /nologo /MD /Ob0 /O2 ./src/main.cpp /Fe"build/stdin-to-clipboard.exe" gdi32.lib user32.lib && echo. && "./build/stdin-to-clipboard.exe"
: /Dshlwapi /Dole32 /Doleprn /Doleaut32
title Waiting for main.cpp change
echo.
echo (development) Program exited with code %errorlevel%
echo (development) Waiting for source to update...
node -e "const f = './src/main.cpp'; const fs = require('fs'); let previous_date = (fs.statSync(f).mtimeMs); setInterval(function() { if (previous_date !== (fs.statSync(f).mtimeMs)){process.exit(0);}}, 100)"
goto start