#pragma once

class ScreenBase {
public:
    ScreenBase() = default;
    virtual ~ScreenBase() = default;

    // Initialize the screen
    virtual void Init() = 0;

    // Render the screen
    virtual void Render() = 0;

    // Handle key events
    virtual void KeyEvent(int key, int scancode, int action, int mods) = 0;

    // Handle mouse button events
    virtual void MouseButtonEvent(int button, int action, int mods) = 0;

};