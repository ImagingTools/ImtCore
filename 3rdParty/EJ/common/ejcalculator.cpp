#include "ejcalculator.h"
#include "ejtableblocks.h"
#include "labelblock.h"
#include <QDebug>
#include <QRegularExpression>

void CalculatorHelper::split(QString &txt, QStringList &lStr)
{
    EjCalculator::split(txt, lStr);
}

void CalculatorHelper::getLinkParams(QString txt, LinkParams &params)
{
    EjCalculator::getLinkParams(txt, params);
}

EjCalculator::EjCalculator(EjDocument *document)
{
    m_doc = document;
}

int EjCalculator::calcTables(bool isReplace)
{
//    return -1;
    EjTableBlock *curTable;
    EjCellBlock *curCell;
	EjBlock *curBlock;
    QString str, strAdd;
    bool bOk;
    double resCalc;
    int limitTxt = 500;
    if(isReplace)
        limitTxt = 1000;

   for(int i = 0; i < m_doc->lTables->count(); i++)
   {
       curTable = m_doc->lTables->at(i);
       if(!curTable->m_doc)
           curTable->m_doc = m_doc;
       curTable->calcLenght(curTable->m_index,m_doc->lBlocks);
       for(int j = curTable->startCell(); j < curTable->endBlock(); j++)
       {
           if(m_doc->lBlocks->at(j)->type == BASECELL)
           {
               curCell = dynamic_cast<EjCellBlock*>(m_doc->lBlocks->at(j));
               str.clear();
               strAdd.clear();
               while(j < m_doc->lBlocks->count() - 1 && m_doc->lBlocks->at(j+1)->type != BASECELL && m_doc->lBlocks->at(j+1)->type != END_GROUP)
               {
                   j++;
                   if(curCell && (curCell->vid == EjCellBlock::CELL_AUTO
                                  || curCell->vid == EjCellBlock::CELL_FORMULA
                                  || curCell->vid == EjCellBlock::CELL_NUMBER))
                   {
                       curBlock = m_doc->lBlocks->at(j);
                       if(curBlock->type == TEXT && str.count() < limitTxt)
						   str += ((EjTextBlock*)m_doc->lBlocks->at(j))->text;
                       if(curBlock->type == PROP_TXT)
                       {
						   EjPropTextBlock *curTxtProp = dynamic_cast<EjPropTextBlock*>(curBlock);
                           if(curTxtProp && curTxtProp->num == EjCellBlock::CELL_ADDITIONAL)
                           {
                              strAdd = curTxtProp->text;
                           }
                       }
                   }
               }
               if(curCell->vid == EjCellBlock::CELL_AUTO
                       || curCell->vid == EjCellBlock::CELL_NUMBER)
               {
                   if(!curCell->text.isEmpty())
                       str = curCell->text;
                   if(str.count() > 0 && str.at(0).toLatin1() != '=') {
                       curCell->value = getDValue(str, &bOk);
                       if(bOk)
                       {
                       }
                   }
               }


               if(curCell->vid == EjCellBlock::CELL_AUTO
                       || curCell->vid == EjCellBlock::CELL_FORMULA)
               {
                   if(!curCell->text.isEmpty() && curCell->text.at(0).toLatin1() == '=')
                   {
                       if(curCell->text.count() > 6 && !is_contains_operator(curCell->text)/*curCell->text.count() > 4 && !is_contains_operator(curCell->text)*/)
                       {
                           str = getStringValue(curCell->text.right(curCell->text.count() - 1),curTable);
                           curCell->value = getDValue(str,&bOk);
                           curCell->setText(str);
                       }
                       else
                       {
                           resCalc = calcFormula(curCell->text,curTable, &bOk);
                           if(bOk) {
                               curCell->value = resCalc;
                               curCell->setText(getDText(curCell->value, curTable->accuracy));
                           }
                       }
                   }
               }
           }
       }
   }
   updateFormulas(false);
   return 0;
}

void EjCalculator::updateFormulas(bool isReplace)
{
    QStringList list; // = cell->formula.split(reg);
    bool bOk;
    foreach (LabelBlock *curLabel, *m_doc->lLabels) {
        list.clear();
        split(curLabel->additionalProp,list);
        if(list.count() == 3 && list.at(1).toLatin1() == "=")
        {
            curLabel->vid = LINK_LABEL;
            if(isReplace)
                setLinkData(list.at(2), curLabel);
            else  {
                QString str = curLabel->getTextData(m_doc->lBlocks);
                curLabel->value = getDValue(str,&bOk);
                curLabel->valueTxt = getDText(curLabel->value,curLabel->accuracy);
             }
        }
        else if(list.count() > 3 && list.at(1) == "=") {
            curLabel->vid = LINK_FORMULA;
            curLabel->value = calcFormula(curLabel->additionalProp,curLabel, &bOk);
            curLabel->valueTxt = getDText(curLabel->value,curLabel->accuracy);
        }
        else  {
            QString str = curLabel->getTextData(m_doc->lBlocks);
            curLabel->value = getDValue(str,&bOk);
            curLabel->valueTxt = getDText(curLabel->value,curLabel->accuracy);
         }
    }

}

