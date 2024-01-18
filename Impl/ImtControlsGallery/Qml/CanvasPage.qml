import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0


Rectangle {
    id: canvasPage;

    anchors.fill: parent;
    clip: true;

    property TreeItemModel objectModel: TreeItemModel{};

    //for scrollBars
    property real contentWidth: mainContainer.width.toFixed(3);
    property real contentHeight: mainContainer.height.toFixed(3);
    property real contentX: -mainContainer.x.toFixed(3);
    property real contentY: -mainContainer.y.toFixed(3);
    property real originX: 0;
    property real originY: 0;
    //for scrollBars

    onContentXChanged: {
        if(mainContainer.x !== - contentX){
            mainContainer.x = - contentX;
        }
    }
    onContentYChanged: {
        if(mainContainer.y !== - contentY){
            mainContainer.y = - contentY;
        }
    }

    Component.onCompleted: {
        //TEST
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
        objectModel.SetData("HasError", true, index);

        index = objectModel.InsertNewItem();
        objectModel.SetData("Id", "03", index);
        objectModel.SetData("X", 0.1, index);
        objectModel.SetData("Y", 0.1, index);
        objectModel.SetData("MainText", "Main text 3", index);
        objectModel.SetData("SecondText", "Second text 3", index);
        objectModel.SetData("IsComposite", true, index);

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

        canvas.requestPaint();
        //TEST
    }

    onObjectModelChanged: {
        canvas.requestPaint();
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

    function goInside(){
        if(objectModel.GetItemsCount() > canvas.selectedIndex && canvas.selectedIndex >= 0){
            let isComposite = objectModel.IsValidData("IsComposite", canvas.selectedIndex) ? objectModel.GetData("IsComposite", canvas.selectedIndex) : false;
            if(!isComposite){
                return;
            }
            let id = objectModel.GetData("Id", canvas.selectedIndex);
            console.log("Go inside");
        }
    }

    TreeItemModel {id: linkModel;/*for test*/}
    TreeItemModel {id: linkModel2;/*for test*/}
    TreeItemModel {id: linkModel3;/*for test*/}

    TreeItemModel {
        id: bufferModel;
    }

    TreeItemModel {
        id: emptyModel;
    }

    Rectangle{
        id: mainContainer;

        x: parent.width/2 - width/2;
        y: parent.height/2 - height/2;

        width: parent.width * canvas.scaleCoeff;
        height:  parent.height * canvas.scaleCoeff;
        color: "transparent";
        border.color: "lightgrey";
        border.width: 2;
        radius: 4;

        property real scaleCoeffContainer: canvas.scaleCoeff.toFixed(3);
        property real scaleCoeffContainerPrev: 1;
        property bool hasPositionShift: false;
        onScaleCoeffContainerChanged: {
            //console.log("scaleCoeffContainer:: ", scaleCoeffContainer);

            let ok1 = scaleCoeffContainerPrev <= 1 && scaleCoeffContainer >= 1;
            let ok2 = scaleCoeffContainerPrev >= 1 && scaleCoeffContainer <= 1;

            let ok3 = scaleCoeffContainerPrev > 1 && scaleCoeffContainer > 1;

            let ok4 = scaleCoeffContainerPrev <= 1 && scaleCoeffContainer <= 1;

            let ok = ok3 && hasPositionShift ? false : true;
            if(ok){
                x = parent.width/2 - width/2;
                y = parent.height/2 - height/2;
                hasPositionShift = false;
            }

            scaleCoeffContainerPrev = scaleCoeffContainer;
        }

        onXChanged: {
            if(canvasPage.contentX !== -x){
                canvasPage.contentX = -x
            }
        }

        onYChanged: {
            if(canvasPage.contentY !== -y){
                canvasPage.contentY = -y
            }
        }

        ControlArea{
            id: controlArea;

            anchors.fill: parent;
            cursorShape: Qt.OpenHandCursor;

            onClicked: {
                if(!wasMoving){
                    if(canvas.selectedIndex >= 0){
                        for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
                            canvasPage.objectModel.SetData("Selected", (i == canvas.selectedIndex), i);
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
                canvas.selectedIndex = -1;
                for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
                    let x_  = canvasPage.objectModel.GetData("X", i)
                    let y_  = canvasPage.objectModel.GetData("Y", i)
                    let width_ = canvasPage.objectModel.IsValidData("Width", i) ? canvasPage.objectModel.GetData("Width", i) * canvas.scaleCoeff : canvas.mainRec_width;

                    let ok = checkInsideMovingItem(canvas.width * x_, canvas.height * y_, width_, canvas.mainRec_height);

                    if(ok){
                        canvas.selectedIndex = i;
                        //break;
                    }
                }
                if(canvas.selectedIndex == -1){
                    controlArea.cursorShape = Qt.ClosedHandCursor;
                }
            }

            onReleased: {
                if(canvas.selectedIndex >= 0){
                    controlArea.cursorShape = Qt.ArrowCursor;
                }
                else {
                    controlArea.cursorShape = Qt.OpenHandCursor
                }
            }

            onDoubleClicked: {
                if(canvas.selectedIndex >= 0){
                    canvasPage.goInside();
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
                if(canvas.selectedIndex >= 0){
                    let x_  = canvasPage.objectModel.GetData("X", canvas.selectedIndex);
                    let y_  = canvasPage.objectModel.GetData("Y", canvas.selectedIndex);
                    let width_ = canvasPage.objectModel.IsValidData("Width", canvas.selectedIndex) ? canvasPage.objectModel.GetData("Width", canvas.selectedIndex) * canvas.scaleCoeff : canvas.mainRec_width;

                    let withinBorders_ = withinBorders(delta, canvas.width * x_, canvas.height * y_, width_, canvas.mainRec_height);

                    if(withinBorders_){
                        let newX = (canvas.width * canvasPage.objectModel.GetData("X", canvas.selectedIndex) + delta.x)/canvas.width;
                        let newY = (canvas.height * canvasPage.objectModel.GetData("Y", canvas.selectedIndex) + delta.y)/canvas.height;

                        canvasPage.objectModel.SetData("X",newX, canvas.selectedIndex);
                        canvasPage.objectModel.SetData("Y",newY, canvas.selectedIndex);

                        canvas.requestPaint();
                    }
                }

                //move Container
                else if(canvas.scaleCoeff !== 1){
                    let ok = false;

                    let x_new = mainContainer.x + delta.x * moveContainerAnimX.coeff;
                    let y_new = mainContainer.y + delta.y  * moveContainerAnimX.coeff;

                    if(canvas.scaleCoeff < 1){
                        ok = x_new > 0 && y_new > 0
                                && x_new < canvasPage.width - mainContainer.width
                                && y_new < canvasPage.height - mainContainer.height
                    }

                    else if(canvas.scaleCoeff > 1){
                        ok = x_new > canvasPage.width - mainContainer.width
                                && y_new > canvasPage.height - mainContainer.height
                                && x_new < 0
                                && y_new < 0
                    }

                    if(ok){
                        //mainContainer.hasPositionShift = true;

                        moveContainerAnimX.delta = delta;
                        moveContainerAnimY.delta = delta;
                        moveContainerAnimX.start();
                        moveContainerAnimY.start();
                    }
                }
            }


            NumberAnimation {
                id: moveContainerAnimX;

                duration: 15;
                target: mainContainer;
                property: "x";
                from: mainContainer.x;
                to: mainContainer.x + coeff * delta.x
                property point delta;
                property real coeff: 2.5;
            }

            NumberAnimation {
                id: moveContainerAnimY;

                duration: moveContainerAnimX.duration;
                target: mainContainer;
                property: "y";
                from: mainContainer.y;
                to: mainContainer.y + coeff * delta.y
                property point delta;
                property real coeff: moveContainerAnimX.coeff;

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
                    canvas.hoverIndex = -1;
                    for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
                        let x_  = canvasPage.objectModel.GetData("X", i)
                        let y_  = canvasPage.objectModel.GetData("Y", i)
                        let width_ = canvasPage.objectModel.IsValidData("Width", i) ? canvasPage.objectModel.GetData("Width", i) * canvas.scaleCoeff : canvas.mainRec_width;

                        let ok = controlArea.checkHoverItem(canvas.width * x_, canvas.height * y_, width_, canvas.mainRec_height, position);
                        if(ok){
                            canvas.hoverIndex = i;
                            break;
                        }
                    }
                    if(canvas.hoverIndex >=0){
                        controlArea.cursorShape = Qt.ArrowCursor;
                        canvas.linkSelected = true;
                        canvas.requestPaint();
                    }
                    else {
                        controlArea.cursorShape = Qt.OpenHandCursor;
                        if(canvas.linkSelected){
                            canvas.linkSelected = false;
                            canvas.requestPaint();

                        }
                    }
                }
            }
            //hover reaction

        }

        Canvas {

            id: canvas

            anchors.fill: parent;

            antialiasing: true;

            property real scaleCoeff: 1.;

            property int selectedIndex: -1;
            property int hoverIndex: -1;
            property bool linkSelected: false;
            property bool hasTailSelection: false;

            //sizes
            property real mainRec_width: 250 * scaleCoeff;
            property real mainRec_height: 60 * scaleCoeff;

            property int fontSize: 20 * scaleCoeff;
            property int fontSizeSmall: 14 * scaleCoeff;
            property int radius_: 2 * scaleCoeff;
            property int borderShift: 4 * scaleCoeff;
            property int shadowSize: 6 * scaleCoeff;
            property int backgroundStep: 30 * scaleCoeff;
            property int intersectionSize: 16 * scaleCoeff;
            property int arcRadius: 8 * scaleCoeff;
            property int textMargin: 8 * canvas.scaleCoeff;
            property int textVerticalOffset: 22 * canvas.scaleCoeff;
            property real imageSize: 20 * scaleCoeff;
            property real imageMargin: 4 * scaleCoeff;

            //image urls
            property string imageUrl_1: "../../../" + Style.theme + "/Icons" + "/AppIcon.svg"
            property string imageUrl_2: "../../../" + Style.theme + "/Icons" + "/Sickle.svg"

            //colors
            property string selectedColor: "#00ff00"//"#90EE90";
            property string mainColor: "#335777";
            property string errorColor: "#ff0000";
            property string compositeColor: "#bcd2e8";
            property string compositeSelectedColor: "#1167b1";
            property string selectedLinkColor: "#00ff00"//"#90EE90";
            property string linkColor: "#335777";
            property string mainTextColor: "#000000";
            property string secondTextColor: "#808080";
            property string backgroundGridColor: "#add8e6";
            property string backgroundColor: "#ffffff";
            property string innerFrameColor: "#808080";


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

                drawBackground(ctx);

                //width calculation
                for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
                    setObjectWidth(ctx, i);
                }

                //drawLink
                for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
                    if(canvasPage.objectModel.GetData("Links", i) !==undefined){
                        let links = canvasPage.objectModel.GetData("Links",i);
                        for(let k = 0; k < links.GetItemsCount(); k++){
                            let objectId = links.GetData("ObjectId", k);
                            let ind = canvasPage.findModelIndex(objectId);
                            canvas.drawLink(ctx, ind, i);
                        }

                    }
                }

                //drawObject
                for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
                    if(i !== canvas.selectedIndex){
                        drawObject(ctx, i);
                    }
                }
                if(canvas.selectedIndex >= 0){
                    drawObject(ctx, canvas.selectedIndex);
                }

            }//onPaint

            onPainted: {
                if(pasteShortcut.prevSelectedIndex >= 0){
                    canvas.selectedIndex = pasteShortcut.prevSelectedIndex;
                    pasteShortcut.prevSelectedIndex = -1;
                }
            }

            function drawBackground(ctx){
                let step = canvas.backgroundStep;

                ctx.lineCap = "round"
                ctx.lineJoin = "round"
                ctx.lineWidth = 1;
                ctx.strokeStyle = canvas.backgroundGridColor;
                ctx.fillStyle = canvas.backgroundGridColor;

                for(let i = 1; i * step < canvas.width; i++){//vertical lines
                    let x1 = i * step;
                    let y1 = 0;
                    let x2 = i * step;
                    let y2 = canvas.height;

                    ctx.beginPath()
                    ctx.moveTo(x1, y1);
                    ctx.lineTo(x2, y2);
                    ctx.stroke();
                }

                for(let i = 1; i * step < canvas.height; i++){//horizontal lines
                    let x1 = 0;
                    let y1 = i * step;
                    let x2 = canvas.width;
                    let y2 = i * step;

                    ctx.beginPath()
                    ctx.moveTo(x1, y1);
                    ctx.lineTo(x2, y2);
                    ctx.stroke();
                }

            }

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
                let hasError = canvasPage.objectModel.IsValidData("HasError", index) ? canvasPage.objectModel.GetData("HasError", index) : false;
                let isComposite = canvasPage.objectModel.IsValidData("IsComposite", index) ? canvasPage.objectModel.GetData("IsComposite", index) : false;

                ctx.lineCap = "round"
                ctx.lineJoin = "round"

                //width calculation
                let mainRecWidth = setObjectWidth(ctx, index);

                //shadow rectangle
                let shadowSize = canvas.shadowSize;
                ctx.lineWidth = 2 * canvas.scaleCoeff;
                ctx.fillStyle = selected ? Qt.rgba(0.2, 0.8, 0, 0.3) : Qt.rgba(0, 0, 0, 0.2);
                ctx.beginPath()
                ctx.roundedRect(x_ + shadowSize, y_ + shadowSize, mainRecWidth, canvas.mainRec_height, canvas.radius_, canvas.radius_);
                ctx.fill();

                //main rectangle
                ctx.lineWidth = 2 * canvas.scaleCoeff;
                ctx.strokeStyle = hasError ? canvas.errorColor : canvas.mainColor;
                ctx.fillStyle = (!selected && !isComposite) ? canvas.backgroundColor :
                                                              (!selected && isComposite) ? canvas.compositeColor:
                                                                                           (selected && isComposite) ? canvas.compositeSelectedColor: (selected && !isComposite) ? canvas.selectedColor : canvas.backgroundColor;
                ctx.beginPath()
                ctx.roundedRect(x_, y_, mainRecWidth, canvas.mainRec_height, canvas.radius_, canvas.radius_);
                ctx.fill();
                ctx.stroke();

                //inner rectangle
                ctx.strokeStyle = canvas.innerFrameColor;

                ctx.beginPath()
                ctx.roundedRect(x_ + canvas.borderShift, y_ + canvas.borderShift, mainRecWidth - 2 * canvas.borderShift, canvas.mainRec_height - 2 * canvas.borderShift, canvas.radius_, canvas.radius_);
                ctx.stroke();

                //Main text
                ctx.strokeStyle = canvas.mainTextColor;
                ctx.fillStyle = canvas.mainTextColor;
                ctx.lineWidth = 1;
                let fontStr = String(canvas.fontSize) + "px sans-serif"
                ctx.font = fontStr; //"20px sans-serif";
                let textStr = mainText
                let textWidth = ctx.measureText(mainText).width

                let text_x = x_ + canvas.borderShift + canvas.textMargin;
                let text_y = y_ + canvas.borderShift + canvas.textVerticalOffset;

                ctx.beginPath()
                ctx.fillText(textStr, text_x, text_y);
                ctx.strokeText(textStr, text_x, text_y);

                //Second text
                ctx.strokeStyle = canvas.secondTextColor;
                ctx.fillStyle = canvas.secondTextColor;
                ctx.lineWidth = 0.5;
                let fontStr2 = String(canvas.fontSizeSmall) + "px sans-serif"
                ctx.font = fontStr2;
                let textStr2 = secondText;
                let textWidth2 = ctx.measureText(secondText).width

                let text_x2 = x_ + canvas.borderShift + canvas.textMargin;
                let text_y2 = y_ + canvas.mainRec_height - canvas.borderShift - canvas.textMargin;

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
                ctx.lineWidth = Math.max(canvas.scaleCoeff *1, 0.5);

                let selected = canvas.linkSelected && canvas.hoverIndex == fromIndex;
                ctx.strokeStyle = selected ? canvas.selectedLinkColor : canvas.linkColor;
                ctx.fillStyle = selected ? canvas.selectedLinkColor : canvas.linkColor;

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

                //for intersection margin
                let angle
                let offset

                let hasMargin = true;
                let complexIntersection = true;
                if(hasMargin){
                    angle = findAngle(x1_link, y1_link,intersection.x, intersection.y)
                    //console.log("angle: ", angle)

                    let quarter =  Math.trunc(angle/(Math.PI/2))
                    let add = 0;

                    //offset length correction
                    if(quarter % 2){
                        add = angle  - quarter * Math.PI/2  < Math.PI/4 ? Math.abs(Math.sin(angle)) : Math.abs(Math.cos(angle))
                    }
                    else {
                        add = angle  - quarter * Math.PI/2  < Math.PI/4 ? Math.abs(Math.cos(angle)) : Math.abs(Math.sin(angle))
                    }

                    let smallAngle = Math.abs(angle  - (quarter + 0) * Math.PI/2) < (Math.PI/180) * 30
                        || Math.abs(angle  - (quarter + 1) * Math.PI/2) < (Math.PI/180) * 30;
                    if(smallAngle){
                        //add = 0.5;
                    }
                    //offset length correction

                    offset = 20 * canvas.scaleCoeff * (1 + add);

                    let newX = intersection.x + offset * Math.cos(angle)
                    let newY = intersection.y + offset * Math.sin(angle)

                    intersection = Qt.point(newX, newY);

                }//for intersection margin

                if(complexIntersection && hasMargin){
                    drawIntersectionExt(ctx, intersection,angle, selected);
                    let selectedArcIndex = canvas.hasTailSelection ? toIndex : fromIndex;
                    let selectedArc = canvas.linkSelected && canvas.hoverIndex == selectedArcIndex;
                    drawIntersectionArc(ctx, intersection, angle + Math.PI, offset, selectedArc)

                }
                else {//simple
                    drawIntersection(ctx, intersection, selected);
                }
            }

            function drawIntersection(ctx, intersection, selected){
                let size = canvas.intersectionSize;

                ctx.lineWidth = 0.5 * canvas.scaleCoeff;
                ctx.strokeStyle = selected ? canvas.selectedLinkColor : canvas.linkColor;
                ctx.fillStyle = selected ? canvas.selectedLinkColor : canvas.linkColor;
                ctx.beginPath()
                ctx.roundedRect(intersection.x - size/2, intersection.y  - size/2, size, size, size, size);
                ctx.fill();
                ctx.stroke();
            }

            function drawIntersectionExt(ctx, intersection, angle, selected){
                let size = canvas.intersectionSize;
                let sizeSmall = size/2;

                //draw white circle
                ctx.lineWidth = 0.5 * canvas.scaleCoeff;
                ctx.strokeStyle = canvas.backgroundColor;
                ctx.fillStyle = canvas.backgroundColor;
                ctx.beginPath()
                ctx.roundedRect(intersection.x - size/2, intersection.y  - size/2, size, size, size, size);
                ctx.fill();
                ctx.stroke();

                //draw colored circle
                ctx.strokeStyle = selected ? canvas.selectedLinkColor : canvas.linkColor;
                ctx.fillStyle = selected ? canvas.selectedLinkColor : canvas.linkColor;
                ctx.beginPath()
                ctx.roundedRect(intersection.x - sizeSmall/2, intersection.y  - sizeSmall/2, sizeSmall, sizeSmall, sizeSmall, sizeSmall);
                ctx.fill();
                ctx.stroke();

                //draw tail

                ctx.beginPath();
                ctx.moveTo(intersection.x, intersection.y)
                ctx.lineWidth = Math.max(canvas.scaleCoeff *1, 0.5);
                let offset = size/2;
                let newX = intersection.x + offset * Math.cos(angle)
                let newY = intersection.y + offset * Math.sin(angle)
                ctx.lineTo(newX, newY);
                ctx.stroke();


            }

            function drawIntersectionArc(ctx, point, angle, offset, selected){
                let rad = canvas.arcRadius;
                let endAngle = 0.6 * Math.PI;
                ctx.lineWidth = 2 * canvas.scaleCoeff;
                ctx.strokeStyle = selected ? canvas.selectedLinkColor : canvas.linkColor;
                ctx.fillStyle = selected ? canvas.selectedLinkColor : canvas.linkColor;
                ctx.beginPath()
                ctx.arc(point.x, point.y, rad, angle , angle - endAngle, true);
                ctx.stroke();
                ctx.beginPath()
                ctx.arc(point.x, point.y, rad, angle , angle + endAngle, false);
                ctx.stroke();

                ctx.beginPath();//draw tail
                let startX = point.x + rad * Math.cos(angle);
                let startY = point.y  + rad * Math.sin(angle);
                ctx.moveTo(startX, startY);
                ctx.lineWidth = Math.max(canvas.scaleCoeff * 1.2, 0.6);
                let finishX = startX + offset * Math.cos(angle)
                let finishY = startY + offset * Math.sin(angle)
                ctx.lineTo(finishX, finishY);
                ctx.stroke();

            }

            function findIntersection(x1, y1, x2, y2, x3, y3, x4, y4){

                let px = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) /
                    ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));

                let py = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) /
                    ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));


                return Qt.point(px, py);
            }

            function findAngle(x1, y1, x2, y2){

                let angle;
                angle = Math.atan((y2 - y1) / (x2 - x1))

                if((x2 == x1) && (y1 == y2)){
                    angle = 0;
                }
                else {
                    let angleBase = Math.atan((y2 - y1) / (x2 - x1))

                    if((y1 == y2) && (x1 > x2)){
                        angle = 0
                    }
                    else if((y1 == y2) && (x1 < x2)){
                        angle = Math.PI;
                    }
                    else if((y1 < y2) && (x1 == x2)){
                        angle = 1.5 * Math.PI;
                    }
                    else if((y1 > y2) && (x1 == x2)){
                        angle = Math.PI/2;
                    }
                    //
                    else if((y1 > y2) && (x1 < x2)){//boottom left
                        angle = angleBase + Math.PI;
                    }
                    else if((y1 < y2) && (x1 < x2)){//top left
                        angle = angleBase + Math.PI;
                    }
                    else if((y1 < y2) && (x1 > x2)){//top right
                        angle = angleBase + 2 * Math.PI;
                    }

                }

                return angle;
            }

        }//canvas
    }//mainContainer


    Shortcut {
        sequence: "Ctrl+C";
        enabled: true;
        onActivated: {
            console.log("Ctrl+C");
            if(canvas.selectedIndex >= 0){
                bufferModel.Clear();
                bufferModel.CopyItemDataFromModel(0, canvasPage.objectModel, canvas.selectedIndex);
                //console.log("bufferModel:: ", bufferModel.toJSON());
            }
        }
    }

    Shortcut {
        id: pasteShortcut;

        sequence: "Ctrl+V";
        enabled: true;
        property int prevSelectedIndex: -1;
        onActivated: {
            console.log("Ctrl+V");
            if(bufferModel.GetItemsCount()){
                bufferModel.SetExternTreeModel("Links", emptyModel, 0);
                let mainText = bufferModel.GetData("MainText") + "_1";
                let secondText = bufferModel.GetData("SecondText") + "_1";
                let x_ = bufferModel.GetData("X") + 10/canvas.width;
                let y_ = bufferModel.GetData("Y") + 10/canvas.height;

                //TO DO: unique Id
                var date = new Date();
                let datString = String(date.valueOf());
                bufferModel.SetData("Id", datString, 0);

                bufferModel.SetData("MainText", mainText, 0);
                bufferModel.SetData("SecondText", secondText, 0);
                bufferModel.SetData("X", x_, 0);
                bufferModel.SetData("Y", y_, 0);
                bufferModel.SetData("Selected", false, 0);
                let index = canvasPage.objectModel.InsertNewItem();
                canvasPage.objectModel.CopyItemDataFromModel(index, bufferModel,0);

                prevSelectedIndex = canvas.selectedIndex;

                canvas.selectedIndex = index;

                canvas.requestPaint();

            }

        }
    }

    Shortcut {
        sequence: "Ctrl+Z";
        enabled: true;
        onActivated: {
            console.log("Ctrl+Z");
        }
    }

    Shortcut {
        sequence: "F2";
        enabled: true;
        onActivated: {
            console.log("F2");
            if(canvas.selectedIndex >= 0){
            }
        }
    }

    Shortcut {
        sequence: "Delete";
        enabled: true;
        onActivated: {
            console.log("Delete");
            if(canvas.selectedIndex >= 0){
                canvasPage.deleteObjectFunc(canvas.selectedIndex);
            }
        }
    }

    function deleteObjectFunc(index){
        console.log("DELETE OBJECT: ", index);
        let id = canvasPage.objectModel.GetData("Id", index);

        //remove links
        for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
            let links = canvasPage.objectModel.GetData("Links", i);
            if(links !== undefined){
                for(let k = 0; k < links.GetItemsCount(); k++){
                    let objectId = links.GetData("ObjectId", k);
                    if(objectId == id){
                        links.RemoveItem(k);
                        break;
                    }
                }
            }
        }

        //remove object
        canvasPage.objectModel.RemoveItem(index);

        canvas.selectedIndex = -1;

        canvas.requestPaint();

    }

    CustomScrollbar{
        id: scrollVert;

        anchors.right: parent.right;
        anchors.rightMargin: 1;
        anchors.bottom: parent.bottom;

        secondSize: 12;
        backgroundColor: "transparent";
        Component.onCompleted: {
            opacity = 0.7;
        }
        targetItem: parent;
    }

    CustomScrollbar{
        id: scrollHoriz;

        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 1;

        secondSize: 12;
        backgroundColor: "transparent";
        Component.onCompleted: {
            opacity = 0.7;
        }
        vertical: false;
        targetItem: parent;
    }

}
