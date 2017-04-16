#ifndef pVIEW_H
#include "AnalogView.h"
#endif

#include <Picture.h>
#include <stdio.h>

char *gProcessorName[] = {
		"unknown",
		"601",
		"603",
		"603e",
		"604",
		"604e",
		"unknown",
		"unknown",
		"unknown",
		"unknown",
		"unknown",
		"unknown",
		"unknown"
};


BRect AnalogView::computeTick(float vu, BPoint center, long length, float start, float stop)
{
	BRect	r;
	float angle;
	float cosAngle;
	float sinAngle;
	
	angle = (-scaleAngle(vu) + 90) * kPI / 180.0;
	cosAngle = cos(angle);
	sinAngle = sin(angle);
	r.top = center.y - cosAngle * length * stop;
	r.bottom = center.y - cosAngle * length * start;
	r.right = center.x + sinAngle * length *  start;
	r.left = center.x + sinAngle * length * stop;
	
	return r;
}

BBitmap* AnalogView::makePanel(BRect r)
{
	system_info		sysInfo;
	BRect			rt;
	short			i;
	float			penSize;
	float			width;
	char			buf[500];
	int			cpu_speed;
	char*		processor;
	BFont			font;

	get_system_info(&sysInfo);	

	panel = new BBitmap(r, B_COLOR_8_BIT, true);
	view = new BView(r, "", B_FOLLOW_NONE, B_WILL_DRAW);
	panel->AddChild(view);
	panel->Lock();

	BPicture *picture = new BPicture();

	view->BeginPicture(picture);

	//
	// Meter Panel
	//
	const rgb_color kBackColor = {255,255,203,0};
	const rgb_color kForeColor = {0,0,0,0};
	const rgb_color kRedColor = {255,0,0,0};
	
	//const rgb_color kBackColor = {0,0,0,0};
	//const rgb_color kForeColor = {0,255,0,0};
	//const rgb_color kRedColor = {255,0,0,0};
	
	view->SetLowColor(kBackColor);
	view->SetHighColor(kBackColor);
	view->FillRect(r);
	view->SetHighColor(0, 0, 0);
	view->StrokeRect(r);


	penSize = param.panelLength * (kTickEnd - kTickStart);
	rt.Set(param.meterCenter.x - param.panelLength, param.meterCenter.y - param.panelLength,
			param.meterCenter.x + param.panelLength, param.meterCenter.y + param.panelLength);
	rt.InsetBy(penSize / 2, penSize / 2);
	view->SetHighColor(kRedColor);
	view->SetPenSize(penSize);
	view->StrokeArc(rt, scaleAngle(81), scaleAngle(100) - scaleAngle(80));
	
	view->SetPenSize(1);
	view->SetHighColor(kForeColor);
	for(i = 0; i <= 100; i = i + 10)
	{
		rt = computeTick(i, param.meterCenter, param.panelLength, kTickStart, kTickEnd);
		view->StrokeLine(rt.RightBottom(), rt.LeftTop());	
	}
	for(i = 0; i <= 100; i = i + 5)
	{
		rt = computeTick(i, param.meterCenter, param.panelLength, kTickStart, kTickMiddle);
		view->StrokeLine(rt.RightBottom(), rt.LeftTop());	
	}

	font.SetFamilyAndStyle("Baskerville MT Italic", 0);
	view->SetFont(&font, 0);
	view->SetFontSize(9);
	
	for(i = 0; i <= 100; i = i + 10)
	{
		rt = computeTick(i, param.meterCenter, param.panelLength+5, kTickStart, kTickEnd);
		sprintf(buf, "%d", i);
		width = view->StringWidth(buf) / 2;
		switch(i)
		{
			case 0:
				view->MovePenTo(rt.left - width - 1, rt.top);
				break;
			case 10:
				view->MovePenTo(rt.left - width - 2, rt.top - 1);
				break;
			case 90:
				view->MovePenTo(rt.left - width + 2, rt.top - 1);
				break;
			case 100:
				view->MovePenTo(rt.left - width + 5, rt.top);
				break;
			default:
				view->MovePenTo(rt.left - width, rt.top);
		}
		view->DrawString(buf);
	}

	rt = r;


	font.SetFamilyAndStyle("Dutch801 Rm BT", "Bold");
	view->SetFont(&font, B_FONT_ALL);
	view->SetFontSize(12);
	strcpy(buf, "CPU [%]");
	view->MovePenTo(param.meterCenter.x - view->StringWidth(buf)/ 2, 75);
	view->DrawString(buf);
	
	if(param.cpu + 1 == sysInfo.cpu_count)
	{

		font.SetFamilyAndStyle("Swis721 BT", "Roman");
		view->SetFont(&font);
		view->SetFontSize(9);
		/*if (sysInfo.cpu_type > 0 && sysInfo.cpu_type < 14)
			processor = gProcessorName[sysInfo.cpu_type];
		else
			processor = "unknown";
		*/
		switch(sysInfo.cpu_type)
		{
		case B_CPU_PPC_601:
			processor = "PowerPC 601";
			break;
		case B_CPU_PPC_603:
			processor = "PowerPC 603";
			break;
		case B_CPU_PPC_603e:
			processor = "PowerPC 603e";
			break;
		case B_CPU_PPC_604:
			processor = "PowerPC 604";
			break;
		case B_CPU_PPC_604e:
			processor = "PowerPC 604e";
			break;
		case B_CPU_PPC_750:
			processor = "PowerPC G3";
			break;
		case B_CPU_INTEL_X86:
			processor = "Intel x86";
			break;	
		case B_CPU_INTEL_PENTIUM:
		case B_CPU_INTEL_PENTIUM75:
			processor = "Intel Pentium";
			break;
		case B_CPU_INTEL_PENTIUM_486_OVERDRIVE:
		case B_CPU_INTEL_PENTIUM75_486_OVERDRIVE:	
			processor = "Intel Pentium OverDrive";
			break;
		case B_CPU_INTEL_PENTIUM_MMX:
//		case B_CPU_INTEL_PENTIUM_MMX_MODEL_4:
		case B_CPU_INTEL_PENTIUM_MMX_MODEL_8:	
			processor = "Intel Pentium MMX";
			break;	
		case B_CPU_INTEL_PENTIUM_PRO:
			processor = "Intel Pentium Pro";
			break;
		case B_CPU_INTEL_PENTIUM_II:
//	case B_CPU_INTEL_PENTIUM_II_MODEL_3:
		case B_CPU_INTEL_PENTIUM_II_MODEL_5:
			processor = "Intel Pentium II";
			break;
		case B_CPU_INTEL_CELERON:
			processor = "Intel Celeron";
			break;
		case B_CPU_INTEL_PENTIUM_III:
#ifdef B_BEOS_VERSION_5
		case B_CPU_INTEL_PENTIUM_III_MODEL_8:
#endif
			processor = "Intel Pentium III";
			break;
#ifdef B_BEOS_VERSION_5
		case B_CPU_AMD_ATHLON_MODEL1:
			processor = "AMD ATHLON";
			break;
#endif
		case B_CPU_AMD_X86:
			processor = "AMD x86";
		 	break;
		case B_CPU_AMD_K5_MODEL0:
		case B_CPU_AMD_K5_MODEL1:
		case B_CPU_AMD_K5_MODEL2:
		case B_CPU_AMD_K5_MODEL3:
			processor = "AMD K5";
			break;
		case B_CPU_AMD_K6_MODEL6:
		case B_CPU_AMD_K6_MODEL7:
		case B_CPU_AMD_K6_MODEL8:
//		case B_CPU_AMD_K6_2:
		case B_CPU_AMD_K6_MODEL9:
			processor = "AMD K6 III";
			break;
		case B_CPU_CYRIX_X86:
			processor = "Cyrix x86";
			break;
		case B_CPU_CYRIX_GXm:
			processor = "Cyrix GXm";
			break;
		case B_CPU_CYRIX_6x86MX:
			processor = "Cyrix 6x86MX";
			break;
		case B_CPU_RISE_X86:
			processor = "RISE x86";
			break;
		case B_CPU_RISE_mP6:
			processor = "RISE mP6";
			break;
		default:
			processor = "unknown";
		}
		cpu_speed = sysInfo.cpu_clock_speed;
		if(strcmp(processor,"PowerPC 604e") == 0)
			cpu_speed *= 3;
		//	cpu_speed = ((cpu_speed + 500000)/1000000)*1000000;
		//	cpu_speed /= 1000000;
		int target = cpu_speed / 1000000;
		int frac = target % 100;
		int delta = -frac;
		int at = 0;
		int freqs[] = { 100, 50, 25, 75, 33, 67, 20, 40, 60, 80, 10, 30, 70, 90 };

		for (uint x = 0; x < sizeof(freqs) / sizeof(freqs[0]); x++) {
			int ndelta = freqs[x] - frac;
			if (abs(ndelta) < abs(delta)) {
				at = freqs[x];
				delta = ndelta;
			}
		}
		sprintf(buf, "%dMHz", target + delta);
		view->MovePenTo(rt.right - view->StringWidth(processor) - view->StringWidth(buf) - 5, 12);
		view->DrawString(processor);
		view->MovePenTo(rt.right - view->StringWidth(buf) - 3, 12);
		view->DrawString(buf);
	}
	
	view->EndPicture();
	view->DrawPicture(picture, BPoint(0,0));
	view->Sync();
	panel->Unlock();

	delete picture;

	return panel;
}