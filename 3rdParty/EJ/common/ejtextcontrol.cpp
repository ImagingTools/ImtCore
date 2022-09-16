#include "ejtextcontrol.h"
#include "ejcalculator.h"
#include "ejcommon.h"
#include "ejtableblocks.h"
#include "labelblock.h"

#include <QDebug>
#include <QMimeData>
#include <QClipboard>
#include <QGuiApplication>
#include <QStaticText>
#include <QGlyphRun>
#include <QTextLayout>
#include <QTextLine>


EjTextControl::EjTextControl(QObject *parent) :
    QObject(parent),
    anchor(0),
    position(0),
    leftColontitul(0),
    rightColontitul(0),
    topColontitul(0),
    bottomColontitul(0),
    m_statusMode(READ_ONLY),
    m_selectMode(NO_SELECTED),
    doc(&m_doc),
    docPrev(nullptr),
    metric(QFontMetrics(getTextStyle(0)->m_font)),
    m_viewMode(RICH_TEXT)
{
    SurroundingText = "";
    CurrentSelection = "";
    position =  anchor = 0;
    activeIndex = id_inputSelBlock = -1;
    m_width = 100;
    m_interval = 10;
    m_painter = NULL;
    m_deltapos = 0;
    m_inputSelectMode = false;
    m_heightCursor = 0;
    m_contentX = m_contentY = 0;
    m_posCursorX = leftColontitul;
    m_posCursorY = metric.height() * 100;// + m_interval;
    m_height = 0;//metric.height() * 110;
    m_isViewDoc = false;
    m_startCursor = true;
    m_showCell = false;
    m_inputSelBlock = NULL;
    is_startInputMode = false;
    m_defaultPageWidth = 21000;
    m_defaultPageHeight = 29700;

    setDocument(doc);
    calcCursor();
    docPrev = nullptr;
    m_currentPatch = -1;
    m_createPatchEnabled = false;
    m_clipboardDoc = nullptr;
}

EjTextControl::~EjTextControl()
{
    //   clear();
}



void EjTextControl::calcInputMethodParams()
{
    int start = 0, end = 0;

    QString str;
    SurroundingText = TextAfterCursor = TextBeforeCursor = "";
    if(doc->lBlocks->count() == 0 || activeIndex < 0 || activeIndex > doc->lBlocks->count() - 1)
        return;
    if(activeIndex >= 0)
        start = end = activeIndex;
    if(!isTable(activeIndex))
    {
        start++;
        while(start > 0 && doc->lBlocks->at(start-1)->type != ENTER && doc->lBlocks->at(start-1)->type != BASECELL
              && doc->lBlocks->at(start-1)->type != EXT_TABLE)
            start--;
        if(start > end)
            start = end;
        while(end < doc->lBlocks->count()-1 && doc->lBlocks->at(end+1)->type != ENTER && doc->lBlocks->at(end+1)->type != BASECELL
              && doc->lBlocks->at(end+1)->type != EXT_TABLE)
            end++;
    }
    else
    {
        while(start > 0 && doc->lBlocks->at(start-1)->type != ENTER && doc->lBlocks->at(start)->type != BASECELL
              && doc->lBlocks->at(start-1)->type != EXT_TABLE)
            start--;
        if(start > end)
            start = end;
        while(end < doc->lBlocks->count()-1 && doc->lBlocks->at(end+1)->type != ENTER && doc->lBlocks->at(end+1)->type != BASECELL
              && doc->lBlocks->at(end+1)->type != EXT_TABLE)
            end++;
    }



    while(start <= end)
    {
        switch (doc->lBlocks->at(start)->type) {
        case TEXT: case BASECELL:
			str = static_cast<EjTextBlock*>(doc->lBlocks->at(start))->text;
            if(start < activeIndex)
                TextBeforeCursor += str;
            if(start > activeIndex)
                TextAfterCursor += str;
            break;
        case SPACE: // case IMAGE_D:
            str = " ";
            if(start <= activeIndex)
                TextBeforeCursor += str;
            if(start > activeIndex)
                TextAfterCursor += str;
            break;
        default:
            break;
        }
        start++;
    }
    {
        if(activeIndex >= 0 && (doc->lBlocks->at(activeIndex)->type == TEXT || doc->lBlocks->at(activeIndex)->type == BASECELL))
        {
			str = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex))->text;

            TextBeforeCursor += str.left(position);
            TextAfterCursor = str.right(str.count() - position) + TextAfterCursor;
        }
    }
    SurroundingText = TextBeforeCursor + TextAfterCursor;
}

int EjTextControl::delText(int delta, int count)
{
    if(activeIndex < 0)
        return 0;
    int start = count + delta;
    int row = -1, colum = -1, row_new = -1, colum_new = -1;
    int activeBlock_back = activeIndex;
    int position_back;
    bool is_rem = false;
    while(start > 0)
    {
        position_back = position;
        cursorRight();
        if(activeBlock_back != activeIndex
                //                && (doc->lBlocks->at(activeBlock_back)->type == TEXT || doc->lBlocks->at(activeBlock_back)->type == BASECELL ))
                && isTable(activeBlock_back) && doc->lBlocks->at(activeIndex)->type == BASECELL)
        {
            activeIndex = activeBlock_back;
            position = position_back;
            count--;
            return 0;
        }
        else
        {
            activeBlock_back = activeIndex;
            position_back = position;
        }
        start--;
    }
    for(int i = 0; i < count; i++)
    {
        inputBackSpace();
        is_rem = true;
    }
    if(is_rem)
        calc(0);
    calcInputMethodParams();
    return 1;
}

QVariant EjTextControl::inputMethodQuery(Qt::InputMethodQuery property, QVariant argument) const
{
    Q_UNUSED(argument)
    int n;
    QString str;
    QRect rect(0,0,m_width,m_height);
    QString blockTextAfterCursor, blockTextBeforeCursor;
    int maxLength = 1024;

    int start = 0, end = 0, startBlock = 0, endBlock = 0;
    int length;
    EjBlock *block;

    QString SurroundingText, TextAfterCursor, TextBeforeCursor;
    if(property == Qt::ImTextBeforeCursor || property == Qt::ImTextAfterCursor)
    {
        maxLength = argument.isValid() ? argument.toInt() : 1024;
    }

    if(doc->lBlocks->count() > 0 && activeIndex > -1 && activeIndex < doc->lBlocks->count())
    {
        start = end = startBlock = endBlock = activeIndex;
        start--;
        end++;
        length = 0;
        while(start > -1)
        {
            block = doc->lBlocks->at(start);
            str = "";
            if(block->type == TEXT)
            {
				str = (dynamic_cast<EjTextBlock*>(block))->text;
            }
            else if(block->type == SPACE)
            {
                str = " ";
            }
            if(doc->lBlocks->at(start)->type == ENTER ||
                    doc->lBlocks->at(start)->type == BASECELL )
            {
                blockTextBeforeCursor = TextBeforeCursor;
                if(length > maxLength)
                    break;
                str = "\n";
            }
            length += str.length();
            TextBeforeCursor = str + TextBeforeCursor;
            start--;
        }
        length = 0;
        while(end < doc->lBlocks->count())
        {
            block = doc->lBlocks->at(end);
            str = "";
            if(block->type == TEXT)
            {
				str = (dynamic_cast<EjTextBlock*>(block))->text;
            }
            else if(block->type == SPACE)
            {
                str = " ";
            }
            if(block->type == ENTER || block->type == BASECELL )
            {
                blockTextAfterCursor = TextAfterCursor;
                if(length > maxLength)
                    break;
                str = "\n";
            }
            length += str.length();
            TextAfterCursor = TextAfterCursor + str;
            end++;
        }
        if(blockTextBeforeCursor == "")
            blockTextBeforeCursor = TextBeforeCursor;
        if(blockTextAfterCursor == "")
            blockTextAfterCursor = TextAfterCursor;
        if(doc->lBlocks->at(activeIndex)->type == TEXT)
        {
			str = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex))->text;

            TextBeforeCursor += str.left(position);
            TextAfterCursor = str.right(str.count() - position) + TextAfterCursor;
            blockTextBeforeCursor += str.left(position);
            blockTextAfterCursor = str.right(str.count() - position) + blockTextAfterCursor;
        }
        else if(doc->lBlocks->at(activeIndex)->type == SPACE)
        {
           TextBeforeCursor += " ";
           blockTextBeforeCursor += " ";
        }
        else if(doc->lBlocks->at(activeIndex)->type == ENTER)
        {
           TextBeforeCursor += "\n";
           blockTextBeforeCursor = "";
        }

    }
    switch (property) {
    case Qt::ImCursorRectangle:
        return QRectF(0,0,0,0);
    case Qt::ImFont:
        // QFont;
        return getTextStyle(0)->m_font;
    case Qt::ImCursorPosition:
        // int
        qWarning() << "ImCursorPosition: " << TextBeforeCursor.count();
        return blockTextBeforeCursor.count();
//        return 0;
    case Qt::ImAnchorPosition:
        // int
        qWarning() << "ImCursorPosition: " << TextBeforeCursor.count();
        return blockTextBeforeCursor.count();
    case Qt::ImSurroundingText:
        return QVariant(blockTextBeforeCursor + blockTextAfterCursor);

    case Qt::ImTextBeforeCursor:
        return QVariant(TextBeforeCursor);

    case Qt::ImTextAfterCursor:
        return QVariant(TextAfterCursor);

    case Qt::ImCurrentSelection:
        return QVariant();

    case Qt::ImMaximumTextLength:
        return QVariant(); // No limit.

    case Qt::ImAbsolutePosition:
        return TextBeforeCursor.count();

    default:
        return QVariant();
    }
}

void EjTextControl::inputText(QString text)
{
    if(text.isEmpty())
        return;
	EjTextBlock *cur_txtBlock;

    if(activeIndex > doc->lBlocks->count() -1)
    {
		doc->lBlocks->insert(doc->lBlocks->count(), new EjTextBlock());
        activeIndex = doc->lBlocks->count() -1;
        position = 0;
    }
    if(activeIndex > -1 && doc->lBlocks->at(activeIndex)->type == EXT_TABLE)
        return;
    if(activeIndex > -1 && doc->lBlocks->at(activeIndex)->type > GROUP_BLOCK)
    {
        activeIndex += ((EjGroupBlock *)doc->lBlocks->at(activeIndex))->m_counts;
        inputText(text);
        return;
    }
    if(m_createPatchEnabled)
    {
        killTimer(m_timerId);
        m_timerId = startTimer(1000);
    }
	EjTableBlock *table = isTable(activeIndex);

	if(!m_startCursor && activeIndex > -1 && doc->lBlocks->at(activeIndex)->type == ENTER)
    {
        {
            activeIndex++;
			doc->lBlocks->insert(activeIndex, new EjTextBlock());
            if(table)
                table->m_counts++;
            position = 0;
        }
    }
    else if(activeIndex == -1 || (doc->lBlocks->at(activeIndex)->type != TEXT))
    {
        if(activeIndex == -1 || (!m_startCursor && doc->lBlocks->at(activeIndex)->type != ENTER
                                 && doc->lBlocks->at(activeIndex)->type != NUM_STYLE) )
            activeIndex++;
		doc->lBlocks->insert(activeIndex, new EjTextBlock());
        if(table)
            table->m_counts++;
        position = 0;
    }
    m_startCursor = false;
	cur_txtBlock = (EjTextBlock*)doc->lBlocks->at(activeIndex);
    if(m_statusMode == EDIT_CELL || ((m_statusMode == EDIT_TEXT) && cur_txtBlock->type == TEXT))
    {
        if(m_inputSelectMode == false)
        {
            cur_txtBlock->text.insert(position, text);
            cur_txtBlock->flag_redraw = true;
        }
        else
        {
            cur_txtBlock->text = text;
            cur_txtBlock->flag_redraw = true;
            position = 0;
        }
        position += text.size();

        QFontMetrics drawMetric = getDrawMetrics(activeIndex);

        int d = 0;
		d = drawMetric.horizontalAdvance(cur_txtBlock->text) * 100 * 0.347;
        if(doc->lBlocks->at(activeIndex)->type == BASECELL)
        {
            if(cur_txtBlock->width < d)
            {
                cur_txtBlock->width = d;
                calcData();
            }
        }
        else
            cur_txtBlock->width = d;
    }
    else if (cur_txtBlock->type == BASECELL && m_statusMode == EDIT_TEXT)
    {
		EjTableBlock *table = isTable(activeIndex);
        if(table)
        {
            int center = (table->startCell() + table->m_index + table->m_counts) / 2;
            if(activeIndex < center)
            {
                activeIndex = table->m_index - 1;
            }
            else
            {
                activeIndex = table->m_index + table->m_counts;
                if(activeIndex > doc->lBlocks->count() - 1)
                {
                    doc->lBlocks->insert(activeIndex,new EjBlock(ENTER));
                    updateFragments(activeIndex, true);
                    activeIndex++;
                    position = 0;
                }
            }
            inputText(text);
        }

    }
}

void EjTextControl::addImage(QString path)
{
    QByteArray full_key;
}

void EjTextControl::addExtBlock(EjBlock *block)
{
    splitText(activeIndex,position);
    doc->lBlocks->insert(activeIndex,block);
    updateFragments(activeIndex, true);
}

void EjTextControl::addShopList()
{

}

void EjTextControl::addClearTable()
{

}


bool EjTextControl::splitText(int &block,int &pos)
{
    QString str1;
    QString str2;
	EjTextBlock *curTextBlock;
    QString text;
    bool res = false;
    if(block < 0 || block > doc->lBlocks->count() - 1) return false;
    if(doc->lBlocks->at(block)->type > GROUP_BLOCK)
    {
        block += ((EjGroupBlock *)doc->lBlocks->at(block))->m_counts;
        return splitText(block,pos);
    }
    if(doc->lBlocks->at(block)->type == TEXT || doc->lBlocks->at(block)->type == BASECELL)
    {
        if(pos > 0)
        {
            QFontMetrics drawMetric = getDrawMetrics(block);
			curTextBlock = static_cast<EjTextBlock*>(doc->lBlocks->at(block));
            text = curTextBlock->text;
            if(pos < text.size())
            {
                str1 = text.left(pos);
                str2 = text.right(text.size() - pos);
                curTextBlock->text = str1;
                curTextBlock->flag_redraw = true;
                curTextBlock->ascent = metric.ascent();
                curTextBlock->width = 0;
                block++;
				doc->lBlocks->insert(block,new EjTextBlock());
				curTextBlock = static_cast<EjTextBlock*>(doc->lBlocks->at(block));
                curTextBlock->text = str2;
                curTextBlock->ascent = drawMetric.ascent();
                curTextBlock->width = 0;
                pos = 0;
                res = true;

				EjTableBlock *table = isTable(pos);
                if(table)
                {
                    table->m_counts++;
                }
            }
            else
                block++;
        }

    }
    else
    {
        if(!doc->lBlocks->at(block)->isProperty())
            block++;
    }
    return res;

}

quint32 EjTextControl::inputEnter(bool force)
{
    quint32 res = 0;
    EjBlock *curBlock;
	EjTableBlock *table = isTable(activeIndex);
	if(activeIndex < 0 || m_startCursor)
	{
		doc->lBlocks->insert(0,new EjBlock(ENTER));
		activeIndex = 0;
		m_startCursor = false;
		position = 0;
	}
	else if(table)
    {
        if(force)
        {
            splitText(activeIndex,position);
            doc->lBlocks->insert(activeIndex,new EjBlock(ENTER));
            table->m_counts++;
        }
        else
        {
            int center = (table->startCell() + table->endBlock()) / 2;
            EjBlock *block = table;
            while(block->parent)
            {
                block = block->parent;
            }

            if(activeIndex < center)
            {
                activeIndex = doc->lBlocks->indexOf(block) - 1;
                if(activeIndex < 0)
                    activeIndex = 0;
            }
            else
            {
                if(block->type >= GROUP_BLOCK)
                    activeIndex = ((EjGroupBlock*)block)->m_index + ((EjGroupBlock*)block)->m_counts + 1;
            }
            if(activeIndex <= doc->lBlocks->count() )
            {
                doc->lBlocks->insert(activeIndex,new EjBlock(ENTER));
                position = 0;
            }
            else activeIndex--;
        }
    }
	else // if(m_statusMode != EDIT_CELL && activeIndex > -1)
	{

        m_startCursor = false;
        int index = activeIndex;
        if(index > doc->lBlocks->count() - 1)
            index = doc->lBlocks->count() - 1;
        while(index > -1 && doc->lBlocks->at(index)->isProperty())
            index--;
        if(index > -1 && index < doc->lBlocks->count()) {
            curBlock = doc->lBlocks->at(index);
            res = curBlock->ascent + curBlock->descent;
        }
        splitText(activeIndex,position);
        doc->lBlocks->insert(activeIndex,new EjBlock(ENTER));
        curBlock = doc->lBlocks->at(activeIndex);

        curBlock->ascent = metric.ascent();
        m_inputSelectMode = false;
    }
    return res;
}

void EjTextControl::inputSpace()
{
	EjTextBlock *curTextBlock;
    int back_activeBlock = activeIndex;
    {
        if(m_createPatchEnabled)
        {
            killTimer(m_timerId);
            m_timerId = startTimer(1000);
        }

        m_startCursor = false;
		EjTableBlock *table = isTable(activeIndex);
        if(splitText(activeIndex,position))
        {
        }
        if(back_activeBlock == activeIndex && activeIndex > -1 && activeIndex < doc->lBlocks->count()
                && doc->lBlocks->at(activeIndex)->type == BASECELL)
        {
			curTextBlock = new EjTextBlock();
            activeIndex++;
            doc->lBlocks->insert(activeIndex,curTextBlock);
            if(table)
                table->m_counts++;
        }
		doc->lBlocks->insert(activeIndex,new EjSpaceBlock());
        if(table)
            table->m_counts++;
        if(activeIndex < 0)
            activeIndex = 0;
        doc->lBlocks->at(activeIndex)->ascent = metric.ascent();
    }
}

