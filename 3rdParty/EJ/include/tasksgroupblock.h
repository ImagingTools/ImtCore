#ifndef TASKSGROUPBLOCKS_H
#define TASKSGROUPBLOCKS_H
#include "tableblocks.h"

class TasksGroupDelegate;

class TasksGroupBlock : public GroupBlock
{
public:
    TasksGroupBlock();
    ~TasksGroupBlock();
    TasksGroupBlock(Document *doc, int index);
    virtual bool isGlassy() override { return true; }

//    virtual void createDefault(QList<Block *> *lBlocks, int index) override;
protected:
    TableBlock *m_table;
    virtual void childCalc(Block *child, CalcParams *calcParams) override;
    TasksGroupDelegate *tgDelegate;
};

#endif // TASKSGROUPBLOCKS_H
