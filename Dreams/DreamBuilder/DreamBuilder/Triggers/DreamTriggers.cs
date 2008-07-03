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
using System.IO;
using System.Text;
using System.Xml;
using System.Xml.Serialization;

namespace DreamBuilder.Triggers
{
	/// <summary>
	/// Store the trigger-based dream info
	/// </summary>
	[XmlRoot("dream")]
	public struct TriggersV1
	{
		/// <summary>
		/// File version
		/// </summary>
		[XmlAttribute("version")]
		public string fileVersion;

		/// <summary>
		/// Trigger version
		/// </summary>
		[XmlElement("version")]
		public int version;

		/// <summary>
		/// List of dynamic dream resources
		/// </summary>

		[XmlElement("triggerset")]
		public TriggerSetV1 triggers;
	}

	/// <summary>
	/// Store a list of triggers
	/// </summary>
	[XmlRoot("triggerset")]
	public struct TriggerSetV1
	{
		/// <summary>
		/// Type of triggerset
		///  - time (only one allowed for now)
		/// </summary>
		[XmlAttribute("type")]
		public string type;
		
		[XmlElement("trigger")]
		public List<TriggerV1> triggers;
	}

	/// <summary>
	/// Store a trigger
	/// </summary>	
	public struct TriggerV1
	{
		/// <summary>
		/// ID of the trigger
		/// </summary>
		[XmlAttribute("id")]
		public int id;

		/// <summary>
		/// Video file name
		/// </summary>
		[XmlAttribute("video")]
		public string video;

		/// <summary>
		/// Hour to trigger the video
		/// </summary>
		[XmlAttribute("hour")]
		public int hour;

		/// <summary>
		/// Minute to trigger the video
		/// </summary>
		[XmlAttribute("minute")]
		public int minute;

		/// <summary>
		/// Second to trigger the video
		/// </summary>
		[XmlAttribute("second")]
		public int second;


	}
}