void EjTextControl::inputBackSpace()
{
    QString str;
    if(activeIndex < 0 || m_startCursor)
    {
        if(m_selectMode == SELECTED && m_selectStart == true && m_endSelectBlock > -1)
        {
            activeIndex = m_endSelectBlock;
            position = m_endSelectPos;
			m_startCursor = false;
            if(m_startSelectBlock < 0)
            {
                m_startSelectBlock = 0;
                m_startSelectPos = 0;
            }
        }
        else
            return;
    }
    if(doc->lBlocks->at(activeIndex)->type == EXT_TABLE)
        return;
    EjBlock *curBlock;
    int count_groups;
    if(m_createPatchEnabled)
    {
        killTimer(m_timerId);
        m_timerId = startTimer(1000);
    }

    if(m_selectMode == SELECTED && m_selectStart == true)
    {
        m_selectStart = false;
        activeIndex = m_endSelectBlock;
        position = m_endSelectPos;
    }

    if(doc->lBlocks->at(activeIndex)->isProperty()) {
        while(doc->lBlocks->at(activeIndex)->isProperty())
        {
            if(activeIndex > 0)
                activeIndex--;
            if(activeIndex == 0)
                return;
        }
        if(doc->lBlocks->at(activeIndex)->type == TEXT)
        {
			position = (dynamic_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex)))->text.count();
        }

    }

    if(doc->lBlocks->at(activeIndex)->type == END_GROUP)
    {
        curBlock = doc->lBlocks->at(activeIndex);
            bool isTable = false;
            count_groups = 1;
            delete doc->lBlocks->takeAt(activeIndex);
            while(count_groups > 0 && activeIndex > 0)
            {
                activeIndex--;
                curBlock = doc->lBlocks->at(activeIndex);
                if(curBlock->type == END_GROUP)
                    count_groups++;
                if(curBlock->type > GROUP_BLOCK)
                {
                    count_groups--;
                    if(!isTable)
                        isTable = curBlock->type == EXT_TABLE;
                }
                delete doc->lBlocks->takeAt(activeIndex);
            }
            activeIndex--;
            if(isTable)
                updateTables(doc);
//        }
        return;
    }



    if(doc->lBlocks->at(activeIndex)->type == BASECELL)
    {
        if(position > 0)
        {
			EjTextBlock *cur_txtBlock = (EjTextBlock*)doc->lBlocks->at(activeIndex);
            cur_txtBlock->text = cur_txtBlock->text.remove(position-1,1);
            cur_txtBlock->flag_redraw = true;
            position--;
            if(position > 0)
            {
                str = cur_txtBlock->text.left(position);
            }
            else
                m_deltapos = 0;
        }
    }
    else if(doc->lBlocks->at(activeIndex)->type == TEXT && position > 0)
    {
		EjTextBlock *cur_txtBlock = (EjTextBlock*)doc->lBlocks->at(activeIndex);
        cur_txtBlock->text = cur_txtBlock->text.remove(position-1,1);
        cur_txtBlock->flag_redraw = true;
        if(cur_txtBlock->text.isEmpty())
        {
            delete doc->lBlocks->at(activeIndex);
            doc->lBlocks->removeAt(activeIndex);
            updateFragments(activeIndex,false);
            activeIndex--;
            position = 0;
            if(activeIndex >= 0 && doc->lBlocks->at(activeIndex)->type == TEXT)
            {
				EjTextBlock *cur_txtBlock = (EjTextBlock*)doc->lBlocks->at(activeIndex);
                position = cur_txtBlock->text.size();
            }
            if(activeIndex < 0)
            {
                m_startCursor = true;
            }
        }
        else
        {
            position--;
            cur_txtBlock->width = 0;
            str = cur_txtBlock->text.left(position);
        }
    }
    else if(doc->lBlocks->at(activeIndex)->type == TEXT)
    {
        if(activeIndex > 0)
        {
            activeIndex--;
            position = 0;
            if(activeIndex >= 0 && (doc->lBlocks->at(activeIndex)->type == TEXT || doc->lBlocks->at(activeIndex)->type == BASECELL))
            {
				position = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex))->text.size();
            }
            inputBackSpace();
        }
        else
            m_startCursor = true;
    }
    else if(activeIndex >= 0)
    {
        if(doc->lBlocks->at(activeIndex)->type == ENTER)
        {
            if(activeIndex + 1 < doc->lBlocks->count() && doc->lBlocks->at(activeIndex + 1)->type == EXT_TABLE)
            {
                return;
            }
            if(activeIndex > 0 && doc->lBlocks->at(activeIndex - 1)->type == BASECELL)
            {
                return;
            }
        }
        if(doc->lBlocks->at(activeIndex)->type >= GROUP_BLOCK)
        {
            EjGroupBlock *curGroupBlock = dynamic_cast<EjGroupBlock*>(doc->lBlocks->at(activeIndex));
            curGroupBlock->remFromBlocks(doc->lBlocks);
            bool isTable = (curGroupBlock->type == EXT_TABLE || curGroupBlock->type == EXT_TASKS_GROUP);
            delete curGroupBlock;
            activeIndex--;
            if(isTable)
                updateTables(doc);
        }
        else
        {
            curBlock = doc->lBlocks->at(activeIndex);
            while(curBlock->parent)
                curBlock = curBlock->parent;
            if(curBlock->type >= GROUP_BLOCK)
            {
                EjGroupBlock *curGroupBlock = dynamic_cast<EjGroupBlock*>(curBlock);
                curGroupBlock->remBlock(doc->lBlocks,doc->lBlocks->at(activeIndex));
            }
            else if(doc->lBlocks->at(activeIndex)->type != END_GROUP)
            {
                delete doc->lBlocks->at(activeIndex);
                doc->lBlocks->removeAt(activeIndex);
            }
            activeIndex--;
        }
        position = 0;
        if(activeIndex >= 0 && (doc->lBlocks->at(activeIndex)->type == TEXT || doc->lBlocks->at(activeIndex)->type == BASECELL))
        {
			EjTextBlock *cur_txtBlock = (EjTextBlock*)doc->lBlocks->at(activeIndex);
            position = cur_txtBlock->text.size();
        }
        if(activeIndex < 0)
        {
            activeIndex = 0;
            m_startCursor = true;
        }
    }
    if(activeIndex > doc->lBlocks->count() - 1)
        activeIndex = doc->lBlocks->count() - 1;

    if(m_selectMode == SELECTED && m_startSelectBlock > -1 && m_endSelectBlock > -1)
    {
        if(activeIndex > m_startSelectBlock)
            inputBackSpace();
        else if(position > m_startSelectPos)
            inputBackSpace();
        else setSelectMode(NO_SELECTED);
    }
}


void EjTextControl::setWidth(int width)
{
    if(m_viewMode == RICH_TEXT)
    {
        m_width = width;
    }
}

void EjTextControl::setHeight(int height)
{
    if(m_viewMode == RICH_TEXT)
    {
        calc();
        if(m_height < height)
        {
            m_height = height;
            emit controlHeightChanged();
        }
    }
}

void EjTextControl::calcCursor(bool force)
{
    int x,y;
    QString str;
    int leftControl;
    EjBlock *curBlock;
    bool isPositionChanged = false;
    bool isHeightChanged = false;
    if(m_isViewDoc && !doc->lPages->isEmpty())
    {
        leftControl = leftColontitul + doc->lPages->at(0)->leftMarging;
        y = doc->lPages->at(0)->topMarging;;
    }
    else
    {
        leftControl = leftColontitul;
        y = 0;
    }

    if(m_startCursor || activeIndex < 0)
    {
        x = leftControl;
        emit cursorPositionChanged(x,y);
        m_posCursorX = x; m_posCursorY = y;
        int h = 0;
        if(!doc->lBlocks->isEmpty())
        {
            int index = 0;
            while (index < doc->lBlocks->count()) {
                if(doc->lBlocks->at(index)->type == TEXT)
                {
                    h = doc->lBlocks->at(index)->height();
                    break;
                }
                index++;
            }
        }
        if(h == 0)
        {
			EjTextStyle *textStyle = getTextStyle(0);
            if(textStyle)
            {
                h = textStyle->fontSize()  * 100 * 0.347;
            }
            else
                h = 400;
        }

        if(force || h != m_heightCursor)
            emit cursorHeightChanged(h);
        m_heightCursor = h;
        return;
    }
    if(doc->lBlocks->empty()) return;

    if(activeIndex >= 0)
    {
        curBlock = doc->lBlocks->at(activeIndex);
        x = curBlock->x;
        if(curBlock->isProperty())
            y = curBlock->y;
        else
            y = curBlock->y + curBlock->interval_top;
        if(curBlock->type == TEXT || curBlock->type == BASECELL)
        {
            QFontMetrics drawMetrics = getDrawMetrics(activeIndex);

			str = static_cast<EjTextBlock*>(curBlock)->text.left(position);
			m_deltapos = drawMetrics.horizontalAdvance(str) * 100 * 0.347;
            x += m_deltapos;
        }
        else if(curBlock->type == ENTER)
        {
        }
        else
        {
            x += curBlock->width;
        }
        if(force || m_posCursorX != x || m_posCursorY != y)
        {
            isPositionChanged = true;
            m_posCursorX = x; m_posCursorY = y;
        }
        if( doc->lBlocks->at(activeIndex)->type == BASECELL)
        {
            QFontMetrics drawMetrics = getDrawMetrics(activeIndex);
            int h = drawMetrics.height() * 100 * 0.347 + m_interval*1.5;
			EjTableBlock *table = isTable(activeIndex);
            x = curBlock->x + table->spacing;
            isPositionChanged = true;

            if(force || h != m_heightCursor)
                isHeightChanged = true;
            m_heightCursor = h;
        }
        else
        {
            int index = activeIndex;
            curBlock = doc->lBlocks->at(index);
            while(curBlock->isProperty() && index > 0)
            {
                index--;
                curBlock = doc->lBlocks->at(index);
            }
            int h = curBlock->ascent + curBlock->descent; // * (curBlock->type == TEXT || curBlock->type == ENTER || curBlock->type == SPACE ? 0.7 : 1.0);
            if(index != activeIndex)
            {
            }
            if(isTable(activeIndex))
                h += m_interval*1.5;
            if(force || h != m_heightCursor)
                isHeightChanged = true;
            m_heightCursor = h;

        }
        if(isPositionChanged)
            emit cursorPositionChanged(x,y);
        if(isHeightChanged)
            emit cursorHeightChanged(m_heightCursor);
    }
}

void EjTextControl::calcData(bool force)
{
    m_calcIndex -= 5000;
    if(m_calcIndex < 0)
        m_calcIndex = 0;
    calcNext(force);
    if(activeIndex > doc->lBlocks->count() - 1)
        activeIndex = doc->lBlocks->count() - 1;
}

void EjTextControl::calcNext(bool force)
{
    if(doc->lBlocks->count() == 0)
        return;
    int index = m_calcIndex;
    if(m_calcIndex > 0 && (!force && m_calcIndex == doc->lBlocks->count() - 1))
        return;
    if(m_calcIndex + 5000 > doc->lBlocks->count() - 1) {
        m_calcIndex = doc->lBlocks->count() - 1;
    }
    else
        m_calcIndex += 5000;
    calc(index, force);
}

void EjTextControl::setCursor(int x, int y)
{
    int cur1 = 0;
    int cur2 = 0;
    int cur3;
    int newActiveBlock;
    m_startCursor = false;

    QString txt;
    newActiveBlock = wichBlock(x,y);
	EjTableBlock *table = isTable(newActiveBlock);

    if(m_statusMode == EDIT_CELL && newActiveBlock != activeIndex)
    {
        if(!table)
            return;
        cur1 = newActiveBlock;
        while(cur1 > -1 && cur1 < doc->lBlocks->count() - 1 && doc->lBlocks->at(cur1)->type != BASECELL)
            cur1--;
        cur2 = activeIndex;
        while(cur2 > -1 && cur2 < doc->lBlocks->count() - 1 && doc->lBlocks->at(cur2)->type != BASECELL)
            cur2--;
        if(cur1 != cur2)
            return;
    }
    if(table && m_statusMode == EDIT_TEXT && doc->lBlocks->at(newActiveBlock)->type != BASECELL)
    {
        newActiveBlock = table->prevCell(newActiveBlock);
    }
    activeIndex = newActiveBlock;
    position = 0;
    if(activeIndex < 0)
    {
        if(doc->lBlocks->count() > 0)
        {
            m_startCursor = false;
            activeIndex = 0;
            position = 0;
            while(activeIndex < doc->lBlocks->count() - 1 &&
                  doc->lBlocks->at(activeIndex)->isProperty())
                activeIndex++;
        }
        else m_startCursor = true;
        calcCursor();
        return;
    }
    if(doc->lBlocks->at(activeIndex)->type == BASECELL && m_statusMode != EDIT_CELL)
    {
        calcCursor();
        return;
    }

    if(doc->lBlocks->at(activeIndex)->type == TEXT || doc->lBlocks->at(activeIndex)->type == BASECELL)
    {
        bool is_remove = false;
		EjTextBlock *cur_txt = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex));

        QFontMetrics drawMetrics = getDrawMetrics(activeIndex);
        QString str3;
        position = 0;

		txt = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex))->text;
        cur1 = 0;
        cur2 = txt.size();
        int d1 = doc->lBlocks->at(activeIndex)->x;
        str3 = txt;
		int d3 = drawMetrics.horizontalAdvance(str3) * 100 * 0.347;
        while(1)
        {
            if(cur2 - cur1 <= 1)
            {
                break;
            }
            cur3 = (cur1 + cur2) / 2;
            str3 = txt.left(cur3);
			d3 = drawMetrics.horizontalAdvance(str3) * 100 * 0.347 + doc->lBlocks->at(activeIndex)->x;
            if(x < d3)
            {
                cur2 = cur3;
            }
            else {
                cur1 = cur3;
                d1 = d3;
            }
        }
        str3 = txt.left(cur2);
		position_w = drawMetrics.horizontalAdvance(str3) * 100 * 0.347;
        d3 = position_w + doc->lBlocks->at(activeIndex)->x;
        if(abs(x-d1) <= abs(d3-x)) position = cur1;
        else position = cur2;
        if(doc->lBlocks->at(activeIndex)->type == BASECELL && doc->lBlocks->at(activeIndex + 1)->type != BASECELL)
        {
			EjCellBlock *curCell = (EjCellBlock*)doc->lBlocks->at(activeIndex);
                while(activeIndex <  doc->lBlocks->count() - 1
                      && (doc->lBlocks->at(activeIndex + 1)->type != BASECELL
                          && doc->lBlocks->at(activeIndex + 1)->type != END_GROUP))
                    activeIndex++;
                if(doc->lBlocks->at(activeIndex)->type == TEXT)
                {
					txt = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex))->text;
                    position = txt.count();
                }
                else
                    position = 0;
//            }
        }
        str3 = txt.left(position);
		position_w = drawMetrics.horizontalAdvance(str3) * 100 * 0.347;

    }
    else if(activeIndex == 0 && doc->lBlocks->at(activeIndex)->type == ENTER)
        m_startCursor = true;

    if(m_statusMode == EDIT_TEXT || m_statusMode == EDIT_CELL)
    {
        calcCursor();
    }
}

void EjTextControl::startSelected(int x, int y)
{
    if(m_selectMode != SELECTED)
        return;
    m_selectStart = true;
    if(doc->lBlocks->isEmpty() || activeIndex < 0) return;
    if(activeIndex < (m_startSelectBlock + m_endSelectBlock) / 2 )
    {
    }
    else if(activeIndex > (m_startSelectBlock + m_endSelectBlock) / 2)
    {
        m_selectStart = false;
    }
    else
    {


        if(doc->lBlocks->at(activeIndex)->type == TEXT && m_startSelectBlock == m_endSelectBlock)
        {
            if(position <= (m_startSelectPos + m_endSelectPos) / 2)
            {
            }
            else
            {
                m_selectStart = false;
            }
        }

    }

}

void EjTextControl::updateSelected()
{
    if(activeIndex < 0 || m_selectMode != SELECTED)
        return;
    doc->lBlocks->at(activeIndex)->flag_redraw = true;
    if(m_selectStart)
    {
        if(activeIndex < m_endSelectBlock || (activeIndex == m_endSelectBlock && position < m_endSelectPos))
        {
            m_startSelectBlock = activeIndex;
            m_startSelectPos = position;
            m_startSelectWidth = position_w;
        }
        else
        {
            m_startSelectBlock = m_endSelectBlock;
            m_startSelectPos = m_endSelectPos;
            m_startSelectWidth = m_endSelectWidth;
            m_endSelectBlock = activeIndex;
            m_endSelectPos = position;
            m_endSelectWidth = position_w;
            m_selectStart = false;
        }
    }
    else
    {
        if(activeIndex > m_startSelectBlock || (activeIndex == m_startSelectBlock && position > m_startSelectPos))
        {
            m_endSelectBlock = activeIndex;
            m_endSelectPos = position;
            m_endSelectWidth = position_w;
        }
        else
        {
            m_endSelectBlock = m_startSelectBlock;
            m_endSelectPos = m_startSelectPos;
            m_endSelectWidth = m_startSelectWidth;;
            m_startSelectBlock = activeIndex;
            m_startSelectPos = position;
            m_startSelectWidth = position_w;
            m_selectStart = true;
        }
    }

}

void EjTextControl::clearSelected()
{
    m_startSelectBlock = m_endSelectBlock = -1;
    m_startSelectPos = m_endSelectPos = 0;
}

int EjTextControl::wichBlock(int x, int y)
{
    int cur1 = 0;
    int cur2 = 0;
    int cur3;
    int cur4;
    int res = -1;
    EjGroupBlock *cur_block;
	EjCellBlock *curCell;
    JString *curString;
    EjBlock *block;
    if(doc->lBlocks->isEmpty() || doc->lStrings->isEmpty())
        return res;
    cur2 = doc->lStrings->count() -1;
    while(1)
    {
        if(cur2 - cur1 <= 1)
        {
            break;
        }
        cur3 = (cur1 + cur2) / 2;
        if(y < doc->lStrings->at(cur3)->y + doc->lStrings->at(cur3)->height)  //+ doc->lStrings->at(cur3)->height
        {
            cur2 = cur3;
        }
        else cur1 = cur3;
        qDebug() << "while 1" << cur1 << cur2;
    }
    if(y < (doc->lStrings->at(cur1)->y + doc->lStrings->at(cur1)->height + m_interval))
        cur3 = cur1;
    else cur3 = cur2;
    curString = doc->lStrings->at(cur3);
    cur1 = curString->startBlock;
    cur2 = curString->endBlock;
    if(cur2 > doc->lBlocks->count() - 1)
        cur2 = doc->lBlocks->count() - 1;

    if(cur1 < 0)
        cur1 = 0;
    if(cur2 < 0)
        cur2 = 0;

    while(1)
    {
        qDebug() << "while 2" << cur1 << cur2 << cur3;
        if(doc->lBlocks->at(cur1)->parent != NULL && !doc->lBlocks->at(cur1)->parent->isGlassy())
        {
            block = doc->lBlocks->at(cur1);
            block = block->rootBlock();
            cur1 = (dynamic_cast<EjGroupBlock*>(block))->m_index;
            if(doc->lBlocks->at(cur1) != block)
            {
                (dynamic_cast<EjGroupBlock*>(block))->calcLenght(cur1,doc->lBlocks);
            }
        }
        cur3 = cur2;
        if(doc->lBlocks->at(cur2)->parent != NULL && !doc->lBlocks->at(cur2)->parent->isGlassy())
        {
            block = doc->lBlocks->at(cur2);
            block = block->rootBlock();
            cur_block = dynamic_cast<EjGroupBlock*>(block);
            cur3 = cur_block->m_index;
        }
        if(doc->lBlocks->at(cur1)->x > x)
        {
            cur2 = cur1;
            break;
        }

        if(cur3 != cur2 && doc->lBlocks->at(cur3)->x < x)
        {
            cur1 = cur2;
            break;
        }
        cur2 = cur3;
        if(cur2 - cur1 <= 1)
        {
            break;
        }
        cur3 = (cur1 + cur2) / 2;
        if(doc->lBlocks->at(cur3)->isGlassy())
            cur3++;
        while(doc->lBlocks->at(cur3)->isProperty() && cur3 < doc->lBlocks->count() - 1)
            cur3++;
        if(doc->lBlocks->at(cur3)->parent != NULL && !doc->lBlocks->at(cur3)->parent->isGlassy())
        {
            block = doc->lBlocks->at(cur3);
            block = block->rootBlock();
            cur3 = (dynamic_cast<EjGroupBlock*>(block))->m_index;
        }
        if(cur3 == cur1)
        {
            cur2 = cur1;
            break;
        }
        if(cur3 == cur2)
        {
            cur1 = cur2;
            break;
        }
        if((cur3 > 0 && x < doc->lBlocks->at(cur3)->x + doc->lBlocks->at(cur3)->width))
        {
            cur2 = cur3;
        }
        else cur1 = cur3;

    }
    if(cur1 >= 0 && x <= (doc->lBlocks->at(cur1)->x + doc->lBlocks->at(cur1)->width))
        res = cur1;
    else res = cur2;
    if(res == 0 && x < doc->lBlocks->at(res)->x)
    {
        res--;
    }
    if(res < 0)
        return res;
	EjTableBlock *table = 0;
	foreach(EjTableBlock *curTable, *doc->lTables)
    {
        if(res >= curTable->m_index && res <= (curTable->m_index + curTable->m_counts))
        {
            table = curTable;
            break;
        }
    }

    if(table)
    {
        double d1;
        double d2;
        curCell = nullptr;
        bool first = true;

        for(int i = table->startCell(); i <= table->endBlock(); i++)
        {
            if(i < doc->lBlocks->count() - 1 && doc->lBlocks->at(i)->type == BASECELL) // && doc->lBlocks->at(i+1)->type != BASECELL)
            {
                if(curCell)
                    break;
                if(x >= doc->lBlocks->at(i)->x && x <= doc->lBlocks->at(i)->x + doc->lBlocks->at(i)->width
                        //                        && y <= doc->lBlocks->at(i)->y && y >= doc->lBlocks->at(i)->y-doc->lBlocks->at(i)->height)
                        && y >= doc->lBlocks->at(i)->y && y <= doc->lBlocks->at(i)->y + doc->lBlocks->at(i)->ascent)
                {
						curCell = (EjCellBlock*)doc->lBlocks->at(i);
                        res = i;
                        d1 = (curCell->x - x) * (curCell->x - x) + (curCell->y - y) * (curCell->y - y);
                        d2 = (curCell->x + curCell->width - x) * (curCell->x + curCell->width - x) + (curCell->y + curCell->height() - y) * (curCell->y + curCell->height() - y);
                        if(curCell->visible == false)
                        {
							curCell = (EjCellBlock*)curCell->parent;
                            res = doc->lBlocks->indexOf(curCell);
                            d1 = (curCell->x - x) * (curCell->x - x) + (curCell->y - y) * (curCell->y - y);
                            d2 = (curCell->x + curCell->width - x) * (curCell->x + curCell->width - x) + (curCell->y + curCell->height() - y) * (curCell->y + curCell->height() - y);
                        }
                }

            }
            else
            {
                EjBlock *curBlock = doc->lBlocks->at(i);
                if(x >= curBlock->x && x <= curBlock->x + curBlock->width
                        && y >= curBlock->y && y <= curBlock->y + curBlock->interval_top + curBlock->ascent + curBlock->descent + curBlock->interval_bottom)
                {
                    res = i;
                    curCell = table->currentCell(i);
                    if(curCell->visible == false)
                    {
						curCell = (EjCellBlock*)curCell->parent;
                        res = doc->lBlocks->indexOf(curCell);
                    }
                    break;
                }
                else
                {
                    double dd1 = (curBlock->x - x) * (curBlock->x - x) + (curBlock->y - y) * (curBlock->y - y);
                    double dd2 = (curBlock->x + curBlock->width - x) * (curBlock->x + curBlock->width - x) + (curBlock->y + curBlock->height() - y) * (curBlock->y + curBlock->height() - y);
                    if(curCell && (dd1 < d1 || dd2 < d2 || first))
                    {
                        res = i;
                        d1 = dd1;
                        d2 = dd2;
                        first = false;
                    }
                }
            }
        }
    }
    if(res > -1)
    {
        block = doc->lBlocks->at(res);
        while(block->isProperty() && res >= 0)
        {
            res--;
            if(res > -1)
                block = doc->lBlocks->at(res);
        }
    }
    return res;
}

