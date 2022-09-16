#include "ejtableblocks.h"
#include "intvarlen.h"


EjTableBlock::EjTableBlock() : EjGroupBlock()
{
    type = EXT_TABLE; style = 0; vid = CLEAN_TABLE; accuracy = 3;  deltaProps = 0;
    accuracy = 3;
    key = 0;
    m_doc = NULL;

}

EjTableBlock::EjTableBlock(quint32 key_) : EjTableBlock()
{
    type = EXT_TABLE; vid = CLEAN_TABLE;
    key = key_;
}

EjTableBlock::EjTableBlock(int rows, int colums, EjDocument *doc, int index) : EjTableBlock()
{
    m_doc = doc;

    EjCellBlock *curCellBlock;
    EjCellStyle tmpStyle;
    EjCellStyle *cellStyle = getCellStyle(-1);
	EjPropIntBlock *curInt;
    if(index > -1 && doc->lBlocks->at(index)->type != TEXT)
        index++;
    if(index < 0)
        index = 0;
    doc->lBlocks->insert(index,this);
    m_index = index;
    index++;
    this->width = 0;
    this->vid = CLEAN_TABLE;
	curInt = new EjPropIntBlock(TBL_KEY);
    curInt->value = doc->nextTableKey();
    doc->lBlocks->insert(index,curInt);
    index++;

	curInt = new EjPropIntBlock(TBL_ROWS);
    curInt->value = rows;
    doc->lBlocks->insert(index,curInt);
    index++;

	curInt = new EjPropIntBlock(TBL_COLUMS);
    curInt->value = colums;
    doc->lBlocks->insert(index,curInt);
    index++;
    m_counts = deltaProps = index - m_index;

    for(int i = 0; i < rows; i++)
    {
        this->lRows.append(new EjSizeProp());
        for(int j = 0; j < colums; j++)
        {
            if(i == 0)
                this->lColums.append(new ColumProp());
            doc->lBlocks->insert(index,new EjCellBlock());
            curCellBlock = dynamic_cast<EjCellBlock*>(doc->lBlocks->at(index));
            curCellBlock->parent = this;
            curCellBlock->cellStyle = cellStyle;
            index++;
            m_counts++;
        }
    }
	doc->lBlocks->insert(index,new EjBlock(END_GROUP,this));
    doc->lTables->append(this);
    tmpStyle.topBorder()->m_width = 200;
    tmpStyle.bottomBorder()->m_width = 200;
    tmpStyle.leftBorder()->m_width = 200;
    tmpStyle.rightBorder()->m_width = 200;
    tmpStyle.verticalLine()->m_width = 100;
    tmpStyle.horisontalLine()->m_width = 100;
    tmpStyle.m_doc = doc;

    setCellStyles(0,0,rows,colums,&tmpStyle);
}


EjTableBlock::~EjTableBlock()
{
    qDeleteAll(lColums);
    qDeleteAll(lRows);
}

EjBlock *EjTableBlock::makeCopy()
{
    EjTableBlock *res = new EjTableBlock();
    EjSizeProp *cur_sizeProp;
    ColumProp *cur_columProp;
    copyData(res);
    res->style = style;
    res->vid = vid;  // 0 - shoplist, 1 - cleantable ...
    res->accuracy = accuracy;
    res->key = key;

    foreach (ColumProp *columProp, lColums)
    {
        cur_columProp = new ColumProp();
        cur_columProp->sizeProp.current = cur_columProp->sizeProp.current;
        cur_columProp->sizeProp.isMax = cur_columProp->sizeProp.isMax;
        cur_columProp->sizeProp.max = cur_columProp->sizeProp.max;
        cur_columProp->sizeProp.min = cur_columProp->sizeProp.min;
        res->lColums.append(cur_columProp);
    }
    foreach (EjSizeProp *sizeProp, lRows)
    {
        cur_sizeProp = new EjSizeProp();
        cur_sizeProp->current = sizeProp->current;
        cur_sizeProp->isMax = sizeProp->isMax;
        cur_sizeProp->max = sizeProp->max;
        cur_sizeProp->min = sizeProp->min;
        res->lRows.append(cur_sizeProp);
    }
    return res;
}

