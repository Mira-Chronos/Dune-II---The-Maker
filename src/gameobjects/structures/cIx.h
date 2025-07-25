#pragma once

#include "cAbstractStructure.h"

class cIx : public cAbstractStructure {
public:
    cIx();

    void thinkFast() override;

    void thinkSlow() override {}

    void think_animation() override;

    void think_guard() override;

    void startAnimating() override {}

    void draw() override {
        drawWithShadow();
    }

    int getType() const override;

    std::string getStatusForMessageBar() const override {
        return getDefaultStatusMessageBar();
    }

    void onNotifyGameEvent(const s_GameEvent &) override {}
};

