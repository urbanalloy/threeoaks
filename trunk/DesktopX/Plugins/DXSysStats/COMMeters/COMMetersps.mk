
COMMetersps.dll: dlldata.obj COMMeters_p.obj COMMeters_i.obj
	link /dll /out:COMMetersps.dll /def:COMMetersps.def /entry:DllMain dlldata.obj COMMeters_p.obj COMMeters_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del COMMetersps.dll
	@del COMMetersps.lib
	@del COMMetersps.exp
	@del dlldata.obj
	@del COMMeters_p.obj
	@del COMMeters_i.obj
