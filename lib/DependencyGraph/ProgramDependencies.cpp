/** ---*- C++ -*--- ProgramDependencies.cpp
 *
 * Copyright (C) 2012 Marco Minutoli <mminutoli@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */


#include "cot/DependencyGraph/ProgramDependencies.h"
#include "cot/DependencyGraph/DataDependencies.h"
#include "cot/DependencyGraph/ControlDependencies.h"

#include "cot/AllPasses.h"
#include "llvm/Function.h"
#include "llvm/Support/raw_ostream.h"


using namespace cot;
using namespace llvm;


char ProgramDependencyGraph::ID = 0;


bool ProgramDependencyGraph::runOnFunction(Function &F)
{
  DataDepGraph* DDG = getAnalysis<DataDependencyGraph>().DDG;
  DataDepGraph* CDG = getAnalysis<ControlDependencyGraph>().CDG;

  for (Function::BasicBlockListType::const_iterator it = F.getBasicBlockList().begin(); it != F.getBasicBlockList().end(); ++it)
  {
    if (CDG->depends(CDG->getRootNode()->getData(), &*it ))
      PDG->addDependency(static_cast<BasicBlock *>(0), &*it, CONTROL);
    for (Function::BasicBlockListType::const_iterator it2 = F.getBasicBlockList().begin(); it2 != F.getBasicBlockList().end(); ++it2)
    {
      if(DDG->depends(&*it, &*it2))
        PDG->addDependency(&*it, &*it2, DATA);
      if(CDG->depends(&*it, &*it2))
        PDG->addDependency(&*it, &*it2, CONTROL);
    }
  }
  return false;
}


void ProgramDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.addRequired<DataDependencyGraph>();
  AU.addRequired<ControlDependencyGraph>();
  AU.setPreservesAll();
}



void ProgramDependencyGraph::print(llvm::raw_ostream &OS, const llvm::Module*) const
{
  PDG->print(OS, getPassName());
}

ProgramDependencyGraph *CreateProgramDependencyGraphPass()
{
  return new ProgramDependencyGraph();
}


INITIALIZE_PASS(ProgramDependencyGraph, "pdg",
                "Program Dependency Graph Construction",
                true,
                true)
