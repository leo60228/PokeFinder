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

#include "Encounters8.hpp"
#include <Core/Enum/Encounter.hpp>
#include <Core/Enum/Game.hpp>
#include <Core/Gen8/EncounterArea8.hpp>
#include <Core/Gen8/Profile8.hpp>
#include <Core/Gen8/StaticTemplate8.hpp>
#include <Core/Parents/PersonalLoader.hpp>
#include <Core/Parents/Slot.hpp>
#include <Core/Resources/Encounters.hpp>
#include <algorithm>
#include <bzlib.h>
#include <cstring>
#include <iterator>

constexpr std::array<StaticTemplate8, 3> starters = {
    StaticTemplate8(Game::BDSP, 387, 5), // Turtwig
    StaticTemplate8(Game::BDSP, 390, 5), // Chimchar
    StaticTemplate8(Game::BDSP, 393, 5) // Piplup
};

constexpr std::array<StaticTemplate8, 3> gifts = {
    StaticTemplate8(Game::BDSP, 133, 5), // Eevee
    StaticTemplate8(Game::BDSP, 440, 1), // Happiny egg
    StaticTemplate8(Game::BDSP, 447, 1) // Riolu egg
};

constexpr std::array<StaticTemplate8, 7> fossils = {
    StaticTemplate8(Game::BDSP, 138, 1, 3), // Omanyte
    StaticTemplate8(Game::BDSP, 140, 1, 3), // Kabuto
    StaticTemplate8(Game::BDSP, 142, 1, 3), // Aerodactyl
    StaticTemplate8(Game::BDSP, 345, 1, 3), // Lileep
    StaticTemplate8(Game::BDSP, 347, 1, 3), // Anorith
    StaticTemplate8(Game::BDSP, 408, 1, 3), // Cranidos
    StaticTemplate8(Game::BDSP, 410, 1, 3) // Shieldon
};

constexpr std::array<StaticTemplate8, 3> stationary = {
    StaticTemplate8(Game::BDSP, 425, 22), // Drifloon
    StaticTemplate8(Game::BDSP, 442, 25), // Spiritomb
    StaticTemplate8(Game::BDSP, 479, 15) // Rotom
};

constexpr std::array<StaticTemplate8, 2> roamers = {
    StaticTemplate8(Game::BDSP, 481, 50, 3), // Mespirit
    StaticTemplate8(Game::BDSP, 488, 50, 3) // Cresselia
};

constexpr std::array<StaticTemplate8, 7> legends = {
    StaticTemplate8(Game::BDSP, 480, 50, 3), // Uxie
    StaticTemplate8(Game::BDSP, 482, 50, 3), // Azelf
    StaticTemplate8(Game::BD, 483, 47, 3), // Dialga
    StaticTemplate8(Game::SP, 484, 47, 3), // Palkia
    StaticTemplate8(Game::BDSP, 485, 70, 3), // Heatran
    StaticTemplate8(Game::BDSP, 486, 70, 3), // Regigigas
    StaticTemplate8(Game::BDSP, 487, 70, 3) // Giratina
};

constexpr std::array<StaticTemplate8, 11> ramanasParkPureSpace = {
    StaticTemplate8(Game::SP, 144, 70, 2, 3), // Articuno
    StaticTemplate8(Game::SP, 145, 70, 2, 3), // Zapdos
    StaticTemplate8(Game::SP, 146, 70, 2, 3), // Moltres
    StaticTemplate8(Game::BD, 243, 70, 2, 3), // Raikou
    StaticTemplate8(Game::BD, 244, 70, 2, 3), // Entei
    StaticTemplate8(Game::BD, 245, 70, 2, 3), // Suicune
    StaticTemplate8(Game::BDSP, 377, 70, 2, 3), // Regirock
    StaticTemplate8(Game::BDSP, 378, 70, 2, 3), // Regice
    StaticTemplate8(Game::BDSP, 379, 70, 2, 3), // Registeel
    StaticTemplate8(Game::BDSP, 380, 70, 3), // Latias
    StaticTemplate8(Game::BDSP, 381, 70, 3) // Latios
};

