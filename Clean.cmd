
::�򿪻���
@echo on

cd %cd%

::ɾ��*.dep�ļ�
del=/s /a /f "*.tmp"
del=/s /a /f "*.bak"
del=/s /a /f "*.dep"
del=/s /a /f "*.wanghao"

::ɾ�� "Exe" "List" "Obj""settings""Debug""DebugConfig"�ļ���
for /f "delims=" %%i in ('dir /ad /b /s "Exe" "List" "Obj" "settings" "Debug" "DebugConfig"') do (
   rd /s /q "%%i"
)

::ɾ�����ļ���
for /f "tokens=*" %%i in ('dir/s/b/ad^|sort /r') do rd "%%i"

exit