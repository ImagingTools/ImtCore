import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0

Item {
	id: collectionViewBaseContainer;

	property alias commands: baseCommands;

	//    property alias gqlModelObjectView: baseCommands.gqlModelObjectView;
	//    property alias gqlModelHeadersInfo: baseCommands.gqlModelHeadersInfo;
	//    property alias gqlModelItemsInfo: baseCommands.gqlModelItemsInfo;

	property bool isWeb: Qt.platform.os == "web";

	property bool hasExtention: true;
	property bool openST: false;

	property alias gridElementsDelegate: gridInternal.delegate;
	property alias gridElementsModel: gridInternal.model;
	property alias gridCellWidth: gridInternal.cellWidth;
	property alias gridCellHeight: collectionViewBaseContainer.gridCellHeightMin;
	property alias gridContentHeight: gridInternal.contentHeight;
	property alias gridCacheBuffer: gridInternal.cacheBuffer;
	property alias gridContentY: gridInternal.contentY;
	property alias gridBottomMargin: backgroundTable.anchors.bottomMargin;

	property alias gridCount: gridInternal.count;
	property int gridCountInLine: Math.trunc(width/gridCellWidth);
	property int gridRowCount : Math.ceil(gridCount/gridCountInLine);
	property int gridSelectedRow: gridInternal.selectedIndex < 0 ? 0 : Math.trunc(gridInternal.selectedIndex/gridCountInLine);
	property bool gridIsLastRow: gridSelectedRow == gridRowCount -1;
	property int gridSelectedIndexInRow: gridSelectedRow < 0 ? 0 : gridInternal.selectedIndex - gridSelectedRow * gridCountInLine;
	property real gridAddHeight: 110*3;
	property int gridCellHeightMin: 110;

	property int gridCellHeightMax: gridRowCount == 0 ? gridCellHeightMin : (gridCellHeightMin * gridRowCount + gridAddHeight) / gridRowCount
														+ !isWeb * (gridIsLastRow * gridAddHeight - gridAddHeight / gridRowCount + 1);

	property int gridDelegateMargin: 10;

	property string gridIndicatorMainColor: Style.color_element;
	property alias gridIndicatorCoverColor: gridIndicatorCover.color;
	property alias gridIndicatorCoverOpacity: gridIndicatorCover.opacity;
	property bool gridIndicatorVisible: true;

	property alias extendingInfoComp: extendingInfoLoader.sourceComponent;

	property alias gridDecoratorPath: loaderTableDecorator.source;
	property alias modelFilter: modelFilterObj;
	property alias pagination: paginationObj;
	property int paginationCurrentIndex: paginationObj.currentIndex;
	property alias paginationPageSize: paginationObj.pagesSize;
	property alias paginationCountAllElements: paginationObj.countAllElements;
	property int paginationCountElements: paginationObj.countElements;
	property string commandsId;

	property alias grid: gridInternal;
	property int selectedIndex: gridInternal.selectedIndex;

	property bool hasPagination: true;

	property string backgroundColor: Style.baseColor;

	signal selectedItem(string id, string name);
	signal selectedIndexChangedSignal(int index);
	signal elementsChanged();


	Component.onCompleted: {
		console.log("CollectionViewBase onCompleted");

		gridInternal.focus = true;
	}

	onCommandsIdChanged: {
		baseCommands.commandId = commandsId;

		baseCommands.updateModels();
	}


	onGridSelectedRowChanged: {
		if(gridSelectedRow >=0){

			var selectedRowY = collectionViewBaseContainer.gridSelectedRow * collectionViewBaseContainer.gridCellHeightMin - gridInternal.contentY;
			var selectedRowBottomY = (collectionViewBaseContainer.gridSelectedRow + 1) * collectionViewBaseContainer.gridCellHeightMin - gridInternal.contentY;
			var infonYBottom = (collectionViewBaseContainer.gridSelectedRow + 1) * collectionViewBaseContainer.gridCellHeightMin
					- gridInternal.contentY +
					collectionViewBaseContainer.gridAddHeight;

			var diff = infonYBottom - gridInternal.height;

			var contY = gridInternal.contentY;

			if((collectionViewBaseContainer.gridAddHeight + collectionViewBaseContainer.gridCellHeightMin) < gridInternal.height){
				if(diff > 0){
					contY += diff;
					gridInternal.contentY = contY;

				}
			}

			else {
				gridInternal.contentY = collectionViewBaseContainer.gridSelectedRow  * collectionViewBaseContainer.gridCellHeightMin;
			}

		}
	}

	onPaginationCurrentIndexChanged: {
		paginationObj.currentIndex = paginationCurrentIndex;
	}

	onPaginationCountElementsChanged: {
		 paginationObj.countElements = paginationCountElements;
	}

	onSelectedIndexChanged: {
		gridInternal.selectedIndex = selectedIndex;
	}

	Rectangle {
		id: backgroundTable;

		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: 50;

		//        anchors.top: filterMenu.visible ? filterMenu.bottom: parent.top;
		//        anchors.left: parent.left;
		//        width: gridInternal.minWidth * gridInternal.columnCount < parent.width ? gridInternal.minWidth * gridInternal.columnCount : parent.width;
		//        anchors.bottom: parent.bottom;

		color: collectionViewBaseContainer.backgroundColor;

		radius: 10;

		TreeItemModel{
			id: gridDecoratorModel;

			property bool compl: false;
			property bool complCompl: gridDecoratorModel.compl && loaderTableDecorator.compl && gridInternal.headersCompl;
			Component.onCompleted: {
				gridDecoratorModel.compl = true;
			}
			onComplComplChanged: {
				if(gridDecoratorModel.complCompl){

					var tableDecorator = loaderTableDecorator.item;
					var count = gridInternal.columnCount;

					if(tableDecorator.getItemsCount()){

						var headers = gridDecoratorModel.addTreeModel("Headers");
						var cells = gridDecoratorModel.addTreeModel("Cells");
						var cellWidth = gridDecoratorModel.addTreeModel("CellWidth");

						for(var i = 0; i < count; i++){
							headers.insertNewItem();
							cells.insertNewItem();
							cellWidth.insertNewItem();
							headers.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("Headers"));
							cells.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("Cells"));
							cellWidth.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("CellWidth"));
						}

						//gridInternal.tableDecorator = gridDecoratorModel;
					}
				}
			}
		}

		Loader{
			id: loaderTableDecorator;

			property bool compl: false;
			onLoaded: {
				if(loaderTableDecorator.item){
					loaderTableDecorator.compl = true;
				}
			}
		}


		Item{
			id: gridFrame;

			width: gridInternal.width;
			height: gridInternal.height;

			property real contentWidth: gridInternal.contentWidth;
			property real contentY: gridInternal.contentY;
			property real originY: 0;
			property real contentHeight: collectionViewBaseContainer.openST ? collectionViewBaseContainer.gridRowCount * collectionViewBaseContainer.gridCellHeightMin + collectionViewBaseContainer.gridAddHeight
																			  //+ !collectionViewBaseContainer.isWeb  * (collectionViewBaseContainer.gridAddHeight / collectionViewBaseContainer.gridRowCount)
																			: collectionViewBaseContainer.gridRowCount * collectionViewBaseContainer.gridCellHeightMin;


		}

		GridView {
			id: gridInternal;

			anchors.fill: parent;

			boundsBehavior: Flickable.StopAtBounds;
			clip: true;
			//cacheBuffer: collectionViewBaseContainer.gridRowCount * collectionViewBaseContainer.gridCellHeightMin + collectionViewBaseContainer.gridAddHeight;
			cellWidth: collectionViewBaseContainer.gridCellHeightMin;
			cellHeight: !collectionViewBaseContainer.hasExtention ? collectionViewBaseContainer.gridCellHeightMin : !collectionViewBaseContainer.openST ? collectionViewBaseContainer.gridCellHeightMin : collectionViewBaseContainer.gridCellHeightMax;

			property int selectedIndex: -1;
			//            property alias elements: model;
			signal selectItem(string idSelected, string name);


			onContentYChanged: {
				gridFrame.contentY = contentY;
			}

			onContentHeightChanged: {
				//console.log("GRID::CONTENT_Height:: ", contentHeight, ", must be:: " ,collectionViewBaseContainer.gridRowCount * collectionViewBaseContainer.gridCellHeightMin + collectionViewBaseContainer.gridAddHeight * collectionViewBaseContainer.openST, ", addHeight:: " , collectionViewBaseContainer.gridAddHeight)
			}
			onCellHeightChanged: {
				//console.log("GRID::Cell_Height:: ", cellHeight, height)
			}

			onSelectItem: {
				console.log("MEGATEST1")
				collectionViewBaseContainer.selectedItem(idSelected, name);
			}

			onSelectedIndexChanged: {
				console.log("CollectionView GridView onSelectedIndexChanged");
				collectionViewBaseContainer.selectedIndexChangedSignal(gridInternal.selectedIndex);
				collectionViewBaseContainer.selectedIndex = selectedIndex;
			}

			//            onElementsChanged: {
			//                collectionViewBaseContainer.elementsChanged();
			//            }

			//            cellWidth: 160
			//            cellHeight: 160

			//            anchors.bottom: pagination.visible ? pagination.top : parent.bottom;
			//            anchors.margins: thumbnailDecoratorContainer.mainMargin;
			//            property real minWidth: 1000000;
		}

		//        Rectangle{//TEST!!!
		//            anchors.fill: gridInternal;
		//            color: "transparent"
		//            border.color: "red"
		//        }

	}

	CustomScrollbar{
		id: scrollBar;

		anchors.right: parent.right;
		anchors.rightMargin: 10;
		anchors.bottom: backgroundTable.bottom;

		targetItem: gridFrame;

		secondSize: 8;
		radius: secondSize;
		indicatorRadius: secondSize;
		indicatorMargin: 0;

		z: 20;
		onContentYSignal:{
			gridInternal.contentY = contentY;
		}

	}

	Item{
		id: extendingInfoContainer;

		width: parent.width;
		height: parent.height - collectionViewBaseContainer.gridBottomMargin;

		clip: true;
		Item{
			id: extendingInfoLoaderContainer;

			anchors.top: parent.top;
			anchors.topMargin: collectionViewBaseContainer.gridCellHeightMin * (collectionViewBaseContainer.gridSelectedRow + 1)  - collectionViewBaseContainer.gridContentY;

			width: collectionViewBaseContainer.gridCellWidth * collectionViewBaseContainer.gridCountInLine - 10;
			height: collectionViewBaseContainer.gridAddHeight - 10;

			visible: collectionViewBaseContainer.openST;

			onYChanged: {
				if(extendingInfoLoader.item && extendingInfoLoader.item.parentY !==undefined){
					extendingInfoLoader.item.parentY = y;
				}
			}


			Loader{
				id: extendingInfoLoader;

				anchors.fill: parent;

				sourceComponent: extendingInfoCompDefault;

				//                onItemChanged:  {
				//                    if(extendingInfoLoader.item && extendingInfoLoader.item.border.color !== undefined){
				//                        console.log("extendingInfoLoader.item.border.color:: ", extendingInfoLoader.item.border.color);
				//                        //indicatorRec.borderColor = extendingInfoLoader.item.border.color;
				//                    }
				//                }

			}
		}

		Rectangle{
			id: indicatorRec;

			anchors.bottom:extendingInfoLoaderContainer.top;
			anchors.bottomMargin: -borderWidth;

			x: collectionViewBaseContainer.gridSelectedIndexInRow * collectionViewBaseContainer.gridCellWidth  + 0.5 * collectionViewBaseContainer.gridCellWidth - width/2 - collectionViewBaseContainer.gridDelegateMargin/2;

			width: 80;
			height: collectionViewBaseContainer.gridDelegateMargin + borderWidth; //- 2 * anchors.bottomMargin;
			color: "transparent";
			visible: collectionViewBaseContainer.openST && collectionViewBaseContainer.gridIndicatorVisible;
			//clip: true;
			property int borderWidth: 2;
			property string borderColor: extendingInfoLoader.item ? extendingInfoLoader.item.border.color : "transparent";



			Rectangle{
				id: gridIndicator;

				width: parent.width;
				height: parent.height;

				color: "transparent";
				clip: true;

				Canvas {
					id: canvas

					anchors.fill: parent;

					antialiasing: true;
					property int gap: 30
					property string borderColor: indicatorRec.borderColor
					onBorderColorChanged: {
						requestPaint()
					}
					onPaint: {
						var ctx = canvas.getContext('2d');
						ctx.reset();
						ctx.clearRect(0, 0, canvas.width, canvas.height);

						ctx.lineCap = "round"
						ctx.lineJoin = "round"
						ctx.lineWidth = indicatorRec.borderWidth +1;
						ctx.strokeStyle = indicatorRec.borderColor;
						ctx.fillStyle = collectionViewBaseContainer.gridIndicatorMainColor;

						ctx.beginPath()

						ctx.moveTo(0, canvas.height);
						let gapX = (canvas.width - canvas.gap)/2
						let ctrX1 = 0.75 * gapX;
						let ctrY1 = 0.75 * canvas.height;
						ctx.quadraticCurveTo(ctrX1 ,ctrY1,(canvas.width - canvas.gap)/2, 0)

						ctx.lineTo((canvas.width + canvas.gap)/2, 0);
						let ctrX2 = canvas.width  - ctrX1;
						let ctrY2 = ctrY1;
						ctx.quadraticCurveTo(ctrX2, ctrY2,canvas.width, canvas.height);

						ctx.lineTo(0, canvas.height);

						ctx.stroke();
						ctx.fill();
					}
				}

				Rectangle{
					id: gridIndicatorCover;

					visible: false;
					anchors.fill: parent;
					color: Style.textColor;
					opacity: 0.2;

				}

			}

			Rectangle{
				id: topPatch;

				anchors.horizontalCenter: gridIndicator.horizontalCenter;
				anchors.bottom: gridIndicator.top;
				anchors.bottomMargin:0;

				height: 4;
				width: 30;
				color: collectionViewBaseContainer.gridIndicatorMainColor;

			}

		}

	}

	Component{
		id: extendingInfoCompDefault;

		Rectangle{
			color: "#ffffff";
			border.color: "gray";
			border.width: 2;

		}

	}

	TreeItemModel {
		id: modelFilterObj;

		Component.onCompleted: {
			modelFilterObj.addTreeModel("FilterIds");
			modelFilterObj.addTreeModel("Sort");
		}
	}

	Pagination {
		id: paginationObj;

		anchors.bottom: parent.bottom;
		anchors.bottomMargin: 10;
		anchors.horizontalCenter: parent.horizontalCenter;

		pagesSize: 1;

		visible: collectionViewBaseContainer.hasPagination && pagesSize > 1;


		onCurrentIndexChanged: {
			collectionViewBaseContainer.paginationCurrentIndex = currentIndex;
		}

		onCurrentValueChanged: {
			console.log("Pagination onCurrentValueChanged", paginationObj.currentValue);
			gridInternal.selectedIndex = -1;
			if(this.commandsId){
				baseCommands.updateModels();
			}
			collectionViewBaseContainer.openST = false;
		}
		onCountElementsChanged: {
			collectionViewBaseContainer.paginationCountElements = countElements;

			console.log("PAGINATION::Pagination onCountElementsChanged", paginationObj.countElements);
			gridInternal.selectedIndex = -1;
			if(this.commandsId){
				baseCommands.updateModels();
			}
			collectionViewBaseContainer.openST = false;
		}
	}

	CollectionViewBaseGqlModels {
		id: baseCommands;

		table: collectionViewBaseContainer.table;

		rootItem: collectionViewBaseContainer;
		commandId: collectionViewBaseContainer.commandsId;
		pagination: paginationObj;

		Component.onCompleted: {
			console.log("CollectionViewBaseGqlModels onCompleted")
		}


		onCommandIdChanged: {
			console.log("CollectionViewBase onCommandsIdChanged");

			baseCommands.updateModels();
		}

		onItemsChanged: {
			console.log("baseCommands.items - ", baseCommands.items);
			console.log("baseCommands.items.toJson - ", baseCommands.items.toJson());
			gridInternal.model = baseCommands.items;
		}
	}
}