constexpr std::array<StaticTemplate8, 6> ramanasParkStrangeSpace = {
    StaticTemplate8(Game::BDSP, 150, 70, 2, 3), // Mewtwo
    StaticTemplate8(Game::SP, 249, 70, 2, 3), // Lugia
    StaticTemplate8(Game::BD, 250, 70, 2, 3), // Ho-Oh
    StaticTemplate8(Game::BDSP, 382, 70, 3), // Kyogre
    StaticTemplate8(Game::BDSP, 383, 70, 3), // Groudon
    StaticTemplate8(Game::BDSP, 384, 70, 3) // Rayquaza
};

constexpr std::array<StaticTemplate8, 5> mythics = {
    StaticTemplate8(Game::BDSP, 151, 1, 1, Shiny::Never, 3), // Mew
    StaticTemplate8(Game::BDSP, 385, 5, 1, Shiny::Never, 3), // Jirachi
    StaticTemplate8(Game::BDSP, 491, 50, 3), // Darkrai
    StaticTemplate8(Game::BDSP, 492, 30, 3), // Shaymin
    StaticTemplate8(Game::BDSP, 493, 80, 3) // Arceus
};

namespace Encounters8
{
    namespace
    {

        void modifySwarm(std::vector<Slot> &mons, const u8 *data, const PersonalInfo *info, bool swarm)
        {
            if (swarm)
            {
                u16 species[2] = { *reinterpret_cast<const u16 *>(data + 38), *reinterpret_cast<const u16 *>(data + 40) };
                mons[0].setSpecie(species[0], &info[species[0]]);
                mons[1].setSpecie(species[1], &info[species[1]]);
            }
        }

        void modifyTime(std::vector<Slot> &mons, const u8 *data, const PersonalInfo *info, int time)
        {
            u16 specie1;
            u16 specie2;
            if (time == 1)
            {
                specie1 = *reinterpret_cast<const u16 *>(data + 42);
                specie2 = *reinterpret_cast<const u16 *>(data + 44);
            }
            else if (time == 2)
            {
                specie1 = *reinterpret_cast<const u16 *>(data + 46);
                specie2 = *reinterpret_cast<const u16 *>(data + 48);
            }
            else
            {
                return;
            }

            mons[2].setSpecie(specie1, &info[specie1]);
            mons[3].setSpecie(specie2, &info[specie2]);
        }

        void modifyRadar(std::vector<Slot> &mons, const u8 *data, const PersonalInfo *info, bool radar)
        {
            if (radar)
            {
                u16 species[4] = { *reinterpret_cast<const u16 *>(data + 50), *reinterpret_cast<const u16 *>(data + 52),
                                   *reinterpret_cast<const u16 *>(data + 54), *reinterpret_cast<const u16 *>(data + 56) };
                mons[4].setSpecie(species[0], &info[species[0]]);
                mons[5].setSpecie(species[1], &info[species[1]]);
                mons[10].setSpecie(species[2], &info[species[2]]);
                mons[11].setSpecie(species[3], &info[species[3]]);
            }
        }

