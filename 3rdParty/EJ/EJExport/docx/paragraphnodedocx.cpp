#include "paragraphnodedocx.h"

ParagraphNodeDocx::ParagraphNodeDocx(){

}
QDomElement ParagraphNodeDocx::addParagraph(QList<QDomElement> nodesList){
    QDomDocument doc;
    QListIterator<QDomElement> iterator(nodesList);
    //iter
    QDomElement wp;
    QDomElement wpPr;
    QDomElement wPstyle;
    QDomElement jc;
    //create
    wp = doc.createElement("w:p");
    //Циклом проходим и склеиваем все в один параграф
    while( iterator.hasNext() ){
        wp.appendChild( iterator.peekNext());
        iterator.next();
    }
    return wp;
}

QDomElement ParagraphNodeDocx::addParagraphWithStyles(QList<QDomElement> nodesList, QMap<QString, int> tumbler){
    QDomDocument doc;
    QListIterator<QDomElement> iterator(nodesList);
    //iter
    QDomElement wp;
    QDomElement wpPr;
    QDomElement wPstyle;
    QDomElement wSpac;
    QDomElement jc;
    //create
    wp = doc.createElement("w:p");
    wpPr = doc.createElement("w:pPr");
        wPstyle = doc.createElement("w:pStyle");
        wPstyle.setAttribute("w:val","Normal");
        jc = doc.createElement("w:jc");
    wSpac = doc.createElement("w:spacing");
        wSpac.setAttribute("w:lineRule","auto");
        wSpac.setAttribute("w:line","360");
    wpPr.appendChild(wSpac);

    //Циклом проходим и склеиваем все в один параграф
    wpPr.appendChild(wPstyle);
        if(tumbler.value("Align") == 0){ jc.setAttribute("w:val","left"); wpPr.appendChild(jc);}
        if(tumbler.value("Align") == 1){ jc.setAttribute("w:val","right"); wpPr.appendChild(jc);}
        if(tumbler.value("Align") == 2){ jc.setAttribute("w:val","center");wpPr.appendChild(jc);}
    wp.appendChild(wpPr);
    while( iterator.hasNext() ){
        wp.appendChild( iterator.peekNext());
        iterator.next();
    }
    return wp;
}

QDomElement ParagraphNodeDocx::addParagraphOne(QDomElement node){
        QDomDocument doc;
        //iter
        QDomElement wp;
        //create
        wp = doc.createElement("w:p");
        wp.appendChild(node);
        return wp;
}

QDomElement ParagraphNodeDocx::addParagraphEmpty(){
    QDomDocument doc;

    QDomElement wp;
    wp = doc.createElement("w:p");
    return wp;
}
