@echo off
cls
	cmake -S . -B build -A x64,version=8.1 -T v142 -DBUILD_GROUP=game -DCMAKE_BUILD_TYPE="Release" -DBUILD_64BIT=ON -DBUILD_VPHYSICS=ON -DBUILD_VPHYSICS_TYPE="IVP" -DCMAKE_SYSTEM_VERSION="8.1" -DBUILD_FOLDER=tf2classic
pause