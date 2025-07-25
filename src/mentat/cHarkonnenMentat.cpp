#include "cHarkonnenMentat.h"

#include "cProceedButtonCommand.h"
#include "cRepeatButtonCommand.h"
#include "d2tmc.h"
#include "data/gfxmentat.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"

cHarkonnenMentat::cHarkonnenMentat(bool allowMissionSelect) : cAbstractMentat(allowMissionSelect)
{
    iBackgroundFrame = MENTATH;
    buildLeftButton(gfxmentat->getTexture(BTN_REPEAT), 293, 423);
    buildRightButton(gfxmentat->getTexture(BTN_PROCEED), 466, 423);
    leftButtonCommand = new cRepeatButtonCommand();
    rightButtonCommand = new cProceedButtonCommand();
}

void cHarkonnenMentat::think()
{
    // think like base class
    cAbstractMentat::think();
}

void cHarkonnenMentat::draw()
{
    cAbstractMentat::draw();
}

void cHarkonnenMentat::draw_other()
{

}

void cHarkonnenMentat::draw_eyes()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(HAR_EYES01+ iMentatEyes), offsetX + 64, offsetY + 256);
}

void cHarkonnenMentat::draw_mouth()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(HAR_MOUTH01+ iMentatMouth), offsetX + 64, offsetY + 288);
}

void cHarkonnenMentat::interact()
{
    cAbstractMentat::interact();
}