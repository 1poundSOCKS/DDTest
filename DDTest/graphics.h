#pragma once
#include "d2d1.h"

namespace graphics
{
	struct Framework
	{
		bool initialized;
		ID2D1Factory* factory;
		ID2D1HwndRenderTarget* renderTarget;
		bool recreateRenderTarget;
		ID2D1SolidColorBrush* lightSlateGrayBrush;
		ID2D1SolidColorBrush* landBrush;
		ID2D1SolidColorBrush* waterBrush;
		float angle;
		float zoom;
	};

	Framework* New();
	Framework* GlobalInitialize(HWND wnd);
	void GlobalUninitialize(Framework* f);
	bool RecreateRenderTarget(Framework* f, HWND wnd);
	void Render(Framework* f, HWND wnd);
	void Resize(Framework* f, UINT width, UINT height, HWND wnd);
	void RotateAnticlockwise(Framework* f);
	void RotateClockwise(Framework* f);
	void ZoomIn(Framework* f);
	void ZoomOut(Framework* f);
}