void EjCalculator::setLinkData(QString str_link, EjBlock *source)
{
    LinkParams params;
    EjLinkProp *curLink;
	EjBlock *curBlock1, *curBlock2;
    EjAttrProp *attrProp = m_doc->m_attrProp;
    QString str;
    int i,j;

    bool bOk;
    int count_groups;

    if(!attrProp)
        return;

    std::function<void ()> insertData = [&] () -> void
    {
        curBlock2 = curLink->m_extDoc->lBlocks->at(j);
        str.clear();
        while(curBlock2->type != BASECELL && curBlock2->type != END_GROUP)
        {
            if(curBlock2->type > GROUP_BLOCK)
            {
                count_groups = 1;
                while(count_groups > 0 && j < curLink->m_extDoc->lBlocks->count())
                {
                    curBlock1 = curBlock2->makeCopy();
                    m_doc->lBlocks->insert(i,curBlock1);
                    i++;
                    j++;
                    if(m_doc == curLink->m_extDoc && i < j)
                        j++;
                    curBlock2 = curLink->m_extDoc->lBlocks->at(j);
                    if(curBlock2->type == END_GROUP)
                        count_groups--;
                    else if(curBlock2->type > GROUP_BLOCK)
                        count_groups++;
                }
            }
            else if(!curBlock2->isProperty())
            {
                curBlock1 = curBlock2->makeCopy();
                m_doc->lBlocks->insert(i,curBlock1);
                if(curBlock1->type == TEXT && str.count() < 20)
                {
					str += ((EjTextBlock*)curBlock1)->text;
                }
                i++;
                if(m_doc == curLink->m_extDoc && i < j)
                    j++;
            }
            j++;
            curBlock2 = curLink->m_extDoc->lBlocks->at(j);
        }
    };

    getLinkParams(str_link, params);
    if(params.numLink >= 0 && params.numLink - 1 < attrProp->m_lLinks.count())
    {
        if(params.type == LINK_LABEL )
        {
            curLink = attrProp->m_lLinks.at(params.numLink - 1);
            if(params.numType > 0 && params.numType - 1 < curLink->m_extDoc->lLabels->count())
            {
                LabelBlock *labelBlock = curLink->m_extDoc->lLabels->at(params.numType - 1);
                if(source->type == EXT_LABEL) {
                    ((LabelBlock*)source)->clear(m_doc->lBlocks);
                    LabelBlock *labelBlock = (LabelBlock*)source;
                    labelBlock->clear(m_doc->lBlocks);
                    i = labelBlock->m_index + labelBlock->m_counts;
                }
                else {
                    EjCellBlock *cellBlock = (EjCellBlock*)source;
                    cellBlock->clearAll(m_doc);
                    i = m_doc->lBlocks->indexOf(source);
                    while(m_doc->lBlocks->at(i)->type != BASECELL || m_doc->lBlocks->at(i)->type != END_GROUP)
                        i++;
                }
                j = curLink->m_extDoc->lBlocks->indexOf(labelBlock) + 1;

                insertData();

                if(source->type == EXT_LABEL) {
                    ((LabelBlock*)source)->value = getDValue(str,&bOk);
                }
                else {
                    ((EjCellBlock*)source)->value = getDValue(str,&bOk);
                }
            }
        }
        if(params.type == LINK_TABLE)
        {
            curLink = attrProp->m_lLinks.at(params.numLink - 1);
            if(params.numType > 0 && params.numType - 1 < curLink->m_extDoc->lTables->count())
            {
                int numColum = params.value.toLatin1().at(0) - 'A';
                EjTableBlock *curTable = curLink->m_extDoc->lTables->at(params.numType - 1);
                int numRow = params.value.right(params.value.count()-1).toInt();
                numRow--;
                if(source->type == EXT_LABEL) {
                    ((LabelBlock*)source)->clear(m_doc->lBlocks);
                    LabelBlock *labelBlock = (LabelBlock*)source;
                    labelBlock->clear(m_doc->lBlocks);
                    i = labelBlock->m_index + labelBlock->m_counts;
                }
                else {
                    EjCellBlock *cellBlock = (EjCellBlock*)source;
                    cellBlock->clearAll(m_doc);
                    i = m_doc->lBlocks->indexOf(source);
                    while(m_doc->lBlocks->at(i)->type != BASECELL || m_doc->lBlocks->at(i)->type != END_GROUP)
                        i++;
                }
				j = EjTextControl::tableCellIndex(curTable,numRow,numColum,curLink->m_extDoc->lBlocks);
                {
                    if(j > -1 && j < curLink->m_extDoc->lBlocks->count() - 1)
                    {
                        j++;
                        curBlock2 = curLink->m_extDoc->lBlocks->at(j);
                        str.clear();
                        insertData();
                        if(source->type == EXT_LABEL) {
                            ((LabelBlock*)source)->value = getDValue(str,&bOk);
                        }
                        else {
                            ((EjCellBlock*)source)->value = getDValue(str,&bOk);
                        }
                    }
                }
            }
        }
    }

}

