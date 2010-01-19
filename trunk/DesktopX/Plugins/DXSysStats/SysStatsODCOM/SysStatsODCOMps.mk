
SysStatsODCOMps.dll: dlldata.obj SysStatsODCOM_p.obj SysStatsODCOM_i.obj
	link /dll /out:SysStatsODCOMps.dll /def:SysStatsODCOMps.def /entry:DllMain dlldata.obj SysStatsODCOM_p.obj SysStatsODCOM_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del SysStatsODCOMps.dll
	@del SysStatsODCOMps.lib
	@del SysStatsODCOMps.exp
	@del dlldata.obj
	@del SysStatsODCOM_p.obj
	@del SysStatsODCOM_i.obj