EjBlock* EjTextControl::getBlock(int x, int y)
{
    EjBlock *block = 0;
    int res = wichBlock(x,y);
    if(res > -1)
        block = doc->lBlocks->at(res);
    return block;
}

EjBlock *EjTextControl::currentBlock()
{
    if(activeIndex > -1 && activeIndex < doc->lBlocks->count() )
        return doc->lBlocks->at(activeIndex);
    return NULL;
}

void EjTextControl::selectBlock(int x, int y)
{
    int sel_block = wichBlock(x,y);
    if(sel_block < 0) return;
	EjTableBlock *table = isTable(sel_block);
    m_startSelectBlock = m_endSelectBlock = sel_block;
    m_startSelectPos = m_endSelectPos = 0;
    if(table && m_statusMode != EDIT_CELL)
    {
        if(doc->lBlocks->at(sel_block)->type != BASECELL)
            m_startSelectBlock = m_endSelectBlock = table->prevCell(sel_block);
    }
    else if(doc->lBlocks->at(sel_block)->type == TEXT || doc->lBlocks->at(sel_block)->type == BASECELL)
    {
        while(m_startSelectBlock > 0)
        {
            if(doc->lBlocks->at(m_startSelectBlock-1)->type != TEXT)
                break;
            m_startSelectBlock--;
        }
        while(m_endSelectBlock < doc->lBlocks->count() - 1)
        {
            if(doc->lBlocks->at(m_endSelectBlock+1)->type != TEXT)
                break;
            m_endSelectBlock++;
        }
		m_endSelectPos = static_cast<EjTextBlock*>(doc->lBlocks->at(m_endSelectBlock))->text.size();
    }
}

void EjTextControl::cursorLeft()
{
    bool changeActive = false;
    int activeBlock_back;
    EjBlock *curBlock;
    int count_groups;
    resetUpDown();
    if(doc->lBlocks->count() == 0 || activeIndex < 0)
        return;
    int index = activeIndex;
    while(doc->lBlocks->at(index)->isProperty())
    {
        position = 0;
        index--;
        if(index < 0)
        {
            break;
        }
    }
    if(index < 0)
        return;
    else
        activeIndex = index;
    if(m_statusMode == EDIT_TEXT)
    {
		EjTableBlock *table = isTable(activeIndex);
        if(table)
        {
            activeIndex = table->currCellIndex(activeIndex);
            if(activeIndex < 0)
                activeIndex = 0;
        }
    }
    if(doc->lBlocks->at(activeIndex)->type == TEXT) // || (doc->lBlocks->at(activeIndex)->type == BASECELL  && m_statusMode == EDIT_CELL))
    {

        if(position > 0)
        {
            position--;
            if(position == 0 && activeIndex > 0 && doc->lBlocks->at(activeIndex-1)->type == END_GROUP)
                changeActive = true;
        }
        else if(activeIndex > 0) //s && doc->lBlocks->at(activeIndex - 1)->type != BASECELL)
        {
            changeActive = true;
        }
    }
    else
    {
        changeActive = true;
    }
	if(changeActive && activeIndex > 0)
    {
        activeBlock_back = index = activeIndex;
        position = 0;
        index--;
        while(index > 0 && doc->lBlocks->at(index)->type != BASECELL && doc->lBlocks->at(index)->isProperty())
        {
            index--;
        }
		if(index > -1 && doc->lBlocks->at(index)->type != BASECELL && !doc->lBlocks->at(index)->isProperty())
                activeIndex = index;

		if(index > -1 && m_statusMode == EDIT_TEXT && doc->lBlocks->at(index)->type == BASECELL)
                activeIndex = index;
        if(doc->lBlocks->at(activeIndex)->type == SPACE || doc->lBlocks->at(activeIndex)->type == ENTER )
        {
            if(activeIndex > 0 && (doc->lBlocks->at(activeBlock_back)->type == TEXT ||
                                   doc->lBlocks->at(activeBlock_back)->type == BASECELL))
            {
                activeBlock_back = activeIndex;
                activeIndex--;
                position = 0;
            }
			else
				m_startCursor = true;
        }
        if(activeBlock_back != activeIndex && doc->lBlocks->at(activeIndex)->type == TEXT) // || doc->lBlocks->at(activeIndex)->type == BASECELL)
        {
			position = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex))->text.size();
            if(doc->lBlocks->at(activeBlock_back)->type == TEXT)
            {
                position--;
                if(position < 0)
                    position = 0;
            }
        }
        curBlock = doc->lBlocks->at(activeIndex);
        if(curBlock->type == END_GROUP)
        {
            count_groups = 1;
            while(count_groups > 0 && activeIndex > 0)
            {
                activeIndex--;
                curBlock = doc->lBlocks->at(activeIndex);
                if(curBlock->type == END_GROUP)
                    count_groups++;
                if(curBlock->type > GROUP_BLOCK)
                    count_groups--;
            }
        }
    }
    else if(changeActive && activeIndex == 0)
    {
		m_startCursor = true;
    }
    qDebug() << "position= " << position;
}

void EjTextControl::cursorRight()
{
    bool changeActive = false;
    resetUpDown();
    if(doc->lBlocks->count()==0)
        return;
    if(m_startCursor)
    {
        m_startCursor = false;
        if(activeIndex > -1 && doc->lBlocks->at(activeIndex)->type != TEXT && doc->lBlocks->at(activeIndex)->type != BASECELL)
            return;
    }
    if(m_statusMode == EDIT_TEXT)
    {
		EjTableBlock *table = isTable(activeIndex);
        if(table)
        {
            activeIndex = table->currCellIndex(activeIndex);
            if(activeIndex < 0)
                activeIndex = 0;
        }
    }
    if(activeIndex > -1 && (doc->lBlocks->at(activeIndex)->type == TEXT)) // || (doc->lBlocks->at(activeIndex)->type == BASECELL && m_statusMode == EDIT_CELL)))
    {
		if(position < static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex))->text.size()) position++;
        else if(activeIndex < doc->lBlocks->count() - 1)
        {
            if(m_statusMode == EDIT_CELL)
            {
                if(doc->lBlocks->at(activeIndex + 1)->type != BASECELL)
                    changeActive = true;
            }
            else
            {
                changeActive = true;
            }
        }
    }
    else if(m_statusMode == EDIT_TEXT || doc->lBlocks->at(activeIndex + 1)->type != BASECELL)
    {
        if(m_statusMode == EDIT_CELL)
        {
            if(doc->lBlocks->at(activeIndex + 1)->type != BASECELL)
                changeActive = true;
        }
        else
        {
            changeActive = true;
        }
    }
    if(changeActive && activeIndex < doc->lBlocks->count() - 1 && doc->lBlocks->count() > 0)
    {
        if(activeIndex > -1 && m_statusMode == EDIT_TEXT && doc->lBlocks->at(activeIndex)->type == BASECELL )
        {
			EjTableBlock *table = isTable(activeIndex);
            if(table)
                activeIndex = table->nextCell(activeIndex);
        }
        else
        {
            int index = activeIndex;
            if(index > -1 && doc->lBlocks->at(index)->type > GROUP_BLOCK)
            {
                index += ((EjGroupBlock*)doc->lBlocks->at(index))->m_counts + 1;
                if(index > doc->lBlocks->count() - 1)
                    index = doc->lBlocks->count() - 1;
                position = 0;
            }
            else
                index++;
            bool isAllProperty = true;
            while(doc->lBlocks->at(index)->isProperty() && index < doc->lBlocks->count() - 1)
            {
                isAllProperty = false;

                if(m_statusMode == EDIT_CELL)
                {
                    if(doc->lBlocks->at(index + 1)->type != BASECELL)
                        index++;
                    else
                        break;
                }
                else
                {
                    index++;
                }
            }
            if(activeIndex != index && !doc->lBlocks->at(index)->isProperty())
            {
                activeIndex = index;
                position = 0;
            }
        }
		if(doc->lBlocks->at(activeIndex)->type == TEXT && static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex))->text.size() > position)
            position++;

    }
    qDebug() << "position= " << position;
}

void EjTextControl::worldLeft()
{
	EjTableBlock *table = isTable(activeIndex);
    if(table && m_statusMode == EDIT_TEXT)
        return;
    if(activeIndex < 0)
    {
        m_startCursor = true;
        return;
    }
    if(activeIndex > 0 && doc->lBlocks->at(activeIndex)->type == TEXT && position == 0)
    {
        int index = activeIndex - 1;
        while(index > 0 &&  doc->lBlocks->at(index)->isProperty() && doc->lBlocks->at(index)->type != BASECELL)
            index--;
        if(!doc->lBlocks->at(index)->isProperty() &&  doc->lBlocks->at(index)->type != BASECELL)
            activeIndex = index;
    }
    while(activeIndex > 0 && doc->lBlocks->at(activeIndex)->type != TEXT)
        cursorLeft();
    if(activeIndex < 0)
    {
        m_startCursor = true;
        return;
    }
    activeIndex = startText(activeIndex);
    if(activeIndex < 0)
    {
        m_startCursor = true;
    }
    position = 0;
}

void EjTextControl::worldRight()
{
	EjTableBlock *table = isTable(activeIndex);
    if(table && m_statusMode == EDIT_TEXT)
        return;
    if(activeIndex < 0)
        activeIndex = 0;
    while(activeIndex < doc->lBlocks->count() - 1 && doc->lBlocks->at(activeIndex)->type != TEXT)
        cursorRight();
    if(activeIndex > doc->lBlocks->count() - 1)
        activeIndex = doc->lBlocks->count() - 1;
    int index = endText(activeIndex);
    while(index < doc->lBlocks->count() - 1 &&  doc->lBlocks->at(index)->isProperty() && doc->lBlocks->at(index)->type != BASECELL)
        index++;
    if(!doc->lBlocks->at(index)->isProperty() && doc->lBlocks->at(index)->type != BASECELL)
        activeIndex = index;
    if(activeIndex > doc->lBlocks->count() - 1)
        activeIndex = doc->lBlocks->count() - 1;
    position = 0;
    if(doc->lBlocks->at(activeIndex)->type == TEXT)
		position = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex))->text.size();

}

void EjTextControl::resetUpDown()
{
    m_upDown_x = m_upDown_y = -1;
}


void EjTextControl::cursorUp()
{
    if(activeIndex < 1)
        return;
    int x_s;
    int y_s;
    if( m_upDown_x == -1 && m_upDown_y == -1)
    {
        x_s = m_upDown_x = m_posCursorX;
    }
    else
    {
        x_s = m_upDown_x;
    }
    int index_string = getIndexString(activeIndex);
    JString *cur_string = doc->lStrings->at(0);
    index_string--;
    if(index_string > -1)
        cur_string = doc->lStrings->at(index_string);
    y_s = m_upDown_y = cur_string->y; //m_posCursorY +
    setCursor(x_s,y_s);
}

void EjTextControl::cursorDown()
{
    if(m_startCursor && doc->lBlocks->count() > 0)
    {
        m_startCursor = false;
        activeIndex = 0;
        position = 0;
    }
    if(activeIndex < 0)
        return;
    int x_s;
    int y_s;
    if( m_upDown_x == -1 && m_upDown_y == -1)
    {
        x_s = m_upDown_x = m_posCursorX;
    }
    else
    {
        x_s = m_upDown_x;
    }
    int index_string = getIndexString(activeIndex);
    JString *cur_string = doc->lStrings->at(index_string);
    int delta = m_posCursorY - cur_string->y;
    index_string++;
    if(index_string < doc->lStrings->count())
        cur_string = doc->lStrings->at(index_string);
    y_s = m_upDown_y =  cur_string->y; // + m_posCursorY

    qDebug() << __FILE__ << __LINE__ << "m_posCursorY:" << m_posCursorY << "y_s:" << y_s;
    setCursor(x_s,y_s);

}

void EjTextControl::cursorFirst()
{
    if(doc->lBlocks->count() > 0)
        activeIndex = 0;
    else
        activeIndex = -1;
    position = 0;
}

void EjTextControl::cursorLast()
{
    activeIndex = doc->lBlocks->count() - 1;
    position = 0;
}

int EjTextControl::setInputMode(bool mode, QString &text, int preeditCursor)
{
	EjTextBlock *cur_txt = nullptr;
    int res = 0;

    if(text.contains(" "))
    {
        QStringList ltext = text.split(" ");
        int pos = 0;
        int activeIndexBack = activeIndex;
        if(id_inputSelBlock > -1)
            activeIndex = id_inputSelBlock;
        else
            id_inputSelBlock = activeIndex;
        bool bSendCursor = true;
        for(int i = 0; i < ltext.count(); i++)
        {

            if(ltext[i].count() > 0)
            {
                if(i > 0)
                {
                    pos++;
                    if(mode == true)
                    {
                        inputSpace();
                    }
                }
                if(mode)
                {
					cur_txt = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex));
                    if(cur_txt->type != TEXT || i > 0)
                    {
                        activeIndex++;
						doc->lBlocks->insert(activeIndex, new EjTextBlock());
						cur_txt = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex));
                    }
                    cur_txt->text = ltext[i];
                    cur_txt->flag_redraw = true;
                    position = cur_txt->text.length();
                    cur_txt->width = 0;
                }
                if(pos + ltext[i].count() >= preeditCursor && bSendCursor)
                {
                    if(getBlocks()->at(activeIndex)->type == TEXT)
                    {
                        if(mode == true)
                            position = preeditCursor - pos;
						EjTextBlock *block = dynamic_cast<EjTextBlock*>(getBlocks()->at(activeIndex));
                        if(block && (block->text.length() < position ) && mode == true) //|| mode == false
                        {
                            position = block->text.length();
                        }
                    }
                    else
                    {
                        position = 0;
                    }
                    bSendCursor = false;
                }
                pos += ltext[i].count();
            }
            else if(i > 0){
                pos++;
                if(mode == true)
                {
                    inputSpace();
                    position = 0;
                }
            }
        }
        if(!mode)
        {
            activeIndex = activeIndexBack;
        }
    }
    else if(text.contains("\n"))
    {
		EjTableBlock *table = isTable(activeIndex);
        if(!table || m_statusMode == EDIT_TEXT)
        {
            res = inputEnter();
        }
        else if(m_statusMode == EDIT_CELL)
        {
            setStatusMode(EDIT_TEXT);
            calcTables();
        }
    }
    else
    {
        if(mode == false)
        {
        }
        else
        {
            bool isInsert = false;
            if(activeIndex > -1)
				cur_txt = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex));
            if(mode != m_inputSelectMode)
                isInsert = true;
            if(!cur_txt || cur_txt->type != TEXT)
            {
                activeIndex++;
                isInsert = true;
            }
            else if(isInsert && position == cur_txt->text.length())
            {
                activeIndex++;
            }
            if(isInsert)
            {
				doc->lBlocks->insert(activeIndex, new EjTextBlock());
                id_inputSelBlock = activeIndex;
            }
			cur_txt = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex));

            cur_txt->text = text;
            cur_txt->flag_redraw = true;
            cur_txt->width = 0;
            position = preeditCursor;
        }
    }


    if(activeIndex > -1 && !mode && activeIndex > -1 && doc->lBlocks->at(activeIndex)->type == TEXT)
    {
        while(activeIndex > 0 && (doc->lBlocks->at(activeIndex-1)->type == TEXT))
        {
            activeIndex--;
			cur_txt = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex));
			cur_txt->text += static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex+1))->text;
            position = cur_txt->text.length();
            cur_txt->flag_redraw = true;
            cur_txt->width = 0;
            delete doc->lBlocks->at(activeIndex+1);
            doc->lBlocks->removeAt(activeIndex+1);
        }
        while(activeIndex < doc->lBlocks->count() - 1 && (doc->lBlocks->at(activeIndex+1)->type == TEXT))
        {
			cur_txt = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex));
			cur_txt->text += static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex+1))->text;
            position = cur_txt->text.length();
            cur_txt->flag_redraw = true;
            cur_txt->width = 0;
            delete doc->lBlocks->at(activeIndex+1);
            doc->lBlocks->removeAt(activeIndex+1);
        }
    }
    if(!mode)
        id_inputSelBlock = activeIndex;
    m_inputSelectMode = mode;
    calcInputMethodParams();
    return res;
}

void EjTextControl::setStatusMode(e_statusMode mode)
{
	EjTableBlock *table;
	EjTextBlock *cur_txtBlock;
    QString str;
    bool bOk;
    EjBlock *curBlock = 0;
	EjCellBlock *curCell = 0;
    int k = activeIndex;
    int row;
    int colum;
    double res, d;
    EjCalculator calculator(doc);

    if(mode != READ_ONLY && !docPrev)
    {
		docPrev = new EjDocument();
        doc->copyData(docPrev);
    }

    table = isTable(activeIndex);

    if(table)
    {
        cellParams(table,activeIndex,row,colum);
        curBlock = doc->lBlocks->at(activeIndex);
        while(doc->lBlocks->at(k)->type != BASECELL && k > table->m_index)
            k--;
        if(k == table->m_index)
            return;
        if(k > -1)
			curCell = (EjCellBlock*)doc->lBlocks->at(k);
    }


    if(m_statusMode == EDIT_CELL && mode != m_statusMode &&  table && curCell)
    {
        //        QLocale locale;
        QString str = curCell->getText();
		if(!str.isEmpty() && str.at(0).toLatin1() == '=' && (curCell->vid == EjCellBlock::CELL_AUTO
							  || curCell->vid == EjCellBlock::CELL_FORMULA))
        {
            curCell->setFormula(str);
        }
		else if(curCell->vid == EjCellBlock::CELL_AUTO
				|| curCell->vid == EjCellBlock::CELL_FORMULA)
        {
            curCell->value = getDValue(str,&bOk);
            if(bOk)
                curCell->text = getDText(curCell->value,12);
            else curCell->text.clear();
			curCell->vid = EjCellBlock::CELL_AUTO;
        }
        activeIndex = doc->lBlocks->indexOf(curCell);

		if(colum == table->nColums() - 1 && row > 0 && row < table->nRows() - 1 && table->vid == EjTableBlock::SHOP_LIST)
        {
        }
    }
    else   if(mode == EDIT_CELL && mode != m_statusMode && table && curCell)
    {
		if(table->vid != EjTableBlock::SHOP_LIST)
        {
            QString formula;
			if(curCell->vid == EjCellBlock::CELL_FORMULA )
                formula = curCell->formula();
			if(curCell->vid == EjCellBlock::CELL_AUTO)
                formula = curCell->text;
            if(!formula.isEmpty())
                curCell->setText(formula,this);
        }
        while(activeIndex < doc->lBlocks->count() - 1 && doc->lBlocks->at(activeIndex+1)->type != BASECELL && doc->lBlocks->at(activeIndex+1)->type != END_GROUP)
            activeIndex++;
        if(doc->lBlocks->at(activeIndex)->type == TEXT)
        {
			position = ((EjTextBlock*)doc->lBlocks->at(activeIndex))->text.count();
        }
        else
            position = 0;
    }
    m_statusMode = mode;
    calcData();
    calcCursor();
}

