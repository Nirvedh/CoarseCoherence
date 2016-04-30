/*
 * Copyright (c) 1999-2012 Mark D. Hill and David A. Wood
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

#include "base/intmath.hh"
#include "debug/RubyCache.hh"
#include "debug/RubyCacheTrace.hh"
#include "debug/RubyResourceStalls.hh"
#include "debug/RubyStats.hh"
#include "debug/RubyPort.hh"
#include "mem/protocol/AccessPermission.hh"
#include "mem/ruby/structures/RCTable.hh"

using namespace std;
int const max_granularity = 256;
int const min_granularity = 16;
int const gstep=1;

/*CHECK*: int state --> State state? */

unordered_map<Addr,RCEntryL1> RCTableL1; // L1 table
unordered_map<Addr,RCEntryL2> RCTableL2; // L2 table
//unordered_map<Addr,RCEntryL1> ::iterator it; // L2 table
ostream&
operator<<(ostream& out, const RCTable& obj)
{
    //obj.print(out);
    out << flush;
    return out;
}

RCTable *
RubyRCTableParams::create()
{
    return new RCTable(this);
}
RCTable::RCTable(const Params *p)
    : SimObject(p)
{
        
    isL1Cache = p->isL1Cache;
}
RCTable::~RCTable()
{
}

void
RCTable::init()
{
}
int
RCTable::test(int a) const // This function was just used to test the working of interface. Leaving it for later debugging
{
  return 2*a;
}
void
RCTable::allocate(Addr address, int granularity)//, L1Cache_State state=L1Cache_State_NP)
{
  RCEntryL1 entry;
  entry.granularity = granularity;
  entry.state = "L1Cache_State_NP";
  RCTableL1.insert(make_pair(address,entry));
}
void
RCTable::allocate_l2(Addr address, MachineID Requester)//, L2Cache_State state=L2Cache_State_NP) /*CHECK* state*/
{
  RCEntryL2 entry;
  entry.granularity = max_granularity;
  entry.state = "L2Cache_State_NP";
  entry.sharer.add(Requester);
  RCTableL2.insert(make_pair(address,entry));
}
string
RCTable::getRCCL1State(Addr address)
{
  int gIter = max_granularity;
  string state= "L1Cache_State_NP";
    while(gIter >= min_granularity)
    {
      RCTableL1.count(maskLowOrderBits(address,gIter));
      gIter=gIter>>1;
    }
  return state;
}
string
RCTable::getRCCL2State(Addr address)
{
  int gIter = max_granularity;
  string state= "L2Cache_State_NP";
    while(gIter >= min_granularity)
    {
      RCTableL2.count(maskLowOrderBits(address,gIter));
      gIter=gIter>>1;
    }
  return state;
}
void 
RCTable::setRCCL1State(Addr address,string state)
{
}
void 
RCTable::setRCCL2State(Addr address,string state)
{
}
int 
RCTable::getGranularity(Addr address)
{
  return 0;
}
Addr 
RCTable::getMask(Addr address)
{
  return 0;
}
NetDest 
RCTable::getSharers(Addr address)
{
  return (RCTableL2.begin()->second).sharer;
}
void 
RCTable::addSharer(Addr address,MachineID Requester)
{
}
void 
RCTable::removeSharer(Addr address,MachineID Requester)
{
}
void 
RCTable::clearSharers(Addr address)
{
}
void 
RCTable::split(Addr address)
{
}
void 
RCTable::splitRCC_l2(Addr address,MachineID Requester)
{
} 
bool
RCTable::isPresent_RCC(Addr address)
{
	return true;
} 

