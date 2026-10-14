import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Rectangle{
	id: treeViewPure;

	color: "transparent";

	property int shift: Style.treeBranchOffset;
	property int delegateWidth: Style.sizeHintXXS;
	property int delegateHeight: Style.controlHeightL;
	property int selectedIndex: -1;
	property int scrollSize: 12;
	property int maxLevel: 0;
	property int revision: 0;

	property string nameId: "name";
	property string scrollBackgroundColor: "";
	property string scrollIndicatorColor: "";

	property string selectionColor: Style.selectedColor !== undefined ? Style.selectedColor : "lightsteelblue";
	property string hoverColor: Style.hover;
	property string textColor: Style.textColor;

	property real selectionRadius: 2;
	property real selectionOpacity: 1;

	property bool hasAddDelegInfo: false;
	property bool hasSelection: false;

	property var nodes: [];
	property var flatNodes: [];

	property alias model: viewModel;
	property Component additionalDelegateComp: Component{Item{}}

	signal requestSignal(int index);
	signal clicked(int index);
	signal doubleClicked(int index);
	signal rightButtonMouseClicked(int mX, int mY);
	signal openButtonClicked(int index);
	signal selectionChanged();
	signal openBranch(int index);
	signal closeBranch(int index);
	signal forcedOpen(int index);
	signal inserted(int index);

	QtObject{
		id: viewModel;

		function getItemsCount(){
			return treeViewPure.flatNodes ? treeViewPure.flatNodes.length : 0;
		}

		function clear(){
			treeViewPure.nodes = [];
			treeViewPure.flatNodes = [];
			treeViewPure.maxLevel = 0;
			treeViewPure.bump();
		}

		function getData(key, index){
			return treeViewPure.getData(key, index);
		}

		function isValidData(key, index){
			let data = treeViewPure.getData(key, index);
			if (data === undefined || data === null){
				return false;
			}
			return true;
		}
	}

	onScrollBackgroundColorChanged: {
		if(scrollBackgroundColor !== ""){
			scrollVert.backgroundColor = scrollBackgroundColor;
		}
	}
	onScrollIndicatorColorChanged: {
		if(scrollIndicatorColor !== ""){
			scrollVert.indicatorColor = scrollIndicatorColor;
		}
	}

	Component{
		id: nodeComp;

		Item{
			id: nodeItem;

			width: Math.max(rootList.width, treeViewPure.delegateWidth + treeViewPure.maxLevel * treeViewPure.shift);
			height: rowItem.height + childList.height;

			property var node: modelData;
			property int index: treeViewPure.revision >= 0 && node ? node.flatIndex : -1;
			property bool isOpen: treeViewPure.revision >= 0 && node ? node.open : false;
			property bool hasChildren: treeViewPure.nodeHasChildren(node);
			property bool hasType: treeViewPure.itemHasType(node ? node.item : null);
			property bool isSelected: index === treeViewPure.selectedIndex;
			property bool isHovered: rowMouse.containsMouse;
			property int addItemWidth: 0;
			property var additionalItem: null;
			property var childItems: treeViewPure.revision >= 0 && node && node.open && node.children ? node.children : [];

			Component.onCompleted: {
				if(treeViewPure.hasAddDelegInfo){
					additionalItem = treeViewPure.additionalDelegateComp.createObject(nodeItem);
					nodeItem.addItemWidth = additionalItem.width;
				}
			}

			Item{
				id: rowItem;

				width: parent.width;
				height: treeViewPure.delegateHeight;

				Rectangle{
					id: selectionRect;

					anchors.fill: parent;
					radius: treeViewPure.selectionRadius;
					opacity: treeViewPure.selectionOpacity;
					color: treeViewPure.selectionColor;
					visible: treeViewPure.hasSelection && nodeItem.isSelected;
					z: -1;
				}

				Rectangle{
					id: hoverRect;

					anchors.fill: parent;
					radius: treeViewPure.selectionRadius;
					opacity: treeViewPure.selectionOpacity;
					color: treeViewPure.hoverColor;
					visible: treeViewPure.hasSelection && nodeItem.isHovered && !nodeItem.isSelected;
					z: -1;
				}

				Item{
					id: rowContent;

					anchors.left: parent.left;
					anchors.leftMargin: nodeItem.node ? nodeItem.node.level * treeViewPure.shift : 0;
					width: treeViewPure.delegateWidth;
					height: parent.height;

					Image{
						id: openButton;

						anchors.verticalCenter: parent.verticalCenter;
						anchors.left: parent.left;
						anchors.leftMargin: 8;

						visible: nodeItem.hasChildren;
						width: 16;
						height: width;
						sourceSize.width: width;
						sourceSize.height: height;
						source: "../../../" + Style.getIconPath(imageName, Icon.State.On, Icon.Mode.Normal);
						property string imageName: nodeItem.isOpen ? "Icons/Down" : "Icons/Right";
					}

					Image{
						id: folderImage;

						anchors.verticalCenter: parent.verticalCenter;
						anchors.left: openButton.right;
						anchors.leftMargin: 8;

						width: Style.iconSizeS;
						height: width;
						sourceSize.width: width;
						sourceSize.height: height;
						source: !nodeItem.hasType ? "" : treeViewPure.getIcon(treeViewPure.fieldValue(nodeItem.node.item, "typeId__"), nodeItem.isOpen);
					}

					Text{
						id: nameText;

						anchors.verticalCenter: parent.verticalCenter;
						anchors.left: !nodeItem.hasType ? folderImage.left : folderImage.right;
						anchors.leftMargin: !nodeItem.hasType ? 0 : 16;

						width: nodeItem.width - parent.x - x - 20 - nodeItem.addItemWidth;
						elide: Text.ElideRight;

						font.family: Style.fontFamily;
						font.pixelSize: Style.fontSizeXL !== undefined ? Style.fontSizeXL : 18;
						color: treeViewPure.textColor;
						text: treeViewPure.itemText(nodeItem.node ? nodeItem.node.item : null);
					}
				}

				MouseArea{
					id: rowMouse;

					anchors.fill: parent;
					acceptedButtons: Qt.LeftButton | Qt.RightButton;
					hoverEnabled: visible;
					cursorShape: Qt.PointingHandCursor;

					onClicked: {
						if(mouse.x <= rowContent.x){
							return;
						}

						if(openButton.visible && mouse.x < openButton.width + 8 + rowContent.x){
							treeViewPure.toggleNode(nodeItem.node);
							return;
						}

						treeViewPure.selectNode(nodeItem.node);

						if (mouse.button === Qt.RightButton){
							let point = mapToItem(null, mouse.x, mouse.y);
							treeViewPure.rightButtonMouseClicked(point.x, point.y);
						}
					}

					onDoubleClicked: {
						if(mouse.x > openButton.width + 8 + rowContent.x){
							treeViewPure.doubleClicked(nodeItem.index);
						}
					}
				}
			}

			ListView{
				id: childList;

				x: 0;
				y: rowItem.height;
				width: parent.width;
				visible: nodeItem.childItems.length > 0;
				height: visible ? contentHeight : 0;

				interactive: false;
				cacheBuffer: 0;
				spacing: 0;
				clip: false;

				model: nodeItem.childItems;
				delegate: nodeComp;
			}
		}
	}

	ListView{
		id: rootList;

		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;

		anchors.rightMargin: treeViewPure.scrollSize;

		clip: true;
		boundsBehavior: Flickable.StopAtBounds;
		spacing: 0;
		model: treeViewPure.nodes;
		delegate: nodeComp;
		contentWidth: treeViewPure.delegateWidth + treeViewPure.maxLevel * treeViewPure.shift;
	}

	CustomScrollbar{
		id: scrollVert;

		anchors.right: parent.right;
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;

		targetItem: rootList;
		secondSize: treeViewPure.scrollSize;
		radius: secondSize;
		indicatorRadius: secondSize;
		indicatorMargin: 0;
	}

	function bump(){
		treeViewPure.revision = treeViewPure.revision + 1;
	}

	function sourceList(items){
		if (!items){
			return [];
		}
		if (items.items && items.items.length !== undefined){
			return items.items;
		}
		if (items.length !== undefined){
			return items;
		}
		return [];
	}

	function wrapItems(sources, level){
		let list = [];
		if (!sources || sources.length === undefined){
			return list;
		}
		for (let i = 0; i < sources.length; i++){
			list.push({
				item: sources[i],
				children: [],
				open: false,
				level: level,
				flatIndex: 0
			});
		}
		return list;
	}

	function rebuildFlat(){
		let flat = [];
		let deepest = 0;

		let walk = function(list, level){
			if (!list){
				return;
			}
			for (let i = 0; i < list.length; i++){
				let node = list[i];
				node.level = level;
				node.flatIndex = flat.length;
				flat.push(node);
				if (level > deepest){
					deepest = level;
				}
				if (node.children && node.children.length > 0){
					walk(node.children, level + 1);
				}
			}
		};

		walk(treeViewPure.nodes, 0);
		treeViewPure.flatNodes = flat;
		treeViewPure.maxLevel = deepest;
	}

	function nodeAt(index){
		if (!treeViewPure.flatNodes || index < 0 || index >= treeViewPure.flatNodes.length){
			return null;
		}
		return treeViewPure.flatNodes[index];
	}

	function fieldValue(item, key){
		if (!item || !key){
			return undefined;
		}
		if (item[key] !== undefined){
			return item[key];
		}
		if (key.indexOf("m_") === 0){
			let plain = key.slice(2);
			if (item[plain] !== undefined){
				return item[plain];
			}
		}
		if (key.length > 2 && key.slice(-2) === "__"){
			return item[key.slice(0, key.length - 2)];
		}
		return undefined;
	}

	function itemText(item){
		let value = treeViewPure.fieldValue(item, treeViewPure.nameId);
		if (value === undefined || value === null){
			return "";
		}
		return value;
	}

	function itemHasType(item){
		let value = treeViewPure.fieldValue(item, "typeId__");
		if (value === undefined || value === null || value === ""){
			return false;
		}
		return true;
	}

	function nodeHasChildren(node){
		if (!node){
			return false;
		}
		if (node.children && node.children.length > 0){
			return true;
		}
		let value = treeViewPure.fieldValue(node.item, "hasChildren__");
		if (value === undefined || value === null){
			return false;
		}
		return value;
	}

	function getData(key, index){
		let node = treeViewPure.nodeAt(index);
		if (!node){
			return undefined;
		}
		return treeViewPure.fieldValue(node.item, key);
	}

	function getModelItemsCount(model_){
		if (!model_ || !model_.getItemsCount){
			return 0;
		}
		return model_.getItemsCount();
	}

	function insertTree(index, items){
		let sources = treeViewPure.sourceList(items);
		if (index < 0){
			treeViewPure.nodes = treeViewPure.wrapItems(sources, 0);
		}
		else {
			let parent = treeViewPure.nodeAt(index);
			if (!parent){
				return;
			}
			parent.children = treeViewPure.wrapItems(sources, parent.level + 1);
			parent.open = true;
		}
		treeViewPure.rebuildFlat();
		treeViewPure.bump();
		treeViewPure.inserted(index);
		if (index >= 0){
			treeViewPure.forcedOpen(index);
		}
	}

	function toggleNode(node){
		if (!node || !treeViewPure.nodeHasChildren(node)){
			return;
		}

		if (node.open){
			node.open = false;
			treeViewPure.bump();
			treeViewPure.closeBranch(node.flatIndex);
			treeViewPure.openButtonClicked(node.flatIndex);
			return;
		}

		if (node.children && node.children.length > 0){
			node.open = true;
			treeViewPure.bump();
			treeViewPure.openBranch(node.flatIndex);
			treeViewPure.openButtonClicked(node.flatIndex);
			treeViewPure.forcedOpen(node.flatIndex);
			return;
		}

		node.open = true;
		treeViewPure.bump();
		treeViewPure.requestSignal(node.flatIndex);
		treeViewPure.openBranch(node.flatIndex);
		treeViewPure.openButtonClicked(node.flatIndex);
	}

	function openFunc(index){
		let node = treeViewPure.nodeAt(index);
		if (!node){
			return;
		}
		if (!treeViewPure.nodeHasChildren(node) || node.open){
			treeViewPure.forcedOpen(index);
			return;
		}
		treeViewPure.toggleNode(node);
	}

	function selectNode(node){
		if (!node){
			return;
		}
		if (treeViewPure.selectedIndex !== node.flatIndex){
			treeViewPure.selectedIndex = node.flatIndex;
			treeViewPure.selectionChanged();
		}
		treeViewPure.clicked(node.flatIndex);
	}

	function checkIsOpen(index){
		let node = treeViewPure.nodeAt(index);
		if (!node){
			return false;
		}
		return node.open;
	}

	function findIndexById(id, nameId, startIndex){
		if (nameId === undefined){
			nameId = "id";
		}
		if (startIndex === undefined){
			startIndex = 0;
		}
		let count = treeViewPure.flatNodes ? treeViewPure.flatNodes.length : 0;
		for (let i = startIndex; i < count; i++){
			let value = treeViewPure.getData(nameId, i);
			if (value == id){
				return i;
			}
		}
		return -1;
	}

	function findParentIndex(index){
		let node = treeViewPure.nodeAt(index);
		if (!node){
			return -1;
		}
		let count = treeViewPure.flatNodes.length;
		for (let i = 0; i < count; i++){
			let parent = treeViewPure.flatNodes[i];
			if (!parent.children){
				continue;
			}
			for (let c = 0; c < parent.children.length; c++){
				if (parent.children[c] === node){
					return parent.flatIndex;
				}
			}
		}
		return -1;
	}

	function moveToElement(index){
		let node = treeViewPure.nodeAt(index);
		if (!node){
			return;
		}
		let y = 0;
		let found = false;

		let walk = function(list){
			if (!list || found){
				return;
			}
			for (let i = 0; i < list.length; i++){
				let current = list[i];
				if (current === node){
					found = true;
					return;
				}
				y = y + treeViewPure.delegateHeight;
				if (current.open && current.children && current.children.length > 0){
					walk(current.children);
					if (found){
						return;
					}
				}
			}
		};

		walk(treeViewPure.nodes);
		let maxY = rootList.contentHeight - rootList.height;
		if (maxY < 0){
			maxY = 0;
		}
		if (y > maxY){
			rootList.contentY = maxY;
		}
		else {
			rootList.contentY = y;
		}
	}

	function getIcon(type, isOpen){
		let imageName = "Icons/New";
		if (type == "Node"){
			imageName = isOpen ? "Icons/FolderOpened" : "Icons/FolderClosed";
		}
		return "../../../" + Style.getIconPath(imageName, Icon.State.On, Icon.Mode.Normal);
	}
}
