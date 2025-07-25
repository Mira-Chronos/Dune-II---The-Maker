/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "cAbstractMentat.h"

#include "cButtonCommand.h"
#include "d2tmc.h"
#include "definitions.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/RNG.hpp"
#include "gui/cGuiButton.h"
#include "gui/actions/cGuiActionToGameState.h"
#include "utils/Graphics.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <fmt/core.h>

cAbstractMentat::cAbstractMentat(bool canMissionSelect)
{
    iMentatSentence = -1;

    TIMER_Speaking = -1;
    TIMER_Mouth = 0;
    TIMER_Eyes = 0;
    TIMER_Other = 0;
    TIMER_movie = 0;

    iMentatMouth = 3;
    iMentatEyes = 3;
    iMovieFrame = -1;

    iBackgroundFrame = -1;

    house = GENERALHOUSE;

    gfxmovie = nullptr;
    leftButton = nullptr;
    rightButton = nullptr;
    // the mentat does not *own* the bitmaps
    leftButtonBmp = nullptr;
    rightButtonBmp = nullptr;
    leftButtonCommand = nullptr;
    rightButtonCommand = nullptr;

    textDrawer.setFont(bene_font);

    // the quick-way to get to a mission select window
    const eGuiButtonRenderKind buttonKind = TRANSPARENT_WITHOUT_BORDER;
    const eGuiTextAlignHorizontal buttonTextAlignment = CENTER;

    if (canMissionSelect) {

        int length = textDrawer.textLength("Mission select");
        const cRectangle &toMissionSelectRect = *textDrawer.getAsRectangle(game.m_screenW - length,
                                                game.m_screenH - textDrawer.getFontHeight(),
                                                "Mission select");
        cGuiButton *gui_btn_toMissionSelect = new cGuiButton(textDrawer, toMissionSelectRect, "Mission select",
                buttonKind);
        gui_btn_toMissionSelect->setTextAlignHorizontal(buttonTextAlignment);
        cGuiActionToGameState *action = new cGuiActionToGameState(GAME_MISSIONSELECT, false);
        gui_btn_toMissionSelect->setOnLeftMouseButtonClickedAction(action);
        m_guiBtnToMissionSelect = gui_btn_toMissionSelect;
    }
    else {
        m_guiBtnToMissionSelect = nullptr;
    }

    state = INIT;

    font = bene_font;

    // offsetX = 0 for screen resolution 640x480, ie, meaning > 640 we take the difference / 2
    offsetX = (game.m_screenW - 640) / 2;
    offsetY = (game.m_screenH - 480) / 2; // same goes for offsetY (but then for 480 height).
    movieTopleftX = offsetX + 256;
    movieTopleftY = offsetY + 120;
    memset(sentence, 0, sizeof(sentence));
    logbook("cAbstractMentat::cAbstractMentat()");
}

cAbstractMentat::~cAbstractMentat()
{
    delete leftButton;
    delete rightButton;
    leftButtonBmp = nullptr;
    rightButtonBmp = nullptr;

    delete leftButtonCommand;
    delete rightButtonCommand;

    delete m_guiBtnToMissionSelect;

    logbook("cAbstractMentat::~cAbstractMentat()");
}

void cAbstractMentat::think()
{
    if (state == INIT) {
        // no thinking for init state
        return;
    }

    if (state == SPEAKING) {
        if (TIMER_Speaking > 0) {
            TIMER_Speaking--;
        }
        else if (TIMER_Speaking == 0) {
            // calculate speaking stuff

            iMentatSentence += 2; // makes 0, 2, etc.

            // done talking
            if (iMentatSentence > 8) {
                state = AWAITING_RESPONSE;
                return;
            }

            // lentgh calculation of time
            int iLength = strlen(sentence[iMentatSentence]);
            iLength += strlen(sentence[iMentatSentence + 1]);

            // nothing to say, await response
            if (iLength < 2) {
                state = AWAITING_RESPONSE;
                return;
            }

            TIMER_Speaking = iLength * 12;
        }

        thinkMouth();
    }
    else {
        iMentatMouth = 0; // keep lips sealed
    }

    // eyes blink always
    thinkEyes();

    // regardless, think about movie animation
    thinkMovie();
}

