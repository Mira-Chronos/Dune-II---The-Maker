#pragma once

#include "cPlayerBrainMissionKind.h"
#include "player/cPlayer.h"

namespace brains {

    class cPlayerBrainMission;

    class cPlayerBrainMissionKindSaboteur : public cPlayerBrainMissionKind {

    public:

        cPlayerBrainMissionKindSaboteur(cPlayer *player, cPlayerBrainMission * mission);

        ~cPlayerBrainMissionKindSaboteur() override;

        cPlayerBrainMissionKind * clone(cPlayer *player, cPlayerBrainMission * mission) override;

        bool think_SelectTarget() override;

        void think_Execute() override;

        void onNotifyGameEvent(const s_GameEvent &event) override;

        const char *toString() override { return "cPlayerBrainMissionKindSaboteur"; }

        void onNotify_SpecificStateSwitch(const s_GameEvent &event) override;

    private:
        int targetStructureID;

        void onEventDestroyed(const s_GameEvent &event);
    };

}
