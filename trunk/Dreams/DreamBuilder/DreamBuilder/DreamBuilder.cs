///////////////////////////////////////////////////////////////////////////////////////////////
//
// DreamBuilder: a command line builder for Dreams
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
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Text;
using System.Text.RegularExpressions;
using DreamBuilder.Properties;
using DreamBuilder.Triggers;
using Microsoft.Win32;

namespace DreamBuilder
{
    /// <summary>
    /// Custom Builder exception
    /// </summary>
    [Serializable]
    public class BuilderException : Exception
    {
		#region Private Data

    	private readonly String _description;		

		#endregion

		#region Public Properties

		public String Description
		{
			get
			{
				return _description;
			}
		}

		#endregion
   
		public BuilderException() {}

		public BuilderException(String message) : base(message) {}

		public BuilderException(String message, String description) : base(message)
        {           
            _description = description;                    
        }

		public BuilderException(String message, Exception exception) : base(message, exception) {}

		public BuilderException(String message, String description, Exception exception) : base(message, exception)
		{
			_description = description;			
		}

        protected BuilderException(SerializationInfo info, StreamingContext context) : base(info, context) { }
    }
  
    /// <summary>
    /// Dream Builder class
    /// </summary>
	public class DreamBuilder
	{
		[DllImport("DreamBuilderHook.dll", CharSet = CharSet.Unicode)]
		private static extern bool GetLastMessage(StringBuilder message, int length);

		[DllImport("DreamBuilderHook.dll", CharSet = CharSet.Unicode)]
		private static extern bool BuildDream(string dllPath, string output, string name, string description, string contents, string preview, string author, string url, string copyright, string permissions);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        static extern IntPtr LoadLibrary(string lpFileName);

		[DllImport("kernel32.dll", SetLastError = true)]
		static extern bool FreeLibrary(IntPtr hModule);
        
        private const string DreamMakerDll = "DreamMaker.dll";

		private const int NO_FILESIZE_CHECK = -1;
    	private const int MAX_VIDEOSIZE = 157286400;

    	private static readonly string[] FILTER_VIDEO = new[] {".avi", ".mpg", ".mpeg", ".wmv"};
    	private static readonly string[] FILTER_IMAGE = new[] {".jpg", ".jpeg", ".png", ".bmp"};
		private static readonly string[] FILTER_INVALID = new[] { ".scr", ".exe" };

        /// <summary>
        /// DreamMaker install path
        /// </summary>
        private string installPath;

        private string buildDir;

        private string inputFile;
        private string outputDirectory;
        private Dictionary<string, string> defines;

        #region Properties

        public string InputFile
        {
            get { return inputFile; }
            set { inputFile = value; }
        }

        public string OutputDirectory
        {
            get { return outputDirectory; }
            set { outputDirectory = value; }
        }

        public Dictionary<string, string> Defines
        {
            get { return defines; }
            set { defines = value; }
        }

        #endregion

        #region Dream Contents

        private string name = "";
        private string description = "";
        private List<string> contents;
        private string preview = "";
        private string author = "";
        private string url = "";
        private string copyright = "";
        private string permissions = "";

        #endregion

		/// <summary>
		/// Initializes a new instance of the <see cref="DreamBuilder"/> class.
		/// </summary>
		/// <param name="input">The input</param>
        public DreamBuilder(string input) : this(input, Environment.CurrentDirectory, null) {}

		/// <summary>
		/// Initializes a new instance of the <see cref="DreamBuilder"/> class.
		/// </summary>
		/// <param name="input">The input</param>
		/// <param name="outputDir">The output dir</param>
        public DreamBuilder(string input, string outputDir) : this(input, outputDir, null) { }

		/// <summary>
		/// Initializes a new instance of the <see cref="DreamBuilder"/> class.
		/// </summary>
		/// <param name="input">The input</param>
		/// <param name="outputDir">The output dir</param>
		/// <param name="defs">The list of variable replacement</param>
        public DreamBuilder(string input, string outputDir, Dictionary<string, string> defs)
		{
            inputFile = input;

            // in the case we have defines, but no ouputDir
            outputDirectory = String.IsNullOrEmpty(outputDir) ? Environment.CurrentDirectory : outputDir;

            defines = defs;
		}

