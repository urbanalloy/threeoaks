DreamBuilder - a command line Dream builder
 © 2008 Julien Templier
 All Rights Reserved
---------------------------------------------------------------------------

DreamBuilder allows you to build dreams from the command line. It uses a
simple XML configuration file to load the info needed to build a Dream file.

Pre-requisites:
 - .Net Framework 2.0
 - DreamMaker v1.02+ should be installed (http://dream.wincustomize.com)
 
***************************************************************************
** Documentation
***************************************************************************

Usage:

DREAMBUILDER inputFile [/O<outputDirectory>] [/D<variable>=<value>]

    inputFile             Path the dream definition file
    /O<ouputDirectory>    Path to the output directory
    /D<variable>=<value>  Defines a variable to be replaced in the XML configuration file.
                          Several such variables can be defined.

Note: the default output directory is the current working directory.
	
DreamBuilder is checking the XML configuration file against an XML Schema,
so it will tell you if there is an error in your configuration (but the
message might be a bit cryptic). 

DreamBuilder allows you to use relative paths when defining the files to use.
It will look for the files relative to the current directory.

This might be a problem if you are executing it from somewhere else. This is
why I introduced variable replacement. This is a way to dynamically replace
content in the configuration file. I use it for path handling, but you can
also use it for other things such as adding a version number to the dream name.

The replacement step is happening before the file is parsed, so you can go crazy
if you want, and generate the resource list at compile time for example.

You can have as many variables as you want (simply add another variable
definition to the command line). To define a variable "name" with the value
"Julien", you use the following define: "/Dname=Julien". You can then add
a new variable to the XML configuration file: $name$. DreamBuilder will
warn you if a variable exists without a substitution.

***************************************************************************
** Changelog
***************************************************************************

Version 1.0.0.79:
	* Fixed typo in Example.xml file
	* Updated documentation

Version 1.0.0.56: First Release

***************************************************************************
** Licence
***************************************************************************
 Redistribution and use in source and binary forms, with or without modification, are
 permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright notice, this list of
     conditions and the following disclaimer. 
  2. Redistributions in binary form must reproduce the above copyright notice, this list
     of conditions and the following disclaimer in the documentation and/or other materials
     provided with the distribution. 
  3. The name of the author may not be used to endorse or promote products derived from this
     software without specific prior written permission. 

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
***************************************************************************