#include "ejcsvdocument.h"
int EjCSVDocument::getCountStrFromCurrentTable() const
{
    return countStrFromCurrentTable;
}

void EjCSVDocument::setCountStrFromCurrentTable(int value)
{
    countStrFromCurrentTable = value;
}

void EjCSVDocument::setCountColFromCurrentTable(int value)
{
    countColFromCurrentTable = value;
}

QString EjCSVDocument::getText() const
{
    return text;
}


EjCSVDocument::EjCSVDocument()
{
}

void EjCSVDocument::setFileName(QString fileName)
{
    file.setFileName(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    out.setDevice(&file);
}

void EjCSVDocument::addText(QString txt, bool newStr){
    if(!newStr){
        text += txt;
        return;
    }
    out << text;
    addSemicolon();
}

void EjCSVDocument::addTextFromTable(QString text)
{
    out << text;
    if(countColFromCurrentTable == index_column){
        //Если есть еще таблица в которой больше столбцов, то дополняем двоеточиями
        if(countColFromCurrentTable < index_max_column){
            int val = index_max_column - countColFromCurrentTable;
            while(val != 0){
                out << ";";
                val--;
            }
        }
        index_column = 1;
        out << "\n";
    }else{
         index_column++;
         out << ";";
    }
}

void EjCSVDocument::addSpace(){
    text += " ";
}

bool EjCSVDocument::newString(){
    //Добавляем весь текст в файл
    index_string++;
    addText(text, true);
    text = "";
    out << "\n";
    return true;
}

void EjCSVDocument::addSemicolon(){
    for(int i = 0; i != index_max_column - 1; i++){
        out << ";";
    }
}

bool EjCSVDocument::findMaxIndexColumn(QList<EjBlock*> *list){
    int i = 0;
    int max_column = 1;
    while(i < list->size()){
		EjBlock* block = list->at(i);
		if(block->type == e_typeBlocks::EXT_TABLE){
            EjTableBlock* table_block = (EjTableBlock*)block;
            if(max_column < table_block->nColums()){
                max_column = table_block->nColums();
            }
        }    
        i++;
    }
    index_max_column = max_column;
    return true;
}

bool EjCSVDocument::getActiveTable() const
{
    return active_table;
}

void EjCSVDocument::setActiveTable(bool value)
{
    active_table = value;
}