void EjTextControl::setSelectMode(e_selectMode mode)
{
    m_selectMode = mode;
    if(mode == NO_SELECTED)
        m_startSelectBlock = m_endSelectBlock = -1;
    else
    {
        m_selectStart = false;
        m_startSelectBlock = m_endSelectBlock = activeIndex;
        m_startSelectPos = m_endSelectPos = position;
        m_startSelectWidth = m_endSelectWidth = position_w;
        updateSelected();

    }

}

QString EjTextControl::getText() const
{
    QString res;
    for(int i = 0; i < doc->lBlocks->count(); i++)
    {
        switch (doc->lBlocks->at(i)->type) {
        case TEXT:
			res += static_cast<EjTextBlock*>(doc->lBlocks->at(i))->text;
            break;
        case SPACE:
            res += ' ';
            break;
        case ENTER:
            res += '\n';
        default:
            break;
        }

    }
    return res;
}

void EjTextControl::setText(const QString &source)
{
    m_startCursor = false;
    QString txt = source;
    QString left;
    int i  = txt.indexOf(' ');
    while(i > -1)
    {
        left = txt.left(i);
        txt = txt.right(txt.size() - i - 1);
        i = txt.indexOf(' ');
        inputText(left);
        inputSpace();
    }
    if(txt == " ")
    {
        inputSpace();
    }
    else
    {
        inputText(txt);
    }
}

void EjTextControl::groupBlocksIndexes()
{
	EjBlock *curBlock;
	for(int i = 0; i < doc->lBlocks->count(); i++)
	{
		curBlock = (*doc->lBlocks)[i];
		if(curBlock->type >= GROUP_BLOCK)
		{
			dynamic_cast<EjGroupBlock*>(curBlock)->m_index = i;
		}
	}
}

void EjTextControl::setFirstTextStyle()
{
	int i = 0;
	EjBlock *cur_block;
	bool bFind = false;
	while(i < doc->lBlocks->count())
	{
		cur_block = doc->lBlocks->at(i);
		if(cur_block->type == NUM_STYLE)
		{
			if(((EjNumStyleBlock *)cur_block)->style)
			{
				if(((EjNumStyleBlock *)cur_block)->style->m_vid == TEXT_STYLE)
				{
					bFind = true;
					break;
				}
			}
			else if(cur_block->type == TEXT)
			{
				break;
			}
		}
		i++;
	}
	if(bFind)
		return;

	EjTextStyle *curTextStyle = getTextStyle(0);
	EjNumStyleBlock *curNumStyle = new EjNumStyleBlock();
	curNumStyle->num = doc->lStyles->indexOf(curTextStyle);
	curNumStyle->style = curTextStyle;

	doc->lBlocks->insert(0,curNumStyle);
	activeIndex++;
	if(m_startSelectBlock > -1)
		m_startSelectBlock++;
	if(m_endSelectBlock > -1)
		m_endSelectBlock++;
	groupBlocksIndexes();
}

void EjTextControl::setTextStyle(EjTextStyle *textStyle, bool force, bool autoClose)
{
    int index;
	EjTextStyle *oldTextStyle;
	EjTextStyle *curTextStyle;
    EjNumStyleBlock *curNumStyle;
	EjTextStyle *newTextStyle = NULL;
	EjTableBlock *table;
    int type;
    int count;
    int max_count = -1;
    int endIndex;

	if(activeIndex > 0)
	{
		setFirstTextStyle();
	}

    if(m_startSelectBlock == -1 || m_endSelectBlock == -1)
    {
        endIndex = activeIndex;
        {
            if(autoClose)
            {
                index = activeIndex;
                if(position > 0 || activeIndex > startText(activeIndex))
                    index = endText(activeIndex);
                oldTextStyle = getTextStyle(index);
                curNumStyle = new EjNumStyleBlock();
                curNumStyle->num = doc->lStyles->indexOf(oldTextStyle);
                curNumStyle->style = oldTextStyle;
                doc->lBlocks->insert(index,curNumStyle);

                table = isTable(index);
                if(table)
                {
                    table->m_counts++;
                }
                endIndex = index;
            }
            index = activeIndex;
            if(isEndText(index))
            {
                index++;
                activeIndex++;
            }
            else if(activeIndex < endText(activeIndex))
                index = startText(activeIndex);
            if(index == endIndex)
				newTextStyle = dynamic_cast<EjTextStyle*>(getTextStyle(index - 1)->makeCopy());
            else
				newTextStyle = dynamic_cast<EjTextStyle*>(getTextStyle(index)->makeCopy());
            newTextStyle->normalizeStyle(textStyle, force);
            curTextStyle = doc->fromTextStyles(newTextStyle);
            curNumStyle = new EjNumStyleBlock();
            curNumStyle->num = doc->lStyles->indexOf(curTextStyle);
            curNumStyle->style = curTextStyle;

            doc->lBlocks->insert(index,curNumStyle);

            table = isTable(index);
            if(table)
            {
                table->m_counts++;
            }
            activeIndex++;
            oldTextStyle = curTextStyle;
            delete newTextStyle;
            newTextStyle = nullptr;

        }
    }
    else
    {
        int pos = m_endSelectPos;
        index = m_endSelectBlock;
        if(splitText(index,pos)) {
            if(index <= activeIndex)
                activeIndex++;
        }

        oldTextStyle = getTextStyle(index);
        curNumStyle = new EjNumStyleBlock();
        curNumStyle->num = doc->lStyles->indexOf(oldTextStyle);
        curNumStyle->style = oldTextStyle;
        doc->lBlocks->insert(index,curNumStyle);
        table = isTable(index);
        if(table)
        {
            table->m_counts++;
        }
        if(index <= activeIndex)
            activeIndex++;
        endIndex = index;

        if(splitText(m_startSelectBlock,m_startSelectPos) )
        {
            if(m_startSelectBlock <= activeIndex)
                activeIndex++;
            if(m_startSelectBlock <= m_endSelectBlock)
                m_endSelectBlock++;
            m_startSelectPos = 0;
            m_startSelectWidth = 0;
        }
        index = m_startSelectBlock;
		while(index > 0)
		{
			if(doc->lBlocks->at(index)->type == TEXT)
				break;
			index--;
		}
		if(index > 0 && index < m_startSelectBlock)
			index++;

		newTextStyle = dynamic_cast<EjTextStyle*>(getTextStyle(index)->makeCopy());
        newTextStyle->normalizeStyle(textStyle, force);
        curTextStyle = doc->fromTextStyles(newTextStyle);
        curNumStyle = new EjNumStyleBlock();
        curNumStyle->num = doc->lStyles->indexOf(curTextStyle);
        curNumStyle->style = curTextStyle;

        doc->lBlocks->insert(index,curNumStyle);

        table = isTable(index);
        if(table)
        {
            table->m_counts++;
        }
        if(index <= activeIndex)
            activeIndex++;
        if(index <= m_endSelectBlock)
            m_endSelectBlock++;
        oldTextStyle = curTextStyle;
        delete newTextStyle;
        newTextStyle = nullptr;
    }

    for(int i = index + 1; i < endIndex; i++)
    {
        if(doc->lBlocks->at(i)->type == NUM_STYLE) {
            curNumStyle = (EjNumStyleBlock*)doc->lBlocks->at(i);
            if(curNumStyle->style && curNumStyle->style->m_vid == TEXT_STYLE)
            {
				curTextStyle = dynamic_cast<EjTextStyle*>(curNumStyle->style);
                if(curTextStyle)
                {
                    bool isSame = false;
                    if(!force)
                    {
						curTextStyle = dynamic_cast<EjTextStyle*>(curTextStyle->makeCopy());
                        curTextStyle->normalizeStyle(textStyle, false);
                        if(curTextStyle->fullCompare(oldTextStyle) == true)
                        {
                            isSame = true;
                        }
                        else
                        {
                            oldTextStyle = doc->fromTextStyles(curTextStyle);
                            curNumStyle->style = oldTextStyle;
                            curNumStyle->num = doc->lStyles->indexOf(curNumStyle->style);
                        }
                        delete curTextStyle;
                    }
                    if(isSame)
                    {
                        table = isTable(i);
                        if(table)
                        {
                            table->m_counts--;
                        }
                        delete doc->lBlocks->takeAt(i);
                        if(activeIndex >= i)
                            activeIndex--;
                        if(m_endSelectBlock >= i)
                            m_endSelectBlock--;
                        i--;
                        endIndex--;
                    }

                }
            }
        }

    }

    textStyleOptimize();
}

bool EjTextControl::textStyleOptimize()
{

    bool res = false;
    EjNumStyleBlock *curNumStyle;
	EjTableBlock *table;
    bool textEnabled = false;
    int type;
    int start = doc->lBlocks->count() - 1;
    int end = -1;
    while(end < start)
    {
        end++;
        type = doc->lBlocks->at(end)->type;
        if(type == NUM_STYLE)
        {
            curNumStyle = (EjNumStyleBlock*)doc->lBlocks->at(end);
            if(curNumStyle->style && curNumStyle->style->m_vid == TEXT_STYLE)
                break;
        }
    }

    for(int i = start; i > end; i--)
    {
        type = doc->lBlocks->at(i)->type;
        if(type == NUM_STYLE && !textEnabled && i != activeIndex - 1)
        {
            curNumStyle = (EjNumStyleBlock*)doc->lBlocks->at(i);
            if(curNumStyle->style && curNumStyle->style->m_vid == TEXT_STYLE)
            {
                table = isTable(i);
                if(table)
                {
                    table->m_counts--;
                }
                delete doc->lBlocks->takeAt(i);
                if(activeIndex >= i)
                    activeIndex--;
                if(m_endSelectBlock >= i)
                    m_endSelectBlock--;
				if(m_startSelectBlock > i)
                    m_startSelectBlock--;
                i--;
                res = true;
            }
        }
        else if(type == TEXT)
        {
            textEnabled = true;
           // oldTextStyle = nullptr;
        }
        else if(type == NUM_STYLE)
        {
            curNumStyle = (EjNumStyleBlock*)doc->lBlocks->at(i);
            if(curNumStyle->style && curNumStyle->style->m_vid == TEXT_STYLE)
            {
                textEnabled = false;
            }
        }

    }
	groupBlocksIndexes();
    return res;
}

void EjTextControl::setParagraphTextStyle(EjTextStyle *textStyle)
{
    int index = activeIndex;
    if(isTable(index))
    {
        while(index > 0)
        {
            if(doc->lBlocks->at(index)->type == BASECELL || doc->lBlocks->at(index)->type == EXT_TABLE )
                break;
            index--;
        }
        m_startSelectBlock = index;
        index = activeIndex;
        if(isTable(index))
        {
            while(index < doc->lBlocks->count() - 1)
            {
                if(doc->lBlocks->at(index)->type == BASECELL || doc->lBlocks->at(index)->type == END_GROUP)
                    break;
                index++;
            }
        }
        else
        {
            while(index < doc->lBlocks->count())
            {
                if(doc->lBlocks->at(index)->type == ENTER)
                    break;
                index++;
            }

        }
        m_endSelectBlock = index;
    }
    else
    {
        while(index > 0)
        {
            if(doc->lBlocks->at(index)->type == ENTER)
                break;
            index--;

        }
        m_startSelectBlock = index;
        index = activeIndex;
        if(isTable(index))
        {
            while(index < doc->lBlocks->count() - 1)
            {
                if(doc->lBlocks->at(index)->type == BASECELL || doc->lBlocks->at(index)->type == END_GROUP)
                    break;
                index++;
            }
        }
        else
        {
            while(index < doc->lBlocks->count())
            {
                if(doc->lBlocks->at(index)->type == ENTER)
                    break;
                index++;
            }

        }
        m_endSelectBlock = index;
    }
    setTextStyle(textStyle);
}

void EjTextControl::setParagraphStyle(EjParagraphStyle *paragraphStyle)
{
    int index;
    int startIndex;
    if(activeIndex > doc->lBlocks->count() - 1)
        activeIndex = doc->lBlocks->count() - 1;
	EjBaseStyle *curStylePrg = NULL;
	EjBaseStyle *oldStylePrg = getParagraphStyle(activeIndex);
    EjNumStyleBlock *curNumStyle;
	EjBaseStyle *curStyle = NULL;
	EjTableBlock *tableStart, *tableEnd, *table;

    if(activeIndex < 0)
        activeIndex = 0;

    index = startIndex = activeIndex;
    tableStart = isTable(m_startSelectBlock);
    tableEnd = isTable(m_endSelectBlock);
    while(index < doc->lBlocks->count() )
    {

        if(doc->lBlocks->at(index)->type == NUM_STYLE && index != startIndex)
        {
            curNumStyle = (EjNumStyleBlock*)doc->lBlocks->at(index);
            if(curNumStyle->style && curNumStyle->style->m_vid == PARAGRAPH_STYLE)
            {
                oldStylePrg = curNumStyle->style;
                table = isTable(index);
                if(table)
                {
                    table->m_counts--;
                }
                delete doc->lBlocks->takeAt(index);
                index--;
                if(index < 0) index = 0;
            }
        }
        if(index >= m_endSelectBlock || m_endSelectBlock < 0)
        {
            table = isTable(index);

            if(table && (doc->lBlocks->at(index)->type == e_typeBlocks::END_GROUP ||
                         doc->lBlocks->at(index)->type == e_typeBlocks::BASECELL))
            {
                index++;
                break;
            }
            else if(doc->lBlocks->at(index)->type == e_typeBlocks::ENTER)
            {
                index++;
                break;
            }
        }
        doc->lBlocks->at(index)->flag_redraw = true;
        index++;
    }
    if(oldStylePrg && index < doc->lBlocks->count() - 1)
    {
        curNumStyle = new EjNumStyleBlock();
        curNumStyle->num = doc->lStyles->indexOf(oldStylePrg);
        curNumStyle->style = oldStylePrg;
        table = isTable(index);
        doc->lBlocks->insert(index,curNumStyle);
        if(table)
        {
            table->m_counts++;
        }
    }
    index = activeIndex;
    while(index > -1 && doc->lBlocks->count() > 0)
    {
        if(doc->lBlocks->at(index)->type == NUM_STYLE) {
            curNumStyle = (EjNumStyleBlock*)doc->lBlocks->at(index);
            if(curNumStyle->style && curNumStyle->style->m_vid == PARAGRAPH_STYLE)
            {
                table = isTable(index);
                if(table)
                {
                    table->m_counts--;
                }
                delete doc->lBlocks->takeAt(index);
                activeIndex--;
                if(index < 0) index = 0;
            }
        }
        if(doc->lBlocks->count() == 0)
            break;
        if(index <= m_startSelectBlock || m_startSelectBlock < 0)
        {
            table = isTable(index);

            if(table && (doc->lBlocks->at(index)->type == e_typeBlocks::END_GROUP ||
                         doc->lBlocks->at(index)->type == e_typeBlocks::BASECELL))
            {
                break;
            }
            else if(index < doc->lBlocks->count() && doc->lBlocks->at(index)->type == e_typeBlocks::ENTER)
            {
                break;
            }
        }
        doc->lBlocks->at(index)->flag_redraw = true;
        index--;
    }
    if(index > 0 && index < doc->lBlocks->count()
            && (doc->lBlocks->at(index)->isProperty()
                || doc->lBlocks->at(index)->type == e_typeBlocks::ENTER
                || doc->lBlocks->at(index)->type == e_typeBlocks::BASECELL ) )
        index++;
    curStylePrg = doc->fromParagraphStyles(paragraphStyle);
    curNumStyle = new EjNumStyleBlock();
    curNumStyle->num = doc->lStyles->indexOf(curStylePrg);
    curNumStyle->style = curStylePrg;
    table = isTable(index);
    doc->lBlocks->insert(index,curNumStyle);
    if(table)
    {
        table->m_counts++;
    }
    activeIndex++;
}



void EjTextControl::clear()
{
	doc->clear();
    activeIndex = doc->lBlocks->count();
    position = 0;
    m_height = 0;
    emit controlHeightChanged();
}

void EjTextControl::inputTable(int rows, int colums)
{

}

void EjTextControl::addTableString()
{

}

void EjTextControl::addTableColum()
{
}

void EjTextControl::delTableString(QList<EjBlock*> *l_blocks, int &active_block)
{
	EjCellBlock *curCell;
	EjTableBlock *curTable = 0;
	EjTableFragment *curFragment;
    EjSizeProp *sizeProp;
    int row = 0;
    int colum = 0;
    int frgs_row, frge_row;
    int d = 0;
    int index;

    if(active_block < 0 || active_block > l_blocks->count() - 2)
        return;

    while(l_blocks->at(active_block)->type != BASECELL)
        active_block--;

	curCell = (EjCellBlock*)l_blocks->at(active_block);
	curTable = ((EjTableBlock*)(curCell->parent));

    if(curTable)
    {
		if(curTable->vid == EjTableBlock::SHOP_LIST)
            d = 1;
        cellParams(curTable,active_block,row,colum, l_blocks);

        if(d && curTable->nRows() == 2)
        {
            while(curTable->endBlock() > curTable->startCell())
            {
                delete l_blocks->takeAt(curTable->startCell() );
                if(l_blocks == doc->lBlocks)
                    updateFragments(curTable->startCell(),false);
                else
                    curTable->m_counts--;
            }
            if(l_blocks == doc->lBlocks)
                updateFragments(curTable->startCell(),false);
            delete l_blocks->takeAt(curTable->startCell());
            curTable = 0;
            if(l_blocks == doc->lBlocks)
            {
                updateTables(doc);
                calcTables();
            }
            return;
        }

        if(d && (row == 0 || row > curTable->nRows() - 2))
            return;
        updateFormulas(row, 0, true, false, false, curTable);
        index = tableCellIndex(curTable,row,0, l_blocks);
        for(int i = 0; i < curTable->nColums(); i++)
        {
            delete l_blocks->takeAt(index);
            if(l_blocks == doc->lBlocks)
                updateFragments(index,false);
            else
                curTable->m_counts--;

            while(l_blocks->at(index)->type != BASECELL)
            {
                delete l_blocks->takeAt(index);
                if(l_blocks == doc->lBlocks)
                    updateFragments(index,false);
                else
                    curTable->m_counts--;
            }
        }
        active_block = index;
        if(active_block < 0)
        {
            active_block = -1;
            m_startCursor = true;
        }
        sizeProp = curTable->lRows.takeLast();
        delete sizeProp;
        if(d && l_blocks == doc->lBlocks)
        {
            updateShopList(curTable);
        }

        if(curTable->lColums.isEmpty() || curTable->lRows.isEmpty())
        {
            if(l_blocks->at(curTable->startCell())->type == BASECELL)
            {
                delete l_blocks->takeAt(curTable->startCell());
                if(l_blocks == doc->lBlocks)
                {
                    updateFragments(curTable->startCell(),false);
                }
                l_blocks->takeAt(curTable->startCell());
            }
            if(l_blocks == doc->lBlocks)
            {
                activeIndex = curTable->m_index - 1;
                if(activeIndex < 0)
                {
                    activeIndex = -1;
                    m_startCursor = true;
                }
                updateTables(doc);
            }
            delete curTable;
        }

        if(l_blocks == doc->lBlocks)
            calcTables();


    }

}

