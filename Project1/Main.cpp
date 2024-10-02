#include "Marquee.h"

const bool USE_THREADS = false;

int main() {
    auto marquee = std::make_shared<MarqueeConsole>("MARQUEE_CONSOLE");

    marquee->activate();

    if (USE_THREADS) {
        std::thread keyboardThread([&marquee]() {
            marquee->updateInput(true);
            });

        while (marquee->isRunning()) {  // Use getter method
            marquee->updateMarquee();
            marquee->render();
        }

        keyboardThread.join();
    }
    else {
        while (marquee->isRunning()) {  // Use getter method
            marquee->updateInput(false);
            marquee->updateMarquee();
            marquee->render();
        }
    }

    marquee->deactivate();

    return 0;
}
