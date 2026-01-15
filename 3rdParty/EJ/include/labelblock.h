#ifndef DOCUMENTPLUG_H
#define DOCUMENTPLUG_H
//#include "imageplug_global.h"
#include <QObject>
//#include "ej_interfaces.h"
#include "calculatorhelper.h"
#include "ejcommon.h"

class LabelBlock : public EjGroupBlock
{
public:
    enum NumProp {
        LABEL_KEY = 0,
        LABEL_NAME,
        LABEL_PROP,
        LABEL_VALUE,
    };


    LabelBlock(quint32 key_source);
    virtual ~LabelBlock(){}


	void createDefault(QList<EjBlock*> *lBlocks, int index) override;
//    virtual void beforeCalc(CalcParams *calcParams) override;
	virtual void childCalc(EjBlock *child, EjCalcParams *calcParams) override;
	virtual void afterCalc(EjCalcParams *calcParams) override;
	void calcBlock(int &index, EjCalcParams *calcParams) override;

//    QQuickItem *getItem(int &index, CalcParams *calcParams, QQuickItem *parentItem) override;
    virtual bool isGlassy() override { return true; }
	QString getTextData(QList<EjBlock *> *lBlocks) override;
//protected:
    int key;
    QString name;
    QString additionalProp;
    QString valueTxt;
    LinkType vid;
    quint8 accuracy;
    double value;
};


#endif // DOCUMENTPLUG_H