int EjCalculator::calcCell(EjCellBlock *cell, EjBlock *table)
{
    int accuracy = 3;
    if(cell->parent)
        accuracy = ((EjTableBlock*)(cell->parent))->accuracy;
    QString formula = cell->formula();
    if(cell->type != EjCellBlock::CELL_FORMULA || formula.isEmpty())
    {
        return -1;
    }

    QString reg_txt = "!(2)!";
    QStringList list; // = cell->formula.split(reg);
  QString txt, txt_from, txt_to;
  CalcItem clItem;
  QList<CalcItem>lItems;
  QList<CalcItem>lItems_out;
  bool bOk;
  int n;
  bool bFindFunc;
  split(formula,list);
  for(int i = 0; i < list.count(); i++)
  {
      txt = list.at(i);
      if(txt == "=" || txt == "" || txt == " ")
          continue;
      if(txt.size() == 1 && op_preced(txt[0].toLatin1()) > 0)
      {
          clItem.type = CalcItem::OPERATOR;
          clItem.text = txt;
          clItem.value = 2;
          if(txt == "%")
              clItem.value = 1;
          lItems.append(clItem);
      }
      else {
          bFindFunc = false;
          if(txt == "SUMM" || txt == "SIN" || txt == "COS" || txt == "MIN" || txt == "MAX")
          {
              clItem.type = CalcItem::FUNC;
              clItem.text = txt;
              clItem.value = 1;
              lItems.append(clItem);
          }
          else if(txt == "(" || txt == ")" || txt == ";")
          {
              clItem.type = CalcItem::NONE;
              clItem.text = txt;
              clItem.value = 0;
              lItems.append(clItem);
          }
          else if(txt == ":")
          {
              if(i > 0 && i < list.count() - 1)
              {
                  txt_from = list.at(i-1);
                  txt_to = list.at(i+1);
                  if(getValues(txt_from,txt_to,lItems,table))
                      i++;
                  else return -1;
              }
          }
          else if(txt != "" && txt != " " && txt != ".")
          {
              clItem.type = CalcItem::NUMERIC;
              clItem.text.clear();

              if(is_value(txt.toLatin1().at(0)) || (txt.count() > 1 && is_value(txt.toLatin1().at(1))) || (txt.count() > 2 && is_value(txt.toLatin1().at(2))) )
              {
                  clItem.value = getValue(txt,table);
              }
              else
              {
                  clItem.value = getDValue(txt, &bOk);   //txt.toDouble();
              }
              lItems.append(clItem);
          }
      }
  }
  shunting_yard(lItems,lItems_out);
  cell->value = execution_order(lItems_out);
  if(cell->parent)
      accuracy = ((EjTableBlock*)(cell->parent))->accuracy;
  cell->setText(getDText(cell->value,accuracy));
}

double EjCalculator::calcFormula(QString formula, EjBlock *source, bool *bOk)
{
    if(formula.isEmpty())
    {
        *bOk = false;
        return 0;
    }

    QStringList list;
    QString txt, txt_from, txt_to;
    CalcItem clItem;
    QList<CalcItem>lItems;
    QList<CalcItem>lItems_out;
    double res;
	bool bFindFunc;
    EjTableBlock* curTable;
    split(formula,list);
    if(list.count() < 3 || list.at(1) != "=")
    {
        *bOk = false;
        return 0;
    }
    for(int i = 0; i < list.count(); i++)
    {
        txt = list.at(i);
        if(txt == "=" || txt == "" || txt == " ")
            continue;
        if (txt[0] == 't')
        {
            if (txt.count('t') != 1)
            {
                return 0;
            }
            QString num = txt.right(txt.size() - 1);
            curTable = m_doc->lTables->at(num.toInt() - 1);
            continue;
        }
        if(txt.size() == 1 && op_preced(txt[0].toLatin1()) > 0)
        {
            clItem.type = CalcItem::OPERATOR;
            clItem.text = txt;
            clItem.value = 2;
            if(txt == "%")
                clItem.value = 1;
            lItems.append(clItem);
        }
        else {
            bFindFunc = false;
            if(txt == "SUMM" || txt == "SIN" || txt == "COS" || txt == "MIN" || txt == "MAX")
            {
                clItem.type = CalcItem::FUNC;
                clItem.text = txt;
                clItem.value = 1;
                lItems.append(clItem);
            }
            else if(txt == "(" || txt == ")" || txt == ";")
            {
                clItem.type = CalcItem::NONE;
                clItem.text = txt;
                clItem.value = 0;
                lItems.append(clItem);
            }
            else if(txt == ":")
            {
                if(i > 0 && i < list.count() - 1)
                {
                    txt_from = list.at(i-1);
                    txt_to = list.at(i+1);
                    if(getValues(txt_from,txt_to,lItems,source))
                        i++;
                    else {
                        *bOk = false;
                        return -1;
                    }
                }
            }
            else if(txt != "" && txt != " " && txt != ".")
            {
                clItem.type = CalcItem::NUMERIC;
                clItem.text.clear();
                clItem.value = getValue(txt, curTable/*source*/);
                lItems.append(clItem);
            }

        }
    }
    shunting_yard(lItems,lItems_out);
    res = execution_order(lItems_out);
    *bOk = true;
    return res;
}

