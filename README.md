# 🎮 Cannon Survival Game (C++ & Direct2D)

![C++](https://img.shields.io/badge/language-C++-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)
![Direct2D](https://img.shields.io/badge/graphics-Direct2D-green.svg)
![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)
[![Demo](https://img.shields.io/badge/demo-video-orange.svg)](https://github.com/user-attachments/assets/1a15c845-9c96-451d-92e1-54cdf0bc460c)


A simple **2D survival game** built in **C++** using the **Direct2D graphics API**.  
The player controls a character who must dodge cannonballs fired from two cannons placed on opposite hills. The game demonstrates fundamental **computer graphics algorithms** and **real-time rendering** with Direct2D.

---

## 🚀 Features
- **Character Movement** – Control a blue circular character using:
  - `W` / `↑` : Move Up  
  - `S` / `↓` : Move Down  
  - `A` / `←` : Move Left  
  - `D` / `→` : Move Right
- **Enemy Cannons** – Two automated cannons positioned on hills:
  - Continuously aim at the player  
  - Fire cannonballs every second
- **Collision Detection** – If a cannonball hits the character, the game ends with a **Game Over** message box.
- **Graphics Rendering** – Implements classic computer graphics algorithms:
  - Line drawing (DDA, Bresenham, Midpoint, Anti-Aliased variations)  
  - Circle & Ellipse midpoint algorithms  
  - Polygon drawing and clipping (Cohen–Sutherland line clipping)  
- **Direct2D Rendering** – Smooth rendering with hardware acceleration.

---

## 🖥️ Gameplay Preview
<p align="center">
  <img src="https://github.com/user-attachments/assets/1a15c845-9c96-451d-92e1-54cdf0bc460c" width="600" alt="Gameplay Preview"/>
</p>

---

## 🛠️ Tech Stack
- **Language**: C++  
- **Graphics Library**: Direct2D (Windows API)  
- **Core Files**:
  - `Graphics.h / Graphics.cpp` → Graphics rendering & algorithms  
  - `winmain.cpp` → Game loop, input handling, rendering pipeline  

---

## ⚙️ Installation & Running
1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/CannonSurvivalGame.git
   cd CannonSurvivalGame
   ```

2. Open the project in **Visual Studio** (Windows only).  
   Make sure you have **Windows SDK** installed.

3. Build & Run:
   - Press **F5** to compile and run.
   - A game window will appear (`800x600` resolution).

---

## 🎯 How to Play
- Move the character with **WASD / Arrow keys**.
- Avoid the cannonballs shot from both sides.
- Survive as long as possible!
- If hit, a **Game Over** dialog will appear with an option to **Restart or Quit**.

---

## 📚 Algorithms Implemented
This project doubles as a **computer graphics learning project**, featuring:
- Line Drawing: **DDA**, **Bresenham**, **Midpoint**, Anti-Aliased Midpoint  
- Circle Drawing: **Midpoint Circle Algorithm**  
- Ellipse Drawing: **Midpoint Ellipse Algorithm**  
- Polygon Drawing & Filling  
- **Cohen–Sutherland Line Clipping**  

---

## 📝 Future Improvements
- Add scoring system (survival time counter)  
- Add difficulty levels (faster cannonballs, more cannons)  
- Implement character textures instead of simple shapes  
- Add sound effects for cannon firing & collisions  

---

## 👨‍💻 Author
Developed as a **learning project for Computer Graphics in C++**.  
If you like this, ⭐ star the repo and contribute enhancements!
