#include "../include/ejcommon.h"
#include "ejtextcontrol.h"
#include "ejtableblocks.h"
#include "labelblock.h"
#include "intvarlen.h"
#include <QLocale>
#include <QIODevice>
#include <QGuiApplication>

/* a=target variable, b=bit number to act upon 0-n */
#define BIT_SET(a,b) ((a) |= (0x01<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(0x01<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (0x01<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (0x01<<(b))))




int writeSmallString(QDataStream &stream, const QString &source)
{
	QByteArray ba = source.toUtf8();
	return  writeBA(stream,ba);
}


void readSmallString(QDataStream &stream, QString &source)
{
	QByteArray ba;
	readBA(stream,ba);
	source = QString::fromUtf8(ba);
}

int writeBA(QDataStream &stream, const QByteArray &source)
{
	quint32 size = (quint32)source.size();
	IntVarLen t(size);
	stream << t;
	return stream.device()->write(source.data(),size);
}


void readBA(QDataStream &stream, QByteArray &source)
{
	quint32 size;
	IntVarLen t(size);
	QByteArray ba;
	stream >> t;
	source = stream.device()->read(size);
}


double getDValue(QString s_value, bool *bOk)
{
    QLocale locale;
    s_value.remove(locale.groupSeparator());
    double res = s_value.toDouble(bOk);
    if(!*bOk)
        res = locale.toDouble(s_value, bOk);
    return res;
}


QString getDText(double value, int accuracy)
{
    QLocale locale;
    QString res = locale.toString(value,'f',accuracy);
    if(accuracy == 0)
        return res;
    int n = res.count() - 1;
    int i = 0;
    for(i = 0; i< accuracy; i++)
    {
        if(res[n - i] == '0')
            continue;
        else
        {
            break;
        }
    }
    if(i == accuracy)
        i++;
    res.resize(n - i + 1);
    return res;
}

Page::Page()
{
    startBlock = endBlock = 0;
    height = 29700; width = 21000; x = y = 0;
    topMarging = bottomMarging = 1500;
    leftMarging = 2000;
    rightMarging = 1500;
    flag_redraw = true;
}

EjBlock *EjPointBlock::makeCopy()
{
	EjPointBlock *res = new EjPointBlock();
    copyData(res);
    res->type = type;
    res->x = x;
    res->y = y;
    return res;
}

bool EjPointBlock::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
    if(this->x != other.x || this->y != other.y)
        res = false;
    return res;
}

EjBlock *EjPropKeyBlock::makeCopy()
{
	EjPropKeyBlock *res = new EjPropKeyBlock();
    copyData(res);
    res->type = type;
    res->key = key;
    return res;
}

bool EjPropKeyBlock::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
	if(this->key != ((EjPropKeyBlock*)(&other))->key)
        res = false;
    return res;
}


EjBlock *EjGroupBlock::findProp(QList<EjBlock*> *lBlocks, int type, int num, bool check)
{
	EjBlock *block = nullptr;
    int index = findPropIndex(lBlocks, type, num, m_index + m_counts, check);
    if(index > -1)
        block = lBlocks->at(index);
    return  block;
}


int EjGroupBlock::findPropIndex(QList<EjBlock *> *lBlocks, int type, int num, int endIndex, bool check)
{
    return findPropIndex(lBlocks, type, num, m_index+1, endIndex, check);
}