void EjTableBlock::calcBlock(int &index, EjCalcParams *calcParams)
{
    int wNum = 5 * 100 * 0.347;
    m_doc = calcParams->control->doc;
	EjCellBlock *curCellBlock;
	EjBlock *cur_block;
	QList<EjBlock*> *lBlocks = calcParams->control->doc->lBlocks;
    EjCellStyle *curCellStyle = NULL;
    int index2;
    int delta = 0;
    int w;
    int width_tmp;
    this->spacing = 4 * 100 * 0.347; // 4 * scaleSize;
    this->m_index = index;
    this->m_counts = 0;
    while(lBlocks->at(index + 1)->type != BASECELL) {
        index2 = index + 1;
        lBlocks->at(index + 1)->calcBlock(index2,calcParams);
        if(index < lBlocks->count() - 1 )
            index++;
        else
            break;
        lBlocks->at(index)->parent = this;
        this->m_counts++;
    }
    deltaProps = m_counts + 1;
    for(int row = 0; row < this->nRows(); row++)
    {
        for(int colum = 0; colum < this->nColums(); colum++)
        {
            delta++;
            index2 = index + delta;
            if(index2 > lBlocks->count() - 2)
                break;
            if(row == 0)
            {
                this->lColums[colum]->sizeProp.current = wNum;
                if( lColums[colum]->sizeProp.current <  lColums[colum]->sizeProp.min)
                    lColums[colum]->sizeProp.current =  lColums[colum]->sizeProp.min;
                this->lColums[colum]->sizeProp.isMax = false;
            }
            if(lBlocks->at(index2)->type == BASECELL)
            {
                curCellBlock = (EjCellBlock*)lBlocks->at(index2);
                curCellBlock->width = this->lColums[colum]->sizeProp.min;
                curCellBlock->ascent = calcParams->textStyle->m_fontMetrics.height() * 100 * 0.347 + this->spacing * 2; //* 1.5;
                curCellBlock->parent = this;
                while(index2 < lBlocks->count() - 1 && lBlocks->at(index2 + 1)->type != BASECELL && lBlocks->at(index2 + 1)->type != END_GROUP)
                {
                    delta++;
                    index2++;
                    cur_block = lBlocks->at(index2);
                    cur_block->calcBlock(index2,calcParams);
                    cur_block->parent = this;
                    if(cur_block->type >= GROUP_BLOCK) {
                        delta += ((EjGroupBlock*)cur_block)->m_counts;
                    }

                    if(cur_block->type == NUM_STYLE)
                    {
                        if(((EjNumStyleBlock*)cur_block)->style
                                && ((EjNumStyleBlock*)cur_block)->style->m_vid == CELL_STYLE)
                            curCellStyle = (EjCellStyle*)((EjNumStyleBlock*)cur_block)->style;
                    }

					if(cur_block->type == PROP_INT && ((EjPropBase*)cur_block)->num == EjCellBlock::CELL_MERGE_COLUMS)
						curCellBlock->mergeColums = ((EjPropIntBlock*)cur_block)->value;
					if(cur_block->type == PROP_INT && ((EjPropBase*)cur_block)->num == EjCellBlock::CELL_MERGE_ROWS)
						curCellBlock->mergeRows = ((EjPropIntBlock*)cur_block)->value;
					if(cur_block->type == PROP_INT8 && ((EjPropBase*)cur_block)->num == EjCellBlock::CELL_VID)
						curCellBlock->vid = ((EjPropInt8Block*)cur_block)->value;

                    width_tmp = cur_block->width + spacing * 2;
                    if(curCellBlock->mergeRows > 0 && curCellBlock->mergeColums > 0)
                    {
                        width_tmp /= curCellBlock->mergeColums;
                        if(width_tmp < 1)
                            width_tmp = 1;
                    }
                    for(int i = 0; i == 0 || i < curCellBlock->mergeColums; i++)
                    {
                        if(this->lColums[colum + i]->sizeProp.max < width_tmp)
                            this->lColums[colum + i]->sizeProp.max = width_tmp;

                        if(this->lColums[colum + i]->sizeProp.current < width_tmp)
                            this->lColums[colum + i]->sizeProp.current = width_tmp;

                    }
                }
                curCellBlock->cellStyle = curCellStyle;
                if(this->lColums[colum]->sizeProp.current > this->lColums[colum]->sizeProp.max)
                    this->lColums[colum]->sizeProp.current = this->lColums[colum]->sizeProp.max;
                if(this->lColums[colum]->sizeProp.current < this->lColums[colum]->sizeProp.min)
                {
                    this->lColums[colum]->sizeProp.current = this->lColums[colum]->sizeProp.min;
                }
            }

        }
    }
     quint8 style = this->style;
    if(this->vid == EjTableBlock::SHOP_LIST && this->nColums() > 0 && this->nRows() > 0)
    {
        //            style = 2 + (1 << 2) + (1 << 3);

        if((style >> 3 & 0x01) <= 0)
            this->lColums[0]->sizeProp.current = 0;
        this->lColums[0]->sizeProp.isMax = true;
        if((style >> 2 & 0x01) <= 0)
            this->lColums[1]->sizeProp.current =  0;
        this->lColums[1]->sizeProp.isMax = true;
        if((style & 0x03) != 0x02)
        {
            this->lColums[3]->sizeProp.current = 0;
            this->lColums[3]->sizeProp.isMax = true;
        }
        if((style & 0x03) == 0)
        {
            this->lColums[4]->sizeProp.current = 0;
            this->lColums[4]->sizeProp.isMax = true;
        }
        if((style  & 0x03) != 0x02)
        {
            this->lColums[5]->sizeProp.current = 0;
            this->lColums[5]->sizeProp.isMax = true;
        }

    }
    bool isCorrect = false;
    int summ = 0;
    int n = 0;
    for(int j = 0; j < 10; j++ )
    {
        isCorrect = false;
        summ = 0;
        n = 0;
        for(int k = 0; k < this->nColums(); k++) {
            summ += this->lColums[k]->sizeProp.current;
            if(!this->lColums[k]->sizeProp.isMax) n++;
        }
        if(n == 0)
            break;
        if(calcParams->isViewDoc)
        {
            Page *curPage = calcParams->control->doc->lPages->at(calcParams->index_page);
            //                    summ = (21000 - rightColontitul)* k_scale / scaleSize - summ -  cur_page->leftMarging;
            summ = curPage->width - curPage->rightMarging - summ -  curPage->leftMarging;
        }
        else
            summ = calcParams->rightPosition - summ - calcParams->leftColontitul;
        if(summ < 0) summ = 0;
        summ /= n;
        if(summ == 0)
            break;
        for(int k = 0; k < this->nColums(); k++) {
            if(!this->lColums[k]->sizeProp.isMax)
            {
                this->lColums[k]->sizeProp.current += summ;
                if(this->lColums[k]->sizeProp.current > this->lColums[k]->sizeProp.max)
                    this->lColums[k]->sizeProp.current = this->lColums[k]->sizeProp.max;
            }
        }

    }

    summ = 0;
    for(int k = 0; k < this->nColums(); k++) {
        summ += this->lColums[k]->sizeProp.current;
    }
    this->width = summ;


    for(int row = 0; row < this->nRows(); row++)
    {
        for(int j = 0; j < this->nColums(); j++)
        {
            if(index < lBlocks->count() - 1)
                index++;
            else
                break;

            this->m_counts++;
            bool bRedraw = false;
            w = this->lColums[j]->sizeProp.current;
            cur_block = lBlocks->at(index);
            cur_block->flag_redraw = calcParams->force;
            if( cur_block->width != w) {
                cur_block->width = w;
                cur_block->flag_redraw = true;
                bRedraw = true;
            }
            if(this->vid ==  EjTableBlock::SHOP_LIST && (style  & 0x03) != 0x02 && row == this->nRows() -1)
                lBlocks->at(index)->ascent = 0;
            while(index < lBlocks->count() - 1 && lBlocks->at(index + 1)->type != BASECELL && lBlocks->at(index + 1)->type != END_GROUP)
            {
                index++;
                this->m_counts++;
                cur_block = lBlocks->at(index);
                cur_block->flag_redraw = calcParams->force;
                if(bRedraw)
                {
                    cur_block->flag_redraw = true;
                }
                if(cur_block->type >= GROUP_BLOCK) {
                    index += ((EjGroupBlock*)cur_block)->m_counts;
                    if(index > lBlocks->count() - 1)
                    {
                        index = lBlocks->count() - 1;
                        m_counts = index - m_index;
                        if(m_counts < 0)
                            m_counts = 0;
                        break;
                    }
                    m_counts += ((EjGroupBlock*)cur_block)->m_counts;
                }
            }
        }
    }
    if(index < lBlocks->count() - 1)
    {
        index++;
        m_counts++;
    }
    int i, i_cell, i_cell2, i_str, i_str_cell, i_first;
    int x, x_cell;
    int d = 0;
    int cellHeight = 0;
    int k, max_k;
    int baseY, baseY_back;
    JString *curString;
    QList<quint8> lKeys;
    int align_tmp = 0;
    int interval = 0;
    int deltaX = 0;
    int cellWidth;
    int rowHeight;
    QMap<int,int> rowsHeight;
    EjParagraphStyle *curPrgStyle = NULL;


    baseY = baseY_back = calcParams->baseY;

    curPrgStyle = calcParams->paragraphStyle;
    if(calcParams->baseX != calcParams->leftColontitul)
    {
        Page *curPage = calcParams->control->doc->lPages->at(calcParams->index_page);
        curString = calcParams->control->doc->lStrings->at(calcParams->index_string);

        if(calcParams->paragraphStyle->m_align & EjParagraphStyle::AlignLeft)
            deltaX = 0;
        else
        {
            //                deltaX = (m_width) * 0.236 * k_scale / scaleSize - cur_string->width - (leftColontitul + rightColontitul) * 0.236;
            if(calcParams->isViewDoc)
                deltaX = calcParams->rightPosition - curString->width - curPage->leftMarging - calcParams->leftColontitul;
            else
                deltaX = calcParams->rightPosition - curString->width - calcParams->leftColontitul;
            if(deltaX < 0)
                deltaX = 0;
        }
        if(calcParams->paragraphStyle->m_align & EjParagraphStyle::AlignHCenter)
        {
            deltaX *= 0.5;
        }

        calcParams->control->calcString(curString,curPage,calcParams);
        calcParams->index_string++;
        if(calcParams->index_string > calcParams->control->doc->lStrings->count() - 1)
            calcParams->control->doc->lStrings->append(new JString());
        calcParams->baseX = calcParams->leftColontitul;
    }
    curString = calcParams->control->doc->lStrings->at(calcParams->index_string);
    curString->endBlock = this->m_index + this->m_counts;
    width = 0;
    this->x = calcParams->baseX;
    this->y = baseY;
    i = this->m_index;
    while(lBlocks->at(i+1)->type != BASECELL)
        i++;
    for(int row = 0; row < this->nRows() - d; row++)
    {
        x = this->x;
        max_k = 0;
        i_str = i;
        cellHeight = calcParams->textStyle->m_fontMetrics.height() * 100 * 0.347 + this->spacing * 2.5;// * 1.5;
        if(rowsHeight.contains(row))
            rowHeight = rowsHeight.value(row);
        else
            rowHeight = cellHeight + interval;


        for(int colum = 0; colum < this->nColums(); colum++)
        {
            if(i < lBlocks->count() - 1)
                i++;
            else
                break;
            x_cell = x;
            i_cell = i;
            i_str_cell = i + 1;
            cur_block = lBlocks->at(i);
            cur_block->x = x_cell;
            cur_block->y = baseY; // + cellHeight;

            align_tmp = 0;
            if(cur_block->type == BASECELL)
            {
                curCellBlock = dynamic_cast<EjCellBlock*>(cur_block);
            }
            else if(cur_block->type == NUM_STYLE )
            {
                if(dynamic_cast<EjNumStyleBlock*>(cur_block)->style->m_vid == PARAGRAPH_STYLE)
                {
                    curPrgStyle = dynamic_cast<EjParagraphStyle*>(dynamic_cast<EjNumStyleBlock*>(cur_block)->style);
                }
                continue;
            }
            k = 0;
            if(row == 0)
                width += this->lColums[colum]->sizeProp.current;
            cellWidth = this->lColums[colum]->sizeProp.current;
            if(curCellBlock->mergeColums > 0 && curCellBlock->mergeRows > 0)
            {
                for(int j = 1; j < curCellBlock->mergeColums; j++)
                {
                    if(colum + j < lColums.count())
                        cellWidth += this->lColums[colum + j]->sizeProp.current;
                }
            }

            delta = 0;
            i_first = -1;

            while(i < endBlock() && lBlocks->at(i+1)->type != BASECELL)
            {
                i++;
                if(lBlocks->at(i)->type == NUM_STYLE)
                {
                    EjNumStyleBlock *numStyle = dynamic_cast<EjNumStyleBlock*>(lBlocks->at(i));
                    if(numStyle && numStyle->style && numStyle->style->m_vid == PARAGRAPH_STYLE)
                    {
                        curPrgStyle = dynamic_cast<EjParagraphStyle*>(dynamic_cast<EjNumStyleBlock*>(lBlocks->at(i))->style);
                    }
                    continue;
                }
                else if(lBlocks->at(i)->height() > cellHeight)
                    cellHeight = lBlocks->at(i)->height();
                if(!lBlocks->at(i)->isProperty() && i_first < 0)
                    i_first = i;

                align_tmp = curPrgStyle->m_align;

                if(i != i_first && (x + spacing*2 + lBlocks->at(i)->width > x_cell + cellWidth || lBlocks->at(i)->type == ENTER))
                {
                    k++;
                    if(align_tmp & Qt::AlignRight)
                    {
                        delta = (x_cell + cellWidth - spacing * 2 - x) / 2;
                        i_cell2 = i_str_cell;
                        while(i_cell2 <= i)
                        {
                            lBlocks->at(i_cell2)->x += delta;
                            if(lBlocks->at(i_cell2)->type >= GROUP_BLOCK)
                                i_cell2 += ((EjGroupBlock*)lBlocks->at(i_cell2))->m_counts;
                            i_cell2++;
                        }

                    }
                    if(align_tmp & Qt::AlignHCenter)
                    {
                        delta = (x_cell + cellWidth - spacing * 2 - x) / 2;
                        i_cell2 = i_str_cell;
                        while(i_cell2 <= i)
                        {
                            lBlocks->at(i_cell2)->x += delta;
                            if(lBlocks->at(i_cell2)->type >= GROUP_BLOCK)
                                i_cell2 += ((EjGroupBlock*)lBlocks->at(i_cell2))->m_counts;
                            i_cell2++;
                        }

                    }
                    else
                    {
                        i_cell2 = i_str_cell;
                        while(i_cell2 <= i)
                        {
                            lBlocks->at(i_cell2)->x += delta;
                            if(lBlocks->at(i_cell2)->type >= GROUP_BLOCK)
                                i_cell2 += ((EjGroupBlock*)lBlocks->at(i_cell2))->m_counts;
                            i_cell2++;
                        }

                    }
                    x = x_cell;
                    i_str_cell = i;
                }
                lBlocks->at(i)->x = x;
                lBlocks->at(i)->y = baseY + k*(cellHeight + interval);

                x += lBlocks->at(i)->width;
                if(lBlocks->at(i)->type > GROUP_BLOCK && ((EjGroupBlock*)lBlocks->at(i))->m_counts > 0)
                    i += ((EjGroupBlock*)lBlocks->at(i))->m_counts;


            }
            curCellBlock->txtHeight = (k + 1)*(cellHeight + interval);

            if(align_tmp & Qt::AlignRight)
            {
                delta = x_cell + cellWidth - spacing * 2 - x; // lBlocks->at(i)->x;
                i_cell2 = i_str_cell;
                while(i_cell2 <= i)
                {
                    lBlocks->at(i_cell2)->x += delta;
                    if(lBlocks->at(i_cell2)->type >= GROUP_BLOCK)
                        i_cell2 += ((EjGroupBlock*)lBlocks->at(i_cell2))->m_counts;
                    i_cell2++;
                }
            }
            if(align_tmp & Qt::AlignHCenter)
            {
                delta = (x_cell + cellWidth - spacing * 2 - x) / 2; // lBlocks->at(i)->x;
                i_cell2 = i_str_cell;
                while(i_cell2 <= i)
                {
                    lBlocks->at(i_cell2)->x += delta;
                    if(lBlocks->at(i_cell2)->type >= GROUP_BLOCK)
                        i_cell2 += ((EjGroupBlock*)lBlocks->at(i_cell2))->m_counts;
                    i_cell2++;
                }
            }
            if(max_k < k)
                max_k = k;
            x = x_cell + lBlocks->at(i_cell)->width;
            k++;
            if(curCellBlock->mergeColums > 0 && curCellBlock->mergeRows > 0)
            {
                if(!rowsHeight.contains(curCellBlock->mergeRows - 1 + row) || rowsHeight.value(curCellBlock->mergeRows - 1 + row) < k * (cellHeight + interval))
                    rowsHeight.insert(curCellBlock->mergeRows - 1 + row,k * (cellHeight + interval));

            }
            else if(rowHeight < k * (cellHeight + interval))
                rowHeight = k * (cellHeight + interval);
            curCellBlock->width = cellWidth;

        }

        delta = 0;
        i = i_str;
        max_k++;

        for (QMap<int, int>::const_iterator it = rowsHeight.cbegin(), end = rowsHeight.cend(); it != end; ++it)
        {
            if(it.key() >= row)
            {
                rowsHeight[it.key()] -= rowHeight;
                if(rowsHeight[it.key()] <= 0) {
                    rowsHeight[it.key()] = 0;
                }
            }
        }

        for(int colum = 0; colum < this->nColums(); colum++)
        {
            if(i < lBlocks->count() - 1)
                i++;
            else
                break;
            lBlocks->at(i)->ascent = rowHeight; // max_k * (cellHeight + interval); // + spacing*2;
            if(lBlocks->at(i)->type == BASECELL)
            {
                curCellBlock = dynamic_cast<EjCellBlock*>(lBlocks->at(i));
            }
            while(i < endBlock() && lBlocks->at(i+1)->type != BASECELL)
            {
                //                    delta++;
                //                    i = indStartTable + delta;
                if(lBlocks->at(i)->type > GROUP_BLOCK && ((EjGroupBlock*)lBlocks->at(i))->m_counts > 0)
                    i += ((EjGroupBlock*)lBlocks->at(i))->m_counts - 1;

                i++;
            }
        }
        baseY += rowHeight;
        rowHeight = cellHeight + interval;
    }

    i = i_str_cell = startCell();

    int height_tmp;
    int colum_back = 0, row_back = 0;
    curPrgStyle = calcParams->paragraphStyle;
    for(int row = 0; row < this->nRows() - d; row++)
    {
        for(int colum = 0; colum < this->nColums(); colum++)
        {
            if(i < 0 || i > m_doc->lBlocks->count() - 1)
                break;
            curCellBlock = dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(i_str_cell));
            height_tmp = curCellBlock->ascent;
            align_tmp = curPrgStyle->m_align;
            if(curCellBlock->mergeRows > 0 && curCellBlock->mergeColums > 0 && i != i_str_cell)
            {
                int index = i_str_cell;
                int colum_tmp = colum_back;
                int row_tmp = row_back;
                while(row_tmp < row_back + curCellBlock->mergeRows)
                {
                    index = nextCell(index);
                    if(index < 0 || m_doc->lBlocks->at(index)->type != BASECELL)
                        break;
                    if(colum_tmp == 0 && row_tmp > row_back)
                    {
                        height_tmp += m_doc->lBlocks->at(index)->ascent;
                    }
                    colum_tmp++;
                    if((colum_tmp > nColums() - 1 && colum_back == 0 && row_tmp < row_back + curCellBlock->mergeRows - 1)
                            || (colum_tmp >= colum_back && colum_tmp < colum_back + curCellBlock->mergeColums
                            && colum_tmp <= nColums()))
                    {
                        dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(index))->visible = false;
                        m_doc->lBlocks->at(index)->parent = curCellBlock;
                        while(index < endBlock() && m_doc->lBlocks->at(index + 1)->type != BASECELL)
                        {
                            index++;
                            m_doc->lBlocks->at(index)->width = 0;
                        }
                    }
                    else {
                    }
                    if(colum_tmp > nColums() - 1)
                    {
                        row_tmp++;
                        colum_tmp = 0;
                        if(row_tmp >= row_tmp + curCellBlock->mergeRows)
                            break;
                    }
                }
            }
            if(align_tmp & Qt::AlignBottom)
            {
                delta = height_tmp; // - curCellBlock->txtHeight;
                cur_block = m_doc->lBlocks->at(i_str_cell + 1);
                while(i_str_cell + 1 < i)
                {
                    if(!cur_block->isProperty())
                    {
                        cur_block->y += (delta - curCellBlock->txtHeight);
                    }
                    if(cur_block->type > GROUP_BLOCK)
                        i_str_cell += ((EjGroupBlock*)cur_block)->m_counts;
                    i_str_cell++;
                    cur_block = m_doc->lBlocks->at(i_str_cell + 1);
                }
            }
                if(align_tmp & Qt::AlignVCenter)
                {
                    delta = height_tmp; // - curCellBlock->txtHeight;
                    cur_block = m_doc->lBlocks->at(i_str_cell + 1);
                    while(i_str_cell + 1 < i)
                    {
                        if(!cur_block->isProperty())
                        {
                            cur_block->y += (delta - curCellBlock->txtHeight) / 2;
                        }
                        if(cur_block->type > GROUP_BLOCK)
                            i_str_cell += ((EjGroupBlock*)cur_block)->m_counts;
                        i_str_cell++;
                        cur_block = m_doc->lBlocks->at(i_str_cell + 1);
                    }
                }
                curCellBlock->ascent = height_tmp;

                i_str_cell = i;
                colum_back = colum;
                row_back = row;
            if(m_doc->lBlocks->at(i)->type == BASECELL && i < endBlock())
                i++;
            while(i < endBlock() && m_doc->lBlocks->at(i)->type != BASECELL)
            {
                if(lBlocks->at(i)->type == NUM_STYLE)
                {
                    EjNumStyleBlock *numStyle = dynamic_cast<EjNumStyleBlock*>(lBlocks->at(i));
                    if(numStyle && numStyle->style && numStyle->style->m_vid == PARAGRAPH_STYLE)
                    {
                        curPrgStyle = dynamic_cast<EjParagraphStyle*>(dynamic_cast<EjNumStyleBlock*>(lBlocks->at(i))->style);
                    }
//                    continue;
                }
                else if(m_doc->lBlocks->at(i)->type > GROUP_BLOCK)
                    i += ((EjGroupBlock*)m_doc->lBlocks->at(i))->m_counts;
                i++;
            }

        }

    }
    curString->height = this->ascent = baseY - baseY_back;
}

