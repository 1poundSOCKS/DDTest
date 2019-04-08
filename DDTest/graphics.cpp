#include "stdafx.h"
#include "graphics.h"
#include "world.h"

#pragma comment(lib, "d2d1")

#define SafeRelease(i) if( i ) i->Release()

graphics::Framework* graphics::New()
{
	// initialize return data struct
	Framework* f = new Framework();
	if (f == NULL)
		return NULL;

	::ZeroMemory(f, sizeof(Framework));
	f->initialized = false;
	f->recreateRenderTarget = true;
	return f;
}

graphics::Framework* graphics::GlobalInitialize(HWND wnd)
{
	Framework* f = New();
	if (f == NULL)
		return NULL;

	// create the factory
	ID2D1Factory* factory = NULL;
	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&factory
	);

	if (SUCCEEDED(hr))
	{
		f->factory = factory;
	}
	else
	{
		return f;
	}

	f->initialized = true;
	return f;
}

void graphics::GlobalUninitialize(Framework* f)
{
	SafeRelease(f->landBrush);
	SafeRelease(f->waterBrush);
	SafeRelease(f->lightSlateGrayBrush);
	SafeRelease(f->renderTarget);
	SafeRelease(f->factory);
}

void graphics::Render(Framework* f, HWND wnd)
{
	if ( f->recreateRenderTarget )
		RecreateRenderTarget(f, wnd);

	f->renderTarget->BeginDraw();
	f->renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	f->renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	D2D1_SIZE_F rtSize = f->renderTarget->GetSize();

	// Draw a grid background.
	int width = static_cast<int>(rtSize.width);
	int height = static_cast<int>(rtSize.height);

	for (int x = 0; x < width; x += 20)
	{
		for (int y = 0; y < height; y += 20)
		{
			float xVal = static_cast<float>(x);
			float yVal = static_cast<float>(y);

			float noise = world::GetNoise(
				xVal / 20, 
				yVal / 20);

			D2D1_RECT_F rectangle = D2D1::RectF(
				xVal,
				yVal,
				xVal + 20,
				yVal + 20
			);

			if (noise >= 0)
			{
				f->renderTarget->FillRectangle(
					&rectangle,
					f->landBrush
				);
			}
			else
			{
				f->renderTarget->FillRectangle(
					&rectangle,
					f->waterBrush
				);
			}
		}
	}

	for (int x = 0; x < width; x += 20)
	{
		f->renderTarget->DrawLine(
			D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
			D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
			f->lightSlateGrayBrush,
			0.5f
		);
	}

	for (int y = 0; y < height; y += 20)
	{
		f->renderTarget->DrawLine(
			D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
			D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
			f->lightSlateGrayBrush,
			0.5f
		);
	}

	HRESULT hr = f->renderTarget->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		f->recreateRenderTarget = true;
	}
}

void graphics::Resize(Framework* const f, UINT width, UINT height, HWND wnd)
{
	// Note: This method can fail, but it's okay to ignore the
	// error here, because the error will be returned again
	// the next time EndDraw is called.
	if (f->recreateRenderTarget)
		RecreateRenderTarget(f, wnd);

	f->renderTarget->Resize(D2D1::SizeU(width, height));
}

bool graphics::RecreateRenderTarget(Framework* f, HWND wnd)
{
	SafeRelease(f->landBrush);
	SafeRelease(f->waterBrush);
	SafeRelease(f->lightSlateGrayBrush);
	SafeRelease(f->renderTarget);

	RECT rc;
	GetClientRect(wnd, &rc);

	ID2D1HwndRenderTarget* rt = NULL;
	HRESULT hr = f->factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			wnd,
			D2D1::SizeU(
				rc.right - rc.left,
				rc.bottom - rc.top)
		),
		&rt
	);

	if (SUCCEEDED(hr))
	{
		f->renderTarget = rt;
	}
	else
	{
		return false;
	}

	// create the brushes
	ID2D1SolidColorBrush* lightSlateGrayBrush;
	hr = f->renderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::LightSlateGray),
		&lightSlateGrayBrush
	);

	if (SUCCEEDED(hr))
	{
		f->lightSlateGrayBrush = lightSlateGrayBrush;
	}
	else
	{
		return false;
	}

	ID2D1SolidColorBrush* greenBrush;
	hr = f->renderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Green),
		&greenBrush
	);

	if (SUCCEEDED(hr))
	{
		f->landBrush = greenBrush;
	}
	else
	{
		return false;
	}

	ID2D1SolidColorBrush* newBrush;
	hr = f->renderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Blue),
		&newBrush
	);

	if (SUCCEEDED(hr))
	{
		f->waterBrush = newBrush;
	}
	else
	{
		return false;
	}

	f->recreateRenderTarget = false;

	return true;
}
