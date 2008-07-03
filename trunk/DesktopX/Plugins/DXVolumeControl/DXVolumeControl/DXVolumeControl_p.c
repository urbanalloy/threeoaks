

/* this ALWAYS GENERATED file contains the proxy stub code */


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

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "DXVolumeControl.h"

#define TYPE_FORMAT_STRING_SIZE   1013                              
#define PROC_FORMAT_STRING_SIZE   141                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   1            

typedef struct _DXVolumeControl_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } DXVolumeControl_MIDL_TYPE_FORMAT_STRING;

typedef struct _DXVolumeControl_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } DXVolumeControl_MIDL_PROC_FORMAT_STRING;

typedef struct _DXVolumeControl_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } DXVolumeControl_MIDL_EXPR_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const DXVolumeControl_MIDL_TYPE_FORMAT_STRING DXVolumeControl__MIDL_TypeFormatString;
extern const DXVolumeControl_MIDL_PROC_FORMAT_STRING DXVolumeControl__MIDL_ProcFormatString;
extern const DXVolumeControl_MIDL_EXPR_FORMAT_STRING DXVolumeControl__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IVistaVolume_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IVistaVolume_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, [wire_marshal] or [user_marshal] attribute.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const DXVolumeControl_MIDL_PROC_FORMAT_STRING DXVolumeControl__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure put_Volume */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter volume */

/* 16 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 18 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 20 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 22 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 24 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 26 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Volume */

/* 28 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 30 */	NdrFcLong( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0x8 ),	/* 8 */
/* 36 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 38 */	NdrFcShort( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x24 ),	/* 36 */
/* 42 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter volume */

/* 44 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 46 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 50 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 52 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 54 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Mute */

/* 56 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 58 */	NdrFcLong( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x9 ),	/* 9 */
/* 64 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 66 */	NdrFcShort( 0x8 ),	/* 8 */
/* 68 */	NdrFcShort( 0x8 ),	/* 8 */
/* 70 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter isMuted */

/* 72 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 74 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 76 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 78 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 80 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 82 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Mute */

/* 84 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 86 */	NdrFcLong( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0xa ),	/* 10 */
/* 92 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x8 ),	/* 8 */
/* 98 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter isMuted */

/* 100 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 102 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 104 */	NdrFcShort( 0x3ea ),	/* Type Offset=1002 */

	/* Return value */

/* 106 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 108 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 110 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_PeakValue */

/* 112 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 114 */	NdrFcLong( 0x0 ),	/* 0 */
/* 118 */	NdrFcShort( 0xb ),	/* 11 */
/* 120 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 124 */	NdrFcShort( 0x24 ),	/* 36 */
/* 126 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter value */

/* 128 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 130 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 132 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 134 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 136 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 138 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const DXVolumeControl_MIDL_TYPE_FORMAT_STRING DXVolumeControl__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/*  8 */	NdrFcShort( 0x3e2 ),	/* Offset= 994 (1002) */
/* 10 */	
			0x13, 0x0,	/* FC_OP */
