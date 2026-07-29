import QtQuick 2.12
import Qt5Compat.GraphicalEffects

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
// SDL imports
import imtbaseImtCollectionSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0


Item {
	id: structureAddressesContainer;
	anchors.fill: parent;

	property TreeItemModel addressModel: TreeItemModel{};
	property TreeItemModel selectedItemAddressModel: TreeItemModel{};
	property TreeItemModel addedItemAddressModel: TreeItemModel{};

	property var addressTypeNames: [qsTr("Country"), qsTr("Region"), qsTr("Area"),
		qsTr("City"), qsTr("Settlement"), qsTr("District"),
		qsTr("Street"), qsTr("Building"), qsTr("Apartment"),
		qsTr("Section"), qsTr("SNT"), qsTr("Alley"), qsTr("Other")]

	property string parentId: "";
	property string parentName: "";
	property bool isEdit: false;

	property string addressListCommandId: "AddressList"
	property string searchNameId: "fullAddress"

	property string idParam: "id"
	property string nameParam: "name"
	property string fullAddressParam: "fullAddress"
	property string latitudeParam: "latitude"
	property string longitudeParam: "longitude"
	property string typeIdParam: "typeId"
	property string parentIdsParam: "parentIds"
	property string hasChildrenParam: "hasChildren__"
	property string textColor: Style.firstColor

	property alias searchComp: treeBody.searchComp
	property alias addressTreeRequest: treeBody.addressTreeRequest
	property alias treeView: treeBody.treeView
	property alias treeLoading: treeBody.treeLoading
	property alias treeBody: treeBody

	property Component addressTreeInputObjectComp: null
	property Component addressTreeSdlObjectComp: null

	property GqlRequestSender addAddressRequest: GqlRequestSender{
		gqlCommandId: "AddressAdd";

		function createQueryParams(query){
			var queryFields = Gql.GqlObject("addedNotification");
			queryFields.InsertField("id");

			var inputParams = Gql.GqlObject("input");

			var jsonString = structureAddressesContainer.addedItemAddressModel.toJson();
			inputParams.InsertField("item", jsonString);
			inputParams.InsertField("collectionId", "Address");
			query.AddParam(inputParams);
			query.AddField(queryFields);
		}

		function onResult(data){
			if (data.containsKey("addedNotification")){
				let items  = data.getTreeItemModel("addedNotification");
				let id = items.getData("id")
				structureAddressesContainer.parentId = id
				let addressCount = structureAddressesContainer.addressModel.getItemsCount()
				structureAddressesContainer.addressModel.setData("itemId", id, addressCount-1);
				addressView.model = 0;
				addressView.model = structureAddressesContainer.addressModel;
			}
		}
	}

	property GqlRequestSender updateAddressRequest: GqlRequestSender{
		gqlCommandId: "AddressUpdate";

		function createQueryParams(query){
			var queryFields = Gql.GqlObject("updatedNotification");
			queryFields.InsertField("id");

			var inputParams = Gql.GqlObject("input");

			var jsonString = structureAddressesContainer.selectedItemAddressModel.toJson();
			inputParams.InsertField("id", structureAddressesContainer.selectedItemAddressModel.getData("itemId", 0));
			inputParams.InsertField("item", jsonString);
			inputParams.InsertField("collectionId", "Address");
			query.AddParam(inputParams);
			query.AddField(queryFields);
		}
	}

	property GqlSdlRequestSender removeAddressRequest: GqlSdlRequestSender{
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_removeElements
		requestType: 1
	}

	function getAddressTypeName(index){
		if (index > -1 && index < 13){
			return structureAddressesContainer.addressTypeNames[index]
		}
		else {
			return qsTr("Other")
		}
	}

	function setCustomFilters(inputParams){
		return
	}

	function setDataToAddressModel(typeId, typeValue, nameValue, itemId, parentId, latitude, longitude, index){
		structureAddressesContainer.addressModel.setData("typeId", typeId !== null ? typeId : 0, index);
		structureAddressesContainer.addressModel.setData("typeValue", typeValue !== null ? typeValue : "", index);
		structureAddressesContainer.addressModel.setData("nameValue", nameValue !== null ? nameValue : "", index);
		structureAddressesContainer.addressModel.setData("itemId", itemId !== null ? itemId : "", index);
		structureAddressesContainer.addressModel.setData("parentId", parentId !== null ? parentId : "", index);
		structureAddressesContainer.addressModel.setData("latitude", latitude !== null ? latitude : "", index);
		structureAddressesContainer.addressModel.setData("longitude", longitude !== null ? longitude : "", index);
	}

	Component {
		id: editAddressDialogComp

		Dialog{
			id: editAddressDialog
			width: 1000;
			height: 700;
			radius:8;

			canMove: false;

			property bool isEdit: structureAddressesContainer.isEdit
			property var model: isEdit ? structureAddressesContainer.selectedItemAddressModel : structureAddressesContainer.addedItemAddressModel


			Component.onCompleted: {
				let hasName = false;
				if (model) {
					let nameVal = model.getData("name", 0);
					if (nameVal && nameVal.trim().length > 0) {
						hasName = true;
					}
				}
				addButton(Enums.save, qsTr("Save"), hasName);
				addButton(Enums.close, qsTr("Close"), true);
			}

			contentComp: Component {
				Item {
					width: editAddressDialog.width;
					height: editAddressDialog.height - 100;

					clip: true;

					CustomScrollbar {
						id: scrollbar;

						z: parent.z + 1;

						anchors.right: parent.right;
						anchors.top: flickable.top;
						anchors.bottom: flickable.bottom;

						secondSize: 10;
						targetItem: flickable;

						radius: 2;
					}

					Flickable {
						id: flickable;
						anchors.left: parent.left;
						anchors.leftMargin: Style.marginXL;
						anchors.right: parent.right
						anchors.rightMargin: Style.marginXL;
						height: parent.height;

						contentHeight: addressEditor.height + Style.marginXL;
						contentWidth: width;

						AddressEditor {
							id: addressEditor;
							anchors.top: parent.top
							anchors.topMargin: Style.marginXL
							width: flickable.width;
							model: editAddressDialog.model

							Component.onCompleted: {
								editAddressDialog.setButtonEnabled(Enums.save, addressEditor.isValid);
							}

							onIsValidChanged: {
								editAddressDialog.setButtonEnabled(Enums.save, addressEditor.isValid);
							}
						}
					}
				}
			}

			onFinished: {
				if (buttonId === Enums.save){
					let typeId = this.model.getData("typeId", 0)
					let typeValue = this.model.getData("typeValue", 0)
					let nameValue = this.model.getData("name", 0)
					let itemId = this.model.getData("itemId", 0)
					let parentId = this.model.getData("parentId", 0)
					let latitude = this.model.getData("latitude", 0)
					let longitude = this.model.getData("longitude", 0)

					if (!this.isEdit){
						var index = structureAddressesContainer.addressModel.insertNewItem();
						structureAddressesContainer.setDataToAddressModel(typeId, typeValue, nameValue, itemId, parentId, latitude, longitude, index)
						structureAddressesContainer.addAddressRequest.send();
					}
					else{
						structureAddressesContainer.setDataToAddressModel(typeId, typeValue, nameValue, itemId, parentId, latitude, longitude, addressColumn.currentIndex)
						structureAddressesContainer.updateAddressRequest.send();
						addressView.model = 0;
						addressView.model = structureAddressesContainer.addressModel;
					}
				}
			}
		}
	}

	RemoveElementsInput {
		id: removeElementsInput
		m_collectionId: "Address"
	}

	AddressTreeView{
		id: treeBody

		anchors.top: parent.top;
		anchors.bottom: parent.bottom;
		anchors.left: parent.left;
		anchors.leftMargin: Style.marginM;
		anchors.topMargin: Style.marginM;
		anchors.bottomMargin: Style.marginM;

		width: 0.25 * structureAddressesContainer.width + 10;

		textColor: structureAddressesContainer.textColor

		addressListCommandId: structureAddressesContainer.addressListCommandId
		searchNameId: structureAddressesContainer.searchNameId
		idParam: structureAddressesContainer.idParam
		nameParam: structureAddressesContainer.nameParam
		fullAddressParam: structureAddressesContainer.fullAddressParam
		latitudeParam: structureAddressesContainer.latitudeParam
		longitudeParam: structureAddressesContainer.longitudeParam
		typeIdParam: structureAddressesContainer.typeIdParam
		parentIdsParam: structureAddressesContainer.parentIdsParam
		hasChildrenParam: structureAddressesContainer.hasChildrenParam
		addressTreeInputObjectComp: structureAddressesContainer.addressTreeInputObjectComp
		addressTreeSdlObjectComp: structureAddressesContainer.addressTreeSdlObjectComp

		onTreeViewSelectedIndexChanged: {
			updateAddressColumn(index)
		}

		onTreeClicked: {
			let parentIds	= treeBody.treeViewModel.getData(treeBody.parentIdsParam, index);
			let address		= treeBody.treeViewModel.getData(treeBody.fullAddressParam, index);
			treeBody.setExternalAddress(address, parentIds);
		}

		function setCustomFilters(inputParams){
			structureAddressesContainer.setCustomFilters(inputParams)
		}

		function updateAddressColumn(index){
			if (index < 0){
				structureAddressesContainer.addressModel.clear();
				addressView.model = 0;
				addressView.model = structureAddressesContainer.addressModel
				structureAddressesContainer.parentId = "";
				structureAddressesContainer.parentName = "";
				addressColumn.currentIndex = -1;
			}
			else{
				let lat			= treeBody.treeViewModel.getData(treeBody.latitudeParam, index);
				let lon			= treeBody.treeViewModel.getData(treeBody.longitudeParam, index);
				let id			= treeBody.treeViewModel.getData(treeBody.idParam, index);
				let name		= treeBody.treeViewModel.getData(treeBody.nameParam, index);
				let typeId		= treeBody.treeViewModel.getData(treeBody.typeIdParam, index);
				let parentIds	= treeBody.treeViewModel.getData(treeBody.parentIdsParam, index);
				let address		= treeBody.treeViewModel.getData(treeBody.fullAddressParam, index);
				let hasChildren = treeBody.treeViewModel.getData(treeBody.hasChildrenParam, index);

				let countAddress = structureAddressesContainer.addressModel.getItemsCount();

				if (countAddress > 0){
					structureAddressesContainer.addressModel.clear();
					structureAddressesContainer.parentId = "";
					structureAddressesContainer.parentName = "";
					addressColumn.currentIndex = -1;
				}

				let parentList = [];
				if (parentIds !== "" && parentIds !== undefined){
					parentList = parentIds.split(',');
					if (parentList.length > 0){
						for (let parentItem of parentList){
							let indexParent = treeBody.treeView.findIndexById(parentItem);
							var indexModel = structureAddressesContainer.addressModel.insertNewItem();
							let parentId = treeBody.treeView.getData(treeBody.idParam, indexParent);
							let parentName = treeBody.treeView.getData(treeBody.nameParam, indexParent);
							let parentTypeId = treeBody.treeView.getData(treeBody.typeIdParam, indexParent)
							let parentTypeName = structureAddressesContainer.getAddressTypeName(parentTypeId);
							let parentLatitude = treeBody.treeView.getData(treeBody.latitudeParam, indexParent)
							let parentLongitude = treeBody.treeView.getData(treeBody.longitudeParam, indexParent)
							structureAddressesContainer.addressModel.setData("typeId", parentTypeId, indexModel);
							structureAddressesContainer.addressModel.setData("typeValue", parentTypeName, indexModel);
							structureAddressesContainer.addressModel.setData("nameId", "1", indexModel);
							structureAddressesContainer.addressModel.setData("nameValue", parentName, indexModel);
							structureAddressesContainer.addressModel.setData("itemId", parentId, indexModel);
							if (indexModel == 0){
								structureAddressesContainer.addressModel.setData("parentId", "", indexModel);
							}
							else{
								structureAddressesContainer.addressModel.setData("parentId", parentList[indexModel-1], indexModel);
							}

							structureAddressesContainer.addressModel.setData("latitude", parentLatitude, indexModel);
							structureAddressesContainer.addressModel.setData("longitude", parentLongitude, indexModel);
						}
					}
				}
				var indexCurrent = structureAddressesContainer.addressModel.insertNewItem();
				structureAddressesContainer.addressModel.setData("typeId", typeId, indexCurrent);
				let typeValue = structureAddressesContainer.getAddressTypeName(typeId);
				structureAddressesContainer.addressModel.setData("typeValue", typeValue, indexCurrent);
				structureAddressesContainer.addressModel.setData("nameId", "1", indexCurrent);
				structureAddressesContainer.addressModel.setData("nameValue", name, indexCurrent);
				structureAddressesContainer.addressModel.setData("itemId", id, indexCurrent);
				if (indexCurrent == 0){
					structureAddressesContainer.addressModel.setData("parentId", "", indexCurrent);
				}
				else{
					structureAddressesContainer.addressModel.setData("parentId", structureAddressesContainer.addressModel.getData("itemId", indexCurrent-1), indexCurrent);
				}

				structureAddressesContainer.addressModel.setData("latitude", lat, indexCurrent);
				structureAddressesContainer.addressModel.setData("longitude", lon, indexCurrent);
				structureAddressesContainer.parentName = typeValue + " " + name;
				structureAddressesContainer.parentId = id;

				addressView.model = 0;
				addressView.model = structureAddressesContainer.addressModel;
				addressColumn.currentIndex = addressView.model.count - 1
			}
		}
	}

	Rectangle{
		id: addressColumnRec;

		anchors.left: treeBody.right;
		anchors.top: treeBody.top;
		anchors.leftMargin: Style.marginM;

		width: addressColumn.width + 20;
		height: addressView.count > 0 ? addressColumn.height + 2*addressColumn.anchors.topMargin : 0;
		radius: Style.size_mainCornerRadius;
		border.color: Style.color_shadow_light;
		border.width: 1;
		visible: addressColumn.currentIndex > -1

		Column{
			id: addressColumn;

			anchors.horizontalCenter: parent.horizontalCenter;
			anchors.top: parent.top;
			anchors.topMargin: Style.marginXS;

			width: 200;
			spacing: 10;
			property int currentIndex: -1;

			onCurrentIndexChanged: {
				if (currentIndex > -1){
					let typeId = structureAddressesContainer.addressModel.getData("typeId", currentIndex)
					let typeValue = structureAddressesContainer.addressModel.getData("typeValue", currentIndex)
					let nameValue = structureAddressesContainer.addressModel.getData("nameValue", currentIndex)
					let itemId = structureAddressesContainer.addressModel.getData("itemId", currentIndex)
					let parentId = structureAddressesContainer.addressModel.getData("parentId", currentIndex)
					let latitude = structureAddressesContainer.addressModel.getData("latitude", currentIndex)
					let longitude = structureAddressesContainer.addressModel.getData("longitude", currentIndex)
					structureAddressesContainer.selectedItemAddressModel.clear()
					structureAddressesContainer.selectedItemAddressModel.insertNewItem()
					structureAddressesContainer.selectedItemAddressModel.setData("typeId", typeId, 0)
					structureAddressesContainer.selectedItemAddressModel.setData("typeValue", typeValue, 0)
					structureAddressesContainer.selectedItemAddressModel.setData("name", nameValue, 0)
					structureAddressesContainer.selectedItemAddressModel.setData("itemId", itemId, 0)
					structureAddressesContainer.selectedItemAddressModel.setData("parentId", parentId, 0)
					structureAddressesContainer.selectedItemAddressModel.setData("latitude", latitude, 0)
					structureAddressesContainer.selectedItemAddressModel.setData("longitude", longitude, 0)
				}

			}

			ListView{
				id: addressView;

				width: parent.width;
				height: Math.min(contentHeight, structureAddressesContainer.height - 2*addressColumnRec.anchors.topMargin);
				spacing: 10;
				delegate: Rectangle{
					id: addressItem;

					width: addressColumn.width
					height: recWithElement.height + 2 * (recWithElement.anchors.topMargin);
					property bool selected: addressColumn.currentIndex === model.index;
					radius: 10;
					Column{
						id: recWithElement;

						anchors.left: parent.left;
						anchors.top: parent.top;
						anchors.leftMargin: 5;
						anchors.topMargin: 5;

						width: parent.width - 2 * (recWithElement.anchors.leftMargin)
						spacing: 10;
						Text {
							width: parent.width;

							color: Style.color_dialogText;
							font.family: Style.fontFamily;
							font.pixelSize: Style.fontSizeM;

							elide: Text.ElideRight;
							wrapMode: Text.WordWrap;

							text: model.typeValue !== undefined ? model.typeValue : "";
						}

						Rectangle{
							width: parent.width;
							height: Math.max(40, addressText.height + 10);
							radius: 5;
							color: addressItem.selected ? Style.secondColor : "transparent";

							Text {
								id: addressText;

								anchors.verticalCenter: parent.verticalCenter;
								anchors.left: parent.left;
								anchors.leftMargin: 4;

								verticalAlignment: Text.AlignVCenter;

								width: parent.width - anchors.leftMargin;

								color: Style.color_text_common;
								font.family: Style.fontFamily;
								font.pixelSize: Style.fontSizeL;

								elide: Text.ElideRight;
								wrapMode: Text.WordWrap;

								text: model.nameValue !== undefined ? model.nameValue : "";
							}

						}

					}
					MouseArea{
						id: ma;

						anchors.fill: parent;

						onClicked: {
							addressColumn.currentIndex = model.index;
						}
					}
				}
			}
		}
	}

	Rectangle{
		id: buttonsColumnRec;

		anchors.left: addressColumnRec.right;
		anchors.top: addressColumnRec.top;
		anchors.leftMargin: Style.marginM;

		width: buttonsColumn.width + 20;
		height: buttonsColumn.height + 2*buttonsColumn.anchors.topMargin;
		radius: Style.size_mainCornerRadius;
		border.color: Style.color_shadow_light;
		border.width: 1;

		Column{
			id: buttonsColumn;

			anchors.horizontalCenter: parent.horizontalCenter;
			anchors.top: parent.top;
			anchors.topMargin: Style.marginXS;

			width: 200;

			spacing: 10;
			ListModel{
				id: buttonsModel;

				ListElement{
					ids: "Add";
					text: qsTr("Add");
				}
				ListElement{
					ids: "Edit";
					text: qsTr("Edit");
				}
				ListElement{
					ids: "Delete"
					text: qsTr("Delete");
				}
				ListElement{
					ids: "clear";
					text: qsTr("Clear");
				}
				// ListElement{
				// 	ids: "addHouse";
				// 	text: qsTr("Add house range");
				// }
				// ListElement{
				// 	ids: "addApartment";
				// 	text: qsTr("Add apartment range");
				// }
			}
			Repeater{
				id: buttons;

				model: buttonsModel;
				delegate: Button{

					width: 200;
					height: Style.size_ButtonHeight;

					text: model.text;

					onClicked: {
						var parameters = {"title": model.text};
						if (model.ids == "Edit"){
							if(addressColumn.currentIndex > -1){
								structureAddressesContainer.isEdit = true
								ModalDialogManager.openDialog(editAddressDialogComp, parameters);
							}
						}
						if(model.ids == "Add"){
							addressColumn.currentIndex = addressView.count - 1
							structureAddressesContainer.isEdit = false
							structureAddressesContainer.addedItemAddressModel.clear()
							structureAddressesContainer.addedItemAddressModel.insertNewItem()
							structureAddressesContainer.addedItemAddressModel.setData("typeId", 0, 0)
							structureAddressesContainer.addedItemAddressModel.setData("typeValue", "", 0)
							structureAddressesContainer.addedItemAddressModel.setData("name", "", 0)
							structureAddressesContainer.addedItemAddressModel.setData("itemId", "", 0)
							structureAddressesContainer.addedItemAddressModel.setData("parentId", structureAddressesContainer.parentId, 0)
							structureAddressesContainer.addedItemAddressModel.setData("latitude", "", 0)
							structureAddressesContainer.addedItemAddressModel.setData("longitude", "", 0)
							ModalDialogManager.openDialog(editAddressDialogComp, parameters);
						}
						if (model.ids == "Delete"){

							if(addressColumn.currentIndex > -1){
								let dialogCallback = function(result){
									if (result === Enums.yes){
										if (addressColumn.currentIndex <= 0){
											structureAddressesContainer.parentId = "";
											structureAddressesContainer.parentName = "";
										}
										else{
											structureAddressesContainer.parentId = structureAddressesContainer.addressModel.getData("itemId", addressColumn.currentIndex-1);
											structureAddressesContainer.parentName = structureAddressesContainer.addressModel.getData("typeValue", addressColumn.currentIndex-1) + " " + structureAddressesContainer.addressModel.getData("nameValue", addressColumn.currentIndex-1);
										}
										
										let deleteId = structureAddressesContainer.addressModel.getData("itemId", addressColumn.currentIndex);
										removeElementsInput.m_elementIds = [deleteId];

										structureAddressesContainer.removeAddressRequest.send(removeElementsInput);
									}
								}
								ModalDialogManager.showConfirmationDialog(qsTr("Remove address"), qsTr("Do you want remove the address?"), dialogCallback)
							}
						}
						if (model.ids == "clear"){
							structureAddressesContainer.addressModel.clear();
							structureAddressesContainer.parentId = "";
							structureAddressesContainer.parentName = "";
							addressColumn.currentIndex = -1;
							treeBody.treeView.selectedIndex = -1;
						}
						// if(model.ids == "addHouse"){
						// 	parameters["parentName"] = qsTr("street");
						// 	parameters["parentTypeName"] = "street";
						// 	parameters["parentId"] = structureAddressesContainer.addressModel.getData("parentId", addressColumn.currentIndex);
						// 	ModalDialogManager.openDialog(addAddressRangeDialog, parameters);
						// }

						// if(model.ids == "addApartment"){
						// 	parameters["parentTypeName"] = qsTr("house");
						// 	parameters["parentId"] = structureAddressesContainer.addressModel.getData("parentId", addressColumn.currentIndex);
						// 	ModalDialogManager.openDialog(addAddressRangeDialog, parameters);
						// }
					}
				}
			}
		}
	}

	Rectangle{
		id: coordinatesColumnRec;
		anchors.left: buttonsColumnRec.right;
		anchors.top: buttonsColumnRec.top;
		anchors.leftMargin: Style.marginM;

		width: coordinatesColumn.width + 20;
		height: coordinatesColumn.height + 2 * Style.marginM;
		radius: Style.size_mainCornerRadius;
		border.color: Style.color_shadow_light;
		border.width: 1;
		visible: addressColumn.currentIndex > -1;

		Column{
			id: coordinatesColumn;
			anchors.horizontalCenter: parent.horizontalCenter;
			anchors.top: parent.top;
			anchors.topMargin: Style.marginXS;

			width: 200;
			spacing: 10;

			Text {
				width: parent.width;

				color: Style.lightBlueColor;
				font.pixelSize: Style.fontSizeL;
				font.family: Style.fontFamilyBold;
				font.bold: true;

				elide: Text.ElideRight;
				wrapMode: Text.WordWrap;

				text: qsTr("Coordinates:")
			}

			Text {
				width: parent.width;

				color: Style.color_dialogText;
				font.family: Style.fontFamily;
				font.pixelSize: Style.fontSizeM;

				elide: Text.ElideRight;
				wrapMode: Text.WordWrap;

				text: qsTr("Latitude: ") + structureAddressesContainer.addressModel.getData("latitude", addressColumn.currentIndex);
			}
			Text {
				width: parent.width;

				color: Style.color_dialogText;
				font.family: Style.fontFamily;
				font.pixelSize: Style.fontSizeM;

				elide: Text.ElideRight;
				wrapMode: Text.WordWrap;

				text: qsTr("Longitude: ") + structureAddressesContainer.addressModel.getData("longitude", addressColumn.currentIndex);
			}
		}
	}
}
