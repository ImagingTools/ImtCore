import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0


Rectangle {
    id: canvasPage;

    anchors.fill: parent;

    property TreeItemModel objectModel: TreeItemModel{};

    Component.onCompleted: {

        //links for test
        let index_link = linkModel.InsertNewItem();
        linkModel.SetData("ObjectId", "02", index_link);
        index_link = linkModel.InsertNewItem();
        linkModel.SetData("ObjectId", "03", index_link);
        index_link = linkModel.InsertNewItem();
        linkModel.SetData("ObjectId", "04", index_link);
        index_link = linkModel.InsertNewItem();
        linkModel.SetData("ObjectId", "05", index_link);

        let index_link2 = linkModel2.InsertNewItem();
        linkModel2.SetData("ObjectId", "02", index_link2);
        index_link2 = linkModel2.InsertNewItem();
        linkModel2.SetData("ObjectId", "03", index_link2);

        let index_link3 = linkModel3.InsertNewItem();
        linkModel3.SetData("ObjectId", "02", index_link3);
        index_link3 = linkModel3.InsertNewItem();
        linkModel3.SetData("ObjectId", "03", index_link3);

        //

        let index = objectModel.InsertNewItem();
        objectModel.SetData("Id", "01", index);
        objectModel.SetData("X", 0.3, index);
        objectModel.SetData("Y", 0.3, index);
        objectModel.SetData("MainText", "Main text Main text Main text", index);
        objectModel.SetData("SecondText", "Second text", index);
        objectModel.SetExternTreeModel("Links", linkModel, index);

        index = objectModel.InsertNewItem();
        objectModel.SetData("Id", "02", index);
        objectModel.SetData("X", 0.6, index);
        objectModel.SetData("Y", 0.6, index);
        objectModel.SetData("MainText", "Main text 2", index);
        objectModel.SetData("SecondText", "Second text 2", index);

        index = objectModel.InsertNewItem();
        objectModel.SetData("Id", "03", index);
        objectModel.SetData("X", 0.1, index);
        objectModel.SetData("Y", 0.1, index);
        objectModel.SetData("MainText", "Main text 3", index);
        objectModel.SetData("SecondText", "Second text 3", index);


        index = objectModel.InsertNewItem();
        objectModel.SetData("Id", "04", index);
        objectModel.SetData("X", 0.6, index);
        objectModel.SetData("Y", 0.1, index);
        objectModel.SetData("MainText", "Main text 4", index);
        objectModel.SetData("SecondText", "Second text 4", index);
        objectModel.SetExternTreeModel("Links", linkModel3, index);

        index = objectModel.InsertNewItem();
        objectModel.SetData("Id", "05", index);
        objectModel.SetData("X", 0.1, index);
        objectModel.SetData("Y", 0.6, index);
        objectModel.SetData("MainText", "Main text 5", index);
        objectModel.SetData("SecondText", "Second text 5", index);
        objectModel.SetExternTreeModel("Links", linkModel2, index);

        canvas.requestPaint()
    }

    function findModelIndex(id){
        let ind = -1;
        for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
            if(canvasPage.objectModel.GetData("Id", i) == id){
                ind = i;
                break;
            }
        }
        return ind;
    }

    TreeItemModel {id: linkModel;/*for test*/}
    TreeItemModel {id: linkModel2;/*for test*/}
    TreeItemModel {id: linkModel3;/*for test*/}


    Rectangle{
        id: mainContainer;

        anchors.centerIn: parent;

        width: parent.width * canvas.scaleCoeff;
        height:  parent.height * canvas.scaleCoeff;
        color: "transparent";
        border.color: "lightgrey";
        border.width: 2;
        radius: 4;

        ControlArea{
            id: controlArea;

            anchors.fill: parent;

            onClicked: {
                if(!wasMoving){
                    if(canvas.foundIndex >= 0){
                        for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
                            canvasPage.objectModel.SetData("Selected", (i == canvas.foundIndex), i);
                        }
                    }
                    else {
                        for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
                            canvasPage.objectModel.SetData("Selected", false, i);
                        }
                    }
                    canvas.requestPaint();
                }
            }

            onPressed: {
                canvas.foundIndex = -1;
                for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
                    let x_  = canvasPage.objectModel.GetData("X", i)
                    let y_  = canvasPage.objectModel.GetData("Y", i)
                    let width_ = canvasPage.objectModel.IsValidData("Width", i) ? canvasPage.objectModel.GetData("Width", i) * canvas.scaleCoeff : canvas.mainRec_width;

                    let ok = checkInsideMovingItem(canvas.width * x_, canvas.height * y_, width_, canvas.mainRec_height);

                    if(ok){
                        canvas.foundIndex = i;
                        break;
                    }
                }
            }

            onWheelDeltaSignal: {
                if(wheelDelta > 0){//up
                    canvas.scaleCoeff += scaleStep;
                }
                else{//down
                    if(canvas.scaleCoeff > scaleStep){
                        canvas.scaleCoeff -= scaleStep;
                    }
                }
            }

            function movingFunction(delta){
                if(canvas.foundIndex >= 0){
                    let x_  = canvasPage.objectModel.GetData("X", canvas.foundIndex);
                    let y_  = canvasPage.objectModel.GetData("Y", canvas.foundIndex);
                    let width_ = canvasPage.objectModel.IsValidData("Width", canvas.foundIndex) ? canvasPage.objectModel.GetData("Width", canvas.foundIndex) * canvas.scaleCoeff : canvas.mainRec_width;

                    let withinBorders_ = withinBorders(delta, canvas.width * x_, canvas.height * y_, width_, canvas.mainRec_height);

                    if(withinBorders_){
                        let newX = (canvas.width * canvasPage.objectModel.GetData("X", canvas.foundIndex) + delta.x)/canvas.width;
                        let newY = (canvas.height * canvasPage.objectModel.GetData("Y", canvas.foundIndex) + delta.y)/canvas.height;

                        canvasPage.objectModel.SetData("X",newX, canvas.foundIndex);
                        canvasPage.objectModel.SetData("Y",newY, canvas.foundIndex);

                        canvas.requestPaint();
                    }
                }
            }

            //hover reaction
            onPositionSignal: {
                positionChangedPause.position = position;
                positionChangedPause.restart();
            }
            PauseAnimation {
                id: positionChangedPause;

                duration: 100;
                property point position;
                onFinished: {
                    //console.log(position.x, position.y)
                    let objectIndex = -1;
                    for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
                        let x_  = canvasPage.objectModel.GetData("X", i)
                        let y_  = canvasPage.objectModel.GetData("Y", i)
                        let width_ = canvasPage.objectModel.IsValidData("Width", i) ? canvasPage.objectModel.GetData("Width", i) * canvas.scaleCoeff : canvas.mainRec_width;

                        let ok = controlArea.checkHoverItem(canvas.width * x_, canvas.height * y_, width_, canvas.mainRec_height, position);
                        if(ok){
                            objectIndex = i;
                            break;
                        }
                    }
                    if(objectIndex >=0){
                        //console.log("FOUND")
                        canvas.linkSelected = true;//TEST
                        canvas.requestPaint();
                    }
                    else if(canvas.linkSelected){
                        canvas.linkSelected = false;
                        canvas.requestPaint();
                    }
                }
            }
            //hover reaction

        }

        Canvas {

            id: canvas

            anchors.fill: parent

            antialiasing: true

            property color strokeStyle:  "#ff6600"
            property color fillStyle: "#ff6600"
            property int lineWidth: 2 * scaleCoeff;

            property real scaleCoeff: 1.;

            //variables
            property real mainRec_width: 250 * scaleCoeff;
            property real mainRec_height: 60 * scaleCoeff;

            property int foundIndex: -1;

            property int fontSize: 20 * scaleCoeff;
            property int fontSizeSmall: 14 * scaleCoeff;
            property int radius_: 2 * scaleCoeff;
            property int borderShift: 4 * scaleCoeff;

            property string imageUrl_1: "../../../" + Style.theme + "/Icons" + "/AppIcon.svg"
            property string imageUrl_2: "../../../" + Style.theme + "/Icons" + "/Sickle.svg"
            property real imageSize: 20 * scaleCoeff;
            property real imageMargin: 4 * scaleCoeff;

            property string selectedColor: "#90EE90";

            property string selectedLinkColor: "#90EE90";
            property string linkColor: linkSelected ? selectedLinkColor : "#ff6600";
            property bool linkSelected: false;//TEST
            //variables

            Component.onCompleted: {
                loadImage(imageUrl_1);
                loadImage(imageUrl_2);
            }

            onWidthChanged: {
                requestPaint()
            }

            onHeightChanged: {
                requestPaint()
            }

            onImageLoaded: {
                //console.log("Image loaded")
                requestPaint()
            }

            onScaleCoeffChanged: {
                requestPaint()
            }

            onPaint: {
                var ctx = canvas.getContext('2d');
                ctx.clearRect(0, 0, canvas.width, canvas.height)

                //width calculation
                for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
                    setObjectWidth(ctx, i);
                }

                //drawLink
                for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
                    if(canvasPage.objectModel.GetData("Links", i) !==undefined){
                        let links = objectModel.GetData("Links",i);
                        for(let k = 0; k < links.GetItemsCount(); k++){
                            let objectId = links.GetData("ObjectId", k);
                            let ind = canvasPage.findModelIndex(objectId);
                            canvas.drawLink(ctx, ind, i);
                        }

                    }
                }

                //drawObject
                for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
                    drawObject(ctx, i);
                }

            }//onPaint

            function setObjectWidth(ctx, index){
                //width calculation
                let mainText  = canvasPage.objectModel.GetData("MainText", index)
                let secondText  = canvasPage.objectModel.GetData("SecondText", index)
                let width_ = canvasPage.objectModel.IsValidData("Width", index) ? canvasPage.objectModel.GetData("Width", index) * canvas.scaleCoeff : canvas.mainRec_width;

                ctx.lineWidth = 1;
                let fontStr_main = String(canvas.fontSize) + "px sans-serif"
                ctx.font = fontStr_main; //"20px sans-serif";
                let textStr_main = mainText
                let textWidth_main = ctx.measureText(mainText).width

                ctx.lineWidth = 0.5;
                let fontStr_second = String(canvas.fontSizeSmall) + "px sans-serif"
                ctx.font = fontStr_second;
                let textStr_second = secondText;
                let textWidth_second = ctx.measureText(secondText).width

                let add = 2 * canvas.imageSize + 2 * canvas.imageMargin + 2 * canvas.borderShift + 30 * scaleCoeff;
                let mainRecWidth = Math.max(textWidth_main + add, textWidth_second + add, width_)
                canvasPage.objectModel.SetData("Width", mainRecWidth / scaleCoeff, index);

                return mainRecWidth;
            }

            function drawObject(ctx, index){

                let x_  = canvas.width * canvasPage.objectModel.GetData("X", index)
                let y_  = canvas.height * canvasPage.objectModel.GetData("Y", index)
                let width_ = canvasPage.objectModel.IsValidData("Width", index) ? canvasPage.objectModel.GetData("Width", index) * canvas.scaleCoeff : canvas.mainRec_width;
                let mainText  = canvasPage.objectModel.GetData("MainText", index)
                let secondText  = canvasPage.objectModel.GetData("SecondText", index)
                let selected = canvasPage.objectModel.IsValidData("Selected", index) ? canvasPage.objectModel.GetData("Selected", index) : false;


                ctx.lineCap = "round"
                ctx.lineJoin = "round"

                //width calculation
                let mainRecWidth = setObjectWidth(ctx, index);

                //shadow rectangle
                let shadowSize = 6 * canvas.scaleCoeff;
                ctx.lineWidth = canvas.lineWidth;
                ctx.fillStyle = selected ? Qt.rgba(0.2, 0.8, 0, 0.3) : Qt.rgba(0, 0, 0, 0.2);
                ctx.beginPath()
                ctx.roundedRect(x_ + shadowSize, y_ + shadowSize, mainRecWidth, canvas.mainRec_height, canvas.radius_, canvas.radius_);
                ctx.fill();

                //main rectangle
                ctx.lineWidth = canvas.lineWidth;
                ctx.strokeStyle = "#ff6600"
                ctx.fillStyle = selected ? canvas.selectedColor : "#ffffff";
                ctx.beginPath()
                ctx.roundedRect(x_, y_, mainRecWidth, canvas.mainRec_height, canvas.radius_, canvas.radius_);
                ctx.fill();
                ctx.stroke();

                //inner rectangle
                ctx.strokeStyle = "#808080"

                ctx.beginPath()
                ctx.roundedRect(x_ + canvas.borderShift, y_ + canvas.borderShift, mainRecWidth - 2 * canvas.borderShift, canvas.mainRec_height - 2 * canvas.borderShift, canvas.radius_, canvas.radius_);
                ctx.stroke();

                //Main text
                ctx.strokeStyle = "#000000"
                ctx.fillStyle = "#000000"
                ctx.lineWidth = 1;
                let fontStr = String(canvas.fontSize) + "px sans-serif"
                ctx.font = fontStr; //"20px sans-serif";
                let textStr = mainText
                let textWidth = ctx.measureText(mainText).width

                let text_x = x_ + canvas.borderShift + 8 * canvas.scaleCoeff;
                let text_y = y_ + canvas.borderShift + 22 * canvas.scaleCoeff;

                ctx.beginPath()
                ctx.fillText(textStr, text_x, text_y);
                ctx.strokeText(textStr, text_x, text_y);

                //Second text
                ctx.strokeStyle = "#808080"
                ctx.fillStyle = "#808080"
                ctx.lineWidth = 0.5;
                let fontStr2 = String(canvas.fontSizeSmall) + "px sans-serif"
                ctx.font = fontStr2;
                let textStr2 = secondText;
                let textWidth2 = ctx.measureText(secondText).width

                let text_x2 = x_ + canvas.borderShift + 8 * canvas.scaleCoeff;
                let text_y2 = y_ + canvas.mainRec_height - canvas.borderShift - 8 * canvas.scaleCoeff;

                ctx.beginPath()
                ctx.fillText(textStr2, text_x2, text_y2);
                ctx.strokeText(textStr2, text_x2, text_y2);

                //images
                let image1_x = x_ + mainRecWidth - canvas.borderShift - canvas.imageSize - canvas.imageMargin;
                let image1_y = text_y - canvas.imageSize + canvas.imageMargin;
                let image2_x = image1_x - canvas.imageSize - canvas.imageMargin;
                let image2_y = text_y - canvas.imageSize + canvas.imageMargin;
                ctx.beginPath()
                ctx.drawImage(imageUrl_1, image1_x, image1_y, canvas.imageSize, canvas.imageSize);
                ctx.drawImage(imageUrl_2, image2_x, image2_y, canvas.imageSize, canvas.imageSize);

            }

            function drawLink(ctx, fromIndex, toIndex){

                let x1 = canvasPage.objectModel.GetData("X", fromIndex);
                let y1 = canvasPage.objectModel.GetData("Y", fromIndex);
                let x2 = canvasPage.objectModel.GetData("X", toIndex);
                let y2 = canvasPage.objectModel.GetData("Y", toIndex);
                let width1 = canvasPage.objectModel.IsValidData("Width", fromIndex) ? canvasPage.objectModel.GetData("Width", fromIndex) * canvas.scaleCoeff : canvas.mainRec_width;
                let width2 = canvasPage.objectModel.IsValidData("Width", toIndex) ? canvasPage.objectModel.GetData("Width", toIndex) * canvas.scaleCoeff : canvas.mainRec_width;

                let x1_link = canvas.width * x1 + width1/2;
                let y1_link = canvas.height * y1 + canvas.mainRec_height/2;
                let x2_link = canvas.width * x2 + width2/2;
                let y2_link = canvas.height * y2 + canvas.mainRec_height/2;

                ctx.lineCap = "round"
                ctx.lineJoin = "round"
                ctx.lineWidth = Math.max(canvas.lineWidth, 0.5);
                ctx.strokeStyle = linkColor//"#ff6600"
                ctx.fillStyle = linkColor//"#ff6600";

                ctx.beginPath()
                ctx.moveTo(x1_link, y1_link);
                ctx.lineTo(x2_link, y2_link);
                ctx.stroke();

                //draw intersection
                let x1_rec2;
                let y1_rec2;
                let x2_rec2;
                let y2_rec2;

                let intersection;

                if(y1 > y2){
                    //bottom line
                    x1_rec2 = canvas.width * x2;
                    y1_rec2 = canvas.height * y2 + canvas.mainRec_height;
                    x2_rec2 = canvas.width * x2 + width2;
                    y2_rec2 = canvas.height * y2 + canvas.mainRec_height;

                    intersection = findIntersection(x1_link, y1_link, x2_link, y2_link, x1_rec2, y1_rec2, x2_rec2, y2_rec2);

                    if(intersection.x < canvas.width * x2 + width2 && intersection.x > canvas.width * x2){
                        //bottom line
                        x1_rec2 = canvas.width * x2;
                        y1_rec2 = canvas.height * y2 + canvas.mainRec_height;
                        x2_rec2 = canvas.width * x2 + width2;
                        y2_rec2 = canvas.height * y2 + canvas.mainRec_height;
                    }
                    else if(intersection.x >= canvas.width * x2 + width2){
                        //right line
                        x1_rec2 = canvas.width * x2  + width2;
                        y1_rec2 = canvas.height * y2 + canvas.mainRec_height;
                        x2_rec2 = canvas.width * x2 + width2;
                        y2_rec2 = canvas.height * y2;
                    }
                    else if(intersection.x <= canvas.width * x2){
                        //left line
                        x1_rec2 = canvas.width * x2;
                        y1_rec2 = canvas.height * y2 + canvas.mainRec_height;
                        x2_rec2 = canvas.width * x2;
                        y2_rec2 = canvas.height * y2;
                    }
                }//y1 > y2

                else if (y1 < y2){
                    //top line
                    x1_rec2 = canvas.width * x2;
                    y1_rec2 = canvas.height * y2;
                    x2_rec2 = canvas.width * x2 + width2;
                    y2_rec2 = canvas.height * y2;

                    intersection = findIntersection(x1_link, y1_link, x2_link, y2_link, x1_rec2, y1_rec2, x2_rec2, y2_rec2);

                    if(intersection.x < canvas.width * x2 + width2 && intersection.x > canvas.width * x2){
                        //top line
                        x1_rec2 = canvas.width * x2;
                        y1_rec2 = canvas.height * y2;
                        x2_rec2 = canvas.width * x2 + width2;
                        y2_rec2 = canvas.height * y2;
                    }
                    else if(intersection.x >= canvas.width * x2 + width2){
                        //right line
                        x1_rec2 = canvas.width * x2  + width2;
                        y1_rec2 = canvas.height * y2 + canvas.mainRec_height;
                        x2_rec2 = canvas.width * x2 + width2;
                        y2_rec2 = canvas.height * y2;
                    }
                    else if(intersection.x <= canvas.width * x2){
                        //left line
                        x1_rec2 = canvas.width * x2;
                        y1_rec2 = canvas.height * y2 + canvas.mainRec_height;
                        x2_rec2 = canvas.width * x2;
                        y2_rec2 = canvas.height * y2;
                    }

                }//y1 < y2

                else if(y1 == y2){
                    if(x1 >= x2){
                        //right line
                        x1_rec2 = canvas.width * x2  + width2;
                        y1_rec2 = canvas.height * y2 + canvas.mainRec_height;
                        x2_rec2 = canvas.width * x2 + width2;
                        y2_rec2 = canvas.height * y2;


                    }
                    else {
                        //left line
                        x1_rec2 = canvas.width * x2;
                        y1_rec2 = canvas.height * y2 + canvas.mainRec_height;
                        x2_rec2 = canvas.width * x2;
                        y2_rec2 = canvas.height * y2;

                    }
                }//y1 == y2

                intersection = findIntersection(x1_link, y1_link, x2_link, y2_link, x1_rec2, y1_rec2, x2_rec2, y2_rec2);

                drawIntersection(ctx, intersection);

            }

            function drawIntersection(ctx, intersection){
                let size = 20 * canvas.scaleCoeff

                ctx.lineWidth = 0.5 * canvas.scaleCoeff;
                ctx.strokeStyle = linkColor//"#ff6600"
                ctx.fillStyle = linkColor//"#ff6600";
                ctx.beginPath()
                ctx.roundedRect(intersection.x - size/2, intersection.y  - size/2, size, size, size, size);
                ctx.fill();
                ctx.stroke();
            }

            function findIntersection(x1, y1, x2, y2, x3, y3, x4, y4){

                let px = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) /
                    ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));

                let py = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) /
                    ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));


                return Qt.point(px, py);
            }

        }//canvas
    }//mainContainer

    Shortcut {
        sequence: "Ctrl+Z";
        enabled: true;
        onActivated: {
            console.log("Ctrl+z");
        }
    }

}
