// winmain.cpp
#include <Windows.h>
#include <wincodec.h>
#include <vector>
#include <cmath>
#include "Graphics.h"
#include <time.h>
using namespace std;

// Window dimensions
#define WIDTH 800
#define HEIGHT 600
#define PI 3.14159265f

// Global Variables
Graphics* graphics;
HWND g_hwnd; // Global window handle for access in WindowProc

// Game Entities
Cannon leftCannon = { 100.0f, HEIGHT - 100.0f, -PI / 4.0f };      // Left hill, initial firing angle
Cannon rightCannon = { WIDTH - 100.0f, HEIGHT - 100.0f, -3.0f * PI / 4.0f }; // Right hill, initial firing angle

pair<float, float> characterPos = { WIDTH / 2.0f, HEIGHT / 2.0f }; // Character Position (Center)
const float CHARACTER_RADIUS = 20.0f; // Radius for collision detection and drawing

// Container for Cannonballs
vector<Cannonball> cannonballs;

// Timing for cannon firing
ULONGLONG lastFireTimeLeft = 0;
ULONGLONG lastFireTimeRight = 0;
const ULONGLONG fireInterval = 1000; // 2 seconds between shots

// Keyboard Input Tracking
bool keys[256] = { false };

// Function Prototypes
void update(HWND hwnd);
void render();

// Window Procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        if (wParam < 256) keys[wParam] = true;
        return 0;

    case WM_KEYUP:
        if (wParam < 256) keys[wParam] = false;
        return 0;

    case WM_MOUSEMOVE:
        // Optional: Implement if character movement via mouse is desired
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Function to calculate angle from cannon to character
float CalculateAngle(float cannonX, float cannonY, float targetX, float targetY) {
    float deltaX = targetX - cannonX;
    float deltaY = targetY - cannonY;
    float angle = atan2f(deltaY, deltaX);
    return angle;
}

// Update Function: Handles game logic
void update(HWND hwnd) {
    ULONGLONG currentTime = GetTickCount64();

    // Update cannon angles to aim toward the character
    leftCannon.angle = CalculateAngle(leftCannon.x, leftCannon.y, characterPos.first, characterPos.second);
    rightCannon.angle = CalculateAngle(rightCannon.x, rightCannon.y, characterPos.first, characterPos.second);

    // Fire from left cannon
    if (currentTime - lastFireTimeLeft >= fireInterval) {
        lastFireTimeLeft = currentTime;

        // Create a new cannonball from the left cannon
        Cannonball cb;
        cb.x = leftCannon.x + 30.0f * cos(leftCannon.angle); // Start at end of barrel
        cb.y = leftCannon.y + 30.0f * sin(leftCannon.angle);

        // Velocity: speed of 5 units per frame towards the character
        cb.vx = 10.0f * cos(leftCannon.angle);
        cb.vy = 10.0f * sin(leftCannon.angle);

        cannonballs.push_back(cb);
    }

    // Fire from right cannon
    if (currentTime - lastFireTimeRight >= fireInterval) {
        lastFireTimeRight = currentTime;

        // Create a new cannonball from the right cannon
        Cannonball cb;
        cb.x = rightCannon.x + 30.0f * cos(rightCannon.angle); // Start at end of barrel
        cb.y = rightCannon.y + 30.0f * sin(rightCannon.angle);

        // Velocity: speed of 5 units per frame towards the character
        cb.vx = 10.0f * cos(rightCannon.angle);
        cb.vy = 10.0f * sin(rightCannon.angle);

        cannonballs.push_back(cb);
    }

    // Move cannonballs
    for (auto it = cannonballs.begin(); it != cannonballs.end(); ) {
        it->x += it->vx;
        it->y += it->vy;

        // Remove cannonball if it goes out of bounds
        if (it->x < 0 || it->x > WIDTH || it->y < 0 || it->y > HEIGHT) {
            it = cannonballs.erase(it);
        }
        else {
            ++it;
        }
    }

    // Handle Character Movement
    const float speed = 5.0f;
    if (keys['W'] || keys[VK_UP]) {
        characterPos.second -= speed;
        if (characterPos.second - CHARACTER_RADIUS < 0)
            characterPos.second = CHARACTER_RADIUS;
    }
    if (keys['S'] || keys[VK_DOWN]) {
        characterPos.second += speed;
        if (characterPos.second + CHARACTER_RADIUS > HEIGHT)
            characterPos.second = HEIGHT - CHARACTER_RADIUS;
    }
    if (keys['A'] || keys[VK_LEFT]) {
        characterPos.first -= speed;
        if (characterPos.first - CHARACTER_RADIUS < 0)
            characterPos.first = CHARACTER_RADIUS;
    }
    if (keys['D'] || keys[VK_RIGHT]) {
        characterPos.first += speed;
        if (characterPos.first + CHARACTER_RADIUS > WIDTH)
            characterPos.first = WIDTH - CHARACTER_RADIUS;
    }

    // Check for collisions
    for (const auto& cb : cannonballs) {
        float dx = cb.x - characterPos.first;
        float dy = cb.y - characterPos.second;
        float distance = sqrtf(dx * dx + dy * dy);

        if (distance <= CHARACTER_RADIUS + 5.0f) { // 5.0f is cannonball radius
            // Collision detected, game over
            int response = MessageBox(hwnd, L"You were hit! Game Over.\nDo you want to play again?", L"Game Over", MB_YESNO | MB_ICONINFORMATION);
            if (response == IDYES) {
                // Reset game state
                cannonballs.clear();
                characterPos = { WIDTH / 2.0f, HEIGHT / 2.0f };
                lastFireTimeLeft = GetTickCount64();
                lastFireTimeRight = GetTickCount64();
            }
            else {
                PostQuitMessage(0);
            }
            break;
        }
    }
}

