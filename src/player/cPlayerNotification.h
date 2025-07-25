#pragma once

#include <string>
#include "utils/Color.hpp"

enum eNotificationType {
    NEUTRAL,    // generic message, in white
    PRIORITY,   // a more important message, yellow
    BAD         // bad news, in red
};

std::string eNotificationTypeString(const eNotificationType &type);

class cPlayerNotification {
public:
    cPlayerNotification(const std::string &msg, eNotificationType type);

    void thinkFast();

    bool isVisible() const {
        return TIMER > 0;
    }

    const std::string &getMessage() const;

    int getTimer() const {
        return TIMER;
    }

    Color getColor() const;

private:
    std::string msg;
    int TIMER;
    int initialDuration;
    eNotificationType type;
};
