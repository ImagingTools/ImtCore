import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0


Rectangle{
    id: treeViewGql;

    color: "transparent";
    property TreeItemModel model: TreeItemModel{};

    property int shift: 70;
    property string nameId: "Name";
    property int delegateWidht: 200;
    property int delegateHeight: 40;
    property alias delegate: list.delegate;
    property bool hasSelection: false;
    property int selectedIndex: -1;
    property int scrollSize: 12;

    property string selectionColor: Style.selectedColor !== undefined ? Style.selectedColor : "lightsteelblue";
    property real selectionOpacity: 0.5;

    signal requestSignal(int index);
    signal rightButtonMouseClicked(int mX, int mY);

    signal clicked(int index);
    signal doubleClicked(int index);
    signal selectionChanged();
    signal openBranch(int index)
    signal closeBranch(int index)

    onWidthChanged: {
        list.contentX = list.originX;
    }

    // onClicked: {
    //     if (selectedIndex != index ){
    //         selectedIndex = index
    //         treeViewGql.selectionChanged()
    //     }

    // }

    onDoubleClicked: {
    }

    function getData(key,index){
        return treeViewGql.model.GetData(key, index);
    }

    function getSelectedIndex(){
        return selectedIndex;
    }

    function getIcon(type, isOpen){
        let source = "";
        let imageName = "";
        if(type == "Node"){
            imageName = isOpen ? "Icons/FolderOpened" : "Icons/FolderClosed";
        }
        else if(type == "Doc"){
            imageName = "Icons/New";
        }
        else {
            imageName = "Icons/New";
        }
        source  =  "../../../" + Style.getIconPath(imageName, Icon.State.On, Icon.Mode.Normal);
        return source;
    }

    function checkIsOpen(index){
        return model.GetData("IsOpen__", index);
    }

    Rectangle{
        id: listContainer;

        width: treeViewGql.width -10;
        height: treeViewGql.height -10;
        radius: 4;

        color : treeViewGql.color;

        Item{
            id: listFrame;

            width: list.width;
            height: list.height;

            property real contentY: list.contentY;
            property real originY: 0;//list.originY;
            property real contentWidth: list.contentWidth;
            property real contentHeight: height;

        }

        ListView{
            id: list;

            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.top: parent.top;
            anchors.bottom: parent.bottom;

            boundsBehavior: Flickable.StopAtBounds;
            contentWidth: delegateWidth;
            clip: true;
            property int delegateWidth: treeViewGql.delegateWidht;
            property int delegateWidthFull: list.maxLevel * treeViewGql.shift + list.delegateWidth;
            property int maxLevel: 1;

            onContentYChanged: {
                listFrame.contentY = contentY;
            }

            model: treeViewGql.model;

            delegate: Item{
                id: deleg;

                width: !model.Visible__ ? 0 : Math.max(list.width, list.delegateWidthFull);
                height: model.Visible__ ? treeViewGql.delegateHeight : 0;
                opacity: model.Visible__;
                property bool isOpen: model.IsOpen__ == undefined ? false : model.IsOpen__;

                Rectangle{
                    id: selectionRec;

                    anchors.fill: parent;

                    radius: 2;
                    opacity: treeViewGql.selectionOpacity;
                    color: treeViewGql.selectionColor;
                    visible: !treeViewGql.hasSelection ? false : model.index == treeViewGql.selectedIndex;
                }

                Rectangle{
                    anchors.left: parent.left;
                    anchors.leftMargin: model.Level__ !== undefined ? model.Level__ * treeViewGql.shift : 0;
                    width: list.delegateWidth;
                    height: parent.height;
                    color: "transparent";


                    Button{
                        id: openButton;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: parent.left;
                        anchors.leftMargin: 8;

                        width: 16;
                        height: width;

                        visible: model.HasChildren__ == undefined ? false : model.HasChildren__;
                        enabled: visible;

                        property string imageName: deleg.isOpen ? "Icons/Down" : "Icons/Right";
                        iconSource:  "../../../" + Style.getIconPath(imageName, Icon.State.On, Icon.Mode.Normal);

                        decorator: ButtonDecorator{
                            color: "transparent";
                            border.color: "transparent";
                        }

                        onClicked: {
                            if(model.HasChildren__){
                                if(!deleg.isOpen){
                                    if(!model.HasBranch__){
                                        treeViewGql.model.SetData("HasBranch__", true, model.index);
                                        treeViewGql.requestSignal(model.index, model.Level__)
                                    }
                                    else {
                                        treeViewGql.setVisibleElements(true, model.index)
                                    }
                                    treeViewGql.model.SetData("IsOpen__", true, model.index);
                                    treeViewGql.model.SetData("OpenState__", 1, model.index);

                                    treeViewGql.openBranch(model.index)
                                }
                                else if(deleg.isOpen){
                                    treeViewGql.model.SetData("IsOpen__", false, model.index);
                                    treeViewGql.model.SetData("OpenState__", 0, model.index);
                                    treeViewGql.setVisibleElements(false, model.index)
                                    treeViewGql.closeBranch(model.index)
                                }
                            }

                        }
                    }

                    Image{
                        id: folderImage;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: openButton.right;
                        anchors.leftMargin: 8;

                        visible: model.TypeId__ !== undefined;
                        width: 16;
                        height: width;
                        sourceSize.width: width;
                        sourceSize.height: height;
                        source: model.TypeId__ == undefined ? "" : treeViewGql.getIcon(model.TypeId__, deleg.isOpen);
                    }

                    Text{
                        id: nameText;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: model.TypeId__ == undefined ? folderImage.left : folderImage.right;
                        anchors.leftMargin: model.TypeId__ == undefined ? 0 : 16;
                        //anchors.right: parent.right;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_subtitle !==undefined ? Style.fontSize_subtitle : 18;
                        color: Style.textColor;

                        text: model[treeViewGql.nameId] !== undefined ? model[treeViewGql.nameId] : "";
                    }



                    MouseArea{
                        id: delegateMA;

                        anchors.top: parent.top;
                        anchors.bottom: parent.bottom;
                        anchors.right: parent.right;
                        anchors.left: folderImage.left;

                        acceptedButtons: Qt.LeftButton | Qt.RightButton;

                        hoverEnabled: visible;
                        cursorShape: Qt.PointingHandCursor;
                        onClicked: {
                            if (treeViewGql.selectedIndex !== model.index ){
                                treeViewGql.selectedIndex = model.index
                                treeViewGql.selectionChanged()
                            }
                            treeViewGql.clicked(model.index);

                            if (mouse.button === Qt.RightButton) {
                                console.log("TreeViewGqlDelegate onRightButtonMouseClicked");

                                var point = mapToItem(null, this.mouseX, this.mouseY);
                                treeViewGql.rightButtonMouseClicked(point.x, point.y);
                            }
                        }
                        onDoubleClicked: {
                            treeViewGql.doubleClicked(model.index, model.Level__);
                        }
                    }
                }


            }//delegate


        }//list

        CustomScrollbar{
            id: scrollVert;

            anchors.left: listFrame.right;
            anchors.leftMargin: 1;

            anchors.bottom: listFrame.bottom;

            targetItem: listFrame;

            secondSize: treeViewGql.scrollSize;
            radius: secondSize;
            indicatorRadius: secondSize;
            indicatorMargin: 0;

            onContentYSignal:{
                list.contentY = contentY;
            }

        }

        CustomScrollbar{
            id: scrollHoriz;

            anchors.right: list.right;
            anchors.top: list.bottom;
            anchors.topMargin: 1 ;

            secondSize: treeViewGql.scrollSize;

            vertical: false;
            targetItem: list;
        }
    }//listContainer


    function setContentWidth(){
        let maxWidth = 0;
        let maxLevel_= 0;
        for(let i = 0; i < treeViewGql.model.GetItemsCount(); i++){
            let visible = treeViewGql.model.GetData("Visible__",i);
            let level = !visible ? 0 : treeViewGql.model.GetData("Level__",i);
            let width_ = !visible ? 0 : level * treeViewGql.shift + list.delegateWidth;
            if(level > maxLevel_){
                maxLevel_ = level;
            }
            if(width_ > maxWidth){
                maxWidth = width_;
            }
        }

        list.contentWidth = maxWidth;
        if(list.contentWidth > list.width){
            list.contentX = Math.min((maxLevel_-1) * treeViewGql.shift + list.originX, list.contentWidth - list.width + list.originX);
        }
        else {
            list.contentX = list.originX;
        }
    }

    function setVisibleElements(visible, index){
        //console.log("SET VISIBLE", visible, index);
        let innerId = treeViewGql.model.GetData("InnerId__", index);
        let found = false;
        let foundChangeCount = 0;

        //for closed
        let currClosedLevel = -1;

        for(let i = index + 1; i < treeViewGql.model.GetItemsCount(); i++){
            let branchIds = treeViewGql.model.IsValidData("BranchIds__", i) ? treeViewGql.model.GetData("BranchIds__", i) : "";
            //console.log("branchIds:: ", branchIds)
            let ok = false;
            let arr = branchIds.split(",");
            let arrCounter = 0;

            //*****************for closed*************//
            let openST = treeViewGql.model.GetData("OpenState__", i);
            let level_curr = treeViewGql.model.GetData("Level__", i);

            if(level_curr <= currClosedLevel){
                currClosedLevel = -1;
            }
            if(visible && openST == 0){
                currClosedLevel = level_curr;
            }

            let ok_visible = true;
            if(!visible){
                ok_visible = true;
            }
            else if(currClosedLevel == -1){
                ok_visible = true;
            }
            else if(level_curr > currClosedLevel && currClosedLevel >= 0){
                ok_visible = false;
            }
            //*****************for closed*************//

            for(let k = 0; k < arr.length; k++){
                if(arr[k] == innerId){
                    ok = true;
                    if(!found){
                        found = true;
                        foundChangeCount = 1;
                    }
                    break;
                }
                arrCounter++;
            }
            if(arrCounter == arr.length && found){
                foundChangeCount = 2;
            }

            if(foundChangeCount == 2){
                //console.log("StopINdex::", i);
                break;
            }

            //
            if(ok){
                treeViewGql.model.SetData("Visible__", visible && ok_visible, i);
                let coeff = visible ? 1 : -1;
                listFrame.contentHeight += coeff * treeViewGql.delegateHeight;
            }
        }

        treeViewGql.setContentWidth();

    }

    function findIndexById(id){
        let foundIndex = -1;
        for(let i = 0; i < treeViewGql.model.GetItemsCount(); i++){
            let id_curr = treeViewGql.model.IsValidData("Id", i) ? treeViewGql.model.GetData("Id", i) : "";
            if(id_curr == id){
                foundIndex = i;
                break;
            }
        }
        return foundIndex;
    }

    function findParentIndex(index){
        let foundIndex = -1;
        let branchIds = treeViewGql.model.IsValidData("BranchIds__", index) ? treeViewGql.model.GetData("BranchIds__", index) : "";
        if(branchIds == ""){
            //console.log("FOUND_INDEX_ RETURN ", foundIndex);
            return -1;
        }
        let arr = branchIds.split(",");
        let parentId = arr[arr.length - 1];
        for(let i = index - 1; i >=0; i--){
            let innerId = treeViewGql.model.GetData("InnerId__", i);
            if(innerId == parentId){
                foundIndex =  i;
                break;
            }
        }
        //console.log("FOUND_INDEX ", foundIndex);
        return foundIndex;
    }

    function deleteBranch(index){
        //console.log("DELETE BRANCH", index);
        if (index < 0){
            treeViewGql.model.Clear()

            return
        }

        let innerId = treeViewGql.model.GetData("InnerId__", index);
        let found = false;
        let foundChangeCount = 0;
        for(let i = index + 1; i < treeViewGql.model.GetItemsCount(); i++){
            let branchIds = treeViewGql.model.IsValidData("BranchIds__", i) ? treeViewGql.model.GetData("BranchIds__", i) : "";
            //console.log("branchIds:: ", branchIds)
            let ok = false;
            let arr = branchIds.split(",");
            let arrCounter = 0;
            for(let k = 0; k < arr.length; k++){
                if(arr[k] == innerId){
                    ok = true;
                    if(!found){
                        found = true;
                        foundChangeCount = 1;
                    }
                    break;
                }
                arrCounter++;
            }
            if(arrCounter == arr.length && found){
                foundChangeCount = 2;
            }

            if(foundChangeCount == 2){
                //console.log("StopINdex::", i);
                break;
            }
            //
            if(ok){
                treeViewGql.model.RemoveItem(i);
                listFrame.contentHeight -= treeViewGql.delegateHeight;
                treeViewGql.deleteBranch(index);
            }
        }

        treeViewGql.model.SetData("IsOpen__", false, index);
        treeViewGql.model.SetData("HasBranch__", false, index);

        treeViewGql.setContentWidth();
    }

    function insertTree(index, model_){

        if(!model_ || !model_.GetItemsCount()){
            return;
        }

        let level_ = treeViewGql.model.IsValidData("Level__", index) ? treeViewGql.model.GetData("Level__", index) : -1;
        console.log("INSERT TREE", index, level_);

        if((level_ + 1) > list.maxLevel){
            list.maxLevel = level_ + 1;
        }

        let date = new Date();
        let val = date.valueOf();

        let branchIds_parent = treeViewGql.model.IsValidData("BranchIds__", index) ? treeViewGql.model.GetData("BranchIds__", index) : "";
        let innerId_parent = treeViewGql.model.IsValidData("InnerId__", index) ? treeViewGql.model.GetData("InnerId__", index) : "";
        let branchIds = branchIds_parent !== "" ? branchIds_parent + "," + innerId_parent: innerId_parent;


        for(let i = 0; i < model_.GetItemsCount(); i++){
            let newIndex =  index + i + 1;
            treeViewGql.model.InsertNewItem(newIndex);
            treeViewGql.model.CopyItemDataFromModel(newIndex, model_, i);
            treeViewGql.model.SetData("Level__", level_ + 1, newIndex);
            treeViewGql.model.SetData("BranchIds__", branchIds, newIndex);
            treeViewGql.model.SetData("Visible__", true, newIndex);
            treeViewGql.model.SetData("IsOpen__", false, newIndex);
            treeViewGql.model.SetData("OpenState__", -1, newIndex);
            treeViewGql.model.SetData("HasBranch__", false, newIndex);
            treeViewGql.model.SetData("InnerId__", String(val + newIndex), newIndex);

            if(i == 0 && level_ == -1){
                listFrame.contentHeight = treeViewGql.delegateHeight;
            }
            else {
                listFrame.contentHeight += treeViewGql.delegateHeight;
            }
        }
        treeViewGql.setContentWidth();
    }

}

