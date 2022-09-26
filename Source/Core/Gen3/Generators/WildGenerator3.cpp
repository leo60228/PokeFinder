/*
 * This file is part of PokéFinder
 * Copyright (C) 2017-2022 by Admiral_Fish, bumba, and EzPzStreamz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "WildGenerator3.hpp"
#include <Core/Enum/Encounter.hpp>
#include <Core/Enum/Game.hpp>
#include <Core/Enum/Lead.hpp>
#include <Core/Enum/Method.hpp>
#include <Core/Gen3/EncounterArea3.hpp>
#include <Core/Gen3/States/WildState3.hpp>
#include <Core/Parents/PersonalInfo.hpp>
#include <Core/Parents/Slot.hpp>
#include <Core/Parents/States/WildState.hpp>
#include <Core/RNG/LCRNG.hpp>
#include <Core/Util/EncounterSlot.hpp>

WildGenerator3::WildGenerator3(u32 initialAdvances, u32 maxAdvances, u32 offset, u16 tid, u16 sid, Game version, Method method,
                               Encounter encounter, Lead lead, const WildStateFilter3 &filter) :
    WildGenerator<WildStateFilter3>(initialAdvances, maxAdvances, offset, tid, sid, version, method, encounter, lead, filter)
{
}

std::vector<WildGeneratorState3> WildGenerator3::generate(u32 seed, const EncounterArea3 &encounterArea) const
{
    std::vector<WildGeneratorState3> states;

    std::vector<u8> modifiedSlots = encounterArea.getSlots(lead);
    u16 rate = encounterArea.getRate() * 16;
    bool safari = encounterArea.safariZone(version);
    bool rse = (version & Game::RSE) != Game::None;

    bool cuteCharm = false;
    auto cuteCharmCheck = [this](const PersonalInfo *info, u32 pid) {
        if (lead == Lead::CuteCharmF)
        {
            return (pid & 0xff) >= info->getGender();
        }
        return (pid & 0xff) < info->getGender();
    };

    PokeRNG rng(seed, initialAdvances + offset);
    for (u32 cnt = 0; cnt <= maxAdvances; cnt++, rng.next())
    {
        PokeRNG go(rng.getSeed());

        // RSE uses the main rng to check for rock smash encounters
        if (rse && encounter == Encounter::RockSmash && go.nextUShort(2880) >= rate)
        {
            continue;
        }

        u8 encounterSlot;
        if ((lead == Lead::MagnetPull || lead == Lead::Static) && go.nextUShort(2) == 0 && !modifiedSlots.empty())
        {
            encounterSlot = modifiedSlots[go.nextUShort(modifiedSlots.size())];
        }
        else
        {
            encounterSlot = EncounterSlot::hSlot(go.nextUShort(100), encounter);
        }

        if (!filter.compareEncounterSlot(encounterSlot))
        {
            continue;
        }

        // Modify level based on pressure if necessary
        u8 level = encounterArea.calculateLevel(encounterSlot, go, lead == Lead::Pressure);

        const Slot &slot = encounterArea.getPokemon(encounterSlot);
        const PersonalInfo *info = slot.getInfo();
        if (lead == Lead::CuteCharmM || lead == Lead::CuteCharmF)
        {
            switch (info->getGender())
            {
            case 0:
            case 254:
            case 255:
                cuteCharm = false;
                break;
            default:
                cuteCharm = go.nextUShort(3) != 0;
                break;
            }
        }

        // Safari zone in RSE takes an extra RNG call to determine shuffling of pokeblocks
        if (safari)
        {
            go.next();
        }

        u8 nature;
        if (lead <= Lead::SynchronizeEnd)
        {
            nature = go.nextUShort(2) == 0 ? toInt(lead) : go.nextUShort(25);
        }
        else
        {
            nature = go.nextUShort(25);
        }

        if (!filter.compareNature(nature))
        {
            continue;
        }

        u32 pid;
        do
        {
            u16 low = go.nextUShort();
            u16 high = go.nextUShort();
            pid = (high << 16) | low;
        } while (pid % 25 != nature || (cuteCharm && !cuteCharmCheck(info, pid)));

        if (method == Method::MethodH2)
        {
            go.next();
        }
        u16 iv1 = go.nextUShort();
        if (method == Method::MethodH4)
        {
            go.next();
        }
        u16 iv2 = go.nextUShort();

        WildGeneratorState3 state(initialAdvances + cnt, pid, nature, iv1, iv2, tsv, level, encounterSlot, slot.getSpecie(), info);
        if (filter.compareState(state))
        {
            states.emplace_back(state);
        }
    }

    return states;
}