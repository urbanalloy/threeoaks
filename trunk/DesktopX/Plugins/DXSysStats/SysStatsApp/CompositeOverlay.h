#pragma once

#include <windows.h>
#include <commdlg.h>
#include <Gdiplus.h>
using namespace Gdiplus;

#include "ArrayPtr.h"
#include "Overlay.h"

/*
 * An overlay that is an ordered collection of other overlays.
 */
class CompositeOverlay : public Overlay, public ArrayPtr<Overlay>
{
public:
	CompositeOverlay();
	virtual ~CompositeOverlay();

	// Overridden members
	virtual bool needsUpdate();
	virtual void render(Graphics *graphics);
	virtual void load(const char *szIni, const char *szIniGroup, const char *prefix, const ArrayPtr<Meter> *meters);
	virtual void save(const char *szIni, const char *szIniGroup, BOOL bIsForExport, const char *prefix, const ArrayPtr<Meter> *meters);
	virtual void configure(Observer *observer, ArrayPtr<Meter> *meters, HWND hwnd, HINSTANCE hInstance);
	virtual Overlay *clone();
	virtual const char* getType() const { return TYPE; }

protected:
	virtual Overlay *createInstance();

private:
	void init();
	CompositeOverlay(const CompositeOverlay&);

	static Overlay *createCompositeOverlay();
	static Initializer initializer;

	static const char* TYPE;
	static const char* COUNT_PROPERTY;
};