bool EjTableBlock::isSelected(int &index, int &startSelect, int &endSelect)
{
    int row,colum;
	QList<EjBlock*> *lBlocks = m_doc->lBlocks;
    if(startSelect != m_startSelectBlock)
    {
        m_startSelectBlock = startSelect;
        m_rowStartSelect = m_columStartSelect = -1;
        if(m_startSelectBlock > -1)
            cellParams(lBlocks->at(m_startSelectBlock),m_rowStartSelect,m_columStartSelect, lBlocks);
    }
    if(endSelect != m_endSelectBlock)
    {
        m_endSelectBlock = endSelect;
        m_rowEndSelect = m_columEndSelect = -1;
        if(m_endSelectBlock > -1)
            cellParams(lBlocks->at(m_endSelectBlock),m_rowEndSelect,m_columEndSelect, lBlocks);
    }
    cellParams(lBlocks->at(index),row,colum, lBlocks);
    return row >= m_rowStartSelect && row <= m_rowEndSelect && colum >= m_columStartSelect && colum <= m_columEndSelect;
}

void EjTableBlock::addString(EjTextControl *control, EjBlock *curBlock, bool force)
{
    if(!curBlock)
        return;
    EjCellBlock *curCell, *curCell2, *curCell3;
    EjNumStyleBlock *curNumStyle;
    EjCellStyle *curCellStyle, *oldCellStyle;
	QList<EjBlock*> *lBlocks = control->doc->lBlocks;
	EjPropIntBlock *propInt;
    EjCellStyle *curCellStyle2 = nullptr;
    EjCellStyle tmpStyle;

    int row = 0;
    int colum = 0;
    int d = 0;
    int index, index2, insertIndex;
    int j, k;
    bool bIncrease = false;

    if(this->vid == EjTableBlock::SHOP_LIST)
        d = 1;
    cellParams(curBlock,row,colum, lBlocks);
    if(!force)
        control->updateFormulas(row + 1, 0, true, true,false, this);
    j = index = insertIndex = cellIndex(row,0,m_doc->lBlocks);
    oldCellStyle = NULL;
    while(j > m_index)
    {
        if(lBlocks->at(j)->type == NUM_STYLE && ((EjNumStyleBlock*)lBlocks->at(j))->style->m_vid == CELL_STYLE)
        {
            //Add makeCopy in the end
            curNumStyle = (EjNumStyleBlock*)lBlocks->at(j)->makeCopy();
            oldCellStyle = curCellStyle2 = (EjCellStyle*)curNumStyle->style;
            break;
        }
        j--;
    }
    index2 = cellIndex(row,nColums() - 1,m_doc->lBlocks);
    index2 = nextCell(index2);
    curCell3 = dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(index2));
    curCell = nullptr;
    k = 1;
    for(int i = 0; i < this->nColums(); i++)
    {
        curCell2 = dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(index));
        curCellStyle = curCell2->cellStyle;
        tmpStyle = *curCellStyle;
        if(row == nRows() - 1)
        {
            *tmpStyle.bottomBorder() = *tmpStyle.topBorder();
            if(curCellStyle2 != fromStyles(&tmpStyle))
            {
                curCellStyle2 = fromStyles(&tmpStyle);
                curCellStyle2->m_doc = m_doc;
                curCell2->cellStyle = curCellStyle2;
                //Add makeCopy in the end
                setCellStyle(curCell2,curCellStyle2);
            }
            else
                clearCellStyle(curCell2);
            if(curCell)
            {
                index2 = m_doc->lBlocks->indexOf(curCell) + k;
            }
            else
            {
                index2 = cellIndex(row,nColums() - 1,m_doc->lBlocks);
                index2 = nextCell(index2);
            }
        }
        else if(row == 0 && curCell3)
        {
            *tmpStyle.topBorder() = *curCell3->cellStyle->topBorder();
            curCellStyle = fromStyles(&tmpStyle);
            curCell3 = nextCell(curCell3);
        }

        curCell = new EjCellBlock();
        curCell->cellStyle = curCellStyle;
        curCell->parent = this;
        m_doc->lBlocks->insert(index2,curCell);
        index2++;
        k = 1;
        m_counts++;
        if(curCellStyle && curCellStyle != oldCellStyle)
        {
            curNumStyle = new EjNumStyleBlock();
            curNumStyle->style = curCellStyle;
            curNumStyle->num = curCellStyle->num;
            curNumStyle->parent = this;
            m_doc->lBlocks->insert(index2,curNumStyle);
            index2++;
            k++;
            m_counts++;
            oldCellStyle = curCellStyle;
        }
        index++;
        while(m_doc->lBlocks->at(index)->type != BASECELL)
            index++;
    }
    this->lRows.append(new EjSizeProp());
	propInt = (EjPropIntBlock*)findProp(m_doc->lBlocks, PROP_INT, TBL_ROWS);
    if(propInt)
        propInt->value = this->lRows.count();

    if(!force)
        control->calcTables();

    control->doc->tableStringsInserted(this,row,row);

}

void EjTableBlock::addColum(EjTextControl *control, EjBlock *curBlock)
{
    EjCellBlock *curCell, *curCell2;
    int colum = 0;
    int row = 0;
    int index;
    EjNumStyleBlock *curNumStyle;
	EjPropIntBlock *propInt;
    EjCellStyle *curCellStyle;
    EjCellStyle *curCellStyle2;
    EjCellStyle tmpStyle;


    if(this->vid == EjTableBlock::SHOP_LIST)
        return;
    cellParams(curBlock,row,colum, control->doc->lBlocks);
    control->updateFormulas(colum,0, false, true, false, this);
    for(int i = 0; i < this->nRows(); i++)
    {
        index = this->cellIndex(i,colum + i,control->doc->lBlocks);
        curCell = dynamic_cast<EjCellBlock*>(control->doc->lBlocks->at(index));
        curCellStyle = curCell->cellStyle;
        if(colum == this->nColums() - 1)
        {
            tmpStyle = *curCellStyle;
            *tmpStyle.rightBorder() = *tmpStyle.leftBorder();
            curCellStyle2 = fromStyles(&tmpStyle);
            curCellStyle2->m_doc = m_doc;
            curCell->cellStyle = curCellStyle2;
            if(curCellStyle != curCellStyle2)
                setCellStyle(curCell,curCellStyle2);
        }
        else if(colum == 0 && this->nColums() > 0)
        {
            curCell2 = nextCell(curCell);
            tmpStyle = *curCellStyle;
            *tmpStyle.leftBorder() = *curCell2->cellStyle->leftBorder();
            curCellStyle = fromStyles(&tmpStyle);
            curCellStyle->m_doc = m_doc;
        }
        index = nextCell(index);
        control->doc->lBlocks->insert(index,new EjCellBlock());
        m_counts++;
        curCell2 = dynamic_cast<EjCellBlock*>(control->doc->lBlocks->at(index));
        curCell2->parent = this;
        curCell2->cellStyle = curCellStyle;

        index++;
        if(curCell2->cellStyle != curCell->cellStyle)
        {
            curNumStyle = new EjNumStyleBlock();
            curNumStyle->style = curCellStyle;
            curNumStyle->num = curCellStyle->num;
            curNumStyle->parent = this;
            control->doc->lBlocks->insert(index,curNumStyle);
            index++;
            m_counts++;
        }

    }
    control->activeIndex += row;
    this->lColums.append(new ColumProp());
	propInt = (EjPropIntBlock*)findProp(control->doc->lBlocks, PROP_INT, TBL_COLUMS);
    if(propInt)
        propInt->value = this->lColums.count();


    control->calcTables();

    control->doc->tableColumsInserted(this,colum,colum);

}

void EjTableBlock::delString(QList<EjBlock *> *l_blocks, int &active_block)
{
    EjCellBlock *curCell, *curCell2;
    EjTableBlock *curTable = nullptr;
    EjSizeProp *sizeProp;
	EjPropIntBlock *propInt;

    int row = 0;
    int colum = 0;
	int d = 0;
    int index;
    int index2;

    EjCellStyle *curCellStyle;
    EjCellStyle *curCellStyle2;
    EjCellStyle tmpStyle;

    if(active_block < 0 || active_block > l_blocks->count() - 2)
        return;

    while(l_blocks->at(active_block)->type != BASECELL)
        active_block--;

    curCell = dynamic_cast<EjCellBlock*>(l_blocks->at(active_block));
    curTable = ((EjTableBlock*)(curCell->parent));

    if(curTable == this)
    {
        if(curTable->vid == EjTableBlock::SHOP_LIST)
            d = 1;
        cellParams(curCell,row,colum, l_blocks);

        if(d && curTable->nRows() == 2)
        {
            while(curTable->endBlock() > curTable->startCell())
            {
                delete l_blocks->takeAt(curTable->startCell() );
                    curTable->m_counts--;
            }
            delete l_blocks->takeAt(curTable->startCell());
            curTable = 0;
            return;
        }

        if(d && (row == 0 || row > curTable->nRows() - 2))
            return;
        index = index2 = cellIndex(row,0, l_blocks);
        if(row == curTable->nRows() - 1 && row > 0)
        {
            index2 = cellIndex(row - 1,0, l_blocks);
        }
        else if(row == 0 && curTable->nRows() > 1)
        {
            index2 = cellIndex(row + 1,0, l_blocks);
        }
        curCellStyle = nullptr;
        for(int i = 0; i < curTable->nColums(); i++)
        {
            if(row == curTable->nRows() - 1 && row > 0)
            {
                curCell = dynamic_cast<EjCellBlock*>(l_blocks->at(index));
                curCell2 = dynamic_cast<EjCellBlock*>(l_blocks->at(index2));
                index = cellIndex(row,0, l_blocks);
                index2 = nextCell(index2);
            }
            else if(row == 0 && curTable->nRows() > 1)
            {
                curCell = dynamic_cast<EjCellBlock*>(l_blocks->at(index));
                curCell2 = dynamic_cast<EjCellBlock*>(l_blocks->at(index2));
                index = cellIndex(row,0, l_blocks);
                index2 = nextCell(index2);

            }
            delete l_blocks->takeAt(index);
            curTable->m_counts--;
            if(row == 0)
                index2--;

            while(l_blocks->at(index)->type != BASECELL && index < endBlock())
            {
                delete l_blocks->takeAt(index);
                curTable->m_counts--;
                if(row == 0)
                    index2--;
            }
        }
        active_block = index;
        if(active_block < 0)
        {
            active_block = -1;
        }
        sizeProp = curTable->lRows.takeLast();
        delete sizeProp;
		propInt = (EjPropIntBlock*)findProp(l_blocks, PROP_INT, TBL_ROWS);
        if(propInt)
            propInt->value = this->lRows.count();

        this->m_doc->tableStringsRemoved(curTable,row,row);

    }

}