void EjTextControl::delTableColum(QList<EjBlock *> *l_blocks, int &active_block)
{
	EjCellBlock *curCell;
	EjTableBlock *curTable = 0;
	EjTableFragment *curFragment;
    EjSizeProp *sizeProp;
	EjTableBlock::ColumProp *columProp;
    int row = 0;
    int colum = 0;
    int frgs_colum, frge_colum;
    int d = 0;
    int index;

    if(active_block < 0 || active_block > l_blocks->count() - 2)
        return;
    while(l_blocks->at(active_block)->type != BASECELL)
        active_block--;

	curCell = (EjCellBlock*)l_blocks->at(active_block);
	curTable = ((EjTableBlock*)(curCell->parent));

    if(curTable)
    {
		if(curTable->vid == EjTableBlock::SHOP_LIST)
            return;
        cellParams(curTable,active_block,row,colum, l_blocks);

        active_block -= row;
        updateFormulas(colum, 0, false, false, false, curTable);

        for(int i = 0; i < curTable->nRows(); i++)
        {
            index = tableCellIndex(curTable,i,colum - i, l_blocks);
            delete l_blocks->takeAt(index);
            if(l_blocks == doc->lBlocks)
                updateFragments(index,false);
            else
                curTable->m_counts--;

            while(l_blocks->at(index)->type != BASECELL)
            {
                delete l_blocks->takeAt(index);
                if(l_blocks == doc->lBlocks)
                    updateFragments(index,false);
                else
                    curTable->m_counts--;
            }
        }
        columProp = curTable->lColums.takeLast();
        delete columProp;
        if(curTable->lColums.isEmpty() || curTable->lRows.isEmpty())
        {
            doc->lTables->removeOne(curTable);
            l_blocks->removeOne(curTable);
            //            active_block = start - 2;
            active_block = curTable->m_index - 1;
            delete curTable;
            if(active_block < 0)
            {
                active_block = -1;
                m_startCursor = true;
            }
            if(l_blocks == doc->lBlocks)
                updateTables(doc);
        }

        if(l_blocks == doc->lBlocks)
            calcTables();
    }
}

void EjTextControl::moveTableString(bool isUp)
{
}

void EjTextControl::moveTableColum(bool isLeft)
{

}

void EjTextControl::updateFormulas(int fromVal, int toVal, bool isRow, bool isAdd,bool isMove, EjTableBlock *curTable)
{
    return;
    QStringList list;
	EjTableBlock *curTable2;
	EjCellBlock *curCell;
    int numRow;
    int numColum;
    int numTable;
    int index;
    bool isEdit;
    QString txt;
    bool isFormula;
    EjCalculator calculator(doc);
    for(int i_table = 0; i_table < doc->lTables->count(); i_table++)
    {
        curTable2 = doc->lTables->at(i_table);
        for(int i_row = 0; i_row < curTable2->nRows(); i_row++)
        {
            for(int i_colum = 0; i_colum < curTable2->nColums(); i_colum++)
            {
                index = tableCellIndex(curTable2,i_row,i_colum,doc->lBlocks);
                if(index < 0)
                    continue;
				curCell = static_cast<EjCellBlock*>(doc->lBlocks->at(index));
				if(curCell->vid == EjCellBlock::CELL_FORMULA)
                {
                    list.clear();
                    txt.clear();
                    QString formula = curCell->formula();
                    for(int i = 0; i < formula.count(); i++)
                    {

                        if(calculator.is_split(formula[i].toLatin1()) )
                        {
                            if(!txt.isEmpty())
                                list << txt;
                            txt = formula[i];
                            list << txt;
                            txt.clear();
                        }
                        else txt += formula[i];
                    }
                    if(!txt.isEmpty())
                        list << txt;
                    for(int i2 = 0; i2 < list.count(); i2++)
                    {
                        txt = list.at(i2);
                        isEdit = false;
                        if(txt == "SUMM" || txt == "MAX" || txt == "MIN")
                            continue;
                        isFormula = tableLinkParams(txt, curTable, curTable2, numTable, numRow, numColum);
                        if(isFormula && numTable == curTable->key)
                        {
                            if(isMove)
                            {
                                if(isRow)
                                {
                                    if(numRow == fromVal+1)
                                    {
                                        numRow = toVal+1;
                                        isEdit = true;
                                    }
                                    else if(numRow == toVal+1)
                                    {
                                        numRow = fromVal+1;
                                        isEdit = true;
                                    }

                                }
                                else
                                {
                                    if(numColum == fromVal)
                                    {
                                        numColum = toVal;
                                        isEdit = true;
                                    }
                                    else if(numColum == toVal)
                                    {
                                        numColum = fromVal;
                                        isEdit = true;
                                    }
                                }

                            }
                            else if(isRow)
                            {
                                if(numRow > fromVal)
                                {

                                    if(isAdd) numRow++;
                                    else
                                        numRow--;
                                    if(numRow < 0)
                                        numRow = 0;
                                    isEdit = true;
                                }
                            }
                            else
                            {
                                if(numColum > fromVal)
                                {

                                    if(isAdd) numColum++;
                                    else
                                        numColum--;
                                    if(numColum < 0)
                                        numColum = 0;
                                    isEdit = true;
                                }
                            }
                        }
                        if(isEdit)
                        {
                            txt = "";
                            if(curTable2 != curTable)
                                txt += QString::number(numTable);
							txt += QString::number(numColum) + 'A';
                            txt += QString::number(numRow);
                            list[i2] = txt;
                        }
                    }
                    txt = "";
                    for(int i = 0; i < list.count(); i++)
                    {
                        txt += list.at(i);
                    }
                    curCell->setFormula(txt);
                }

            }
        }

    }

}

bool EjTextControl::tableLinkParams(QString txt, EjTableBlock *curTable, EjTableBlock *curTable2, int &numTable, int &numRow, int &numColum)
{
    bool isFormula = true;
    if(txt.count() > 1 && curTable == curTable2 && is_value(txt.toLatin1().at(0)))
    {
        numTable = curTable->key;
        numRow = txt.right(txt.count()-1).toInt();
        numColum = txt.toLatin1().at(0) - 'A';
    }
    else if(txt.count() > 2 && is_value(txt.toLatin1().at(1)))
    {
        numTable = txt.left(1).toInt();
        numRow = txt.right(txt.count()-2).toInt();
        numColum = txt.toLatin1().at(1) - 'A';
    }
    else if(txt.count() > 3 && is_value(txt.toLatin1().at(2)))
    {
        numTable = txt.left(2).toInt();
        numRow = txt.right(txt.count()-3).toInt();
        numColum = txt.toLatin1().at(2) - 'A';
    }
    else
    {
        isFormula = false;
    }

    return isFormula;

}




void EjTextControl::updateShopList(EjTableBlock *curTableBlock)
{
    int index = curTableBlock->startCell();
	EjCellBlock *curCell;
    while(doc->lBlocks->at(index)->type != BASECELL)
        index++;
    index--;
    for(int row = 0; row < curTableBlock->nRows() - 1; row++)
    {
        index++;
        if(row > 0)
        {
			curCell = (EjCellBlock*)doc->lBlocks->at(index);
            curCell->value = row;
			curCell->vid = EjCellBlock::CELL_NUMBER;
        }
        index++;
        while(doc->lBlocks->at(index+1)->type != BASECELL)
            index++;
        if(row > 0)
        {
			curCell = (EjCellBlock*)doc->lBlocks->at(index);
			curCell->vid = EjCellBlock::CELL_CHECK;
        }
        // name
        index++;
        while(doc->lBlocks->at(index+1)->type != BASECELL)
            index++;
        // price
        index++;
        while(doc->lBlocks->at(index+1)->type != BASECELL)
            index++;
        if(row > 0)
        {
			curCell = (EjCellBlock*)doc->lBlocks->at(index);
			if(curCell->vid != EjCellBlock::CELL_NUMBER)
            {
				curCell->vid = EjCellBlock::CELL_NUMBER;
                curCell->setText("0");
                curCell->value = 0;
            }
        }
        // qty
        index++;
        while(doc->lBlocks->at(index+1)->type != BASECELL)
            index++;
        if(row > 0)
        {
			curCell = (EjCellBlock*)doc->lBlocks->at(index);
			if(curCell->vid != EjCellBlock::CELL_NUMBER)
            {
				curCell->vid = EjCellBlock::CELL_NUMBER;
                curCell->setText("1");
                curCell->value = 1;
            }
        }


        index++;
        while(doc->lBlocks->at(index+1)->type != BASECELL)
            index++;
        //        index++;
        if(row > 0)
        {
			curCell = (EjCellBlock*)doc->lBlocks->at(index);
            curCell->setFormula(QString("=D%1*E%1").arg(row+1));
			curCell->vid = EjCellBlock::CELL_FORMULA;
        }

    }
    index = tableCellIndex(curTableBlock,curTableBlock->nRows()-1,curTableBlock->nColums()-1, doc->lBlocks);
    curCell = NULL;
    if(index > -1)
		curCell = (EjCellBlock*)doc->lBlocks->at(index);
    if(curCell)
    {
        curCell->setFormula(QString("=SUMM(F2:F%1)").arg(curTableBlock->nRows()-1));
		curCell->vid = EjCellBlock::CELL_FORMULA;
    }
}

void EjTextControl::cellParams(EjTableBlock *TableBlock, int index, int &row, int &colum, QList<EjBlock *> *l_blocks)
{
    int table_index = -1;
    if(!l_blocks)
        l_blocks = doc->lBlocks;
    for(int i = TableBlock->startCell(); i <= TableBlock->endBlock(); i++ )
    {
        if(l_blocks->at(i)->type == BASECELL)
            table_index++;
        if(i == index)
        {
            if(TableBlock->nColums() > 0)
                row = (table_index) / TableBlock->nColums();
            colum = table_index - row*TableBlock->nColums();
            break;
        }
    }
}

void EjTextControl::cellParams(EjTableBlock *tableBlock, EjBlock *block, int &row, int &colum, QList<EjBlock *> *l_blocks)
{
    int table_index = -1;
    if(!l_blocks)
        l_blocks = doc->lBlocks;
    EjBlock *curBlock;
    for(int i = tableBlock->startCell(); i <= tableBlock->endBlock(); i++ )
    {
        curBlock = l_blocks->at(i);
        if(curBlock->type == BASECELL)
            table_index++;
        if(curBlock == block)
        {
            if(tableBlock->nColums() > 0)
                row = (table_index) / tableBlock->nColums();
            colum = table_index - row * tableBlock->nColums();
            break;
        }
    }
}


int EjTextControl::tableCellIndex(EjTableBlock *tableBlock, int row, int colum, QList<EjBlock *> *l_blocks)
{
    int table_index = -1;
    int res = -1;
    int index = row * tableBlock->nColums() + colum;

    if(!l_blocks)
        return res;
    int i = tableBlock->startCell();
    while(l_blocks->at(i)->type != BASECELL)
        i++;
    for(i; i <= tableBlock->endBlock(); i++ )
    {
        if(l_blocks->at(i)->type == BASECELL)
        {
            table_index++;
            if(table_index >= index)
            {
                res = i;
                break;
            }
        }
    }
    return res;

}

void EjTextControl::updateTables(EjDocument *fdoc)
{
	EjCalcParams calcParams;
	EjTableBlock *curTable = 0;
	EjPropIntBlock *propInt;
    EjBlock *curBlock;
	EjCellBlock *curCell;
    QString text;
    bool bOk;

    fdoc->lTables->clear();
    fdoc->lLabels_bak->clear();
    fdoc->lLabels->clear();
    for(int i = 0; i < fdoc->lBlocks->count(); i++)
    {
        curBlock = fdoc->lBlocks->at(i);
        if(curBlock->type == EXT_TABLE)
        {

			curTable = dynamic_cast<EjTableBlock*>(curBlock);
            curTable->m_index = i;
            curTable->m_doc = fdoc;
            if(i < fdoc->lBlocks->count() - 1)
                i++;
            else
                break;
            fdoc->lTables->append(curTable);
            while(i < fdoc->lBlocks->count() - 1 && curBlock->type != BASECELL && curBlock->type != END_GROUP) {
                curBlock = fdoc->lBlocks->at(i);
                curBlock->parent = curTable;
                if(curBlock->type == PROP_INT) {
					propInt = dynamic_cast<EjPropIntBlock*>(curBlock);
					if(propInt->num == EjTableBlock::TBL_ROWS)
                    {
                        while(curTable->lRows.count() > propInt->value)
                            delete curTable->lRows.takeLast();
                        while(curTable->lRows.count() < propInt->value)
                            curTable->lRows.append(new EjSizeProp());
                    }
					if(propInt->num == EjTableBlock::TBL_COLUMS)
                    {
                        while(curTable->lColums.count() > propInt->value)
                            delete curTable->lColums.takeLast();
                        while(curTable->lColums.count() < propInt->value)
							curTable->lColums.append(new EjTableBlock::ColumProp());
                    }
                }
                if(curBlock->type == PROP_PNT) {
					EjPropPntBlock *propPnt = dynamic_cast<EjPropPntBlock*>(curBlock);
                    if(propPnt && propPnt->y_value < curTable->lColums.count())
                    {
						if(propPnt->num == EjTableBlock::TBL_COLUM_MAX_WIDTH)
                            curTable->lColums.at(propPnt->y_value)->sizeProp.max = propPnt->x_value;
						if(propPnt->num == EjTableBlock::TBL_COLUM_MIN_WIDTH)
                            curTable->lColums.at(propPnt->y_value)->sizeProp.min = propPnt->x_value;
                    }
                }

                i++;
            }
            i--;
            curTable->deltaProps = i - curTable->m_index;
            curBlock = fdoc->lBlocks->at(i);
            while(i < fdoc->lBlocks->count() - 1 && curBlock->type != END_GROUP) {
                curBlock = fdoc->lBlocks->at(i);
                curBlock->parent = curTable;
                if(curBlock->type >= GROUP_BLOCK)
                {
                    EjGroupBlock *groupBlock = dynamic_cast<EjGroupBlock*>(curBlock);
                    if(groupBlock)
                    {
                        groupBlock->calcLenght(i,fdoc->lBlocks);
                        i += groupBlock->m_counts;
                    }
                }
                if(curBlock->type == BASECELL)
                {
					curCell = dynamic_cast<EjCellBlock*>(curBlock);
					if(curCell->vid == EjCellBlock::CELL_AUTO || curCell->vid == EjCellBlock::CELL_NUMBER)
                    {
                        if(!curCell->text.isEmpty() && curCell->text.at(0).toLatin1() != '=')
                            curCell->value = getDValue(curCell->text, &bOk);
						else if(curCell->vid == EjCellBlock::CELL_AUTO)
                        {
                            text = curCell->getText();
                            curCell->value = getDValue(text, &bOk);
                        }
                    }
                }
                i++;
            }
            curTable->m_counts = i - curTable->m_index;
            if(curTable->m_counts < 0)
                curTable->m_counts = 0;
        }
        if(fdoc->lBlocks->at(i)->type == EXT_LARGETEXT_BAK)
        {
            fdoc->lLabels_bak->append((LargeTextBlock*)fdoc->lBlocks->at(i));
        }
        if(fdoc->lBlocks->at(i)->type == EXT_LABEL)
        {
            LabelBlock *label = (LabelBlock*)fdoc->lBlocks->at(i);
            fdoc->lLabels->append(label);
        }
    }
}

EjTableBlock *EjTextControl::isTable(int index)
{
	EjTableBlock *res = 0;
	foreach(EjTableBlock *curTable, *doc->lTables)
    {
        if(index >= curTable->m_index && index <= curTable->endBlock())
        {
            res = curTable;
            break;
        }
    }
    return res;
}

void EjTextControl::checkFormula()
{
    if(activeIndex < 0 || doc->lBlocks->at(activeIndex)->type != BASECELL)
        return;
	EjCellBlock *curCell = dynamic_cast<EjCellBlock*>(doc->lBlocks->at(activeIndex));
    QString text = curCell->getText();
    if(text.isEmpty())
    {
        text = "=";
        position = 1;
    }
    else if(text[0] != '=')
    {
        text.insert(0,"=");
        position++;
    }
    curCell->setText(text);
}

bool EjTextControl::isNumber()
{
    if(activeIndex < 0 || doc->lBlocks->at(activeIndex)->type != BASECELL)
        return false;
	EjCellBlock *curBlock = (EjCellBlock*)doc->lBlocks->at(activeIndex);
	if(((EjTableBlock*)(curBlock->parent))->vid == EjTableBlock::SHOP_LIST)
    {
		EjTableBlock *curTable = (EjTableBlock*)curBlock->parent;
        int row;   // = (activeBlock - index) / curTable->nColums();
        int colum; // = activeBlock - index - row * curTable->nColums();
        cellParams(curTable,activeIndex,row,colum);
        if(colum > 2)
            return true;
    }
    return false;
}

bool EjTextControl::isActiveText()
{
    bool res = false;
    if(activeIndex >= 0 && activeIndex < doc->lBlocks->count() && doc->lBlocks->at(activeIndex)->type == TEXT)
        res = true;
    return res;
}

bool EjTextControl::isCellEditFormula()
{
    if(activeIndex < 0 || doc->lBlocks->at(activeIndex)->type != BASECELL)
        return false;
	EjCellBlock *curBlock = (EjCellBlock*)doc->lBlocks->at(activeIndex);
	if(((EjTableBlock*)(curBlock->parent))->vid != EjTableBlock::SHOP_LIST && curBlock->vid == EjCellBlock::CELL_FORMULA)
        return true;
    return false;

}

bool EjTextControl::isCellSelected(int index)
{
    bool res = false;
    int row_index, colum_index;
    int row_start, colum_start;
    int row_end, colum_end;
    int n;
	EjTableBlock *startTable;
	EjTableBlock *endTable;
    startTable = isTable(m_startSelectBlock);
    endTable = isTable(m_endSelectBlock);
    if(startTable && endTable && startTable == endTable)
    {
        cellParams(startTable, index, row_index, colum_index);
        cellParams(startTable, m_startSelectBlock, row_start, colum_start);
        cellParams(startTable, m_endSelectBlock, row_end, colum_end);
        if(row_start > row_end)
        {
            n = row_start;
            row_start = row_end;
            row_end = n;
        }
        if(colum_start > colum_end)
        {
            n = colum_start;
            colum_start = colum_end;
            colum_end = n;
        }
        if( row_index >= row_start && row_index <= row_end
                && colum_index >= colum_start && colum_index <= colum_end)
            res = true;
    }
    else  if(index >= m_startSelectBlock && index <= m_endSelectBlock)
        res = true;
    return res;
}

bool EjTextControl::getBaseCellParams(int index, int &row, int &colum)
{
    bool res = false;
	EjTableBlock *curTable = isTable(index);
    if(curTable)
    {
        cellParams(curTable,index,row,colum);
        res = true;
    }
    return res;
}

int EjTextControl::getIndexString(int index)
{
    int index_string = 0;
    for(int i = 0; i < doc->lStrings->count(); i++)
    {
        if(index >= doc->lStrings->at(i)->startBlock) {
            index_string = i;
        }
        else
            break;
    }
    return index_string;
}

int EjTextControl::tableVid()
{
    int res = -1;
	EjTableBlock *table = isTable(activeIndex);
    if(table)
        res = table->vid;
    return res;
}

void EjTextControl::loadLinks()
{
}

void EjTextControl::signLinks(qint32 sign_id,qint32 group_id, int status, QString comment)
{
}

void EjTextControl::updateLinks()
{
  EjLinkProp *curLink;
  EjCalculator calculator(doc);

  if(!doc->m_attrProp)
      return;
  for(int i = 0; i < doc->m_attrProp->m_lLinks.count(); i++) {
      curLink = doc->m_attrProp->m_lLinks.at(i);
      if(curLink->m_extDoc)
          updateTables(curLink->m_extDoc);
  }
  calculator.calcTables(true);
  calculator.m_doc = doc;
  calculator.updateFormulas(true);

}


void EjTextControl::setIsViewDoc(bool source)
{
    m_isViewDoc = source;
    qDeleteAll(*doc->lStrings);
    doc->lStrings->clear();
    qDeleteAll(*doc->lPages);
    doc->lPages->clear();
}

