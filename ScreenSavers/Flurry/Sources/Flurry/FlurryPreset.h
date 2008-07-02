/*
 * Flurry for Windows.
 *
 * Flurry preset specification
 *
 * Created 5/31/2003 by Matt Ginzton, magi@cs.stanford.edu
 */

#pragma once

#include <atlbase.h>
#include <vector>
using std::vector;

class FlurryClusterSpec {
	public:
		int nStreams;
		int color;
		float thickness;
		float speed;
};

class FlurrySpec {
	public:
		FlurrySpec(char *format);
		~FlurrySpec();

		bool valid;
		char *name;
		vector<FlurryClusterSpec> clusters;

		bool WriteToString(char *format, int formatLen);

	private:
		bool ParseFromString(char *format);
};

/*
 * Exported globals from this module
 */

extern vector<int> g_multiMonPreset;
extern vector<FlurrySpec *> g_visuals;


/*
 * Exported functions
 */

void Visuals_Read(CRegKey& reg);
void Visuals_Write(CRegKey& reg);

void Monitors_Read(CRegKey& reg);
void Monitors_Write(CRegKey& reg);
