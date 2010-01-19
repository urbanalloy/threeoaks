
COMControllersps.dll: dlldata.obj COMControllers_p.obj COMControllers_i.obj
	link /dll /out:COMControllersps.dll /def:COMControllersps.def /entry:DllMain dlldata.obj COMControllers_p.obj COMControllers_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del COMControllersps.dll
	@del COMControllersps.lib
	@del COMControllersps.exp
	@del dlldata.obj
	@del COMControllers_p.obj
	@del COMControllers_i.obj