void EjTableBlock::delColum(QList<EjBlock *> *l_blocks, int &active_block)
{
	EjCellBlock *curCell, *curCell2, *curCell3;
    EjTableBlock *curTable = 0;
    ColumProp *columProp;
	EjPropIntBlock *propInt;

    int row = 0;
    int colum = 0;

    int d = 0;
    int index, index2, index3;

    EjCellStyle tmpStyle;

    if(active_block < 0 || active_block > l_blocks->count() - 2)
        return;

    while(l_blocks->at(active_block)->type != BASECELL)
        active_block--;

    curCell = (EjCellBlock*)l_blocks->at(active_block);
    curTable = ((EjTableBlock*)(curCell->parent));

    if(curTable == this)
    {
        if(curTable->vid == EjTableBlock::SHOP_LIST)
            return;
        cellParams(curCell,row,colum, l_blocks);

        active_block -= row;

        for(int i = 0; i < curTable->nRows(); i++)
        {
            index = cellIndex(i,colum - i, l_blocks);

            if(colum == curTable->nColums() - 1 && colum > 0)
            {
                index2 = index3 = prevCell(index);
                if(curTable->nColums() > 2)
                    index3 = prevCell(index2);
                curCell = dynamic_cast<EjCellBlock*>(l_blocks->at(index));
                curCell2 = dynamic_cast<EjCellBlock*>(l_blocks->at(index2));
                curCell3 = dynamic_cast<EjCellBlock*>(l_blocks->at(index3));
                index = cellIndex(i,colum - i, l_blocks);
            }
            else if(colum == 0 && curTable->nColums() > 1)
            {
                index2 = nextCell(index);
                index3 = index;
                if(i > 0)
                    index3 = prevCell(index);
                curCell = dynamic_cast<EjCellBlock*>(l_blocks->at(index));
                curCell2 = dynamic_cast<EjCellBlock*>(l_blocks->at(index2));
                curCell3 = dynamic_cast<EjCellBlock*>(l_blocks->at(index3));
            }

            delete l_blocks->takeAt(index);
                curTable->m_counts--;

            while(l_blocks->at(index)->type != BASECELL && l_blocks->at(index)->type != END_GROUP)
            {
                delete l_blocks->takeAt(index);
                    curTable->m_counts--;
            }
        }
        columProp = curTable->lColums.takeLast();
        delete columProp;
		propInt = (EjPropIntBlock*)findProp(l_blocks, PROP_INT, TBL_COLUMS);
        if(propInt)
            propInt->value = this->lColums.count();

        this->m_doc->tableColumsRemoved(curTable,colum,colum);

    }
}

void EjTableBlock::moveString(EjTextControl *control, EjBlock *curBlock, bool isUp)
{
    int row = 0;
    int colum = 0;
    int d = 0;
    int index, index2;
    int new_index;
    EjCellBlock *curCell, *curCell2;
    EjCellStyle *curCellStyle = nullptr;
    EjCellStyle *curCellStyle2 = nullptr;
    EjCellStyle *curCellStyle_tmp;
    EjCellStyle tmpStyle;
    EjBorderStyle borderStyle;


    if(this->vid == EjTableBlock::SHOP_LIST)
        d = 1;

    cellParams(curBlock,row,colum, control->doc->lBlocks);

    if(row == 0 && nRows() > 1)
    {
        index = cellIndex(1,0,m_doc->lBlocks);
        curCell = dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(index));
        moveString(control,curCell, true);
        return;
    }
    else if(row == nRows() - 2 && row > 0)
    {
        index = cellIndex(nRows() - 1,0,m_doc->lBlocks);
        curCell = dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(index));
        moveString(control,curCell, true);
        return;
    }

    if(isUp && (row < d + 1 || row > this->nRows() - d - 1))
        return;
    if(!isUp && (row < d || row > this->nRows() - d - 2) )
        return;
    if(isUp)
        control->updateFormulas(row, row - 1, true, false, true, this);
    else
        control->updateFormulas(row, row + 1, true, false, true, this);
    for(int i = 0; i < this->nColums(); i++)
    {

        if(isUp)
        {
            index = cellIndex(row,i,m_doc->lBlocks);
            new_index = cellIndex(row - 1,i,m_doc->lBlocks);
            index2 = cellIndex(row - 1,i*2,m_doc->lBlocks);
            curCell = dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(index));
            curCell2 = dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(index2));
            if(row == 1)
            {
                tmpStyle = *curCell->cellStyle;
                borderStyle = *tmpStyle.topBorder();
                *tmpStyle.topBorder() = *curCell2->cellStyle->topBorder();
                curCellStyle_tmp = fromStyles(&tmpStyle);
                curCellStyle_tmp->m_doc = m_doc;
                if(i == 0 || curCellStyle != curCellStyle_tmp)
                {
                    setCellStyle(curCell,curCellStyle_tmp);
                    curCellStyle = curCellStyle_tmp;
                }
                else
                    clearCellStyle(curCell);

                tmpStyle = *curCell2->cellStyle;
                *tmpStyle.topBorder() = borderStyle;
                curCellStyle_tmp = fromStyles(&tmpStyle);
                curCellStyle_tmp->m_doc = m_doc;
                if(i == 0 || curCellStyle2 != curCellStyle_tmp)
                {
                    setCellStyle(curCell2,curCellStyle_tmp);
                    curCellStyle2 = curCellStyle_tmp;
                }
                else
                    clearCellStyle(curCell2);
                index = cellIndex(row,i,m_doc->lBlocks);
            }
            else if(row == nRows() - 1 && row > 0)
            {
                tmpStyle = *curCell->cellStyle;
                borderStyle = *tmpStyle.bottomBorder();
                *tmpStyle.bottomBorder() = *curCell2->cellStyle->bottomBorder();
                curCellStyle_tmp = fromStyles(&tmpStyle);
                curCellStyle_tmp->m_doc = m_doc;
                if(i == 0 || curCellStyle != curCellStyle_tmp)
                {
                    setCellStyle(curCell,curCellStyle_tmp);
                    curCellStyle = curCellStyle_tmp;
                }
                else
                    clearCellStyle(curCell);

                tmpStyle = *curCell2->cellStyle;
                *tmpStyle.bottomBorder() = borderStyle;
                curCellStyle_tmp = fromStyles(&tmpStyle);
                curCellStyle_tmp->m_doc = m_doc;
                if(i == 0 || curCellStyle2 != curCellStyle_tmp)
                {
                    setCellStyle(curCell2,curCellStyle_tmp);
                    curCellStyle2 = curCellStyle_tmp;
                }
                else
                    clearCellStyle(curCell2);
                index = cellIndex(row,i,m_doc->lBlocks);
            }
            control->doc->lBlocks->move(index,new_index);
            index++;
            new_index++;
            while(control->doc->lBlocks->at(index)->type != BASECELL && control->doc->lBlocks->at(index)->type != END_GROUP)
            {
                control->doc->lBlocks->move(index,new_index);
                index++;
                new_index++;
            }
        }
        else
        {
            index = cellIndex(row,0,m_doc->lBlocks);
            new_index = cellIndex(row+2,-1,m_doc->lBlocks);
            while(control->doc->lBlocks->at(new_index + 1)->type != BASECELL && control->doc->lBlocks->at(new_index + 1)->type != END_GROUP)
                new_index++;

            control->doc->lBlocks->move(index,new_index);
            while(control->doc->lBlocks->at(index)->type != BASECELL && control->doc->lBlocks->at(index)->type != END_GROUP)
            {
                control->doc->lBlocks->move(index,new_index);
            }
        }

    }
    if(d)
        control->updateShopList(this);
    if(isUp)
        control->activeIndex -= this->nColums();
    else
        control->activeIndex += this->nColums();

    control->calcTables();

}

void EjTableBlock::moveColum(EjTextControl *control, EjBlock *curBlock, bool isLeft)
{
    int row = 0;
    int colum = 0;
    int d = 0;
    //    int start;
    int index;
    int new_index;
    EjCellBlock *curCell, *curCell2;
    EjCellStyle *curCellStyle = nullptr;
    EjCellStyle *curCellStyle2 = nullptr;
    EjCellStyle *curCellStyle_tmp;
    EjCellStyle tmpStyle;
    EjBorderStyle borderStyle;

    if(this->vid == EjTableBlock::SHOP_LIST)
        d = 1;

    cellParams(curBlock,row,colum, control->doc->lBlocks);

    if(isLeft && (colum < d + 1 || colum > this->nColums() - d - 1))
        return;
    if(!isLeft && (colum < d || colum > this->nColums() - d - 2) )
        return;
    if(isLeft)
        control->updateFormulas(colum, colum - 1, false, false, true, this);
    else
        control->updateFormulas(colum, colum + 1, false, false, true, this);

    for(int i = 0; i < this->nRows(); i++)
    {
        if(isLeft)
        {
            index = cellIndex(i,colum,m_doc->lBlocks);
        }
        else
            index = cellIndex(i,colum+1,m_doc->lBlocks);

        new_index = index - 1;
        while(control->doc->lBlocks->at(new_index)->type != BASECELL && control->doc->lBlocks->at(new_index)->type != END_GROUP)
            new_index--;
        curCell = dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(index));
        curCell2 = dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(new_index));
        if((isLeft && colum == 1) || (!isLeft && colum == 0) )
        {
            tmpStyle = *curCell->cellStyle;
            borderStyle = *tmpStyle.leftBorder();
            *tmpStyle.leftBorder() = *curCell2->cellStyle->leftBorder();
            curCellStyle_tmp = fromStyles(&tmpStyle);
            curCellStyle_tmp->m_doc = m_doc;
			setCellStyle(curCell,curCellStyle_tmp);

            tmpStyle = *curCell2->cellStyle;
            *tmpStyle.leftBorder() = borderStyle;
            curCellStyle_tmp = fromStyles(&tmpStyle);
            curCellStyle_tmp->m_doc = m_doc;
			setCellStyle(curCell2,curCellStyle_tmp);
                if(isLeft)
                {
                    index = cellIndex(i,colum,m_doc->lBlocks);
                }
                else
                    index = cellIndex(i,colum+1,m_doc->lBlocks);
        }
        control->doc->lBlocks->move(index,new_index);
        index++;
        new_index++;
        while(control->doc->lBlocks->at(index)->type != BASECELL && control->doc->lBlocks->at(index)->type != END_GROUP)
        {
            control->doc->lBlocks->move(index,new_index);
            index++;
            new_index++;
        }
    }
    if(d)
        control->updateShopList(this);
    if(isLeft)
        control->activeIndex -= 1;
    else
        control->activeIndex += 1;

    control->calcTables();

}

int EjTableBlock::cellIndex(int row, int colum, QList<EjBlock *> *l_blocks)
{
    int table_index = -1;
    int res = -1;
    int index = row * this->nColums() + colum;
	QList<EjBlock *> *lBlocks = l_blocks;
    if(!lBlocks && m_doc)
        lBlocks = m_doc->lBlocks;

    if(!lBlocks)
        return -1;

    int i = this->m_index; //startCell();
    while(lBlocks->at(i)->type != BASECELL)
        i++;
    while(i < lBlocks->count())
    {
        if(lBlocks->at(i)->type == BASECELL)
        {
            table_index++;
            if(table_index >= index)
            {
                res = i;
                break;
            }
        }
        i++;
    }
    return res;
}

int EjTableBlock::cellIndex(int row, int colum)
{
	QList<EjBlock *> *lBlocks = nullptr;
    if(m_doc)
        lBlocks = m_doc->lBlocks;
    return cellIndex(row, colum, lBlocks);
}

int EjTableBlock::prevCell(int index)
{
    if(m_doc->lBlocks->at(index)->type == BASECELL ||  m_doc->lBlocks->at(index)->type == END_GROUP)
        index--;
    while(index > 0 && m_doc->lBlocks->at(index)->type != BASECELL) {
        if(m_doc->lBlocks->at(index)->type == EXT_TABLE)
            return -1;
        index--;
    }
    return index;
}

int EjTableBlock::currCellIndex(int index)
{
    if(m_doc->lBlocks->at(index)->type == BASECELL)
        return index;
    return prevCell(index);

}

