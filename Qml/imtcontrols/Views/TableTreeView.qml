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
        if(tableTreeView.model.getItemsCount()){
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
            width: tableTreeView.widthDecoratorDynamic.getData("Width",0);

            color: "transparent";

            Component.onCompleted: {
                tableTreeView.widthRecalc.connect(topElement.widthRecalc);
            }

            function widthRecalc(){
                if(tableTreeView && tableTreeView.widthDecoratorDynamic && tableTreeView.widthDecoratorDynamic.getItemsCount())
                    width = tableTreeView.widthDecoratorDynamic.isValidData("Width",0) ? tableTreeView.widthDecoratorDynamic.getData("Width",0) : 0;
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
                                    tableTreeView.model.setData("HasBranch__", true, model.index);
                                    tableTreeView.requestSignal(model.index, model.Level__)
                                }
                                else {
                                    tableTreeView.setVisibleElements(true, model.index)
                                }
                                tableTreeView.model.setData("IsOpen__", true, model.index);
                                tableTreeView.model.setData("OpenState__", 1, model.index);

                                tableTreeView.openBranch(model.index)
                                tableTreeView.openButtonClicked(model.index);
                            }
                            else if(deleg.isOpen){
                                let count_ = tableTreeView.getVisibleCountInBranch(model.index);
                                //let count_ = model.ChildrenCount__;
                                if(count_ <= tableTreeView._maxCountToClose){
                                    tableTreeView.model.setData("IsOpen__", false, model.index);
                                    tableTreeView.model.setData("OpenState__", 0, model.index);
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

        if(!model_ || !model_.getItemsCount()){
            return;
        }

        let level_ = tableTreeView.model.isValidData("Level__", index) ? tableTreeView.model.getData("Level__", index) : -1;
        //console.log("INSERT TREE", index, level_);

        if((level_ + 1) > tableTreeView.maxLevel){
            tableTreeView.maxLevel = level_ + 1;
        }

        let date = new Date();
        let val = date.valueOf();

        let branchIds_parent = tableTreeView.model.isValidData("BranchIds__", index) ? tableTreeView.model.getData("BranchIds__", index) : "";
        let innerId_parent = tableTreeView.model.isValidData("InnerId__", index) ? tableTreeView.model.getData("InnerId__", index) : "";
        let branchIds = branchIds_parent !== "" ? branchIds_parent + "," + innerId_parent: innerId_parent;

        let counter = 0;
        for(let i = 0; i < model_.getItemsCount(); i++){
            let newIndex =  index + i + 1;
            tableTreeView.model.insertNewItem(newIndex);
            tableTreeView.model.copyItemDataFromModel(newIndex, model_, i);
            tableTreeView.model.setData("Level__", level_ + 1, newIndex);
            tableTreeView.model.setData("BranchIds__", branchIds, newIndex);
            tableTreeView.model.setData("Visible__", true, newIndex);
            tableTreeView.model.setData("IsOpen__", false, newIndex);
            tableTreeView.model.setData("OpenState__", -1, newIndex);
            tableTreeView.model.setData("HasBranch__", false, newIndex);
            tableTreeView.model.setData("InnerId__", String(val + newIndex), newIndex);
            tableTreeView.model.setData("ChildrenCount__", -1, newIndex);

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
            tableTreeView.model.setData("ChildrenCount__", model_.getItemsCount(), index);
        }

        if(tableTreeView.selectedIndex >=0 && tableTreeView.selectedIndex > index){
            tableTreeView.selectedIndex += counter;
        }
        tableTreeView.setContentWidth();

        tableTreeView.inserted(index);
    }

    function deleteBranch(index){
        //console.log("DELETE BRANCH", index);
        let innerId = tableTreeView.model.getData("InnerId__", index);
        let found = false;
        let foundChangeCount = 0;

        for(let i = index + 1; i < tableTreeView.model.getItemsCount(); i++){
            let branchIds = tableTreeView.model.isValidData("BranchIds__", i) ? tableTreeView.model.getData("BranchIds__", i) : "";
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
                tableTreeView.model.removeItem(i);
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

        tableTreeView.model.setData("IsOpen__", false, index);
        tableTreeView.model.setData("OpenState__", -1, index);
        tableTreeView.model.setData("HasBranch__", false, index);

        tableTreeView.setContentWidth();
    }

    function setVisibleElements(visible, index){
        // console.log("SET VISIBLE", visible, index);
        let innerId = tableTreeView.model.getData("InnerId__", index);
        let level = tableTreeView.model.getData("Level__", index);
        let found = false;
        let foundChangeCount = 0;

        //for closed
        let currClosedLevel = -1;

        for(let i = index + 1; i < tableTreeView.model.getItemsCount(); i++){
            let branchIds = tableTreeView.model.isValidData("BranchIds__", i) ? tableTreeView.model.getData("BranchIds__", i) : "";
            //console.log("branchIds:: ", branchIds)

            let ok = false;
            let arr = branchIds.split(",");
            let arrCounter = 0;

            //*****************for closed*************//
            let openST = tableTreeView.model.getData("OpenState__", i);
            let level_curr = tableTreeView.model.getData("Level__", i);

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
                tableTreeView.model.setData("Visible__", visible && ok_visible, i);
                let coeff = visible ? 1 : -1;
                let content_height = tableTreeView.getContentHeight();
                content_height += coeff * tableTreeView.delegateHeight;
                tableTreeView.setContentHeight(content_height);
            }
        }

        tableTreeView.setContentWidth();

    }

    function getVisibleCountInBranch(index){
        let count_ = 0;
        let level_ = tableTreeView.model.getData("Level__", index)
        for(let i = index + 1; i < tableTreeView.model.getItemsCount(); i++){
            let currLevel_ = tableTreeView.model.getData("Level__", i)
            if(currLevel_ === level_){
                break;
            }
            let currVisible_ = tableTreeView.model.getData("Visible__", i)
            if(currVisible_){
                count_++;
            }
        }
        //console.log("VisibleCountInBranch:: ", count_);

        return count_;
    }

    function setContentWidth(){
        //console.log("___setContentWidth___")
        let maxWidth = 0;
        let maxLevel_= 0;
        for(let i = 0; i < tableTreeView.model.getItemsCount(); i++){
            let visible = tableTreeView.model.getData("Visible__",i);
            let level = !visible ? 0 : tableTreeView.model.getData("Level__",i);
            let width_ = !visible ? 0 : level * tableTreeView.shift + tableTreeView.delegateWidth;
            if(level > maxLevel_){
                maxLevel_ = level;
            }
            if(width_ > maxWidth){
                maxWidth = width_;
            }
        }
        tableTreeView.maxLevel = maxLevel_;
        tableTreeView.widthDecorator.setData("MinWidth", maxWidth, 0);

        let restWidth = 0;
        for(let i = 1; i < tableTreeView.widthDecoratorDynamic.getItemsCount(); i++){
            let currCellWidth = tableTreeView.widthDecoratorDynamic.getData("Width",i);
            restWidth += currCellWidth;
        }

        let firstCellWidth = tableTreeView.widthDecoratorDynamic.getData("Width",0);
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

        tableTreeView.widthDecorator.setData("Width", maxWidth, 0);
        tableTreeView.setWidth();

    }

    //
    function openFunc(index){
        if(index < 0){
            return;
        }
        let isForcedOpen = false;
        if(tableTreeView.model.getData("HasChildren__", index)){
            if(tableTreeView.model.getData("OpenState__", index) !== 1){
                if(!tableTreeView.model.getData("HasBranch__", index)){
                    tableTreeView.model.setData("HasBranch__", true, index);
                    tableTreeView.requestSignal(index)
                }
                else {
                    tableTreeView.setVisibleElements(true, index)
                    isForcedOpen = true;
                }
                tableTreeView.model.setData("IsOpen__", true, index);
                tableTreeView.model.setData("OpenState__", 1, index);

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
        if(index < 0 || index >= tableTreeView.model.getItemsCount()){
            return;
        }
        let maxContentY = tableTreeView.contentHeight - tableTreeView.height + tableTreeView.originY;
        if(maxContentY <= 0){
            return;
        }
        let contentY__ = tableTreeView.originY;
        for(let i = 0; i < index; i++){
            let isVisible = tableTreeView.model.getData("Visible__", i);
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
        return tableTreeView.model.getData(key, index);
    }

    function getSelectedIndex(){
        return selectedIndex;
    }


    function checkIsOpen(index){
        return tableTreeView.model.getData("IsOpen__", index);
    }

    function checkHasChildren(index){
        return tableTreeView.model.getData("HasChildren__", index);
    }


    function findParentIndex(index){
        let foundIndex = -1;
        let branchIds = tableTreeView.model.isValidData("BranchIds__", index) ? tableTreeView.model.getData("BranchIds__", index) : "";
        if(branchIds == ""){
            //console.log("FOUND_INDEX_ RETURN ", foundIndex);
            return -1;
        }
        let arr = branchIds.split(",");
        let parentId = arr[arr.length - 1];
        for(let i = index - 1; i >=0; i--){
            let innerId = tableTreeView.model.getData("InnerId__", i);
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
        for(let i = 0; i < tableTreeView.model.getItemsCount(); i++){
            let id_curr = tableTreeView.model.isValidData("Id", i) ? tableTreeView.model.getData("Id", i) : "";
            if(id_curr == id){
                foundIndex = i;
                break;
            }
        }
        return foundIndex;
    }
}
