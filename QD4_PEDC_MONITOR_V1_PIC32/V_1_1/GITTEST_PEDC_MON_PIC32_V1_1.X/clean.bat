@echo "==========编译后清理中间文件=========="
@echo "<<<Delete mid obj files: .\build>>>"

rd .\build /s/q
rd .\debug /s/q

@echo "<<<Delete elf files: .\dist\default\production\*.elf>>>"
del .\dist\default\production\*.elf


@echo "<<<Delete elf files: .\dist\default\production\*.map>>>"
del .\dist\default\production\*.map

@echo "<<<Delete elf files: .\dist\default\debug>>>"
rd .\dist\default\debug /s/q



@echo "==========完成清理=========="

