#pragma once

#include "cBuildingListDrawer.h"
#include "cTextDrawer.h"
#include "controls/cKeyboardEvent.h"
#include "observers/cInputObserver.h"
#include "sMouseEvent.h"

class cPlayer;

class cSideBarDrawer : cInputObserver {
public:
    explicit cSideBarDrawer(cPlayer *player);
    ~cSideBarDrawer() override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    void draw();

    cBuildingListDrawer *getBuildingListDrawer() {
        return &m_buildingListDrawer;
    }

    void setPlayer(cPlayer *pPlayer);

protected:
    void drawBuildingLists();
    void drawCapacities();

    void drawCandybar();

    void drawMinimap();

private:
    cPlayer *m_player;
    cBuildingListDrawer m_buildingListDrawer;
    cSideBar *m_sidebar;

    // the 'm_candybar' is the bar with the ball at the top. Colored with the house color.
    BITMAP *m_candybar;
    cTextDrawer m_textDrawer;

    int m_sidebarColor;

    void createCandyBar();

    void drawPowerUsage() const;
    void drawCreditsUsage();
};