        public void Build()
        {
            CheckBuildParameters();

            CheckDeskcapesPresent();

            PrepareBuildDirectory();

            ParseDreamDefinition();

            CreateDream();

            CleanupBuildDir();

        	Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("\nSuccessfully built dream: " + name + ".Dream");
			Console.ResetColor();        	
        }

        #region Main Build functions

        /// <summary>
        /// Check input & output Directories
        /// </summary>
        private void CheckBuildParameters()
        {
            // Check input/output initialized
            if (String.IsNullOrEmpty(inputFile))
                OutputError("Input file not initialized", null, null);

            buildDir = Path.GetTempFileName();            

            // Check input File
            if (!File.Exists(inputFile))
            {
                OutputError("Input file not found", "file: " + inputFile, null);
            }            

            // Check for output directory, create it if it does not exist
            if (!Directory.Exists(outputDirectory))
            {
                try
                {
                    Directory.CreateDirectory(outputDirectory);
                }
                catch (Exception e)
                {
                    OutputError("Invalid output directory", "Could not create directory" + outputDirectory, e);
                }
            }
        }    

        /// <summary>
        /// Check for Deskcapes installation, get install path for later loading of the DreamMaker dll
        /// </summary>
        private void CheckDeskcapesPresent()
        {                   
            // Check for Deskcapes install directory
            RegistryKey deskcapesKey = Registry.LocalMachine;
            deskcapesKey = deskcapesKey.OpenSubKey("SOFTWARE\\Stardock\\ObjectDesktop\\DeskScapes");

            if (deskcapesKey == null)
            {
                OutputError("Deskcapes does not seem to be installed.", "Please download and install Deskcapes from http://dream.wincustomize.com", null);
                return;
            }

            installPath = deskcapesKey.GetValue("Path").ToString();            

            if (installPath == null)
            {
                OutputError("Deskcapes does not seem to be installed.", "Please download and install Deskcapes from http://dream.wincustomize.com", null);
                return;
            }

            // Check for DreamMaker dll
            string DreamDll = Path.Combine(installPath, DreamMakerDll);
            if (!File.Exists(DreamDll))
            {
                OutputError("DreamMaker does not seem to be installed.", "Please download and install DreamMaker from http://dream.wincustomize.com", null);    
            }

        }

        /// <summary>
        /// Create temporary directory for build
        /// </summary>
		private void PrepareBuildDirectory()
		{
			if (File.Exists(buildDir))
				File.Delete(buildDir);

			if (!Directory.Exists(buildDir))
				Directory.CreateDirectory(buildDir);
		}

         /// <summary>
        /// Parse Dream XML definition file
        /// </summary>
        private void ParseDreamDefinition()
        {

            // Read data
            var configuration = new DreamConfiguration();

			string buffer = ReplaceDefines(File.ReadAllText(inputFile));
            try 
            {	          
				configuration.Load(buffer);
            } catch (Exception e) {
                OutputError("Error loading configuration file", null, e);
            }

            DreamConfig dream = configuration.Config;

            name = dream.name;
            description = dream.description;
            author = dream.author;
            url = dream.url;
			preview = LoadPreview(dream.preview);

         	copyright = dream.copyright;
			permissions = dream.permissions;

			// Check that name is valid as a filename
			Console.ForegroundColor = ConsoleColor.Green;
			Console.Write("Checking dream output name...");			 
			CheckForInvalidChars(name);
			Console.Write("\t[OK]\n");
			Console.ResetColor();  

            Console.WriteLine("");
            switch (configuration.Type)
            {
                case DreamType.Video:
                    LoadVideoDream(dream.data.video);
                    break;

                case DreamType.Trigger:            		
                    LoadTriggersDream(dream.data.triggers);					
                    break;

                case DreamType.Dynamic:
                    LoadDynamicDream(dream.data.dynamic);
                    break;

                case DreamType.Invalid:
                    OutputError("Invalid dream!", "The Dream configuration file is invalid", null);
                    break;
            }
        }

      
        /// <summary>
        /// Create the dream file by calling into the DreamMaker DLL
        /// </summary>
        private void CreateDream()
        {
            // Output Dream info
            Console.WriteLine("");
            Console.WriteLine("Building Dream...");
            Console.WriteLine("  Name: \t" + name);
            Console.WriteLine("  Description: \t" + description);
            Console.WriteLine("  Author: \t" + author);
            Console.WriteLine("  Url: \t\t" + url);
            Console.WriteLine("  Copyright: \t" + copyright);
            Console.WriteLine("  Permissions: \t" + permissions);

        	Console.WriteLine();
			
			string outputFile = Path.Combine(outputDirectory, name + ".Dream");
        	string dllPath = Path.Combine(installPath, DreamMakerDll);        	

			IntPtr hLibrary = LoadLibrary("DreamBuilderHook.dll");
			if (hLibrary == IntPtr.Zero)
				OutputError("Error loading DreamBuilderHook.dll", null, new Win32Exception(Marshal.GetLastWin32Error()));

			bool ret = BuildDream(dllPath, outputFile, name, description, GetContentList(), preview, author, url, copyright, permissions);

			var buffer = new StringBuilder(2000);
        	GetLastMessage(buffer, 2000);

			FreeLibrary(hLibrary);

			if (buffer.Length > 0)
				OutputError("Error creating Dream file", buffer.ToString(), null);

			if (!ret || !File.Exists(outputFile))
				OutputError("Unknown error creating dream file", null, null);			
        }

