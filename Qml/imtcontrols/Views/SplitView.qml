import QtQuick 2.12
import Acf 1.0;
import imtcontrols 1.0


Item{
    id: splitView;

    property int orientation: Qt.Horizontal;

    property int _childrenCount: 0;
    property int _splitterCount: !_childrenCount  ? 0 : _childrenCount -1;

    property bool compl: false;
    property bool ready: compl && height > 0 && width > 0;

    property bool hasAnimation: false;
    property int animationDuration: 70;

    property real parentWidth: parent ? parent.width : 0;
    property real parentHeight: parent ? parent.height : 0;

    property real prevWidth: 0;
    property real prevHeight: 0;

    property TreeItemModel sizeModel: TreeItemModel{};

    clip: true;

    Component.onCompleted: {
        compl = true;
    }

    onReadyChanged: {
        if(ready){
            console.log("children.length:: ", children.length)
            let count = children.length;
            splitView._childrenCount = count;

            if(splitView.orientation == Qt.Horizontal){
                for(let i = 0; i < count ; i++){
                    splitView.children[i].height = splitView.height;
                }

                for(let i = 0; i < count -1; i++){
                  let obj =  splitterComp.createObject(splitView)
                  obj.height = splitView.height;
                }

                let newCount = children.length;
                let list = []

                for(let i = 0; i < count; i++){
                    let obj_ = children[i];
                    list.push(obj_)
                    if(i < count - 1){
                        let splitObj =  children[newCount - 1 - i];
                        splitObj.rowIndex = 2 * i + 1;
                        list.push(splitObj)
                    }

                }
                children = [];
                row.createObject(splitView);
                children[0].children = list;
            }//horizontal

            else if(splitView.orientation == Qt.Vertical){
                for(let i = 0; i < count ; i++){
                    splitView.children[i].width = splitView.width;
                }
                for(let i = 0; i < count -1; i++){
                   let obj = splitterCompForVert.createObject(splitView);
                   obj.width = splitView.width;
                }

                let newCount = children.length;
                let list = []

                for(let i = 0; i < count; i++){
                    let obj_ = children[i];
                    list.push(obj_)
                    if(i < count - 1){
                        let splitObj =  children[newCount - 1 - i];
                        splitObj.rowIndex = 2 * i + 1;
                        list.push(splitObj)
                    }
                }

                children = [];
                column.createObject(splitView);
                children[0].children = list;
            }

            fillSizeModelDefault(count);
            correctSize();
            sizeModel.modelChanged.connect(onModelDataChanged)

            prevWidth = width;
            prevHeight = height;

        }//if(ready)

    }

    onWidthChanged: {
        if(!ready){
            return;
        }
        if(!splitView.children[0]){
            return;
        }
        if(splitView.orientation == Qt.Vertical){
            splitView.children[0].width = splitView.width;
            let count = splitView.children[0].children.length;
            for(let i = 0; i < count ; i++){
                splitView.children[0].children[i].width = splitView.width;
            }
        }
        else if(splitView.orientation == Qt.Horizontal){
            //correct splitter position
            if(prevWidth){
                let coeff = width / prevWidth;
                let count = splitView.children[0].children.length;
                for(let i = 0; i < count ; i++){
                    if(!(i % 2)){
                        splitView.children[0].children[i].width = coeff * splitView.children[0].children[i].width;
                    }
                }
            }
        }

        splitView.prevWidth = width;
    }

    onHeightChanged: {
        if(!ready){
            return;
        }
        if(!splitView.children[0]){
            return;
        }
        if(splitView.orientation == Qt.Horizontal ){
            splitView.children[0].height = splitView.height
            let count = splitView.children[0].children.length;
            for(let i = 0; i < count ; i++){
                splitView.children[0].children[i].height = splitView.height;
            }
        }
        else if(splitView.orientation == Qt.Vertical){
            //correct splitter position
            if(prevHeight > 0){

                let coeff = height / prevHeight;
                let count = splitView.children[0].children.length;
                console.log("count", count)

                for(let i = 0; i < count ; i++){
                    if(!(i % 2)){
                        splitView.children[0].children[i].height =
                                coeff * splitView.children[0].children[i].height;
                        console.log(splitView.children[0].children[i].height, i, coeff)
                    }
                }
            }

        }

        splitView.prevHeight = height;
    }


    function fillSizeModelDefault(count_){
        for(let i = 0; i < count_; i++){
            let index = sizeModel.insertNewItem();
            sizeModel.setData("MinimumWidth", 0, index);
            sizeModel.setData("MinimumHeight", 0, index);
            sizeModel.setData("MaximumWidth", 1000000, index);
            sizeModel.setData("MaximumHeight", 1000000, index);

            let width_ = splitView.children[0].children[2*i].width;
            let height_ = splitView.children[0].children[2*i].height;

            sizeModel.setData("PreferredWidth", width_, index);
            sizeModel.setData("PreferredHeight", height_, index);

            let fillWidth = i < (count_ - 1) ? false : true;
            let fillHeight = i < (count_ - 1) ? false : true;
            sizeModel.setData("FillWidth", fillWidth, index);
            sizeModel.setData("FillHeight", fillHeight, index);

        }
    }

    function chechModel(){
        let hasFillWidth = false;
        let hasFillHeight = false;
        for(let i = 0; i < sizeModel.getItemsCount(); i++){
            if(hasFillWidth){
                sizeModel.setData("FillWidth", false, i);
            }
            if(hasFillHeight){
                sizeModel.setData("FillHeight", false, i);
            }
            let fillWidth = sizeModel.getData("FillWidth", i);
            let fillHeight = sizeModel.getData("FillHeight", i);
            if(fillWidth){
                hasFillWidth = true;
            }
            if(fillHeight){
                hasFillHeight = true;
            }
        }
    }

    function correctSize(){

        setElementsSizeFromModel();

        if(splitView.orientation == Qt.Horizontal){
            let widthRest = 0;
            let widthSplitters = 0;
            let indexFillWidth = - 1;
            let totalValue = 0;

            for(let i = 0; i < (splitView._childrenCount + splitView._splitterCount); i++){
                totalValue += splitView.children[0].children[i].width;
                if(i % 2){
                    widthSplitters += splitView.children[0].children[i].width;
                }
            }

            if(totalValue <= splitView.width){
                widthRest = widthSplitters;
                for(let i = 0; i < sizeModel.getItemsCount(); i++){
                    let ok = sizeModel.getData("FillWidth", i) && indexFillWidth < 0;
                    if(ok){
                        indexFillWidth = 2*i;
                    }
                    else {
                        widthRest += sizeModel.getData("PreferredWidth",i);
                    }
                }
                if(indexFillWidth >=0 && splitView.width - widthRest > 0){
                    splitView.children[0].children[indexFillWidth].width = splitView.width - widthRest;
                }

            }//totalValue <= splitView.width
            else {//totalValue > splitView.width
                let coeff = totalValue / splitView.width;

                let correction = Math.ceil((widthSplitters - widthSplitters / coeff) / splitView._childrenCount)
                for(let i = 0; i < (splitView._childrenCount + splitView._splitterCount); i++){
                    if(!(i % 2)){
                        splitView.children[0].children[i].width =
                                splitView.children[0].children[i].width / coeff - correction;
                    }
                }
            }

        }//Horizontal

        else if(splitView.orientation == Qt.Vertical){
            let heightRest = 0;
            let heightSplitters = 0;
            let indexFillHeight = - 1;
            let totalValue = 0;

            for(let i = 0; i < (splitView._childrenCount + splitView._splitterCount); i++){
                totalValue += splitView.children[0].children[i].height;
                if(i % 2){
                    heightSplitters += splitView.children[0].children[i].height;
                }
            }

            if(totalValue <= splitView.height){
                heightRest = heightSplitters;
                for(let i = 0; i < sizeModel.getItemsCount(); i++){
                    let ok =  sizeModel.getData("FillHeight", i) && indexFillHeight < 0;
                    if(ok){
                        indexFillHeight = 2*i;
                    }
                    else {
                        heightRest += sizeModel.getData("PreferredHeight",i);
                    }
                }
                if(indexFillHeight >=0 && splitView.height - heightRest > 0){
                    splitView.children[0].children[indexFillHeight].height = splitView.height - heightRest;
                }
            }//totalValue <= splitView.height

            else {//totalValue >splitView.height
                let coeff = totalValue / splitView.height;

                let correction = Math.ceil((heightSplitters - heightSplitters / coeff) / splitView._childrenCount)
                for(let i = 0; i < (splitView._childrenCount + splitView._splitterCount); i++){
                    if(!(i % 2)){
                        splitView.children[0].children[i].height =
                                splitView.children[0].children[i].height / coeff - correction;
                    }
                }
            }

        }

    }

    function setElementsSizeFromModel(){
        for(let i = 0; i < sizeModel.getItemsCount(); i++){
            let width_ = sizeModel.getData("PreferredWidth", i);
            let height_ = sizeModel.getData("PreferredHeight", i);
            if(splitView.orientation == Qt.Horizontal){
                splitView.children[0].children[2*i].width = width_;
            }
            else if(splitView.orientation == Qt.Vertical){
                splitView.children[0].children[2*i].height = height_;
            }

        }
    }

    function onModelDataChanged(topLeft, bottomRight, keyRoles){
        if(ready){
            correctSize();
        }
    }

    Component{
        id: column;
        Column{
            width: splitView.width;
            clip: true;
        }
    }

    Component{
        id: row;
        Row{
            height:  splitView.width;
            clip: true;
        }
    }


    Component{
        id: splitterComp;

        Rectangle{
            id: splitter;

            width: 6;
            height: parent.height;
            color: Style.color_scrollBackground;
            property int rowIndex: -1;
            function correctSizes(delta_){
                //console.log("delta_ :: ", delta_, rowIndex)
                if (!splitView.children[0]){
                    return
                }

                let prevModelIndex = (rowIndex - 1)/2;
                let nextModelIndex = prevModelIndex + 1;
                let childrenPrev = splitView.children[0].children[splitter.rowIndex - 1]
                let childrenNext = splitView.children[0].children[splitter.rowIndex + 1]

                if(childrenPrev.width + delta_ > splitView.sizeModel.getData("MinimumWidth", prevModelIndex) &&
                        childrenPrev.width + delta_ <  splitView.sizeModel.getData("MaximumWidth", prevModelIndex) &&
                        childrenNext.width - delta_ > splitView.sizeModel.getData("MinimumWidth", nextModelIndex) &&
                        childrenNext.width - delta_ <  splitView.sizeModel.getData("MaximumWidth", nextModelIndex))
                {
                    if(splitView.hasAnimation){
                        let width1 = childrenPrev.width;
                        let width2 = childrenNext.width;
                        anim1.from = width1; anim1.to = width1 + delta_;
                        anim2.from = width2; anim2.to = width2 - delta_;
                        anim1.target = childrenPrev;
                        anim2.target = childrenNext;
                        anim1.start(); anim2.start();
                    }
                    else {
                        childrenPrev.width += delta_;
                        childrenNext.width -= delta_;
                    }
                }
            }

            Rectangle{
                anchors.centerIn: parent;

                width: 2;
                height: 10;//20;
                radius: width;

                color: "gray";
                opacity: 0.5;
            }

            MouseArea {
                id: splitterMA;

                anchors.centerIn: parent;

                width: parent.width * (1 + isPressed * 9);
                height: parent.height;

                hoverEnabled: true;
                acceptedButtons: Qt.LeftButton;
                propagateComposedEvents: true;
                //cursorShape: splitView.orientation == Qt.Horizontal ? Qt.SplitHCursor : Qt.SplitVCursor;
                cursorShape: pressed && containsMouse ? Qt.SplitHCursor : !pressed && withinParent ? Qt.SplitHCursor : Qt.ArrowCursor;
                property bool withinParent:  mouseX > (width - parent.width)/2 && mouseX <  (width + parent.width)/2;
                property int startX;
                property bool isPressed;

                onPressed: {
                    startX = mouse.x;
                    isPressed = true;
                }


                onClicked: {
                    mouse.accepted = true;
                    isPressed = false;
                }


                onPositionChanged:  {

                    if (splitterMA.pressed){
                        let x_ = mouse.x;
                        let delta = x_ - startX;
                        splitter.correctSizes(delta);

                        movingPause.restart();
                    }
                }



                PauseAnimation {
                    id: movingPause
                    duration: 2000;
                    onFinished: {
                        splitterMA.isPressed = false;
                    }
                }
            }
        }
    }

    //
    Component{
        id: splitterCompForVert;

        Rectangle{
            id: splitterForVert;

            width: parent.width ;
            height: 6;
            color: Style.color_scrollBackground;
            property int rowIndex: -1;
            function correctSizes(delta_){
                //console.log("delta_ :: ", delta_, rowIndex)
                if (!splitView.children[0]){
                    return
                }

                let prevModelIndex = (rowIndex - 1)/2;
                let nextModelIndex = prevModelIndex + 1;
                let childrenPrev = splitView.children[0].children[splitterForVert.rowIndex - 1]
                let childrenNext = splitView.children[0].children[splitterForVert.rowIndex + 1]


                if(childrenPrev.height + delta_ > splitView.sizeModel.getData("MinimumHeight", prevModelIndex) &&
                        childrenPrev.height + delta_ <  splitView.sizeModel.getData("MaximumHeight", prevModelIndex) &&
                        childrenNext.height - delta_ > splitView.sizeModel.getData("MinimumHeight", nextModelIndex) &&
                        childrenNext.height - delta_ <  splitView.sizeModel.getData("MaximumHeight", nextModelIndex))
                {

                    if(splitView.hasAnimation){
                        let height1 = childrenPrev.height;
                        let height2 = childrenNext.height;
                        anim1.from = height1; anim1.to = height1 + delta_;
                        anim2.from = height2; anim2.to = height2 - delta_;
                        anim1.target = childrenPrev;
                        anim2.target = childrenNext;
                        anim1.start(); anim2.start();

                    }
                    else {
                        childrenPrev.height += delta_;
                        childrenNext.height -= delta_;
                    }

                }

            }

            Rectangle{
                anchors.centerIn: parent;

                width: 10;//20;
                height: 2;
                radius: height;

                color: "gray";
                opacity: 0.5;
            }

            MouseArea {
                id: splitterMAForVert;

                anchors.centerIn: parent;

                width: parent.width;
                height: parent.height * 10;

                hoverEnabled: true;
                acceptedButtons: Qt.LeftButton;
                propagateComposedEvents: true;
                //cursorShape: splitView.orientation == Qt.Horizontal ? Qt.SplitHCursor : Qt.SplitVCursor;
                cursorShape: pressed && containsMouse ? Qt.SplitVCursor : !pressed && withinParent ? Qt.SplitVCursor : Qt.ArrowCursor;
                property bool withinParent:  mouseY > (height - parent.height)/2 && mouseY <  (height + parent.height)/2;
                property int startY;

                onPressed: {
                    startY = mouse.y;
                }

                onClicked: {
                    mouse.accepted = false;
                }

                onPositionChanged:  {

                    if (splitterMAForVert.pressed){
                        let y_ = mouse.y;
                        let delta = y_ - startY;
                        splitterForVert.correctSizes(delta);
                    }
                }
            }
        }
    }


    NumberAnimation {
        id: anim1;

        property: splitView.orientation == Qt.Horizontal ? "width" : "height";
        duration: splitView.animationDuration;
    }

    NumberAnimation {
        id: anim2

        property: splitView.orientation == Qt.Horizontal ? "width" : "height";
        duration: splitView.animationDuration;
    }

}

