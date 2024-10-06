#include "Graphics.h"
#include <iostream>

// Constants for Cohen-Sutherland Clipping
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

Graphics::Graphics()
{
	factory = NULL;
	renderTarget = NULL;
	brush = NULL;
	bitmap = NULL;
	size = D2D1::SizeU(0, 0);
}

Graphics::~Graphics()
{
	if (factory) factory->Release();
	if (renderTarget) renderTarget->Release();
	if (brush) brush->Release();
	if (bitmap) bitmap->Release();
}

bool Graphics::Init(HWND windowHandle)
{
	HRESULT result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	if (FAILED(result)) {
		std::cerr << "Failed to create D2D1 Factory." << std::endl;
		return false;
	}

	RECT rect;
	GetClientRect(windowHandle, &rect);
	size = D2D1::SizeU(
		rect.right - rect.left,
		rect.bottom - rect.top
	);

	result = factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			windowHandle,
			size),
		&renderTarget);
	if (FAILED(result)) {
		std::cerr << "Failed to create HwndRenderTarget." << std::endl;
		return false;
	}

	// Create a default brush (black)
	result = renderTarget->CreateSolidColorBrush(
		D2D1::ColorF(
			0.0f, // r
			0.0f, // g
			0.0f, // b
			1.0f), // a
		&brush);
	if (FAILED(result)) {
		std::cerr << "Failed to create SolidColorBrush." << std::endl;
		return false;
	}

	return true;
}

D2D1_COLOR_F Graphics::GetBrushColor()
{
	return brush->GetColor();
}

void Graphics::SetBrushColor(D2D1_COLOR_F color)
{
	brush->SetColor(color);
}

void Graphics::SetBrushColor(float r, float g, float b, float a)
{
	brush->SetColor(D2D1::ColorF(r, g, b, a));
}

void Graphics::ClearScreen()
{
	// Clear with a sky-blue color
	renderTarget->Clear(D2D1::ColorF(0.529f, 0.808f, 0.922f)); // Light Blue
}

void Graphics::DrawPoint(float x, float y)
{
	renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), 0.5f, 0.5f), brush, 1.0f);
}

void Graphics::DrawPoints(std::vector<std::pair<float, float>> points, std::vector<D2D1::ColorF> intensity)
{
	D2D1_COLOR_F oldBrushColor = GetBrushColor();
	for (size_t it = 0; it < points.size(); it++)
	{
		SetBrushColor(intensity[it]);
		DrawPoint(points[it].first, points[it].second);
	}
	SetBrushColor(oldBrushColor);
}

void Graphics::DrawHill(float centerX, float centerY, float radius)
{
	// Draw a filled semi-circle (hill)
	D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(centerX, centerY), radius, radius);

	// Create a green brush for hills
	ID2D1SolidColorBrush* hillBrush = nullptr;
	HRESULT hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &hillBrush);
	if (FAILED(hr) || hillBrush == nullptr) {
		std::cerr << "Failed to create hillBrush." << std::endl;
		return;
	}

	// Fill the lower semi-circle to represent the hill
	// Clip the drawing to the lower half
	D2D1_RECT_F clipRect = D2D1::RectF(centerX - radius, centerY, centerX + radius, centerY + radius);
	renderTarget->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	renderTarget->FillEllipse(ellipse, hillBrush);
	renderTarget->PopAxisAlignedClip();

	// Release the hill brush
	hillBrush->Release();
}