EjCellBlock *EjTableBlock::currentCell(int index)
{
    EjCellBlock *res = nullptr;
    index = currCellIndex(index);
    if(index > 0)
        res = dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(index));
    return res;
}

EjCellBlock *EjTableBlock::prevCell(EjBlock *block)
{
    if(!m_doc)
        return nullptr;
    EjCellBlock *cellBlock = nullptr;
    int index = m_doc->lBlocks->indexOf(block);
    if(index > -1){
        index = prevCell(index);
        if(m_doc->lBlocks->at(index)->type == BASECELL)
        {
            cellBlock = dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(index));
        }
    }
    return cellBlock;
}

int EjTableBlock::nextCell(int index)
{
    if(index >= m_doc->lBlocks->count())
        return -1;
    if(m_doc->lBlocks->at(index)->type == BASECELL)
        index++;
    while(index < endBlock() && m_doc->lBlocks->at(index)->type != BASECELL && m_doc->lBlocks->at(index)->type != END_GROUP)
    {
        qDebug() << "Index: " << index;
        index++;
        qDebug() << "Index: " << index;
    }
    return index;
}

EjCellBlock *EjTableBlock::nextCell(EjBlock *block)
{
    if(!m_doc)
        return nullptr;
    EjCellBlock *cellBlock = nullptr;
    int index = m_doc->lBlocks->indexOf(block);
    if(index > -1){
        index = nextCell(index);
        if(m_doc->lBlocks->at(index)->type == BASECELL)
        {
            cellBlock = dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(index));
        }
    }
    return cellBlock;
}

bool EjTableBlock::chekMinMax(int &startRow, int &startColum, int &endRow, int &endColum, EjCellBlock *cell)
{
    EjCellBlock *mergeCell = NULL;
    bool bChange = false;
    if(cell->visible == false)
    {
        mergeCell = dynamic_cast<EjCellBlock*>(cell->parent);
    }
    if(cell->mergeRows > 0 && cell->mergeColums > 0)
    {
        mergeCell = cell;
    }
    if(mergeCell)
    {
        int row_tmp;
        int colum_tmp;
        cellParams(mergeCell,row_tmp,colum_tmp,m_doc->lBlocks);
        if(row_tmp < startRow)
        {
            startRow = row_tmp;
            bChange = true;
        }
        if(colum_tmp < startColum)
        {
            startColum = colum_tmp;
            bChange = true;
        }
        row_tmp += mergeCell->mergeRows - 1;
        colum_tmp += mergeCell->mergeColums - 1;
        if(row_tmp > endRow)
        {
            endRow = row_tmp;
        }
        if(colum_tmp > endColum)
        {
            endColum = colum_tmp;
        }
    }
    return bChange;
}


void EjTableBlock::setCellStyles(int startRow, int startColum, int endRow, int endColum, EjCellStyle *style)
{
    int row = startRow;
    int colum = startColum;

    int index;
    EjCellStyle *oldStyleCell, *oldWriteStyleCell, *curStyleCell;
    EjCellStyle tmpStyle;
    EjCellBlock *curCell;
	EjBlock *curBlock;
    EjNumStyleBlock *curNumStyle;
    bool isChangeStyle;
    bool isWrite;
    bool bChange;
    EjCellBlock *mergeCell;

    index = cellIndex(row,colum,m_doc->lBlocks);
    if(index < 0)
        return;
    curCell = (EjCellBlock*)m_doc->lBlocks->at(index);
    oldStyleCell = curStyleCell = getCellStyle(index); //   curCell->cellStyle;

    while(index < endBlock())
    {
        tmpStyle = *style;
        tmpStyle.horisontalLine()->m_width = -1;
        tmpStyle.verticalLine()->m_width = -1;

        isChangeStyle = false;
        curCell = (EjCellBlock*)m_doc->lBlocks->at(index);
        if(row <= endRow && colum >= startColum && colum <= endColum)
        {
            bChange = false;
            bChange = chekMinMax(startRow,startColum,endRow,endColum,curCell);
            if(bChange)
            {
                index = cellIndex(startRow,startColum,m_doc->lBlocks);
                curCell = (EjCellBlock*)m_doc->lBlocks->at(index);
                oldStyleCell = curStyleCell = getCellStyle(index);

                continue;
            }
        }
        curStyleCell = curCell->cellStyle;

        if(!curStyleCell)
        {
            isChangeStyle = true;
        }
        else if(row > endRow && colum >= startColum && colum <= endColum + 1)
        {
            if(colum <= endColum)
            {
                *tmpStyle.leftBorder() = *curStyleCell->leftBorder();
                *tmpStyle.rightBorder() = *curStyleCell->rightBorder();
                *tmpStyle.bottomBorder() = *curStyleCell->bottomBorder();
                *tmpStyle.topBorder() = *style->bottomBorder();
            }
            else
                tmpStyle = *curStyleCell;
            tmpStyle.m_brushColor = curStyleCell->m_brushColor;
            isChangeStyle = true;
        }
        else if(row >= startRow && row <= endRow && colum == endColum + 1)
        {
            *tmpStyle.leftBorder() = *tmpStyle.rightBorder();
            *tmpStyle.rightBorder() = *curStyleCell->rightBorder();
            *tmpStyle.bottomBorder() = *curStyleCell->bottomBorder();
            *tmpStyle.topBorder() = *curStyleCell->topBorder();
            tmpStyle.m_brushColor = curStyleCell->m_brushColor;
            isChangeStyle = true;
        }
        else if(row >= startRow && row <= endRow && colum >= startColum && colum <= endColum)
        {
            if(colum != startColum)
                *tmpStyle.leftBorder() = *style->verticalLine();
            if(colum + curCell->mergeColums < this->lColums.count() - 1)
                tmpStyle.rightBorder()->m_width = -1;

            if(curCell->mergeRows > 0) {
                if(row + curCell->mergeRows < this->lRows.count())
                        tmpStyle.bottomBorder()->m_width = -1;
            }
            else if(row < this->lRows.count() - 1)
                        tmpStyle.bottomBorder()->m_width = -1;

            if(row != startRow)
                *tmpStyle.topBorder() = *style->horisontalLine();
            isChangeStyle = true;
        }
        else if(colum == endColum + 2)
        {
            tmpStyle = *curStyleCell;
            isChangeStyle = true;
        }
        if(isChangeStyle)
        {
            curStyleCell = fromStyles(&tmpStyle);
            curStyleCell->m_doc = m_doc;
			curCell->cellStyle = curStyleCell;
            if(!oldStyleCell->fullCompare(curStyleCell) || (row == 0 && colum == 0))
            {
                index++;
                curNumStyle = new EjNumStyleBlock();
                curNumStyle->num = m_doc->lStyles->indexOf(curStyleCell);
                curNumStyle->style = curStyleCell;
                curNumStyle->parent = this;
                m_doc->lBlocks->insert(index,curNumStyle);
                oldStyleCell = curStyleCell;
                m_counts++;
            }
        }

        index++;
        while(index < endBlock() && m_doc->lBlocks->at(index)->type != BASECELL)
        {
            curBlock = m_doc->lBlocks->at(index);
            if(curBlock->type == NUM_STYLE && ((EjNumStyleBlock*)curBlock)->style && ((EjNumStyleBlock*)curBlock)->style->m_vid == CELL_STYLE)
            {
                if(isChangeStyle)
                {
                    delete m_doc->lBlocks->takeAt(index);
                    m_counts--;
                    index--;
                }
            }
            index++;
        }
        colum++;
        if(colum > this->lColums.count() - 1)
        {
            colum = 0;
            row++;
        }
        if(row > endRow + 1)
            break;
        oldStyleCell = curStyleCell;
    }

}

void EjTableBlock::setCellStyle(EjCellBlock *cell, EjCellStyle *style)
{
    if(!style)
        return;
    int index =m_doc->lBlocks->indexOf(cell);
    EjCellStyle *curStyleCell = fromStyles(style);
	EjBlock *curBlock;
    cell->cellStyle = curStyleCell;
    index++;
    EjNumStyleBlock *curNumStyle = new EjNumStyleBlock();
    curNumStyle->num = curStyleCell->num;
    curNumStyle->style = curStyleCell;
    curNumStyle->parent = this;
    m_doc->lBlocks->insert(index,curNumStyle);
    m_counts++;
    index++;
    while(index < endBlock() && m_doc->lBlocks->at(index)->type != BASECELL)
    {
        curBlock = m_doc->lBlocks->at(index);
        if(curBlock->type == NUM_STYLE && ((EjNumStyleBlock*)curBlock)->style->m_vid == CELL_STYLE)
        {
            delete m_doc->lBlocks->takeAt(index);
            m_counts--;
            index--;
        }
        index++;
    }
}

void EjTableBlock::clearCellStyle(EjCellBlock *cell)
{
	EjBlock *curBlock;
    int index =m_doc->lBlocks->indexOf(cell);
    index++;
    while(index < endBlock() && m_doc->lBlocks->at(index)->type != BASECELL)
    {
        curBlock = m_doc->lBlocks->at(index);
        if(curBlock->type == NUM_STYLE && ((EjNumStyleBlock*)curBlock)->style->m_vid == CELL_STYLE)
        {
            delete m_doc->lBlocks->takeAt(index);
            m_counts--;
            index--;
        }
        index++;
    }

}


void EjTableBlock::setParagraphStyle(int startRow, int startColum, int endRow, int endColum, EjParagraphStyle *style)
{
    int row = startRow;
    int colum = startColum;

    int index;
    EjParagraphStyle *oldStylePrg, *writeStylePrg, *curStylePrg;
    EjParagraphStyle tmpStyle;
    EjCellBlock *curCell;
	EjBlock *curBlock;
    EjNumStyleBlock *curNumStyle;
    bool isChangeStyle;
    bool isWrite;
    bool bChange;
    bool bFinishStyle;
    EjCellBlock *mergeCell;

    index = cellIndex(row,colum,m_doc->lBlocks);
    if(index < 0)
        return;
    curCell = (EjCellBlock*)m_doc->lBlocks->at(index);
    oldStylePrg = writeStylePrg = m_doc->getParagraphStyle(index);
    bFinishStyle = false;

    while(index < m_doc->lBlocks->count())
    {
        tmpStyle.m_align = style->m_align;

        isChangeStyle = false;
        curCell = (EjCellBlock*)m_doc->lBlocks->at(index);
        bChange = chekMinMax(startRow,startColum,endRow,endColum,curCell);
        if(bChange)
        {
            index = cellIndex(startRow,startColum,m_doc->lBlocks);
            curCell = (EjCellBlock*)m_doc->lBlocks->at(index);
            oldStylePrg = writeStylePrg = m_doc->getParagraphStyle(index);
            continue;
        }
        curStylePrg = oldStylePrg;

        if(bFinishStyle)
        {
            if(curStylePrg != writeStylePrg)
            {
                tmpStyle.m_align = curStylePrg->m_align;
                isChangeStyle = true;
            }

        }
        else if(row >= startRow && row <= endRow && colum >= startColum && colum <= endColum)
        {
            if(style != writeStylePrg)
            {
                tmpStyle.m_align = style->m_align;
                isChangeStyle = true;
                bFinishStyle = false;
                if(colum == endColum)
                    bFinishStyle = true;
            }
        }

        if(isChangeStyle)
        {
            curStylePrg = m_doc->fromParagraphStyles(&tmpStyle);
            {
                index++;
                curNumStyle = new EjNumStyleBlock();
                curNumStyle->num = curStylePrg->num;
                curNumStyle->style = curStylePrg;
                curNumStyle->parent = this;
                m_doc->lBlocks->insert(index,curNumStyle);
                if(!bFinishStyle || colum == endColum)
                    writeStylePrg = curStylePrg;
                else
                    oldStylePrg = curStylePrg;
                m_counts++;
            }
            bFinishStyle = false;
        }

        if(row > endRow)
            break;

        index++;
        while(index < m_doc->lBlocks->count() && m_doc->lBlocks->at(index)->type != BASECELL)
        {
            curBlock = m_doc->lBlocks->at(index);
            if(curBlock->type == NUM_STYLE && ((EjNumStyleBlock*)curBlock)->style->m_vid == PARAGRAPH_STYLE)
            {
                if(isChangeStyle)
                {
                    delete m_doc->lBlocks->takeAt(index);
                    m_counts--;
                    index--;
                }
                else
                {
                    curNumStyle = (EjNumStyleBlock*)curBlock;
                    if(curNumStyle->style && curNumStyle->style->m_vid == CELL_STYLE)
                        oldStylePrg = (EjParagraphStyle*)curNumStyle->style;
                }
            }
            index++;
        }
        colum++;
        if(colum > this->lColums.count() - 1)
        {
            colum = 0;
            row++;
        }
    }

}

