

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0499 */
/* at Mon Feb 26 06:40:52 2007
 */
/* Compiler settings for .\DXVolumeControl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __DXVolumeControl_h__
#define __DXVolumeControl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVistaVolume_FWD_DEFINED__
#define __IVistaVolume_FWD_DEFINED__
typedef interface IVistaVolume IVistaVolume;
#endif 	/* __IVistaVolume_FWD_DEFINED__ */


#ifndef ___IVolumeEvents_FWD_DEFINED__
#define ___IVolumeEvents_FWD_DEFINED__
typedef interface _IVolumeEvents _IVolumeEvents;
#endif 	/* ___IVolumeEvents_FWD_DEFINED__ */


#ifndef __VistaVolume_FWD_DEFINED__
#define __VistaVolume_FWD_DEFINED__

#ifdef __cplusplus
typedef class VistaVolume VistaVolume;
#else
typedef struct VistaVolume VistaVolume;
#endif /* __cplusplus */

#endif 	/* __VistaVolume_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IVistaVolume_INTERFACE_DEFINED__
#define __IVistaVolume_INTERFACE_DEFINED__

/* interface IVistaVolume */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IVistaVolume;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("972CCBF6-9B3A-4019-974E-2C47ED4CF7B3")
    IVistaVolume : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Volume( 
            /* [in] */ int volume) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Volume( 
            /* [retval][out] */ int *volume) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Mute( 
            /* [in] */ BOOL isMuted) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Mute( 
            /* [retval][out] */ VARIANT *isMuted) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PeakValue( 
            /* [retval][out] */ int *value) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVistaVolumeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVistaVolume * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVistaVolume * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVistaVolume * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVistaVolume * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVistaVolume * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVistaVolume * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVistaVolume * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Volume )( 
            IVistaVolume * This,
            /* [in] */ int volume);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Volume )( 
            IVistaVolume * This,
            /* [retval][out] */ int *volume);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Mute )( 
            IVistaVolume * This,
            /* [in] */ BOOL isMuted);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Mute )( 
            IVistaVolume * This,
            /* [retval][out] */ VARIANT *isMuted);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PeakValue )( 
            IVistaVolume * This,
            /* [retval][out] */ int *value);
        
        END_INTERFACE
    } IVistaVolumeVtbl;

    interface IVistaVolume
    {
        CONST_VTBL struct IVistaVolumeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVistaVolume_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVistaVolume_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVistaVolume_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVistaVolume_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVistaVolume_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVistaVolume_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVistaVolume_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVistaVolume_put_Volume(This,volume)	\
    ( (This)->lpVtbl -> put_Volume(This,volume) ) 

#define IVistaVolume_get_Volume(This,volume)	\
    ( (This)->lpVtbl -> get_Volume(This,volume) ) 

#define IVistaVolume_put_Mute(This,isMuted)	\
    ( (This)->lpVtbl -> put_Mute(This,isMuted) ) 

#define IVistaVolume_get_Mute(This,isMuted)	\
    ( (This)->lpVtbl -> get_Mute(This,isMuted) ) 

#define IVistaVolume_get_PeakValue(This,value)	\
    ( (This)->lpVtbl -> get_PeakValue(This,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVistaVolume_INTERFACE_DEFINED__ */



#ifndef __DXVolumeControlLib_LIBRARY_DEFINED__
#define __DXVolumeControlLib_LIBRARY_DEFINED__

/* library DXVolumeControlLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_DXVolumeControlLib;

#ifndef ___IVolumeEvents_DISPINTERFACE_DEFINED__
#define ___IVolumeEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IVolumeEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IVolumeEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C4DAD7A8-7750-4FE9-BFA8-293364E98134")
    _IVolumeEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IVolumeEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IVolumeEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IVolumeEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IVolumeEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IVolumeEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IVolumeEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IVolumeEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IVolumeEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IVolumeEventsVtbl;

    interface _IVolumeEvents
    {
        CONST_VTBL struct _IVolumeEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IVolumeEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IVolumeEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IVolumeEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IVolumeEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IVolumeEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IVolumeEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IVolumeEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IVolumeEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_VistaVolume;

#ifdef __cplusplus

class DECLSPEC_UUID("F36B9A47-AF5B-4AE2-A039-48CC18FEEA4C")
VistaVolume;
#endif
#endif /* __DXVolumeControlLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