// http://alfalavista.ru/index.php/2013-06-18-22-25-47/191-2014-02-08-17-15-38
// Операторы
// Приоритет Оператор Ассоциативность
// 4 ! правая
// 3 * / % левая
// 2 + - левая
// 1 = левая
int EjCalculator::op_preced(const char c)
{
    switch(c)
    {
        case '!':
        return 4;

        case '*':
        case '/':
        case '%':
        return 3;

        case '+':
        case '-':
        return 2;

        case '=':
        return 1;
    }
    return 0;
}

bool EjCalculator::is_split(const char c)
{
    switch(c)
    {
    case '*':
    case '/':
    case '%':
    case '+':
    case '-':
    case '=':
    case '(':
    case ')':
    case ':':
    case ';':
        return true;
    }
    return false;
}

bool EjCalculator::is_ext_link(QString &txt)
{
    bool res = false;
    if(txt.count() > 0 && txt.at(0) == 'r')
        res = true;
    return res;
}

bool EjCalculator::is_contains_operator(QString &txt)
{
    bool res = false;
    QString str;
    char chr;
    for(int i = 1; i < txt.count(); i++)
    {
        chr = txt.at(i).toLatin1();
        if(chr != '=')
            str.append(chr);
        if(is_operator(chr))
        {
            res = true;
            break;
        }
        if(str == "SUMM")
        {
            res = true;
            break;
        }
    }
    return res;
}

void EjCalculator::split(QString &txt, QStringList &lStr)
{
    if(txt == "")
        return;
    QString cur_txt;

    for(int i = 0; i < txt.count(); i++)
    {
        if (txt[i] == 't')
        {
            if(!cur_txt.isEmpty())
            {
                lStr << cur_txt;
                cur_txt.clear();
            }

            cur_txt += 't';
            int j = i + 1;
            while (j < txt.count() - 1 && is_num(txt[j]))
            {
                cur_txt += txt[j];
                j++;
            }
            i = j;
            lStr << cur_txt;
            cur_txt.clear();
        }

        if (is_split(txt[i].toLatin1()) )
        {
            if(!cur_txt.isEmpty())
            {
                lStr << cur_txt;
                cur_txt.clear();
            }
            lStr << cur_txt;
            cur_txt = txt[i];
            lStr << cur_txt;
            cur_txt.clear();
        } else if(txt[i] != ' ')
        {
            cur_txt += txt[i];
        }

    }

    if(!cur_txt.isEmpty())
        lStr << cur_txt;

}

bool EjCalculator::op_left_assoc(const char c)
{
    switch(c)
    {
        // лево-ассоциативные операторы
        case '*':
        case '/':
        case '%':
        case '+':
        case '-':
        case '=':
        return true;
        // право-ассоциативные операторы
        case '!':
        return false;
    }
    return false;
}

unsigned int EjCalculator::op_arg_count(const char c)
{
    switch(c)
    {
        case '*':
        case '/':
        case '%':
        case '+':
        case '-':
        case '=':
        return 2;
        case '!':
        return 1;

    default:
    return c - 'A';
    }
    return 0;
}