int EjGroupBlock::findPropIndex(QList<EjBlock *> *lBlocks, int type, int num, int startIndex, int endIndex, bool check)
{
    int index;
    int res = -1;
	EjBlock *curBlock;
	EjPropBase *curPropBlock;
	EjPropDoc *curPropDoc;
    for(index = startIndex; index < endIndex; index++)
    {
        if(index > lBlocks->count() - 1)
            break;
        curBlock = lBlocks->at(index);
        if(curBlock->type == static_cast<quint8>(type))
        {
            if(curBlock->type == PROP_BIG_TEXT)
            {
				EjPropBigTextBlock *curBigText = dynamic_cast<EjPropBigTextBlock*>(curBlock);
                if(curBigText && curBigText->num == static_cast<quint8>(num))
                    res = index;
                break;

            }
            else if(curBlock->type == ACCESS_RULES)
            {
				EjAccessBlock *curAccesBlock = dynamic_cast<EjAccessBlock*>(curBlock);
                if(curAccesBlock)
                {
                    res = index;
                    break;
                }
            }
            else
            {
				curPropBlock = dynamic_cast<EjPropBase*>(curBlock);
                if(curPropBlock)
                {
                    if(curPropBlock->num == static_cast<quint8>(num))
                    {
                        if(!check) {
                            res = index;
                            break;
                        }
                        else {
                            if(res < 0)
                            {
                                res = index;
                            }
                            else
                            {
                                remBlock(lBlocks,curPropBlock);
                            }
                        }
                    }
                }
                else
                {
					curPropDoc = dynamic_cast<EjPropDoc*>(curBlock);
                    if(curPropDoc->num == static_cast<quint8>(num))
                    {
                        if(!check) {
                            res = index;
                            break;
                        }
                        else {
                            if(res < 0)
                            {
                                res = index;
                            }
                            else
                            {
                                remBlock(lBlocks,curPropDoc);
                            }
                        }
                    }
                }
            }

        }
        if(curBlock->type > GROUP_BLOCK)
            index += (dynamic_cast<EjGroupBlock*>(curBlock))->m_counts;
    }
    return res;
}


void EjGroupBlock::addProp(QList<EjBlock*> *lBlocks, EjBlock *block)
{
    lBlocks->insert(m_index + m_counts,block);
    block->parent = this;
    m_counts++;
}


void EjGroupBlock::remBlock(QList<EjBlock *> *lBlocks, EjBlock *block)
{
    if(!block)
        return;
    int index = lBlocks->indexOf(this);
    if(index < 0)
        return;
    m_index = index;
    calcLenght(m_index,lBlocks);
    index = lBlocks->indexOf(block);
    if(index < m_index + 1 && index > m_index + m_counts - 1)
        return;
    if(block->type >= GROUP_BLOCK)
    {
        EjGroupBlock *groupBlock = dynamic_cast<EjGroupBlock*>(block);
        if(groupBlock)
        {
            groupBlock->remFromBlocks(lBlocks);
            delete groupBlock;
            calcLenght(m_index,lBlocks);
        }
    }
    else if(block->type == END_GROUP)
    {
        EjGroupBlock *groupBlock = dynamic_cast<EjGroupBlock*>(block->parent);
        if(groupBlock && groupBlock != this)
        {
            groupBlock->remFromBlocks(lBlocks);
            delete groupBlock;
            calcLenght(m_index,lBlocks);
        }

    }
    else
    {
        delete lBlocks->takeAt(index);
        m_counts--;
    }
}

void EjGroupBlock::createDefault(QList<EjBlock *> *lBlocks, int index)
{
    lBlocks->insert(index,this);
	EjBlock *cur_block;
    m_index = index = lBlocks->indexOf(this);
    index++;
	cur_block = new EjBlock(END_GROUP);
    lBlocks->insert(index,cur_block);
    m_counts = 1;
}

void EjGroupBlock::createDefaultWithNum(QList<EjBlock *> *lBlocks, int index, int num)
{
    m_index = index;
    lBlocks->insert(index,this);
    index++;
	lBlocks->insert(index,new EjPropIntBlock(0));
	((EjPropIntBlock*)lBlocks->at(index))->value = num;
    index++;
	lBlocks->insert(index,new EjBlock(END_GROUP,this));
    m_counts = 2;
}

void EjGroupBlock::remFromBlocks(QList<EjBlock *> *lBlocks)
{
	QList<EjBlock*>::iterator iter;
	EjBlock *curBlock;
    EjGroupBlock *groupBlock;
    if(lBlocks->at(m_index) != this)
    {
        int index = lBlocks->indexOf(this);
        if(index < 0)
            return;
        m_index = index;
    }
    while (lBlocks->at(m_index + 1)->type != END_GROUP)
    {
        curBlock = lBlocks->at(m_index + 1);
        if(curBlock != this && curBlock->type >= GROUP_BLOCK) {
            groupBlock = dynamic_cast<EjGroupBlock*>(curBlock);
            if(groupBlock)
                groupBlock->remFromBlocks(lBlocks);
            delete groupBlock;
        }
        else delete lBlocks->takeAt(m_index + 1);
    }
    delete lBlocks->takeAt(m_index + 1);
    lBlocks->takeAt(m_index);
}


