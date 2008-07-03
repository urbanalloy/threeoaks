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

#include "Todo.h"

#pragma warning (disable: 4100)
#include <atlstr.h>

const char* Todo::PriorityNames[] = {"0 - No Priority",
									 "1 - Not Urgent",
									 "2 - Not Important",
									 "3 - Normal",
									 "4 - Important",
									 "5 - Urgent"};

const char* Todo::INI_NAME			= "Name";
const char* Todo::INI_TITLE			= "Title";
const char* Todo::INI_PRIORITY		= "Priority";
const char* Todo::INI_DESCRIPTION	= "Description";
const char* Todo::INI_ICON			= "Icon";
const char* Todo::INI_IMAGE			= "Image";
const char* Todo::INI_TASK			= "Task";
const char* Todo::INI_DATE			= "Date";

const char* Todo::TASKICON			= "TodoList.ico";
const char* Todo::REMINDERICON		= "Reminder.ico";
const char* Todo::DEFAULTICON		= "TodoList.png";
const char* Todo::ICONOVERDUE		= "TaskOverdue.png";
const char* Todo::ICONREMINDER		= "TaskReminder.png";
const char* Todo::INITASKS			= "tasks.ini";
const char* Todo::INITASKSORIG		= ".orig";
const char* Todo::ICONFOLDER		= "icons\\";
const char* Todo::TASKFOLDER		= "tasks\\";
const char* Todo::PRIORITYFOLDER	= "priority\\";

Todo::Todo(Docklet* docklet, ODConsole* console)
{
	this->docklet = docklet;
	this->console = console;
	data = new DATA;
	tasks = new TASKS;
	taskIcon = NULL;
	reminderIcon = NULL;

	LoadTasksFromIniFile();
}

Todo::~Todo()
{
	if (taskIcon != NULL)
		DestroyIcon((HICON)taskIcon);
	if (reminderIcon != NULL)
		DestroyIcon((HICON)reminderIcon);
	delete(data);
	delete(tasks);
}