bool EjCalculator::shunting_yard(QList<CalcItem> &input, QList<CalcItem> &output)
{
    int pos = 0;
    int startPos;
    char c,sc;
    CalcItem curItem, curItem2;
    QList<CalcItem> stack;
    QList<int> stack_args;
    for(pos = 0; pos < input.count(); pos++)
    {
        curItem = input.at(pos);
            // Если токен является числом (идентификатором), то добавить его в очередь вывода.
            if(curItem.type == CalcItem::NUMERIC)
            {
                output.append(curItem);
                if(!stack_args.isEmpty())
                    stack_args.last()++;
            }
            // Если токен - функция, то положить его в стек.
            else if(curItem.type == CalcItem::FUNC)
            {
                if(!stack_args.isEmpty())
                    stack_args.last()++;
                stack.append(curItem);
                stack_args.append(0);
            }
            //Если токен - разделитель аргументов функции (запятая):
            else if(curItem.type == CalcItem::NONE && curItem.text == ";")
            {
                bool pe = false;
                while(stack.count() > 0)
                {
                    curItem2 = stack.last();
                    if(curItem2.type == CalcItem::NONE && curItem2.text == "(")
                    {
                        pe = true;
                        break;
                    }
                    else
                    {
                        // Пока на вершине не левая круглая скобка,
                        // перекладывать операторы из стека в очередь вывода.
//                        *outpos = sc; ++outpos;
//                        sl--;
                        output.append(stack.takeLast());
                    }
                }
                // Если не была достигнута левая круглая скобка, либо разделитель не в том месте
                // либо была пропущена скобка
                if(!pe)
                {
                    qDebug() << "Error: separator or parentheses mismatched\n";
                    return false;
                }
            }
            // Если токен оператор op1, то:
            else if(curItem.type == CalcItem::OPERATOR)
            {
                while(stack.count() > 0)
                {
                    curItem2 = stack.last();
                    sc = curItem2.text[0].toLatin1();
                    c = curItem.text[0].toLatin1();
                    // Пока на вершине стека присутствует токен оператор op2,
                    // а также оператор op1 лево-ассоциативный и его приоритет меньше или такой же чем у оператора op2,
                    // или оператор op1 право-ассоциативный и его приоритет меньше чем у оператора op2
                    if(curItem2.type == CalcItem::OPERATOR  &&
                        ((op_left_assoc(c) && (op_preced(c) <= op_preced(sc))) ||
                           (!op_left_assoc(c) && (op_preced(c) < op_preced(sc)))))
                    {
                        // Переложить оператор op2 из стека в очередь вывода.
                         output.append(stack.takeLast());
                    }
                    else
                    {
                        break;
                    }
                }
                // положить в стек оператор op1
                stack.append(curItem);
                if(!stack_args.isEmpty())
                    stack_args.last() -= (curItem.value - 1);
            }
            // Если токен - левая круглая скобка, то положить его в стек.
            else if(curItem.type == CalcItem::NONE && curItem.text == "(")
            {
                stack.append(curItem);
            }
            // Если токен - правая круглая скобка:
            else if(curItem.type == CalcItem::NONE && curItem.text == ")")
            {
                bool pe = false;
                // До появления на вершине стека токена "левая круглая скобка"
                // перекладывать операторы из стека в очередь вывода.
                while(stack.count() > 0)
                {
                    curItem2 = stack.last();
                    if(curItem2.text == "(")
                    {
                        pe = true;
                        break;
                    }
                    else
                    {
                        output.append(stack.takeLast());
                    }
                }
                // Если стек кончится до нахождения токена левая круглая скобка, то была пропущена скобка.
                if(!pe)
                {
                    qDebug() << ("Error: parentheses mismatched\n");
                    return false;
                }
                // выкидываем токен "левая круглая скобка" из стека (не добавляем в очередь вывода).
//                sl--;
                stack.takeLast();
                // Если на вершине стека токен - функция, положить его в стек.
//                if(sl > 0)
                if(stack.count() > 0)
                {
//                    sc = stack[sl - 1];
                    curItem2 = stack.last();
//                    if(is_function(sc))
                    if(curItem2.type == CalcItem::FUNC)
                    {
                        output.append(stack.takeLast());
//                        output.last().value = output.count() - stack_args.takeLast() - 1;
                      output.last().value = stack_args.takeLast();
                    }
                }
            }
            else
            {
                qDebug() << ("Unknown token %c\n", c);
                return false; // Unknown token
            }
//        ++strpos;
    }
    // Когда не осталось токенов на входе:
    // Если в стеке остались токены:
//    while(sl > 0)
    while(stack.count() > 0)
    {
//        sc = stack[sl - 1];
        curItem2 = stack.last();
//        if(sc == '(' || sc == ')')
        if(curItem2.text == "(" || curItem2.text == ")")
        {
            qDebug() << ("Error: parentheses mismatched\n");
            return false;
        }
        output.append(stack.takeLast());

    }

//    *outpos = 0; // Добавляем завершающий ноль к строке
    return true;
}