void cAbstractMentat::thinkMovie()
{
    if (gfxmovie != nullptr) {
        TIMER_movie++;

        if (TIMER_movie > 20) {
            iMovieFrame++;

            if (iMovieFrame == gfxmovie->getNumberOfFiles()) {
                iMovieFrame = 0;
            }
            TIMER_movie = 0;
        }
    }
}

void cAbstractMentat::thinkEyes()
{
    if (TIMER_Eyes > 0) {
        TIMER_Eyes--;
    }
    else {
        int i = RNG::rnd(100);

        int iWas = iMentatEyes;

        if (i < 30) {
            iMentatEyes = 3;
        }
        else if (i < 60) {
            iMentatEyes = 0;
        }
        else {
            iMentatEyes = 4;
        }

        // its the same
        if (iMentatEyes == iWas) {
            iMentatEyes = RNG::rnd(4);
        }

        if (iMentatEyes != 4) {
            TIMER_Eyes = 90 + RNG::rnd(160);
        }
        else {
            TIMER_Eyes = 30;
        }
    }
}

void cAbstractMentat::thinkMouth()  // MOUTH
{
    if (TIMER_Mouth > 0) {
        TIMER_Mouth--;
    }
    else {
        // still speaking
        if (TIMER_Speaking > 0) {

            int iOld = iMentatMouth;

            if (iMentatMouth == 0) { // mouth is shut
                // when mouth is shut, we wait a bit.
                if (RNG::rnd(100) < 45) {
                    iMentatMouth += (1 + RNG::rnd(4));
                }
                else {
                    TIMER_Mouth = 25; // wait
                }
            }
            else {
                iMentatMouth += (1 + RNG::rnd(4));
            }

            // correct any frame
            if (iMentatMouth > 4) {
                iMentatMouth -= 5;
            }

            // Test if we did not set the timer, when not, we changed stuff, and we
            // have to make sure we do not reshow the same animation.. which looks
            // odd!
            if (TIMER_Mouth <= 0) {
                // did not change mouth, force it
                if (iMentatMouth == iOld) {
                    iMentatMouth++;
                }

                // correct if needed:
                if (iMentatMouth > 4) {
                    iMentatMouth -= 5;
                }
            }
        }
        else {
            iMentatMouth = 0; // when there is no sentence, do not animate mouth
        }

        if (TIMER_Mouth <= 0) {
            TIMER_Mouth = 13 + RNG::rnd(15);
        }
    } // Animating mouth

}

void cAbstractMentat::draw()
{
    renderDrawer->renderClearToColor(Color{7,7,15,255});
    renderDrawer->renderRectColor(offsetX-1, offsetY-1, 641, 481, Color{64, 64,89,255});
    renderDrawer->renderRectColor(offsetX-2, offsetY-2, 642, 482, Color{40,40,60,255});
    renderDrawer->renderRectColor(offsetX-3, offsetY-3, 643, 483, Color{0,0,0,255});

    Texture *tmp = getBackgroundBitmap();
    cRectangle src = {0,0,tmp->w, tmp->h};
    cRectangle dest = {offsetX, offsetY,640, 480};
    renderDrawer->renderStrechSprite(tmp, src, dest);

    // movie
    draw_movie();
    draw_eyes();
    draw_mouth();
    draw_other();

    if (state == SPEAKING) {
        // draw text that is being spoken
        if (iMentatSentence >= 0) {
            textDrawer.drawText(offsetX + 17, offsetY + 17,Color{0, 0, 0,255},sentence[iMentatSentence]);
            textDrawer.drawText(offsetX + 17, offsetY + 17,Color{255, 255, 255,255},sentence[iMentatSentence]);

            textDrawer.drawText(offsetX + 17, offsetY + 37,Color{0, 0, 0,255},sentence[iMentatSentence+1]);
            textDrawer.drawText(offsetX + 17, offsetY + 36,Color{255, 255, 255,255},sentence[iMentatSentence+1]);
        }
    }

    if (state == AWAITING_RESPONSE) {
        renderDrawer->renderSprite(leftButtonBmp, leftButton->getX(),leftButton->getY());
        renderDrawer->renderSprite(rightButtonBmp, rightButton->getX(),rightButton->getY());
    }

    if (m_guiBtnToMissionSelect) {
        m_guiBtnToMissionSelect->draw();
    }
}