void Todo::LoadSettings(char *szIni, char *szIniGroup)
{
	char name[SIZE_NAME];
	char description[SIZE_DESCRIPTION];
	char icon[MAX_PATH];
	strcpy(name, "");
	strcpy(description, "");
	strcpy(icon, "");

	//load the plugin's recorded data
	GetPrivateProfileString(szIniGroup, INI_TITLE, "", (char*)&name, SIZE_NAME*sizeof(char), szIni);
	data->priority = (Priority)GetPrivateProfileInt(szIniGroup, INI_PRIORITY, NOPRIORITY, szIni);
	GetPrivateProfileString(szIniGroup, INI_DESCRIPTION, "", (char*)&description, SIZE_DESCRIPTION*sizeof(char) , szIni);
	GetPrivateProfileString(szIniGroup, INI_IMAGE, "", (char*)&icon, MAX_PATH*sizeof(char), szIni);
	tasks->currentTask = GetPrivateProfileInt(szIniGroup, INI_TASK, NO_TASK, szIni);

	data->name = string(name);
	data->description = string(description);
	data->icon = string(icon);

	// load the date
	char date[SIZE_DATE+1];
	strcpy(date, "");
	GetPrivateProfileString(szIniGroup, INI_DATE, "", date, (SIZE_DATE+1)*sizeof(char), szIni);
	Date2SystemTime(date, &(data->date));

	docklet->SetImageFile(data->icon);
	docklet->SetLabel(data->name);

	// convert multiline
	data->description = string(ConvertFromMultiLine((char *)data->description.c_str()));

	docklet->SetImageFile(data->icon);

	//Create a priority overlay image and set it on top of our loaded file image.
	docklet->SetImageOverlay(CreateOverlayImage());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Load the default settings and set the title appropriately
void Todo::LoadDefaultSettings()
{
	//Set the docklet's image to the default image.
	SetDefaultIcon();
	docklet->SetImageFile(data->icon);
	docklet->SetLabel("Click to Edit...");
}

///////////////////////////////////////////////////////////////////////////////////////////////
//	Save Settings
void Todo::SaveSettings(char *szIni, char *szIniGroup, bool isForExport)
{
	//WritePrivateProfileString(szIniGroup, INI_NAME, data->name.c_str(), szIni);
	WritePrivateProfileInt(szIniGroup, INI_PRIORITY, data->priority, szIni);
	WritePrivateProfileString(szIniGroup, INI_DESCRIPTION, ConvertToMultiLine((char*)data->description.c_str()), szIni);
	//WritePrivateProfileString(szIniGroup, INI_ICON, data->icon.c_str(), szIni);

	if (data->hasDate)	
		WritePrivateProfileString(szIniGroup, INI_DATE, SystemTime2Date(data->date).c_str(), szIni);

	WritePrivateProfileInt(szIniGroup, INI_TASK, tasks->currentTask, szIni);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Set the default icon
void Todo::SetDefaultIcon()
{
	data->icon  = docklet->GetRelativeFolder() +
				  string(ICONFOLDER) +
				  string(DEFAULTICON);

	tasks->currentTask = NO_TASK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
string Todo::GetIconOverdue()
{
	return docklet->GetRelativeFolder() + string(ICONFOLDER) + string(ICONOVERDUE);
}

HANDLE Todo::GetTaskIcon()
{
	if (taskIcon != NULL)
		return taskIcon;

	char iconPath[MAX_PATH+1];
	sprintf(iconPath, "%s%s%s%s", docklet->GetRootFolder().c_str(),
								  docklet->GetRelativeFolder().c_str(),
								  ICONFOLDER,
								  TASKICON);

	// Load the icon
	taskIcon = LoadImage(docklet->GetInstance(),
						 (char*)&iconPath,
						 IMAGE_ICON,
						 0,
						 0,
						 LR_DEFAULTSIZE | LR_LOADFROMFILE);

	return taskIcon;
}

HANDLE Todo::GetReminderIcon()
{
	if (reminderIcon != NULL)
		return reminderIcon;

	char iconPath[MAX_PATH+1];
	sprintf(iconPath, "%s%s%s%s", docklet->GetRootFolder().c_str(),
								  docklet->GetRelativeFolder().c_str(),
								  ICONFOLDER,
								  REMINDERICON);

	// Load the icon
	reminderIcon = LoadImage(docklet->GetInstance(),
							 (char*)&iconPath,
							 IMAGE_ICON,
							 0,
							 0,
							 LR_DEFAULTSIZE | LR_LOADFROMFILE);

	return reminderIcon;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void Todo::SetCurrentTask(int index)
{
	data->icon = docklet->GetRelativeFolder() +
				 string(ICONFOLDER) +
				 string(TASKFOLDER) +
				 tasks->paths[index];

	tasks->currentTask = index;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Create an overlay image with the Priority of the task on top of that image.
Image* Todo::CreateOverlayImage()
{
	// No overlay necessary
	if (data->priority == NOPRIORITY && !IsOverdue()) {
		// necessary to erase previous overlay
		Bitmap *imageReturn = new Bitmap(128, 128, PixelFormat32bppARGB);
		return imageReturn;
	}

	// Create bitmap
	Bitmap *imageReturn = new Bitmap(128, 128, PixelFormat32bppARGB);

	Graphics graphics(imageReturn);
	graphics.SetInterpolationMode(InterpolationModeHighQuality);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);

	/////////////////////////////////////
	//		PRIORITY
	/////////////////////////////////////
	if (data->priority != NOPRIORITY)
	{
		char priority[MAX_PATH+10];
		sprintf(priority, "%s%s%s%i.png", docklet->GetRelativeFolder().c_str(), ICONFOLDER, PRIORITYFOLDER, data->priority);

		// Load Icon
		Bitmap* priorityImage = docklet->LoadGDIPlusImage(priority);

		// Draw overlay
		if (priorityImage != NULL) {
			graphics.DrawImage(priorityImage, 80, 80, 48, 48);
			DeleteObject(priorityImage);
		}

	}

	/////////////////////////////////////
	//		OVERDUE
	/////////////////////////////////////
	if (IsOverdue())
	{
		char overdue[MAX_PATH+10];
		sprintf(overdue, "%s%s%s", docklet->GetRelativeFolder().c_str(), ICONFOLDER, ICONOVERDUE);

		// Load Icon
		Bitmap* overdueImage = docklet->LoadGDIPlusImage(overdue);

		// Draw overlay
		if (overdueImage != NULL) {
			graphics.DrawImage(overdueImage, 0, 80, 48, 48);
			DeleteObject(overdueImage);
		}
	}

	return imageReturn;

}

///////////////////////////////////////////////////////////////////////////////////////////////
// Return true if the task is overdue, false if not overdue or if the item has no date
bool Todo::IsOverdue()
{
	if (!HasDate())
		return false;

	// Get the current time
	FILETIME currentTime;
	GetSystemTimeAsFileTime(&currentTime);

	// convert the task time
	FILETIME taskTime;
	SystemTimeToFileTime(&(data->date), &taskTime);

	// Compare them: if (taskTime - currenTime) is negative, the task is overdue
	if (CompareFileTime(&taskTime, &currentTime) == -1)
		return true;

	return false;
}

#include <sstream>

template<typename Source>
std::string to_string(Source arg)
{
	std::ostringstream os;
	os << arg;
	return os.str();
}

string Todo::SystemTime2Date(SYSTEMTIME date)
{
	string day, month, hour, minute;
	
	(date.wDay < 10) ? day = "0" + to_string(date.wDay) : day = to_string(date.wDay);
	(date.wMonth < 10) ? month = "0" + to_string(date.wMonth) : month = to_string(date.wMonth);
	(date.wHour < 10) ? hour = "0" + to_string(date.wHour) : hour = to_string(date.wHour);
	(date.wMinute < 10) ? minute = "0" + to_string(date.wMinute) : minute = to_string(date.wMinute);

	if (data->hasTime)
		return day + "/" + month + "/" + to_string(date.wYear) + "-" + hour + ":" + minute;
	else
		return day + "/" + month + "/" + to_string(date.wYear);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Parse date into SYSTEMTIME Structure
// return true if the date was correctly parsed, false otherwise
void Todo::Date2SystemTime(char* date, SYSTEMTIME* time)
{
	int type = strlen(date);
	// date should be of the form dd/mm/yyyy (v1.1) or dd/mm/yyyy-hh:mm (v1.2 and later)
	if (type != SIZE_OLD_DATE && type != SIZE_DATE)
		return;

	char* token = strtok(date, "/");
	if (token == NULL) return;
	time->wDay = (WORD)atoi(token);

	token = strtok(NULL, "/");
	if (token == NULL) return;
	time->wMonth =  (WORD)atoi(token);

	token = strtok(NULL, "-");
	if (token == NULL) return;
	time->wYear = (WORD)atoi(token);

	// Set HasDate
	data->hasDate = true;

	// version 1.2 and later (includes hours and minutes)
	if (type == SIZE_DATE) {
		token = strtok(NULL, ":");
		if (token == NULL) return;
		time->wHour = (WORD)atoi(token);

		token = strtok(NULL, ":");
		if (token == NULL) return;
		time->wMinute = (WORD)atoi(token);

		console->Output("test\n");
		data->hasTime = true;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
// Check if tasks ini file exists or "create it" otherwise
// Load the data
void Todo::LoadTasksFromIniFile()
{
	// Maximum number of tasks
	const int TASKS_STARTSIZE = 15;
	const int TASKS_EXTENDSIZE = 5;
	const int TASKS_TXTSIZE = 50;

	// Path to the ini file
	char ini[MAX_PATH+10];
	sprintf(ini, "%s%s%s", docklet->GetRootFolder().c_str(),
						   docklet->GetRelativeFolder().c_str(),
						   INITASKS);

	// Check if the ini file exits
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(ini, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		// Try to find the ini.orig file
		char orig[MAX_PATH+10];
		strcpy(orig, ini);
		strcat(orig, INITASKSORIG);
		hFind = FindFirstFile(orig, &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) {
			FindClose(hFind);
			return;
		}
		// ini.orig file found, rename it to .ini
		MoveFile(orig, ini);
	}
	FindClose(hFind);

	// Read the sections from the ini file
	int arrsize = TASKS_STARTSIZE*TASKS_TXTSIZE;
	char* buffer = (char*)malloc(arrsize*sizeof(char));
	if (!buffer)
		return;
	DWORD read = 0;
	do {
		read = GetPrivateProfileSectionNames(buffer, arrsize*sizeof(char), ini);
		if (read == arrsize*sizeof(char)-2) {
			char *tmp;
			tmp = (char*) realloc(buffer, (arrsize+TASKS_EXTENDSIZE)*sizeof(char));
			if (!tmp){
				return;
			} else {
				buffer = tmp;
				arrsize += TASKS_EXTENDSIZE;
			}
		}
	} while(read == arrsize*sizeof(char)-2);

	// Get the sections names
	char* section;
	for ( section = &buffer[0]; (*section) != '\0'; section += lstrlen(section)+1 )
	{
		char name[TASKS_TXTSIZE];
		strcpy(name, "");
		char icon[TASKS_TXTSIZE];
		strcpy(icon, "");
		char sec[TASKS_TXTSIZE];
		strcpy(sec, "");
		strcat(sec, section);
		GetPrivateProfileString(section, INI_NAME, "", name, sizeof(name), ini);
		GetPrivateProfileString(section, INI_ICON, "", icon, sizeof(icon), ini);

		tasks->names.push_back(string(name));
		tasks->paths.push_back(string(icon));
	}

	free(buffer);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Convert back and from "multiline" strings
char* Todo::ConvertToMultiLine(char* text)
{
	CString str(text);
	char* ch = str.LockBuffer();

	int i = str.Find('\r');
	while (i >= 0)
	{
		ch[i] = '~';
		i = str.Find('\r');
	}

	i = str.Find('\n');
	while (i >= 0)
	{
		ch[i] = '|';
		i = str.Find('\n');
	}

	str.UnlockBuffer();

	return ch;
}


char* Todo::ConvertFromMultiLine(char* text)
{
	CString str(text);
	char* ch = str.LockBuffer();

	int i = str.Find('~');
	while (i >= 0)
	{
		ch[i] = '\r';
		i = str.Find('~');
	}

	i = str.Find('|');
	while (i >= 0)
	{
		ch[i] = '\n';
		i = str.Find('|');
	}

	str.UnlockBuffer();

	return ch;
}
