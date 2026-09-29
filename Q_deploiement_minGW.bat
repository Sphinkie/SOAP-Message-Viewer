@echo OFF
@echo --------------------------------------------------
@echo Run me in Qt Creator terminal.. "Run Environment"
@echo --------------------------------------------------

setlocal 
set "AppName=SOAP-Message-Viewer"
set "SourceDir=.\build\Desktop_Qt_6_11_1_MinGW_64_bit_Release\release"
REM set "TargetDir=.\Installeur\packages\sphinkie.%AppName%\data\"
set TargetDir=.\dist

@echo.
@echo --------------------------------------------------
@echo Copie des binaries externes (et creation target folder)
@echo --------------------------------------------------
REM xcopy .\Bin\*.exe %TargetDir%\Bin /I /Y
REM xcopy .\Data %TargetDir%\Data /S /I /Y

@echo.
@echo --------------------------------------------------
@echo Copie de l'executable %AppName%
@echo --------------------------------------------------
xcopy %SourceDir%\%AppName%.exe %TargetDir% /Y

@echo ON
@echo.
@echo --------------------------------------------------
@echo Deploiment des librairies pour MINGV LLVM
@echo --------------------------------------------------
REM Si QML, ajouter:  --qmldir .\Sources\Qml
windeployqt --no-translations %TargetDir%

@echo -------------------------------------------------
@echo Done

