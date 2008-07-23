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
using System.IO;
using System.Text;
using System.Xml;
using System.Xml.Schema;
using MbUnit.Framework;

namespace DreamBuilder.Test
{
    [TestFixture]
    public class DreamConfigurationTest
    {
        private DreamConfiguration configuration;        

        [SetUp]
        public void Init()
        {
            configuration = new DreamConfiguration();
        }

        [Test]
		[ExpectedException(typeof(ArgumentNullException))]
        public void Test_InvalidInput()
        {
			configuration.Load(null);
        }

		[Test]
		[ExpectedException(typeof(XmlException))]
		public void Test_InvalidXmlChar()
		{
			string path = Path.Combine(Environment.CurrentDirectory, "Resources.Test\\XSDValidation\\Invalid\\InvalidXmlChar.xml");
			configuration.Load(File.ReadAllText(path));
		}

		[Test]
		[ExpectedException(typeof(XmlException))]
		public void Test_InvalidXmlFile()
		{
			string path = Path.Combine(Environment.CurrentDirectory, "Resources.Test\\XSDValidation\\Invalid\\InvalidXmlFile.xml");
			configuration.Load(File.ReadAllText(path));
		}

        [Test]
		[Row("NameNotPresent.xml")]
		[Row("NameTooLong.xml")]
		[Row("DescriptionNotPresent.xml")]
		[Row("DescriptionTooLong.xml")]
		[Row("AuthorNotPresent.xml")]
		[Row("AuthorTooLong.xml")]
		[Row("AuthorUrlNotPresent.xml")]
		[Row("AuthorUrlTooLong.xml")]
		[Row("PreviewNotPresent.xml")]
		[Row("DataNotPresent.xml")]		
		[Row("VideoNotPresent.xml")]
		[Row("TriggerNoType.xml")]
		[Row("TriggerTypeUnknown.xml")]
		[Row("TriggerTimeNotPresent.xml")]
		[Row("TriggerTimeInvalidFormat.xml")]
		[Row("TriggerFileNotPresent.xml")]					
		[Row("TriggerUniqueTrigger.xml")]
		[Row("DynamicDll32NotPresent.xml")]
		[Row("DynamicDll64NotPresent.xml")]
		[Row("DynamicResourcesEmpty.xml")]
		[Row("DynamicFileNotPresent.xml")]
		[Row("DynamicMoreThan250Resources.xml")]
		[Row("DataContentNotPresent.xml")]   		
		[ExpectedException(typeof(XmlSchemaException))]
        public void Test_InvalidConfigurationFormat(string file)
        {			
            string invalidXSDPath = Path.Combine(Environment.CurrentDirectory, "Resources.Test\\XSDValidation\\Invalid");

            string path = Path.Combine(invalidXSDPath, file);
			configuration.Load(File.ReadAllText(path));
        }

        [Test]	
		[Row("ValidVideoDream.xml")]
		[Row("ValidTriggerDream.xml")]
		[Row("ValidDynamicDream.xml")]
		[Row("ValidDynamicDreamNoResources.xml")]					
		public void Test_ValidConfigurationFormat(string file)
		{
			string validXSDPath = Path.Combine(Environment.CurrentDirectory, "Resources.Test\\XSDValidation\\Valid");

			string path = Path.Combine(validXSDPath, file);
			configuration.Load(File.ReadAllText(path));
		}

		[Test]
		public void Test_DreamTypeVideo()
		{
			string validXSDPath = Path.Combine(Environment.CurrentDirectory, "Resources.Test\\XSDValidation\\Valid\\");

			string path = Path.Combine(validXSDPath, "ValidVideoDream.xml");
			configuration.Load(File.ReadAllText(path));

			Assert.AreEqual(configuration.Type, DreamType.Video);
		}

		[Test]
		public void Test_DreamTypeTrigger()
		{
			string validXSDPath = Path.Combine(Environment.CurrentDirectory, "Resources.Test\\XSDValidation\\Valid\\");

			string path = Path.Combine(validXSDPath, "ValidTriggerDream.xml");
			configuration.Load(File.ReadAllText(path));

			Assert.AreEqual(configuration.Type, DreamType.Trigger);
		}

        [Test]
		[Row("ValidDynamicDream.xml")]
		[Row("ValidDynamicDreamNoResources.xml")]
		public void Test_DreamTypeDynamic(string file)
		{
			string validXSDPath = Path.Combine(Environment.CurrentDirectory, "Resources.Test\\XSDValidation\\Valid\\");

			string path = Path.Combine(validXSDPath, file);
			configuration.Load(File.ReadAllText(path));

			Assert.AreEqual(configuration.Type, DreamType.Dynamic);
		}


    }
}
