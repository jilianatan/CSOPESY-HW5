#include "Marquee.h"

MarqueeConsole::MarqueeConsole(const String& title) : name(title) { // Initialize name in initializer list
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle for the console
    GetConsoleScreenBufferInfo(consoleHandle, &screenBufferInfo);  // Retrieve initial buffer info
}

MarqueeConsole::~MarqueeConsole() {
}

void MarqueeConsole::activate() {
    running = true;

    // Hide cursor for cleaner display
    CONSOLE_CURSOR_INFO cursorSettings;
    GetConsoleCursorInfo(consoleHandle, &cursorSettings);
    cursorSettings.bVisible = false;
    SetConsoleCursorInfo(consoleHandle, &cursorSettings);

    // Start the blinking cursor thread
    cursorThread = std::thread(&MarqueeConsole::toggleCursorBlink, this);
}

void MarqueeConsole::deactivate() {
    running = false;

    // Restore the cursor visibility
    CONSOLE_CURSOR_INFO cursorSettings;
    GetConsoleCursorInfo(consoleHandle, &cursorSettings);
    cursorSettings.bVisible = true;
    SetConsoleCursorInfo(consoleHandle, &cursorSettings);

    if (cursorThread.joinable()) {
        cursorThread.join();
    }
}

void MarqueeConsole::updateInput(bool useThread) {
    do {
        if (_kbhit()) {  // Check for key press
            char key = _getch();  // Fetch key

            if (key == 27) {  // ESC key to stop
                running = false;
                break;
            }
            else if (key == 13) {  // Enter key
                currentInput.push_back('\0');
                commandHistory.emplace_back(currentInput.data());
                currentInput.clear();
                cursorPosition = 0;
            }
            else if (key == 8) {  // Backspace key
                if (!currentInput.empty() && cursorPosition > 0) {
                    currentInput.erase(currentInput.begin() + cursorPosition - 1);
                    cursorPosition--;
                }
            }
            else {  // Other printable keys
                currentInput.insert(currentInput.begin() + cursorPosition, key);
                cursorPosition++;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(INPUT_DELAY));
    } while (running && useThread);
}

void MarqueeConsole::updateMarquee() {
    currentX += xSpeed;

    // Change direction when hitting the screen boundaries
    if (currentX <= 0 || currentX >= screenWidth - textSize - 1) {
        xSpeed *= -1;
    }
}

void MarqueeConsole::render() {
    drawHeader();
    drawMarquee();

    std::cout << std::endl;
    drawInput();
    showCommandHistory();

    GetConsoleScreenBufferInfo(consoleHandle, &screenBufferInfo);
    COORD cursorLocation = screenBufferInfo.dwCursorPosition;
    SHORT consoleHeight = screenBufferInfo.srWindow.Bottom - screenBufferInfo.srWindow.Top + 1;

    if (screenHeight + commandHistory.size() + 5 <= consoleHeight) {
        cursorLocation.X = 0;
        cursorLocation.Y = 0;
        SetConsoleCursorPosition(consoleHandle, cursorLocation);

        std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL));
    }
    else {
        system("cls");
        std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL));
    }
}

void MarqueeConsole::drawHeader() {
    std::cout << "*****************************************" << std::endl;
    std::cout << "* Displaying a marquee console! *" << std::endl;
    std::cout << "*****************************************" << std::endl;
}

void MarqueeConsole::drawMarquee() {
    for (int row = 0; row < screenHeight; row++) {
        if (row == currentY) {
            std::cout << String(currentX, ' ') << displayText << String(screenWidth - currentX - textSize, ' ') << std::endl;
        }
        else {
            std::cout << String(screenWidth, ' ') << std::endl;
        }
    }
}

void MarqueeConsole::drawInput() {
    GetConsoleScreenBufferInfo(consoleHandle, &screenBufferInfo);
    COORD cursorLocation = screenBufferInfo.dwCursorPosition;
    cursorLocation.X = 0;
    SetConsoleCursorPosition(consoleHandle, cursorLocation);

    std::cout << String(screenBufferInfo.dwSize.X, ' ');
    SetConsoleCursorPosition(consoleHandle, cursorLocation);

    std::cout << inputPrompt;

    for (int i = 0; i < currentInput.size(); ++i) {
        if (i == cursorPosition) {
            std::cout << (isCursorVisible ? '_' : ' ');
        }
        std::cout << currentInput[i];
    }

    if (cursorPosition == currentInput.size()) {
        std::cout << (isCursorVisible ? '_' : ' ');
    }

    std::cout << std::endl;
}

void MarqueeConsole::toggleCursorBlink() {
    while (running) {
        isCursorVisible = !isCursorVisible;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));  // Blink every 500 ms
    }
}

void MarqueeConsole::showCommandHistory() {
    for (const String& cmd : commandHistory) {
        std::cout << "Command processed in MARQUEE_CONSOLE: " << cmd;

        if (&cmd != &commandHistory.back()) {
            std::cout << std::endl;
        }
    }
}
