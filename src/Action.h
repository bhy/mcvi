#ifndef __ACTION_H
#define __ACTION_H

#include "Model.h"

enum actType{None, Initial, Macro, Act}; // types of actions

class Action
{
  public:
    int actNum;
    actType type;
    static Model* model;

    explicit Action(int actNum, actType type = None);
    Action();
    virtual ~Action() {}

    bool operator==(const Action& action) const;

    static int compare(const Action& a, const Action& b);

    void operator()(int actNum, actType type = None);

    /**
       Get the type of action given its index in the \a actEdges
    */
    actType getActType();

    void computeType();

    /**
       Given the action type \a aType and action number \a aNum, return
       the position in \a actEdges.
    */
    static long getBeliefAct(Model& model, actType aType, long aNum);
    static void initStatic(Model* model);
};

#endif