double EjCalculator::execution_order(QList<CalcItem> &input)
{
    double res = 0, val1,val2;
    CalcItem curItem, curItem2;
    QList<CalcItem> stack;
//    // Пока на входе остались токены
    while(input.count() > 0)
    {
//        // Прочитать следующий токен
//        c = *strpos;
        curItem = input.takeFirst();
//        // Если токен - значение или идентификатор
//        if(is_ident(c))
        if(curItem.type == CalcItem::NUMERIC)
        {
//        // Поместить его в стек
            stack.append(curItem);
//            stack[sl] = c;
//            ++sl;
        }
//        // В противном случае, токен - оператор (здесь под оператором понимается как оператор, так и название функции)
//        else if(is_operator(c) || is_function(c))
        else if(curItem.type == CalcItem::FUNC || curItem.type == CalcItem::OPERATOR)
        {
//            sprintf(res, "_%02d", rn);
//            printf("%s = ", res);
//            ++rn;
//            // Априори известно, что оператор принимает n аргументов
//            unsigned int nargs = op_arg_count(c);
//            curItem2 = input.takeFirst();
            unsigned int nargs = curItem.value;
//                        unsigned int Tnargs = nargs;
//            // Если в стеке значений меньше, чем n
//            if(sl < nargs)
            if(stack.count() < nargs)
//            {
                // (ошибка) Недостаточное количество аргументов в выражении.
                return false;
//            }
//            // В противном случае, взять последние n аргументов из стека
//            // Вычислить оператор, взяв эти значения в качестве аргументов
//            if(is_function(c))
            if(curItem.type == CalcItem::FUNC)
            {
//                printf("%c(", c);
                res = 0;
                while(nargs > 0)
                {
//                    sc = stack[sl - nargs];
                    curItem2 = stack.takeAt(stack.count() - nargs);
                    if(curItem.text == "SUMM")
                        res += curItem2.value;
                    --nargs;
                }
//                                sl -= Tnargs;
            }
            else
            {
                if(nargs == 1)
                {
                    curItem2 = stack.takeLast();
                    val1 = curItem2.value;
                    if(curItem.text == "%")
                        res = val1*0.01;
                }
                else
                {
                    curItem2 = stack.takeLast();
                    val2 = curItem2.value;
                    curItem2 = stack.takeLast();
                    val1 = curItem2.value;
                    if(curItem.text == "+")
                        res = val1 + val2;
                    else if(curItem.text == "-")
                        res = val1 - val2;
                    else if(curItem.text == "*")
                        res = val1 * val2;
                    else if(curItem.text == "/")
                        res = val1 / val2;
                }
            }
//            // Если получены результирующие значения, поместить таковые в стек.
            curItem2.type = CalcItem::NUMERIC;
            curItem2.value = res;
            stack.append((curItem2));
        }
    }
//    // Если в стеке осталось лишь одно значение,
//    // оно будет являться результатом вычислений.
    if(stack.count() > 0)
    {
        curItem = stack.takeFirst();
        res = curItem.value;
    }
//    // Если в стеке большее количество значений,
//    // (ошибка) Пользователь ввёл слишком много значений.
    return res;
}

double EjCalculator::getValue(QString txt, EjBlock *source)
{
//    char *chr;
    int numRow;
    int numColum;
	int numTable;
	int start;
    EjCellBlock *cellBlock;
    double res = 0;
    bool isValue = false;
    EjTableBlock *curTable = 0;

    LinkParams params;
    EjAttrProp *attrProp = m_doc->attributes();
    EjLinkProp *curLink = NULL;
	QList<EjBlock*> *lBlocks = m_doc->lBlocks;
	EjBlock *curBlock;
    bool bOk;
    if(!attrProp)
        return 0;

    if(source->type == EXT_TABLE)
    {
        curTable = (EjTableBlock*) source;
    }
    getLinkParams(txt, params);
    if(params.numLink >= 0 && params.numLink - 1 < attrProp->m_lLinks.count())
    {

        if(params.type == LINK_TABLE || params.type == LINK_CLEAN)
        {
            if(params.type == LINK_TABLE)
            {
                curLink = attrProp->m_lLinks.at(params.numLink - 1);
                if(params.numType > 0 && !curLink->m_extDoc)
                    return 0;
                if(params.numType > 0 && params.numType - 1 < curLink->m_extDoc->lTables->count())
                {
                    curTable = curLink->m_extDoc->lTables->at(params.numType - 1);
                    lBlocks = curLink->m_extDoc->lBlocks;
                }
            }

            if(is_value(params.value.toLatin1().at(0)))
            {
                numRow = params.value.right(params.value.count()-1).toInt();
                numRow--;
                numColum = params.value.toLatin1().at(0) - 'A';
                isValue = true;
            }


            if(isValue && curTable)
            {
                int i = curTable->startCell();
                while(i < curTable->endBlock() && lBlocks->at(i)->type != BASECELL)
                    i++;
                i--;
                for(int row = 0; row < curTable->nRows(); row++)
                {
                    for(int colum = 0; colum < curTable->nColums(); colum++)
                    {
                        i++;
                        if(i < curTable->endBlock() && lBlocks->at(i)->type == BASECELL && colum == numColum && row == numRow)
                        {
                            cellBlock = (EjCellBlock*)lBlocks->at(i);
                            res = cellBlock->value;
                            break;
                        }
                        while(i < curTable->endBlock() && lBlocks->at(i+1)->type != BASECELL)
                            i++;
                    }
                }
            }
            else {
                res = getDValue(params.value,&bOk);
            }
        }
        if(params.type == LINK_LABEL )
        {
            curLink = attrProp->m_lLinks.at(params.numLink - 1);
            if(params.numType > 0 && params.numType - 1 < curLink->m_extDoc->lLabels->count())
            {
                curBlock = curLink->m_extDoc->lLabels->at(params.numType - 1);
                int j = curLink->m_extDoc->lBlocks->indexOf(curBlock);
                QString str;

                while(curBlock->type != BASECELL && curBlock->type != END_GROUP)
                {
                    j++;
                    curBlock = curLink->m_extDoc->lBlocks->at(j);
                    if(curBlock->type == TEXT && str.count() < 20)
                    {
						str += ((EjTextBlock*)curBlock)->text;
                    }
                }

                res = getDValue(str,&bOk);
            }
        }
    }

    return res;
}

