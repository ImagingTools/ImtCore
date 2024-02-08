import QtQuick 2.12
import Acf 1.0

import imtcontrols 1.0


Rectangle {
    id: canvasPage;

    anchors.fill: parent;
    clip: true;

    color: "#ececec";

    property TreeItemModel objectModel: TreeItemModel{};

    //for scrollBars
    property real contentHeight: backgroundRec.height;
    property real contentY: canvas.deltaY;
    property real originY: 0;

    property real contentWidth: backgroundRec.width;
    property real contentX: canvas.deltaX;
    property real originX: 0;

    onContentXChanged: {
        canvas.deltaX = contentX
        //canvas.requestPaint();
    }
    onContentYChanged: {
        canvas.deltaY = contentY
        //canvas.requestPaint();
    }


    onObjectModelChanged: {
        canvas.requestPaint();
    }

    function requestPaint(){
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

    function correctPosition(addX, addY){
        for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){

            let x_ = canvasPage.objectModel.GetData("X", i) ;
            let y_ = canvasPage.objectModel.GetData("Y", i) ;
            let newX = x_ + addX;
            let newY = y_ + addY;
            canvasPage.objectModel.SetData("X", newX, i);
            canvasPage.objectModel.SetData("Y", newY, i);
        }
    }

    function correctPositionScaled(addX, addY, scale_){
        for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
            //console.log("correctPositionScaled:: ", addX, addY, scale_)
            let x_ = canvasPage.objectModel.GetData("X", i) ;
            let y_ = canvasPage.objectModel.GetData("Y", i) ;
            let width_ = canvasPage.objectModel.GetData("Width", i) ;
            //            let newX = x_ + addX;
            //            let newY = y_ + addY;
            let newX =  x_; // + (width_ * scale_ - width_)/2;
            let newY =  y_ ;
            let newWidth = scale_ * width_;
            canvasPage.objectModel.SetData("X", newX, i);
            canvasPage.objectModel.SetData("Y", newY, i);
            canvasPage.objectModel.SetData("Width", newWidth, i);
        }
    }


    TreeItemModel {
        id: bufferModel;
    }

    TreeItemModel {
        id: emptyModel;
    }

    Item{
        id: mainContainer;

        anchors.fill: parent

        ControlArea {
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
                    let width_ = canvasPage.objectModel.IsValidData("Width", i) ? canvasPage.objectModel.GetData("Width", i)  : canvas.mainRec_width;
                    let height_ = canvas.mainRec_height;

                    x_ = x_ * canvas.scaleCoeff + canvas.deltaX;
                    y_ = y_ * canvas.scaleCoeff + canvas.deltaY;
                    width_ = width_ * canvas.scaleCoeff;
                    height_ = height_  * canvas.scaleCoeff;


                    let ok = checkInsideMovingItem(x_, y_, width_, height_);

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

            //            onWheelDeltaSignal: {
            //                if(wheelDelta > 0){//up
            //                    if (canvas.scaleCoeff < 3){
            //                        canvas.scaleCoeff += scaleStep;
            //                    }
            //                }
            //                else{//down
            //                    if(canvas.scaleCoeff > scaleStep * 2){
            //                        canvas.scaleCoeff -= scaleStep;
            //                    }
            //                }
            ////                canvas.markDirty()
            ////                canvas.requestPaint();
            //            }
            onDeltaSignal: {
                if(canvas.selectedIndex < 0){
                    canvas.deltaX += delta.x
                    canvas.deltaY += delta.y

                    //canvasPage.correctPosition(delta.x, delta.y)

                    canvas.requestPaint();
                }
                else {
                    movingFunction(delta);
                }
            }

            onWheel: {
                let scaleCoeffBack = canvas.scaleCoeff;
                let deltaX = (wheel.x + canvas.deltaX) / canvas.scaleCoeff
                let wheelDelta = wheel.angleDelta.y
                let scaleCoeff_ = canvas.scaleCoeff;
                if(wheelDelta > 0){//up
                    if (canvas.scaleCoeff < 3){
                        scaleCoeff_ += scaleStep;
                    }
                }
                else{//down
                    if(canvas.scaleCoeff > scaleStep * 2){
                        scaleCoeff_ -= scaleStep;
                    }
                }
                let newX = (wheel.x - canvas.deltaX) / scaleCoeffBack * scaleCoeff_ + canvas.deltaX
                let newY = (wheel.y - canvas.deltaY) / scaleCoeffBack * scaleCoeff_ + canvas.deltaY
                canvas.deltaX -= (newX - wheel.x)
                canvas.deltaY -= (newY - wheel.y)

                //canvasPage.correctPositionScaled(wheel.x - newX, wheel.y - newY, scaleCoeff_/ scaleCoeffBack)

                canvas.scaleCoeff = scaleCoeff_
                canvas.scaleCoeffPrev = scaleCoeff_;
            }

            function movingFunction(delta){//reimplemented
                if(canvas.selectedIndex >= 0){
                    let x_  = canvasPage.objectModel.GetData("X", canvas.selectedIndex);
                    let y_  = canvasPage.objectModel.GetData("Y", canvas.selectedIndex);
                    let width_ = canvasPage.objectModel.IsValidData("Width", canvas.selectedIndex) ? canvasPage.objectModel.GetData("Width", canvas.selectedIndex)  : canvas.mainRec_width;
                    let height_ = canvas.mainRec_height;

                    x_ = x_ * canvas.scaleCoeff + canvas.deltaX;
                    y_ = y_ * canvas.scaleCoeff + canvas.deltaY;
                    width_ = width_ * canvas.scaleCoeff;
                    height_ = height_  * canvas.scaleCoeff;

                    let withinBorders_ = withinBorders(delta, x_,  y_, width_, height_);

                    if(withinBorders_){
                        let newX = (canvasPage.objectModel.GetData("X", canvas.selectedIndex) + delta.x);
                        let newY = (canvasPage.objectModel.GetData("Y", canvas.selectedIndex) + delta.y);

                        //fit to borders
                        let margin_ = 10;
                        if(newX < margin_){
                            newX = margin_;
                        }
                        if(newY < margin_){
                            newY = margin_;
                        }
                        if(newX > (backgroundRec.width  - width_) / canvas.scaleCoeff - margin_){
                            newX = (backgroundRec.width  - width_) / canvas.scaleCoeff - margin_
                        }
                        if(newY > (backgroundRec.height  - height_) / canvas.scaleCoeff - margin_){
                            newY = (backgroundRec.height  - height_) / canvas.scaleCoeff - margin_
                        }

                        canvasPage.objectModel.SetData("Y", newY, canvas.selectedIndex);
                        canvasPage.objectModel.SetData("X", newX, canvas.selectedIndex);

                        canvas.requestPaint();

                    }
                }

            }

            onPositionSignal: {
                positionChangedPause.position = position;
                positionChangedPause.restart();
            }
            PauseAnimation {
                id: positionChangedPause;

                duration: 100;
                property point position;
                onFinished: {
                    controlArea.hoverReaction(position);
                }
            }

            function hoverReaction(position){
                //console.log(position.x, position.y)
                canvas.hoverIndex = -1;
                for(let i = 0; i < canvasPage.objectModel.GetItemsCount(); i++){
                    let x_  = canvasPage.objectModel.GetData("X", i) ;
                    let y_  = canvasPage.objectModel.GetData("Y", i);
                    let width_ = canvasPage.objectModel.IsValidData("Width", i) ? canvasPage.objectModel.GetData("Width", i)  : canvas.mainRec_width;
                    let height_ = canvas.mainRec_height ;

                    x_ = x_ * canvas.scaleCoeff + canvas.deltaX;
                    y_ = y_ * canvas.scaleCoeff + canvas.deltaY;
                    width_ = width_ * canvas.scaleCoeff;
                    height_ = height_  * canvas.scaleCoeff;

                    let ok = controlArea.checkHoverItem(x_, y_, width_, height_, position);

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
            //hover reaction

        }

        Rectangle{
            id: backgroundRec;

            x: canvas.deltaX;
            y: canvas.deltaY;

            width: canvas.backgroundWidth * canvas.scaleCoeff;
            height: canvas.backgroundHeight * canvas.scaleCoeff;

        }

        Canvas {

            id: canvas

            anchors.fill: parent;

            antialiasing: true;

            property real scaleCoeff: 1.0;
            property real scaleCoeffPrev: 1.0;
            property real deltaX: 0.0;
            property real deltaY: 0.0;

            property int selectedIndex: -1;
            property int hoverIndex: -1;
            property bool linkSelected: false;
            property bool hasTailSelection: false;

            //sizes
            property real mainRec_width: 250 //* scaleCoeff;
            property real mainRec_height: 60 //* scaleCoeff;

            property int fontSize: 20 //* scaleCoeff;
            property int fontSizeSmall: 14 //* scaleCoeff;
            property int radius_: 2 //* scaleCoeff;
            property int borderShift: 4 //* scaleCoeff;
            property int shadowSize: 6 //* scaleCoeff;

            property int backgroundStep: 30 //* scaleCoeff;
            property int backgroundWidth: 1024 //* scaleCoeff;
            property int backgroundHeight: 768 //* scaleCoeff;

            property int intersectionSize: 16 //* scaleCoeff;
            property int arcRadius: 8 //* scaleCoeff;
            property int textMargin: 8 //* canvas.scaleCoeff;
            property int textVerticalOffset: 22 //* canvas.scaleCoeff;
            property real imageSize: 20 //* scaleCoeff;
            property real imageMargin: 4 //* scaleCoeff;

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
                //console.log("SCALE_COEFF:: ", scaleCoeff)
                requestPaint()
            }


            onPaint: {
                var ctx = canvas.getContext('2d');
                ctx.reset()
                ctx.clearRect(0, 0, canvas.width, canvas.height)

                let transtateX = canvas.deltaX
                let transtateY = canvas.deltaY
                ctx.translate(transtateX, transtateY)
                ctx.scale(canvas.scaleCoeff, canvas.scaleCoeff)

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

                //console.log("Count object", canvasPage.objectModel.GetItemsCount())

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

                for(let i = 0; i * step < canvas.backgroundWidth; i++){//vertical lines
                    let x1 = i * step;
                    let y1 =  0 ;
                    let x2 = i * step;
                    let y2 = canvas.backgroundHeight ;

                    ctx.beginPath()
                    ctx.moveTo(x1, y1);
                    ctx.lineTo(x2, y2);
                    ctx.stroke();
                }

                for(let i = 0; i * step < canvas.backgroundHeight; i++){//horizontal lines
                    let x1 = 0 ;
                    let y1 =  i * step ;
                    let x2 =  canvas.backgroundWidth ;
                    let y2 =  i * step;

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
                let width_ = canvasPage.objectModel.IsValidData("Width", index) ? canvasPage.objectModel.GetData("Width", index)  : canvas.mainRec_width;

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

                let add = 2 * canvas.imageSize + 2 * canvas.imageMargin + 2 * canvas.borderShift + 30 /** scaleCoeff*/;
                let mainRecWidth = Math.max(textWidth_main + add, textWidth_second + add, width_)
                canvasPage.objectModel.SetData("Width", mainRecWidth /*/ scaleCoeff*/, index);

                return mainRecWidth;
            }

            function drawObject(ctx, index){

                let x_  = canvasPage.objectModel.GetData("X", index)
                let y_  = canvasPage.objectModel.GetData("Y", index)
                let width_ = canvasPage.objectModel.IsValidData("Width", index) ? canvasPage.objectModel.GetData("Width", index)  : canvas.mainRec_width;
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
                ctx.lineWidth = 2 ;
                ctx.fillStyle = selected ? Qt.rgba(0.2, 0.8, 0, 0.3) : Qt.rgba(0, 0, 0, 0.2);
                ctx.beginPath()
                ctx.roundedRect(x_ + shadowSize, y_ + shadowSize, mainRecWidth, canvas.mainRec_height , canvas.radius_, canvas.radius_);
                ctx.fill();

                //main rectangle
                ctx.lineWidth = 2 ;
                ctx.strokeStyle = hasError ? canvas.errorColor : canvas.mainColor;
                ctx.fillStyle = (!selected && !isComposite) ? canvas.backgroundColor :
                                                              (!selected && isComposite) ? canvas.compositeColor:
                                                                                           (selected && isComposite) ? canvas.compositeSelectedColor: (selected && !isComposite) ? canvas.selectedColor : canvas.backgroundColor;
                ctx.beginPath()
                ctx.roundedRect(x_, y_, mainRecWidth, canvas.mainRec_height , canvas.radius_, canvas.radius_);
                ctx.fill();
                ctx.stroke();

                //inner rectangle
                ctx.strokeStyle = canvas.innerFrameColor;

                ctx.beginPath()
                ctx.roundedRect(x_ + canvas.borderShift, y_ + canvas.borderShift, mainRecWidth - 2 * canvas.borderShift, canvas.mainRec_height  - 2 * canvas.borderShift, canvas.radius_, canvas.radius_);
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
                // ctx.drawImage(imageUrl_2, image2_x, image2_y, canvas.imageSize, canvas.imageSize);

            }

            function drawLink(ctx, fromIndex, toIndex){

                let x1 = canvasPage.objectModel.GetData("X", fromIndex);
                let y1 = canvasPage.objectModel.GetData("Y", fromIndex);
                let x2 = canvasPage.objectModel.GetData("X", toIndex);
                let y2 = canvasPage.objectModel.GetData("Y", toIndex);
                let width1 = canvasPage.objectModel.IsValidData("Width", fromIndex) ? canvasPage.objectModel.GetData("Width", fromIndex) : canvas.mainRec_width;
                let width2 = canvasPage.objectModel.IsValidData("Width", toIndex) ? canvasPage.objectModel.GetData("Width", toIndex) : canvas.mainRec_width;

                let x1_link = x1 + width1/2;
                let y1_link = y1 + canvas.mainRec_height/2;
                let x2_link = x2 + width2/2;
                let y2_link = y2 + canvas.mainRec_height/2;

                ctx.lineCap = "round"
                ctx.lineJoin = "round"
                ctx.lineWidth = 1;

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
                    x1_rec2 = x2;
                    y1_rec2 = y2 + canvas.mainRec_height;
                    x2_rec2 = x2 + width2;
                    y2_rec2 = y2 + canvas.mainRec_height;

                    intersection = findIntersection(x1_link, y1_link, x2_link, y2_link, x1_rec2, y1_rec2, x2_rec2, y2_rec2);

                    if(intersection.x < x2 + width2 && intersection.x > x2){
                        //bottom line
                        x1_rec2 = x2;
                        y1_rec2 = y2 + canvas.mainRec_height;
                        x2_rec2 = x2 + width2;
                        y2_rec2 = y2 + canvas.mainRec_height;
                    }
                    else if(intersection.x >= x2 + width2){
                        //right line
                        x1_rec2 = x2  + width2;
                        y1_rec2 = y2 + canvas.mainRec_height;
                        x2_rec2 = x2 + width2;
                        y2_rec2 = y2;
                    }
                    else if(intersection.x <= x2){
                        //left line
                        x1_rec2 = x2;
                        y1_rec2 = y2 + canvas.mainRec_height;
                        x2_rec2 = x2;
                        y2_rec2 = y2;
                    }
                }//y1 > y2

                else if (y1 < y2){
                    //top line
                    x1_rec2 = x2;
                    y1_rec2 = y2;
                    x2_rec2 = x2 + width2;
                    y2_rec2 = y2;

                    intersection = findIntersection(x1_link, y1_link, x2_link, y2_link, x1_rec2, y1_rec2, x2_rec2, y2_rec2);

                    if(intersection.x < x2 + width2 && intersection.x > x2){
                        //top line
                        x1_rec2 = x2;
                        y1_rec2 = y2;
                        x2_rec2 = x2 + width2;
                        y2_rec2 = y2;
                    }
                    else if(intersection.x >= x2 + width2){
                        //right line
                        x1_rec2 = x2  + width2;
                        y1_rec2 = y2 + canvas.mainRec_height;
                        x2_rec2 = x2 + width2;
                        y2_rec2 = y2;
                    }
                    else if(intersection.x <= x2){
                        //left line
                        x1_rec2 = x2;
                        y1_rec2 = y2 + canvas.mainRec_height;
                        x2_rec2 = x2;
                        y2_rec2 = y2;
                    }

                }//y1 < y2

                else if(y1 == y2){
                    if(x1 >= x2){
                        //right line
                        x1_rec2 = x2  + width2;
                        y1_rec2 = y2 + canvas.mainRec_height;
                        x2_rec2 = x2 + width2;
                        y2_rec2 = y2;


                    }
                    else {
                        //left line
                        x1_rec2 = x2;
                        y1_rec2 = y2 + canvas.mainRec_height;
                        x2_rec2 = x2;
                        y2_rec2 = y2;

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

                    offset = 20 * (1 + add);

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

                ctx.lineWidth = 0.5;
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
                ctx.lineWidth = 0.5;
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
                ctx.lineWidth = 0.5;
                let offset = size/2;
                let newX = intersection.x + offset * Math.cos(angle)
                let newY = intersection.y + offset * Math.sin(angle)
                ctx.lineTo(newX, newY);
                ctx.stroke();


            }

            function drawIntersectionArc(ctx, point, angle, offset, selected){
                let rad = canvas.arcRadius;
                let endAngle = 0.6 * Math.PI;
                ctx.lineWidth = 2;
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
                ctx.lineWidth = 0.6;
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
                let x_ = bufferModel.GetData("X") + 10;
                let y_ = bufferModel.GetData("Y") + 10;

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
        backgroundColor: "gray";
        indicatorColor: "lightgray"
        Component.onCompleted: {
            opacity = 1;
        }
        targetItem: parent;
        onContentYSignal: {
            canvas.requestPaint();
        }

    }

    CustomScrollbar{
        id: scrollHoriz;

        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 1;

        secondSize: 12;
        backgroundColor: "gray";
        indicatorColor: "lightgray"

        Component.onCompleted: {
            opacity = 1;
        }
        vertical: false;
        targetItem: parent;
        onContentXSignal: {
            canvas.requestPaint();
        }
    }

}