void EjTextControl::setDocument(EjDocument *document)
{
	EjCalcParams calcParams;
    LabelBlock *curLabel;
    int index;

    doc = document;
    EjCalculator calculator(doc);

    if(docPrev)
    {
        delete docPrev;
        docPrev = nullptr;
    }
    qDeleteAll(m_lPatches);
    m_lPatches.clear();


    if(activeIndex > doc->lBlocks->count() - 1)
    {
        activeIndex = doc->lBlocks->count() - 1;
        position = 0;
    }
    if(activeIndex > -1 && (doc->lBlocks->at(activeIndex)->type == TEXT || doc->lBlocks->at(activeIndex)->type == BASECELL))
    {
		int len = ((EjTextBlock*)(doc->lBlocks->at(activeIndex)))->text.length();
        if(position > len)
            position = len;
    }
    else
        position = 0;
    m_startSelectBlock = -1;
    m_endSelectBlock = -1;
    m_startSelectPos = 0;
    m_endSelectPos = 0;
    m_calcIndex = 0;
    updateTables(doc);

    calcParams.textStyle = getTextStyle(0);
    calcParams.control = this;
    calcParams.contentX = m_contentX;
    calcParams.contentY = m_contentY;
    calcParams.viewScale = scaleSize;
    calcParams.statusMode = m_statusMode;
    if(doc->lLabels->count() > 0)
    {
        index = 0;
        doc->lBlocks->at(0)->calcBlock(index, &calcParams);
    }
    for(int i = 0; i < doc->lLabels->count(); i++)
    {
        curLabel = doc->lLabels->at(i);
        index = doc->lBlocks->indexOf(curLabel);
        curLabel->calcBlock(index, &calcParams);
    }
    calcNext();
    calcCursor();
}

bool EjTextControl::pasteCells()
{

    int index = activeIndex;
    EjBlock* curBlock;

    int count_cells = 0;

    for (int i = 0; i < m_clipboardDoc->lBlocks->count(); i++)
    {
        if (m_clipboardDoc->lBlocks->at(i)->type == e_typeBlocks::BASECELL)
        {
            count_cells++;
        }
    }
    int temp_index = 1;
    for (int i = 0; i < count_cells; i++)
    {
        while (doc->lBlocks->at(index)->type != e_typeBlocks::BASECELL && doc->lBlocks->at(index)->type != e_typeBlocks::END_GROUP)
        {
           doc->lBlocks->removeAt(index);
        }


        for (int i = temp_index; i < m_clipboardDoc->lBlocks->count() && m_clipboardDoc->lBlocks->at(i)->type != e_typeBlocks::BASECELL ; i++)
        {
            doc->lBlocks->insert(index, m_clipboardDoc->lBlocks->at(i));
            index++;
            temp_index++;
        }
        index++;
        temp_index++;
    }


    return true;
}

bool EjTextControl::copyCells()
{
    if(m_clipboardDoc)
        delete m_clipboardDoc;
	m_clipboardDoc = new EjDocument();
    EjBlock* curBlock;
    for(int i = 0; i < doc->lPropBlocks->count(); i++)
    {
        curBlock = doc->lPropBlocks->at(i);
        curBlock = curBlock->makeCopy();
        m_clipboardDoc->lPropBlocks->append(curBlock);
    }
    m_clipboardDoc->calcProps();
    int startBlock = m_startSelectBlock;
    int endBlock = m_endSelectBlock;
    if (m_startSelectBlock > -1 && m_endSelectBlock > -1)
    {

        if (doc->lBlocks->at(endBlock + 1)->type != e_typeBlocks::BASECELL)
        {
           while (doc->lBlocks->at(endBlock + 1)->type != e_typeBlocks::BASECELL && doc->lBlocks->at(endBlock + 1)->type != e_typeBlocks::END_GROUP)
           {
                endBlock++;
           }
        }

        for (int i = startBlock; i <= endBlock; i++)
        {
            curBlock = doc->lBlocks->at(i)->makeCopy();
            m_clipboardDoc->lBlocks->append(curBlock);
        }
    }

    return true;
}

