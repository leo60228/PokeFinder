/*
 * This file is part of PokéFinder
 * Copyright (C) 2017-2024 by Admiral_Fish, bumba, and EzPzStreamz
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

#ifndef STATICGENERATOR5_HPP
#define STATICGENERATOR5_HPP

#include <Core/Gen5/Profile5.hpp>
#include <Core/Parents/Filters/StateFilter.hpp>
#include <Core/Parents/Generators/StaticGenerator.hpp>
#include <Core/Parents/StaticTemplate.hpp>

class State5;

/**
 * @brief Static encounter generator for Gen5
 */
class StaticGenerator5 : public StaticGenerator<StaticTemplate, Profile5, StateFilter>
{
public:
    /**
     * @brief Construct a new StaticGenerator5 object
     *
     * @param initialAdvances Initial number of advances
     * @param maxAdvances Maximum number of advances
     * @param delay Number of advances to offset
     * @param method Encounter method
     * @param lead Encounter lead
     * @param staticTemplate Pokemon template
     * @param profile Profile Information
     * @param filter State filter
     */
    StaticGenerator5(u32 initialAdvances, u32 maxAdvances, u32 delay, Method method, Lead lead, const StaticTemplate &staticTemplate,
                     const Profile5 &profile, const StateFilter &filter);

    /**
     * @brief Generates states for the \p encounterArea
     *
     * @param seed Starting PRNG state
     * @param initialAdvances Initial number of IV advances
     * @param maxAdvances Maximum number of IV advances
     *
     * @return Vector of computed states
     */
    std::vector<State5> generate(u64 seed, u32 initialAdvances, u32 maxAdvances) const;

    /**
     * @brief Generates states for the \p encounterArea
     *
     * @param seed Starting PRNG state
     * @param iv Vector of IV advances and IVs
     *
     * @return Vector of computed states
     */
    std::vector<State5> generate(u64 seed, const std::vector<std::pair<u32, std::array<u8, 6>>> &ivs) const;
};

#endif // STATICGENERATOR5_HPP