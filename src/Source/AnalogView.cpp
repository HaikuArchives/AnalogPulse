#ifndef pVIEW_H
#include "AnalogView.h"
#endif
#include <Window.h>
#include <Dragger.h>
#include <Message.h>
#include <Alert.h>
#include <Autolock.h>

AnalogView::AnalogView(BRect r, int cpu)
		   : _inherited(r, "", B_FOLLOW_NONE, B_WILL_DRAW | B_PULSE_NEEDED)
{
	BDragger*	dragger; 
    BRect		rt = r; 
    	
	InitObject(r, cpu);

	rt.OffsetTo(B_ORIGIN);
	rt.left = rt.right - 7;
	rt.top = rt.bottom - 7;
	rt.OffsetBy(-1, -1);
	dragger = new BDragger(rt, (BView *)this ); 
	AddChild(dragger); 

}

AnalogView::AnalogView(BMessage *data) :_inherited(data)
{
	InitObject(data->FindRect("bounds"), data->FindInt32("cpu"));
}

void AnalogView::InitObject(BRect rt, int cpu)
{
	rt.OffsetTo(B_ORIGIN);
	param.cpu = cpu;

	system_info		sysInfo;
	
	get_system_info(&sysInfo);
	oldActiveTime = sysInfo.cpu_infos[cpu].active_time; 
	oldSystemTime = system_time();
	nowCPUTime = 0;
	oldCPUTime = -1;

	param.panelLength = rt.Width() / 2 - 3 * 7;
	param.needle = (rt.Width() / 2 - 3 * 7) * 0.93;
	param.meterCenter.Set(rt.Width() / 2, param.panelLength + 3 * 8);
	makeTable();
	
	makePanel(rt);

	SetViewColor(216, 216, 216);
	SetLowColor(216, 216, 216);
	SetHighColor(0, 0, 0);
}

AnalogView*
AnalogView::Instantiate(BMessage *data)
{
	if (!validate_instantiation(data, "AnalogView"))
		return NULL;	

	return new AnalogView(data);
}

status_t
AnalogView::Archive(BMessage *data,bool deep) const
{
	BView::Archive(data, deep);
    
    data->AddString("class", "AnalogView"); 
    data->AddString("add_on", "application/x-vnd.takamatsu-analog-pulse"); 
    data->AddRect("bounds", Bounds());
    data->AddInt32("cpu", param.cpu);
	return B_NO_ERROR;
}

void AnalogView::Pulse()
{
	system_info		sysInfo;
	short			CPUTime;
	double			now;
	
	now  = system_time();
	get_system_info(&sysInfo);
		
	// calculate the CPU load fraction
	CPUTime = 100 * (sysInfo.cpu_infos[param.cpu].active_time - oldActiveTime)
			/ (now - oldSystemTime);
	if(CPUTime > 100)
		CPUTime = 100;
	// store values for next iteration
	oldActiveTime = sysInfo.cpu_infos[param.cpu].active_time;
	oldSystemTime = now;
	
	UpdateNeedle(CPUTime);
}


void AnalogView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case B_ABOUT_REQUESTED:
		{
			(new BAlert("","AnalogPulse Lite","OK"))->Go();
			break;
		}
		default:
			_inherited::MessageReceived(message);
	}
}


void AnalogView::UpdateNeedle(short cpuTime)
{
	BRect	updateRect;
	BRect	rt;

	if(cpuTime != oldCPUTime)
	{
		rt.SetRightBottom(param.meterCenter);
		rt.SetLeftTop(angle[oldCPUTime].arry);
		if(rt.left > rt.right)
		{
			double tmp = rt.left;
			rt.left = rt.right;
			rt.right = tmp;
		}

		DrawBitmap(panel, rt, rt);

		const rgb_color kNeedleColor = {0,0,0,0};
		//const rgb_color kNeedleColor = {0,255,0,0};
		this->SetHighColor(kNeedleColor);

		StrokeLine(param.meterCenter, angle[cpuTime].arry);
	}
	Sync();
	oldCPUTime = cpuTime;
}

AnalogView::~AnalogView()
{
	delete panel;
}


void AnalogView::Draw( BRect updateRect )
{
	BAutolock lock(Window());
	if(lock.IsLocked())
		DrawBitmapAsync(panel, BPoint(0, 0));
}

float AnalogView::scaleAngle(float dB)
{
	return(180 - 15 - kSCALE_DEGREES / 100 * dB);
}

void AnalogView::makeTable(void)
{
	float	ang;

	for(short i = 0; i<=100; i++)
	{
		ang = (-scaleAngle(i) + 90)* kPI / 180.0;
		angle[i].arry.Set(param.meterCenter.x + sin(ang) * param.needle,
							param.meterCenter.y - cos(ang) * param.needle);
	}
}