/* 12 */	NdrFcShort( 0x3ca ),	/* Offset= 970 (982) */
/* 14 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 16 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 18 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 20 */	NdrFcShort( 0x2 ),	/* Offset= 2 (22) */
/* 22 */	NdrFcShort( 0x10 ),	/* 16 */
/* 24 */	NdrFcShort( 0x2f ),	/* 47 */
/* 26 */	NdrFcLong( 0x14 ),	/* 20 */
/* 30 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 32 */	NdrFcLong( 0x3 ),	/* 3 */
/* 36 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 38 */	NdrFcLong( 0x11 ),	/* 17 */
/* 42 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 44 */	NdrFcLong( 0x2 ),	/* 2 */
/* 48 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 50 */	NdrFcLong( 0x4 ),	/* 4 */
/* 54 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 56 */	NdrFcLong( 0x5 ),	/* 5 */
/* 60 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 62 */	NdrFcLong( 0xb ),	/* 11 */
/* 66 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 68 */	NdrFcLong( 0xa ),	/* 10 */
/* 72 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 74 */	NdrFcLong( 0x6 ),	/* 6 */
/* 78 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (310) */
/* 80 */	NdrFcLong( 0x7 ),	/* 7 */
/* 84 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 86 */	NdrFcLong( 0x8 ),	/* 8 */
/* 90 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (316) */
/* 92 */	NdrFcLong( 0xd ),	/* 13 */
/* 96 */	NdrFcShort( 0xf4 ),	/* Offset= 244 (340) */
/* 98 */	NdrFcLong( 0x9 ),	/* 9 */
/* 102 */	NdrFcShort( 0x100 ),	/* Offset= 256 (358) */
/* 104 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 108 */	NdrFcShort( 0x10c ),	/* Offset= 268 (376) */
/* 110 */	NdrFcLong( 0x24 ),	/* 36 */
/* 114 */	NdrFcShort( 0x31a ),	/* Offset= 794 (908) */
/* 116 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 120 */	NdrFcShort( 0x314 ),	/* Offset= 788 (908) */
/* 122 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 126 */	NdrFcShort( 0x312 ),	/* Offset= 786 (912) */
/* 128 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 132 */	NdrFcShort( 0x310 ),	/* Offset= 784 (916) */
/* 134 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 138 */	NdrFcShort( 0x30e ),	/* Offset= 782 (920) */
/* 140 */	NdrFcLong( 0x4014 ),	/* 16404 */
/* 144 */	NdrFcShort( 0x30c ),	/* Offset= 780 (924) */
/* 146 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 150 */	NdrFcShort( 0x30a ),	/* Offset= 778 (928) */
/* 152 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 156 */	NdrFcShort( 0x308 ),	/* Offset= 776 (932) */
/* 158 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 162 */	NdrFcShort( 0x2f2 ),	/* Offset= 754 (916) */
/* 164 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 168 */	NdrFcShort( 0x2f0 ),	/* Offset= 752 (920) */
/* 170 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 174 */	NdrFcShort( 0x2fa ),	/* Offset= 762 (936) */
/* 176 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 180 */	NdrFcShort( 0x2f0 ),	/* Offset= 752 (932) */
/* 182 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 186 */	NdrFcShort( 0x2f2 ),	/* Offset= 754 (940) */
/* 188 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 192 */	NdrFcShort( 0x2f0 ),	/* Offset= 752 (944) */
/* 194 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 198 */	NdrFcShort( 0x2ee ),	/* Offset= 750 (948) */
/* 200 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 204 */	NdrFcShort( 0x2ec ),	/* Offset= 748 (952) */
/* 206 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 210 */	NdrFcShort( 0x2ea ),	/* Offset= 746 (956) */
/* 212 */	NdrFcLong( 0x10 ),	/* 16 */
/* 216 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 218 */	NdrFcLong( 0x12 ),	/* 18 */
/* 222 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 224 */	NdrFcLong( 0x13 ),	/* 19 */
/* 228 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 230 */	NdrFcLong( 0x15 ),	/* 21 */
/* 234 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 236 */	NdrFcLong( 0x16 ),	/* 22 */
/* 240 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 242 */	NdrFcLong( 0x17 ),	/* 23 */
/* 246 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 248 */	NdrFcLong( 0xe ),	/* 14 */
/* 252 */	NdrFcShort( 0x2c8 ),	/* Offset= 712 (964) */
/* 254 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 258 */	NdrFcShort( 0x2cc ),	/* Offset= 716 (974) */
/* 260 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 264 */	NdrFcShort( 0x2ca ),	/* Offset= 714 (978) */
/* 266 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 270 */	NdrFcShort( 0x286 ),	/* Offset= 646 (916) */
/* 272 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 276 */	NdrFcShort( 0x284 ),	/* Offset= 644 (920) */
/* 278 */	NdrFcLong( 0x4015 ),	/* 16405 */
/* 282 */	NdrFcShort( 0x282 ),	/* Offset= 642 (924) */
/* 284 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 288 */	NdrFcShort( 0x278 ),	/* Offset= 632 (920) */
/* 290 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 294 */	NdrFcShort( 0x272 ),	/* Offset= 626 (920) */
/* 296 */	NdrFcLong( 0x0 ),	/* 0 */
/* 300 */	NdrFcShort( 0x0 ),	/* Offset= 0 (300) */
/* 302 */	NdrFcLong( 0x1 ),	/* 1 */
/* 306 */	NdrFcShort( 0x0 ),	/* Offset= 0 (306) */
/* 308 */	NdrFcShort( 0xffff ),	/* Offset= -1 (307) */
/* 310 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 312 */	NdrFcShort( 0x8 ),	/* 8 */
/* 314 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 316 */	
			0x13, 0x0,	/* FC_OP */
