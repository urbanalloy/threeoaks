///////////////////////////////////////////////////////////////////////////////////////////////
//
// Dream command line builder
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
using System.IO;
using MbUnit.Framework;

namespace DreamBuilder.Test
{
    [TestFixture]
    public class DreamBuilderTest
    {
        private DreamBuilder builder;
    	private string resourceFolder;

        [SetUp]
        public void Init()
        {			
			resourceFolder = Path.Combine(Environment.CurrentDirectory, "Resources.Test");
		}

		public void CreateNewEmptyFile(string path, Int64 size)
		{
			if (!Directory.Exists(Path.GetDirectoryName(path)))			
				Directory.CreateDirectory(Path.GetDirectoryName(path));

			if (File.Exists(path)) {
				FileInfo info = new FileInfo(path);
				if (info.Length == size)
					return;				
			}

			FileStream file = File.Create(path);
			
			for(int i = 0; i < size; i++){
				file.WriteByte(0);
			}

			file.Close();			
		}

		#region Basic Tests

		[Test]
		[ExpectedException(typeof(BuilderException), "Input file not initialized")]		
        public void Test_NoInputFile()
        {		
			builder = new DreamBuilder(null);
            builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "Input file not found")]
		public void Test_InputFileNotFound()
		{
			builder = new DreamBuilder("NotAFileConfig.xml");
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "Invalid output directory")]
		public void Test_InputInvalidOutputDirectory()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamVideo\\VideoFormatAVI.xml"), "QZX:\\Directory");
			builder.Build();
		}

		[RowTest]
		[Row("BasicInvalidFileExe.xml")]
		[Row("BasicInvalidFileScr.xml")]
		[ExpectedException(typeof(BuilderException), "File is not in a valid format")]
		public void Test_ForbiddenFiles(string file)
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "Basic\\" + file), null, null);
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "Filename contains invalid characters")]
		public void Test_InvalidDreamName()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "Basic\\BasicInvalidName.xml"), null, null);
			builder.Build();
		}

		#endregion

		#region Variable Replacement
		
		// Make sure that we can replace big chunks of the config via variable replacement
		// For example, we might want to dynamically build the list of resources at compile time.
		[Test]		
		public void Test_BigReplacement()
		{
			Assert.Fail();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "No value specified for variable")]
		public void Test_UnknownVariable()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "Variables\\UnknownVariable.xml"), null, null);
			builder.Build();
		}

		[RowTest]
		[Row("VariableReplacementVideo.xml")]
		[Row("VariableReplacementTrigger.xml")]
		[Row("VariableReplacementDynamic.xml")]
		public void Test_VariableReplacement(string file)
		{
			Dictionary<string, string>  defines = new Dictionary<string, string>();
			defines.Add("name", "Variable Replacement");
			defines.Add("description", "Description Variable Replacement");
			defines.Add("author", "Julien Templier");
			defines.Add("url", "http://julien.wincustomize.com");
			defines.Add("copyright", "(C) 2007, Three Oaks Crossing, LLC");
			defines.Add("permissions", "N/A");
			defines.Add("preview", "../preview.jpg");

			defines.Add("video", "../Videos/TestVideo.avi");

			defines.Add("time1", "01:00:00");
			defines.Add("path1", "../Videos/TestVideo.avi");
			defines.Add("time2", "02:00:00");
			defines.Add("path2", "../Videos/TestVideo.mpg");

			defines.Add("dll32", "../DreamDynamic/resources/dynamic32.dll");
			defines.Add("dll64", "../DreamDynamic/resources/dynamic32.dll");
			defines.Add("file1", "../preview.jpg");
			defines.Add("path_res1", "resources");

			builder = new DreamBuilder(Path.Combine(resourceFolder, "Variables\\" + file), null, defines);
			builder.Build();
		}

		#endregion

		#region Preview File

		[Test]
		[ExpectedException(typeof(BuilderException), "File not found")]
    	public void Test_PreviewFileNotFound()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "PreviewFile\\PreviewFileNotFound.xml"));
			builder.Build();
    	}

		[Test]
		[ExpectedException(typeof(BuilderException), "Path contains invalid characters")]
		public void Test_PreviewFileInvalidCharsInPath()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "PreviewFile\\PreviewFileInvalidCharsInPath.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "Filename contains invalid characters")]
		public void Test_PreviewFileInvalidCharsInFilename()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "PreviewFile\\PreviewFileInvalidCharsInFilename.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "File is empty")]
		public void Test_PreviewFileSizeInvalid()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "PreviewFile\\PreviewFileSizeInvalid.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "File is not in a valid format")]
		public void Test_PreviewInvalidImageFormatExtension()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "PreviewFile\\PreviewInvalidImageFormatExtension.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "Preview file is not in a valid format (.JPG, .JPEG, .PNG, .BMP)")]
		public void Test_PreviewInvalidImageFormat()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "PreviewFile\\PreviewInvalidImageFormat.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "Invalid preview file")]
		public void Test_PreviewInvalidImage()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "PreviewFile\\PreviewInvalidImage.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "Image is larger than 256x256")]
		public void Test_PreviewInvalidImageDimensions()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "PreviewFile\\PreviewInvalidImageDimensions.xml"));
			builder.Build();
		}

		[RowTest]
		[Row("PreviewFormatBMP.xml")]
		[Row("PreviewFormatJPEG.xml")]
		[Row("PreviewFormatJPG.xml")]
		[Row("PreviewFormatPNG.xml")]
		public void Test_PreviewImageFormat(string file)
		{			
			builder = new DreamBuilder(Path.Combine(resourceFolder, "PreviewFile\\" + file));
			builder.Build();
		}


		#endregion

		#region Video Dream

		[Test]
		[ExpectedException(typeof(BuilderException), "File not found")]
		public void Test_VideoFileNotFound()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamVideo\\VideoFileNotFound.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "Path contains invalid characters")]
		public void Test_VideoInvalidCharsInPath()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamVideo\\VideoInvalidCharsInPath.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "Filename contains invalid characters")]
		public void Test_VideoInvalidCharsInFilename()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamVideo\\VideoInvalidCharsInFilename.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "File is empty")]
		public void Test_VideoFileSizeInvalid()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamVideo\\VideoFileSizeInvalid.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "File is not in a valid format")]
		public void Test_VideoInvalidFormat()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamVideo\\VideoInvalidFormat.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "File is too big")]
		public void Test_VideoFileSizeTooBig()
		{
			CreateNewEmptyFile(Path.Combine(resourceFolder, "DreamVideo\\FileSizeTooBig.avi"), 152 * 1024 * 1024);

			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamVideo\\VideoFileSizeTooBig.xml"));
			builder.Build();
		}

		[RowTest]
		[Row("VideoFormatAVI.xml")]
		[Row("VideoFormatMPEG.xml")]
		[Row("VideoFormatMPG.xml")]
		[Row("VideoFormatWMV.xml")]
		public void Test_VideoFormat(string file)
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamVideo\\" + file));
			builder.Build();
		}

		#endregion

		#region Trigger-Based Dream

		[Test]
		[ExpectedException(typeof(BuilderException), "File not found")]
		public void Test_TriggerFileNotFound()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamTrigger\\TriggerFileNotFound.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "Path contains invalid characters")]
		public void Test_TriggerInvalidCharsInPath()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamTrigger\\TriggerInvalidCharsInPath.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "Filename contains invalid characters")]
		public void Test_TriggerInvalidCharsInFilename()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamTrigger\\TriggerInvalidCharsInFilename.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "File is empty")]
		public void Test_TriggerFileSizeInvalid()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamTrigger\\TriggerFileSizeInvalid.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "File is not in a valid format")]
		public void Test_TriggerInvalidFormat()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamTrigger\\TriggerInvalidFormat.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "File is too big")]
		public void Test_TriggerFileSizeTooBig()
		{
			CreateNewEmptyFile(Path.Combine(resourceFolder, "DreamTrigger\\FileSizeTooBig.avi"), 152 * 1024 * 1024);

			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamTrigger\\TriggerFileSizeTooBig.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "Another trigger is set to start at the same time")]
		public void Test_TriggerSameTime()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamTrigger\\TriggerSameTime.xml"));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "At least two different videos are needed for trigger-based dreams")]
		public void Test_TriggerSameVideo()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamTrigger\\TriggerSameVideo.xml"));
			builder.Build();
		}

		[RowTest]
		[Row("TriggerFormatAVI-WMV.xml")]
		[Row("TriggerFormatMPG-MPEG.xml")]	
		[Row("TriggerTwoIdenticalFiles.xml")]
		public void Test_TriggerFormat(string file)
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamTrigger\\" + file));
			builder.Build();
		}

		#endregion

		#region Dynamic Dream

		[RowTest]
		[Row("DynamicDll32NotFound.xml")]
		[Row("DynamicDll64NotFound.xml")]
		[Row("DynamicResourceNotFound.xml")]
		[ExpectedException(typeof(BuilderException), "File not found")]
		public void Test_DynamicDllNotFound(string file)
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamDynamic\\" + file));
			builder.Build();
		}

		[RowTest]
		[Row("DynamicInvalidCharsInPathDll32.xml")]
		[Row("DynamicInvalidCharsInPathDll64.xml")]
		[Row("DynamicInvalidCharsInPathResourceFile.xml")]
		[Row("DynamicInvalidCharsInPathResourcePath.xml")]
		[ExpectedException(typeof(BuilderException), "Path contains invalid characters")]
		public void Test_DynamicInvalidCharsInPath(string file)
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamDynamic\\" + file));
			builder.Build();
		}

		[RowTest]
		[Row("DynamicInvalidCharsInFilenameDll32.xml")]
		[Row("DynamicInvalidCharsInFilenameDll64.xml")]
		[Row("DynamicInvalidCharsInFilenameResourceFile.xml")]
		[ExpectedException(typeof(BuilderException), "Filename contains invalid characters")]
		public void Test_DynamicInvalidCharsInFilename(string file)
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamDynamic\\" + file));
			builder.Build();
		}

		[Test]
		[ExpectedException(typeof(BuilderException), "Resource path needs to be relative to the dream root directory")]
		public void Test_DynamicResourcePathFull()
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamDynamic\\DynamicResourcePathFull.xml"));
			builder.Build();			
		}

		// There might be no resource in the parent folder, in which case the dream is not properly created
		[Test]		
		public void Test_ResourceInSubFolder()
		{
			Assert.Fail();
		}

		[RowTest]
		[Row("../resources")]
		[Row("../resources/")]
		[Row("./../resources")]
		[Row("././../resources")]
		[Row("..//.\\resources")]
		[Row("..//.\\resources/../..")]
		[Row("resources/.././../test/double")]
		[Row("resources/.././test/../../test/double")]
		[ExpectedException(typeof(BuilderException), "Resulting resource path is outside the build directory")]
		public void Test_DynamicResourcePathOutsideBuildDirectory(string path)
		{
			Dictionary<string, string> defines = new Dictionary<string, string>();
			defines.Add("test_path", path);			

			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamDynamic\\DynamicResourcePathOutsideBuildDirectory.xml"), null, defines);
			builder.Build();
		}

		[RowTest]
		[Row("DynamicNoResources.xml")]
		[Row("DynamicResourcesWithoutPath.xml")]
		[Row("DynamicResourcesWithPath.xml")]		
		public void Test_DynamicDream(string file)
		{
			builder = new DreamBuilder(Path.Combine(resourceFolder, "DreamDynamic\\" + file));
			builder.Build();
		}

		#endregion
	}
}
