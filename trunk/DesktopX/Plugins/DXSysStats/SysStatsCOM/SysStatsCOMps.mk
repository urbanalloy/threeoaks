
SysStatsCOMps.dll: dlldata.obj SysStatsCOM_p.obj SysStatsCOM_i.obj
	link /dll /out:SysStatsCOMps.dll /def:SysStatsCOMps.def /entry:DllMain dlldata.obj SysStatsCOM_p.obj SysStatsCOM_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del SysStatsCOMps.dll
	@del SysStatsCOMps.lib
	@del SysStatsCOMps.exp
	@del dlldata.obj
	@del SysStatsCOM_p.obj
	@del SysStatsCOM_i.obj