void EjGroupBlock::calcBlock(int &index, EjCalcParams *calcParams)
{
	QList<EjBlock*>::iterator iter;
	EjBlock *cur_block;
	QList<EjBlock*> *lBlocks = calcParams->control->doc->lBlocks;

    if(calcParams->force || m_index == -1 || lBlocks->at(m_index) != this)
    {
        m_index = index;
        if(calcParams->force)
            m_counts = 0;
        if(m_counts == 0)
        {
            beforeCalc(calcParams);
            this->width = 0;
            iter = lBlocks->begin() + m_index + 1;
            cur_block = (*iter);
            while(iter != lBlocks->end() && cur_block->type != END_GROUP) {
                m_counts++;
                cur_block->parent = this;
                if(cur_block->type >= GROUP_BLOCK) {
                    int index2 = iter - lBlocks->begin();
                    ((EjGroupBlock*)cur_block)->calcBlock(index2, calcParams);
                    iter += ((EjGroupBlock*)cur_block)->m_counts;
                    m_counts += ((EjGroupBlock*)cur_block)->m_counts;
                    this->width += cur_block->width;
                }
                else if(!cur_block->isProperty())
                {
                    int index2 = iter - lBlocks->begin();
                    cur_block->calcBlock(index2, calcParams);
                    this->width += cur_block->width;
                }
                childCalc(cur_block, calcParams);
                iter++;
                cur_block = (*iter);
            }
            if(iter != lBlocks->end() && (*iter)->type == END_GROUP) {
                (*iter)->parent = this;
                m_counts++;
            }
            afterCalc(calcParams);
        }
    }
    index += m_counts;
}

void EjGroupBlock::calcLenght(int &index, QList<EjBlock *> *lBlocks)
{
	QList<EjBlock*>::iterator iter;
	EjBlock *cur_block;
    if(index < 0 || index > lBlocks->count() - 1 || lBlocks->at(index) != this)
    {
        index = lBlocks->indexOf(this);
    }
    m_index = index;
    m_counts = 0;
    iter = lBlocks->begin() + m_index + 1;
    cur_block = (*iter);
    while(iter != lBlocks->end() && cur_block->type != END_GROUP) {
        m_counts++;
        cur_block->parent = this;
        if(cur_block->type >= GROUP_BLOCK) {
            int index2 = iter - lBlocks->begin();
            ((EjGroupBlock*)cur_block)->calcLenght(index2, lBlocks);
            iter += ((EjGroupBlock*)cur_block)->m_counts;
            m_counts += ((EjGroupBlock*)cur_block)->m_counts;
        }
        iter++;
        if(iter != lBlocks->end())
            cur_block = (*iter);
    }
    if(iter != lBlocks->end() && (*iter)->type == END_GROUP) {
        (*iter)->parent = this;
        m_counts++;
    }

}


void EjGroupBlock::clear(QList<EjBlock *> *lBlocks)
{
    m_index = lBlocks->indexOf(this);
    if(m_index == -1)
        return;
    int i = m_index + 1;
    int count_groups = 0;
	EjBlock *curBlock = lBlocks->at(i);
    while(curBlock->type != END_GROUP)
    {
        if(curBlock->type > GROUP_BLOCK)
        {
            count_groups = 1;
            delete lBlocks->takeAt(i);
            m_counts--;
            while(count_groups > 0 && i < lBlocks->count())
            {
                curBlock = lBlocks->at(i);
                if(curBlock->type == END_GROUP)
                    count_groups--;
                else if(curBlock->type > GROUP_BLOCK)
                    count_groups++;
                delete lBlocks->takeAt(i);
                m_counts--;
            }
        }
        else // if(!curBlock->isProperty())
        {
            delete lBlocks->takeAt(i);
            m_counts--;
        }
       // else
         //   i++;
        curBlock = lBlocks->at(i);
    }

}


