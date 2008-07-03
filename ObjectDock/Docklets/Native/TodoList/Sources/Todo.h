///////////////////////////////////////////////////////////////////////////////////////////////
//
// TodoList : a ToDo Docklet for ObjectDock
//
// (c) 2004 - Julien Templier
// Parts from Weather Docklet by Jeff Bargmann
//
// This work is licensed under the Creative Commons
// Attribution-ShareAlike License. To view a copy of this license, visit
// http://creativecommons.org/licenses/by-sa/2.0/ or send a letter to
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>

#include <Docklet/Docklet.h>
#include <Docklet/Console.h>

#include <string>
#include <vector>
using namespace std;

class Todo
{
	public:
		//////////////////////////////////////////////////////
		// Priorities
		const static int PrioritySize = 6;
		const static char* PriorityNames[]; 

		enum Priority
		{
			NOPRIORITY,
			NONURGENT,
			NOTIMPORTANT,
			NORMAL,
			IMPORTANT,
			URGENT,
		};

		const static int NO_TASK = -1;

		//////////////////////////////////////////////////////	
		const static int SIZE_NAME			= 50;
		const static int SIZE_DESCRIPTION	= 500;

	private:
		//////////////////////////////////////////////////////
		// Values
		const static int SIZE_OLD_DATE		= 10;	// dd/mm/yyyy (v1.1)
		const static int SIZE_DATE			= 16;	// dd/mm/yyyy-hh:mm (v1.2 and later)

		//////////////////////////////////////////////////////
		// Strings
		const static char* INI_NAME;
		const static char* INI_TITLE;
		const static char* INI_PRIORITY;
		const static char* INI_DESCRIPTION;
		const static char* INI_ICON;
		const static char* INI_IMAGE;
		const static char* INI_TASK;
		const static char* INI_DATE;

		const static char* TASKICON;
		const static char* REMINDERICON;
		const static char* DEFAULTICON;
		const static char* ICONOVERDUE;
		const static char* ICONREMINDER;
		const static char* INITASKS;
		const static char* INITASKSORIG;
		const static char* ICONFOLDER;
		const static char* TASKFOLDER;
		const static char* PRIORITYFOLDER;

		//////////////////////////////////////////////////////
		// Private Data
		struct DATA
		{
			string	   name;
			Priority   priority;
			string	   description;
			string	   icon;

			// Date
			SYSTEMTIME date;
			bool	   hasDate;
			bool	   hasTime;

			DATA()
			{
				name = string("");
				priority = NOPRIORITY;
				description = string("");
                icon = "";
				hasDate = false;
				hasTime = false;

				date.wDay = 0; date.wDayOfWeek = 0; date.wHour = 0; date.wMilliseconds = 0;
				date.wMinute = 0; date.wMonth = 0; date.wSecond = 0; date.wYear = 0;
			}
		};

		struct TASKS
		{
			int	currentTask;	// Index of the current task
			vector<string> names;
			vector<string> paths;

			TASKS()
			{
				currentTask = NO_TASK;
			}
		};
		//////////////////////////////////////////////////////

		Docklet* docklet;
		ODConsole* console;
		TASKS* tasks;
		DATA* data;

		HANDLE taskIcon;
		HANDLE reminderIcon;
        
		// Functions
		void LoadTasksFromIniFile();

		// Save related
		string SystemTime2Date(SYSTEMTIME date);
		void Date2SystemTime(char* date, SYSTEMTIME* time);
		char* ConvertToMultiLine(char* text);
		char* ConvertFromMultiLine(char* text);


	public:
		Todo(Docklet* docklet, ODConsole* console);
		~Todo();

		//////////////////////////////////////////////////////
		// Data 
		string GetName() { return data->name; }
		void setName(string name) { data->name = name; }

		Priority GetPriority() { return data->priority; }
		void SetPriority(Priority priority) { data->priority = priority; }

		string GetDescription() { return data->description; }
		void setDescription(string description) { data->description = description; }

		void SetDefaultIcon();
		void SetIcon(string file) { data->icon = file; tasks->currentTask = NO_TASK; }
		string GetIcon() { return data->icon; }
		string GetIconOverdue();

		bool HasDate() { return data->hasDate; }
		bool HasTime() { return data->hasTime; }
		SYSTEMTIME* GetDate() { return &data->date; }

		bool IsOverdue();
		void SetNoDate() { data->hasDate = false; console->Output("No Date\n"); }
		void SetNoTime() { data->hasTime = false; console->Output("No Time\n"); }
		void SetDate(SYSTEMTIME date) { data->date = date; data->hasDate = true; }
		void SetTime(SYSTEMTIME time) { data->date = time; data->hasTime = true; }

		vector<string> GetTasksNames() { return tasks->names; }
		int GetCurrentTask() { return tasks->currentTask; }
		void SetCurrentTask(int index);

		//////////////////////////////////////////////////////
		void LoadSettings(char *szIni, char *szIniGroup);
		void LoadDefaultSettings();
		void SaveSettings(char *szIni, char *szIniGroup, bool isForExport);

		//////////////////////////////////////////////////////
		Image* CreateOverlayImage();

		HANDLE GetTaskIcon();
		HANDLE GetReminderIcon();

};
