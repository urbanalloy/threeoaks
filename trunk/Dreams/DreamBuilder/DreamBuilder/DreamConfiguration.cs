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
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using System.IO;
using System.Xml.Schema;
using DreamBuilder.Properties;

namespace DreamBuilder
{

    /// <summary>
    /// Store a time trigger info
    /// </summary>
    [XmlRoot("trigger")]
    public class DreamTrigger
    {
		/// <summary>
		/// Path to the file
		/// Path is either relative to the configuration file or a full path
		/// </summary>
		[XmlElement("path")]
		public string file;  

		/// <summary>
		/// Time to trigger the video
		/// </summary>
		[XmlElement("time")]
		public DateTime time; 
    }

	/// <summary>
	/// Store a trigger set
	/// </summary>
	[XmlRoot("triggers")]
	public class DreamTriggers
	{
		/// <summary>
		/// Type of triggerset
		/// </summary>
		[XmlAttribute("type")]
		public string type;

		/// <summary>
		/// Time to trigger the video
		/// </summary>
		[XmlElement("trigger")]
		public List<DreamTrigger> triggers;
	}

    /// <summary>
    /// Store a single dynamic dream resource
    /// </summary>
    [XmlRoot("resource")]
    public struct DreamResource 
    {
        /// <summary>
        /// Path to the file
        /// Path is either relative to the configuration file or a full path
        /// </summary>
        [XmlElement("file")]
        public string file;

        /// <summary>
        /// Target path in the resulting dream file
        /// It must be a relative path (ex: "Resources" to put the file in "[dream root]/Resources/")
        /// </summary>
        [XmlElement("path")]
        public string path;
    }

    /// <summary>
    /// Dynamic Dream data info
    /// </summary>
    [XmlRoot("dynamic")]
    public struct DreamDynamic 
    {
        /// <summary>
        /// Path to the 32-bit dynamic dream dll
        /// Path is either relative to the configuration file or a full path
        /// </summary>
        [XmlElement("dll32")]
        public string dll32;

        /// <summary>
        /// Path to the 64-bit dynamic dream dll
        /// Path is either relative to the configuration file or a full path
        /// </summary>
        [XmlElement("dll64")]
        public string dll64;

        /// <summary>
        /// List of dynamic dream resources
        /// </summary>
        [XmlArray("resources")]
        [XmlArrayItem("resource")]
        public List<DreamResource> resources;
    }

    /// <summary>
    /// Dream Data
    /// </summary>
    [XmlRoot("data")]
    public struct DreamData
    {
        /// <summary>
        /// Path to the video file
        /// Path is either relative to the configuration file or a full path
        /// </summary>
        [XmlElement("video")]
        public string video;

        /// <summary>
        /// List of triggers
        /// </summary>
        [XmlElement("triggers")]
		public DreamTriggers triggers;

        /// <summary>
        /// Dynamic dream info
        /// </summary>
        [XmlElement("dynamic")]
        public DreamDynamic dynamic;
    }

    /// <summary>
    /// Dream configuration
    /// </summary>
    [XmlRoot("dream")]
    public struct DreamConfig
    {
        [XmlElement("name")]
        public string name;

        [XmlElement("description")]
        public string description;

        [XmlElement("author")]
        public string author;

        [XmlElement("url")]
        public string url;

        [XmlElement("copyright")]
        public string copyright;

        [XmlElement("permissions")]
        public string permissions;

        [XmlElement("preview")]
        public string preview;

        [XmlElement("data")]
        public DreamData data;
    }

    /// <summary>
    /// Type of Dream 
    /// </summary>
    public enum DreamType
    {
        Invalid,
        Video,
        Trigger,
        Dynamic
    }

    public class DreamConfiguration
    {
        private DreamConfig config;

#if DEBUG
    	private static StringBuilder SerializeOutput;
#endif

        #region Properties

        public DreamConfig Config
        {
            get { return config; }
            set { config = value; }
        }