bool EjMapLabelBlock::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
    if(this->x != other.x || this->y != other.y)
        res = false;
    return res;
}


void EjMapLabelBlock::setCoords(qint32 x_souce, qint32 y_source)
{
    x = x_souce; y = y_source;
}


EjBlock *EjPropPntBlock::makeCopy()
{
	EjPropPntBlock *res = new EjPropPntBlock();
    copyData(res);
    res->type = type;
    res->num = num;
    res->x_value = x_value;
    res->y_value = y_value;
    return res;
}


bool EjPropPntBlock::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
		  EjPropPntBlock const *cur_propPntBlock;
		  cur_propPntBlock = (EjPropPntBlock *)&other;
    if(this->x_value != cur_propPntBlock->x_value || this->y_value != cur_propPntBlock->y_value || this->num != cur_propPntBlock->num)
        res = false;
    return res;
}


EjBlock *EjPropIntBlock::makeCopy()
{
	EjPropIntBlock *res = new EjPropIntBlock();
    copyData(res);
    res->num = num;
    res->value = value;
    return res;
}


bool EjPropIntBlock::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
		  EjPropIntBlock const *cur_propIntBlock;
		  cur_propIntBlock = (EjPropIntBlock *)&other;
    if(this->value != cur_propIntBlock->value || this->num != cur_propIntBlock->num)
        res = false;
    return res;
}


EjBlock *EjPropTextBlock::makeCopy()
{
	EjPropTextBlock *res = new EjPropTextBlock();
    copyData(res);
    res->num = num;
    res->text = text;
    return res;
}

bool EjPropTextBlock::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
		  EjPropTextBlock const *cur_propTxtBlock;
		  cur_propTxtBlock = (EjPropTextBlock *)&other;
    if(this->num != cur_propTxtBlock->num || this->text != cur_propTxtBlock->text)
        res = false;
    return res;
}


ItemBlock *EjVectorText::newItem(qreal viewScale, QQuickItem *parent)
{
    return NULL;
}


QQuickItem *EjVectorText::newViewItem(int vid, QQuickItem *parent)
{
    return NULL;
}


void EjTableFragment::setStartRow(quint16 row)
{
    quint32 tmp = row;
    tmp  = tmp << 16;
    tmp += (startBlock & 0xffff);
    startBlock = tmp;
}


void EjTableFragment::setStartColum(quint16 colum)
{
    quint32 tmp = (startBlock & 0xffff0000);
    tmp += colum;
    startBlock = tmp;

}

void EjTableFragment::setEndRow(quint16 row)
{
    quint32 tmp = row;
    tmp = tmp << 16;
    tmp += (endBlock & 0xffff);
    endBlock = tmp;
}

void EjTableFragment::setEndColum(quint16 colum)
{
    quint32 tmp = (endBlock & 0xffff0000);
    tmp += colum;
    endBlock = tmp;
}

EjTableFragment *EjTableFragment::makeCopy()
{
	EjTableFragment * res = new EjTableFragment();
    res->type = type;
    res->startBlock = startBlock;
    res->endBlock = endBlock;
    return res;
}

bool EjTableFragment::compare(const EjBlock &other) const
{
	if(!EjFragmentBlock::compare(other)) {
        return false;
    }
    bool res = true;
	EjTableFragment *curFragment = (EjTableFragment*)(&other);
    if(startBlock != curFragment->startBlock || endBlock != curFragment->endBlock)
        res = false;
    return res;
}


EjBlock *EjFragmentBlock::makeCopy()
{
	EjFragmentBlock *res = new EjFragmentBlock();
    copyData(res);
    res->type = type;
    res->vid = vid;
    res->countBlocks = countBlocks;
    res->iValue = iValue;
    res->sValue = sValue;
    return res;
}

