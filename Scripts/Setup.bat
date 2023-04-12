@echo off
rem this script is to setup the boost submodule + headers. you must run this before using the project.
git submodule update --init --recursive
cd external\boost
bootstrap.bat
b2 headers