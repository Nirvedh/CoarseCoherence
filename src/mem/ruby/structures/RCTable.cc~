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
int const max_granularity = 64;
int const min_granularity = 1;
int const gstep=1;
int b_sz = RubySystem::getBlockSizeBits();

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
  RCTableL1.insert(make_pair(maskLowOrderBits(address,granularity+b_sz),entry));
}
void
RCTable::allocate_l2(Addr address, MachineID Requester)//, L2Cache_State state=L2Cache_State_NP) /*CHECK* state*/
{
  RCEntryL2 entry;
  entry.granularity = max_granularity;
  entry.state = "L2Cache_State_NP";
  entry.sharer.add(Requester);
  RCTableL2.insert(make_pair(maskLowOrderBits(address,max_granularity+b_sz),entry));
}
string
RCTable::getRCCL1State(Addr address)
{
  int gIter = max_granularity;
  string default_state= "L1Cache_State_NP";
    while(gIter >= min_granularity)
    {
      int count = RCTableL1.count(maskLowOrderBits(address,gIter+ b_sz));
      assert(count <= 1);
      if (count==1 && ((RCTableL1.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
	return ((RCTableL1.find(maskLowOrderBits(address,gIter+b_sz)))->second).state;
      gIter=gIter>>gstep;
    }
  return default_state;
}
string
RCTable::getRCCL2State(Addr address)
{
  int gIter = max_granularity;
  string default_state= "L2Cache_State_NP";
    while(gIter >= min_granularity)
    {
      int count = RCTableL2.count(maskLowOrderBits(address,gIter+ b_sz));
      assert(count <= 1);
      if (count==1 &&  ((RCTableL2.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
	return ((RCTableL2.find(maskLowOrderBits(address,gIter+ b_sz)))->second).state;
      gIter=gIter>>gstep;
    }
  return default_state;
}
void 
RCTable::setRCCL1State(Addr address,string state)
{
    int gIter = max_granularity;
    while(gIter >= min_granularity)
    {
      int count = RCTableL1.count(maskLowOrderBits(address,gIter+ b_sz));
      assert(count <= 1);
      if (count==1 && ((RCTableL1.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
      {
	((RCTableL1.find(maskLowOrderBits(address,gIter+ b_sz)))->second).state=state;
	return;
      }
      gIter=gIter>>gstep;
    }
}
void 
RCTable::setRCCL2State(Addr address,string state)
{
  int gIter = max_granularity;
  while(gIter >= min_granularity)
  {
    int count = RCTableL2.count(maskLowOrderBits(address,gIter+ b_sz));
    assert(count <= 1);
    if (count==1 && ((RCTableL2.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
    {
      ((RCTableL2.find(maskLowOrderBits(address,gIter+ b_sz)))->second).state=state;
      return;
    }
    gIter=gIter>>gstep;
  }
}
int 
RCTable::getGranularity(Addr address)
{
    int gIter = max_granularity;
    if(isL1Cache)
    {
      while(gIter >= min_granularity)
      {
	int count = RCTableL1.count(maskLowOrderBits(address,gIter+ b_sz));
	assert(count <= 1);
	if (count==1 && ((RCTableL1.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
	  return ((RCTableL1.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity;
	gIter=gIter>>gstep;
       }
    }
    else
    {
        while(gIter >= min_granularity)
	{
	  int count = RCTableL2.count(maskLowOrderBits(address,gIter+ b_sz));
	  assert(count <= 1);
	  if (count==1 && ((RCTableL2.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
	    return ((RCTableL2.find(maskLowOrderBits(address,gIter+ b_sz)))->second).granularity;
	  gIter=gIter>>gstep;
	}
      
    }
    
  return 0;
}
Addr 
RCTable::getMask(Addr address)
{
    int gIter = max_granularity;
    if(isL1Cache)
    {
      while(gIter >= min_granularity)
      {
	int count = RCTableL1.count(maskLowOrderBits(address,gIter+ b_sz));
	assert(count <= 1);
	if (count==1 && ((RCTableL1.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter )
	  return ((RCTableL1.find(maskLowOrderBits(address,gIter+b_sz)))->first);
	gIter=gIter>>gstep;
       }
    }
    else
    {
        while(gIter >= min_granularity)
	{
	  int count = RCTableL2.count(maskLowOrderBits(address,gIter+ b_sz));
	  assert(count <= 1);
	  if (count==1 && ((RCTableL2.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
	    return ((RCTableL2.find(maskLowOrderBits(address,gIter+ b_sz)))->first);
	  gIter=gIter>>gstep;
	}
      
    }
  return 0;
}
NetDest 
RCTable::getSharers(Addr address)
{
  int gIter = max_granularity;
  string default_state= "L2Cache_State_NP";
  while(gIter >= min_granularity)
  {
    int count = RCTableL2.count(maskLowOrderBits(address,gIter+ b_sz));
    assert(count <= 1);
    if (count==1 && ((RCTableL2.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
    return ((RCTableL2.find(maskLowOrderBits(address,gIter+ b_sz)))->second).sharer;
      gIter=gIter>>gstep;
   }
 assert(1==0); //Basically pray (hope) you never get here
  
  return (RCTableL2.begin()->second).sharer;
}
void 
RCTable::addSharer(Addr address,MachineID Requester)
{
  int gIter = max_granularity;
  string default_state= "L2Cache_State_NP";
  while(gIter >= min_granularity)
  {
    int count = RCTableL2.count(maskLowOrderBits(address,gIter+ b_sz));
    assert(count <= 1);
    if (count==1 && ((RCTableL2.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
    {
    ((RCTableL2.find(maskLowOrderBits(address,gIter+ b_sz)))->second).sharer.add(Requester);
    return;
    }
      gIter=gIter>>gstep;
   }
 assert(1==0); //pray (hope) you never get here
  
}
void 
RCTable::removeSharer(Addr address,MachineID Requester)
{
  int gIter = max_granularity;
  string default_state= "L2Cache_State_NP";
  while(gIter >= min_granularity)
  {
    int count = RCTableL2.count(maskLowOrderBits(address,gIter+ b_sz));
    assert(count <= 1);
    if (count==1 && ((RCTableL2.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
    {
      ((RCTableL2.find(maskLowOrderBits(address,gIter+ b_sz)))->second).sharer.remove(Requester);
      return;
    }
      gIter=gIter>>gstep;
   }
   assert(1==0); //pray (hope) you never get here
}
void 
RCTable::clearSharers(Addr address)
{
  //DPRINTF(RubySlicc,"got to function in RCTable.cc\n");
  int gIter = max_granularity;
  string default_state= "L2Cache_State_NP";
DPRINTF(RubySlicc,"start clear RCC sharers in RCTable.cc\n");
  while(gIter >= min_granularity)
  {
	//DPRINTF(RubySlicc,"Checking Giter in ClearSharers. Iter: %d: \n",gIter);
    int count = RCTableL2.count(maskLowOrderBits(address,gIter+ b_sz));
    assert(count <= 1);
    if (count==1 && ((RCTableL2.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
    {
	DPRINTF(RubySlicc,"Found in Clear Sharers. about to return at gIter: %d: \n",gIter);
      ((RCTableL2.find(maskLowOrderBits(address,gIter+ b_sz)))->second).sharer.clear();
      return;
    }
	//DPRINTF(RubySlicc,"before shift: %d: \n",gIter);
      gIter=gIter>>gstep;
	//DPRINTF(RubySlicc,"after shift: %d: \n",gIter);
   }
   assert(1==0); //pray (hope) you never get here
  
}
void 
RCTable::split(Addr address)
{
  assert(isL1Cache);
  int gIter = max_granularity;
  while(gIter >= min_granularity)
  {
    int count = RCTableL1.count(maskLowOrderBits(address,gIter+ b_sz));
    assert(count <= 1);
    if (count==1 && ((RCTableL1.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
    {
      Addr curr_addr=(RCTableL1.find(maskLowOrderBits(address,gIter+ b_sz)))->first;
      if(gIter==min_granularity)
      {
	RCTableL1.erase(curr_addr);
      }
      else if (curr_addr<=maskLowOrderBits(address,gIter+ b_sz-1))
      {
	((RCTableL1.find(curr_addr))->second).granularity=gIter-1;
      }
      else
      {
	assert(curr_addr==maskLowOrderBits(address,gIter+ b_sz-1));
	RCEntryL1 temp;
	temp.state =  ((RCTableL1.find(curr_addr))->second).state;
	temp.granularity = gIter-1;
	RCTableL1.erase(curr_addr);
	RCTableL1.insert(make_pair(makeNextStrideAddress(curr_addr,gIter-1),temp));
      }
       return;
     }
    
      gIter=gIter>>gstep;
    }
   assert(1==0); //pray (hope) you never get here
}
void 
RCTable::splitRCC_l2(Addr address,MachineID Requester)
{
  assert(!isL1Cache);
  int gIter = max_granularity;
  while(gIter >= min_granularity)
  {
    int count = RCTableL2.count(maskLowOrderBits(address,gIter+ b_sz));
    assert(count <= 1);
    if (count==1 && ((RCTableL2.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
    {
      Addr curr_addr=(RCTableL2.find(maskLowOrderBits(address,gIter+ b_sz)))->first;
      if(gIter==min_granularity)
      {
	
	RCEntryL2 temp;
	temp.state = (RCTableL2.find(curr_addr)->second).state;
	temp.sharer.add(Requester);
	temp.granularity = min_granularity;
	RCTableL2.erase(curr_addr);
	RCTableL2.insert(make_pair(curr_addr,temp));
	return;
      }
      else if (curr_addr<=maskLowOrderBits(address,gIter+ b_sz-1))
      {
	((RCTableL2.find(curr_addr))->second).granularity=gIter-1;
	RCEntryL2 temp;
	temp.state = (RCTableL2.find(curr_addr)->second).state;
	temp.sharer.add(Requester);
	temp.granularity = gIter-1;
	RCTableL2.insert(make_pair(curr_addr,temp));
      }
      else
      {
	assert(curr_addr==maskLowOrderBits(address,gIter+ b_sz-1));
	RCEntryL2 temp;
	temp.state =  ((RCTableL2.find(curr_addr))->second).state;
	temp.granularity = gIter-1;
	temp.sharer = ((RCTableL2.find(curr_addr))->second).sharer;
	RCTableL2.erase(curr_addr);
	RCTableL2.insert(make_pair(makeNextStrideAddress(curr_addr,gIter-1),temp));
	
	RCEntryL2 temp1;
	temp1.state =  temp.state;
	temp1.granularity = gIter-1;
	temp1.sharer.add(Requester);
	RCTableL2.insert(make_pair(curr_addr,temp));
      }
      return;
     }
      gIter=gIter>>gstep;
    }
   assert(1==0); //pray (hope) you never get here
} 
bool
RCTable::isPresent_RCC(Addr address)
{
 DPRINTF(RubySlicc,"got to function in isPresent RCC in RCTable\n");
    int gIter = max_granularity;
    if(isL1Cache)
    {
	 DPRINTF(RubySlicc,"Thinks L1 cache\n");
      while(gIter >= min_granularity)
      {
	int count = RCTableL1.count(maskLowOrderBits(address,gIter+ b_sz));
	assert(count <= 1);
	if (count==1 && ((RCTableL1.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
	  return true;
	gIter=gIter>>gstep;
       }
    }
    else
    {
	DPRINTF(RubySlicc,"Thinks L2 cache\n");
        while(gIter >= min_granularity)
	{
	
	  int count = RCTableL2.count(maskLowOrderBits(address,gIter+ b_sz));
	  assert(count <= 1);
	  if (count==1 && ((RCTableL2.find(maskLowOrderBits(address,gIter+b_sz)))->second).granularity==gIter)
	    return true;
	  gIter=gIter>>gstep;
	}
      
    }
  return false;
} 