bool EjFragmentBlock::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
	EjFragmentBlock *curFragment = (EjFragmentBlock*)(&other);
    if(vid != curFragment->vid)
        res = false;
    return res;
}


EjBlock *EjEndFragmentBlock::makeCopy()
{
	EjEndFragmentBlock *res = new EjEndFragmentBlock();
    copyData(res);
    res->type = type;
    res->vid = vid;
    return res;
}


bool EjEndFragmentBlock::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
	EjEndFragmentBlock *curFragment = (EjEndFragmentBlock*)(&other);
    if(vid != curFragment->vid)
        res = false;
    return res;
}


EjTextBlock::~EjTextBlock()
{
    text = "";
}

EjBlock *EjTextBlock::makeCopy()
{
	EjTextBlock *res = new EjTextBlock();
    copyData(res);
    res->type = type;
    res->text = text;
    return res;
}

bool EjTextBlock::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
	if(this->text != static_cast<const EjTextBlock*>(&other)->text)
        res = false;
    return res;
}


void EjTextBlock::calcBlock(int &index, EjCalcParams *calcParams)
{
    if(!calcParams || !calcParams->textStyle)
        return;
    this->ascent = calcParams->textStyle->m_fontMetrics.ascent() * 100 * 0.347;
    this->descent = calcParams->textStyle->m_fontMetrics.descent() * 100 * 0.347;
        {
            if(this->width == 0 || calcParams->force)
            {
				this->width = calcParams->textStyle->m_fontMetrics.horizontalAdvance(this->text) * 100 * 0.347; //0.236;
                if(calcParams->textStyle->m_font.italic())
                {
                    this->width += (calcParams->textStyle->m_fontMetrics.height() * 0.1 * 100 * 0.347);
                }
                this->flag_redraw = true;

            }
            //                    if(x > leftColontitul * k_scale / scaleSize && x + cur_txtBlock->width > (width - rightColontitul) * k_scale / scaleSize && back_type != TEXT)

        }


}



EjBlock *EjSizeProp::makeCopy()
{
    EjSizeProp *res = new EjSizeProp();
    copyData(res);
    res->num = num;
    res->min = min;
    res->max = max;
    res->current = current;
    res->isMax = isMax;
    return res;
}

bool EjSizeProp::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other))
        return false;
    const EjSizeProp *cur = static_cast<const EjSizeProp*>(&other);
    if(!cur || this->num != cur->num ||  this->min != cur->min
            || this->max != cur->max )
    {
        return false;
    }
    return true;
}


EjBlock *EjNumStyleBlock::makeCopy()
{
    EjNumStyleBlock *res = new EjNumStyleBlock();
    copyData(res);
    res->type = type;
    res->x = x;
    res->y = y;
    res->num = num;
    res->style = style;
    return res;
}

bool EjNumStyleBlock::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
    EjNumStyleBlock *cur_numStyleBlock = (EjNumStyleBlock *)&other;
    if(this->num != cur_numStyleBlock->num)
        res = false;
    return res;
}


void EjNumStyleBlock::calcBlock(int &index, EjCalcParams *calcParams)
{
	Q_UNUSED(index)

	if(!this->style) {
		this->style = calcParams->control->doc->getUndefinedStyle(this->num);
		if(!this->style)
			return;
	}
	if(this->style->m_vid == TEXT_STYLE)
	{
		calcParams->textStyle = (EjTextStyle*)(this->style);
		calcParams->textStyle->m_fontMetrics = QFontMetrics(calcParams->textStyle->m_font);
		calcParams->interval = calcParams->textStyle->m_fontMetrics.height() * 100 * 0.347 * 0.5;

	}
	if(this->style->m_vid == PARAGRAPH_STYLE)
	{
		calcParams->paragraphStyle = (EjParagraphStyle*)(this->style);
	}
}


EjBlock *EjPropInt8Block::makeCopy()
{
	EjPropInt8Block *res = new EjPropInt8Block();
    copyData(res);
    res->num = num;
    res->value = value;
    return res;
}

