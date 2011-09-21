#include "Action.h"
#include <cstdio>
using namespace std;

Model* Action::model;

void Action::initStatic(Model* model)
{
    Action::model = model;
}

Action::Action(int actNum, actType type): actNum(actNum), type(type)
{
    if (type == None)
        this->computeType();
}

Action::Action()
{
    type = None;
}

bool Action::operator==(const Action& action) const
{
    if (action.actNum != actNum) return false;
    return true;
}

int Action::compare(const Action& a, const Action& b)
{
    if (a.type < b.type) return 1;
    else if (a.type > b.type) return -1;

    if (a.actNum < b.actNum) return 1;
    else if (a.actNum > b.actNum) return -1;

    return 0;
}

void Action::operator()(int actNum, actType type)
{
    this->actNum = actNum;
    this->type   = type;

    if (type == None)
        this->computeType();
}

actType Action::getActType()
{
    if (type != None)
        return type;

    computeType();
    return type;
}

void Action::computeType()
{
    if (model == NULL) {
        cerr<<"Action::model has not been initialized\n";
        exit(1);
    }

    if (actNum - model->getNumInitPolicies() - model->getNumMacroActs() >=0)
        type = Act; // simple action
    else if (actNum - model->getNumInitPolicies() >= 0)
        type = Macro; // macro action
    else type = Initial; // initial action
}

long Action::getBeliefAct(Model& model, actType aType, long aNum)
{
    if (aType == Act)
        return aNum + model.getNumInitPolicies() + model.getNumMacroActs();
    else if (aType == Macro)
        return aNum + model.getNumInitPolicies();
    else return aNum;
}
