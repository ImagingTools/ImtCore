#include "imagenodedocx.h"

ImageNodeDocx::ImageNodeDocx(){

}

QDomElement ImageNodeDocx::addImage(QString name, int x, int y, int id, int idJ){

    QDomDocument doc;
    QString cx = QString::number(x);
    QString cy = QString::number(y);
    QString rId = QString::number(id);
    QDomElement wr;
    wr = doc.createElement("w:r");

        //Init
        QDomElement wDrawning;
        QDomElement wpInline;
            QDomElement wpExtent;
            QDomElement wpEffectExtent;
            QDomElement wpDocPr;
            QDomElement wpCNvGraphicFramePr;
                QDomElement aGraphicFrameLocks;
            QDomElement aGraphic;
                QDomElement aGraphicData;
                    QDomElement picPic;
                        QDomElement picNvPicPr;
                            QDomElement picCNvPr;
                            QDomElement picCNvPicPr;
                                QDomElement aPicLocks;
                        QDomElement picBlipFill;
                            QDomElement aBlip;
                            QDomElement aStretch;
                                QDomElement aFillRect;
                        QDomElement picSpPr;
                            QDomElement aXfrm;
                                QDomElement aOff;
                                QDomElement aExt;
                            QDomElement aPrstGeom;
                                QDomElement aAvLst;
        //Create
        wDrawning = doc.createElement("w:drawing");
        wpInline = doc.createElement("wp:inline");
            wpInline.setAttribute("distT", "0");
            wpInline.setAttribute("distB", "0");
            wpInline.setAttribute("distL", "0");
            wpInline.setAttribute("distR", "0");
        wpExtent = doc.createElement("wp:extent");
            wpExtent.setAttribute("cx", cx); // !!!
            wpExtent.setAttribute("cy", cy);
        wpEffectExtent = doc.createElement("wp:effectExtent");
            wpEffectExtent.setAttribute("l", "0");
            wpEffectExtent.setAttribute("t", "0");
            wpEffectExtent.setAttribute("r", "0");
            wpEffectExtent.setAttribute("b", "0");
        wpDocPr = doc.createElement("wp:docPr");
            wpDocPr.setAttribute("id", rId); // !!!!!!!1
            wpDocPr.setAttribute("name", name);
            wpDocPr.setAttribute("descr", name);
        wpCNvGraphicFramePr = doc.createElement("wp:cNvGraphicFramePr");
        aGraphicFrameLocks = doc.createElement("a:graphicFrameLocks");
            aGraphicFrameLocks.setAttribute("xmlns:a", "http://schemas.openxmlformats.org/drawingml/2006/main");
            aGraphicFrameLocks.setAttribute("noChangeAspect", "1");
        aGraphic = doc.createElement("a:graphic");
            aGraphic.setAttribute("xmlns:a","http://schemas.openxmlformats.org/drawingml/2006/main");
        aGraphicData = doc.createElement("a:graphicData");
            aGraphicData.setAttribute("uri", "http://schemas.openxmlformats.org/drawingml/2006/picture");
        picPic = doc.createElement("pic:pic");
            picPic.setAttribute("xmlns:pic", "http://schemas.openxmlformats.org/drawingml/2006/picture");
        picNvPicPr = doc.createElement("pic:nvPicPr");
        picCNvPr = doc.createElement("pic:cNvPr");
            picCNvPicPr.setAttribute("id", idJ); // !!!!!!!
            picCNvPicPr.setAttribute("name", name);
            picCNvPicPr.setAttribute("descr", name);
        picCNvPicPr = doc.createElement("pic:cNvPicPr");
        aPicLocks = doc.createElement("a:picLocks");
            aPicLocks.setAttribute("noChangeAspect", "1");
            aPicLocks.setAttribute("noChangeArrowheads", "1");
        picBlipFill = doc.createElement("pic:blipFill");
        aBlip = doc.createElement("a:blip");
            aBlip.setAttribute("r:embed", "rId" + rId); // !!!!!!
        aStretch = doc.createElement("a:stretch");
        aFillRect = doc.createElement("a:fillRect");
        picSpPr = doc.createElement("pic:spPr");
            picSpPr.setAttribute("bwMode", "auto");
        aXfrm = doc.createElement("a:xfrm");
        aOff = doc.createElement("a:off");
            aOff.setAttribute("x","0");
            aOff.setAttribute("y","0");
        aExt = doc.createElement("a:ext");
            aExt.setAttribute("cx",cx); // !!!!!!!!!
            aExt.setAttribute("cy",cy);
        aPrstGeom = doc.createElement("a:prstGeom");
            aPrstGeom.setAttribute("prst", "rect");
        aAvLst = doc.createElement("a:avLst");

        //glue
        wr.appendChild(wDrawning);
        wDrawning.appendChild(wpInline);
        wpInline.appendChild(wpExtent);
        wpInline.appendChild(wpEffectExtent);
        wpInline.appendChild(wpDocPr);
        wpInline.appendChild(wpCNvGraphicFramePr);
            wpCNvGraphicFramePr.appendChild(aGraphicFrameLocks);
        wpInline.appendChild(aGraphic);
            aGraphic.appendChild(aGraphicData);
            aGraphicData.appendChild(picPic);
            picPic.appendChild(picNvPicPr);
                picNvPicPr.appendChild(picCNvPr);
                picNvPicPr.appendChild(picCNvPicPr);
                    picCNvPicPr.appendChild(aPicLocks);
            picPic.appendChild(picBlipFill);
                picBlipFill.appendChild(aBlip);
                picBlipFill.appendChild(aStretch);
                    aStretch.appendChild(aFillRect);
            picPic.appendChild(picSpPr);
                picSpPr.appendChild(aXfrm);
                    aXfrm.appendChild(aOff);
                    aXfrm.appendChild(aExt);
                picSpPr.appendChild(aPrstGeom);
                    aPrstGeom.appendChild(aAvLst);



         return wr;

}

