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

#ifndef COLOSEEDSEARCHER_HPP
#define COLOSEEDSEARCHER_HPP

#include <Core/RNG/LCRNG.hpp>
#include <atomic>
#include <mutex>
#include <vector>

struct ColoCriteria
{
    u8 lead;
    u8 trainer;
};

/**
 * @brief Searches for candidate PRNG states
 */
class ColoSeedSearcher
{
public:
    /**
     * @brief Construct a new GalesSeedSearcher object
     *
     * @param criteria Filtering data
     */
    ColoSeedSearcher(const ColoCriteria &criteria);

    /**
     * @brief Cancels the running search
     */
    void cancelSearch();

    /**
     * @brief Returns the progress of the running search
     *
     * @return Progress
     */
    int getProgress() const;

    /**
     * @brief Returns the states of the running search
     *
     * @return Vector of computed states
     */
    std::vector<u32> getResults() const;

    /**
     * @brief Starts the search
     *
     * @param threads Number of threads to search with
     */
    void startSearch(int threads);

    /**
     * @brief Starts the search
     *
     * @param seeds Candidiate PRNG states to search from
     */
    void startSearch(const std::vector<u32> &seeds);

private:
    std::mutex mutex;
    std::vector<u32> results;
    std::atomic<u32> progress;
    bool searching;
    ColoCriteria criteria;

    /**
     * @brief Generates a pokemon that has the matching \p nature and \p gender
     *
     * @param rng Starting PRNG state
     * @param tsv Trainer shiny value
     * @param nature Pokemon nature
     * @param gender Pokemon gender
     * @param genderRatio Pokemon gender ratio
     */
    void generatePokemon(XDRNG &rng, u16 tsv, u8 nature, u8 gender, u8 genderRatio) const;

    /**
     * @brief Searches over a range of PRNG states for valid candidate seeds
     *
     * @param start Lower PRNG state
     * @param end Upper PRNG state
     */
    void search(u32 start, u32 end);

    /**
     * @brief Determines if PRNG state is valid for the criteria
     *
     * @param rng Starting PRNG state
     *
     * @return true PRNG state is valid
     * @return false PRNG state is not valid
     */
    bool searchSeed(XDRNG &rng) const;

    /**
     * @brief Determines if PRNG state is valid for the criteria
     *
     * @param rng Starting PRNG state
     *
     * @return true PRNG state is valid
     * @return false PRNG state is not valid
     */
    bool searchSeedSkip(XDRNG &rng) const;
};
#endif // COLOSEEDSEARCHER_HPP