        /// <summary>
        /// Cleanup the build directory
        /// </summary>
        private void CleanupBuildDir()
        {
            if (buildDir != null && Directory.Exists(buildDir))
                Directory.Delete(buildDir, true);
        }

        #endregion

        #region Errors, Variable Replacement & Invalid characters checks

		/// <summary>
		/// Cleanup and throw exception
		/// </summary>
		/// <param name="error">the error message</param>		
		/// <param name="desc">the error description</param>
		/// <param name="e">the inner exception</param>
		private void OutputError(string error, string desc, Exception e)
		{
			CleanupBuildDir();
			throw new BuilderException(error, desc, e);
		}

        private string ReplaceDefines(string input) { 

            // Check for strings to replace            
            var FindVars = new Regex(@"\$(\w+)\$");

            string Output = FindVars.Replace(input,
                delegate(Match m)
                {
                    // no define for this key
                    if (defines == null || !defines.ContainsKey(m.Groups[1].Value)) {
                        OutputError("No value specified for variable", "Variable name: " + m.Groups[0].Value, null);
                    }

                    return defines[m.Groups[1].Value];
                });

            return Output;
        }

		private void CheckForInvalidChars(string path)
		{
			foreach (char invalid in Path.GetInvalidPathChars())
			{
				if (path.Contains(invalid.ToString()))
					OutputError("Path contains invalid characters", path, null);
			}

			if (Path.GetFileName(path) == null)
				return;

			foreach (char invalid in Path.GetInvalidFileNameChars())
			{
				if (Path.GetFileName(path).Contains(invalid.ToString()))
					OutputError("Filename contains invalid characters", Path.GetFileName(path), null);
			}
		}


        #endregion

		#region File & preview management

		private void AddContent(string file)
		{
			if (contents == null)
				contents = new List<string>();

			if (contents.Contains(file))
				return;

			contents.Add(file);
		}

		private string GetContentList()
		{
			string list = "";
			foreach (string file in contents)
			{
				list += '|' + file;
			}

			// remove first |
			return list.TrimStart('|');
		}

		/// <summary>
        /// Check for file existence and copy it to the build dir
        /// </summary>
        /// <param name="file">Full/relative path to the file</param>
        /// <param name="targetPath">path relative to the build directory where the file will be copied</param>
		/// <param name="formatFilter">an array of file formats (".[format]") (null if N/A)</param>
		/// <param name="maxSize">a maximum file size (0 if N/A)</param>
        /// <returns>full path to the file (in the build directory)</returns> 
        private string LoadAndCheckFile(string file, string targetPath, int maxSize, IEnumerable<string> formatFilter)
        {

			CheckForInvalidChars(file);

        	if (!Path.IsPathRooted(file))
                file = Path.Combine(Path.GetDirectoryName(Path.GetFullPath(inputFile)), file);

            if (!File.Exists(file))
                OutputError("File not found", file, null);

            if ((new FileInfo(file)).Length == 0)
                OutputError("File is empty", file, null);

			if (maxSize != NO_FILESIZE_CHECK)			
				if ((new FileInfo(file)).Length > maxSize)
					OutputError("File is too big", "Maximum file size is " + maxSize/(1024*1024) + " MB - " + file, null);
			
			if (formatFilter != null)
			{
				bool check = false;
				string extension = Path.GetExtension(file).ToUpper();
				string formatList = "";

				foreach (string format in formatFilter)
				{
					formatList += format.ToUpper() + ", ";
					if (extension.Equals(format.ToUpper()))
						check = true;
				}

				if (!check)									
					OutputError("File is not in a valid format", "Accepted Formats are " + formatList.TrimEnd(new[] {' ', ','}) + " - " + file, null);				
			}

            // Check for target directory  
            string output = Path.Combine(buildDir, targetPath);			
			if (!Directory.Exists(output))
			{
				try
				{
					Directory.CreateDirectory(output);
				}
				catch (Exception e)
				{
					OutputError("Invalid target directory", "Could not create directory" + output, e);
				}
			}

            // Copy file to build directory                 
            string target = Path.Combine(output, Path.GetFileName(file));
            File.Copy(file, target, true);

            return target;
        }

