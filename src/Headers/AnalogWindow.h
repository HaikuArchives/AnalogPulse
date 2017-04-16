#ifndef WINDOW_H
#define WINDOW_H
#endif

#include <Window.h>

#define kMeterWidth		200.
#define kMeterHeight	90.
#define kBorderWidth	5.

class AnalogWindow : public BWindow {

	public:
						AnalogWindow(BRect, const char *);
		bool			QuitRequested();
};