/* 318 */	NdrFcShort( 0xc ),	/* Offset= 12 (330) */
/* 320 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 322 */	NdrFcShort( 0x2 ),	/* 2 */
/* 324 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 326 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 328 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 330 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 332 */	NdrFcShort( 0x8 ),	/* 8 */
/* 334 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (320) */
/* 336 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 338 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 340 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 342 */	NdrFcLong( 0x0 ),	/* 0 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	NdrFcShort( 0x0 ),	/* 0 */
/* 350 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 352 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 354 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 356 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 358 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 360 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 368 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 370 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 372 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 374 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 376 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 378 */	NdrFcShort( 0x2 ),	/* Offset= 2 (380) */
/* 380 */	
			0x13, 0x0,	/* FC_OP */
/* 382 */	NdrFcShort( 0x1fc ),	/* Offset= 508 (890) */
/* 384 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 386 */	NdrFcShort( 0x18 ),	/* 24 */
/* 388 */	NdrFcShort( 0xa ),	/* 10 */
/* 390 */	NdrFcLong( 0x8 ),	/* 8 */
/* 394 */	NdrFcShort( 0x58 ),	/* Offset= 88 (482) */
/* 396 */	NdrFcLong( 0xd ),	/* 13 */
/* 400 */	NdrFcShort( 0x78 ),	/* Offset= 120 (520) */
/* 402 */	NdrFcLong( 0x9 ),	/* 9 */
/* 406 */	NdrFcShort( 0x94 ),	/* Offset= 148 (554) */
/* 408 */	NdrFcLong( 0xc ),	/* 12 */
/* 412 */	NdrFcShort( 0xbc ),	/* Offset= 188 (600) */
/* 414 */	NdrFcLong( 0x24 ),	/* 36 */
/* 418 */	NdrFcShort( 0x114 ),	/* Offset= 276 (694) */
/* 420 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 424 */	NdrFcShort( 0x130 ),	/* Offset= 304 (728) */
/* 426 */	NdrFcLong( 0x10 ),	/* 16 */
/* 430 */	NdrFcShort( 0x148 ),	/* Offset= 328 (758) */
/* 432 */	NdrFcLong( 0x2 ),	/* 2 */
/* 436 */	NdrFcShort( 0x160 ),	/* Offset= 352 (788) */
/* 438 */	NdrFcLong( 0x3 ),	/* 3 */
/* 442 */	NdrFcShort( 0x178 ),	/* Offset= 376 (818) */
/* 444 */	NdrFcLong( 0x14 ),	/* 20 */
/* 448 */	NdrFcShort( 0x190 ),	/* Offset= 400 (848) */
/* 450 */	NdrFcShort( 0xffff ),	/* Offset= -1 (449) */
/* 452 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 454 */	NdrFcShort( 0x4 ),	/* 4 */
/* 456 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 458 */	NdrFcShort( 0x0 ),	/* 0 */
/* 460 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 462 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 464 */	NdrFcShort( 0x4 ),	/* 4 */
/* 466 */	NdrFcShort( 0x0 ),	/* 0 */
/* 468 */	NdrFcShort( 0x1 ),	/* 1 */
/* 470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 474 */	0x13, 0x0,	/* FC_OP */
/* 476 */	NdrFcShort( 0xff6e ),	/* Offset= -146 (330) */
/* 478 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 480 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 482 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 484 */	NdrFcShort( 0x8 ),	/* 8 */
/* 486 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 488 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 490 */	NdrFcShort( 0x4 ),	/* 4 */
/* 492 */	NdrFcShort( 0x4 ),	/* 4 */
/* 494 */	0x11, 0x0,	/* FC_RP */
/* 496 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (452) */
/* 498 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 500 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 502 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 506 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 510 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 514 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 516 */	NdrFcShort( 0xff50 ),	/* Offset= -176 (340) */
/* 518 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 520 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 522 */	NdrFcShort( 0x8 ),	/* 8 */
/* 524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 526 */	NdrFcShort( 0x6 ),	/* Offset= 6 (532) */
/* 528 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 530 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 532 */	
			0x11, 0x0,	/* FC_RP */
