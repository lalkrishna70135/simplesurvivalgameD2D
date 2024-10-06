// Graphics.h
#pragma once

#include <d2d1.h>
#include <wincodec.h>
#include <vector>
#include <utility> // For std::pair

#define ROUND(a) ((int)(a + 0.5f))
#define PI 3.14159265f

// Structure for a Cannon
struct Cannon {
    float x;     // X-coordinate position on the hill
    float y;     // Y-coordinate position on the hill
    float angle; // Firing angle in radians
};

// Structure for a Cannonball
struct Cannonball {
    float x;  // Current X position
    float y;  // Current Y position
    float vx; // Velocity in X direction
    float vy; // Velocity in Y direction
};

class Graphics
{
private:
    ID2D1Factory* factory;
    ID2D1HwndRenderTarget* renderTarget;
    ID2D1SolidColorBrush* brush;
    ID2D1Bitmap* bitmap;

    D2D1_SIZE_U size;

    // Private helper methods
    D2D1_COLOR_F GetBrushColor();
    void SetBrushColor(D2D1_COLOR_F color);
    void SetBrushColor(float r, float g, float b, float a);

    void Swap(float& a, float& b);

    void CirclePlotPoints(float xc, float yc, float x, float y);
    void EllipsePlotPoints(float xc, float yc, float x, float y);

    void BoundaryFill4(float x, float y, D2D1::ColorF fill, D2D1::ColorF boundary);
    void BoundaryFill8(float x, float y, D2D1::ColorF fill, D2D1::ColorF boundary);

public:
    Graphics();
    ~Graphics();

    bool Init(HWND windowHandle);

    void BeginDraw() { renderTarget->BeginDraw(); }
    void EndDraw() { renderTarget->EndDraw(); }

    void ClearScreen();
    void DrawPoint(float x, float y);
    void DrawPoints(std::vector<std::pair<float, float>> points, std::vector<D2D1::ColorF> intensity);

    // New Drawing Methods for the Game
    void DrawHill(float centerX, float centerY, float radius);
    void DrawCannon(const Cannon& cannon);
    void DrawCannonball(const Cannonball& cannonball);
    void DrawCharacter(float x, float y, float radius);

    // Existing Drawing Methods
    void LineDDA(float xa, float ya, float xb, float yb);
    void LineDDA_SSAA3x3(float xa, float ya, float xb, float yb);
    void LineBresenham(float xa, float ya, float xb, float yb);
    void LineMidpoint(float xa, float ya, float xb, float yb);
    void LineMidpoint_GuptaSproullAA(float xa, float ya, float xb, float yb);
    void CircleMidpoint(float xc, float yc, float r);
    void EllipseMidpoint(float xc, float yc, float rx, float ry);
    void Polygon(std::vector<std::pair<float, float>> points);
    void BoundaryFill(float x, float y, D2D1::ColorF fill, D2D1::ColorF boundary, bool Fill8);
    void CohenSutherlandLineClipping(float xwmin, float ywmin, float xwmax, float ywmax, float x1, float y1, float x2, float y2);
    int ComputeOutCode(float x, float y, float xwmin, float ywmin, float xwmax, float ywmax);
    void CreateBitmap();
};
