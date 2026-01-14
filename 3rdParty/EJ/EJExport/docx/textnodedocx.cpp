#include "textnodedocx.h"

TextNodeDocx::TextNodeDocx(){

}

struct s_color{
    int red;
    int green;
    int blue;
    QString name;
};

QString TextNodeDocx::colorNear(int red, int green, int blue){
    QString color;
    qDebug() << "1:" << red;
    qDebug() << green;
    qDebug() << blue;
    if( red == 255){
        qDebug() << " ";
    }
    double f;
    double min = std::numeric_limits<double>::infinity();
    const int number_color = 30;
    s_color array[number_color]= {
        {0,0,0, "black"},
        {255, 0, 0, "red"},                  //red
        {139,0,0, "darkRed"},
        {220,20,60,"crimson"},
        {250,128,114, "salmon"},
        {255,99,71, "tomato"},
        {0, 255, 0, "green"},                //green
        {0,100,0,"darkGreen"},
        {0, 0, 255, "blue"},                //blue
        {0,0,139, "darkBlue"},
        {175,216,230, "lightBlue"},
        {255,255,0, "yellow"},              //yellow
        {204,204,0, "darkYellow"},
        {255,255,204, "lightYellow"},
        {0,255,255, "cyan"},                //cyan
        {0,139,139, "darkCyan"},
        {225,255,255, "lightCyan"},
        {255,0,255, "magenta"},             //magenta
        {139,0,139, "darkMagenta"},
        {106,90,205, "slateBlue"},
        {128,128,128, "gray"},                    //gray
        {211,211,211,"lightGray"},
        {192,192,192, "silver"},
        {169,169,169,"darkGray"}
    };
    for (int i = 0; i < number_color; ++i){
        s_color const& color_rgb = array[i];
        f = 30*(color_rgb.red - red)*(color_rgb.red - red) +
                59*(color_rgb.green - green)*(color_rgb.green - green) +
                11*(color_rgb.blue - blue)*(color_rgb.blue - blue);
        if (f < min) {
            min = f;
            color = color_rgb.name;
        }
    }
    qDebug() << "Allena: " << color;
    return color;
}

QDomElement TextNodeDocx::addTextWithStyle(QString text, QMap<QString, int> tumbler){
    //init nodes
    QDomDocument doc;

    QDomElement wr;
    QDomElement wt;
    QDomText txt;
    //Style details


    QDomElement wColor;
    QDomElement wrPr;
    int sizeFont;
    QString szFont;
    QDomElement sizeNode;
    QDomElement sizeCsNode;
    QDomElement wTblInd; //!
    QDomElement bold;
    QDomElement boldCs;
    QDomElement italic;
    QDomElement italicCs;
    QDomElement underline;
    QDomElement strikeLine;
    QDomElement wrFonts;
    QDomElement wHighlight;



    //add in xml-doc
    wr = doc.createElement("w:r");
    wt = doc.createElement("w:t");
    wt.setAttribute("xml:space","preserve");
    txt = doc.createTextNode(text);

    //STYLE 
    sizeFont =tumbler.value("SizeFont");//12
    szFont = QString::number(sizeFont);
    // COLOR FONT
    //color.remove("#");
    //r,g,b->fullcolor
    QString strRed;
    if(tumbler.value("Red") == 0){
        strRed = "00";
    }else{
        strRed = QString("%1").arg(tumbler.value("Red"), 0,16).toUpper();
    }
    QString strGreen;
    if(tumbler.value("Green") == 0){
        strGreen = "00";
    }else{
        strGreen = QString("%1").arg(tumbler.value("Green"), 0,16).toUpper();
    }
    QString strBlue;
    if(tumbler.value("Blue") == 0){
        strBlue = "00";
    }else{
        strBlue = QString("%1").arg(tumbler.value("Blue"), 0,16).toUpper();
    }
    QString color = strRed + strGreen + strBlue;

    //BRUSH
    QString highlight;
    if( tumbler.value("AlphaBrush") != 0){
         highlight = colorNear(tumbler.value("RedBrush"),tumbler.value("GreenBrush"),tumbler.value("BlueBrush"));
         wHighlight = doc.createElement("w:highlight");
             wHighlight.setAttribute("w:val", highlight);
        wrPr.appendChild(wHighlight);
    }


    //color = QString("%1").arg( tumbler.value("ColorFont"),0,16).toUpper();
    if( color.size() != 6){
        color.resize(6, '0');
        qDebug() << "|!| Color:  " << color;
    }

    sizeNode = doc.createElement("w:sz");
    sizeNode.setAttribute("w:val",szFont);
    sizeCsNode=doc.createElement("w:szCs");
    sizeCsNode.setAttribute("w:val", szFont);
    bold = doc.createElement("w:b");
    boldCs = doc.createElement("w:bCs");
    italic = doc.createElement("w:i");
    italicCs = doc.createElement("w:iCs");
    wrPr = doc.createElement("w:rPr");
    wColor = doc.createElement("w:color");
    underline = doc.createElement("w:u");
    strikeLine = doc.createElement("w:strike");
    wColor.setAttribute("w:val", color);


    wrPr.appendChild(wColor);
    //infoFont.
    wrFonts = doc.createElement("w:rFonts");
    wrFonts.setAttribute("w:ascii", "Helvetica");
    wrFonts.setAttribute("w:hAnsi", "Helvetica");
    wrPr.appendChild(wrFonts);
    if( tumbler.value("Bold") == 1){
        wrPr.appendChild(bold);
        wrPr.appendChild(boldCs);
    }
    if( tumbler.value("Italic") == 1){
        wrPr.appendChild(italic);
        wrPr.appendChild(italicCs);
    }
    if( tumbler.value("UnderLine") == 1){
        underline.setAttribute("w:val","single");
        wrPr.appendChild(underline);
    }
    if( tumbler.value("StrikeOut") == 1){
        wrPr.appendChild(strikeLine);
    }

    wrPr.appendChild(sizeNode);
    wrPr.appendChild(sizeCsNode);
    //wrPr.appendChild(wHighlight);
    wr.appendChild(wrPr);
    //FINAL

    wt.appendChild(txt);
    wr.appendChild(wt);

    return wr;
}

QDomElement TextNodeDocx::addText(QString textI){

    //init nodes
    QDomDocument doc;
    QDomElement wr;
    QDomElement wt;
    QDomText txt;
    QDomElement wrFonts;

    QDomElement wrPr;
    QDomElement sizeNode;
    QDomElement sizeCsNode;
    wrFonts = doc.createElement("w:rFonts");
    wrFonts.setAttribute("w:ascii", "Helvetica");
    wrFonts.setAttribute("w:hAnsi", "Helvetica");
    wrPr = doc.createElement("w:rPr");
    int sizeFont = 24;
    QString szFont = QString::number(sizeFont);
    sizeNode = doc.createElement("w:sz");
    sizeNode.setAttribute("w:val",szFont);
    sizeCsNode=doc.createElement("w:szCs");
    sizeCsNode.setAttribute("w:val", szFont);

    //glue

    wrPr.appendChild(sizeNode);
    wrPr.appendChild(sizeCsNode);
    wrPr.appendChild(wrFonts);

    //add in xml-doc
    wr = doc.createElement("w:r");
    wt = doc.createElement("w:t");
        wt.setAttribute("xml:space","preserve");
    txt = doc.createTextNode(textI);
    //STYLE

    //FINAL
    wt.appendChild(txt);
    wr.appendChild(wrPr);
    wr.appendChild(wt);


    return wr;
}
