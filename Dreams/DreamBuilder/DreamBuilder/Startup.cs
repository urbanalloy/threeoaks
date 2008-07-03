///////////////////////////////////////////////////////////////////////////////////////////////
//
// DreamBuilder: a command line Dream builder
//
// Copyright (c) 2008 Three Oaks Crossing, LLC
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
//	1. Redistributions of source code must retain the above copyright notice, this list of
//		 conditions and the following disclaimer. 
//	2. Redistributions in binary form must reproduce the above copyright notice, this list
//		 of conditions and the following disclaimer in the documentation and/or other materials
//		 provided with the distribution. 
//	3. The name of the author may not be used to endorse or promote products derived from this
//		 software without specific prior written permission. 
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//	COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//	POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////////////////


using System;
using System.Reflection;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace DreamBuilder
{
    class Startup
    {

        static void Main(string[] args)
        {
			String name = "", copyright = "", description = "", company = "";
            Assembly caller = Assembly.GetExecutingAssembly();
            Object[] objArray = caller.GetCustomAttributes(false);

            foreach (Object obj in objArray)
            {
                AssemblyTitleAttribute title = obj as AssemblyTitleAttribute;
                if (title != null) name = title.Title;

                AssemblyCopyrightAttribute cop = obj as AssemblyCopyrightAttribute;
				if (cop != null) copyright = cop.Copyright;

                AssemblyCompanyAttribute comp = obj as AssemblyCompanyAttribute;
                if (comp != null) company = comp.Company;

                AssemblyDescriptionAttribute note = obj as AssemblyDescriptionAttribute;
                if (note != null) description = note.Description;
            }

			Console.ResetColor();
			Console.WriteLine();
			Console.WriteLine("-----------------------------------------------------------------");
            Console.WriteLine("  " + name + "         ::        " + description);
        	Console.WriteLine("  Build " + Assembly.GetExecutingAssembly().GetName().Version);
			Console.WriteLine("  " + copyright + " " + company);
            Console.WriteLine("-----------------------------------------------------------------");            
            Console.WriteLine();

            if (args.Length == 0 || args.Length < 1 || args[0] == "/?")
            {
                OutputCommandLineHelp();
                return;
            }

            string inputFile = null;
            string outputDir = null;
            Dictionary<String, String> defines = null;

            string pattern = @"(?<argname>/[DO])(?<argvalue>.+)";
            foreach (string arg in args)
            {
                // First string is the input file
                // Get the full path
                if (!arg.StartsWith("/") && inputFile == null)
                {
                    inputFile = arg;
                    continue;
                }

                Match match = Regex.Match(arg, pattern);

                // If match not found, command line args are improperly formed.
                if (!match.Success)
                {
                    Console.WriteLine("Improperly formated command line!\n");
                    OutputCommandLineHelp();
                    return;
                }

                string action = match.Groups["argname"].Value;
                string param = match.Groups["argvalue"].Value;

                if (action == "/O")
				{
					if (!String.IsNullOrEmpty(outputDir))
					{
						Console.WriteLine("The output directory is already defined!\n");
						Console.WriteLine("Only one /O argument is authorized!\n");
						OutputCommandLineHelp();
						return;
					}
						
					outputDir = param;                	
                }
                    

                if (action == "/D")
                {
                    if (defines == null)
                        defines = new Dictionary<string, string>();

                    string[] parts = param.Split('=');

					if (parts == null || parts.Length != 2 || String.IsNullOrEmpty(parts[0]))
                    {
                        Console.WriteLine("Improperly formatted define:" + param);
                        OutputCommandLineHelp();
                        return;
                    }

                    defines.Add(parts[0], parts[1]);
                }
            }


            DreamBuilder builder = new DreamBuilder(inputFile, outputDir, defines);
            
            try
            {
                builder.Build();
            } 
            catch (BuilderException e)
			{
				Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("\n\nError: " + e.Message);
				
				if (!String.IsNullOrEmpty(e.Description))
					Console.WriteLine("       " + e.Description);

                if (e.InnerException != null) {
                    Console.WriteLine("\nInner Exception: " + e.InnerException.Message);                    
                }
				Console.ResetColor();
            }
			catch (Exception e)
			{
				Console.ForegroundColor = ConsoleColor.Magenta;
				Console.WriteLine("\n");
				Console.WriteLine("INTERNAL ERROR: Please report this bug to bugs@threeoakscrossing.com.");
				Console.WriteLine("                Don't forget to provide the full output from the program.");

				Console.WriteLine("\nException:");
				Console.WriteLine(e.Message);
				Console.WriteLine("\n" + e.StackTrace);

				if (e.InnerException != null)
				{
					Console.WriteLine("\nInner Exception:");					
					Console.WriteLine(e.InnerException.Message);
					Console.WriteLine("\n" + e.InnerException.StackTrace);
				}
				Console.ResetColor();
			}

			
        }

        private static void OutputCommandLineHelp()
        {
			Console.WriteLine("DREAMBUILDER inputFile [/O<outputDirectory>] [/D<variable>=<value>]\n");

			Console.WriteLine("    inputFile             Path the dream definition file");
            Console.WriteLine("    /O<outputDirectory>   Path to the output directory");
        	Console.WriteLine("    /D<variable>=<value>  Defines a variable to be replaced in the Xml configuration file.");
			Console.WriteLine("                          Several such variables can be defined.");
			Console.WriteLine();
			Console.WriteLine("Note: the default output directory is the current working directory.");
        }
    }
}
