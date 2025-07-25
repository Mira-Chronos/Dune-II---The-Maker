/*

	Dune II - The Maker

	Author : Stefan Hendriks
	Contact: stefan@fundynamic.com
	Website: http://dune2themaker.fundynamic.com

	2001 - 2020 (c) code by Stefan Hendriks

	AbstractMentat class

	The mentat class encapsulates all mentat specific functions.
	This is a global class. In the Mentat directory there will be:

	c<anyhouse>Mentat.cpp / c<anyhouse>Mentat.h which will be derived from this class.

	There is only one stateMentat in-game shown to the player. Therefore a good creation/deletion of it (when switching
	houses) is essential.
*/
#pragma once

#include "utils/cRectangle.h"
#include "observers/cInputObserver.h"
#include "utils/Graphics.hpp"
#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"
#include <string>
#include <memory>
#include <SDL2/SDL_ttf.h>

class cButtonCommand;
class cGuiButton;

struct SDL_Surface;
class Texture;

enum eMentatState {
    INIT,               // for loading data (Default state)
    SPEAKING,           // the mentat is speaking about whatever
    AWAITING_RESPONSE,  // the mentat is done speaking, the user needs to click something (yes/no, proceed/repeat, etc)
    DESTROY,            // the mentat became 'invalid' (BeneGeserit-><house> transition)
};

class cAbstractMentat : public cInputObserver {

protected:
    virtual void draw_mouth() = 0;

    virtual void draw_eyes() = 0;

    virtual void draw_other() = 0;

    void draw_movie();

    // Timed animation
    int TIMER_Mouth;
    int TIMER_Eyes;
    int TIMER_Other;
    int TIMER_Speaking;

    // Movie playback (scene's from datafile)
    int TIMER_movie;
    int iMovieFrame;

    int iBackgroundFrame;

    // draw 2 sentences at once, so 0 1, 2 3, 4 5, 6 7, 8 9
    char sentence[10][255];

    int iMentatSentence;  // = sentence to draw and speak with (-1 = not ready)

    int iMentatMouth;
    int iMentatEyes;

    std::shared_ptr<Graphics> gfxmovie;
    TTF_Font *font;

    eMentatState state;
    cTextDrawer textDrawer;

    cRectangle *leftButton;
    cRectangle *rightButton;

    Texture *leftButtonBmp;
    Texture *rightButtonBmp;

    void buildLeftButton(Texture *bmp, int x, int y);

    void buildRightButton(Texture *bmp, int x, int y);

    cButtonCommand *leftButtonCommand;
    cButtonCommand *rightButtonCommand;

    // this is used for Bene Geserit house selection
    int house;

    int offsetX;
    int offsetY;

public:
    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    virtual void draw() = 0;

    virtual void think() = 0;

    virtual void interact() = 0;

    void loadScene(const std::string &scene);

    cAbstractMentat(bool canMissionSelect);

    virtual ~cAbstractMentat();

    Texture *getBackgroundBitmap() const;

    void initSentences();

    void speak();

    void setSentence(int i, const char text[256]);

    void thinkMouth();

    void thinkEyes();

    void thinkMovie();

    void setHouse(int value) {
        house = value;
    }

    int getHouse() {
        return house;
    }

    void resetSpeak();

    cGuiButton *m_guiBtnToMissionSelect;
    int movieTopleftX;
    int movieTopleftY;
};