void Graphics::DrawCannon(const Cannon& cannon)
{
	// Draw the base of the cannon
	float baseWidth = 20.0f;
	float baseHeight = 10.0f;
	D2D1_RECT_F baseRect = D2D1::RectF(
		cannon.x - baseWidth / 2,
		cannon.y - baseHeight,
		cannon.x + baseWidth / 2,
		cannon.y
	);

	// Set brush color to dark gray for the cannon base
	SetBrushColor(0.2f, 0.2f, 0.2f, 1.0f);
	renderTarget->FillRectangle(baseRect, brush);

	// Draw the barrel
	float barrelLength = 30.0f;
	float barrelWidth = 5.0f;

	// Calculate the end point of the barrel based on the angle
	float endX = cannon.x + barrelLength * cos(cannon.angle);
	float endY = cannon.y + barrelLength * sin(cannon.angle);

	// Calculate perpendicular vectors for the barrel width
	float perpX = barrelWidth * sin(cannon.angle);
	float perpY = -barrelWidth * cos(cannon.angle);

	// Define the six corners of the barrel polygon
	D2D1_POINT_2F barrelPoints[6] = {
		D2D1::Point2F(cannon.x, cannon.y),
		D2D1::Point2F(cannon.x + perpX, cannon.y + perpY),
		D2D1::Point2F(endX + perpX, endY + perpY),
		D2D1::Point2F(endX, endY),
		D2D1::Point2F(endX - perpX, endY - perpY),
		D2D1::Point2F(cannon.x - perpX, cannon.y - perpY)
	};

	// Create a path geometry for the barrel
	ID2D1PathGeometry* pathGeometry = nullptr;
	HRESULT hr = factory->CreatePathGeometry(&pathGeometry);
	if (FAILED(hr) || pathGeometry == nullptr) {
		std::cerr << "Failed to create pathGeometry." << std::endl;
		return;
	}

	ID2D1GeometrySink* geometrySink = nullptr;
	hr = pathGeometry->Open(&geometrySink);
	if (FAILED(hr) || geometrySink == nullptr) {
		std::cerr << "Failed to open geometrySink." << std::endl;
		pathGeometry->Release();
		return;
	}

	// Begin the figure
	geometrySink->BeginFigure(barrelPoints[0], D2D1_FIGURE_BEGIN_FILLED);
	// Add lines to form the barrel polygon
	for (int i = 1; i < 6; ++i) {
		geometrySink->AddLine(barrelPoints[i]);
	}
	geometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);
	geometrySink->Close();

	// Fill the barrel geometry
	renderTarget->FillGeometry(pathGeometry, brush);

	// Release resources
	geometrySink->Release();
	pathGeometry->Release();
}

void Graphics::DrawCannonball(const Cannonball& cannonball)
{
	// Set brush color to black for cannonballs
	SetBrushColor(D2D1::ColorF(D2D1::ColorF::Black));

	D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(cannonball.x, cannonball.y), 5.0f, 5.0f);
	renderTarget->FillEllipse(ellipse, brush);
}

void Graphics::DrawCharacter(float x, float y, float radius)
{
	// Set brush color to blue for the character
	SetBrushColor(D2D1::ColorF(D2D1::ColorF::Blue));

	D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
	renderTarget->FillEllipse(ellipse, brush);
}


// Existing Methods Implementation (LineDDA, etc.) remain unchanged
// ... [Other methods like LineDDA, LineBresenham, etc.] ...
void Graphics::LineDDA(float xa, float ya, float xb, float yb)
{

	float dx = xb - xa, dy = yb - ya, steps;
	float xInc, yInc, x = xa, y = ya;

	if (abs(dx) > abs(dy)) steps = abs(dx);
	else steps = abs(dy);
	xInc = dx / steps;
	yInc = dy / steps;
	DrawPoint(ROUND(x), ROUND(y));
	for (int k = 0; k < steps; k++)
	{
		x += xInc;
		y += yInc;
		DrawPoint(ROUND(x), ROUND(y));
	}
}

void Graphics::LineDDA_SSAA3x3(float xa, float ya, float xb, float yb)
{
	//TODO
	std::vector<std::pair<float, float>> points;
	std::vector<D2D1::ColorF> intensity;

	D2D1::ColorF pixel_intensity(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));

	float dx = xb - xa, dy = yb - ya, steps;
	float xInc, yInc, x = xa, y = ya;

	if (abs(dx) > abs(dy)) steps = abs(dx);
	else steps = abs(dy);
	xInc = dx / steps;
	yInc = dy / steps;
	points.push_back(std::make_pair(ROUND(x), ROUND(y)));
	intensity.push_back(pixel_intensity);
	for (int k = 0; k < steps; k++)
	{
		x += xInc;
		y += yInc;
		points.push_back(std::make_pair(ROUND(x), ROUND(y)));
		intensity.push_back(pixel_intensity);
	}
	DrawPoints(points, intensity);
}

void Graphics::LineBresenham(float xa, float ya, float xb, float yb)
{
	float dx = abs(xa - xb), dy = abs(ya - yb);
	float p = 2 * dy - dx;
	float twoDy = 2 * dy, twoDyDx = 2 * (dy - dx);
	float x, y, xEnd;

	if (xa > xb)
	{
		x = xb;
		y = yb;
		xEnd = xa;
	}
	else
	{
		x = xa;
		y = ya;
		xEnd = xb;
	}
	DrawPoint(x, y);
	while (x < xEnd)
	{
		x++;
		if (p < 0)
			p += twoDy;
		else
		{
			y++;
			p += twoDyDx;
		}
		DrawPoint(x, y);
	}
}



