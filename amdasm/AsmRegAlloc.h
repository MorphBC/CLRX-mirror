/*
 *  CLRadeonExtender - Unofficial OpenCL Radeon Extensions Library
 *  Copyright (C) 2014-2018 Mateusz Szpakowski
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __CLRX_ASMREGALLOC_H__
#define __CLRX_ASMREGALLOC_H__

#include <CLRX/Config.h>
#include <iostream>
#include <vector>
#include <utility>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <CLRX/utils/Utilities.h>
#include <CLRX/utils/Containers.h>
#include <CLRX/amdasm/Assembler.h>
#include "AsmInternals.h"

using namespace CLRX;

#define ASMREGALLOC_DEBUGDUMP 0

namespace CLRX
{

typedef AsmRegAllocator::CodeBlock CodeBlock;
typedef AsmRegAllocator::NextBlock NextBlock;
typedef AsmRegAllocator::SSAInfo SSAInfo;
typedef std::pair<const AsmSingleVReg, SSAInfo> SSAEntry;

//  BlockIndex

struct CLRX_INTERNAL BlockIndex
{
    size_t index;
    size_t pass;
    
    BlockIndex(size_t _index = 0, size_t _pass = 0)
            : index(_index), pass(_pass)
    { }
    
    bool operator==(const BlockIndex& v) const
    { return index==v.index && pass==v.pass; }
    bool operator!=(const BlockIndex& v) const
    { return index!=v.index || pass!=v.pass; }
    
    BlockIndex operator+(size_t p) const
    { return BlockIndex(index+p, pass); }
};

};

#if ASMREGALLOC_DEBUGDUMP
extern CLRX_INTERNAL std::ostream& operator<<(std::ostream& os, const CLRX::BlockIndex& v);
#endif

namespace std
{

/// std::hash specialization for CLRX CString
template<>
struct hash<BlockIndex>
{
    typedef BlockIndex argument_type;    ///< argument type
    typedef std::size_t result_type;    ///< result type
    
    /// a calling operator
    size_t operator()(const BlockIndex& r1) const
    {
        std::hash<size_t> h1;
        return h1(r1.index) ^ h1(r1.pass);
    }
};

}

namespace CLRX
{

class CLRX_INTERNAL CBlockBitPool: public std::vector<bool>
{
public:
    CBlockBitPool(size_t n = 0, bool v = false) : std::vector<bool>(n<<1, v)
    { }
    
    reference operator[](BlockIndex i)
    { return std::vector<bool>::operator[](i.index + (i.pass ? (size()>>1) : 0)); }
    const_reference operator[](BlockIndex i) const
    { return std::vector<bool>::operator[](i.index + (i.pass ? (size()>>1) : 0)); }
};

/** Simple cache **/

// map of last SSAId for routine, key - varid, value - last SSA ids
class CLRX_INTERNAL LastSSAIdMap: public
            std::unordered_map<AsmSingleVReg, VectorSet<size_t> >
{
public:
    LastSSAIdMap()
    { }
    
    iterator insertSSAId(const AsmSingleVReg& vreg, size_t ssaId)
    {
        auto res = insert({ vreg, { ssaId } });
        if (!res.second)
            res.first->second.insertValue(ssaId);
        return res.first;
    }
    
    void eraseSSAId(const AsmSingleVReg& vreg, size_t ssaId)
    {
        auto it = find(vreg);
        if (it != end())
             it->second.eraseValue(ssaId);
    }
    
    size_t weight() const
    { return size(); }
};

typedef LastSSAIdMap RBWSSAIdMap;
typedef std::unordered_map<BlockIndex, VectorSet<BlockIndex> > SubrLoopsMap;

struct CLRX_INTERNAL RetSSAEntry
{
    std::vector<BlockIndex> routines;
    VectorSet<size_t> ssaIds;
    size_t prevSSAId; // for curSSAId
};

typedef std::unordered_map<AsmSingleVReg, RetSSAEntry> RetSSAIdMap;

struct CLRX_INTERNAL LoopSSAIdMap
{
    LastSSAIdMap ssaIdMap;
    bool passed;
};

struct CLRX_INTERNAL RoutineData
{
    // rbwSSAIdMap - read before write SSAId's map
    std::unordered_map<AsmSingleVReg, size_t> rbwSSAIdMap;
    std::unordered_map<AsmSingleVReg, size_t> origRbwSSAIdMap;
    LastSSAIdMap curSSAIdMap;
    LastSSAIdMap lastSSAIdMap;
    // key - loop block, value - last ssaId map for loop end
    std::unordered_map<BlockIndex, LoopSSAIdMap> loopEnds;
    bool notFirstReturn;
    size_t weight_;
    
    RoutineData() : notFirstReturn(false), weight_(0)
    { }
    
    void calculateWeight()
    {
        weight_ = rbwSSAIdMap.size() + lastSSAIdMap.weight();
        for (const auto& entry: loopEnds)
            weight_ += entry.second.ssaIdMap.weight();
    }
    
    size_t weight() const
    { return weight_; }
};