Texture *cAbstractMentat::getBackgroundBitmap() const
{
    if (iBackgroundFrame < 0) return nullptr;
    return gfxmentat->getTexture(iBackgroundFrame);
}

void cAbstractMentat::draw_movie()
{
    if (gfxmovie == nullptr) return;
    if (iMovieFrame < 0) return;

    // drawing only, circulating is done in think function
    Texture *tmp = gfxmovie->getTexture(iMovieFrame);
    cRectangle src = {0,0,tmp->w, tmp->h};
    cRectangle dest = {movieTopleftX, movieTopleftY,tmp->w, tmp->h};
    renderDrawer->renderStrechSprite(tmp, src, dest);
}

void cAbstractMentat::interact()
{
    if (state == INIT) return;
    if (state == SPEAKING) {
        if (game.getMouse()->isLeftButtonClicked()) {
            if (TIMER_Speaking > 0) {
                TIMER_Speaking = 1;
            }
        }
        return;
    }
    if (state != AWAITING_RESPONSE) return;

    auto m_mouse = game.getMouse();
    if (m_mouse->isLeftButtonClicked()) {
        // execute left button logic
        if (leftButton && leftButton->isPointWithin(m_mouse->getX(), m_mouse->getY())) {
            leftButtonCommand->execute(*this);
        }

        // execute right button logic
        if (rightButton && rightButton->isPointWithin(m_mouse->getX(), m_mouse->getY())) {
            rightButtonCommand->execute(*this);
        }
    }
}

void cAbstractMentat::initSentences()
{
    memset(sentence, 0, sizeof(sentence));
}

void cAbstractMentat::setSentence(int i, const char *text)
{
    sprintf(sentence[i], "%s", text);
    logbook(fmt::format("Sentence[{}]={}", i, text));
}

void cAbstractMentat::loadScene(const std::string &scene)
{
    gfxmovie = nullptr;

    char filename[255];
    sprintf(filename, "data/scenes/sdl_%s.dat", scene.c_str());

    gfxmovie = std::make_shared<Graphics>(renderDrawer->getRenderer(),filename);

    TIMER_movie = 0;
    iMovieFrame=0;

    if (gfxmovie != nullptr) {
        logbook(fmt::format("Successful loaded scene [{}]", filename));
        return;
    }

    gfxmovie=nullptr;
    logbook(fmt::format("Failed to load scene [{}]", filename));
}

void cAbstractMentat::speak()
{
    TIMER_Speaking = 0;
    TIMER_Mouth = 0;
    iMentatSentence = -2; // ugh
    state = SPEAKING;
}

void cAbstractMentat::buildLeftButton(Texture *bmp, int x, int y)
{
    delete leftButton;
    leftButton = new cRectangle(offsetX + x, offsetY + y, bmp->w, bmp->h);
    leftButtonBmp = bmp;
}

void cAbstractMentat::buildRightButton(Texture *bmp, int x, int y)
{
    delete rightButton;
    rightButton = new cRectangle(offsetX + x, offsetY + y, bmp->w, bmp->h);
    rightButtonBmp = bmp;
}

void cAbstractMentat::resetSpeak()
{
    speak();
}

void cAbstractMentat::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (m_guiBtnToMissionSelect) {
        m_guiBtnToMissionSelect->onNotifyMouseEvent(event);
    }
}

void cAbstractMentat::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (m_guiBtnToMissionSelect) {
        m_guiBtnToMissionSelect->onNotifyKeyboardEvent(event);
    }
}