    	/// <summary>
        /// Load and check preview file
        ///  - check file existence
        ///  - check extension
        /// </summary>
        /// <param name="file">path to the preview file</param>
        /// <returns></returns>
        private string LoadPreview(string file)
        {
			Console.ForegroundColor = ConsoleColor.Green;
            Console.Write("Checking preview file...");

			string previewFile = LoadAndCheckFile(file, "", NO_FILESIZE_CHECK, FILTER_IMAGE);

			// Additional image format checks
        	Bitmap bitmap = null;
			try
			{
				bitmap = new Bitmap(previewFile);
			}
			catch (Exception e)
			{
				OutputError("Invalid preview file", file, e);
			}

			if (bitmap == null)
			{
				OutputError("Invalid preview file", file, null);
				return null;
			}

        	if (!bitmap.RawFormat.Equals(ImageFormat.Bmp) &&
			    !bitmap.RawFormat.Equals(ImageFormat.Png) &&
			    !bitmap.RawFormat.Equals(ImageFormat.Jpeg))
			{
				bitmap.Dispose();
				OutputError("Preview file is not in a valid format (.JPG, .JPEG, .PNG, .BMP)", file, null);
			}

			if (bitmap.Width > 256 || bitmap.Height > 256)
			{
				bitmap.Dispose();
				OutputError("Image is larger than 256x256", file, null);
			}

			bitmap.Dispose();
	
        	Console.WriteLine("\t[OK]");
			Console.ResetColor();
            return previewFile;
        }

        #endregion

		#region Dream Data Parsing

		/// <summary>
        /// Load data for a video dream
        /// </summary>
        /// <param name="video">the path to the video file</param>
        private void LoadVideoDream(string video)
        {
            Console.WriteLine("Building a video Dream");

			Console.ForegroundColor = ConsoleColor.Green;
            Console.Write("  Checking video file...");

			// max file size: 150MB = 157286400 bytes
			AddContent(LoadAndCheckFile(video, "", MAX_VIDEOSIZE, FILTER_VIDEO));
    
            Console.WriteLine("\t[OK]");  
			Console.ResetColor();
        }

