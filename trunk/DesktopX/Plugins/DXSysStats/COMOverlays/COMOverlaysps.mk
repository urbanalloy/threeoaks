
COMOverlaysps.dll: dlldata.obj COMOverlays_p.obj COMOverlays_i.obj
	link /dll /out:COMOverlaysps.dll /def:COMOverlaysps.def /entry:DllMain dlldata.obj COMOverlays_p.obj COMOverlays_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del COMOverlaysps.dll
	@del COMOverlaysps.lib
	@del COMOverlaysps.exp
	@del dlldata.obj
	@del COMOverlays_p.obj
	@del COMOverlays_i.obj
