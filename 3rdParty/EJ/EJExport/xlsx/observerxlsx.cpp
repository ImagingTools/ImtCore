#include "observerxlsx.h"
#include "tablenodexlsx.h"
#include "textnode.h"
#include "textnodexslx.h"
#include "tablesplug.h"
#include "imageplug.h"
#include "imagenodexlsx.h"
#include "cellnodexlsx.h"
#include "positionnode.h"
#include "zipper.h"

ObserverXlsx::ObserverXlsx(){

}


QXlsx::Format chgFormat(QXlsx::Format format, QMap<QString, int> styles){

    if(styles.value("Bold") == 1 ){
     format.setFontBold(true);
    }
    if(styles.value("Italic") == 1){
        format.setFontItalic(true);
    }
    if(styles.value("UnderLine") == 1 ){
        format.setFontUnderline(QXlsx::Format::FontUnderlineSingle);
    }
    if(styles.value("StrikeOut") == 1){
        format.setFontStrikeOut(true);
    }
    format.setFontSize(styles.value("SizeFont"));
    return format;
}

QMap<QString, int> ObserverXlsx::changeStyle(QMap<QString, int>style_tumbler, NumStyleBlock *nsb){
    style_tumbler.remove("StyleMode");
    style_tumbler.insert("StyleMode", 1);
    if( nsb->style->m_vid == e_PropDoc::PARAGRAPH_STYLE ){
        ParagraphStyle *stl = (ParagraphStyle*)nsb->style;
        if( stl->m_align == ParagraphStyle::AlignLeft){ style_tumbler.insert("Align",0);}
        if( stl->m_align == ParagraphStyle::AlignRight){ style_tumbler.insert("Align",1);}
        if( stl->m_align == ParagraphStyle::AlignHCenter){ style_tumbler.insert("Align",2);}

    }else if( nsb->style->m_vid == e_PropDoc::TEXT_STYLE){
        TextStyle *stl = (TextStyle*)nsb->style;
        style_tumbler.insert("SizeFont", stl->m_font.pixelSize()); //or pixel?
        qDebug() << "<><><>brushColor: " << stl->m_brushColor;
        style_tumbler.insert("RedBrush",stl->m_brushColor.red() );
        style_tumbler.insert("GreenBrush",stl->m_brushColor.green() );
        style_tumbler.insert("BlueBrush",stl->m_brushColor.blue() );

        style_tumbler.insert("Red",stl->m_fontColor.red() );
        style_tumbler.insert("Green",stl->m_fontColor.green() );
        style_tumbler.insert("Blue",stl->m_fontColor.blue());

        if (stl->m_font.bold())         { style_tumbler.insert("Bold",1);}else{style_tumbler.insert("Bold",0);}
        if (stl->m_font.italic() )      { style_tumbler.insert("Italic", 1);}else{style_tumbler.insert("Italic",0);}
        if (stl->m_font.underline())    {style_tumbler.insert("UnderLine",1);}else{style_tumbler.insert("Underline",0);}
        if (stl->m_font.strikeOut() )   { style_tumbler.insert("StrikeOut", 1);}else{style_tumbler.insert("StrikeOut",0);}

        //STYLE
        if( stl->m_fontStyle == TextStyle::NORMAL){
            style_tumbler.insert("SizeFont", 24);
        }
        if( stl->m_fontStyle == TextStyle::NORMAL_BIG1){
            style_tumbler.insert("SizeFont", 30);
        }
        if( stl->m_fontStyle == TextStyle::NORMAL_BIG2){
            style_tumbler.insert("SizeFont", 32);
        }
        if( stl->m_fontStyle == TextStyle::NORMAL_SMALL1){
            style_tumbler.insert("SizeFont", 21);
        }
        if( stl->m_fontStyle == TextStyle::NORMAL_SMALL2){
            style_tumbler.insert("SizeFont", 16);
        }
        if( stl->m_fontStyle == TextStyle::HEADING1){
            style_tumbler.insert("Bold",1);
            style_tumbler.insert("Italic",0);
            style_tumbler.insert("UnderLine",0);
            style_tumbler.insert("StrikeOut",0);
            style_tumbler.insert("SizeFont", 38);
        }
        if( stl->m_fontStyle == TextStyle::HEADING2){
            style_tumbler.insert("Bold",1);
            style_tumbler.insert("Italic",0);
            style_tumbler.insert("UnderLine",0);
            style_tumbler.insert("StrikeOut",0);
            style_tumbler.insert("SizeFont", 36);
        }
        if( stl->m_fontStyle == TextStyle::HEADING3){
            style_tumbler.insert("Bold",1);
            style_tumbler.insert("Italic",0);
            style_tumbler.insert("UnderLine",0);
            style_tumbler.insert("StrikeOut",0);
            style_tumbler.insert("SizeFont", 28);
        }
        if( stl->m_fontStyle == TextStyle::HEADING4){
            style_tumbler.insert("Bold",1);
            style_tumbler.insert("Italic",1);
            style_tumbler.insert("UnderLine",0);
            style_tumbler.insert("StrikeOut",0);
            style_tumbler.insert("SizeFont", 26);
        }
        if( stl->m_fontStyle == TextStyle::HEADING5){
            style_tumbler.insert("Bold",1);
            style_tumbler.insert("Italic",1);
            style_tumbler.insert("UnderLine",1);
            style_tumbler.insert("StrikeOut",0);
            style_tumbler.insert("SizeFont", 38);
        }

    }

    return style_tumbler;
}