/* 534 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (502) */
/* 536 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 538 */	NdrFcShort( 0x0 ),	/* 0 */
/* 540 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 542 */	NdrFcShort( 0x0 ),	/* 0 */
/* 544 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 548 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 550 */	NdrFcShort( 0xff40 ),	/* Offset= -192 (358) */
/* 552 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 554 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 556 */	NdrFcShort( 0x8 ),	/* 8 */
/* 558 */	NdrFcShort( 0x0 ),	/* 0 */
/* 560 */	NdrFcShort( 0x6 ),	/* Offset= 6 (566) */
/* 562 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 564 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 566 */	
			0x11, 0x0,	/* FC_RP */
/* 568 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (536) */
/* 570 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 572 */	NdrFcShort( 0x4 ),	/* 4 */
/* 574 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 576 */	NdrFcShort( 0x0 ),	/* 0 */
/* 578 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 580 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 582 */	NdrFcShort( 0x4 ),	/* 4 */
/* 584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 586 */	NdrFcShort( 0x1 ),	/* 1 */
/* 588 */	NdrFcShort( 0x0 ),	/* 0 */
/* 590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 592 */	0x13, 0x0,	/* FC_OP */
/* 594 */	NdrFcShort( 0x184 ),	/* Offset= 388 (982) */
/* 596 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 598 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 600 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 602 */	NdrFcShort( 0x8 ),	/* 8 */
/* 604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 606 */	NdrFcShort( 0x6 ),	/* Offset= 6 (612) */
/* 608 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 610 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 612 */	
			0x11, 0x0,	/* FC_RP */
/* 614 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (570) */
/* 616 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 618 */	NdrFcLong( 0x2f ),	/* 47 */
/* 622 */	NdrFcShort( 0x0 ),	/* 0 */
/* 624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 626 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 628 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 630 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 632 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 634 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 636 */	NdrFcShort( 0x1 ),	/* 1 */
/* 638 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 640 */	NdrFcShort( 0x4 ),	/* 4 */
/* 642 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 644 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 646 */	NdrFcShort( 0x10 ),	/* 16 */
/* 648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 650 */	NdrFcShort( 0xa ),	/* Offset= 10 (660) */
/* 652 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 654 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 656 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (616) */
/* 658 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 660 */	
			0x13, 0x0,	/* FC_OP */
/* 662 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (634) */
/* 664 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 666 */	NdrFcShort( 0x4 ),	/* 4 */
/* 668 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 670 */	NdrFcShort( 0x0 ),	/* 0 */
/* 672 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 674 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 676 */	NdrFcShort( 0x4 ),	/* 4 */
/* 678 */	NdrFcShort( 0x0 ),	/* 0 */
/* 680 */	NdrFcShort( 0x1 ),	/* 1 */
/* 682 */	NdrFcShort( 0x0 ),	/* 0 */
/* 684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 686 */	0x13, 0x0,	/* FC_OP */
/* 688 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (644) */
/* 690 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 692 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 694 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 696 */	NdrFcShort( 0x8 ),	/* 8 */
/* 698 */	NdrFcShort( 0x0 ),	/* 0 */
/* 700 */	NdrFcShort( 0x6 ),	/* Offset= 6 (706) */
/* 702 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 704 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 706 */	
			0x11, 0x0,	/* FC_RP */
