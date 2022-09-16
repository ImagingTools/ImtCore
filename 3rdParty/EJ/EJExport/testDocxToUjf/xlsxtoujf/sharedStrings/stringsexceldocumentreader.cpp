#include "stringsexceldocumentreaders.h"
#include <QDebug>

StringsExcelDocumentReader::~StringsExcelDocumentReader(){}

bool TagSiExcelDocumentReader::read(SharedStrings& document, QDomNode node,
                                    QMap<QString, StringsExcelDocumentReader*> *editors){
    document.curText = "";
    bool result = true;
    node = node.firstChild();
    while (!node.isNull()) {
       StringsExcelDocumentReader *editor = editors->value(node.nodeName());
       if(editor != nullptr){
           result &= editor->read(document, node, editors);
       }
       node = node.nextSibling();
    }
    qDebug() << "Добавленный текст" <<document.curText;
    document.addString(document.curText);
    return result;
}

bool TagRExcelDocumentReader::read(SharedStrings& document, QDomNode node,
                                    QMap<QString, StringsExcelDocumentReader*> *editorsStrings){
    bool result = true;
    node = node.firstChild();
    while (!node.isNull()) {
       StringsExcelDocumentReader *editor = editorsStrings->value(node.nodeName());
       if(editor != nullptr){
           result &= editor->read(document, node, editorsStrings);
       }
       node = node.nextSibling();
    }

    return result;
}

bool TagRprExcelDocumentReader::read(SharedStrings& document, QDomNode node,
                                    QMap<QString, StringsExcelDocumentReader*> *editorsStrings){

    bool result = true;
    node = node.firstChild();
    while (!node.isNull()) {
       StringsExcelDocumentReader *editor = editorsStrings->value(node.nodeName());
       if(editor != nullptr){
           result &= editor->read(document, node, editorsStrings);
       }
       node = node.nextSibling();
    }

    return result;
}

bool TagTExcelDocumentReader::read(SharedStrings& document, QDomNode node,
                                    QMap<QString, StringsExcelDocumentReader*> *editorsStrings){
    node = node.firstChild();
    if(node.isText()){
       QString text = node.nodeValue();
        document.curText += node.nodeValue();
    }
    return true;
}
