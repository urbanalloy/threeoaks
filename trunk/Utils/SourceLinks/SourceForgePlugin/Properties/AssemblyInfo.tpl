// This file is used as a template by SubWCRev
// 
// DO NOT MODIFY IT, IT WILL BE OVERWRITTEN AT BUILD TIME
// MAKE MODIFICATIONS To THE TEMPLATE FILE (.TPL)
//
//"c:/Program Files/TortoiseSVN/bin/SubWCRev.exe" $(ProjectDir) $(InputPath) $(InputDir)/AssemblyInfo.cs

#region Using directives

using System.Reflection;
using System.Runtime.InteropServices;
using System.Windows;

#endregion

// General Information about an assembly is controlled through the following 
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
[assembly: AssemblyTitle("SourceForgePlugin")]
[assembly: AssemblyDescription("SourceForge Plugin for SourceLinks")]
[assembly: AssemblyConfiguration("")]
[assembly: AssemblyCompany("Julien Templier")]
[assembly: AssemblyProduct("SourceForgePlugin")]
[assembly: AssemblyCopyright("Copyright © Julien Templier 2010")]
[assembly: AssemblyTrademark("")]
[assembly: AssemblyCulture("")]

// Setting ComVisible to false makes the types in this assembly not visible 
// to COM components.  If you need to access a type in this assembly from 
// COM, set the ComVisible attribute to true on that type.
[assembly: ComVisible(false)]

// The following GUID is for the ID of the typelib if this project is exposed to COM
[assembly: Guid("0DEAB0F3-A29F-463F-9440-5BBEA5A3519C")]

// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version
//      Build Number
//      Revision
//
// You can specify all the values or you can default the Revision and Build Numbers
// by using the '*' as shown below:
[assembly: AssemblyVersion("1.0.$WCREV$.0")]
[assembly: AssemblyFileVersion("1.0.$WCREV$.0")]