/* 708 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (664) */
/* 710 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 712 */	NdrFcShort( 0x8 ),	/* 8 */
/* 714 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 716 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 718 */	NdrFcShort( 0x10 ),	/* 16 */
/* 720 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 722 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 724 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (710) */
			0x5b,		/* FC_END */
/* 728 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 730 */	NdrFcShort( 0x18 ),	/* 24 */
/* 732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 734 */	NdrFcShort( 0xa ),	/* Offset= 10 (744) */
/* 736 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 738 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 740 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (716) */
/* 742 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 744 */	
			0x11, 0x0,	/* FC_RP */
/* 746 */	NdrFcShort( 0xff0c ),	/* Offset= -244 (502) */
/* 748 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 750 */	NdrFcShort( 0x1 ),	/* 1 */
/* 752 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 754 */	NdrFcShort( 0x0 ),	/* 0 */
/* 756 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 758 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 760 */	NdrFcShort( 0x8 ),	/* 8 */
/* 762 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 764 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 766 */	NdrFcShort( 0x4 ),	/* 4 */
/* 768 */	NdrFcShort( 0x4 ),	/* 4 */
/* 770 */	0x13, 0x0,	/* FC_OP */
/* 772 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (748) */
/* 774 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 776 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 778 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 780 */	NdrFcShort( 0x2 ),	/* 2 */
/* 782 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 786 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 788 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 790 */	NdrFcShort( 0x8 ),	/* 8 */
/* 792 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 794 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 796 */	NdrFcShort( 0x4 ),	/* 4 */
/* 798 */	NdrFcShort( 0x4 ),	/* 4 */
/* 800 */	0x13, 0x0,	/* FC_OP */
/* 802 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (778) */
/* 804 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 806 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 808 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 810 */	NdrFcShort( 0x4 ),	/* 4 */
/* 812 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 814 */	NdrFcShort( 0x0 ),	/* 0 */
/* 816 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 818 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 820 */	NdrFcShort( 0x8 ),	/* 8 */
/* 822 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 824 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 826 */	NdrFcShort( 0x4 ),	/* 4 */
/* 828 */	NdrFcShort( 0x4 ),	/* 4 */
/* 830 */	0x13, 0x0,	/* FC_OP */
/* 832 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (808) */
/* 834 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 836 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 838 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 840 */	NdrFcShort( 0x8 ),	/* 8 */
/* 842 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 844 */	NdrFcShort( 0x0 ),	/* 0 */
/* 846 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 848 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 850 */	NdrFcShort( 0x8 ),	/* 8 */
/* 852 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 854 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 856 */	NdrFcShort( 0x4 ),	/* 4 */
/* 858 */	NdrFcShort( 0x4 ),	/* 4 */
/* 860 */	0x13, 0x0,	/* FC_OP */
/* 862 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (838) */
/* 864 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 866 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 868 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 870 */	NdrFcShort( 0x8 ),	/* 8 */
/* 872 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 874 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 876 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 878 */	NdrFcShort( 0x8 ),	/* 8 */
/* 880 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 882 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 884 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 886 */	NdrFcShort( 0xffee ),	/* Offset= -18 (868) */
/* 888 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 890 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 892 */	NdrFcShort( 0x28 ),	/* 40 */
/* 894 */	NdrFcShort( 0xffee ),	/* Offset= -18 (876) */
/* 896 */	NdrFcShort( 0x0 ),	/* Offset= 0 (896) */
/* 898 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 900 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 902 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 904 */	NdrFcShort( 0xfdf8 ),	/* Offset= -520 (384) */
/* 906 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 908 */	
			0x13, 0x0,	/* FC_OP */
/* 910 */	NdrFcShort( 0xfef6 ),	/* Offset= -266 (644) */
/* 912 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 914 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 916 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 918 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 920 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 922 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 924 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 926 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 928 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 930 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 932 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 934 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 936 */	
			0x13, 0x0,	/* FC_OP */