QString EjCalculator::getStringValue(QString txt, EjBlock *source)
{
    int numRow = 0;
    int numColum = 0;
    EjCellBlock *cellBlock;
    double res = 0;
    bool isValue = false;
    EjTableBlock *curTable = nullptr;
	EjTextBlock *curTextBlock;

    LinkParams params;
    EjAttrProp *attrProp = m_doc->m_attrProp;
    EjLinkProp *curLink = NULL;
	QList<EjBlock*> *lBlocks = m_doc->lBlocks;
	EjBlock *curBlock;
    bool bOk;
    QString str;
    if(!attrProp)
        return QString("0");

    if(source->type == EXT_TABLE)
    {
        curTable = (EjTableBlock*) source;
    }
    getLinkParams(txt, params);
    if(params.numLink > 0 && params.numLink - 1 < attrProp->m_lLinks.count())
    {

        if(params.type == LINK_TABLE || params.type == LINK_CLEAN)
        {
            if(params.type == LINK_TABLE)
            {
                curLink = attrProp->m_lLinks.at(params.numLink - 1);
                if(!curLink->m_extDoc)
                    return QString("0");
                if(params.numType > 0 && params.numType - 1 < curLink->m_extDoc->lTables->count())
                {
                    curTable = curLink->m_extDoc->lTables->at(params.numType - 1);
                    lBlocks = curLink->m_extDoc->lBlocks;
                }
            }

            if(is_value(params.value.toLatin1().at(0)))
            {
                numRow = params.value.right(params.value.count()-1).toInt();
                numRow--;
                numColum = params.value.toLatin1().at(0) - 'A';
                isValue = true;
            }

            if(isValue && curTable)
            {
                int i = curTable->cellIndex(numRow,numColum);
                if(i > -1)
                {
                    cellBlock = (EjCellBlock*)lBlocks->at(i);
                    res = cellBlock->value;
                    while(i < curTable->endBlock() && lBlocks->at(i+1)->type != BASECELL) {
                        i++;
                        curBlock = lBlocks->at(i);
                        if(curBlock->type == TEXT)
                        {
							curTextBlock = dynamic_cast<EjTextBlock*>(curBlock);
                            str += curTextBlock->text;
                        }
                        else if(curBlock->type == SPACE)
                        {
                            str += " ";
                        }
                    }
                }

            }
            else {
                res = getDValue(params.value,&bOk);
            }
        }
        if(params.type == LINK_LABEL )
        {
            curLink = attrProp->m_lLinks.at(params.numLink - 1);
            if(!curLink->m_extDoc)
                return QString("0");
            if(params.numType > 0 && params.numType - 1 < curLink->m_extDoc->lLabels->count())
            {
                curBlock = curLink->m_extDoc->lLabels->at(params.numType - 1);
                int j = curLink->m_extDoc->lBlocks->indexOf(curBlock);

                while(curBlock->type != BASECELL && curBlock->type != END_GROUP)
                {
                    j++;
                    curBlock = curLink->m_extDoc->lBlocks->at(j);
                    if(curBlock->type == TEXT && str.count() < 500)
                    {
						str += ((EjTextBlock*)curBlock)->text;
                    }
                }

                res = getDValue(str,&bOk);
            }
        }
    }

    if(str.isEmpty())
        return "0";
    return str;

}

