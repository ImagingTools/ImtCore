import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Rectangle{
	id: treeViewSdl;

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
	property string hoverColor: Style.hover;
	property string selectedTextColor: Style.textColor;
	property string textColor: Style.textColor;

	property real selectionRadius: 2;
	property real selectionOpacity: 1;

	property bool hasAddDelegInfo: false;
	property bool hasSelection: false;
	property bool traceTiming: false;

	property alias model: viewModel;
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

	ListModel{
		id: rows;

		dynamicRoles: true;
	}

	QtObject{
		id: viewModel;

		function getItemsCount(){
			return rows.count;
		}

		function clear(){
			rows.clear();
			treeViewSdl.updateContentHeight();
		}

		function getData(key, index){
			return treeViewSdl.getData(key, index);
		}

		function setData(key, value, index){
			return treeViewSdl.setData(key, value, index);
		}

		function isValidData(key, index){
			return treeViewSdl.isValidData(key, index);
		}

		function removeItem(index){
			rows.remove(index, 1);
		}

		function blockSignals(flag){
			rows.blockSignals(flag);
		}
	}

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

		width: treeViewSdl.width -10;
		height: treeViewSdl.height -10;
		radius: Style.radiusM;

		color : treeViewSdl.color;

		Item{
			id: listFrame;

			width: list.width;
			height: list.height;

			property real contentY: list.contentY;
			property real originY: 0;
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
			property int delegateWidth: treeViewSdl.delegateWidth;
			property int delegateWidthFull: list.maxLevel * treeViewSdl.shift + list.delegateWidth;
			property int maxLevel: 1;
			reuseItems: true

			onContentYChanged: {
				listFrame.contentY = contentY;
			}

			model: rows;

			delegate: Item{
				id: deleg;

				width: !model.Visible__ ? 0 : Math.max(list.width, list.delegateWidthFull);
				height: model.Visible__ == undefined ? 0 : model.Visible__ ? treeViewSdl.delegateHeight : 0;
				opacity: model.Visible__ !== undefined ?  model.Visible__ : false;
				property bool isOpen: model.IsOpen__ == undefined ? false : model.IsOpen__;
				property bool hasChildren: treeViewSdl.itemHasChildren(model.item);
				property bool hasType: treeViewSdl.itemHasType(model.item);
				property int index: model.index;
				property bool isSelected: model.index == treeViewSdl.selectedIndex;
				property bool isHovered: delegateMA.containsMouse;
				property int addItemWidth: 0;
				property var additionalItem: null

				ListView.onReused: {
					if (additionalItem && typeof additionalItem.onRedraw === "function"){
						additionalItem.onRedraw()
					}
				}

				Component.onCompleted: {
					if(treeViewSdl.hasAddDelegInfo){
						additionalItem =  treeViewSdl.additionalDelegateComp.createObject(deleg);
						deleg.addItemWidth = additionalItem.width;
					}
				}

				Rectangle {
					id: selectionRect
					anchors.fill: parent
					radius: treeViewSdl.selectionRadius
					opacity: treeViewSdl.selectionOpacity
					color: treeViewSdl.selectionColor
					visible: treeViewSdl.hasSelection && deleg.isSelected
					z: -1
				}

				Rectangle {
					id: hoverRect
					anchors.fill: parent
					radius: treeViewSdl.selectionRadius
					opacity: treeViewSdl.selectionOpacity
					color: treeViewSdl.hoverColor
					visible: treeViewSdl.hasSelection && deleg.isHovered && !deleg.isSelected
					z: -1
				}

				Item{
					id: delegateContainer;

					anchors.left: parent.left;
					anchors.leftMargin: model.Level__ !== undefined ? model.Level__ * treeViewSdl.shift : 0;
					width: list.delegateWidth;
					height: parent.height;

					function openButtonClicked(){
						if(!deleg.hasChildren){
							return
						}

						if(!model.IsOpen__){
							if(model.HasBranch__){
								treeViewSdl.setVisibleElements(true, model.index)
							}
							else {
								treeViewSdl.model.setData("HasBranch__", true, model.index);
								treeViewSdl.requestSignal(model.index)
							}

							treeViewSdl.model.setData("IsOpen__", true, model.index);
							treeViewSdl.model.setData("OpenState__", 1, model.index);

							treeViewSdl.openBranch(model.index)
							treeViewSdl.openButtonClicked(model.index);

							return
						}

						let count_ = treeViewSdl.getVisibleCountInBranch(model.index);
						if(count_ > treeViewSdl._maxCountToClose){
							treeViewSdl.deleteBranch(model.index);
							treeViewSdl.closeBranch(model.index);
						}
						else {
							treeViewSdl.model.setData("IsOpen__", false, model.index);
							treeViewSdl.model.setData("OpenState__", 0, model.index);
							treeViewSdl.setVisibleElements(false, model.index)
							treeViewSdl.closeBranch(model.index)
						}
					}

					Image{
						id: openButton;

						anchors.verticalCenter: parent.verticalCenter;
						anchors.left: parent.left;
						anchors.leftMargin: 8;

						visible: deleg.hasChildren;
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
						source: !deleg.hasType ? "" : treeViewSdl.getIcon(treeViewSdl.itemValue(model.item, "typeId__"), deleg.isOpen);
					}

					Text{
						id: nameText;

						anchors.verticalCenter: parent.verticalCenter;
						anchors.left: !deleg.hasType ? folderImage.left : folderImage.right;
						anchors.leftMargin: !deleg.hasType ? 0 : 16;

						width: deleg.width - parent.x - x - 20 - deleg.addItemWidth;

						elide: Text.ElideRight;

						font.family: Style.fontFamily;
						font.pixelSize: Style.fontSizeXL !==undefined ? Style.fontSizeXL : 18;
						color: treeViewSdl.textColor;

						text: treeViewSdl.itemText(model.item);
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

						if (treeViewSdl.selectedIndex !== model.index ){
							treeViewSdl.selectedIndex = model.index
							treeViewSdl.selectionChanged()
						}

						treeViewSdl.clicked(model.index);

						if (mouse.button === Qt.RightButton){
							let point = mapToItem(null, this.mouseX, this.mouseY);
							treeViewSdl.rightButtonMouseClicked(point.x, point.y);
						}
					}

					onDoubleClicked: {
						if(mouse.x > openButton.width + 8 + delegateContainer.x){
							treeViewSdl.doubleClicked(model.index);
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
				}
			}
		}

		CustomScrollbar{
			id: scrollVert;

			anchors.left: listFrame.right;
			anchors.leftMargin: 1;

			anchors.bottom: listFrame.bottom;

			targetItem: listFrame;
			secondSize: treeViewSdl.scrollSize;

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

			secondSize: treeViewSdl.scrollSize;

			vertical: false;
			targetItem: list;
		}
	}


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

	function isViewKey(key){
		return key === "Level__" || key === "Visible__" || key === "IsOpen__" || key === "OpenState__" || key === "HasBranch__" || key === "BranchIds__" || key === "InnerId__" || key === "ChildrenCount__";
	}

	function itemValue(sdlItem, key){
		if (!sdlItem || !key){
			return undefined;
		}

		if (sdlItem[key] !== undefined){
			return sdlItem[key];
		}

		if (key.indexOf("m_") === 0){
			let plain = key.slice(2);
			if (sdlItem[plain] !== undefined){
				return sdlItem[plain];
			}
		}

		let prop = "m_" + key[0].toLowerCase() + key.slice(1);
		return sdlItem[prop];
	}

	function itemText(sdlItem){
		let value = treeViewSdl.itemValue(sdlItem, treeViewSdl.nameId);
		if (value === undefined || value === null){
			return "";
		}

		return value;
	}

	function itemHasChildren(sdlItem){
		let value = treeViewSdl.itemValue(sdlItem, "hasChildren__");
		if (value === undefined || value === null){
			return false;
		}

		return value;
	}

	function itemHasType(sdlItem){
		let value = treeViewSdl.itemValue(sdlItem, "typeId__");
		if (value === undefined || value === null){
			return false;
		}

		return true;
	}

	function sourceItem(model_, index){
		if (!model_ || !model_.get){
			return null;
		}

		let row = model_.get(index);
		if (!row){
			return null;
		}

		return row.item;
	}

	function getModelItemsCount(model_){
		if (!model_ || !model_.getItemsCount){
			return 0;
		}

		return model_.getItemsCount();
	}

	function insertTree(index, model_){
		if(!model_ || !treeViewSdl.getModelItemsCount(model_)){
			return;
		}

		let level_ = treeViewSdl.model.isValidData("Level__", index) ? treeViewSdl.model.getData("Level__", index) : -1;

		if((level_ + 1) > list.maxLevel){
			list.maxLevel = level_ + 1;
		}

		let date = new Date();
		let val = date.valueOf();

		let branchIds_parent = treeViewSdl.model.isValidData("BranchIds__", index) ? treeViewSdl.model.getData("BranchIds__", index) : "";
		let innerId_parent = treeViewSdl.model.isValidData("InnerId__", index) ? treeViewSdl.model.getData("InnerId__", index) : "";
		let branchIds = branchIds_parent !== "" ? branchIds_parent + "," + innerId_parent: innerId_parent;

		let counter = 0;
		let count = treeViewSdl.getModelItemsCount(model_);
		let traceStarted = 0;
		if (treeViewSdl.traceTiming){
			traceStarted = treeViewSdl.traceNow();
		}

		treeViewSdl.model.blockSignals(true);

		for(let i = 0; i < count; i++){
			let newIndex = index + i + 1;
			rows.insert(newIndex, {
				item: treeViewSdl.sourceItem(model_, i),
				Level__: level_ + 1,
				BranchIds__: branchIds,
				Visible__: true,
				IsOpen__: false,
				OpenState__: -1,
				HasBranch__: false,
				InnerId__: String(val + newIndex),
				ChildrenCount__: -1
			});
			counter++;
		}

		let insertMs = 0;
		if (treeViewSdl.traceTiming){
			insertMs = treeViewSdl.traceNow() - traceStarted;
			traceStarted = treeViewSdl.traceNow();
		}

		if(index >= 0){
			treeViewSdl.model.setData("ChildrenCount__", count, index);
		}

		treeViewSdl.model.blockSignals(false);
		treeViewSdl.updateContentHeight();

		if(treeViewSdl.selectedIndex >=0 && treeViewSdl.selectedIndex > index){
			treeViewSdl.selectedIndex += counter;
		}
		treeViewSdl.setContentWidth();

		let widthMs = 0;
		if (treeViewSdl.traceTiming){
			widthMs = treeViewSdl.traceNow() - traceStarted;
			traceStarted = treeViewSdl.traceNow();
		}

		if(index === -1){
			treeViewSdl.forceRefresh();
		}

		if (treeViewSdl.traceTiming){
			let refreshMs = 0;
			if (index === -1){
				refreshMs = treeViewSdl.traceNow() - traceStarted;
			}
			console.log("AddressTree: insertTree вставка", Math.round(insertMs), "мс, ширина", Math.round(widthMs), "мс, refresh", Math.round(refreshMs), "мс, узлов", count, "index", index);
		}

		treeViewSdl.inserted(index);
	}

	function traceNow(){
		if (typeof performance !== "undefined" && performance.now){
			return performance.now()
		}

		return Date.now()
	}

	/**
		Removes all descendants of the node at `index` from the model list and marks it closed.
		Each node's BranchIds__ holds a full parent chain (built by \sa treeViewSdl.insertTree),
			any descendant at any depth carries the parent's InnerId__.
		If the node is last at its level, the loop exhausts naturally; cleanup runs unconditionally.
	*/
	function deleteBranch(index){
		if (index < 0){
			treeViewSdl.model.clear()
			return
		}

		let innerId = treeViewSdl.model.getData("InnerId__", index);

		let deletedCount = 0;
		let i = index + 1;
		while(i < treeViewSdl.model.getItemsCount()){
			let branchIds = treeViewSdl.model.isValidData("BranchIds__", i) ? treeViewSdl.model.getData("BranchIds__", i).split(",") : [];

			if(!branchIds.includes(String(innerId))){
				break;
			}
			else {
				treeViewSdl.model.removeItem(i);
				++deletedCount;
			}
		}

		if(treeViewSdl.selectedIndex > index){
			treeViewSdl.selectedIndex -= deletedCount;
		}

		treeViewSdl.model.setData("IsOpen__", false, index);
		treeViewSdl.model.setData("OpenState__", false, index);
		treeViewSdl.model.setData("HasBranch__", false, index);
		treeViewSdl.updateContentHeight();
		treeViewSdl.setContentWidth();
	}

	/*
		Shows or hides all child elements of the node at index.
		Child nodes of a still-closed sub-node stay hidden.
	*/
	function setVisibleElements(visible, index){
		let innerId = treeViewSdl.model.getData("InnerId__", index);
		let found = false;
		let currClosedLevel = -1;

		for(let i = index + 1; i < treeViewSdl.model.getItemsCount(); i++){
			let branchIds = treeViewSdl.model.isValidData("BranchIds__", i) ? treeViewSdl.model.getData("BranchIds__", i).split(",") : [];

			if(!branchIds.includes(String(innerId))){
				if(found){
					break;
				}
				continue;
			}

			found = true;

			let level_curr = treeViewSdl.model.getData("Level__", i);
			let openST = treeViewSdl.model.getData("OpenState__", i);

			if(level_curr <= currClosedLevel){
				currClosedLevel = -1;
			}
			if(visible && openST == 0){
				currClosedLevel = level_curr;
			}

			let ok_visible = !visible || currClosedLevel === -1 || level_curr <= currClosedLevel;

			treeViewSdl.model.setData("Visible__", visible && ok_visible, i);
		}

		treeViewSdl.updateContentHeight();
		treeViewSdl.setContentWidth();
	}

	function updateContentHeight(){
		let count = 0;
		let total = rows.count;
		for (let i = 0; i < total; i++){
			let row = rows.get(i);
			if (row && row.Visible__){
				count++;
			}
		}
		listFrame.contentHeight = count * treeViewSdl.delegateHeight;
	}

	function getVisibleCountInBranch(index){
		let count_ = 0;
		let level_ = treeViewSdl.model.getData("Level__", index)

		for(let i = index + 1; i < treeViewSdl.model.getItemsCount(); i++){
			let currLevel_ = treeViewSdl.model.getData("Level__", i)
			if(currLevel_ === level_){
				break;
			}

			let currVisible_ = treeViewSdl.model.getData("Visible__", i)
			if(currVisible_){
				count_++;
			}
		}

		return count_;
	}

	function setContentWidth(){
		let maxWidth = 0;
		let maxLevel_= 0;
		let total = rows.count;

		for(let i = 0; i < total; i++){
			let row = rows.get(i);
			if(!row || !row.Visible__){
				continue;
			}
			let level = row.Level__ ? row.Level__ : 0;
			let width_ = level * treeViewSdl.shift + list.delegateWidth;
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
			list.contentX = Math.min((maxLevel_-1) * treeViewSdl.shift + list.originX, list.contentWidth - list.width + list.originX);
		}
	}

	function openFunc(index){
		if(index < 0){
			return;
		}

		if(!treeViewSdl.model.getData("hasChildren__", index)){
			treeViewSdl.forcedOpen(index);
			return;
		}

		if(treeViewSdl.model.getData("OpenState__", index) === 1){
			treeViewSdl.forcedOpen(index);
			return;
		}

		if(treeViewSdl.model.getData("HasBranch__", index)){
			treeViewSdl.setVisibleElements(true, index);
			treeViewSdl.forcedOpen(index);
		}
		else {
			treeViewSdl.model.setData("HasBranch__", true, index);
			treeViewSdl.requestSignal(index);
		}

		treeViewSdl.model.setData("IsOpen__", true, index);
		treeViewSdl.model.setData("OpenState__", 1, index);
		treeViewSdl.openBranch(index);
	}

	function moveToElement(index){
		if(index < 0 || index >= treeViewSdl.model.getItemsCount()){
			return;
		}

		let maxContentY = list.contentHeight - list.height + list.originY;
		if(maxContentY <= 0){
			return;
		}

		let contentY__ = list.originY;
		for(let i = 0; i < index; i++){
			let isVisible = treeViewSdl.model.getData("Visible__", i);
			contentY__ += isVisible * treeViewSdl.delegateHeight;
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

	function getData(key, index){
		if(index === undefined || index === null){
			index = 0;
		}

		let row = rows.get(index);
		if(!row){
			return undefined;
		}

		if(treeViewSdl.isViewKey(key)){
			return row[key];
		}

		return treeViewSdl.itemValue(row.item, key);
	}

	function setData(key, value, index){
		if(index === undefined || index === null){
			index = 0;
		}

		if(!treeViewSdl.isViewKey(key)){
			return false;
		}

		rows.setProperty(index, key, value);
		return true;
	}

	function isValidData(key, index){
		let data = treeViewSdl.getData(key, index);
		if(data === undefined || data === null){
			return false;
		}

		return true;
	}

	function getSelectedIndex(){
		return selectedIndex;
	}

	function checkIsOpen(index){
		return treeViewSdl.model.getData("IsOpen__", index);
	}

	function checkHasChildren(index){
		return treeViewSdl.model.getData("hasChildren__", index);
	}

	function findParentIndex(index){
		let branchIds = treeViewSdl.model.isValidData("BranchIds__", index) ? treeViewSdl.model.getData("BranchIds__", index) : "";
		if(branchIds == ""){
			return -1;
		}

		let arr = branchIds.split(",");
		let parentId = arr[arr.length - 1];

		for(let foundIndex = index - 1; foundIndex >= 0; foundIndex--){
			if(treeViewSdl.model.getData("InnerId__", foundIndex) === parentId){
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
		for(let i = startIndex; i < treeViewSdl.model.getItemsCount(); i++){
			let id_curr = treeViewSdl.model.isValidData(nameId, i) ? treeViewSdl.model.getData(nameId, i) : "";
			if(id_curr == id){
				foundIndex = i;
				break;
			}
		}
		return foundIndex;
	}
}
