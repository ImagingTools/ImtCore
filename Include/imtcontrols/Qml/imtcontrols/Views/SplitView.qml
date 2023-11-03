import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0
import imtcontrols 1.0


Row{
    id: splitView;

    property int orientation: Qt.Horizontal;

    property int _childrenCount: 0;
    property int _splitterCount: !_childrenCount  ? 0 : _childrenCount -1;

    property bool compl: false;

    property TreeItemModel sizeModel: TreeItemModel{};


    Component.onCompleted: {
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
            children = list;
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
        splitView.compl = true;
        sizeModel.dataChanged.connect(onModelDataChanged)
    }

    function fillSizeModelDefault(count_){
        for(let i = 0; i < count_; i++){
            let index = sizeModel.InsertNewItem();
            sizeModel.SetData("MinimumWidth", 0, index);
            sizeModel.SetData("MinimumHeight", 0, index);
            sizeModel.SetData("MaximumWidth", 1000000, index);
            sizeModel.SetData("MaximumHeight", 1000000, index);

            let width_ = splitView.orientation == Qt.Horizontal ? splitView.children[2*i].width:
                                                                  splitView.children[0].children[2*i].width;
            let height_ = splitView.orientation == Qt.Horizontal ? splitView.children[2*i].height :
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
                    widthRest += splitView.children[i].width;
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
            if(indexFillWidth >=0){
                splitView.children[indexFillWidth].width = splitView.width - widthRest;
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
            if(indexFillHeight >=0){
                splitView.children[0].children[indexFillHeight].height = splitView.height - heightRest;
            }

        }

    }

    function setElementsSizeFromModel(){
        for(let i = 0; i < sizeModel.GetItemsCount(); i++){
            let width_ = sizeModel.GetData("PreferredWidth", i);
            let height_ = sizeModel.GetData("PreferredHeight", i);
            if(splitView.orientation == Qt.Horizontal){
                splitView.children[2*i].width = width_;
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
        }
    }


    Component{
        id: splitterComp;

        Rectangle{
            id: splitter;

            width: 6;
            height: parent.height;
            color: "#ffffff";
            property int rowIndex: -1;
            function correctSizes(delta_){
                //console.log("delta_ :: ", delta_, rowIndex)

                let prevModelIndex = (rowIndex - 1)/2;
                let nextModelIndex = prevModelIndex + 1;

                if(splitView.children[rowIndex - 1].width + delta_ > splitView.sizeModel.GetData("MinimumWidth", prevModelIndex) &&
                    splitView.children[rowIndex - 1].width + delta_ <  splitView.sizeModel.GetData("MaximumWidth", prevModelIndex) &&
                        splitView.children[rowIndex + 1].width - delta_ > splitView.sizeModel.GetData("MinimumWidth", nextModelIndex) &&
                        splitView.children[rowIndex + 1].width - delta_ <  splitView.sizeModel.GetData("MaximumWidth", nextModelIndex))
                {
                    splitView.children[rowIndex - 1].width += delta_;
                    splitView.children[rowIndex + 1].width -= delta_;
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

            width: parent.width;
            height: 6;
            color: "#ffffff";
            property int rowIndex: -1;
            function correctSizes(delta_){
                //console.log("delta_ :: ", delta_, rowIndex)

                let prevModelIndex = (rowIndex - 1)/2;
                let nextModelIndex = prevModelIndex + 1;

                if(splitView.children[0].children[rowIndex - 1].height + delta_ > splitView.sizeModel.GetData("MinimumHeight", prevModelIndex) &&
                    splitView.children[0].children[rowIndex - 1].height + delta_ <  splitView.sizeModel.GetData("MaximumHeight", prevModelIndex) &&
                        splitView.children[0].children[rowIndex + 1].height - delta_ > splitView.sizeModel.GetData("MinimumHeight", nextModelIndex) &&
                        splitView.children[0].children[rowIndex + 1].height - delta_ <  splitView.sizeModel.GetData("MaximumHeight", nextModelIndex))
                {
                    splitView.children[0].children[rowIndex - 1].height += delta_;
                    splitView.children[0].children[rowIndex + 1].height -= delta_;
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

}