bool EjTextControl::menuActivate(QString command, QString data)
{
	EjFragmentBlock *fragment;
    Param param;
    EjBlock *cur_Block;
	EjTextBlock *cur_textBlock;
    bool res = true;
    int n_blockEnd = m_endSelectBlock;
    int n_pos = m_startSelectPos;
    int n_blockStart = m_startSelectBlock;
    QClipboard *clipboard = QGuiApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    QString str;


    if(m_endSelectBlock > 0 && doc->lBlocks->at(m_endSelectBlock)->type == TEXT && m_endSelectPos == 0){
        m_endSelectBlock--;
        if(doc->lBlocks->at(m_endSelectBlock)->type == TEXT)
			m_endSelectPos = static_cast<EjTextBlock*>(doc->lBlocks->at(m_endSelectBlock))->text.size();

    }

	fragment = new EjFragmentBlock();
    if(command == "B" || command == "I" || command == "U" || command == "dB" || command == "dI" || command == "dU" || command == "Cut" || command == "Del")
    {

    }
    if(command == "B")
    {
		EjTextStyle *textStyle = dynamic_cast<EjTextStyle*>(getSelectedTextStyle(activeIndex)->makeCopy());
		textStyle->setFontBold(true);
		setTextStyle(textStyle,false,true);
        delete textStyle;
    }
    else if(command == "I")
    {
		EjTextStyle *textStyle = dynamic_cast<EjTextStyle*>(getSelectedTextStyle(activeIndex)->makeCopy());
		textStyle->setFontItalic(true);
		setTextStyle(textStyle,false,true);
        delete textStyle;
    }
    else if(command == "U")
    {
		EjTextStyle *textStyle = dynamic_cast<EjTextStyle*>(getSelectedTextStyle(activeIndex)->makeCopy());
		textStyle->setFontUnderline(true);
		setTextStyle(textStyle,false,true);
        delete textStyle;
    }
    else if(command == "dB")
    {
		EjTextStyle *textStyle = dynamic_cast<EjTextStyle*>(getSelectedTextStyle(activeIndex)->makeCopy());
		textStyle->setFontBold(false);
		setTextStyle(textStyle,false,true);
        delete textStyle;
    }
    else if(command == "dI")
    {
		EjTextStyle *textStyle = dynamic_cast<EjTextStyle*>(getSelectedTextStyle(activeIndex)->makeCopy());
		textStyle->setFontItalic(false);
		setTextStyle(textStyle,false,true);
        delete textStyle;
    }
    else if(command == "dU")
    {
		EjTextStyle *textStyle = dynamic_cast<EjTextStyle*>(getSelectedTextStyle(activeIndex)->makeCopy());
		textStyle->setFontUnderline(false);
		setTextStyle(textStyle,false,true);
		delete textStyle;
    }
    else if(command == "FontStyle" && data != "")
    {
		EjTextStyle *textStyle = dynamic_cast<EjTextStyle*>(getSelectedTextStyle(activeIndex)->makeCopy());
        int fontStyle = data.toInt();
        if(textStyle->fontStyle() != fontStyle)
        {
			textStyle->setFontStyle(EjTextStyle::e_fontStyle(fontStyle));
            if(fontStyle == 0) {
                textStyle->setFontSize(12); textStyle->setFontBold(false); textStyle->setFontItalic(false); textStyle->setFontUnderline(false); textStyle->setFontStrikeOut(false);
            }
            if(fontStyle == 1) {
                textStyle->setFontSize(10); textStyle->setFontBold(false); textStyle->setFontItalic(false); textStyle->setFontUnderline(false); textStyle->setFontStrikeOut(false);
            }
            if(fontStyle == 2) {
                textStyle->setFontSize(8); textStyle->setFontBold(false); textStyle->setFontItalic(false); textStyle->setFontUnderline(false); textStyle->setFontStrikeOut(false);
            }
            if(fontStyle == 3) {
                textStyle->setFontSize(16); textStyle->setFontBold(false); textStyle->setFontItalic(false); textStyle->setFontUnderline(false); textStyle->setFontStrikeOut(false);
            }
            if(fontStyle == 4) {
                textStyle->setFontSize(18); textStyle->setFontBold(false); textStyle->setFontItalic(false); textStyle->setFontUnderline(false); textStyle->setFontStrikeOut(false);
            }
			if(fontStyle == 10) {
                textStyle->setFontSize(20); textStyle->setFontBold(true); textStyle->setFontItalic(false); textStyle->setFontUnderline(false); textStyle->setFontStrikeOut(false);
            }
            if(fontStyle == 11) {
                textStyle->setFontSize(18); textStyle->setFontBold(true); textStyle->setFontItalic(false); textStyle->setFontUnderline(false); textStyle->setFontStrikeOut(false);
            }
            if(fontStyle == 12) {
                textStyle->setFontSize(14); textStyle->setFontBold(true); textStyle->setFontItalic(false); textStyle->setFontUnderline(false); textStyle->setFontStrikeOut(false);
            }
            if(fontStyle == 13) {
                textStyle->setFontSize(13); textStyle->setFontBold(true); textStyle->setFontItalic(true); textStyle->setFontUnderline(false); textStyle->setFontStrikeOut(false);
            }
            if(fontStyle == 14) {
                textStyle->setFontSize(13); textStyle->setFontBold(true); textStyle->setFontItalic(true); textStyle->setFontUnderline(true); textStyle->setFontStrikeOut(false);
            }

            setTextStyle(textStyle,false,true);
        }
        else
            res = false;
        delete textStyle;
    }
    else if(command == "TextColor" && data != "")
    {
		EjTextStyle *textStyle = dynamic_cast<EjTextStyle*>(getSelectedTextStyle(activeIndex)->makeCopy());
        QColor textColor = QColor(data);
        if(textStyle->fontColor() != textColor)
        {
            textStyle->setFontColor(textColor);
            setTextStyle(textStyle,false,true);
        }
        delete textStyle;
    }
    else if(command == "FillColor" && data != "")
    {
		EjTextStyle *textStyle = dynamic_cast<EjTextStyle*>(getSelectedTextStyle(activeIndex)->makeCopy());
        QColor fillColor = QColor(data);
        if(textStyle->brushColor() != fillColor)
        {
            textStyle->setBrushColor(fillColor);
            setTextStyle(textStyle,false,true);
        }
        delete textStyle;
    }
    else if(command == "AlignLeft")
    {
		EjParagraphStyle *prgStyle = dynamic_cast<EjParagraphStyle*>(getParagraphStyle(activeIndex)->makeCopy());
        int align = prgStyle->align();
		if(!(align & EjParagraphStyle::AlignLeft))
        {
			align |= EjParagraphStyle::AlignLeft;
			align &= ~EjParagraphStyle::AlignHCenter;
			align &= ~EjParagraphStyle::AlignRight;
            prgStyle->setAlign(align);
            setParagraphStyle(prgStyle);
        }
        else
            res = false;
        delete prgStyle;
    }
    else if(command == "AlignHCenter")
    {
		EjParagraphStyle *prgStyle = dynamic_cast<EjParagraphStyle*>(getParagraphStyle(activeIndex)->makeCopy());
        int align = prgStyle->align();
		if(!(align & EjParagraphStyle::AlignHCenter))
        {
			align &= ~EjParagraphStyle::AlignLeft;
			align |= EjParagraphStyle::AlignHCenter;
			align &= ~EjParagraphStyle::AlignRight;
            prgStyle->setAlign(align);
            setParagraphStyle(prgStyle);
        }
        else
            res = false;
        delete prgStyle;
    }
    else if(command == "AlignRight")
    {
		EjParagraphStyle *prgStyle = dynamic_cast<EjParagraphStyle*>(getParagraphStyle(activeIndex)->makeCopy());
        int align = prgStyle->align();
		if(!(align & EjParagraphStyle::AlignRight))
        {
			align &= ~EjParagraphStyle::AlignLeft;
			align &= ~EjParagraphStyle::AlignHCenter;
			align |= EjParagraphStyle::AlignRight;
            prgStyle->setAlign(align);
            setParagraphStyle(prgStyle);
        }
        else
            res = false;
        delete prgStyle;
    }
    else if(command == "Call")
    {
        QString tel = isTell();
        if(tel.count() > 3)
            emit ring(tel);
    }
    else if(command == "Copy" || command == "Cut" || command == "Del")
    {
        int startSelectBlock = m_startSelectBlock;
        int endSelectBlock = m_endSelectBlock;
        if(startSelectBlock == -1)
            startSelectBlock = activeIndex;
        if(endSelectBlock == -1)
            endSelectBlock = activeIndex;
        if(startSelectBlock == -1 || endSelectBlock == -1)
            return false;
        if(doc->lBlocks->at(endSelectBlock)->type >= GROUP_BLOCK)
        {
            EjGroupBlock *block = dynamic_cast<EjGroupBlock*>(doc->lBlocks->at(endSelectBlock));
            if(block)
            {
                endSelectBlock = block->m_index + block->m_counts;
            }
        }
        if(command != "Del")
        {
            bool res = false;
            int row_index, colum_index;
            int row_start, colum_start;
            int row_end, colum_end;
			EjCellBlock *curCell;
			EjTableBlock *curTable;
            EjBlock *curBlock;
			EjTableFragment *curFragment;
            int indexStart;
            int indexEnd;
            int startSelect = startSelectBlock;
            int endSelect = endSelectBlock;

            if(m_clipboardDoc)
                delete m_clipboardDoc;
			m_clipboardDoc = new EjDocument();
            for(int i = 0; i < doc->lPropBlocks->count(); i++)
            {
                curBlock = doc->lPropBlocks->at(i);
                curBlock = curBlock->makeCopy();
                m_clipboardDoc->lPropBlocks->append(curBlock);
            }
            m_clipboardDoc->calcProps();
            curTable = isTable(startSelectBlock);
            if(curTable && curTable == isTable(endSelectBlock))
            {
                getBaseCellParams(startSelectBlock, row_start, colum_start);
                getBaseCellParams(endSelectBlock, row_end, colum_end);
                if(row_start != row_end || colum_start != colum_end || startSelect == endSelect)
                {
                    while(doc->lBlocks->at(startSelect)->type != BASECELL)
                        startSelect--;
                    while(doc->lBlocks->at(endSelect+1)->type != END_GROUP && doc->lBlocks->at(endSelect+1)->type != BASECELL)
                        endSelect++;
                }
            }
            for(int i = startSelect; i <= endSelect; i++)
            {
                if(doc->lBlocks->at(i)->type == EXT_TABLE)
                {
					curTable = (EjTableBlock *)doc->lBlocks->at(i)->makeCopy();
                    curTable->m_doc = m_clipboardDoc;

                    m_clipboardDoc->lBlocks->append(curTable);
                }
                else if(doc->lBlocks->at(i)->type == BASECELL)
                {
					curCell = (EjCellBlock*)doc->lBlocks->at(i);
					indexStart = ((EjTableBlock*)(curCell->parent))->m_index + 1;
					indexEnd = ((EjTableBlock*)(curCell->parent))->endBlock();


					curTable = (EjTableBlock*)curTable->makeCopy();
                    curTable->m_doc = m_clipboardDoc;
                    m_clipboardDoc->lBlocks->append(curTable);
                    curTable->m_index = m_clipboardDoc->lBlocks->indexOf(curTable);
                    curTable->m_counts = indexEnd - indexStart + 1;
                    if(curTable->m_counts < 0)
                        curTable->m_counts = 0;

					EjCellBlock *cell_block;
                    for(int j = indexStart; j <= indexEnd; j++)
                    {
                        curBlock = doc->lBlocks->at(j)->makeCopy();

                        if(curBlock->type == BASECELL)
                        {
							cell_block = (EjCellBlock *)curBlock;
							((EjCellBlock *)curBlock)->parent = curTable;
							int index = doc->lPropBlocks->indexOf(((EjCellBlock *)doc->lBlocks->at(j))->cellStyle);
							((EjCellBlock *)curBlock)->cellStyle = (EjCellStyle *)(m_clipboardDoc->lPropBlocks->at(index));
						}
                        m_clipboardDoc->lBlocks->append(curBlock);
                        i = j;
                    }
                    if(startSelect > indexStart)
                        indexStart = startSelectBlock;
                    if(endSelect < indexEnd)
                        indexEnd = endSelectBlock;
                    getBaseCellParams(indexStart, row_start, colum_start);
                    getBaseCellParams(indexEnd, row_end, colum_end);
                    curTable->start_column = colum_start;
                    curTable->start_row = row_start;
					if(curTable->vid == EjTableBlock::SHOP_LIST)
                    {
                        colum_start = 0;
						colum_end = ((EjTableBlock*)(curCell->parent))->nColums();
                    }
					row_end = ((EjTableBlock*)(curCell->parent))->nRows() - row_end - 1;
                    if(row_end < 0)
                        row_end = 0;
					colum_end = ((EjTableBlock*)(curCell->parent))->nColums() - colum_end - 1;
                    if(colum_end < 0)
                        colum_end = 0;
                    indexStart = curTable->cellIndex(0,0,m_clipboardDoc->lBlocks);
					if(curTable->vid == EjTableBlock::SHOP_LIST)
                    {
                        row_start--;
                        if(row_start < 0)
                            row_start = 0;
                        row_end--;
                        if(row_end < 0)
                            row_end = 0;
                        indexStart += curTable->nColums();
                    }
                    for(int j = 0; j < row_start; j++)
                    {
                        curTable->delString(m_clipboardDoc->lBlocks,indexStart);
                    }
                    for(int j = 0; j < row_end; j++)
                    {
                        indexEnd = m_clipboardDoc->lBlocks->count() - 2;
						if(curTable->vid == EjTableBlock::SHOP_LIST)
                            indexEnd -= curTable->nColums();
                        curTable->delString(m_clipboardDoc->lBlocks,indexEnd);
                    }
                    for(int j = 0; j < colum_start; j++)
                    {
                        curTable->delColum(m_clipboardDoc->lBlocks,indexStart);
                    }
                    for(int j = 0; j < colum_end; j++)
                    {
                        indexEnd = m_clipboardDoc->lBlocks->count() - 2;
						if(curTable->vid == EjTableBlock::SHOP_LIST)
                            indexEnd -= curTable->nColums();
                        curTable->delColum(m_clipboardDoc->lBlocks,indexEnd);
                    }
                }
                else if(doc->lBlocks->at(i)->type != EXT_TABLE)
                    m_clipboardDoc->lBlocks->append(doc->lBlocks->at(i)->makeCopy());
            }
            if(m_startSelectPos > 0 && m_clipboardDoc->lBlocks->count() > 0 && m_clipboardDoc->lBlocks->at(0)->type == TEXT)
            {
				cur_textBlock = (EjTextBlock*)m_clipboardDoc->lBlocks->at(0);
                cur_textBlock->text = cur_textBlock->text.right(cur_textBlock->text.size() - m_startSelectPos);
            }
            if(m_endSelectPos > 0 && m_clipboardDoc->lBlocks->count() > 0 && m_clipboardDoc->lBlocks->at(m_clipboardDoc->lBlocks->count()-1)->type == TEXT)
            {
				cur_textBlock = (EjTextBlock*)m_clipboardDoc->lBlocks->at(m_clipboardDoc->lBlocks->count()-1);
                cur_textBlock->text = cur_textBlock->text.left(m_endSelectPos);
            }
            str.clear();
            curTable = 0;
            for(int i = 0; i < m_clipboardDoc->lBlocks->count(); i++)
            {
                switch (m_clipboardDoc->lBlocks->at(i)->type) {
                case EXT_IMAGE:
                    str += " Image ";
                    break;
                case TEXT:
					str += static_cast<EjTextBlock*>(m_clipboardDoc->lBlocks->at(i))->text;
                    break;
                case EXT_TABLE:
					curTable = (EjTableBlock*)m_clipboardDoc->lBlocks->at(i);
                    break;
                case BASECELL:
                    if(curTable)
                    {
						EjCellBlock *curBlock = (EjCellBlock*)m_clipboardDoc->lBlocks->at(i);
                        int row;
                        int colum;
                        cellParams(curTable,i,row,colum,m_clipboardDoc->lBlocks);
						if(colum > 0 || curBlock->vid == EjCellBlock::CELL_CHECK)
                            str += QChar(QChar::Tabulation);
                        str += curBlock->getText();
                        while(i < m_clipboardDoc->lBlocks->count() - 1 && m_clipboardDoc->lBlocks->at(i + 1)->type != BASECELL)
                        {
                            i++;
                            if(m_clipboardDoc->lBlocks->at(i)->type == SPACE)
                                str += ' ';
                            else if(m_clipboardDoc->lBlocks->at(i)->type == TEXT)
								str += static_cast<EjTextBlock*>(m_clipboardDoc->lBlocks->at(i))->text;
                        }
                        if(colum == curTable->nColums() - 1)
                            str += "\r\n";
                    }
                    break;

                case SPACE:
                    str += ' ';
                    break;
                case ENTER:
                    str += "\r\n";
                default:
                    break;
                }
            }
            QMimeData mimeData;
            clipboard->setText(str);

            m_htmlBuffer = str;
            if(command == "Call")
            {
                QString tel;
                for (int i = 0; i < str.size(); ++i) {
                    if ((str.at(i) >= QChar('0') && str.at(i) <= QChar('9')) || str.at(i) == QChar('-') || str.at(i) == QChar('+'))
                        tel += str.at(i);
                    if(tel.count() > 50)
                        break;
                }
                if(tel.count() > 3)
                    emit ring(tel);
            }
            str.clear();
            str += QString(" <!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"> \n"
                           " <html> \n"
                           " <head> \n"
                           " <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"> \n"
                           " <meta http-equiv=\"Content-Style-Type\" content=\"text/css\"> \n"
                           " <title></title> \n"
                           " <meta name=\"Generator\" content=\"Jotter\"> \n"
                           " <meta name=\"JotterVersion\" content=\"1265.21\"> \n"
                           " <style type=\"text/css\"> \n"
                           " p { "
                           " font-size: 12.0px; font-family: Arial; color: black "
                           " } \n"
                           " table { "
                           " border-style: solid; "
                           " border-collapse: collapse "
                           " } \n"
                           " td { "
                           //                           " background-color: #bfbfbf; "
                           " border-style: solid; "
                           " border-width: 1.0px; "
                           " border-color: #323333; padding: 4.0px "
                           " } \n"
                           " </style> \n"
                           " </head> \n"
                           " <body> \n"
                           " <p> \n" );
            bool isTable = false;
            for(int i = 0; i < m_clipboardDoc->lBlocks->count(); i++)
            {
                if(isTable && !(m_clipboardDoc->lBlocks->at(i)->type == BASECELL ) )
                {
                    str += " </table> \n";
                    str += " <p> \n";
                    isTable = false;
                }
                switch (m_clipboardDoc->lBlocks->at(i)->type) {
                case TEXT:
					str += static_cast<EjTextBlock*>(m_clipboardDoc->lBlocks->at(i))->text;
                    break;
                case EXT_TABLE:
                    str += " </p> \n";
                    str += " <table cellspacing=\"0\" cellpadding=\"4\" > \n";
                    str += " <tr> \n";
					curTable = (EjTableBlock*)m_clipboardDoc->lBlocks->at(i);
                    isTable = true;
                    break;
                case BASECELL:
                    if(curTable)
                    {
						EjCellBlock *curBlock = (EjCellBlock*)m_clipboardDoc->lBlocks->at(i);
                        int row;
                        int colum;
                        cellParams(curTable,i,row,colum,m_clipboardDoc->lBlocks);

                        str += "<td><p>";
                        while(i < m_clipboardDoc->lBlocks->count() - 1 &&  m_clipboardDoc->lBlocks->at(i + 1)->type != BASECELL)
                        {
                            i++;
                            if(m_clipboardDoc->lBlocks->at(i)->type == SPACE)
                                str += ' ';
                            else if(m_clipboardDoc->lBlocks->at(i)->type == TEXT)
								str += static_cast<EjTextBlock*>(m_clipboardDoc->lBlocks->at(i))->text;
                        }
                        str += "</p></td>";

                        if(colum == curTable->nColums() - 1)
                        {
                            str += "\n </tr> \n ";
                            if(row < curTable->nRows() -1)
                                str += "<tr> \n";
                        }
                    }
                    break;

                case SPACE:
                    str += ' ';
                    break;
                case ENTER:
                    str += " <br> \n";
                default:
                    break;
                }
            }

            if(isTable)
            {
                str += " </table> \n";
            }
            else
                str += " </p> \n";
			str += " </body> \n  </html> ";
        }
        res = false;
        if(command == "Cut")
        {
            if(m_startSelectBlock > -1 || m_endSelectBlock > -1)
            {
                inputBackSpace();
                calc(0);
            }
        }
        else if(command == "Del")
        {
            inputBackSpace();
            calc(0);
        }
    }
    else if(command == "Past")
    {
        if(activeIndex > -1 && doc->lBlocks->at(activeIndex)->type == EXT_TABLE)
            return false;
        m_createPatchEnabled = false;
		EjCellBlock *curCell;
        int count;
        e_statusMode statusMode_bak;
        QString txt;
        QStringList list;
        int  numTable, numRow, numColum;
        bool isFormula;
        EjCalculator calculator(doc);


        if(activeIndex > -1 && isTable(activeIndex))
		{
            if (m_clipboardDoc->lBlocks->at(0)->type != EXT_TABLE && m_clipboardDoc->lBlocks->size() > 1)
            {
                return false;
            }

            int index = activeIndex;
            while(doc->lBlocks->at(index)->type != BASECELL)
                index--;
			curCell = (EjCellBlock*)doc->lBlocks->at(index);
			curCell->vid = EjCellBlock::CELL_AUTO;
            statusMode_bak = m_statusMode;
            m_statusMode = EDIT_CELL;
            if(mimeData && mimeData->text() == m_htmlBuffer)
            {
                for(int i = 0; i < m_clipboardDoc->lBlocks->count(); i++)
                {
                    cur_Block = m_clipboardDoc->lBlocks->at(i);
                    if(cur_Block->type == EXT_TABLE)
                    {
						EjTableBlock *curTable = (EjTableBlock*)m_clipboardDoc->lBlocks->at(i);
						EjTableBlock *curActiveTable = ((EjTableBlock*)(curCell->parent));
						EjCellBlock *curCell2;
						EjTableFragment *curFragment = NULL;
                        int active_row, active_colum;
                        int activeBlockStart;
                        int nColums = 0;
                        int nRows = 0;
                        int startRow = 0, endRow = 0, startColum = 0, endColum = 0;
						if(curTable->vid == EjTableBlock::SHOP_LIST)
                        {
                        }
                        else
                        {
                            nColums = curTable->nColums();
                            nRows = curTable->nRows();
                            startRow = startColum = 0;
                            endRow = nRows - 1;
                            endColum = nColums - 1;
                        }
                        getBaseCellParams(activeIndex, active_row, active_colum);
                        while(active_colum + nColums > curActiveTable->nColums())
                        {
                            curActiveTable->addColum(this, curCell);
                        }
                        while(active_row + nRows > curActiveTable->nRows())
                        {
                            curActiveTable->addString(this, curCell);
                        }
                        activeBlockStart = activeIndex;
                        for(int row = 0; row < curTable->nRows(); row++)
                        {

                            for(int colum = 0; colum < curTable->nColums(); colum++)
                            {
                                //                                i++;
                                if(colum < startColum || colum > endColum || row < startRow || row > endRow)
                                    continue;
                                activeIndex = tableCellIndex(curActiveTable,row + active_row,colum + active_colum,doc->lBlocks);
                                i = tableCellIndex(curTable,row,colum, m_clipboardDoc->lBlocks);
								curCell = (EjCellBlock*)doc->lBlocks->at(activeIndex);
								curCell2 = (EjCellBlock*)m_clipboardDoc->lBlocks->at(i);
                                while(doc->lBlocks->at(activeIndex+1)->type != END_GROUP && doc->lBlocks->at(activeIndex+1)->type != BASECELL)
                                {
                                    delete doc->lBlocks->takeAt(activeIndex + 1);
                                    updateFragments(activeIndex,false);
                                }
								if(curCell->vid == EjCellBlock::CELL_FORMULA)
                                {
                                    list.clear();
                                    txt.clear();
                                    QString formula = curCell->formula();
                                    for(int i = 0; i < formula.count(); i++)
                                    {

                                        if(calculator.is_split(formula[i].toLatin1()) )
                                        {
                                            if(!txt.isEmpty())
                                                list << txt;
                                            txt = formula[i];
                                            list << txt;
                                            txt.clear();
                                        }
                                        else txt += formula[i];
                                    }
                                    if(!txt.isEmpty())
                                        list << txt;
                                    for(int i2 = 0; i2 < list.count(); i2++)
                                    {
                                        txt = list.at(i2);
                                        //                                        isEdit = false;
                                        if(txt == "SUMM" || txt == "MAX" || txt == "MIN")
                                            continue;
                                        isFormula = tableLinkParams(txt, curActiveTable, curActiveTable, numTable, numRow, numColum);
                                        if(isFormula && numTable == curActiveTable->key)
                                        {
                                            numRow = numRow - curTable->start_row + active_row;
                                            if(numRow < 1)
                                                numRow = 1;
                                            numColum = numColum - curTable->start_column + active_colum;
                                            if(numColum < 0)
                                                numColum = 0;
                                            txt = "";
											txt += QString::number(numColum) + 'A';
                                            txt += QString::number(numRow);
                                            list[i2] = txt;
                                        }
                                    }
                                    txt = "";
                                    for(int i2 = 0; i2 < list.count(); i2++)
                                    {
                                        txt += list.at(i2);
                                    }
                                    curCell->setFormula(txt);
                                }

                                if(row == curTable->nRows() - 1 && colum == curTable->nColums() - 1 && statusMode_bak == EDIT_CELL
										&& curCell->vid == EjCellBlock::CELL_FORMULA)
                                    curCell->setText(curCell->formula());
                                while(m_clipboardDoc->lBlocks->at(i+1)->type != e_typeBlocks::END_GROUP && m_clipboardDoc->lBlocks->at(i+1)->type != BASECELL)
                                {
                                    i++;
                                    cur_Block = m_clipboardDoc->lBlocks->at(i)->makeCopy();
                                    if(cur_Block->type == TEXT)
                                    {
										cur_textBlock = (EjTextBlock*)cur_Block;
                                        inputText(cur_textBlock->text);
                                    }
                                    else if(cur_Block->type == SPACE)
                                    {
                                        activeIndex++;
										doc->lBlocks->insert(activeIndex, new EjSpaceBlock());
                                        updateFragments(activeIndex, true);
                                    }
                                    else if (cur_Block->type == NUM_STYLE)
                                    {
                                        EjNumStyleBlock *block_style = (EjNumStyleBlock*)cur_Block;
                                        if (block_style->style->m_vid == e_PropDoc::CELL_STYLE)
                                        {
											EjCellStyle *cell_style = new EjCellStyle();
                                            cell_style->setTopBorder(curCell->cellStyle->topBorder());
                                            cell_style->setLeftBorder(curCell->cellStyle->leftBorder());
                                            cell_style->setBottomBorder(curCell->cellStyle->bottomBorder());
                                            cell_style->setRightBorder(curCell->cellStyle->rightBorder());
                                            cell_style->setBrushColor(curCell2->cellStyle->m_brushColor);
                                            block_style->style = cell_style;
                                            activeIndex++;
                                            doc->lBlocks->insert(activeIndex, block_style);

                                            doc->lPropBlocks->append(cell_style);
                                            doc->lStyles->append(cell_style);

                                        }else if (block_style->style->m_vid == e_PropDoc::TEXT_STYLE)
                                        {
											EjTextStyle *text_style = (EjTextStyle*)block_style;
                                            block_style->style = text_style;
                                        }else if (block_style->style->m_vid == e_PropDoc::PARAGRAPH_STYLE)
                                        {
											EjParagraphStyle *p_style = (EjParagraphStyle*)block_style;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if(cur_Block->type == ENTER)
                        {
							count = ((EjTableBlock*)(curCell->parent))->m_counts;
							if(activeIndex + ((EjTableBlock*)(curCell->parent))->nColums() > doc->lBlocks->indexOf(curCell->parent) + 1 + count)
                            {
                                addTableString();
                            }
							activeIndex += ((EjTableBlock*)(curCell->parent))->nColums();
							curCell = (EjCellBlock*)doc->lBlocks->at(activeIndex);
							curCell->vid = EjCellBlock::CELL_AUTO;
                            position = 0;
                        }
                        else if(cur_Block->type == TEXT)
                        {

							cur_textBlock = (EjTextBlock*)m_clipboardDoc->lBlocks->at(i);
                            inputText(cur_textBlock->text);
                        }
                        else if(cur_Block->type == SPACE)
                        {
                            inputSpace();
                        }
                    }
                }
            }
            else
            {
                if(mimeData && (mimeData->hasHtml() || mimeData->hasText()))
                {
                    int row, colum;
                    int base_colum;
					EjTableBlock *curTable = ((EjTableBlock*)(curCell->parent));

                    cellParams(curTable,activeIndex,row,base_colum);

                    txt = mimeData->text();
                    QChar chr;
                    for(int i = 0; i < txt.count(); i++)
                    {
                        chr = txt[i];
                        if(chr == QChar::Space)
                        {
                            inputSpace();

                        }
                        else if(chr == QChar::Tabulation)
                        {
                            cellParams(curTable,activeIndex,row,colum);
                            colum++;
                            if(colum > curTable->nColums() - 1)
                                addTableColum();
                            activeIndex = tableCellIndex(curTable,row,colum,doc->lBlocks);
                            while(doc->lBlocks->at(activeIndex + 1)->type != END_GROUP && doc->lBlocks->at(activeIndex + 1)->type != BASECELL)
                            {
                                delete doc->lBlocks->takeAt(activeIndex + 1);
                                updateFragments(activeIndex,false);
                            }
                            position = 0;
                        }
                        else if(chr == QChar::CarriageReturn)
                        {
                            cellParams(curTable,activeIndex,row,colum);
                            row++;
                            if(row > curTable->nRows() - 1)
                                addTableString();
                            activeIndex = tableCellIndex(curTable,row,base_colum,doc->lBlocks);
                            position = 0;

                        }
                        else if(chr != QChar::LineFeed)
                            inputText(chr);
                    }
                }

            }

            m_statusMode = statusMode_bak;

        }
        else
        {
            if(activeIndex > -1)
                splitText(activeIndex,position);
            int i = 0;
            if(mimeData && mimeData->text() == m_htmlBuffer && !0)
            {
				EjTableFragment *curFragment;
                for(i = 0; i < m_clipboardDoc->lBlocks->count(); i++)
                {
                    cur_Block = m_clipboardDoc->lBlocks->at(i)->makeCopy();
                    if(cur_Block->type == EXT_TABLE)
                    {
                    }
                    doc->lBlocks->insert(activeIndex,cur_Block);
                    updateFragments(activeIndex,true);
                    activeIndex++;
                    position = 0;
                }
                updateTables(doc);
                calcTables();

                if(i > 0)
                {
                    activeIndex--;
                    if(activeIndex > -1 && doc->lBlocks->at(activeIndex)->type==TEXT)
                    {
						position = static_cast<EjTextBlock*>(doc->lBlocks->at(activeIndex))->text.size();
                    }
                }
            }
            else
            {
                if(activeIndex > -1)
                    activeIndex--;
                if(mimeData && ( mimeData->hasHtml() || mimeData->hasText()))
                {
                    str = mimeData->text();
                }
                else
                    str.clear();
                QStringList ltext = str.split(" ");
                for(int i = 0; i < ltext.count(); i++)
                {
                    if(i > 0)
                        inputSpace();
                    str = ltext[i];
                    QStringList ltext2 = str.split('\n');
                    for(int j = 0; j < ltext2.count(); j++)
                    {
                        if(j > 0)
                            inputEnter();
                        str = ltext2[j];
                        if(str != "")
                            inputText(str);
                    }
                }
            }
        }
        m_createPatchEnabled = true;

    }
    else
    {
        delete fragment;
        fragment = 0;
        res = false;
    }
    if(fragment) {
        updateTables(doc);
    }
    return res;
}

QString EjTextControl::isTell()
{
    bool res = false;
    int j;
    int start;
    int end;
    bool exit = false;
    QString str;
    int startSelectBlock = m_startSelectBlock;
    int endSelectBlock = m_endSelectBlock;
    int startSelectPos = m_startSelectPos;
    int endSelectPos = m_endSelectPos;
    if( startSelectBlock < 0 || endSelectBlock < 0 )
    {
        if(activeIndex < 0 || doc->lBlocks->isEmpty())
            return str;
		EjTableBlock *table = isTable(activeIndex);
        if(table)
        {
            startSelectBlock = activeIndex;
            if(getBlocks()->at(startSelectBlock)->type != BASECELL)
            {
                startSelectBlock = table->prevCell(startSelectBlock);
            }
            endSelectBlock = table->nextCell(startSelectBlock);
            if(endSelectBlock < 0)
                endSelectBlock = startSelectBlock;
            startSelectPos = 0;
            if(doc->lBlocks->at(endSelectBlock)->type == TEXT)
				endSelectPos = dynamic_cast<EjTextBlock*>(doc->lBlocks->at(endSelectBlock))->text.count();
        }
        else
        {
            if(doc->lBlocks->at(activeIndex)->type != TEXT)
                return str;
            startSelectBlock = activeIndex;
            while(startSelectBlock - 1 > 0 && doc->lBlocks->at(startSelectBlock - 1)->type == TEXT)
                startSelectBlock--;
            startSelectPos = 0;
            endSelectBlock = startSelectBlock;
            while(endSelectBlock + 1 < m_doc.lBlocks->count() && doc->lBlocks->at(endSelectBlock + 1)->type == TEXT)
                endSelectBlock++;
			endSelectPos = dynamic_cast<EjTextBlock*>(doc->lBlocks->at(endSelectBlock))->text.count();
        }

    }
    str = "";
    for(int i = startSelectBlock; i <= endSelectBlock; i++)
    {
        if(exit)
            break;
        if(doc->lBlocks->at(i)->type == TEXT || doc->lBlocks->at(i)->type == BASECELL)
			str += ((EjTextBlock*)doc->lBlocks->at(i))->text;
        else
        {
            res = false;
            if(doc->lBlocks->at(i)->type == SPACE || doc->lBlocks->at(i)->isProperty())
            {}
            else
                break;
        }
        if(str.count() > 50)
        {
            res = false;
            break;
        }
        start = 0;
        end = str.count();
        if(i == startSelectBlock && startSelectPos < start)
            start = startSelectPos;
        if(i == endSelectBlock && endSelectPos < end)
            end = endSelectPos;
        for(j = start; j < end; j++)
        {

            if ((str.at(j) >= QChar('0') && str.at(j) <= QChar('9')) || str.at(j) == QChar('(') || str.at(j) == QChar(')') || str.at(j) == QChar('-') || str.at(j) == QChar('+')  || str.at(j) == QChar(' '))
                res = true;
            else
            {
                res = false;
                exit = true;
                break;
            }
        }
    }
    if(!res)
        str = "";
    return str;
}

QList<EjFragmentBlock *> EjTextControl::getActualFragments(int block, EjTableBlock *cur_Table, int row, int colum)
{
	EjFragmentBlock *curFragment;
	EjTableFragment *curTableFragment;
	QList<EjFragmentBlock*> lActualFragments;
	QList<EjFragmentBlock*> *cur_lFragments = 0;

    bool bInsert;
    bool bIsBaseCell = false;
    return lActualFragments;

}

QList<EjFragmentBlock *> EjTextControl::getSelectFragments()
{
	EjFragmentBlock *curFragment;
	QList<EjFragmentBlock*> lActualFragments;
    return lActualFragments;

}

void EjTextControl::removeFragments(QList<EjFragmentBlock *> *l_fragments, int vid)
{
    for(int i = 0; i < l_fragments->count(); i++)
    {
        if(l_fragments->at(i)->vid == vid)
        {
            l_fragments->removeAt(i);
            i--;
        }
    }
}

QFontMetrics EjTextControl::getDrawMetrics(int block)
{
    return QFontMetrics(getTextStyle(block)->m_font);
}

EjTextStyle *EjTextControl::getTextStyle(int block) const
{
    return doc->currentTextStyle(block);
}

EjTextStyle *EjTextControl::getSelectedTextStyle(int block) const
{
    int resIndex;
    if(m_startSelectBlock > -1 && m_endSelectBlock > -1)
    {
        resIndex = m_startSelectBlock;
        bool bFindfText = false;
        while(resIndex < doc->lBlocks->count() && resIndex < m_endSelectBlock)
        {
            if(doc->lBlocks->at(resIndex)->type == TEXT)
            {
                bFindfText = true;
                break;
            }
            resIndex++;
        }
        if(!bFindfText)
            resIndex = m_startSelectBlock;
    }
    else
    {
        resIndex = block;
    }
    return doc->currentTextStyle(resIndex);
}

EjParagraphStyle *EjTextControl::getParagraphStyle(int block)
{
    return doc->currentParagraphStyle(block);
}



QRect EjTextControl::selectArea()
{
    int x1,y1,x2,y2;
    EjBlock *cur_Block;
    JString *cur_String = 0;
    int index;
    x1=x2=y1=y2=0;
    if(m_startSelectBlock >= 0 && m_startSelectBlock < doc->lBlocks->count())
    {
        cur_Block = doc->lBlocks->at(m_startSelectBlock);
        x1 = cur_Block->x + m_contentX;
        y1 = cur_Block->y + m_contentY - cur_Block->ascent - cur_Block->descent;
        index = doc->lBlocks->indexOf(cur_Block);
        cur_String = wichString(index);
        if(cur_String)
        {
            y1 = cur_String->y - cur_String->height;
        }
        if(cur_Block->type == TEXT)
        {
			QString txt = static_cast<EjTextBlock*>(cur_Block)->text;
            QFontMetrics drawMetrics = getDrawMetrics(m_startSelectBlock);
            if(m_startSelectPos > 0 && cur_Block->type == TEXT)
            {
                txt = txt.left(m_startSelectPos);
				x1 = x1 + drawMetrics.horizontalAdvance(txt);
            }
        }
    }
    if(m_endSelectBlock >= 0 && m_endSelectBlock < doc->lBlocks->count())
    {
        cur_Block = doc->lBlocks->at(m_endSelectBlock);
        x2 = cur_Block->x + m_contentX;
        y2 = cur_Block->y + m_contentY;
        if(cur_Block->type == TEXT)
        {
			QString txt = static_cast<EjTextBlock*>(cur_Block)->text;
            QFontMetrics drawMetrics = getDrawMetrics(m_endSelectBlock);
            if(m_endSelectPos > 0 && cur_Block->type == TEXT)
            {
                txt = txt.left(m_endSelectPos);
				x2 = x2 + drawMetrics.horizontalAdvance(txt);
            }
        }
    }
    QRect res;
    res.setCoords(x1,y1,x2,y2);
    return res;

}

void EjTextControl::updateFragments(int index, bool is_add, bool posNotNul)
{
	EjFragmentBlock *curFragment;
    int del_index;
}

void EjTextControl::blockOptimize()
{
	EjTextBlock *cur_textBlock;
	QList<EjFragmentBlock*> lActualFragments;
	QList<EjFragmentBlock*> lActualFragments2;

    for(int i = 0; i < doc->lBlocks->count(); i++)
    {
        if(doc->lBlocks->at(i)->type == TEXT)
        {
			cur_textBlock = (EjTextBlock*)doc->lBlocks->at(i);
            if(i != activeIndex && cur_textBlock->text == "")
            {
                updateFragments(i,false);
                if(m_startSelectBlock >= i) m_startSelectBlock--;
                if(m_endSelectBlock >= i) m_endSelectBlock--;
                doc->lBlocks->removeAt(i);
                if(activeIndex > i) activeIndex--;
                i--;
                continue;
            }
            lActualFragments = getActualFragments(i);
            if(i+1 < doc->lBlocks->count()) lActualFragments2 = getActualFragments(i+1);
            if(i+1 < doc->lBlocks->count() && doc->lBlocks->at(i+1)->type == TEXT && lActualFragments == lActualFragments2)
            {
                if(m_startSelectBlock >= i+1) {
                    if(m_startSelectBlock == i+1) m_startSelectPos += cur_textBlock->text.size();
                    m_startSelectBlock--;
                }

                if(m_endSelectBlock >= i+1) {
                    if(m_endSelectBlock == i+1) m_endSelectPos += cur_textBlock->text.size();
                    m_endSelectBlock--;
                    //                    m_endSelectPos += cur_textBlock->text.size();
                }
				cur_textBlock->text += static_cast<EjTextBlock*>(doc->lBlocks->at(i+1))->text;
                updateFragments(i+1,false);
                doc->lBlocks->removeAt(i+1);
                if(activeIndex > i+1) activeIndex--;
                if(i+1 < doc->lBlocks->count()) lActualFragments2 = getActualFragments(i+1);
                i--;
            }
        }
    }
    if(activeIndex > doc->lBlocks->count() - 1) activeIndex = doc->lBlocks->count() - 1;

}

void EjTextControl::fragmentOptimize(int vid)
{
}

void EjTextControl::fragmentDOptimize(int vid, int startBlock, int endBlock)
{
}

void EjTextControl::calcTables()
{
    EjCalculator calculator(doc);
    calculator.calcTables();
}

void EjTextControl::moveTable(int dX)
{
    if(m_isViewDoc)
        return;
	EjTableBlock *table = isTable(activeIndex);
	EjCellBlock *curCellBlock;


    if(table)
    {
        int start = table->startCell();
        int x_bak = doc->lBlocks->at(start)->x;
        int x = x_bak + dX / scaleSize;
        float k_scale = 0.0423333 / scaleSize * 0.236;

        if(x > 0 && x > leftColontitul * k_scale)
            x = leftColontitul * k_scale;
        else if(x < 0 && x + table->width < (m_width - rightColontitul) * k_scale) {
            x = (m_width - rightColontitul) * k_scale - table->width ;
        }
        qDebug() << "x= " << x << " " << doc->lBlocks->at(start)->x;
        dX = (x - x_bak);
        table->x += dX;
        for(int j = start; j <= table->endBlock(); j++)
        {
            doc->lBlocks->at(j)->x += dX;
            if(doc->lBlocks->at(j)->type == BASECELL)
            {
				curCellBlock = static_cast<EjCellBlock*>(doc->lBlocks->at(j));
            }
        }
    }
}



int EjTextControl::getBaseWidth(int index, QFontMetrics &drawMetric)
{
    int res = 0;
	EjTextBlock *cur_txtBlock;

    switch(doc->lBlocks->at(index)->type)
    {
    case TEXT:
        drawMetric = getDrawMetrics(index);
		cur_txtBlock = (EjTextBlock*)doc->lBlocks->at(index);
        if(cur_txtBlock)
        {
			res = drawMetric.horizontalAdvance(cur_txtBlock->text);
        }
        break;
    case SPACE:
        res = 5;
        break;
    }
    return res * 100 * 0.347;

}

void EjTextControl::calc(int index, bool force)
{
    int index_string = 0;
    int index_page = 0;
    EjBlock *cur_Block;
    Page *cur_page;
    JString *cur_string;
	EjCalcParams calcParams;
    calcParams.viewScale = scaleSize;

    int right_pos;
    int back_type = 0;
    bool new_string = false;
    double k_scale;
    int deltaX = 0;

    int x; // + metric.height() / 1.2;

    if(m_calcIndex > doc->lBlocks->count() - 1)
        m_calcIndex = doc->lBlocks->count() - 1;
    if(doc->lPages->isEmpty())
    {
        cur_page = new Page;
        cur_page->width = m_defaultPageWidth;
        cur_page->height = m_defaultPageHeight;
        doc->lPages->append(cur_page);
    }
    else cur_page = doc->lPages->at(0);

    if(doc->lStrings->isEmpty())
    {
        cur_string = new JString;
        if(m_isViewDoc)
            cur_string->y = cur_page->topMarging;
        else
            cur_string->y = 0; // + metric.height() / 1.2;
        doc->lStrings->append(cur_string);
    }
    else {
        index_string = 0;
        cur_string = doc->lStrings->at(0);
        for(int i = 0; i < doc->lStrings->count(); i++)
        {
            if(index >= doc->lStrings->at(i)->startBlock) {
                cur_string = doc->lStrings->at(i);
                index_string = i;
				break;
            }
        }
    }

    for (int i = 0; i < doc->lPages->count(); i++) {
        Page *page = doc->lPages->at(i);
        if(cur_string->y > page->y && cur_string->y < page->y + page->height) {
            cur_page = page;
            index_page = i;

        }
        else if(cur_string->y < page->y && !force)
            break;
        if(force)
            page->flag_redraw = true;
    }

    if(m_isViewDoc) {
        k_scale = scaleSize;
        right_pos = (cur_page->width - cur_page->rightMarging + leftColontitul );
    }
    else {
        k_scale = 0.0423333;  //23.622; // 1/4.23333*100
        right_pos = m_width * 0.236  * k_scale / scaleSize - rightColontitul;
    }
    if(force) {
    }

	int baseY = 0;
	if(index_string > 0)
		baseY = cur_string->y;
    if(m_isViewDoc)
        x = leftColontitul + cur_page->leftMarging;
    else
        x = leftColontitul;
    cur_page->x = leftColontitul;
    if(force)
        cur_page->flag_redraw = true;

    if(cur_string == doc->lStrings->at(0)) {
        index = 0;
        if(doc->lBlocks->count() > 0)
        {
            cur_Block = doc->lBlocks->at(0);
        }
    }
    else
    {
        index = cur_string->startBlock;
        if(doc->lBlocks->at(index)->type == ENTER)
        {
            index++;
            if(index > doc->lBlocks->count() - 1)
                index = doc->lBlocks->count() - 1;
        }
    }
    calcParams.viewScale = k_scale;
    calcParams.leftColontitul = x; //leftColontitul;
    calcParams.rightPosition = right_pos;
    calcParams.index_page = index_page;
    calcParams.index_string = index_string;
    calcParams.textStyle = getTextStyle(index);
    calcParams.paragraphStyle = getParagraphStyle(index);
    calcParams.control = this;
    calcParams.baseY = baseY;
    calcParams.interval = calcParams.textStyle->m_fontMetrics.height() * 100 * 0.347 * 0.5;
    calcParams.isViewDoc = m_isViewDoc;
    calcParams.statusMode = m_statusMode;
    calcParams.force = force;

    cur_string =  doc->lStrings->at(index_string);
    cur_string->width = 0;

    m_interval = 0;

    int indexBack = index;

    for(int i = index; i <= m_calcIndex; i++)
    {
        cur_Block = doc->lBlocks->at(i);
        if(force)
            cur_Block->flag_redraw = true;
        calcParams.baseX = x;
        if(cur_Block->type == ENTER)
        {
            cur_Block->descent = calcParams.textStyle->m_fontMetrics.descent() * 100 * 0.347;
            cur_Block->ascent = calcParams.textStyle->m_fontMetrics.ascent() * 100 * 0.347;
            if(m_isViewDoc)
                x = leftColontitul + cur_page->leftMarging;
            else
                x = leftColontitul;
            cur_Block->x = x;
            {
                new_string = true;
                indexBack = i;
            }
        }
        else
        {
            indexBack = i;
            cur_Block->calcBlock(i, &calcParams);
        }

        if((x > leftColontitul + (m_isViewDoc ? cur_page->leftMarging : 0))
                && x + cur_Block->width > right_pos)
        {
            new_string = true;
        }
        else
        {
            cur_Block->x = x;
            x += cur_Block->width;
            cur_string->width += cur_Block->width;
        }
        if(new_string)
        {

            calcParams.control->calcString(cur_string, cur_page, &calcParams);
            calcParams.control->addString(&calcParams, indexBack);
            index_string = calcParams.index_string;
            cur_string =  doc->lStrings->at(index_string);
            baseY = calcParams.baseY;
            new_string = false;
            x = calcParams.baseX;
            cur_Block->x = x;
            cur_string->width = cur_Block->width;
            x += cur_Block->width;
        }

        cur_string->endBlock = i;
        if(cur_string->endBlock - cur_string->startBlock > 1000)
        {
            qWarning() << "Error for string" << __FILE__ << __LINE__ << ": " << doc->lBlocks->count() << i << doc->lStrings->count() << index_string << cur_string->startBlock << cur_string->endBlock;

        }

        if(i < doc->lBlocks->count())
            back_type = cur_Block->type;

    }
    cur_string = doc->lStrings->at(index_string);
    calcString(cur_string, cur_page, &calcParams);
    if(m_isViewDoc && cur_string->y > cur_page->y + (cur_page->height - cur_page->bottomMarging))
    {
        baseY += (cur_page->bottomMarging) + 1500;
        cur_page = new Page;
        cur_page->width = m_defaultPageWidth;
        cur_page->height = m_defaultPageHeight;
        cur_page->y = baseY;
        doc->lPages->append(cur_page);
        baseY += (cur_page->topMarging );
        for(int i = cur_string->startBlock; i <= cur_string->endBlock; i++)
        {
            doc->lBlocks->at(i)->y = baseY;
        }
    }
    index_string++;
    while (index_string < doc->lStrings->count()) {
        delete doc->lStrings->at(index_string);
        doc->lStrings->removeAt(index_string);
    }
    if(m_isViewDoc)
    {
        cur_page = doc->lPages->last();
        if(m_height != cur_page->y + cur_page->height)
        {
            m_height = cur_page->y + cur_page->height;
            emit controlHeightChanged();
        }
    }
    else
        if(m_height != baseY + cur_string->height + bottomColontitul * k_scale / scaleSize * 0.236)
        {
            m_height = baseY + cur_string->height + bottomColontitul * k_scale / scaleSize * 0.236;
            if(!doc->lBlocks->isEmpty() && doc->lBlocks->last()->type == ENTER)
            {
				m_height += cur_string->height;
            }
            emit controlHeightChanged();
        }
}


void EjTextControl::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    killTimer(m_timerId);
}

void EjTextControl::calcString(JString *string, Page *page, EjCalcParams *calcParams)
{
    if(doc->lBlocks->isEmpty()) return;
    int deltaX = 0;
	if(calcParams->paragraphStyle->m_align & EjParagraphStyle::AlignLeft)
        deltaX = 0;
	if(calcParams->paragraphStyle->m_align & EjParagraphStyle::AlignRight)
    {
        if(calcParams->isViewDoc)
            deltaX = calcParams->rightPosition - string->width - page->leftMarging - calcParams->leftColontitul;
        else
            deltaX = calcParams->rightPosition - string->width - calcParams->leftColontitul;
        if(deltaX < 0)
            deltaX = 0;
    }
	if(calcParams->paragraphStyle->m_align & EjParagraphStyle::AlignHCenter)
    {
        //                deltaX = (m_width) * 0.236 * k_scale / scaleSize - cur_string->width - (leftColontitul + rightColontitul) * 0.236;
        if(calcParams->isViewDoc)
            deltaX = calcParams->rightPosition - string->width - page->leftMarging - calcParams->leftColontitul;
        else
            deltaX = calcParams->rightPosition - string->width - calcParams->leftColontitul;
        deltaX *= 0.5;
        if(deltaX < 0)
            deltaX = 0;
    }

    if(string->startBlock < 0)
    {
		string->height = metric.height();
        string->width = 0;
        return;
    }
    EjBlock *cur_Block = doc->lBlocks->at(string->startBlock);
    string->height = cur_Block->ascent + cur_Block->descent;
    string->width = 0;

    {
        string->height = 0;
        string->ascent = 0;
        string->width = 0;
        string->interval = 0;
        for(int i = string->startBlock; i <= string->endBlock; i++)
        {
            cur_Block = doc->lBlocks->at(i);
            cur_Block->x += deltaX;
            string->width += cur_Block->width;
            if(string->height < cur_Block->ascent + cur_Block->descent)
                string->height = cur_Block->ascent + cur_Block->descent;
            if(string->ascent < cur_Block->ascent) string->ascent = cur_Block->ascent;
			if((cur_Block->type == TEXT || cur_Block->type == ENTER) && string->interval < (cur_Block->ascent + cur_Block->descent) * 0.5)
                string->interval = (cur_Block->ascent + cur_Block->descent) * 0.5;
            if(cur_Block->type >= GROUP_BLOCK && !((EjGroupBlock*)cur_Block)->isGlassy())
            {
                i += ((EjGroupBlock*)cur_Block)->m_counts;
            }
        }
        string->height += string->interval;
        for(int i = string->startBlock; i <= string->endBlock; i++)
        {
            cur_Block = doc->lBlocks->at(i);
            if(cur_Block->type == SPACE)
                cur_Block->ascent = string->ascent;
            cur_Block->y = calcParams->baseY;
            cur_Block->interval_top = string->ascent - cur_Block->ascent;
            cur_Block->interval_bottom = string->interval;
            if(cur_Block->type >= GROUP_BLOCK && !((EjGroupBlock*)cur_Block)->isGlassy())
            {
                i += ((EjGroupBlock*)cur_Block)->m_counts;
                doc->lBlocks->at(i)->y = calcParams->baseY;
            }
        }
        string->x = doc->lBlocks->at(string->startBlock)->x;
        string->y = calcParams->baseY;
        calcParams->baseY += string->height;
    }
}

void EjTextControl::addString(EjCalcParams *calcParams, int indexBlock)
{
    Page *cur_page = calcParams->control->doc->lPages->at(calcParams->index_page);
    JString *cur_string = calcParams->control->doc->lStrings->at(calcParams->index_string);

    if(calcParams->isViewDoc && cur_string->y > cur_page->y + cur_page->height - cur_page->bottomMarging)
    {
        calcParams->baseY = cur_page->y + cur_page->height + 1500;
        {
            cur_page = new Page;
            cur_page->width = m_defaultPageWidth;
            cur_page->height = m_defaultPageHeight;
            cur_page->y = calcParams->baseY;
            cur_page->x = leftColontitul;
            doc->lPages->append(cur_page);
        }
        calcParams->index_page++;
        calcParams->baseY += (cur_page->topMarging);
        for(int i = cur_string->startBlock; i <= cur_string->endBlock; i++)
        {
            doc->lBlocks->at(i)->y = calcParams->baseY;
        }
        cur_string->y = calcParams->baseY;
        calcParams->baseY += cur_string->height;
    }
    calcParams->index_string++;
    if(calcParams->index_string > doc->lStrings->count() - 1)
    {
        doc->lStrings->append(new JString());
    }
    doc->lStrings->at(calcParams->index_string)->startBlock = indexBlock;
    doc->lStrings->at(calcParams->index_string)->endBlock = indexBlock;
    if(indexBlock > doc->lBlocks->count() - 1)
    {
        doc->lStrings->at(calcParams->index_string)->startBlock = doc->lBlocks->count() - 1;
        doc->lStrings->at(calcParams->index_string)->endBlock = doc->lBlocks->count() - 1;
    }
    cur_string = doc->lStrings->at(calcParams->index_string);
    cur_string->width = 0;

    if(m_isViewDoc)
        calcParams->baseX = leftColontitul + cur_page->leftMarging;
    else
        calcParams->baseX = leftColontitul;

}

int EjTextControl::startText(int index) const
{
   int res = index;
   for(int i = index; i > -1; i--)
   {
       int type = doc->lBlocks->at(i)->type;
       if(!doc->lBlocks->at(i)->isProperty() && type != TEXT)
           break;
       if(type == TEXT)
           res = i;
   }
   return res;
}

int EjTextControl::endText(int index) const
{
    int res = index;
    if(index > -1 && index < doc->lBlocks->count())
    {
        for(int i = index; i < doc->lBlocks->count(); i++)
        {
            int type = doc->lBlocks->at(i)->type;
            if(type == TEXT)
                res = i;
            if(!doc->lBlocks->at(i)->isProperty() && type != TEXT)
                break;
        }
        if(doc->lBlocks->at(res)->type == TEXT)
            res++;
    }
    return res;
}

bool EjTextControl::isEndText(int index) const
{
    bool res = false;
    if(index > -1 && index < doc->lBlocks->count() && doc->lBlocks->at(index)->type == TEXT)
    {
		EjTextBlock *block = dynamic_cast<EjTextBlock*>(doc->lBlocks->at(index));
        if(index == activeIndex && block && block->text.count() <= position)
        {
            res = true;
            index++;
        }
    }
    return res;
}

JString *EjTextControl::wichString(int index)
{
    JString *resString = 0;
    foreach(JString *curString, *doc->lStrings)
    {
        if(index >= curString->startBlock && index <= curString->endBlock)
            resString = curString;
        break;
    }
    return resString;

}
