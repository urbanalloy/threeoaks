///////////////////////////////////////////////////////////////////////////////////////////////
//
// DotNetDocklets : bringing .NET to ObjectDock
//
// Copyright (c) 2004-2009, Julien Templier
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer. 
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution. 
//  3. The name of the author may not be used to endorse or promote products derived from this
//     software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////////////////

using System;
using System.Runtime.InteropServices;
using System.Reflection;
using Microsoft.Win32;

namespace ObjectDockSDK.Registration
{
    /// <summary>
	/// Used internally by the Interop Docklet
	/// </summary>
	/// <exclude />
    [Guid("89CF607D-F586-416f-8C93-BC7DE95FA36F")]
	public class Register : IRegisterInterface
	{

        #region Imports

        // Keys
        public static readonly UIntPtr HkeyClassesRoot = (UIntPtr)0x80000000;
        public static readonly UIntPtr HkeyCurrentUser = (UIntPtr)0x80000001;

        public const uint Delete = 0x00010000;
        public const uint ReadOnly = 0x00020000;
        public const uint WriteDac = 0x00040000;
        public const uint WriteOwner = 0x00080000;
        public const uint Synchronize = 0x00100000;
        public const uint StandardRightsRequired = 0x000F0000;
        public const uint StandardRightsAll = 0x001F0000;

        public const uint GenericRead = 0x80000000;
        public const uint GenericWrite = 0x40000000;
        public const uint GenericExecute = 0x20000000;
        public const uint GenericAll = 0x10000000;

        // Open a key
        [DllImport("advapi32.dll", EntryPoint="RegCreateKeyExW", CharSet=CharSet.Unicode, ExactSpelling=true, SetLastError=true)]
        internal static extern int RegCreateKey(UIntPtr key, string subkey, uint reserved, string className, uint options, uint desiredSam, uint securityAttributes, out IntPtr openedKey, out uint disposition);
        
        // Maps a predefined registry key to the specified registry key.
        [DllImport("advapi32.dll", EntryPoint = "RegOverridePredefKey", CharSet = CharSet.Unicode, ExactSpelling = true, SetLastError = true)]
        internal static extern int RegOverridePredefKey(UIntPtr key, IntPtr newKey);
       
        // Close a key
        [DllImport("advapi32.dll", EntryPoint="RegCloseKey", CharSet=CharSet.Unicode, ExactSpelling=true, SetLastError=true)]
        internal static extern int RegCloseKey(IntPtr key);

        #endregion

        #region Wrappers

        // Wrapper for creating Registry Keys 
        internal static IntPtr OpenRegistryKey(UIntPtr key, string path)
        {
            IntPtr newKey;
            uint disposition;
            const uint security = GenericWrite;
            int error = RegCreateKey(key, path, 0, null, 0, security, 0, out newKey, out disposition);

            if (error != 0)
                return IntPtr.Zero;

            return newKey;
        }

        //Wrapper for the mapping
        internal static void OverrideRegistryKey(UIntPtr key, IntPtr newKey)
        {
            RegOverridePredefKey(key, newKey);
        }

        //Wrapper for freeing the handle
        internal static void CloseRegistryKey(IntPtr key)
        {
            RegCloseKey(key);
        }

        private static bool MapRegistryKey(UIntPtr key, string location)
        {
            IntPtr createdKey = IntPtr.Zero;            
            try
            {
                createdKey = OpenRegistryKey(HkeyCurrentUser, location);

                if (createdKey == IntPtr.Zero)
                    return false;

                OverrideRegistryKey(key, createdKey);
                return true;
            }
            catch (Exception)
            {
                return false;
            }
            finally
            {                
                CloseRegistryKey(createdKey);                
            }          
        }

        private static void UnMapRegistryKey(UIntPtr key)
        {
            OverrideRegistryKey(key, IntPtr.Zero);
        }

        #endregion

        /// <summary>
		/// Register the dll pointed by path
		/// </summary>
		/// <param name="path">Absolute path to the dll to register</param>
		/// <returns>
		///		true if registration succeeded,
		///		false otherwise.
		///	</returns>
		public Boolean RegisterDll(string path) 
		{
			try	{
				// Load the helper docklet when requested (as it is not in the base search path)
				// (we cannot use AppDomain.CurrentDomain.AppendPrivatePath as it should be used
				//  on the app startup)
				AppDomain.CurrentDomain.AssemblyResolve += AssemblyResolve;
				
				Assembly asm = Assembly.LoadFrom(path);

                // Check if assembly provides a minimal version
                foreach (Attribute attribute in Attribute.GetCustomAttributes(asm))
                {
                    if (attribute.GetType() == typeof(SDKVersionAttribute))
                    {
                        // Get ObjectDockSDK Version
                        Assembly SDK = Assembly.Load("ObjectDockSDK");

                        if (((SDKVersionAttribute)attribute).Version > SDK.GetName().Version)
                            return false;
                    }
                }                

                // RegisterAssembly is writing to HKCR, redirect it to HKCU\\Software\\Classes\\
                if (!MapRegistryKey(HkeyClassesRoot, "Software\\Classes\\"))
                    return false;

                var reg = new RegistrationServices();
				return reg.RegisterAssembly(asm, AssemblyRegistrationFlags.SetCodeBase);               
			}
			catch (Exception) {
				return false;
			}
            finally
			{
                UnMapRegistryKey(HkeyClassesRoot);
			}
		}       

		private static Assembly AssemblyResolve(object sender, ResolveEventArgs args)
		{
			if (args.Name.StartsWith("ObjectDockSDK, Version=2"))
			{
				// Get the assembly path from the registry (looking for AssemblyData)
                RegistryKey codebase = Registry.CurrentUser.OpenSubKey("Software\\Classes\\CLSID\\{0C16326F-E9A1-436a-ABFE-CF2057A6DB89}\\InprocServer32\\2.1.0.0");
			    if (codebase != null)
			    {
			        Assembly asm = Assembly.LoadFrom((String)codebase.GetValue("CodeBase"));
			        codebase.Close();
			        return asm;
			    }
			}
			return null;
		}
	}
}
// #EOF
