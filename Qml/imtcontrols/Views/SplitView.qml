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
                    splitterComp.createObject(splitView)
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
                    splitterCompForVert.createObject(splitView);
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
            sizeModel.dataChanged.connect(onModelDataChanged)

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
            let index = sizeModel.InsertNewItem();
            sizeModel.SetData("MinimumWidth", 0, index);
            sizeModel.SetData("MinimumHeight", 0, index);
            sizeModel.SetData("MaximumWidth", 1000000, index);
            sizeModel.SetData("MaximumHeight", 1000000, index);

            let width_ = splitView.orientation == Qt.Horizontal ? splitView.children[0].children[2*i].width:
                                                                  splitView.children[0].children[2*i].width;
            let height_ = splitView.orientation == Qt.Horizontal ? splitView.children[0].children[2*i].height :
                                                                   splitView.children[0].children[2*i].height;

            sizeModel.SetData("PreferredWidth", width_, index);
            sizeModel.SetData("PreferredHeight", height_, index);

            let fillWidth = i < (count_ - 1) ? false : true;
            let fillHeight = i < (count_ - 1) ? false : true;
            sizeModel.SetData("FillWidth", fillWidth, index);
            sizeModel.SetData("FillHeight", fillHeight, index);

        }
    }

    function chechModel(){
        let hasFillWidth = false;
        let hasFillHeight = false;
        for(let i = 0; i < sizeModel.GetItemsCount(); i++){
            if(hasFillWidth){
                sizeModel.SetData("FillWidth", false, i);
            }
            if(hasFillHeight){
                sizeModel.SetData("FillHeight", false, i);
            }
            let fillWidth = sizeModel.GetData("FillWidth", i);
            let fillHeight = sizeModel.GetData("FillHeight", i);
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
            let indexFillWidth = - 1;

            for(let i = 0; i < (splitView._childrenCount + splitView._splitterCount); i++){
                if(i % 2){
                    widthRest += splitView.children[0].children[i].width;
                }
            }

            for(let i = 0; i < sizeModel.GetItemsCount(); i++){
                let ok = sizeModel.GetData("FillWidth", i) && indexFillWidth < 0;
                if(ok){
                    indexFillWidth = 2*i;
                }
                else {
                    widthRest += sizeModel.GetData("PreferredWidth",i);
                }
            }
            if(indexFillWidth >=0 && splitView.width - widthRest > 0){
                splitView.children[0].children[indexFillWidth].width = splitView.width - widthRest;
            }
        }

        else if(splitView.orientation == Qt.Vertical){
            let heightRest = 0;
            let indexFillHeight = - 1;

            for(let i = 0; i < (splitView._childrenCount + splitView._splitterCount); i++){
                if(i % 2){
                    heightRest += splitView.children[0].children[i].height;
                }
            }

            for(let i = 0; i < sizeModel.GetItemsCount(); i++){
                let ok =  sizeModel.GetData("FillHeight", i) && indexFillHeight < 0;
                if(ok){
                    indexFillHeight = 2*i;
                }
                else {
                    heightRest += sizeModel.GetData("PreferredHeight",i);
                }
            }
            if(indexFillHeight >=0 && splitView.height - heightRest > 0){
                splitView.children[0].children[indexFillHeight].height = splitView.height - heightRest;
            }

        }

    }

    function setElementsSizeFromModel(){
        for(let i = 0; i < sizeModel.GetItemsCount(); i++){
            let width_ = sizeModel.GetData("PreferredWidth", i);
            let height_ = sizeModel.GetData("PreferredHeight", i);
            if(splitView.orientation == Qt.Horizontal){
                splitView.children[0].children[2*i].width = width_;
            }
            else if(splitView.orientation == Qt.Vertical){
                splitView.children[0].children[2*i].height = height_;
            }

        }
    }

    function onModelDataChanged(topLeft, bottomRight, keyRoles){
        if(compl){
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
            height: parent ? parent.height : 0;
            color: "#ffffff";
            z: 100
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

                if(childrenPrev.width + delta_ > splitView.sizeModel.GetData("MinimumWidth", prevModelIndex) &&
                        childrenPrev.width + delta_ <  splitView.sizeModel.GetData("MaximumWidth", prevModelIndex) &&
                        childrenNext.width - delta_ > splitView.sizeModel.GetData("MinimumWidth", nextModelIndex) &&
                        childrenNext.width - delta_ <  splitView.sizeModel.GetData("MaximumWidth", nextModelIndex))
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

                width: parent.width * 10;
                height: parent.height;

                hoverEnabled: true;
                acceptedButtons: Qt.LeftButton;
                cursorShape: splitView.orientation == Qt.Horizontal ? Qt.SplitHCursor : Qt.SplitVCursor;
                property int startX;

                onPressed: {
                    startX = mouse.x;
                }

                onPositionChanged:  {

                    if (splitterMA.pressed){
                        let x_ = mouse.x;
                        let delta = x_ - startX;
                        splitter.correctSizes(delta);
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

            width: parent ? parent.width : 0;
            height: 6;
            color: "#ffffff";
            z: 100
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


                if(childrenPrev.height + delta_ > splitView.sizeModel.GetData("MinimumHeight", prevModelIndex) &&
                        childrenPrev.height + delta_ <  splitView.sizeModel.GetData("MaximumHeight", prevModelIndex) &&
                        childrenNext.height - delta_ > splitView.sizeModel.GetData("MinimumHeight", nextModelIndex) &&
                        childrenNext.height - delta_ <  splitView.sizeModel.GetData("MaximumHeight", nextModelIndex))
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
                cursorShape: splitView.orientation == Qt.Horizontal ? Qt.SplitHCursor : Qt.SplitVCursor;
                property int startY;

                onPressed: {
                    startY = mouse.y;
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

