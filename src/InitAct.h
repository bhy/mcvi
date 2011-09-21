#ifndel __INITACT_H
#define __INITACT_H

#include "Act.h"

class InitAct : public Act
{
  public:
    InitAct(Model& model, int actNum):Act(model,actNum,initial) {}

    /**
       Get the action number from its index in the \a actEdges
    */
    inline long getActNum(Action act)
    {
        return act; // initial action index
    };

    //inline backup?
}

#endif
