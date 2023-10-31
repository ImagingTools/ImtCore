import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0
import imtcontrols 1.0


Row{
    id: splitView;

    property int orientation: Qt.Horizontal;

    property int _childrenCount: 0;
    property int _splitterCount: !_childrenCount  ? 0 : _childrenCount -1;

    property TreeItemModel sizeModel: TreeItemModel{};


    Component.onCompleted: {
        console.log("children.length:: ", children.length)
        var count = children.length;
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
                var obj_ = children[i];
                list.push(obj_)
                if(i < count - 1){
                    var splitObj =  children[newCount - 1 - i];
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
        setSize();
    }

    function fillSizeModelDefault(count_){
        for(var i = 0; i < count_; i++){
            var index = sizeModel.InsertNewItem();
            sizeModel.SetData("MinimumWidth", 0, index);
            sizeModel.SetData("MinimumHeight", 0, index);
            sizeModel.SetData("PreferredWidth", 0, index);
            sizeModel.SetData("PreferredHeight", 0, index);
            sizeModel.SetData("MaximumWidth", 1000000, index);
            sizeModel.SetData("MaximumHeight", 1000000, index);

            var fillWidth = i < (count_ - 1) ? false : true;
            var fillHeight = i < (count_ - 1) ? false : true;
            sizeModel.SetData("FillWidth", fillWidth, index);
            sizeModel.SetData("FillHeight", fillHeight, index);

        }
        //console.log("MODEL::" , sizeModel.toJSON())
    }

    function chechModel(){
        var hasFillWidth = false;
        var hasFillHeight = false;
        for(var i = 0; i < sizeModel.GetItemsCount(); i++){
            if(hasFillWidth){
                sizeModel.SetData("FillWidth", false, i);
            }
            if(hasFillHeight){
                sizeModel.SetData("FillHeight", false, i);
            }
            var fillWidth = sizeModel.GetData("FillWidth", i);
            var fillHeight = sizeModel.GetData("FillHeight", i);
            if(fillWidth){
                hasFillWidth = true;
            }
            if(fillHeight){
                hasFillHeight = true;
            }
        }
    }

    function setSize(){
        if(splitView.orientation == Qt.Horizontal){
            var widthRest = 0;
            var indexFillWidth = - 1;

            for(let i = 0; i < (splitView._childrenCount + splitView._splitterCount); i++){
                if(i % 2){
                    widthRest += splitView.children[i].width;
                }
            }

            for(let i = 0; i < sizeModel.GetItemsCount(); i++){
                var ok =  sizeModel.GetData("FillWidth", i);
                if(!ok){
                   widthRest += splitView.children[2*i].width;
                }
                else {
                    if(indexFillWidth < 0){
                        indexFillWidth = 2*i;
                    }
                }
            }
            console.log("NewWidth:: ", splitView.width, splitView.width - widthRest, indexFillWidth)
            splitView.children[indexFillWidth].width = splitView.width - widthRest;
        }
        else if(splitView.orientation == Qt.Vertical){

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

                var prevModelIndex = (rowIndex - 1)/2;
                var nextModelIndex = prevModelIndex + 1;

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
                height: 20;
                radius: width;

                color: "gray";
            }

            MouseArea {
                id: splitterMA;

                anchors.fill: parent;

                hoverEnabled: true;
                acceptedButtons: Qt.LeftButton;
                cursorShape: splitView.orientation == Qt.Horizontal ? Qt.SplitHCursor : Qt.SplitVCursor;
                property int startX;

                onPressed: {
                    startX = mouse.x;
                }

                onPositionChanged:  {

                    if (splitterMA.pressed){
                        var x_ = mouse.x;
                        var delta = x_ - startX;
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

                var prevModelIndex = (rowIndex - 1)/2;
                var nextModelIndex = prevModelIndex + 1;

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

                width: 20;
                height: 2;
                radius: height;

                color: "gray";
            }

            MouseArea {
                id: splitterMAForVert;

                anchors.fill: parent;

                hoverEnabled: true;
                acceptedButtons: Qt.LeftButton;
                cursorShape: splitView.orientation == Qt.Horizontal ? Qt.SplitHCursor : Qt.SplitVCursor;
                property int startY;

                onPressed: {
                    startY = mouse.y;
                }

                onPositionChanged:  {

                    if (splitterMAForVert.pressed){
                        var y_ = mouse.y;
                        var delta = y_ - startY;
                        splitterForVert.correctSizes(delta);
                    }
                }
            }
        }
    }


}