bool EjPropInt8Block::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
		  EjPropInt8Block const *cur_propIntBlock;
		  cur_propIntBlock = (EjPropInt8Block *)&other;
    if(this->value != cur_propIntBlock->value || this->num != cur_propIntBlock->num)
        res = false;
    return res;
}


EjBlock *EjPropInt64Block::makeCopy()
{
	EjPropInt64Block *res = new EjPropInt64Block();
    copyData(res);
    res->num = num;
    res->value = value;
    return res;
}

bool EjPropInt64Block::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
		  EjPropInt64Block const *cur_propIntBlock;
		  cur_propIntBlock = (EjPropInt64Block *)&other;
    if(this->value != cur_propIntBlock->value || this->num != cur_propIntBlock->num)
        res = false;
    return res;
}


EjBlock *EjPropByteArrayBlock::makeCopy()
{
	EjPropByteArrayBlock *res = new EjPropByteArrayBlock();
    copyData(res);
    res->num = num;
    res->data = data;
    return res;
}

bool EjPropByteArrayBlock::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
		  EjPropByteArrayBlock const *cur_propBABlock;
		  cur_propBABlock = (EjPropByteArrayBlock *)&other;
    if(this->num != cur_propBABlock->num || this->data != cur_propBABlock->data)
        res = false;
    return res;
}


EjCalcParams::EjCalcParams()
{
    textStyle = NULL; /*brushStyle = NULL;*/ lineStyle = NULL; paragraphStyle = NULL;
    control = NULL;
    viewScale = 0;
    index_string = 0;
}

EjBlock *EjSpaceBlock::makeCopy()
{
	EjSpaceBlock *res = new EjSpaceBlock();
    copyData(res);
    return res;
}


void EjSpaceBlock::calcBlock(int &index, EjCalcParams *calcParams)
{
    if(!calcParams || !calcParams->textStyle)
        return;
    this->ascent = calcParams->textStyle->m_fontMetrics.ascent() * 100 * 0.347;
    this->descent = calcParams->textStyle->m_fontMetrics.descent() * 100 * 0.347;
//    this->height += calcParams->interval;
    this->width = calcParams->textStyle->m_fontMetrics.descent() * 100 * 0.347;
}



EjBlock *EjBlock::rootBlock()
{
	EjBlock *res = this;
    while(res->parent)
    {
        if(res->parent->isGlassy())
        {
			EjBlock *next = res->parent;
            while (next && next->isGlassy())
            {
                next = next->parent;
            }
            if(next && next->parent)
                res = next;
            else
                break;
        }
        else
            res = res->parent;

    }
    return res;
}

bool EjBlock::isProperty()
{
    bool res = type >= NUM_STYLE && type <= PROP_BIG_TEXT;
    return res;
}


EjBlock *EjPropDoc::makeCopy() {
	EjPropDoc *res = new EjPropDoc();
    copyData(res);
    res->m_vid = m_vid;
    res->m_counts = m_counts;
    res->num = num;
    return res;
}

bool EjPropDoc::compare(const EjBlock &other) const{
	return EjBlock::compare(other) && ((EjPropDoc*)&other)->m_vid == m_vid; // && ((PropDoc*)&other)->num == num;
}


EjBlock *EjPropColorBlock::makeCopy()
{
	EjPropColorBlock *res = new EjPropColorBlock();
    copyData(res);
    res->num = num;
    res->color = color;
    return res;
}

bool EjPropColorBlock::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
		  EjPropColorBlock const *cur_propColorBlock;
		  cur_propColorBlock = (EjPropColorBlock *)&other;
    if(this->color != cur_propColorBlock->color || this->num != cur_propColorBlock->num)
        res = false;
    return res;
}


EjBlock *EjPropBigTextBlock::makeCopy()
{
	EjPropBigTextBlock *res = new EjPropBigTextBlock();
    EjGroupBlock::copyData((EjGroupBlock*)res);
    res->num = num;
    return (EjGroupBlock*)res;
}

bool EjPropBigTextBlock::compare(const EjBlock &other) const
{
    if(!EjGroupBlock::compare(other)) {
        return false;
    }
    bool res = true;
	EjPropBigTextBlock const *cur_propBigTextBlock;
	cur_propBigTextBlock = (EjPropBigTextBlock *)(EjPropBase*)(&other);
    if(this->num != cur_propBigTextBlock->num)
        res = false;
    return res;
}


