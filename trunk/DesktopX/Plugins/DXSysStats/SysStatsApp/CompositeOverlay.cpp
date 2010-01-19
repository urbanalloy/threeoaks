#include <stdio.h>
#include <windows.h>
#include <GdiplusH.h>

#include "Docklet.h"
#include "Meter.h"
#include "ArrayPtr.h"
#include "ArrayPtr.cpp"
#include "CompositeOverlay.h"
#include "CompositeOverlayDialog.h"
#include "OverlayFactory.h"

const char* CompositeOverlay::TYPE = "Composite";
const char* CompositeOverlay::COUNT_PROPERTY = "Count";

Overlay* CompositeOverlay::createCompositeOverlay()
{
	return new CompositeOverlay();
}

/*
 * Add our create function to the factory.
 */
CompositeOverlay::Initializer CompositeOverlay::initializer(CompositeOverlay::TYPE, createCompositeOverlay);

CompositeOverlay::CompositeOverlay()
{
	init();
}

CompositeOverlay::~CompositeOverlay()
{
	clearAndDestroy();
}

CompositeOverlay::CompositeOverlay(const CompositeOverlay&)
{
}

void CompositeOverlay::init()
{
}

void CompositeOverlay::load(const char *szIni, const char *szIniGroup, const char *prefix, const ArrayPtr<Meter> *meters)
{
	// Clean out our children
	clearAndDestroy();

	Overlay::load(szIni, szIniGroup, prefix, meters);

	char buf[64];
	int offset = strlen(prefix);
	strcpy(buf, prefix);

	strcpy(buf+offset, COUNT_PROPERTY);
	int oCount = GetPrivateProfileInt(szIniGroup, buf, 0, szIni);
	char type[64];
	char name[64];
	strcpy(name, szIniGroup);
	strcat(name, "-");
	offset = strlen(name);
	for (int i=0; i<oCount; i++)
	{
		itoa(i, name+offset, 10);

		GetPrivateProfileString(name, Overlay::TYPE_PROPERTY, "unknown", type, 64, szIni);
#ifdef logger
		fprintf(Docklet::getLogFile(), "Group=%s, Type=%s\n", name, type);
#endif
		Overlay *pOverlay = OverlayFactory::createOverlay(type);
		if (pOverlay != NULL)
		{
			add(pOverlay);
			pOverlay->load(szIni, name, "", meters);
		}
	}
}

/*
 * Called when we need to persist our state.
 */
void CompositeOverlay::save(const char *szIni, const char *szIniGroup, BOOL bIsForExport, const char *prefix, const ArrayPtr<Meter> *meters)
{
	Overlay::save(szIni, szIniGroup, bIsForExport, prefix, meters);

	char buf[32];
	char iBuf[32];
	int offset = strlen(prefix);
	strcpy(buf, prefix);

	strcpy(buf+offset, COUNT_PROPERTY);
	WritePrivateProfileString(szIniGroup, buf, itoa(count(), iBuf, 10), szIni);

	char name[64];
	strcpy(name, szIniGroup);
	strcat(name, "-");
	offset = strlen(name);
	for (int i=0; i<count(); i++)
	{
		itoa(i, name+offset, 10);
		(*this)[i]->save(szIni, name, bIsForExport, "", meters);
	}
}

Overlay *CompositeOverlay::clone()
{
	CompositeOverlay *o = (CompositeOverlay*)Overlay::clone();

	for (int i=0; i<count(); i++)
		o->add((*this)[i]->clone());

	return o;
}

void CompositeOverlay::configure(Observer *observer, ArrayPtr<Meter> *meter, HWND hwnd, HINSTANCE hInstance)
{
	CompositeOverlayDialog *pDialog = new CompositeOverlayDialog(this, "Configure Overlay...", observer);
	pDialog->show(hwnd, hInstance);
	delete pDialog;
}

/*
 */
bool CompositeOverlay::needsUpdate()
{
	bool ret = 0;

	for (int i=count()-1; i>=0; i--)
	{
		ret = ret || (*this)[i]->needsUpdate();
	}

	return ret;
}

/*
 */
void CompositeOverlay::render(Graphics *graphics)
{
	PointF origin(32768, 32768);

	for (int i=count()-1; i>=0; i--)
	{
		if ((*this)[i]->getX() < origin.X)
			origin.X = (*this)[i]->getX();
		if ((*this)[i]->getY() < origin.Y)
			origin.Y = (*this)[i]->getY();
	}

	Matrix matrix;
    matrix.Translate((float)getX(), (float)getY());
    matrix.RotateAt((float)rotation, origin);

	GraphicsState gState = graphics->Save();
    graphics->MultiplyTransform(&matrix);
   
	for (i=count()-1; i>=0; i--)
	{
		(*this)[i]->render(graphics);
	}

	graphics->Restore(gState);
}

Overlay *CompositeOverlay::createInstance()
{
	return new CompositeOverlay();
}
