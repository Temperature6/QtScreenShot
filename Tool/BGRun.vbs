set ws = createobject("wscript.shell")
ws.currentdirectory = "D:\Qt\QtApplication\QScreenShot"
CurrentPath = createobject("Scripting.FileSystemObject").GetFolder(".").Path 

dim program  
program=".\ScreenShot.exe -background" '你的exe路径
set Wshell=CreateObject("Wscript.Shell")  
set oexec=Wshell.Exec(program) 