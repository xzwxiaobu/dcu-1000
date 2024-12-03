@echo off
REM usage: cutBin inputBinFile outputBinFile startAddr Length
REM usage: Bin2C  inputBinFile outputCFile   startAddr Length flashBits

:CopyHex

REM ===== COPY BOOTLOADER V2.0 ===== 
rem copy E:\fangda\lly\Project\_UniversalPCBSoft_\PIC24\Bootloader\PIC24_Bootloader\V2.0\PIC24_Bootloader.X\dist\default\production\PIC24_Bootloader.X.production.hex P24src_bl.hex


REM ===== COPY BOOTLOADER V2.01 ===== 
rem copy E:\fangda\lly\Project\_UniversalPCBSoft_\PIC24\Bootloader\PIC24_Bootloader\V2.01\PIC24_Bootloader.X\dist\default\production\PIC24_Bootloader.X.production.hex P24src_bl.hex

rem IF %ERRORLEVEL% GTR 0 GOTO err_exit

REM ===== COPY APP ===== 
copy E:\fangda\lly\Project\QingDao4\soft\QD4_PEDC_MONITOR_V1_PIC24\V_1_0\QD4_PEDC_MON_PIC24_APP_V1_0.X\dist\default\production\QD4_PEDC_MON_PIC24_APP_V1_0.X.production.hex P24src_app.hex
IF %ERRORLEVEL% GTR 0 GOTO err_exit


REM ===== FILE PROC ===== 
:HexToBin
hex2bin    P24src_bl.hex
hex2bin    P24src_app.hex


:CutBinFile
cutBin.exe P24src_bl.bin      0_BL.tmp     0      8
cutBin.exe P24src_app.bin     8_App.tmp    8      0x7F8
cutBin.exe P24src_bl.bin      800_BL.tmp   0x800  0x1800
cutBin.exe P24src_app.bin     2000_App.tmp 0x2000 0x9000

copy 0_BL.tmp/b+8_App.tmp/b+800_BL.tmp/b+2000_App.tmp/b p24BL_App.cod/b
IF %ERRORLEVEL% GTR 0 GOTO err_exit

bin2c.exe    p24BL_App.cod p24BL_App.c 0 0xffff 24
bin2hex.exe  p24BL_App.cod QD4_PEDC_PIC24_BL_APP.hex


REM ===== delete temporary files ===== 
del *.tmp
del *.bin
del *.cod

@echo "finished!"

pause
exit

:err_exit

@echo "ERROR occurs!"
pause
exit
