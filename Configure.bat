@echo off

set build="build/Win64"
if not exist "%build%" mkdir %build%
pushd %build%

cmake -G "Visual Studio 14 2015 Win64" "%~dp0" 

popd
