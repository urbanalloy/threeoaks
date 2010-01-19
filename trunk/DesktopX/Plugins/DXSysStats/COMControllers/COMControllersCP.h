/*
 * SysStats Widget Framework
 * Copyright (C) 2002-2006 Paul Andrews
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _COMCONTROLLERSCP_H_
#define _COMCONTROLLERSCP_H_


template <class T>
class CProxy_IScriptControllerEvents : public IConnectionPointImpl<T, &DIID__IScriptControllerEvents, CComDynamicUnkArray>
{
	//Warning this class may be recreated by the wizard.
public:
	LONG Fire_HandleMessage(UINT wParam, UINT lParam, SHORT X, SHORT Y)
	{
		CComVariant varResult;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		CComVariant* pvars = new CComVariant[4];
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IDispatch* pDispatch = reinterpret_cast<IDispatch*>(sp.p);
			if (pDispatch != NULL)
			{
				VariantClear(&varResult);
#ifdef notdef
				pvars[3] = &wParam;
				pvars[2] = &lParam;
#else
				pvars[3] = (int)wParam;
				pvars[2] = (int)lParam;
#endif
				pvars[1] = X;
				pvars[0] = Y;
				DISPPARAMS disp = { pvars, NULL, 4, 0 };
				pDispatch->Invoke(0x1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, NULL, NULL);
			}
		}
		delete[] pvars;
		return varResult.lVal;
	
	}
};
#endif