/* 938 */	NdrFcShort( 0xfd8c ),	/* Offset= -628 (310) */
/* 940 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 942 */	NdrFcShort( 0xfd8e ),	/* Offset= -626 (316) */
/* 944 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 946 */	NdrFcShort( 0xfda2 ),	/* Offset= -606 (340) */
/* 948 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 950 */	NdrFcShort( 0xfdb0 ),	/* Offset= -592 (358) */
/* 952 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 954 */	NdrFcShort( 0xfdbe ),	/* Offset= -578 (376) */
/* 956 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 958 */	NdrFcShort( 0x2 ),	/* Offset= 2 (960) */
/* 960 */	
			0x13, 0x0,	/* FC_OP */
/* 962 */	NdrFcShort( 0x14 ),	/* Offset= 20 (982) */
/* 964 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 966 */	NdrFcShort( 0x10 ),	/* 16 */
/* 968 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 970 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 972 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 974 */	
			0x13, 0x0,	/* FC_OP */
/* 976 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (964) */
/* 978 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 980 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 982 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 984 */	NdrFcShort( 0x20 ),	/* 32 */
/* 986 */	NdrFcShort( 0x0 ),	/* 0 */
/* 988 */	NdrFcShort( 0x0 ),	/* Offset= 0 (988) */
/* 990 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 992 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 994 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 996 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 998 */	NdrFcShort( 0xfc28 ),	/* Offset= -984 (14) */
/* 1000 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1002 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1006 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1008 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1010 */	NdrFcShort( 0xfc18 ),	/* Offset= -1000 (10) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            VARIANT_UserSize
            ,VARIANT_UserMarshal
            ,VARIANT_UserUnmarshal
            ,VARIANT_UserFree
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IVistaVolume, ver. 0.0,
   GUID={0x972CCBF6,0x9B3A,0x4019,{0x97,0x4E,0x2C,0x47,0xED,0x4C,0xF7,0xB3}} */

#pragma code_seg(".orpc")
static const unsigned short IVistaVolume_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    56,
    84,
    112
    };

static const MIDL_STUBLESS_PROXY_INFO IVistaVolume_ProxyInfo =
    {
    &Object_StubDesc,
    DXVolumeControl__MIDL_ProcFormatString.Format,
    &IVistaVolume_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IVistaVolume_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    DXVolumeControl__MIDL_ProcFormatString.Format,
    &IVistaVolume_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IVistaVolumeProxyVtbl = 
{
    &IVistaVolume_ProxyInfo,
    &IID_IVistaVolume,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IVistaVolume::put_Volume */ ,
    (void *) (INT_PTR) -1 /* IVistaVolume::get_Volume */ ,
    (void *) (INT_PTR) -1 /* IVistaVolume::put_Mute */ ,
    (void *) (INT_PTR) -1 /* IVistaVolume::get_Mute */ ,
    (void *) (INT_PTR) -1 /* IVistaVolume::get_PeakValue */
};


static const PRPC_STUB_FUNCTION IVistaVolume_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IVistaVolumeStubVtbl =
{
    &IID_IVistaVolume,
    &IVistaVolume_ServerInfo,
    12,
    &IVistaVolume_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    DXVolumeControl__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x70001f3, /* MIDL Version 7.0.499 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * _DXVolumeControl_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IVistaVolumeProxyVtbl,
    0
};

const CInterfaceStubVtbl * _DXVolumeControl_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IVistaVolumeStubVtbl,
    0
};

PCInterfaceName const _DXVolumeControl_InterfaceNamesList[] = 
{
    "IVistaVolume",
    0
};

const IID *  _DXVolumeControl_BaseIIDList[] = 
{
    &IID_IDispatch,
    0
};


#define _DXVolumeControl_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _DXVolumeControl, pIID, n)

int __stdcall _DXVolumeControl_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_DXVolumeControl_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo DXVolumeControl_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _DXVolumeControl_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _DXVolumeControl_StubVtblList,
    (const PCInterfaceName * ) & _DXVolumeControl_InterfaceNamesList,
    (const IID ** ) & _DXVolumeControl_BaseIIDList,
    & _DXVolumeControl_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

