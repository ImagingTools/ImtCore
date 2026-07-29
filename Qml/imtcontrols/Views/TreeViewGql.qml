import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Rectangle{
	id: treeViewGql;

	color: "transparent";

	property int shift: Style.treeBranchOffset;
	property int delegateWidth: Style.sizeHintXXS;
	property int delegateHeight: Style.controlHeightL;
	property int selectedIndex: -1;
	property int scrollSize: 12;
	property int _maxCountToClose: 10;

	property string nameId: "name";
	property string scrollBackgroundColor: "";
	property string scrollIndicatorColor: "";

	property string selectionColor: Style.selectedColor !== undefined ? Style.selectedColor : "lightsteelblue";
	property string hoverColor: "lightsteelblue";
	property string selectedTextColor: Style.textColor;
	property string textColor: Style.textColor;

	property real selectionRadius: 2;
	property real selectionOpacity: 1;

	property bool hasAddDelegInfo: false;
	property bool hasSelection: false;

	property TreeItemModel model: TreeItemModel{};
	property Component additionalDelegateComp: Component{Item{}}

	property alias delegate: list.delegate;
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
			property real originY: 0; // list.originY;
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
				property int addItemWidth: 0;
				property var additionalItem: null

				ListView.onReused: {
					if (additionalItem && typeof additionalItem.onRedraw === "function"){
						additionalItem.onRedraw()
					}
				}

				onAddItemWidthChanged: {
					// console.log("addItemWidth:: ", addItemWidth)
				}

				Component.onCompleted: {
					if(treeViewGql.hasAddDelegInfo){
						additionalItem =  treeViewGql.additionalDelegateComp.createObject(deleg);
						deleg.addItemWidth = additionalItem.width;
						// console.log("width::: ",deleg.width, list.width)
					}
				}

				Rectangle {
					id: selectionRect
					anchors.fill: parent
					radius: treeViewGql.selectionRadius
					opacity: treeViewGql.selectionOpacity
					color: treeViewGql.selectionColor
					visible: treeViewGql.hasSelection && deleg.isSelected
					z: -1
				}

				Rectangle {
					id: hoverRect
					anchors.fill: parent
					radius: treeViewGql.selectionRadius
					opacity: treeViewGql.selectionOpacity
					color: treeViewGql.hoverColor
					visible: treeViewGql.hasSelection && deleg.isHovered && !deleg.isSelected
					z: -1
				}

				Item{
					id: delegateContainer;

					anchors.left: parent.left;
					anchors.leftMargin: model.Level__ !== undefined ? model.Level__ * treeViewGql.shift : 0;
					width: list.delegateWidth;
					height: parent.height;

					function openButtonClicked(){
						if(!model.hasChildren__){
							return
						}

						// if branch is not open - open it and request data if it was not requested before
						if(!model.IsOpen__){
							if(model.HasBranch__){
								treeViewGql.setVisibleElements(true, model.index)
							}
							else {
								treeViewGql.model.setData("HasBranch__", true, model.index);
								treeViewGql.requestSignal(model.index)
							}

							treeViewGql.model.setData("IsOpen__", true, model.index);
							treeViewGql.model.setData("OpenState__", 1, model.index);

							treeViewGql.openBranch(model.index)
							treeViewGql.openButtonClicked(model.index);

							return
						}

						/// close branch or delete if count in branch is more than _maxCountToClose

						// console.log(model.ChildrenCount__, treeViewGql._maxCountToClose)
						let count_ = treeViewGql.getVisibleCountInBranch(model.index);
						// let count_ = model.ChildrenCount__;
						if(count_ > treeViewGql._maxCountToClose){
							treeViewGql.deleteBranch(model.index);
							treeViewGql.closeBranch(model.index);
						}
						else {
							treeViewGql.model.setData("IsOpen__", false, model.index);
							treeViewGql.model.setData("OpenState__", 0, model.index);
							treeViewGql.setVisibleElements(false, model.index)
							treeViewGql.closeBranch(model.index)
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

						visible: true
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
						if(mouse.x <= delegateContainer.x){
							return
						}

						if(openButton.visible && mouse.x < openButton.width + 8 + delegateContainer.x){
							delegateContainer.openButtonClicked();

							return
						}

						if (treeViewGql.selectedIndex !== model.index ){
							treeViewGql.selectedIndex = model.index
							treeViewGql.selectionChanged()
						}

						treeViewGql.clicked(model.index);

						if (mouse.button === Qt.RightButton){
							// console.log("TreeViewGqlDelegate onRightButtonMouseClicked");

							let point = mapToItem(null, this.mouseX, this.mouseY);
							treeViewGql.rightButtonMouseClicked(point.x, point.y);
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

							return;
						}

						let point = mapToItem(null, mouseX - toolTip.componentWidth/2, -toolTip.componentHeight);
						sizeText.toolTipX = point.x;
						sizeText.toolTipY = point.y;
						sizeText.sourceWidth = nameText.width;
						sizeText.text = nameText.text;
						toolTipPause.restart();
					}
				} // delegateMA
			} // delegate
		} // list

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
	} // listContainer


	Text{
		id: sizeText;

		anchors.bottom: parent.bottom;

		visible: false;

		font.family: Style.fontFamily;
		font.pixelSize:  Style.fontSizeXL !==undefined ? Style.fontSizeXL : 18;
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

	// _________________FUNCTIONS__________________

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

		if(index === -1){
			treeViewGql.forceRefresh();
		}

		treeViewGql.inserted(index);
	}

	/**
		Removes all descendants of the node at `index` from the model list and marks it closed.
		Each node's BranchIds__ holds a full parent chain (built by \sa treeViewGql.insertTree),
			any descendant at any depth carries the parent's InnerId__.
		If the node is last at its level, the loop exhausts naturally; cleanup runs unconditionally.
	*/
	function deleteBranch(index){
		if (index < 0){
			treeViewGql.model.clear()
			return
		}

		let innerId = treeViewGql.model.getData("InnerId__", index);
		// console.log("inner id", innerId)

		let deletedCount = 0;
		let i = index + 1;
		while(i < treeViewGql.model.getItemsCount()){
			let branchIds = treeViewGql.model.isValidData("BranchIds__", i) ? treeViewGql.model.getData("BranchIds__", i).split(",") : [];
			// console.log("branchIds::", branchIds)

			if(!branchIds.includes(String(innerId))){
				// not a child element: branch is fully processed
				break;
			}
			else {
				treeViewGql.model.removeItem(i);
				listFrame.contentHeight -= treeViewGql.delegateHeight;
				++deletedCount;
			}
		}

		/*
			Shift selectedIndex back by removed item count to select correct node after deletion.
			If the last selected node is deleted, it becomes -1 (no selection)
		*/
		if(treeViewGql.selectedIndex > index){
			treeViewGql.selectedIndex -= deletedCount;
		}

		treeViewGql.model.setData("IsOpen__", false, index);
		treeViewGql.model.setData("OpenState__", false, index);
		treeViewGql.model.setData("HasBranch__", false, index);
		treeViewGql.setContentWidth();
	}

	/*
		Shows or hides all child elements of the node at index.
		Child nodes of a still-closed sub-node stay hidden.
	*/
	function setVisibleElements(visible, index){
		// console.log("SET VISIBLE", visible, index);
		let innerId = treeViewGql.model.getData("InnerId__", index);
		let found = false;
		let currClosedLevel = -1; // for closed

		for(let i = index + 1; i < treeViewGql.model.getItemsCount(); i++){
			let branchIds = treeViewGql.model.isValidData("BranchIds__", i) ? treeViewGql.model.getData("BranchIds__", i).split(",") : [];

			// console.log("branchIds:: ", branchIds)
			if(!branchIds.includes(String(innerId))){
				if(found){
					break;
				}
				continue;
			}

			found = true;

			let level_curr = treeViewGql.model.getData("Level__", i);
			let openST = treeViewGql.model.getData("OpenState__", i);

			if(level_curr <= currClosedLevel){
				currClosedLevel = -1;
			}
			if(visible && openST == 0){
				currClosedLevel = level_curr;
			}

			let ok_visible = !visible || currClosedLevel === -1 || level_curr <= currClosedLevel;

			treeViewGql.model.setData("Visible__", visible && ok_visible, i);
			listFrame.contentHeight += visible ? treeViewGql.delegateHeight : -treeViewGql.delegateHeight;
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
		if(list.contentWidth <= list.width){
			list.contentX = list.originX;
		}
		else {
			list.contentX = Math.min((maxLevel_-1) * treeViewGql.shift + list.originX, list.contentWidth - list.width + list.originX);
		}
	}

	function openFunc(index){
		if(index < 0){
			return;
		}

		if(!treeViewGql.model.getData("hasChildren__", index)){
			treeViewGql.forcedOpen(index);
			return;
		}

		if(treeViewGql.model.getData("OpenState__", index) === 1){
			treeViewGql.forcedOpen(index);
			return;
		}

		if(treeViewGql.model.getData("HasBranch__", index)){
			treeViewGql.setVisibleElements(true, index);
			treeViewGql.forcedOpen(index);
		}
		else {
			treeViewGql.model.setData("HasBranch__", true, index);
			treeViewGql.requestSignal(index);
		}

		treeViewGql.model.setData("IsOpen__", true, index);
		treeViewGql.model.setData("OpenState__", 1, index);
		treeViewGql.openBranch(index);
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

	function getIcon(type, isOpen){
		let imageName = "Icons/New";
		if(type == "Node"){
			imageName = isOpen ? "Icons/FolderOpened" : "Icons/FolderClosed";
		}

		let source = "../../../" + Style.getIconPath(imageName, Icon.State.On, Icon.Mode.Normal);

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
		let branchIds = treeViewGql.model.isValidData("BranchIds__", index) ? treeViewGql.model.getData("BranchIds__", index) : "";
		if(branchIds == ""){
			return -1;
		}

		let arr = branchIds.split(",");
		let parentId = arr[arr.length - 1];

		for(let foundIndex = index - 1; foundIndex >= 0; foundIndex--){
			if(treeViewGql.model.getData("InnerId__", foundIndex) === parentId){
				return foundIndex;
			}
		}

		return -1;
	}

	function forceRefresh(){
		let temp = list.model;
		list.model = null;
		list.model = temp;
	}

	function findIndexById(id, nameId, startIndex){
		if(nameId == undefined){
			nameId = "id";
		}
		if(startIndex == undefined){
			startIndex = 0;
		}

		let foundIndex = -1;
		for(let i = startIndex; i < treeViewGql.model.getItemsCount(); i++){
			let id_curr = treeViewGql.model.isValidData(nameId, i) ? treeViewGql.model.getData(nameId, i) : "";
			if(id_curr == id){
				foundIndex = i;
				break;
			}
		}
		return foundIndex;
	}
}

