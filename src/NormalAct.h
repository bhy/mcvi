#ifndef __MACROACT_H
#define __MACROACT_H

#include <Action.h>

class MacroAct
{
  public:
    MacroAct(Model& model, int actNum):Act(model,actNum,initial) {}

    /**
       Get the action number from its index in the \a actEdges
    */
    inline long getActNum(Action act)
    {
        return act - model.getNumInitPolicies() - model.getNumMacroActs();
    };

    //inline backup?
}

#endif