void ObserverXlsx::observe(Document *doc, const QString fileName){

    QXlsx::Document xlsx;
    QXlsx::Format format;
    int index = 0;
    QList<Block*> *lBlocks = doc->lBlocks;
    Block *curBlock;
    QList<QString> lText;
    QMap<QString, int> styles;

    int columnXlsx =1;
    int rowXlsx = 1;


    //chg
    while ( index < doc->lBlocks->count()) {
        curBlock = lBlocks->at(index);

        qDebug() << " ---";

        if( curBlock->type == e_typeBlocks::TEXT){
            TextBlock *blocText = (TextBlock*)curBlock;
            lText.append(blocText->text);


        }

        if( curBlock->type == e_typeBlocks::SPACE){
            lText.append(" ");
        }

        if(curBlock->type == e_typeBlocks::ENTER){
            QListIterator<QString> iterator(lText);
            QString string;
            while( iterator.hasNext()){
                string += iterator.next();
            }
            lText.clear();
            if (styles.value("StyleMode") == 0){
                xlsx.write(rowXlsx, columnXlsx, string);
            }
            if( styles.value("StyleMode") == 1 ){
                xlsx.write(rowXlsx, columnXlsx, string,format);
            }
            rowXlsx++;
        }

        // <--- STYLES
        if(curBlock->type == e_typeBlocks::NUM_STYLE){
            //format = chgFormat(format, styles);
            NumStyleBlock *nsb = (NumStyleBlock*)curBlock;
            styles = changeStyle(styles, nsb);
            if(styles.value("Bold") == 1 ){
             format.setFontBold(true);
            }
            if(styles.value("Italic") == 1){
                format.setFontItalic(true);
            }
            if(styles.value("UnderLine") == 1 ){
                format.setFontUnderline(QXlsx::Format::FontUnderlineSingle);
            }
            if(styles.value("StrikeOut") == 1){
                format.setFontStrikeOut(true);
            }
            format.setFontSize(styles.value("SizeFont"));
        }

        if(curBlock->type == e_typeBlocks::EXT_IMAGE){}

        if(curBlock->type == e_typeBlocks::EXT_TABLE){}

        index++;
    }
    QString pathTempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString fullPath = pathTempDir + "/" +fileName + ".xlsx";


    xlsx.saveAs(fullPath);

}