void EjTableBlock::setParagraphStyle(EjCellBlock *cell, EjParagraphStyle *style)
{
    if(!style)
        return;
    int index =m_doc->lBlocks->indexOf(cell);
    EjParagraphStyle *curStylePrg = m_doc->fromParagraphStyles(style);
	EjBlock *curBlock;
    index++;
    EjNumStyleBlock *curNumStyle = new EjNumStyleBlock();
    curNumStyle->num = curStylePrg->num;
    curNumStyle->style = curStylePrg;
    curNumStyle->parent = this;
    m_doc->lBlocks->insert(index,curNumStyle);
    m_counts++;
    index++;
    while(index < endBlock() && m_doc->lBlocks->at(index)->type != BASECELL)
    {
        curBlock = m_doc->lBlocks->at(index);
        if(curBlock->type == NUM_STYLE && ((EjNumStyleBlock*)curBlock)->style->m_vid == PARAGRAPH_STYLE)
        {
            delete m_doc->lBlocks->takeAt(index);
            m_counts--;
            index--;
        }
        index++;
    }


}

EjCellStyle *EjTableBlock::getCellStyle(int block)
{
    EjCellStyle *curCellStyle = NULL;
	EjBlock *cur_block;
    int i = block;
    bool bFind = false;

    if(m_doc->lBlocks->isEmpty()) {

    }
    while(i > -1 && !m_doc->lBlocks->isEmpty())
    {
        cur_block = m_doc->lBlocks->at(i);
        if(cur_block->type == NUM_STYLE)
        {
            if(!((EjNumStyleBlock *)cur_block)->style)
            {
                ((EjNumStyleBlock *)cur_block)->style = m_doc->getUndefinedStyle(((EjNumStyleBlock *)cur_block)->num);
            }
            if(((EjNumStyleBlock *)cur_block)->style && ((EjNumStyleBlock *)cur_block)->style->m_vid == CELL_STYLE)
            {
                curCellStyle = (EjCellStyle*)((EjNumStyleBlock *)cur_block)->style;
                break;
            }

        }
        i--;
    }
    if(!curCellStyle) {
        foreach (EjBaseStyle *style, *m_doc->lStyles) {
            if(style->m_vid == CELL_STYLE)
            {
                curCellStyle = (EjCellStyle*)style;
                break;
            }
        }
        if(!curCellStyle) {
            curCellStyle = m_doc->createDefaultCellStyle();
        }
    }
    return curCellStyle;
}


QQuickItem * EjTableBlock::onCellClicked(int statusMode,EjTextControl *control, EjCellBlock *cell, QQuickItem *parent)
{
    QQuickItem *res = nullptr;
    int row, colum = -1;
    cellParams(cell,row,colum);
    return res;
}

QString EjTableBlock::tableName()
{
    QString res;
	EjPropTextBlock *propText = dynamic_cast<EjPropTextBlock*>(findProp(m_doc->lBlocks,PROP_TXT,TBL_NAME));
    if(propText)
        res = propText->text;
    return res;
}

void EjTableBlock::setTableName(QString name)
{
	EjPropTextBlock *propText = dynamic_cast<EjPropTextBlock*>(findProp(m_doc->lBlocks,PROP_TXT,TBL_NAME));
    if(!name.isEmpty())
    {
        if(!propText) {
			propText = new EjPropTextBlock(TBL_NAME);
            m_doc->lBlocks->insert(lastIndexProp(),propText);
            deltaProps++;
            m_counts++;
        }
        propText->text = name;
    }
    else
    {
        remBlock(m_doc->lBlocks,propText);
    }
}

QString EjTableBlock::tableAdditional()
{
    QString res;
	EjPropTextBlock *propText = dynamic_cast<EjPropTextBlock*>(findProp(m_doc->lBlocks,PROP_TXT,TBL_ADDITIONAL));
    if(propText)
        res = propText->text;
    return res;

}

void EjTableBlock::setTableAdditional(QString additional)
{
	EjPropTextBlock *propText = dynamic_cast<EjPropTextBlock*>(findProp(m_doc->lBlocks,PROP_TXT,TBL_ADDITIONAL));
    if(!additional.isEmpty())
    {
        if(!propText) {
			propText = new EjPropTextBlock(TBL_ADDITIONAL);
            m_doc->lBlocks->insert(lastIndexProp(),propText);
            deltaProps++;
            m_counts++;
        }
        propText->text = additional;
    }
    else
    {
        remBlock(m_doc->lBlocks,propText);
    }
}

EjPropByteArrayBlock *EjTableBlock::getNameColumProp(int colum)
{
    if(!m_doc)
        return nullptr;
    quint32 num_colum;
    IntVarLen t(num_colum);
	EjPropByteArrayBlock *propBA = nullptr;
	EjPropBase *curBlock;
    int index;
    for(index = m_index + 1; index < startCell(); index++)
    {
		curBlock = dynamic_cast<EjPropBase*>(m_doc->lBlocks->at(index));
        if(curBlock && curBlock->type == PROP_BA && curBlock->num == TBL_COLUM_NAME)
        {
			propBA = dynamic_cast<EjPropByteArrayBlock*>(curBlock);
            if(propBA)
            {
                QDataStream ds(propBA->data);
                ds >> t;
                if(num_colum == (quint32)colum)
                {
                    break;
                }
            }
        }
        if(curBlock && curBlock->type > GROUP_BLOCK)
            index += ((EjGroupBlock*)curBlock)->m_counts;
    }
    return propBA;

}

int EjTableBlock::getColumPropIndex(int colum)
{
    int res = -1;
    if(!m_doc)
        return res;
    quint32 num_colum;
    IntVarLen t(num_colum);
	EjPropIntBlock *propColum = nullptr;
	EjPropBase *curBlock;
    int index;
    for(index = m_index + 1; index < startCell(); index++)
    {
		curBlock = dynamic_cast<EjPropBase*>(m_doc->lBlocks->at(index));
        if(curBlock && curBlock->type == PROP_INT && curBlock->num == TBL_PROP_COLUM)
        {
			propColum = dynamic_cast<EjPropIntBlock*>(curBlock);
            if(propColum && propColum->value == colum)
            {
                res = index;
                break;
            }
        }
        if(curBlock && curBlock->type > GROUP_BLOCK)
            index += ((EjGroupBlock*)curBlock)->m_counts;
    }
    return res;
}

EjPropBase *EjTableBlock::getPropFromMulty(int startIndex, int type, int num)
{
	EjPropBase *curProp;
	EjPropBase *res = nullptr;
    for(int index = startIndex; index < startCell(); index++)
    {
		curProp = dynamic_cast<EjPropBase*>(m_doc->lBlocks->at(index));
        if(curProp->type == PROP_INT && (curProp->num == TBL_PROP_COLUM || curProp->num == TBL_PROP_ROW))
            break;
        if(curProp && curProp->type == type && curProp->num == num)
        {
            res = curProp;
            break;
        }
        if(curProp && curProp->type > GROUP_BLOCK)
            index += ((EjGroupBlock*)curProp)->m_counts;
    }
    return res;
}

QString EjTableBlock::columName(int colum)
{
    QString res;
	EjPropByteArrayBlock *propBA = getNameColumProp(colum);
    quint32 num_colum;
    IntVarLen t(num_colum);
    if(propBA)
    {
        QDataStream ds(propBA->data);
        ds.setVersion(QDataStream::Qt_4_5);

        ds >> t;
        if(num_colum == (quint32)colum)
        {
            readSmallString(ds,res);
        }
    }
    return res;
}

void EjTableBlock::setColumName(QString name, int colum)
{
	EjPropByteArrayBlock *propBA = getNameColumProp(colum);

    if(!name.isEmpty())
    {
        if(!propBA) {
			propBA = new EjPropByteArrayBlock(TBL_COLUM_NAME);
            m_doc->lBlocks->insert(lastIndexProp(),propBA);
            deltaProps++;
            m_counts++;
        }
        propBA->data.clear();
        QDataStream ds(&propBA->data,QIODevice::WriteOnly);
        ds.setVersion(QDataStream::Qt_4_5);

        quint32 num_colum = (quint32)colum;
        IntVarLen t(num_colum);
        ds << t;
        writeSmallString(ds,name);
    }
    else if(propBA)
            remBlock(m_doc->lBlocks,propBA);

}

int EjTableBlock::lastIndexProp()
{
    int index = findPropIndex(m_doc->lBlocks,PROP_INT,TBL_PROP_COLUM, m_index + 1, startCell());
    if (index < 0)
    {
        index = findPropIndex(m_doc->lBlocks,PROP_INT,TBL_PROP_ROW, m_index + 1, startCell());
    }
    if(index < 0)
        index = startCell();
    return index;
}

void EjTableBlock::getAccessColum(EjPropAccessBlock *source, int colum)
{
    if(!m_doc)
        return;
	EjPropAccessBlock *propAccess = nullptr;
	EjPropIntBlock *propInt;
    int indexColum = m_index + 1;
    while(1)
    {
        indexColum = findPropIndex(m_doc->lBlocks,PROP_INT,TBL_PROP_COLUM, indexColum, m_index + deltaProps);
        if(indexColum < 0)
            break;
		propInt = dynamic_cast<EjPropIntBlock*>(m_doc->lBlocks->at(indexColum));
        if(propInt && propInt->value == colum)
            break;
    }
    if(indexColum < 0)
        return;
	propAccess = dynamic_cast<EjPropAccessBlock*>(getPropFromMulty(indexColum,PROP_ACCESS,TBL_COLUM_ACCESS));
    if(propAccess && propAccess->num == colum)
    {
        source->value = propAccess->value;
    }

}

void EjTableBlock::setAccessColum(EjPropAccessBlock *source, int colum)
{
	EjPropAccessBlock *propAccess = nullptr;
    int indexColum = findPropIndex(m_doc->lBlocks,PROP_INT,TBL_PROP_COLUM, m_index + 1, startCell());
    if(indexColum > -1)
    {
		propAccess = dynamic_cast<EjPropAccessBlock*>(getPropFromMulty(indexColum,PROP_ACCESS,TBL_COLUM_ACCESS));
    }
    if(!propAccess)
    {
        if(indexColum < 0)
        {
			EjPropIntBlock *propInt;
            int indexColum = m_index + 1;
            while(1)
            {
                indexColum = findPropIndex(m_doc->lBlocks,PROP_INT,TBL_PROP_COLUM, indexColum, startCell());
                if(indexColum < 0)
                {
                    indexColum = startCell();
                    break;
                }
				propInt = dynamic_cast<EjPropIntBlock*>(m_doc->lBlocks->at(indexColum));
                if(propInt && propInt->value < colum)
                    break;
            }
			propInt = new EjPropIntBlock(TBL_PROP_COLUM);
            propInt->value = colum;
            m_doc->lBlocks->insert(indexColum,propInt);
            deltaProps++;
            m_counts++;
        }
		propAccess = new EjPropAccessBlock(TBL_NAME);
        propAccess->value = source->value;
        m_doc->lBlocks->insert(indexColum,propAccess);
        deltaProps++;
        m_counts++;
    }
}


void EjTableBlock::setColumMaxWidth(quint16 width, int colum)
{
    if(!m_doc || colum < 0 || colum > lColums.count() - 1)
        return;
    quint32 num_colum;
    IntVarLen t(num_colum);
	EjPropPntBlock *propPnt = nullptr, *findProp = nullptr;
	EjPropBase *curBlock;
    int res = lColums.at(colum)->sizeProp.max;
    int index;
    for(index = m_index + 1; index < startCell(); index++)
    {
		curBlock = dynamic_cast<EjPropBase*>(m_doc->lBlocks->at(index));
        if(curBlock && curBlock->type == PROP_PNT && curBlock->num == TBL_COLUM_MAX_WIDTH)
        {
			propPnt = dynamic_cast<EjPropPntBlock*>(curBlock);
            if(propPnt && propPnt->y_value == colum)
            {
                findProp = propPnt;
                break;
            }
        }
        if(curBlock && curBlock->type > GROUP_BLOCK)
            index += (dynamic_cast<EjGroupBlock*>(curBlock))->m_counts;
    }
    if(width < 0xffff)
    {
        if(!findProp) {
			propPnt = new EjPropPntBlock(TBL_COLUM_MAX_WIDTH);
            m_doc->lBlocks->insert(lastIndexProp(),propPnt);
            deltaProps++;
            m_counts++;
        }
        propPnt->x_value = width;
        propPnt->y_value = colum;
    }
    else
    {
        if(findProp)
            remBlock(m_doc->lBlocks,findProp);
    }
    lColums.at(colum)->sizeProp.max = width;
}