bool EjCalculator::getValues(QString txt_from, QString txt_to, QList<CalcItem> &output, EjBlock *source)
{
    EjTableBlock *curTable = NULL;
    EjTableBlock *curTable_to = NULL;
    EjCellBlock *cellBlock;
    CalcItem calcItem;
    LinkParams params, params_to;
    EjAttrProp *attrProp = m_doc->m_attrProp;
    EjLinkProp *curLink;
	QList<EjBlock*> *lBlocks = m_doc->lBlocks;
    int numRow, numColum, numRow_to, numColum_to;
    bool isValue = false;
    int index;
    int numTable;
    int start;
    int row, colum;
    if(!attrProp)
        return false;

    if(source->type == EXT_TABLE)
    {
        curTable = curTable_to = (EjTableBlock*) source;
    }
    getLinkParams(txt_from, params);
    if(params.type == LINK_TABLE)
    {
        curLink = attrProp->m_lLinks.at(params.numLink - 1);
        if(params.numType > 0 && params.numType - 1 < curLink->m_extDoc->lTables->count())
        {
            curTable = curLink->m_extDoc->lTables->at(params.numType - 1);
            lBlocks = curLink->m_extDoc->lBlocks;
        }
    }
    getLinkParams(txt_to, params_to);
    if(params.type == LINK_TABLE)
    {
        curLink = attrProp->m_lLinks.at(params_to.numLink - 1);
        if(params_to.numType > 0 && params_to.numType - 1 < curLink->m_extDoc->lTables->count())
        {
            curTable_to = curLink->m_extDoc->lTables->at(params_to.numType - 1);
        }
    }
    else if(params.type == LINK_CLEAN)
        curTable_to = curTable;

    if(!curTable || curTable != curTable_to)
        return false;

    if(is_value(params.value.toLatin1().at(0)))
    {
        numRow = txt_from.right(txt_from.count()-1).toInt();
        numRow--;
        numColum = txt_from.toLatin1().at(0) - 'A';
    }
    else
        return false;

    if(is_value(params_to.value.toLatin1().at(0)))
    {
        numRow_to = txt_to.right(txt_to.count()-1).toInt();
        numRow_to--;
        numColum_to = txt_to.toLatin1().at(0) - 'A';
    }
    else
        return false;


    if(numColum_to < numColum)
    {
        int tmp = numColum;
        numColum = numColum_to;
        numColum_to = tmp;
    }
    if(numRow_to < numRow)
    {
        int tmp = numRow;
        numRow = numRow_to;
        numRow_to = tmp;
    }
    start = lBlocks->indexOf(curTable)+1;
//
    index = curTable->startCell();
    index--;
    for(row = 0; row < curTable->nRows(); row++)
    {
        for(colum = 0; colum < curTable->nColums(); colum++)
        {
            index++;
            if(lBlocks->at(index)->type == BASECELL && row >= numRow && row <= numRow_to && colum >= numColum && colum <= numColum_to)
            {
                if(!(row == numRow && colum == numColum))
                {
                    cellBlock = (EjCellBlock*)lBlocks->at(index);
                    //                cellBlock = (BaseCellBlock*)doc->lBlocks->at(index);
                    calcItem.text = ";";
                    calcItem.value = 0;
                    calcItem.type = CalcItem::NONE;
                    output.append(calcItem);
                    calcItem.text = "";
                    calcItem.type = CalcItem::NUMERIC;
                    calcItem.value = cellBlock->value;
                    output.append(calcItem);
                }
            }
            while(index < curTable->endBlock() && lBlocks->at(index+1)->type != BASECELL)
            {
                index++;
            }
        }
    }
    return true;
}

void EjCalculator::getLinkParams(QString txt, LinkParams &params)
{
    params.numLink = 0;
    params.type = LINK_CLEAN;
    params.value = txt;
    params.numType = -1;
    int i = 0;
    char c;
    QString str;
    while (i < txt.count()) {
        c = txt.at(i).toLatin1();
        if(is_extend(c))
        {
            switch (c) {
            case 'r':
                params.type = LINK_LINK;
                break;
            case 'b':
                params.type = LINK_LABEL;
                break;
            case 't':
                params.type = LINK_TABLE;
                break;
            default:
                params.type = LINK_CLEAN;
                break;
            }
            i++;
            str.clear();
            while (i < txt.count()) {
                c = txt.at(i).toLatin1();
                if(is_num(c))
                {
                    str.append(c);
                }
                else
                    break;
                i++;
            }
            if(params.type == LINK_LINK)
                params.numLink = str.toInt();
            else {
                params.numType = str.toInt();
            }
            params.value = txt.right(txt.count() - i);

        }
        else
            break;

    }
}
