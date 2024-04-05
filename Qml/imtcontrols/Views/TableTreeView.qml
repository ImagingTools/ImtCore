import QtQuick 2.12
import Acf 1.0;
import imtcontrols 1.0

Table{
    id: tableTreeView;

    property TreeItemModel model: TreeItemModel{};

    property int shift: 35;
    property string nameId: "Name";

    property int delegateHeight: itemHeight;
    property int delegateWidth: 200;
    property int delegateWidthFull: maxLevel * shift + delegateWidth;
    property int maxLevel: 1;
    property bool hasSelection: true;

    property int _maxCountToClose: 10;
    property int _deleteCounter: 0;

    signal requestSignal(int index);

//    signal doubleClicked(int index); // redefine!!!
//    signal rightButtonMouseClicked(int mX, int mY); // redefine!!!

    signal openButtonClicked(int index);

//    signal selectionChanged(); // redefine!!!
    signal openBranch(int index)
    signal closeBranch(int index)

    signal forcedOpen(int index);
    signal inserted(int index);

    onWidthChanged: {
        if(tableTreeView.model.GetItemsCount()){
            setContentWidth();
        }
        tableTreeView.contentX = tableTreeView.originX;
    }

    itemHeight: 40;
    headerHeight: 40;
    visibleLeftBorderFirst: false;
    hasFilter: false;
    hasSort: false;
    cacheBuffer: elementsCount * itemHeight;
    canSetBorderParams: true;
    enableAlternating : false;
    canMoveColumns: true;
    isFrameScrolling: true;
    contentWidth: width;
    elements: tableTreeView.model;
    onHeadersChanged: {
        if(headers){
            let nameIndex = tableTreeView.getHeaderIndex("Name");
            tableTreeView.setColumnContentComponent(nameIndex, nameComp);
        }

    }

    rowDelegate: Component{
        TableRowDelegateBase{
        id: deleg;

        width: Math.max(tableTreeView.width, tableTreeView.contentWidth);

        height: model.Visible__ ? tableTreeView.delegateHeight : 0;
        opacity: model.Visible__;
        contentHeight: tableTreeView.itemHeight;
        clip: true;
        tableItem: tableTreeView;
        property bool isOpen: model.IsOpen__ == undefined ? false : model.IsOpen__;

        onClicked: {
            if (tableTreeView.selectedIndex !== model.index ){
                tableTreeView.selectedIndex = model.index;
                tableTreeView.selectionChanged();
            }
            tableTreeView.clicked(model.index);
        }
        onRightButtonMouseClicked: {
            var point = mapToItem(null, mX, mY);
            tableTreeView.rightButtonMouseClicked(point.x, point.y);
        }
        onDoubleClicked: {
            tableTreeView.doubleClicked(model.index);
        }


//        TableDelegate {
//            id: tableRow;

//            minHeight: tableTreeView.itemHeight;
//            width: parent.width;
//            headers: tableTreeView.headers;
//            //!!!
//            cellDecorator: tableTreeView.cellDecorator;
//            widthDecorator: tableTreeView.widthDecorator;
//            canSetBorderParams: true;
//            tableItem: tableTreeView;
//            selected: !tableTreeView.hasSelection ? false : tableTreeView.selectedIndex == model.index;


//        }
        TableRowViewer {
            id: tableRowViewer;

            width: parent.width;
            height: tableTreeView.itemHeight;
            z: 10;

            rowDelegate: deleg
            model: deleg.columnCount;
        }

        Rectangle{
            id : topElement;

            height: parent.height;
            width: tableTreeView.widthDecoratorDynamic.GetData("Width",0);

            color: "transparent";

            Component.onCompleted: {
                tableTreeView.widthRecalc.connect(topElement.widthRecalc);
            }

            function widthRecalc(){
                if(tableTreeView && tableTreeView.widthDecoratorDynamic && tableTreeView.widthDecoratorDynamic.GetItemsCount())
                    width = tableTreeView.widthDecoratorDynamic.IsValidData("Width",0) ? tableTreeView.widthDecoratorDynamic.GetData("Width",0) : 0;
            }

            Rectangle{

                anchors.left: parent.left;
                anchors.leftMargin: model.Level__ !== undefined ? model.Level__ * tableTreeView.shift : 0;
                width: tableTreeView.delegateWidth;
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
                    iconSource:  "../../../" +Style.getIconPath(imageName, Icon.State.On, Icon.Mode.Normal);

                    decorator: Component { ButtonDecorator{
                        color: "transparent";
                        border.color: "transparent";
                    }}

                    onClicked: {
                        tableTreeView.selectedIndex = model.index;
                        if(model.HasChildren__){
                            if(!deleg.isOpen){
                                if(!model.HasBranch__){
                                    tableTreeView.model.SetData("HasBranch__", true, model.index);
                                    tableTreeView.requestSignal(model.index, model.Level__)
                                }
                                else {
                                    tableTreeView.setVisibleElements(true, model.index)
                                }
                                tableTreeView.model.SetData("IsOpen__", true, model.index);
                                tableTreeView.model.SetData("OpenState__", 1, model.index);

                                tableTreeView.openBranch(model.index)
                                tableTreeView.openButtonClicked(model.index);
                            }
                            else if(deleg.isOpen){
                                if(model.ChildrenCount__ <= tableTreeView._maxCountToClose){
                                    tableTreeView.model.SetData("IsOpen__", false, model.index);
                                    tableTreeView.model.SetData("OpenState__", 0, model.index);
                                    tableTreeView.setVisibleElements(false, model.index)
                                    tableTreeView.closeBranch(model.index);
                                }
                                else {
                                    tableTreeView.deleteBranch(model.index);
                                    tableTreeView.closeBranch(model.index);
                                }
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
                    source: model.TypeId__ == undefined ? "" : tableTreeView.getIcon(model.TypeId__, deleg.isOpen);
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

                    text: model[tableTreeView.nameId] !== undefined ? model[tableTreeView.nameId] : "";
                }


            }

        }//topElement
    }}//delegate


    Component{
        id: nameComp;

        TableCellDelegateBase{
            height: tableTreeView.itemHeight;

        }
    }

    function insertTree(index, model_){

        if(!model_ || !model_.GetItemsCount()){
            return;
        }

        let level_ = tableTreeView.model.IsValidData("Level__", index) ? tableTreeView.model.GetData("Level__", index) : -1;
        //console.log("INSERT TREE", index, level_);

        if((level_ + 1) > tableTreeView.maxLevel){
            tableTreeView.maxLevel = level_ + 1;
        }

        let date = new Date();
        let val = date.valueOf();

        let branchIds_parent = tableTreeView.model.IsValidData("BranchIds__", index) ? tableTreeView.model.GetData("BranchIds__", index) : "";
        let innerId_parent = tableTreeView.model.IsValidData("InnerId__", index) ? tableTreeView.model.GetData("InnerId__", index) : "";
        let branchIds = branchIds_parent !== "" ? branchIds_parent + "," + innerId_parent: innerId_parent;

        let counter = 0;
        for(let i = 0; i < model_.GetItemsCount(); i++){
            let newIndex =  index + i + 1;
            tableTreeView.model.InsertNewItem(newIndex);
            tableTreeView.model.CopyItemDataFromModel(newIndex, model_, i);
            tableTreeView.model.SetData("Level__", level_ + 1, newIndex);
            tableTreeView.model.SetData("BranchIds__", branchIds, newIndex);
            tableTreeView.model.SetData("Visible__", true, newIndex);
            tableTreeView.model.SetData("IsOpen__", false, newIndex);
            tableTreeView.model.SetData("OpenState__", -1, newIndex);
            tableTreeView.model.SetData("HasBranch__", false, newIndex);
            tableTreeView.model.SetData("InnerId__", String(val + newIndex), newIndex);
            tableTreeView.model.SetData("ChildrenCount__", -1, newIndex);

            if(i == 0 && level_ == -1){
                tableTreeView.setContentHeight(tableTreeView.delegateHeight);
            }
            else {
                let content_height = tableTreeView.getContentHeight();
                content_height += tableTreeView.delegateHeight;
                tableTreeView.setContentHeight(content_height);
            }
            counter++;
        }

        if(index >= 0){
            tableTreeView.model.SetData("ChildrenCount__", model_.GetItemsCount(), index);
        }

        if(tableTreeView.selectedIndex >=0 && tableTreeView.selectedIndex > index){
            tableTreeView.selectedIndex += counter;
        }
        tableTreeView.setContentWidth();

        tableTreeView.inserted(index);
    }

    function deleteBranch(index){
        //console.log("DELETE BRANCH", index);
        let innerId = tableTreeView.model.GetData("InnerId__", index);
        let found = false;
        let foundChangeCount = 0;

        for(let i = index + 1; i < tableTreeView.model.GetItemsCount(); i++){
            let branchIds = tableTreeView.model.IsValidData("BranchIds__", i) ? tableTreeView.model.GetData("BranchIds__", i) : "";
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
                tableTreeView.model.RemoveItem(i);
                tableTreeView._deleteCounter++;
                let content_height = tableTreeView.getContentHeight();
                content_height -= tableTreeView.delegateHeight;
                tableTreeView.setContentHeight(content_height);

                tableTreeView.deleteBranch(index);
            }

        }

        if(tableTreeView.selectedIndex >=0 && tableTreeView.selectedIndex > index){
            tableTreeView.selectedIndex -= tableTreeView._deleteCounter;
        }

        tableTreeView._deleteCounter = 0;

        tableTreeView.model.SetData("IsOpen__", false, index);
        tableTreeView.model.SetData("OpenState__", -1, index);
        tableTreeView.model.SetData("HasBranch__", false, index);

        tableTreeView.setContentWidth();
    }

    function setVisibleElements(visible, index){
        // console.log("SET VISIBLE", visible, index);
        let innerId = tableTreeView.model.GetData("InnerId__", index);
        let level = tableTreeView.model.GetData("Level__", index);
        let found = false;
        let foundChangeCount = 0;

        //for closed
        let currClosedLevel = -1;

        for(let i = index + 1; i < tableTreeView.model.GetItemsCount(); i++){
            let branchIds = tableTreeView.model.IsValidData("BranchIds__", i) ? tableTreeView.model.GetData("BranchIds__", i) : "";
            //console.log("branchIds:: ", branchIds)

            let ok = false;
            let arr = branchIds.split(",");
            let arrCounter = 0;

            //*****************for closed*************//
            let openST = tableTreeView.model.GetData("OpenState__", i);
            let level_curr = tableTreeView.model.GetData("Level__", i);

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
                tableTreeView.model.SetData("Visible__", visible && ok_visible, i);
                let coeff = visible ? 1 : -1;
                let content_height = tableTreeView.getContentHeight();
                content_height += coeff * tableTreeView.delegateHeight;
                tableTreeView.setContentHeight(content_height);
            }
        }

        tableTreeView.setContentWidth();

    }

    function setContentWidth(){
        //console.log("___setContentWidth___")
        let maxWidth = 0;
        let maxLevel_= 0;
        for(let i = 0; i < tableTreeView.model.GetItemsCount(); i++){
            let visible = tableTreeView.model.GetData("Visible__",i);
            let level = !visible ? 0 : tableTreeView.model.GetData("Level__",i);
            let width_ = !visible ? 0 : level * tableTreeView.shift + tableTreeView.delegateWidth;
            if(level > maxLevel_){
                maxLevel_ = level;
            }
            if(width_ > maxWidth){
                maxWidth = width_;
            }
        }
        tableTreeView.maxLevel = maxLevel_;
        tableTreeView.widthDecorator.SetData("MinWidth", maxWidth, 0);

        let restWidth = 0;
        for(let i = 1; i < tableTreeView.widthDecoratorDynamic.GetItemsCount(); i++){
            let currCellWidth = tableTreeView.widthDecoratorDynamic.GetData("Width",i);
            restWidth += currCellWidth;
        }

        let firstCellWidth = tableTreeView.widthDecoratorDynamic.GetData("Width",0);
        tableTreeView.contentWidth += (maxWidth - firstCellWidth);
        if(tableTreeView.contentWidth < tableTreeView.elementsListWidth){
            tableTreeView.contentWidth = tableTreeView.elementsListWidth;
        }
        if(tableTreeView.contentWidth > tableTreeView.elementsListWidth){
            tableTreeView.contentX = Math.min((maxLevel_-1) * tableTreeView.shift + tableTreeView.originX, tableTreeView.contentWidth - tableTreeView.elementsListWidth + tableTreeView.originX);
        }
        else {
            tableTreeView.contentX = tableTreeView.originX;
        }

        let tableWidth = Math.max(tableTreeView.width, tableTreeView.contentWidth);
        if(maxWidth < tableWidth - restWidth) {

            maxWidth =  tableWidth - restWidth;
        }

        tableTreeView.widthDecorator.SetData("Width", maxWidth, 0);
        tableTreeView.setWidth();

    }

    //
    function openFunc(index){
        if(index < 0){
            return;
        }
        let isForcedOpen = false;
        if(tableTreeView.model.GetData("HasChildren__", index)){
            if(tableTreeView.model.GetData("OpenState__", index) !== 1){
                if(!tableTreeView.model.GetData("HasBranch__", index)){
                    tableTreeView.model.SetData("HasBranch__", true, index);
                    tableTreeView.requestSignal(index)
                }
                else {
                    tableTreeView.setVisibleElements(true, index)
                    isForcedOpen = true;
                }
                tableTreeView.model.SetData("IsOpen__", true, index);
                tableTreeView.model.SetData("OpenState__", 1, index);

                tableTreeView.openBranch(index);
            }
            else {
                isForcedOpen = true;
            }

        }
        else {
            isForcedOpen = true;
        }
        if(isForcedOpen){
            tableTreeView.forcedOpen(index);
        }
    }

    function moveToElement(index){
        if(index < 0 || index >= tableTreeView.model.GetItemsCount()){
            return;
        }
        let maxContentY = tableTreeView.contentHeight - tableTreeView.height + tableTreeView.originY;
        if(maxContentY <= 0){
            return;
        }
        let contentY__ = tableTreeView.originY;
        for(let i = 0; i < index; i++){
            let isVisible = tableTreeView.model.GetData("Visible__", i);
            contentY__ += isVisible * tableTreeView.delegateHeight;
        }
        tableTreeView.contentY = Math.min(contentY__, maxContentY);
    }
    //

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

    function getData(key,index){
        return tableTreeView.model.GetData(key, index);
    }

    function getSelectedIndex(){
        return selectedIndex;
    }


    function checkIsOpen(index){
        return tableTreeView.model.GetData("IsOpen__", index);
    }

    function checkHasChildren(index){
        return tableTreeView.model.GetData("HasChildren__", index);
    }


    function findParentIndex(index){
        let foundIndex = -1;
        let branchIds = tableTreeView.model.IsValidData("BranchIds__", index) ? tableTreeView.model.GetData("BranchIds__", index) : "";
        if(branchIds == ""){
            //console.log("FOUND_INDEX_ RETURN ", foundIndex);
            return -1;
        }
        let arr = branchIds.split(",");
        let parentId = arr[arr.length - 1];
        for(let i = index - 1; i >=0; i--){
            let innerId = tableTreeView.model.GetData("InnerId__", i);
            if(innerId == parentId){
                foundIndex =  i;
                break;
            }
        }
        //console.log("FOUND_INDEX ", foundIndex);
        return foundIndex;
    }

    function findIndexById(id){
        let foundIndex = -1;
        for(let i = 0; i < tableTreeView.model.GetItemsCount(); i++){
            let id_curr = tableTreeView.model.IsValidData("Id", i) ? tableTreeView.model.GetData("Id", i) : "";
            if(id_curr == id){
                foundIndex = i;
                break;
            }
        }
        return foundIndex;
    }
}