void EjTableBlock::setColumMinWidth(quint16 width, int colum)
{
    if(!m_doc || colum < 0 || colum > lColums.count() - 1)
        return;
    quint32 num_colum;
    IntVarLen t(num_colum);
	EjPropPntBlock *propPnt = nullptr, *findProp = nullptr;
	EjPropBase *curBlock;
    int res = lColums.at(colum)->sizeProp.min;
    int index;
    for(index = m_index + 1; index < startCell(); index++)
    {
		curBlock = dynamic_cast<EjPropBase*>(m_doc->lBlocks->at(index));
        if(curBlock && curBlock->type == PROP_PNT && curBlock->num == TBL_COLUM_MIN_WIDTH)
        {
			propPnt = dynamic_cast<EjPropPntBlock*>(curBlock);
            if(propPnt && propPnt->y_value == colum)
            {
                findProp = propPnt;
                break;
            }
        }
        if(curBlock && curBlock->type > GROUP_BLOCK)
            index += (dynamic_cast<EjGroupBlock*>(curBlock))->m_counts;
    }
    if(width > 0)
    {
        if(!findProp) {
			propPnt = new EjPropPntBlock(TBL_COLUM_MIN_WIDTH);
            m_doc->lBlocks->insert(lastIndexProp(),propPnt);
            deltaProps++;
            m_counts++;
        }
        propPnt->x_value = width;
        propPnt->y_value = colum;
    }
    else
    {
        if(findProp)
            remBlock(m_doc->lBlocks,findProp);
    }
    lColums.at(colum)->sizeProp.min = width;

}

bool EjTableBlock::containsMerginCells(int &startSelect, int &endSelect)
{
    int startRow, startColum, endRow, endColum;
    selectParams(startRow,startColum,endRow,endColum,startSelect,endSelect);
    return containsMerginCells(startRow,startColum,endRow,endColum);
}

bool EjTableBlock::containsMerginCells(int startRow, int startColum, int endRow, int endColum)
{
    int index = cellIndex(startRow,startColum);
    int row = startRow;
    int colum = startColum;
    EjCellBlock *curCell;
    bool res = false;
    while(row <= endRow)
    {
        if(m_doc->lBlocks->at(index)->type == BASECELL && colum >= startColum && colum <= endColum)
        {
            curCell = dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(index));
            while(curCell->parent && curCell->parent->type == BASECELL)
            {
                curCell = dynamic_cast<EjCellBlock*>(curCell->parent);
            }
            if((curCell->mergeRows > 0 || curCell->mergeColums > 0) )
            {
                res = true;
                break;
            }

        }
        index = nextCell(index);
        colum++;
        if(colum > lColums.count() - 1)
        {
            row++;
            colum = 0;
        }

    }
    return res;

}

void EjTableBlock::selectParams(int &startRow, int &startColum, int &endRow, int &endColum, int &startSelect, int &endSelect)
{
    int row, colum;
    if(startSelect < 0 || endSelect < 0)
        return;
    cellParams(m_doc->lBlocks->at(startSelect),startRow,startColum,m_doc->lBlocks);
    isSelected(startSelect,startSelect,endSelect);
    row = endRow = startRow; colum = endColum = startColum;
    if(m_rowStartSelect > -1)
        startRow = m_rowStartSelect;
    if(m_rowEndSelect > -1)
        endRow = m_rowEndSelect;
    if(m_columStartSelect > -1)
        startColum = m_columStartSelect;
    if(m_columEndSelect > -1)
        endColum = m_columEndSelect;

}

EjCellStyle *EjTableBlock::fromStyles(EjCellStyle *newStyle)
{
    int max_count = -1;
    int count;
    EjCellStyle *curStyle = NULL;
    EjCellStyle *resStyle = NULL;

    foreach (EjBaseStyle *style, *m_doc->lStyles) {
        if(style->m_vid == CELL_STYLE)
        {
            count = 0;
            EjCellStyle *cellStyle = (EjCellStyle*)style;
            if(cellStyle->m_brushColor == newStyle->m_brushColor)
                count++;
            if(cellStyle->topBorder()->fullCompare(newStyle->topBorder()))
                count++;
            if(cellStyle->bottomBorder()->fullCompare(newStyle->bottomBorder()))
                count++;
            if(cellStyle->leftBorder()->fullCompare(newStyle->leftBorder()))
                count++;
            if(cellStyle->rightBorder()->fullCompare(newStyle->rightBorder()))
                count++;
            if(count > max_count)
            {
                curStyle = (EjCellStyle*)style;
                max_count = count;
            }
        }
    }
    resStyle = curStyle;
    if(max_count < 5)
    {
        resStyle = new EjCellStyle();        
        *resStyle = *newStyle;
        resStyle->num = m_doc->lStyles->count();
        resStyle->m_doc = m_doc;
        m_doc->appendStyle(resStyle);

        resStyle->setParentStyle(curStyle);
    }
    return resStyle;
}

void EjTableBlock::childCalc(EjBlock *child, EjCalcParams *calcParams)
{
	EjPropIntBlock *curInt;
    switch (child->type) {
    case PROP_INT:
		curInt = ((EjPropIntBlock*)child);
        if(curInt->num == TBL_KEY)
            key = curInt->value;
        if(curInt->num == TBL_COLUMS && curInt->value != lColums.count())
        {
            while(lColums.count() > curInt->value)
                delete lColums.takeLast();

            while (lColums.count() < curInt->value)
                lColums.append(new ColumProp());
        }
        if(curInt->num == TBL_ROWS && curInt->value != lColums.count())
        {
            while(lRows.count() > curInt->value)
                delete lRows.takeLast();

            while (lRows.count() < curInt->value)
                lRows.append(new EjSizeProp());
        }
        break;
    case PROP_PNT: {
		EjPropPntBlock *curPnt = dynamic_cast<EjPropPntBlock*>(child);
        if(curPnt && curPnt->y < lColums.count())
        {
            if(curPnt->num == TBL_COLUM_MAX_WIDTH)
                lColums.at(curPnt->y_value)->sizeProp.max = curPnt->x_value;
            if(curPnt->num == TBL_COLUM_MIN_WIDTH)
                lColums.at(curPnt->y_value)->sizeProp.min = curPnt->x_value;
        }

    }
        break;
    default:
        break;
    }
}


void EjTableBlock::cellParams(EjBlock *block, int &row, int &colum, QList<EjBlock *> *l_blocks)
{
    int table_index = -1;
	EjBlock *curBlock;
    if(l_blocks == NULL)
        l_blocks = m_doc->lBlocks;
    for(int i = this->startCell(); i <= (this->m_index + this->m_counts); i++ )
    {
        curBlock = l_blocks->at(i);
        if(curBlock->type == BASECELL)
            table_index++;
        if(curBlock == block)
        {
            if(this->nColums() > 0)
                row = (table_index) / this->nColums();
            colum = table_index - row * this->nColums();
            break;
        }
    }
}


EjCellBlock::EjCellBlock() : EjTextBlock()
{
    type = BASECELL; value = 0; parent = nullptr; vid = CELL_AUTO;
    cellStyle = nullptr;
    visible = true;
    mergeColums = 0;
    mergeRows = 0;
}

EjCellBlock::~EjCellBlock()
{
}

void EjCellBlock::setValue(double value)
{
	QList<EjBlock*> *lBlocks = ((EjTableBlock*)this->parent)->m_doc->lBlocks;
    int index = lBlocks->indexOf(this);
	EjPropTextBlock *curTxtProp = dynamic_cast<EjPropTextBlock*>(findProp(CELL_ADDITIONAL));

    bool bOk;

    this->value = value;
    QString text = getDText(this->value, ((EjTableBlock*)(this->parent))->accuracy);
    this->value = getDValue(text, &bOk);
    setText(text);
    this->text = getDText(this->value, 12);
}

void EjCellBlock::setFormula(QString formula)
{
    this->text = formula;
    this->vid = CELL_FORMULA;
}

QString EjCellBlock::formula()
{
    QString res;
    if(!text.isEmpty() && text.at(0) == QChar('='))
        res = text;
    return  res;
}

void EjCellBlock::setAdditional(QString additional)
{
	EjPropTextBlock *textProp = dynamic_cast<EjPropTextBlock*>(findProp(CELL_ADDITIONAL));
    if(!additional.isEmpty())
    {
        if(!textProp){
			textProp = new EjPropTextBlock(CELL_ADDITIONAL);
            addProp(textProp);
        }
        textProp->text = additional;
    }
    else if(textProp){
        removeProp(CELL_ADDITIONAL);
    }

}

QString EjCellBlock::additional()
{
    QString res;
	EjPropTextBlock *curTxtProp = dynamic_cast<EjPropTextBlock*>(findProp(CELL_ADDITIONAL));
    if(curTxtProp)
        res = curTxtProp->text;
    return res;

}

void EjCellBlock::setHiddenText(QString hiddenText)
{
	EjPropTextBlock *textProp = dynamic_cast<EjPropTextBlock*>(findProp(CELL_HIDDEN_TEXT));
    if(!hiddenText.isEmpty())
    {
        if(!textProp){
			textProp = new EjPropTextBlock(CELL_HIDDEN_TEXT);
            addProp(textProp);
        }
        textProp->text = hiddenText;
    }
    else if(textProp){
        removeProp(CELL_HIDDEN_TEXT);
    }

}

QString EjCellBlock::hiddentext()
{
    QString res;
	EjPropTextBlock *curTxtProp = dynamic_cast<EjPropTextBlock*>(findProp(CELL_HIDDEN_TEXT));
    if(curTxtProp)
        res = curTxtProp->text;
    return res;
}

void EjCellBlock::setTimeFormat(QString timeFormat)
{
    if(vid == CELL_DATETIME)
        text = timeFormat;
}

QString EjCellBlock::timeFormat()
{
    QString res;
    if(vid == CELL_DATETIME)
        res = text;
    if(res == "")
        res = "dd.MM.yyyy hh:mm";
    return res;
}

void EjCellBlock::setName(QString name)
{
	EjPropTextBlock *textProp = dynamic_cast<EjPropTextBlock*>(findProp(CELL_NAME));
    if(!name.isEmpty())
    {
        if(!textProp){
			textProp = new EjPropTextBlock(CELL_NAME);
            addProp(textProp);
        }
        textProp->text = name;
    }
    else if(textProp){
        removeProp(CELL_NAME);
    }
}

QString EjCellBlock::getName()
{
    QString res;
	EjPropTextBlock *textProp = dynamic_cast<EjPropTextBlock*>(findProp(CELL_NAME));
    if(textProp)
        res = textProp->text;
    return res;
}

void EjCellBlock::setVid(quint8 vidSource)
{
	EjPropInt8Block *intProp = dynamic_cast<EjPropInt8Block*>(findProp(CELL_VID));
    if(vidSource > CELL_AUTO)
    {
        if(!intProp){
			intProp = new EjPropInt8Block(CELL_VID);
            addProp(intProp);
        }
        intProp->value = vidSource;
    }
    else if(intProp){
        removeProp(CELL_VID);
    }
    this->vid = vidSource;
}

void EjCellBlock::setGUIDRefBook(qint64 guidRef)
{
	EjPropInt64Block *intProp = dynamic_cast<EjPropInt64Block*>(findProp(CELL_GUID_REF));
    if(!intProp){
		intProp = new EjPropInt64Block(CELL_GUID_REF);
        addProp(intProp);
    }
    intProp->value = guidRef;

}

qint64 EjCellBlock::getGUIDRefBook()
{
    qint64 res = -1;
	EjPropInt64Block *intProp = dynamic_cast<EjPropInt64Block*>(findProp(CELL_GUID_REF));
    if(intProp)
        res = intProp->value;
    return res;
}

void EjCellBlock::setIsMultiSelect(bool isMultiSelect)
{
	EjPropInt8Block *intProp = dynamic_cast<EjPropInt8Block*>(findProp(CELL_MULTI_SELECT));
    if(!intProp){
		intProp = new EjPropInt8Block(CELL_MULTI_SELECT);
        addProp(intProp);
    }
    intProp->value = isMultiSelect;
}

bool EjCellBlock::getIsMultiSelect()
{
    qint8 res = 0;
	EjPropInt8Block *intProp = dynamic_cast<EjPropInt8Block*>(findProp(CELL_MULTI_SELECT));
    if(intProp)
        res = intProp->value;
    return res;
}

