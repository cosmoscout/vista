
set "oldpath_backup=%path%"

Set "path=%path%;%VISTA_ROOT%\lib;%OPENSG_ROOT%\lib"

set "BobPath=Bob_ApplicationDemo"
set "AlicePath=Alice_shellDemo"

start /D%~d0%~p0%BobPath% "" "./Bob_ApplicationDemo/17BobD.exe"

echo "Waiting 10secs to launch Alice"
sleep 10

start /D%~d0%~p0%AlicePath% "" "./Alice_shellDemo/17AliceD.exe"

set "path=%oldpath_backup%"

:end