void EjPropBigTextBlock::setText(QString source, QList<EjBlock *> *lBlocks)
{
    if(m_index < 0)
        return;
    clear(lBlocks);
    int i  = source.indexOf(' ');
    QString left, right;
    while(i > 0)
    {
        left = source.left(i);
        source = source.right(source.size() - i - 1);
        i = source.indexOf(' ');
		addProp(lBlocks,new EjTextBlock(left));
		addProp(lBlocks, new EjSpaceBlock());
    }
    if(source == " ")
    {
		addProp(lBlocks, new EjSpaceBlock());
    }
    else
    {
		addProp(lBlocks,new EjTextBlock(source));
    }
}

QString EjPropBigTextBlock::text(QList<EjBlock *> *lBlocks)
{
    int index = m_index + 1;
    QString res;
    while(index < m_index + m_counts)
    {
        if(lBlocks->at(index)->type == TEXT)
        {
			res += ((EjTextBlock*)lBlocks->at(index))->text;
        }
        else if(lBlocks->at(index)->type == SPACE)
        {
            res += ' ';
        }
        index++;
    }
    return res;
}

EjBlock *EjAccessBlock::makeCopy()
{
	EjAccessBlock *res = new EjAccessBlock();
    copyData(res);
    res->value = value;
    return res;
}

bool EjAccessBlock::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
	if(this->value != static_cast<const EjAccessBlock*>(&other)->value)
        res = false;
    return res;
}

void EjAccessBlock::setIsEditAsParent(bool source)
{
    if(source)
        BIT_SET(value,0);
    else
        BIT_CLEAR(value,0);
}

void EjAccessBlock::setIsSecAsParent(bool source)
{
    if(source)
        BIT_SET(value,5);
    else
        BIT_CLEAR(value,5);
}

void EjAccessBlock::setEditLevel(int source)
{
    for(int i = 0; i < 2; i++)
    {
        if(BIT_CHECK(source, i))
            BIT_SET(value,1 + i);
        else
            BIT_CLEAR(value,1 + i);
    }
}

void EjAccessBlock::setSecLevel(int source)
{
    for(int i = 0; i < 2; i++)
    {
        if(BIT_CHECK(source, i))
            BIT_SET(value,6 + i);
        else
            BIT_CLEAR(value,6 + i);
    }
}

void initCommonResources()
{
}

EjBlock *EjPropAccessBlock::makeCopy()
{
	EjPropAccessBlock *res = new EjPropAccessBlock();
    copyData(res);
    res->num = num;
    res->value = value;
    return res;
}

bool EjPropAccessBlock::compare(const EjBlock &other) const
{
	if(!EjBlock::compare(other)) {
        return false;
    }
    bool res = true;
		  EjPropAccessBlock const *cur_propAccessBlock;
		  cur_propAccessBlock = (EjPropAccessBlock *)&other;
    if(this->num != cur_propAccessBlock->num || this->value != cur_propAccessBlock->value)
        res = false;
    return res;
}

void EjPropAccessBlock::setIsEditAsParent(bool source)
{
    if(source)
        BIT_SET(value,0);
    else
        BIT_CLEAR(value,0);
}

void EjPropAccessBlock::setIsSecAsParent(bool source)
{
    if(source)
        BIT_SET(value,5);
    else
        BIT_CLEAR(value,5);
}

void EjPropAccessBlock::setEditLevel(int source)
{
    for(int i = 0; i < 2; i++)
    {
        if(BIT_CHECK(source, i))
            BIT_SET(value,1 + i);
        else
            BIT_CLEAR(value,1 + i);
    }
}

void EjPropAccessBlock::setSecLevel(int source)
{
    for(int i = 0; i < 2; i++)
    {
        if(BIT_CHECK(source, i))
            BIT_SET(value,6 + i);
        else
            BIT_CLEAR(value,6 + i);
    }
}