void Graphics::LineMidpoint(float xa, float ya, float xb, float yb)
{
	float dx = xb - xa;
	float dy = yb - ya;
	float x = xa, y = ya;
	float d = 0;
	bool swapped = false;

	if (dy > dx)
	{
		d = dx - (dy / 2);
		Swap(x, y);
		Swap(dx, dy);
		Swap(xb, yb);
		swapped = true;
	}
	else d = dy - (dx / 2);

	if (!swapped) DrawPoint(x, y);
	else DrawPoint(y, x);
	while (x < xb)
	{
		x++;
		if (d > 0)
		{
			y++;
			d = d + (dy - dx);
		}
		else d = d + dy;
		if (!swapped) DrawPoint(x, y);
		else DrawPoint(y, x);
	}
	if (!swapped) DrawPoint(xb, yb);
	else DrawPoint(yb, xb);
}

void Graphics::LineMidpoint_GuptaSproullAA(float xa, float ya, float xb, float yb)
{
	//TODO
	float dx = xb - xa;
	float dy = yb - ya;
	float x = xa, y = ya;
	float d = 0;
	float D = 0, Dlower = 0, Dupper = 0;
	float num = 0;
	float denom = 2 * (sqrtf(dx * dx + dy * dy));
	bool swapped = false;

	std::vector<std::pair<float, float>> points;
	std::vector<D2D1::ColorF> intensity;

	D2D1::ColorF pixel_intensity(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));

	if (dy > dx)
	{
		d = dx - (dy / 2);
		Swap(x, y);
		Swap(dx, dy);
		Swap(xb, yb);
		swapped = true;
	}
	else d = dy - (dx / 2);

	if (!swapped) points.push_back(std::make_pair(ROUND(x), ROUND(y)));
	else points.push_back(std::make_pair(ROUND(y), ROUND(x)));
	intensity.push_back(pixel_intensity);

	while (x < xb)
	{
		x++;
		if (d >= 0)
		{
			y++;
			d = d + (dy - dx);
		}
		else d = d + dy;

		if (!swapped) points.push_back(std::make_pair(ROUND(x), ROUND(y)));
		else points.push_back(std::make_pair(ROUND(y), ROUND(x)));
		intensity.push_back(pixel_intensity);
	}

	if (!swapped) points.push_back(std::make_pair(ROUND(x), ROUND(y)));
	else points.push_back(std::make_pair(ROUND(y), ROUND(x)));
	intensity.push_back(pixel_intensity);

	DrawPoints(points, intensity);
}

void Graphics::CirclePlotPoints(float xc, float yc, float x, float y)
{
	DrawPoint(xc + x, yc + y);
	DrawPoint(xc - x, yc + y);
	DrawPoint(xc + x, yc - y);
	DrawPoint(xc - x, yc - y);
	DrawPoint(xc + y, yc + x);
	DrawPoint(xc - y, yc + x);
	DrawPoint(xc + y, yc - x);
	DrawPoint(xc - y, yc - x);
}

void Graphics::CircleMidpoint(float xc, float yc, float r)
{
	float x = 0;
	float y = r;
	float p = 1 - r;

	CirclePlotPoints(xc, yc, x, y);

	while (x < y)
	{
		x++;
		if (p < 0)
			p += 2 * x + 1;
		else
		{
			y--;
			p += 2 * (x - y) + 1;
		}
		CirclePlotPoints(xc, yc, x, y);
	}
}

void Graphics::EllipsePlotPoints(float xc, float yc, float x, float y)
{
	DrawPoint(xc + x, yc + y);
	DrawPoint(xc - x, yc + y);
	DrawPoint(xc + x, yc - y);
	DrawPoint(xc - x, yc - y);
}

void Graphics::EllipseMidpoint(float xc, float yc, float rx, float ry)
{
	float rx2 = rx * rx;
	float ry2 = ry * ry;
	float twoRx2 = 2 * rx2;
	float twoRy2 = 2 * ry2;
	float p;
	float x = 0;
	float y = ry;
	float px = 0;
	float py = twoRx2 * y;

	EllipsePlotPoints(xc, yc, x, y);

	/*Region 1*/
	p = ROUND(ry2 - (rx2 * ry) + (0.25 * rx2));
	while (px < py)
	{
		x++;
		px += twoRy2;
		if (p < 0)
			p += ry2 + px;
		else
		{
			y--;
			py -= twoRx2;
			p += ry2 + px - py;
		}
		EllipsePlotPoints(xc, yc, x, y);
	}

	/*Region 2*/
	p = ROUND(ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2);
	while (y > 0)
	{
		y--;
		py -= twoRx2;
		if (p > 0)
			p += rx2 - py;
		else
		{
			x++;
			px += twoRy2;
			p += rx2 - py + px;
		}
		EllipsePlotPoints(xc, yc, x, y);
	}
}

