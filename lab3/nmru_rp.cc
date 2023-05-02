/**
 * Copyright (c) 2018-2020 Inria
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "mem/cache/replacement_policies/nmru_rp.hh"

#include <cassert>
#include <memory>

#include "params/NMRURP.hh"
#include "base/random.hh"
#include "sim/cur_tick.hh"

namespace gem5
{

GEM5_DEPRECATED_NAMESPACE(ReplacementPolicy, replacement_policy);
namespace replacement_policy
{

NMRU::NMRU(const Params &p)
  : Base(p)
{
}

void
NMRU::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
{
    // Reset last touch timestamp
    std::static_pointer_cast<NMRUReplData>(
        replacement_data)->lastTouchTick = Tick(0);
}

void
NMRU::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Update last touch timestamp
    std::static_pointer_cast<NMRUReplData>(
        replacement_data)->lastTouchTick = curTick();
}

void
NMRU::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Set last touch timestamp
    std::static_pointer_cast<NMRUReplData>(
        replacement_data)->lastTouchTick = curTick();
}

ReplaceableEntry*
NMRU::getVictim(const ReplacementCandidates& candidates) const
{
    // There must be at least one replacement candidate
    assert(candidates.size() > 0);

    // if there is only one candidate, return it
    if (candidates.size() == 1) {
        return candidates[0];
    }

    // Visit all candidates to search for an invalid entry. If one is found,
    // its eviction is prioritized
    ReplaceableEntry* victim = candidates[0];
    for (const auto& candidate : candidates) {
        if (!std::static_pointer_cast<NMRUReplData>(
                    candidate->replacementData)->lastTouchTick) {
            victim = candidate;
            return victim;
        }
    }

    // Visit all candidates to find MRU entry
    ReplaceableEntry* MRUentry = candidates[0];
    for (const auto& candidate : candidates) {
        std::shared_ptr<NMRUReplData> candidate_replacement_data =
            std::static_pointer_cast<NMRUReplData>(candidate->replacementData);

        if (candidate_replacement_data->lastTouchTick >
                std::static_pointer_cast<NMRUReplData>(
                    MRUentry->replacementData)->lastTouchTick) {
            MRUentry = candidate;
        }
    }

    // Choose one candidate at random except the MRU entry
    unsigned index = 0;
    while (true) {
        index = random_mt.random<unsigned>(0, candidates.size() - 1);
        if (candidates[index] != MRUentry) {
            victim = candidates[index];
            break;
        }
    }

    return victim;
}

std::shared_ptr<ReplacementData>
NMRU::instantiateEntry()
{
    return std::shared_ptr<ReplacementData>(new NMRUReplData());
}

} // namespace replacement_policy
} // namespace gem5
