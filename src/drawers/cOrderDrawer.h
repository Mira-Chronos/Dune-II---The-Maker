#pragma once

#include "sMouseEvent.h"
#include "utils/cRectangle.h"

class cPlayer;
struct SDL_Surface;
class Texture;

class cOrderDrawer {
public:
    explicit cOrderDrawer(cPlayer *thePlayer);

    ~cOrderDrawer();

    void drawOrderButton(cPlayer *thePlayer);

    void onNotify(const s_MouseEvent &event);

    void setPlayer(cPlayer *pPlayer);

private:
    void drawRectangleOrderButton();

    void onMouseAt(const s_MouseEvent &event);
    void onMouseClickedLeft(const s_MouseEvent &event);

    bool _isMouseOverOrderButton;

    cPlayer *player;
    cRectangle buttonRect;
    Texture *buttonBitmap;
};