struct CLRX_INTERNAL FlowStackEntry
{
    BlockIndex blockIndex;
    size_t nextIndex;
    bool isCall;
    bool haveReturn;
    RetSSAIdMap prevRetSSAIdSets;
};

struct CLRX_INTERNAL FlowStackEntry2
{
    size_t blockIndex;
    size_t nextIndex;
};

struct CLRX_INTERNAL FlowStackEntry3
{
    size_t blockIndex;
    size_t nextIndex;
    bool isCall;
};

struct CLRX_INTERNAL CallStackEntry
{
    BlockIndex callBlock; // index
    size_t callNextIndex; // index of call next
    BlockIndex routineBlock;    // routine block
};

typedef std::unordered_map<BlockIndex, RoutineData> RoutineMap;

typedef std::unordered_map<size_t, std::pair<size_t, size_t> > PrevWaysIndexMap;

class CLRX_INTERNAL ResSecondPointsToCache: public CBlockBitPool
{
public:
    explicit ResSecondPointsToCache(size_t n) : CBlockBitPool(n<<1, false)
    { }
    
    void increase(BlockIndex ip)
    {
        const size_t i = ip.index + (ip.pass ? (size()>>2) : 0);
        if ((*this)[i<<1])
            (*this)[(i<<1)+1] = true;
        else
            (*this)[i<<1] = true;
    }
    
    cxuint count(BlockIndex ip) const
    {
        const size_t i = ip.index + (ip.pass ? (size()>>2) : 0);
        return cxuint((*this)[i<<1]) + (*this)[(i<<1)+1];
    }
};

typedef AsmRegAllocator::SSAReplace SSAReplace; // first - orig ssaid, second - dest ssaid
typedef AsmRegAllocator::SSAReplacesMap SSAReplacesMap;

#if ASMREGALLOC_DEBUGDUMP
#define ARDOut std::cout
#else
struct NoOutput
{
    template<typename T>
    NoOutput& operator<<(const T& v)
    { return *this; }
};

#define ARDOut NoOutput()
#endif

struct Liveness
{
    std::map<size_t, size_t> l;
    
    Liveness() { }
    
    void clear()
    { l.clear(); }
    
    void join(std::map<size_t, size_t>::iterator it)
    {
        if (it != l.begin())
        {
            auto prevIt = it;
            --prevIt;
            if (prevIt->second >= it->first)
            {
                // join with previous region
                prevIt->second = std::max(it->second, prevIt->second);
                l.erase(it);
                it = prevIt;
            }
        }
        
        auto nextIt = it;
        ++nextIt;
        if (nextIt != l.end())
        {
            if (nextIt->first <= it->second)
            {
                // join with next region
                it->second = std::max(it->second, nextIt->second);
                l.erase(nextIt);
            }
        }
    }
    
    void expand(size_t k)
    {
        auto it = l.upper_bound(k);
        if (it != l.begin())
            --it;
        else // do nothing
            return;
        // try expand, lower new bound, then use older
        it->second = std::max(it->second, k+1);
        join(it);
    }
    void newRegion(size_t k)
    { join(l.insert(std::make_pair(k, k)).first); }
    
    void insert(size_t k, size_t k2)
    { join(l.insert(std::make_pair(k, k2)).first); }
    
    bool contain(size_t t) const
    {
        auto it = l.lower_bound(t);
        if (it==l.begin() && it->first>t)
            return false;
        if (it==l.end() || it->first>t)
            --it;
        return it->first<=t && t<it->second;
    }
};

typedef AsmRegAllocator::VarIndexMap VarIndexMap;

typedef std::deque<FlowStackEntry3>::const_iterator FlowStackCIter;

struct CLRX_INTERNAL VRegLastPos
{
    size_t ssaId; // last SSA id
    std::vector<size_t> blockChain; // subsequent blocks that changes SSAId
};

typedef std::unordered_map<AsmSingleVReg, VRegLastPos> LastVRegMap;

struct CLRX_INTERNAL LiveBlock
{
    size_t start;
    size_t end;
    size_t vidx;
    
    bool operator==(const LiveBlock& b) const
    { return start==b.start && end==b.end && vidx==b.vidx; }
    
    bool operator<(const LiveBlock& b) const
    { return start<b.start || (start==b.start &&
            (end<b.end || (end==b.end && vidx<b.vidx))); }
};

typedef AsmRegAllocator::LinearDep LinearDep;
typedef std::unordered_map<size_t, LinearDep> LinearDepMap;

typedef AsmRegAllocator::InterGraph InterGraph;

struct CLRX_INTERNAL SDOLDOCompare
{
    const InterGraph& interGraph;
    const Array<size_t>& sdoCounts;
    
    SDOLDOCompare(const InterGraph& _interGraph, const Array<size_t>&_sdoCounts)
        : interGraph(_interGraph), sdoCounts(_sdoCounts)
    { }
    
    bool operator()(size_t a, size_t b) const
    {
        if (sdoCounts[a] > sdoCounts[b])
            return true;
        return interGraph[a].size() > interGraph[b].size();
    }
};

};

#endif