// Render Function: Draws all game entities
void render()
{
    graphics->BeginDraw();
    graphics->ClearScreen();

    // Draw Hills
    graphics->DrawHill(leftCannon.x, leftCannon.y, 100.0f); // Left hill
    graphics->DrawHill(rightCannon.x, rightCannon.y, 100.0f); // Right hill

    // Draw Cannons
    graphics->DrawCannon(leftCannon);
    graphics->DrawCannon(rightCannon);

    // Draw Cannonballs
    for (const auto& cb : cannonballs) {
        graphics->DrawCannonball(cb);
    }

    // Draw Character
    graphics->DrawCharacter(characterPos.first, characterPos.second, CHARACTER_RADIUS);

    graphics->EndDraw();
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
    // Register Window Class
    WNDCLASSEX windowClass;
    ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.hInstance = hInstance;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.lpszClassName = L"MainWindow";
    windowClass.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassEx(&windowClass)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    // Create Window
    RECT rect = { 0, 0, WIDTH, HEIGHT };
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

    HWND windowHandle = CreateWindowEx(
        0,
        L"MainWindow",
        L"Canon Game",
        WS_OVERLAPPEDWINDOW,
        100,
        50,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL,
        NULL,
        hInstance,
        0);

    if (!windowHandle) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    g_hwnd = windowHandle; // Assign to global variable

    // Initialize Graphics
    graphics = new Graphics();
    if (!graphics->Init(windowHandle)) {
        MessageBox(NULL, L"Graphics Initialization Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        delete graphics;
        return -1;
    }

    ShowWindow(windowHandle, nShowCmd);

    // Initialize firing times
    lastFireTimeLeft = GetTickCount64();
    lastFireTimeRight = GetTickCount64();

    // Main Message Loop
    MSG message;
    ZeroMemory(&message, sizeof(MSG));
    while (message.message != WM_QUIT) {
        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        else {
            update(windowHandle); // Update game logic
            render();             // Render the game
        }
    }

    // Cleanup
    delete graphics;

    return (int)message.wParam;
}