        /// <summary>
        /// Load data for a trigger based dream
        /// </summary>
        /// <param name="triggers">a list of Trigger structures</param>
        private void LoadTriggersDream(DreamTriggers triggers)
        {	
            Console.WriteLine("Building a trigger-based Dream\n");

			Debug.Assert(triggers.type == "time");

			if (triggers.type == "time")
        		Console.WriteLine("Trigger is time-based.\n");

        	string triggersDefinition = Path.Combine(buildDir, "DreamTriggers.xml");
			AddContent(triggersDefinition);

			var triggersV1 = new TriggersV1
		                     {
		                         version = 100,
		                         fileVersion = "1.0",
		                         triggers = new TriggerSetV1
	                                        {
	                                            triggers =
	                                                new List<TriggerV1>(),
	                                            type = triggers.type
	                                        }
		                     };

            // Process the list of triggers
            //   - Check for duplicates
			//		+ triggers that start at the same time
			//		The case where there is only two triggers, but with the same file is not handled        	
			//   - Check & copy the files	
			//   - Build the DreamTriggersV1 structure				
        	int id = 1;
			var timeList = new List<DateTime>();
			var fileList = new List<string>();
			foreach (DreamTrigger trigger in triggers.triggers)
			{
				// Check unique time
				if (!timeList.Contains(trigger.time))
					timeList.Add(trigger.time);
				else
					OutputError("Another trigger is set to start at the same time", "Time: " + trigger.time + " - file: " + trigger.file, null);

				// Check that video is not already included
				if (!fileList.Contains(trigger.file))
				{
					fileList.Add(trigger.file);					
					AddContent(LoadAndCheckFile(trigger.file, buildDir, MAX_VIDEOSIZE, FILTER_VIDEO));
				}				

				// Add trigger
				var triggerV1 = new TriggerV1
		                        {
		                            id = id,
		                            video = Path.GetFileName(trigger.file),
		                            hour = trigger.time.Hour,
		                            minute = trigger.time.Minute,
		                            second = trigger.time.Second
		                        };

			    triggersV1.triggers.triggers.Add(triggerV1);

				Console.WriteLine("  Adding trigger: " + triggerV1.video + " at " + trigger.time.Hour + ":" + trigger.time.Minute + ":" + trigger.time.Second);

				id++;
			}

			// Check that there are at least two unique videos
			if (fileList.Count < 2)
				OutputError("At least two different videos are needed for trigger-based dreams", "Dream configuration uses only one video in the trigger section", null);

			// Output a trigger config file
			DreamConfiguration.Serialize(triggersV1, triggersDefinition);

			// Check that the file is in the correct format        	
			DreamConfiguration.ValidateConfiguration(new StringReader(File.ReadAllText(Path.Combine(buildDir, "DreamTriggers.xml"))), Resources.TriggersV1XSD);
        }
		

        /// <summary>
        /// Load data for a dynamic dream
        /// </summary>
        /// <param name="dynamic">the DreamDynamic Struct</param>
        private void LoadDynamicDream(DreamDynamic dynamic)
        {
            Console.WriteLine("Building a dynamic Dream");

            // Check 32-bit & 64-bit dlls
			Console.ForegroundColor = ConsoleColor.Green;
            Console.Write("  Checking 32-bit dll file...");
			string dll32 = LoadAndCheckFile(dynamic.dll32, "", NO_FILESIZE_CHECK, null);			
            Console.WriteLine("\t[OK]");
			AddContent(dll32);

            Console.Write("  Checking 64-bit dll file...");
			string dll64 = LoadAndCheckFile(dynamic.dll64, "", NO_FILESIZE_CHECK, null);
            Console.WriteLine("\t[OK]");
			Console.ResetColor();
			AddContent(dll64);

            Console.WriteLine("");
            // Load resources if any       
            foreach (DreamResource res in dynamic.resources) {
                if (String.IsNullOrEmpty(res.file))
                    continue;

				// Check for invalid characters in path
				// we have to do it ahead of time to be able to verify the path is relative
				// (otherwise we could get an InvalidArgumentException)
            	CheckForInvalidChars(res.path);

				// Check target resource path is relative to dream root folder
				if (Path.IsPathRooted(res.path))
					OutputError("Resource path needs to be relative to the dream root directory", res.path, null);

				
				//////////////////////////////////////////////////////////////////////////
				// HACK... (is there a nice & clean way to do that ?			
				// Check target path within build directory (ex: ../resources is not allowed)

				// normalize path (replace all / by \ and \\ by \)
            	string path = res.path.Replace("/", "\\").Replace("\\\\", "\\");

				// Edge cases
				// Path starting with \, . or ..
				while (path.StartsWith(".") || path.StartsWith("\\"))
				{
					path = path.TrimStart('\\');

					if (path.StartsWith(".."))
						OutputError("Resulting resource path is outside the build directory", res.path, null);

					path = path.TrimStart('.');
				}

            	// We split the path, count the number of ".." and compare it to the remaining number of parts
            	string[] parts = path.Split('\\');

            	int parent = 0;            	

				foreach (string part in parts)
				{
					if (string.IsNullOrEmpty(part))
						continue;

					if (part == "..")
						parent--;
					else if (part != ".")
						parent++;

					if (parent < 0)
						OutputError("Resulting resource path is outside the build directory", res.path, null);
				}
				//////////////////////////////////////////////////////////////////////////



				string additionalFile = LoadAndCheckFile(res.file, res.path, NO_FILESIZE_CHECK, null);

                Console.WriteLine("  Adding resource file: " + Path.GetFileName(res.file));

                AddContent(additionalFile);
            }

        }

        #endregion

    }
}