void Graphics::Polygon(std::vector<std::pair<float, float>> points)
{
	for (int it = 1; it < points.size(); it++)
	{
		LineDDA(points[it - 1].first, points[it - 1].second, points[it].first, points[it].second);
	}
	LineDDA(points.back().first, points.back().second, points[0].first, points[0].second);
}

void Graphics::BoundaryFill(float x, float y, D2D1::ColorF fill, D2D1::ColorF boundary, bool Fill8)
{
	D2D1_COLOR_F oldBrushColor = GetBrushColor();

	SetBrushColor(fill);
	if (Fill8) BoundaryFill8(x, y, fill, boundary);
	else BoundaryFill4(x, y, fill, boundary);

	SetBrushColor(oldBrushColor);
}



void Graphics::CreateBitmap()
{
	D2D1_BITMAP_PROPERTIES bitmapProperties = D2D1::BitmapProperties(
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)
	);
	renderTarget->CreateBitmap(
		size,
		nullptr,
		0,
		bitmapProperties,
		&bitmap
	);

	ID3D10Texture2D* d3dTexture;
	IDXGISurface* sharedSurface;

	//TODO
}

int Graphics::ComputeOutCode(float x, float y, float xwmin, float ywmin, float xwmax, float ywmax) {
	int code = INSIDE;

	if (x < xwmin)
		code |= LEFT;
	else if (x > xwmax)
		code |= RIGHT;
	if (y < ywmin)
		code |= BOTTOM;
	else if (y > ywmax)
		code |= TOP;

	return code;
}

void Graphics::CohenSutherlandLineClipping(float xwmin, float ywmin, float xwmax, float ywmax, float x1, float y1, float x2, float y2)
{
	int outcode1 = ComputeOutCode(x1, y1, xwmin, ywmin, xwmax, ywmax);
	int outcode2 = ComputeOutCode(x2, y2, xwmin, ywmin, xwmax, ywmax);
	bool accept = false;

	while (true) {
		if (!(outcode1 | outcode2)) {
			accept = true;
			break;
		}
		else if (outcode1 & outcode2) {
			break;
		}
		else {
			float x, y;
			int outcodeOut = outcode1 ? outcode1 : outcode2;

			if (outcodeOut & TOP) {
				x = x1 + (x2 - x1) * (ywmax - y1) / (y2 - y1);
				y = ywmax;
			}
			else if (outcodeOut & BOTTOM) {
				x = x1 + (x2 - x1) * (ywmin - y1) / (y2 - y1);
				y = ywmin;
			}
			else if (outcodeOut & RIGHT) {
				y = y1 + (y2 - y1) * (xwmax - x1) / (x2 - x1);
				x = xwmax;
			}
			else if (outcodeOut & LEFT) {
				y = y1 + (y2 - y1) * (xwmin - x1) / (x2 - x1);
				x = xwmin;
			}

			if (outcodeOut == outcode1) {
				x1 = x;
				y1 = y;
				outcode1 = ComputeOutCode(x1, y1, xwmin, ywmin, xwmax, ywmax);
			}
			else {
				x2 = x;
				y2 = y;
				outcode2 = ComputeOutCode(x2, y2, xwmin, ywmin, xwmax, ywmax);
			}
		}
	}

	if (accept) {
		LineDDA(x1, y1, x2, y2);
	}

}


// Utility Methods
void Graphics::Swap(float& a, float& b)
{
    float temp = a;
    a = b;
    b = temp;
}

// Placeholder implementations for TODO methods
void Graphics::BoundaryFill4(float x, float y, D2D1::ColorF fill, D2D1::ColorF boundary)
{
    // Not implemented for this game
}

void Graphics::BoundaryFill8(float x, float y, D2D1::ColorF fill, D2D1::ColorF boundary)
{
    // Not implemented for this game
}



// Remove duplicate function definitions to prevent C2084 errors
// Ensure GetBrushColor, SetBrushColor, and SetBrushColor(float, float, float, float) are defined only once
