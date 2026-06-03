# Tesseris

A 3D rendering engine in C, built with **Vulkan** and **SDL2**.

Tesseris is a personal learning project focused on graphics programming and projective geometry.  
The long-term goal is to build a clean, extensible renderer that can eventually explore ideas like 4D-to-2D projection.

## Why this project exists

This engine is being developed while studying:

- [Projective Geometry course by this-vijay](https://this-vijay.github.io/vision-math/)
- [Foundations for Projective Geometry (Hartshorne)](https://www.math.columbia.edu/~dejong/reu/lib/exe/fetch.php%3Fmedia=hartshorne_foundations_of_projective_geometry.pdf)
- [vulkan-tutorial.com](https://vulkan-tutorial.com/)

## Current status

The renderer is actively being rebuilt to improve GPU support and architecture.

## Build and run

From the repository root:

```bash
make all
make run
```

Or build + run in one step:

```bash
make play
```

## Requirements

- `clang`
- `SDL2`
- `Vulkan SDK`
- `glslangValidator` (for shader compilation)

> Note: The current `Makefile` uses Homebrew-style paths (`/opt/homebrew/...`) by default.

## Project structure

- `src/` — engine source code
- `shaders/` — GLSL shaders
- `build/` — build artifacts

## Preview

<img width="2874" height="1800" alt="Tesseris renderer preview" src="https://github.com/user-attachments/assets/789ce14a-8792-46da-b7fa-f76997289f0e" />