        public DreamType Type
        {
            get
            {
                if (!String.IsNullOrEmpty(config.data.video))
                    return DreamType.Video;

				if (config.data.triggers != null && config.data.triggers.triggers != null && config.data.triggers.triggers.Count > 0)
                    return DreamType.Trigger;

                if (!String.IsNullOrEmpty(config.data.dynamic.dll32))
                    return DreamType.Dynamic;

                return DreamType.Invalid;
            }
        }

        #endregion
        
        /// <summary>
        /// Load a dream configuration file
        /// </summary>
        /// <param name="buffer">a buffer containing the document to load</param>
    	public void Load(string buffer) {
			ValidateConfiguration(new StringReader(buffer), Resources.DreamBuilderXSD);

			var document = new XmlDocument();
			document.Load(new StringReader(buffer));

            config = (DreamConfig)Deserialize(document, typeof(DreamConfig));      
        }

        #region Schema validation

        /// <summary>
        /// Check the configuration against the schema:
        ///     + Check the presence of mandatory fields (except for at least one node inside of data)
        ///     + Check the proper format of the fields
        ///         - max length
        ///         - max number of items       
        /// </summary>
        public static void ValidateConfiguration(StringReader file, string schema) {                                   	

            // Set the validation settings.
            var settings = new XmlReaderSettings {ValidationType = ValidationType.Schema};
            settings.ValidationFlags |= XmlSchemaValidationFlags.ProcessSchemaLocation | 
									    XmlSchemaValidationFlags.ReportValidationWarnings;
			settings.Schemas.Add(null, new XmlTextReader(new StringReader(schema)));
            settings.ValidationEventHandler += ValidationCallBack;

			using (XmlReader reader = XmlReader.Create(file, settings))
				while (reader.Read()) {}			

        }

        public static void ValidationCallBack(object sender, ValidationEventArgs e)
        {
			if (e.Severity == XmlSeverityType.Warning)
				Console.WriteLine("Warning: " + e.Message);
			else
				throw e.Exception;
        }

        #endregion

        #region Serialization

        /// <summary>
        /// Deserializes an xml document back into an object
        /// </summary>
        /// <param name="xml">The xml data to deserialize</param>
        /// <param name="type">The type of the object being deserialized</param>
        /// <returns>A deserialized object</returns>
        public static object Deserialize(XmlNode xml, Type type)
        {
            var s = new XmlSerializer(type);
            string xmlString = xml.OuterXml;
            byte[] buffer = Encoding.UTF8.GetBytes(xmlString);
            var ms = new MemoryStream(buffer);
            XmlReader reader = new XmlTextReader(ms);
            Exception caught = null;

            try
            {
                object o = s.Deserialize(reader);
                return o;
            }

            catch (Exception e)
            {
                caught = e;
            }
            finally
            {
                reader.Close();

                if (caught != null)
                    throw caught;
            }
            return null;
        }

        /// <summary>
        /// Serializes an object into an Xml Document
        /// </summary>
        /// <param name="o">The object to serialize</param>
        /// <param name="file">The path to the file to serialize to</param>        
        public static void Serialize(object o, string file)
        {
            var s = new XmlSerializer(o.GetType());

            var fs = new FileStream(file, FileMode.OpenOrCreate, FileAccess.Write);             
            var writer = new XmlTextWriter(fs, new UnicodeEncoding())
                         {
                             Formatting = Formatting.Indented,
                             IndentChar = ' ',
                             Indentation = 4
                         };
            Exception caught = null;

            try
            {
                s.Serialize(writer, o);  
              
#if DEBUG
				SerializeOutput = new StringBuilder();
				s.Serialize(new StringWriter(SerializeOutput), o);
#endif
            }
            catch (Exception e)
            {
                caught = e;
            }
            finally
            {
                writer.Close();
                fs.Close();

                if (caught != null)
                    throw caught;
            }
        }

        #endregion

    }
}
