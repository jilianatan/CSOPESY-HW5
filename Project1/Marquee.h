#pragma once

#include <iostream>
#include <vector>
#include <windows.h>
#include <string>
#include <conio.h>
#include <thread>
#include <chrono>

using String = std::string;

class MarqueeConsole {
public:
    explicit MarqueeConsole(const String& title);
    ~MarqueeConsole();

    void activate();
    void deactivate();

    void updateInput(bool useThread);
    void updateMarquee();
    void render();

    bool isRunning() const { return running; } // Getter for isRunning

private:
    static const int UPDATE_INTERVAL = 50;
    static const int INPUT_DELAY = 10;

    HANDLE consoleHandle;  // Handle to the console window
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;  // To get buffer info

    int screenWidth = 80;  // Default screen width
    int screenHeight = 20;
    int xSpeed = 1;
    int ySpeed = 1;

    String displayText = "Hello world in marquee!";
    int textSize = displayText.length();
    int currentX = 0;
    int currentY = 0;

    String inputPrompt = "Enter a command for MARQUEE_CONSOLE: ";
    int cursorPosition = 0;
    std::vector<String> commandHistory;
    std::vector<char> currentInput;

    bool isCursorVisible = true;
    std::thread cursorThread;
    bool running = false;

    void drawHeader();
    void drawMarquee();
    void drawInput();
    void toggleCursorBlink();
    void showCommandHistory();
    String name;
};
