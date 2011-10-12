#ifndef __ACTION_H
#define __ACTION_H

#include "Model.h"

enum actType{Initial, Macro, Act, None}; // types of actions

class Action
{
  public:
    long actNum, actNumUser;
    actType type;
    static Model* model;

    explicit Action(long actNum);
    Action(actType type, long actNumUser);
    virtual ~Action() {}

    bool operator==(const Action& action) const;

    static int compare(const Action& a, const Action& b);

    void setActNum(long actNum);
    void setActNumUser(long actNumUser);

    /**
       Get the type of action given its index in the \a actEdges
    */
    actType getActType();

    long getActNumUser() const;

    void computeType();

    /**
       Given the action type \a aType and action number \a aNum, return
       the position in \a actEdges.
    */
    static long getBeliefAct(Model& model, actType aType, long aNum);
    static void initStatic(Model* model);
};

#endif
