import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0


Rectangle{
    id: treeViewGql;

    color: "transparent";
    property TreeItemModel model: TreeItemModel{};

	property int shift: Style.treeBranchOffset;
    property string nameId: "name";
	property int delegateWidth: Style.sizeHintXXS;
	property int delegateHeight: Style.controlHeightL;
    property alias delegate: list.delegate;
    property bool hasSelection: false;
    property int selectedIndex: -1;
    property int scrollSize: 12;
    property string scrollBackgroundColor: "";
    property string scrollIndicatorColor: "";

    property string selectionColor: Style.selectedColor !== undefined ? Style.selectedColor : "lightsteelblue";
    property string hoverColor: "lightsteelblue";
    property real selectionOpacity: 1;
    property real selectionRadius: 2;
    property string selectedTextColor: Style.textColor;
    property string textColor: Style.textColor;

    property int _maxCountToClose: 10;
    property int _deleteCounter: 0;

    property bool hasAddDelegInfo: false;
    property Component additionalDelegateComp: Component{Item{}}

	property alias reuseItems: list.reuseItems

    signal requestSignal(int index);

    signal clicked(int index);
    signal doubleClicked(int index);
    signal rightButtonMouseClicked(int mX, int mY);

    signal openButtonClicked(int index);

    signal selectionChanged();
    signal openBranch(int index)
    signal closeBranch(int index)

    signal forcedOpen(int index);
    signal inserted(int index);


    onWidthChanged: {
        list.contentX = list.originX;
    }

    onScrollBackgroundColorChanged: {
        if(scrollBackgroundColor !== ""){
            scrollHoriz.backgroundColor = scrollBackgroundColor;
            scrollVert.backgroundColor = scrollBackgroundColor;
        }
    }
    onScrollIndicatorColorChanged: {
        if(scrollIndicatorColor !== ""){
            scrollHoriz.indicatorColor = scrollIndicatorColor;
            scrollVert.indicatorColor = scrollIndicatorColor;
        }

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
			property int delegateWidth: treeViewGql.delegateWidth;
            property int delegateWidthFull: list.maxLevel * treeViewGql.shift + list.delegateWidth;
            property int maxLevel: 1;
			reuseItems: true

            onContentYChanged: {
                listFrame.contentY = contentY;
            }

            model: treeViewGql.model;

            delegate: Item{
                id: deleg;

                width: !model.Visible__ ? 0 : Math.max(list.width, list.delegateWidthFull);
				height: model.Visible__ == undefined ? 0 : model.Visible__ ? treeViewGql.delegateHeight : 0;
				opacity: model.Visible__ !== undefined ?  model.Visible__ : false;
                property bool isOpen: model.IsOpen__ == undefined ? false : model.IsOpen__;
                property int index: model.index;
                property bool isSelected: model.index == treeViewGql.selectedIndex;
                property bool isHovered: delegateMA.containsMouse;
                property var selection_ : null;
                property var hover_ : null;
                property int addItemWidth: 0;
				property var additionalItem: null

				ListView.onReused: {
					if(deleg.hover_){
						deleg.hover_.destroy();
						deleg.hover_ = null;
					}
					if (additionalItem && typeof additionalItem.onRedraw === "function"){
						additionalItem.onRedraw()
					}
				}

                onAddItemWidthChanged: {
                    //console.log("addItemWidth:: ", addItemWidth)
                }

                Component.onCompleted: {
                    if(treeViewGql.hasAddDelegInfo){
						additionalItem =  treeViewGql.additionalDelegateComp.createObject(deleg);
						deleg.addItemWidth = additionalItem.width;
                        //console.log("width::: ",deleg.width, list.width)
                    }
                }

                onIsSelectedChanged: {
                    if(treeViewGql.hasSelection){
                        if(deleg.isSelected){
                            if(deleg.hover_){
                                deleg.hover_.destroy();
                                deleg.hover_ = null;
                            }
                            if(!deleg.selection_){
                                deleg.selection_ = selectionComp.createObject(deleg)
                                deleg.selection_.z = -1;
                            }
                        }
                        else {
                            if(deleg.selection_){
                                deleg.selection_.destroy();
                                deleg.selection_ = null;
                            }
                        }
                    }
                }

                onIsHoveredChanged: {
                    if(treeViewGql.hasSelection && !deleg.isSelected){
                        if(deleg.isHovered){
                            if(!deleg.hover_){
                                deleg.hover_ = hoverComp.createObject(deleg)
                                deleg.hover_.z = -1;
                            }
                        }
                        else {
                            if(deleg.hover_){
                                deleg.hover_.destroy();
                                deleg.hover_ = null;
                            }
                        }
                    }

                }

                Component{
                    id: selectionComp;
                    Rectangle{
                        anchors.fill: parent;
                        radius: treeViewGql.selectionRadius;
                        opacity: treeViewGql.selectionOpacity;
                        color: treeViewGql.selectionColor;
                    }
                }

                Component{
                    id: hoverComp;
                    Rectangle{
                        anchors.fill: parent;
                        radius: treeViewGql.selectionRadius;
                        opacity: treeViewGql.selectionOpacity;
                        color: treeViewGql.hoverColor;
                    }
                }

                Item{
                    id: delegateContainer;

                    anchors.left: parent.left;
                    anchors.leftMargin: model.Level__ !== undefined ? model.Level__ * treeViewGql.shift : 0;
                    width: list.delegateWidth;
                    height: parent.height;

                    function openButtonClicked(){
						if(model.hasChildren__){
							if(!model.IsOpen__){
                                if(!model.HasBranch__){
                                    treeViewGql.model.setData("HasBranch__", true, model.index);
                                    treeViewGql.requestSignal(model.index)
                                }
                                else {
                                    treeViewGql.setVisibleElements(true, model.index)
                                }
                                treeViewGql.model.setData("IsOpen__", true, model.index);
                                treeViewGql.model.setData("OpenState__", 1, model.index);

                                treeViewGql.openBranch(model.index)
                                treeViewGql.openButtonClicked(model.index);
                            }
							else if(model.IsOpen__){

								//console.log(model.ChildrenCount__, treeViewGql._maxCountToClose)
                                let count_ = treeViewGql.getVisibleCountInBranch(model.index);
                                //let count_ = model.ChildrenCount__;
                                if(count_ <= treeViewGql._maxCountToClose){
                                    treeViewGql.model.setData("IsOpen__", false, model.index);
                                    treeViewGql.model.setData("OpenState__", 0, model.index);
									treeViewGql.setVisibleElements(false, model.index)
									treeViewGql.closeBranch(model.index)
                                }
                                else {
                                    treeViewGql.deleteBranch(model.index);
                                    treeViewGql.closeBranch(model.index);
                                }
                            }
                        }
                    }

                    Image{
                        id: openButton;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: parent.left;
                        anchors.leftMargin: 8;

						visible: model.hasChildren__ == undefined ? false : model.hasChildren__;
                        width: 16;
                        height: width;
                        sourceSize.width: width;
                        sourceSize.height: height;
                        source: "../../../" + Style.getIconPath(imageName, Icon.State.On, Icon.Mode.Normal);
						property string imageName: deleg.isOpen ? "Icons/Down" : "Icons/Right";

                    }

                    Image{
                        id: folderImage;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: openButton.right;
                        anchors.leftMargin: 8;

						visible: true//model.typeId__ !== undefined;
						width: Style.iconSizeS;
                        height: width;
                        sourceSize.width: width;
                        sourceSize.height: height;
						source: model.typeId__ == undefined ? "" : treeViewGql.getIcon(model.typeId__, deleg.isOpen);
                    }

                    Text{
                        id: nameText;

                        anchors.verticalCenter: parent.verticalCenter;
						anchors.left: model.typeId__ == undefined ? folderImage.left : folderImage.right;
						anchors.leftMargin: model.typeId__ == undefined ? 0 : 16;

                        width: deleg.width - parent.x - x - 20 - deleg.addItemWidth;

                        elide: Text.ElideRight;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSizeXL !==undefined ? Style.fontSizeXL : 18;
                        color: treeViewGql.textColor;

                        text: model[treeViewGql.nameId] !== undefined ? model[treeViewGql.nameId] : "";
                    }

                }

                MouseArea{
                    id: delegateMA;

                    anchors.fill: parent;

                    acceptedButtons: Qt.LeftButton | Qt.RightButton;

                    hoverEnabled: visible;
                    cursorShape: Qt.PointingHandCursor;
                    onClicked: {
                        if(mouse.x > delegateContainer.x){
                            if(openButton.visible && mouse.x < openButton.width + 8 + delegateContainer.x){
                                delegateContainer.openButtonClicked();
                            }
                            else {
                                if (treeViewGql.selectedIndex !== model.index ){
                                    treeViewGql.selectedIndex = model.index
                                    treeViewGql.selectionChanged()
                                }
                                treeViewGql.clicked(model.index);

                                if (mouse.button === Qt.RightButton){
                                    console.log("TreeViewGqlDelegate onRightButtonMouseClicked");

                                    var point = mapToItem(null, this.mouseX, this.mouseY);
                                    treeViewGql.rightButtonMouseClicked(point.x, point.y);
                                }
                            }
                        }

                    }
                    onDoubleClicked: {
                        if(mouse.x > openButton.width + 8 + delegateContainer.x){
                            treeViewGql.doubleClicked(model.index);
                        }
                    }

                    onContainsMouseChanged: {
                        if (!containsMouse){
                            toolTip.closeTooltip();
                        }
                        else{
                            var point = mapToItem(null, mouseX - toolTip.componentWidth/2, -toolTip.componentHeight);
                            sizeText.toolTipX = point.x;
                            sizeText.toolTipY = point.y;
                            sizeText.sourceWidth = nameText.width;
                            sizeText.text = nameText.text;
                            toolTipPause.restart();

                        }
                    }

                }//delegateMA

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

	function getModelItemsCount(model_){
		return model_.getItemsCount();
	}

	function copyModelItemData(index, externModel_, externIndex){
		treeViewGql.model.copyItemDataFromModel(index, externModel_, externIndex);
	}

    function insertTree(index, model_){

		if(!model_ || !treeViewGql.getModelItemsCount(model_)){
            return;
        }

        let level_ = treeViewGql.model.isValidData("Level__", index) ? treeViewGql.model.getData("Level__", index) : -1;
        //console.log("INSERT TREE", index, level_);

        if((level_ + 1) > list.maxLevel){
            list.maxLevel = level_ + 1;
        }

        let date = new Date();
        let val = date.valueOf();

        let branchIds_parent = treeViewGql.model.isValidData("BranchIds__", index) ? treeViewGql.model.getData("BranchIds__", index) : "";
        let innerId_parent = treeViewGql.model.isValidData("InnerId__", index) ? treeViewGql.model.getData("InnerId__", index) : "";
        let branchIds = branchIds_parent !== "" ? branchIds_parent + "," + innerId_parent: innerId_parent;

        let counter = 0;
		for(let i = 0; i < treeViewGql.getModelItemsCount(model_); i++){
            let newIndex =  index + i + 1;
            treeViewGql.model.insertNewItem(newIndex);

			//treeViewGql.model.copyItemDataFromModel(newIndex, model_, i);
			treeViewGql.copyModelItemData(newIndex, model_, i);

            treeViewGql.model.setData("Level__", level_ + 1, newIndex);
            treeViewGql.model.setData("BranchIds__", branchIds, newIndex);
            treeViewGql.model.setData("Visible__", true, newIndex);
            treeViewGql.model.setData("IsOpen__", false, newIndex);
            treeViewGql.model.setData("OpenState__", -1, newIndex);
            treeViewGql.model.setData("HasBranch__", false, newIndex);
            treeViewGql.model.setData("InnerId__", String(val + newIndex), newIndex);
            treeViewGql.model.setData("ChildrenCount__", -1, newIndex);

            if(i == 0 && level_ == -1){
                listFrame.contentHeight = treeViewGql.delegateHeight;
            }
            else {
                listFrame.contentHeight += treeViewGql.delegateHeight;
            }
            counter++;
        }
        //console.log("ChildrenCount__", model_.getItemsCount())
        if(index >= 0){
			treeViewGql.model.setData("ChildrenCount__", treeViewGql.getModelItemsCount(model_), index);
        }

        if(treeViewGql.selectedIndex >=0 && treeViewGql.selectedIndex > index){
            treeViewGql.selectedIndex += counter;
        }
        treeViewGql.setContentWidth();

        treeViewGql.inserted(index);
    }

    function deleteBranch(index){
        //console.log("DELETE BRANCH", index);
        if (index < 0){
            treeViewGql.model.clear()
            return
        }
        let deletionCompleted = false;

        let innerId = treeViewGql.model.getData("InnerId__", index);
        for(let i = index + 1; i < treeViewGql.model.getItemsCount(); i++){
            let branchIds = treeViewGql.model.isValidData("BranchIds__", i) ? treeViewGql.model.getData("BranchIds__", i) : "";
            //console.log("branchIds:: ", branchIds)
            let ok = false;
            let arr = branchIds.split(",");
            for(let k = 0; k < arr.length; k++){
                if(arr[k] == innerId){
                    ok = true;
                    break;
                }

            }//for k

            if(ok){
                treeViewGql.model.removeItem(i);
                treeViewGql._deleteCounter++;
                listFrame.contentHeight -= treeViewGql.delegateHeight;
                treeViewGql.deleteBranch(index);
                break;
            }
            else {
                deletionCompleted = true;
                break;
            }

        }//for i

        if(deletionCompleted){
            //console.log("deletionCompleted", deletionCompleted)
            if(treeViewGql.selectedIndex >=0 && treeViewGql.selectedIndex > index){
                treeViewGql.selectedIndex -= treeViewGql._deleteCounter;
            }

            treeViewGql._deleteCounter = 0;

            treeViewGql.model.setData("IsOpen__", false, index);
            treeViewGql.model.setData("OpenState__", -1, index);
            treeViewGql.model.setData("HasBranch__", false, index);

            treeViewGql.setContentWidth();
        }
    }

    function setVisibleElements(visible, index){
        //console.log("SET VISIBLE", visible, index);
        let innerId = treeViewGql.model.getData("InnerId__", index);
        let found = false;
        let foundChangeCount = 0;

        //for closed
        let currClosedLevel = -1;

        for(let i = index + 1; i < treeViewGql.model.getItemsCount(); i++){
            let branchIds = treeViewGql.model.isValidData("BranchIds__", i) ? treeViewGql.model.getData("BranchIds__", i) : "";
            //console.log("branchIds:: ", branchIds)
            let ok = false;
            let arr = branchIds.split(",");
            let arrCounter = 0;

            //*****************for closed*************//
            let openST = treeViewGql.model.getData("OpenState__", i);
            let level_curr = treeViewGql.model.getData("Level__", i);

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
                treeViewGql.model.setData("Visible__", visible && ok_visible, i);
                let coeff = visible ? 1 : -1;
                listFrame.contentHeight += coeff * treeViewGql.delegateHeight;
            }
        }

        treeViewGql.setContentWidth();

    }

    function getVisibleCountInBranch(index){
        let count_ = 0;
        let level_ = treeViewGql.model.getData("Level__", index)
        for(let i = index + 1; i < treeViewGql.model.getItemsCount(); i++){
            let currLevel_ = treeViewGql.model.getData("Level__", i)
            if(currLevel_ === level_){
                break;
            }
            let currVisible_ = treeViewGql.model.getData("Visible__", i)
            if(currVisible_){
                count_++;
            }
        }
        //console.log("VisibleCountInBranch:: ", count_);

        return count_;
    }

    function setContentWidth(){
        //console.log("setContentWidth")
        let maxWidth = 0;
        let maxLevel_= 0;
        for(let i = 0; i < treeViewGql.model.getItemsCount(); i++){
            let visible = treeViewGql.model.getData("Visible__",i);
            let level = !visible ? 0 : treeViewGql.model.getData("Level__",i);
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

    function openFunc(index){
        if(index < 0){
            return;
        }
        let isForcedOpen = false;
		if(treeViewGql.model.getData("hasChildren__", index)){
			if(treeViewGql.model.getData("OpenState__", index) !== 1){
				if(!treeViewGql.model.getData("HasBranch__", index)){
					treeViewGql.model.setData("HasBranch__", true, index);
					treeViewGql.requestSignal(index)
                }
                else {
                    treeViewGql.setVisibleElements(true, index)
                    isForcedOpen = true;
                }
				treeViewGql.model.setData("IsOpen__", true, index);
				treeViewGql.model.setData("OpenState__", 1, index);

                treeViewGql.openBranch(index);
            }
            else {
                isForcedOpen = true;
            }

        }
        else {
            isForcedOpen = true;
        }
        if(isForcedOpen){
            treeViewGql.forcedOpen(index);
        }
	}

    function moveToElement(index){
        if(index < 0 || index >= treeViewGql.model.getItemsCount()){
            return;
        }
        let maxContentY = list.contentHeight - list.height + list.originY;
        if(maxContentY <= 0){
            return;
        }
        let contentY__ = list.originY;
        for(let i = 0; i < index; i++){
            let isVisible = treeViewGql.model.getData("Visible__", i);
            contentY__ += isVisible * treeViewGql.delegateHeight;
        }
        list.contentY = Math.min(contentY__, maxContentY);
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
        return treeViewGql.model.getData(key, index);
    }

    function getSelectedIndex(){
        return selectedIndex;
    }


    function checkIsOpen(index){
        return model.getData("IsOpen__", index);
    }

    function checkHasChildren(index){
		return model.getData("hasChildren__", index);
    }


    function findParentIndex(index){
        let foundIndex = -1;
        let branchIds = treeViewGql.model.isValidData("BranchIds__", index) ? treeViewGql.model.getData("BranchIds__", index) : "";
        if(branchIds == ""){
            //console.log("FOUND_INDEX_ RETURN ", foundIndex);
            return -1;
        }
        let arr = branchIds.split(",");
        let parentId = arr[arr.length - 1];
        for(let i = index - 1; i >=0; i--){
            let innerId = treeViewGql.model.getData("InnerId__", i);
            if(innerId == parentId){
                foundIndex =  i;
                break;
            }
        }
        //console.log("FOUND_INDEX ", foundIndex);
        return foundIndex;
    }

    function findIndexById(id, nameId){
        if(nameId == undefined){
            nameId = "id";
        }
        let foundIndex = -1;
        for(let i = 0; i < treeViewGql.model.getItemsCount(); i++){
            let id_curr = treeViewGql.model.isValidData(nameId, i) ? treeViewGql.model.getData(nameId, i) : "";
            if(id_curr == id){
                foundIndex = i;
                break;
            }
        }
        return foundIndex;
    }

    Text{
        id: sizeText;

        anchors.bottom: parent.bottom;

        visible: false;

        font.family: Style.fontFamily;
		font.pixelSize:  Style.fontSizeXL !==undefined ? Style.fontSizeXL : 18;//Style.fontSizeXL;
        color: Style.textColor;

        property real sourceWidth: 0;
        property real toolTipX: 0;
        property real toolTipY: 0;
    }

    PauseAnimation {
        id: toolTipPause;

        duration: 500;

        onFinished:{
            if(sizeText.text !== "" && sizeText.width > sizeText.sourceWidth){
                if (!toolTip.openST){
                    toolTip.text = sizeText.text;
                    toolTip.openTooltip(sizeText.toolTipX, sizeText.toolTipY)
                }
            }
        }
    }

    Timer {
        id: timerToolTip;
        interval: 3000;
        onTriggered: {
            toolTip.closeTooltip();
        }
    }

    CustomTooltip {
        id: toolTip;

        text: "";
		componentMinHeight: Style.controlHeightM;

        function openTooltip(xX, yY){
            open(xX, yY)

            timerToolTip.restart();
        }
    }

}