void EjCellBlock::setGUIDRefData(QList<qint64> lGUIDData)
{
	EjPropInt64Block *intProp;
    removeProp(CELL_GUID_DATA,true);
    foreach(qint64 value,lGUIDData)
    {
		intProp = new EjPropInt64Block(CELL_GUID_DATA);
        intProp->value = value;
        addProp(intProp);
    }
}

QList<qint64> EjCellBlock::getGUIDRefData()
{
    QList<qint64> res;
	EjPropInt64Block *intProp;
	QList<EjBlock *> *lBlocks = ((EjTableBlock*)this->parent)->m_doc->lBlocks;
    int index = lBlocks->indexOf(this) + 1;
    if(index == 0)
        return res;
    while(lBlocks->at(index)->isProperty())
    {
		if(((EjPropBase*)lBlocks->at(index))->num == CELL_GUID_DATA)
        {
			intProp = dynamic_cast<EjPropInt64Block*>(lBlocks->at(index));
            if(intProp)
                res.append(intProp->value);
        }
        index++;
    }
    return res;
}

void EjCellBlock::getCurrentCellStyle(EjCellStyle *style)
{
    EjTableBlock *table = getTable();
    EjCellBlock *bottomCell;
    int row, colum;
    int index;
    *style = *cellStyle;
    table->cellParams(this,row,colum);
    if(row < table->lRows.count() - 1)
    {
        row++;
        index = table->cellIndex(row,colum);
        bottomCell = dynamic_cast<EjCellBlock*>(table->m_doc->lBlocks->at(index));
        if(bottomCell)
            *style->bottomBorder() = *bottomCell->cellStyle->topBorder();
    }
}

void EjCellBlock::setAccessBlock(EjPropAccessBlock *source)
{
	EjPropAccessBlock *accessProp = dynamic_cast<EjPropAccessBlock*>(findProp(CELL_ACCESS));
    if(source)
    {
        if(!accessProp){
			accessProp = new EjPropAccessBlock(CELL_NAME);
            addProp(accessProp);
        }
        accessProp->value = source->value;
    }
    else if(accessProp){
        removeProp(CELL_ACCESS);
    }
}

void EjCellBlock::getAccessBlock(EjPropAccessBlock *source)
{
    if(source)
    {
		EjPropAccessBlock *accessProp = dynamic_cast<EjPropAccessBlock*>(findProp(CELL_ACCESS));
        if(accessProp)
            source->value = accessProp->value;
    }
}

EjTableBlock *EjCellBlock::getTable()
{
    EjTableBlock *res = nullptr;
	EjBlock *curBlock = nullptr;
    curBlock = this->parent;
    while(curBlock && curBlock->type != EXT_TABLE)
        curBlock = curBlock->parent;
    if(curBlock && curBlock->type == EXT_TABLE)
        res = dynamic_cast<EjTableBlock*>(curBlock);
    return res;
}

void EjCellBlock::addProp(EjPropBase *prop)
{
	QList<EjBlock *> *lBlocks = ((EjTableBlock*)this->parent)->m_doc->lBlocks;
    int index = lBlocks->indexOf(this) + 1;
    if(index == 0)
        return;
    while(lBlocks->at(index)->isProperty())
        index++;
    lBlocks->insert(index,prop);
    ((EjTableBlock*)this->parent)->m_counts++;
}

EjPropBase *EjCellBlock::findProp(int num)
{
	QList<EjBlock *> *lBlocks = ((EjTableBlock*)this->parent)->m_doc->lBlocks;
	EjPropBase *res = nullptr;
    int index = lBlocks->indexOf(this) + 1;
    if(index == 0)
        return res;
    while(lBlocks->at(index)->isProperty())
    {
		EjPropBase *propBase = dynamic_cast<EjPropBase *>(lBlocks->at(index));
        if(propBase && propBase->num == num)
        {
			res = (EjPropBase*)lBlocks->at(index);
            break;
        }
        index++;
    }
    return res;
}

void EjCellBlock::removeProp(int num, bool isAll)
{
	QList<EjBlock *> *lBlocks = ((EjTableBlock*)this->parent)->m_doc->lBlocks;
	EjPropBase *prop = findProp(num);
    while(prop) {
        delete lBlocks->takeAt(lBlocks->indexOf(prop));
        ((EjTableBlock*)this->parent)->m_counts--;
        ((EjTableBlock*)this->parent)->deltaProps--;
        if(isAll)
            prop = findProp(num);
        else
            break;
    }
}

void EjCellBlock::clearAll(EjDocument *doc)
{
    clearData(doc,true);
}

void EjCellBlock::clearData(EjDocument *doc, bool isAll)
{
    if(!parent || parent->type != EXT_TABLE)
        return;
    EjTableBlock *table = (EjTableBlock*)parent;
    int i = doc->lBlocks->indexOf(this);
    if(i == -1)
        return;
    i++;
    int count_groups = 0;
	EjBlock *curBlock = doc->lBlocks->at(i);
    while(curBlock->type != BASECELL && curBlock->type != END_GROUP)
    {
        if(curBlock->type > GROUP_BLOCK)
        {
            if(!curBlock->isProperty() || isAll)
            {
                count_groups = 1;
                delete doc->lBlocks->takeAt(i);
                table->m_counts--;
                while(count_groups > 0 && i < doc->lBlocks->count())
                {
                    curBlock = doc->lBlocks->at(i);
                    if(curBlock->type == END_GROUP)
                        count_groups--;
                    else if(curBlock->type > GROUP_BLOCK)
                        count_groups++;
                    delete doc->lBlocks->takeAt(i);
                    table->m_counts--;
                }
            }
            else
                i += ((EjGroupBlock*)curBlock)->m_counts;
        }
        else if(!curBlock->isProperty() || isAll)
        {
            delete doc->lBlocks->takeAt(i);
            table->m_counts--;
        }
        else
            i++;
        curBlock = doc->lBlocks->at(i);
    }

}


bool EjCellBlock::isSelected(int &index, int &startSelect, int &endSelect)
{
    if(!this->parent)
        return true;
    EjTableBlock *curTable = (EjTableBlock*)(this->parent);
    return curTable->isSelected(index,startSelect,endSelect);
}

void EjCellBlock::setText(const QString &source, EjTextControl *control)
{
    QString txt = source;
    QString left;
    EjTableBlock *table = (EjTableBlock*) parent;
    int index = table->m_doc->lBlocks->indexOf(this);
	EjTextBlock *curTextBlock;
	EjSpaceBlock *curSpaceBlock;
	EjBlock *curBlock;
    EjDocument *doc = table->m_doc;
    int startIndex = index;
    QString str;
    bool bInsertText;



    index++;
    while(index < table->endBlock() && table->m_doc->lBlocks->at(index)->isProperty()) {
        index++;
    }
    int i = index;
    while(index < table->m_doc->lBlocks->count() && table->m_doc->lBlocks->at(index)->type != BASECELL && index < table->endBlock() )
    {
        if(!table->m_doc->lBlocks->at(index)->isProperty() && table->m_doc->lBlocks->at(index)->type != END_GROUP)
        {
            delete table->m_doc->lBlocks->takeAt(index);
            table->m_counts--;
        }
        else index++;
    }

    std::function<void ()> insertText = [&] () -> void
    {
       if(!str.isEmpty())
       {
			curTextBlock = new EjTextBlock(str);
            curTextBlock->parent = table;
            table->m_doc->lBlocks->insert(index,curTextBlock);
            table->m_counts++;
            index++;
            str.clear();
       }
    };

    index = i;
    bInsertText = false;
    for(i = 0; i < txt.count(); i++)
    {
        if(txt.at(i) == ' ')
        {
            insertText();

			curBlock = new EjSpaceBlock();
            curBlock->parent = table;
            table->m_doc->lBlocks->insert(index,curBlock);
            table->m_counts++;
            index++;
            bInsertText = true;
        }
        else if(txt.at(i) == '\n')
        {
            insertText();

			curBlock = new EjBlock(ENTER);
            curBlock->parent = table;
            table->m_doc->lBlocks->insert(index,curBlock);
            table->m_counts++;
            index++;
            bInsertText = true;
        }
        else
            str.append(txt.at(i));

    }

    insertText();
    if(control)
        control->activeIndex = index;
    for(int i = 0; i < doc->lTables->count(); i++)
    {
        table = doc->lTables->at(i);
		table->m_doc->lTables->at(i)->calcLenght(table->m_index, doc->lBlocks);
    }

    table->m_doc->cellDataChanged(this);
}

QString EjCellBlock::getText()
{
    QString res;
    EjTableBlock *table = getTable();
	EjBlock *curBlock;
    int index = table->m_doc->lBlocks->indexOf(this);
    if(index > -1)
    {
        index++;
        curBlock = table->m_doc->lBlocks->at(index);
        while(curBlock->type != BASECELL && curBlock->type != END_GROUP)
        {
            if(curBlock->type == SPACE)
            {
                res += " ";
            }
            if(curBlock->type == TEXT)
            {
				res += (dynamic_cast<EjTextBlock*>(curBlock))->text;
            }
            if(curBlock->type > GROUP_BLOCK) {
                EjGroupBlock *grBlock = (dynamic_cast<EjGroupBlock*>(curBlock));
                if(grBlock)
                {
                    if(grBlock->m_index < 0 || grBlock->m_counts == 0)
                    {
                        grBlock->calcLenght(index,table->m_doc->lBlocks);
                    }
                    index += grBlock->m_counts;
                }
                else
                    index++;
            }
            else {
                index++;
            }
            if(index < table->m_doc->lBlocks->count())
                curBlock = table->m_doc->lBlocks->at(index);
            else {
                break;
            }
        }
    }
    return res;
}

void EjCellBlock::setTextStyle(EjTextStyle *style, EjTextControl *control)
{
    EjTableBlock *table = (EjTableBlock*)parent;
    int index = control->doc->lBlocks->indexOf(this);
    while(control->doc->lBlocks->at(index)->type != BASECELL && control->doc->lBlocks->at(index)->type != END_GROUP )
    {
        if(control->doc->lBlocks->at(index)->type == NUM_STYLE && ((EjNumStyleBlock*)control->doc->lBlocks->at(index))->style->m_vid == TEXT_STYLE)
        {
            delete control->doc->lBlocks->takeAt(index);
            table->m_counts--;
        }
        else
            index++;
    }
    control->activeIndex = index;
    control->setTextStyle(style);
}

void EjCellBlock::merge(int rows, int colums)
{
	QList<EjBlock*> *lBlocks = ((EjTableBlock*)this->parent)->m_doc->lBlocks;

    if(rows <= 0 || colums <= 0)
    {

    }
    mergeRows = rows;
    mergeColums = colums;
	EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(CELL_MERGE_COLUMS);
    if(!propInt)
    {
		propInt = new EjPropIntBlock(CELL_MERGE_COLUMS);
        addProp(propInt);
    }
    propInt->value = colums;
	propInt = (EjPropIntBlock*)findProp(CELL_MERGE_ROWS);
    if(!propInt)
    {
		propInt = new EjPropIntBlock(CELL_MERGE_ROWS);
        addProp(propInt);
    }
    propInt->value = rows;
}

void EjCellBlock::unMerge()
{
    visible = true;
    if(mergeRows > 0 || mergeColums > 0)
    {
        EjTableBlock *table = dynamic_cast<EjTableBlock*>(parent);
		QList<EjBlock*> *lBlocks = table->m_doc->lBlocks;
        int index = lBlocks->indexOf(this);
        int colum;
        int row;
        table->cellParams(this,row,colum);
        int row_tmp = row;
        int colum_tmp = colum;
        while(row_tmp < row + mergeRows && index < lBlocks->count())
        {
            index = table->nextCell(index);
            if(index < 0 || lBlocks->at(index)->type == END_GROUP)
                break;
            colum_tmp++;
            if(colum_tmp > table->nColums() - 1)
            {
                row_tmp++;
                colum_tmp = 0;
                if(row_tmp >= row + mergeRows)
                    break;
            }
            if(colum_tmp >= colum && colum_tmp < colum + mergeColums)
            {
                dynamic_cast<EjCellBlock*>(lBlocks->at(index))->visible = true;
            }
        }
        mergeRows = mergeColums = 0;
        removeProp(CELL_MERGE_COLUMS);
        removeProp(CELL_MERGE_ROWS);

    }
    if(parent->type == BASECELL)
    {
        EjCellBlock *curCell = dynamic_cast<EjCellBlock*>(parent);
        if(curCell)
            curCell->unMerge();
    }
}