        std::vector<EncounterArea8> getBDSP(Encounter encounter, int time, bool radar, bool swarm, Game version, const PersonalInfo *info)
        {
            const u8 *compressedData;
            size_t compressedSize;

            if (version == Game::BD)
            {
                compressedData = bd.data();
                compressedSize = bd.size();
            }
            else
            {
                compressedData = sp.data();
                compressedSize = sp.size();
            }

            u32 size = *reinterpret_cast<const u16 *>(compressedData);
            u8 *data = new u8[size];

            BZ2_bzBuffToBuffDecompress(reinterpret_cast<char *>(data), &size,
                                       reinterpret_cast<char *>(const_cast<u8 *>(compressedData + sizeof(u16))), compressedSize, 0, 0);

            std::vector<EncounterArea8> encounters;
            for (size_t offset = 0; offset < size; offset += 142)
            {
                const u8 *entry = data + offset;

                u8 location = entry[0];
                u8 grass = entry[1];
                u8 surf = entry[58];
                u8 old = entry[79];
                u8 good = entry[100];
                u8 super = entry[121];

                switch (encounter)
                {
                case Encounter::Grass:
                    if (grass != 0)
                    {
                        std::vector<Slot> slots;
                        slots.reserve(12);
                        for (size_t i = 0; i < 12; i++)
                        {
                            u8 level = entry[2 + (i * 3)];
                            u16 specie = *reinterpret_cast<const u16 *>(entry + 3 + (i * 3));
                            slots.emplace_back(specie, level, &info[specie]);
                        }
                        modifySwarm(slots, entry, info, swarm);
                        modifyTime(slots, entry, info, time);
                        modifyRadar(slots, entry, info, radar);
                        encounters.emplace_back(location, grass, encounter, slots);
                    }
                    break;
                case Encounter::Surfing:
                    if (surf != 0)
                    {
                        std::vector<Slot> slots;
                        slots.reserve(5);
                        for (size_t i = 0; i < 5; i++)
                        {
                            u8 max = entry[59 + (i * 4)];
                            u8 min = entry[60 + (i * 4)];
                            u16 specie = *reinterpret_cast<const u16 *>(entry + 61 + (i * 4));
                            slots.emplace_back(specie, min, max, &info[specie]);
                        }
                        encounters.emplace_back(location, surf, encounter, slots);
                    }
                    break;
                case Encounter::OldRod:
                    if (old != 0)
                    {
                        std::vector<Slot> slots;
                        slots.reserve(5);
                        for (size_t i = 0; i < 5; i++)
                        {
                            u8 max = entry[80 + (i * 4)];
                            u8 min = entry[81 + (i * 4)];
                            u16 specie = *reinterpret_cast<const u16 *>(entry + 82 + (i * 4));
                            slots.emplace_back(specie, min, max, &info[specie]);
                        }
                        encounters.emplace_back(location, old, encounter, slots);
                    }
                    break;
                case Encounter::GoodRod:
                    if (good != 0)
                    {
                        std::vector<Slot> slots;
                        slots.reserve(5);
                        for (size_t i = 0; i < 5; i++)
                        {
                            u8 max = entry[101 + (i * 4)];
                            u8 min = entry[102 + (i * 4)];
                            u16 specie = *reinterpret_cast<const u16 *>(entry + 103 + (i * 4));
                            slots.emplace_back(specie, min, max, &info[specie]);
                        }
                        encounters.emplace_back(location, good, encounter, slots);
                    }
                    break;
                case Encounter::SuperRod:
                    if (super != 0)
                    {
                        std::vector<Slot> slots;
                        slots.reserve(5);
                        for (size_t i = 0; i < 5; i++)
                        {
                            u8 max = entry[122 + (i * 4)];
                            u8 min = entry[123 + (i * 4)];
                            u16 specie = *reinterpret_cast<const u16 *>(entry + 124 + (i * 4));
                            slots.emplace_back(specie, min, max, &info[specie]);
                        }
                        encounters.emplace_back(location, super, Encounter::SuperRod, slots);
                    }
                    break;
                default:
                    break;
                }
            }
            delete[] data;
            return encounters;
        }
    }

    std::vector<EncounterArea8> getEncounters(Encounter encounter, int time, bool radar, bool swarm, const Profile8 *profile)
    {
        const auto *info = PersonalLoader::getPersonal(profile->getVersion());
        return getBDSP(encounter, time, radar, swarm, profile->getVersion(), info);
    }

    const StaticTemplate8 *getStaticEncounters(int index, int *size)
    {
        if (index == 0)
        {
            if (size)
            {
                *size = starters.size();
            }
            return starters.data();
        }
        else if (index == 1)
        {
            if (size)
            {
                *size = gifts.size();
            }
            return gifts.data();
        }
        else if (index == 2)
        {
            if (size)
            {
                *size = fossils.size();
            }
            return fossils.data();
        }
        else if (index == 3)
        {
            if (size)
            {
                *size = stationary.size();
            }
            return stationary.data();
        }
        else if (index == 4)
        {
            if (size)
            {
                *size = roamers.size();
            }
            return roamers.data();
        }
        else if (index == 5)
        {
            if (size)
            {
                *size = legends.size();
            }
            return legends.data();
        }
        else if (index == 6)
        {
            if (size)
            {
                *size = ramanasParkPureSpace.size();
            }
            return ramanasParkPureSpace.data();
        }
        else if (index == 7)
        {
            if (size)
            {
                *size = ramanasParkStrangeSpace.size();
            }
            return ramanasParkStrangeSpace.data();
        }
        else
        {
            if (size)
            {
                *size = mythics.size();
            }
            return mythics.data();
        }
    }

    const StaticTemplate8 *getStaticEncounter(int type, int index)
    {
        const StaticTemplate8 *templates = getStaticEncounters(type);
        return &templates[index];
    }
}