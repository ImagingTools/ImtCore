import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0


/**
    Dialog for browsing the folder hierarchy provided by the server-side
    file system controller ('GetFileSystemEntries' GraphQL query).

    Usage example:

    FileSystemBrowserDialog {
        startPath: "";
        onPathSelected: {
            console.log("Selected path:", selectedPath);
        }
    }
*/

Dialog {
	id: root;

	title: qsTr("Select Folder");

	width: Style.sizeHintM;
	height: Style.sizeHintM;

	modal: true;

	property string startPath: "";
	property string currentPath: "";
	property string parentPath: "";
	property string selectedPath: "";
	property bool showFiles: false;
	property bool isLoading: false;
	property string errorText: "";

	signal pathSelected(string selectedPath);

	ListModel {
		id: entriesModel;
	}

	GqlRequestSender {
		id: requestSender;

		requestType: 0; // Query
		gqlCommandId: "GetFileSystemEntries";

		function createQueryParams(query, params){
			var inputParams = Gql.GqlObject("input");
			inputParams.InsertField("path", params["path"]);
			query.AddParam(inputParams);

			query.AddField(Gql.GqlObject("path"));
			query.AddField(Gql.GqlObject("parentPath"));

			var entriesFields = Gql.GqlObject("entries");
			entriesFields.InsertField("name");
			entriesFields.InsertField("path");
			entriesFields.InsertField("isDir");
			query.AddField(entriesFields);
		}

		function onResult(data){
			root.isLoading = false;
			root.errorText = "";

			root.currentPath = data.getData("path");
			root.parentPath = data.containsKey("parentPath") ? data.getData("parentPath") : "";
			root.selectedPath = root.currentPath;

			entriesModel.clear();

			if (data.isTreeModel("entries")){
				var entries = data.getTreeItemModel("entries");
				var count = entries.getItemsCount();
				for (var i = 0; i < count; i++){
					var isDir = entries.getData("isDir", i);
					if (!isDir && !root.showFiles){
						continue;
					}

					entriesModel.append({
						name: entries.getData("name", i),
						path: entries.getData("path", i),
						isDir: isDir
					});
				}
			}
		}

		function onError(message, type){
			root.isLoading = false;
			root.errorText = message;
		}
	}

	function browse(path){
		root.isLoading = true;
		root.errorText = "";
		requestSender.send({path: path});
	}

	Component.onCompleted: {
		addButton(Enums.ok, qsTr("Select"), true);
		addButton(Enums.cancel, qsTr("Cancel"), true);

		root.browse(root.startPath);
	}

	onFinished: {
		if (buttonId == Enums.ok){
			root.pathSelected(root.selectedPath);
		}
	}

	onLocalizationChanged: {
		setButtonName(Enums.ok, qsTr("Select"));
		setButtonName(Enums.cancel, qsTr("Cancel"));
	}

	contentComp: Component {
		Item {
			width: root.width;
			height: root.height - Style.sizeHintXXS;

			Item {
				id: navigationPanel;

				anchors.top: parent.top;
				anchors.left: parent.left;
				anchors.right: parent.right;
				anchors.margins: Style.marginM;

				height: Style.controlHeightM;

				Button {
					id: upButton;

					anchors.left: parent.left;
					anchors.verticalCenter: parent.verticalCenter;

					widthFromDecorator: true;

					text: qsTr("Up");
					enabled: root.parentPath !== "" && !root.isLoading;

					onClicked: {
						root.browse(root.parentPath);
					}
				}

				CustomTextField {
					id: pathField;

					anchors.left: upButton.right;
					anchors.leftMargin: Style.marginM;
					anchors.right: parent.right;
					anchors.verticalCenter: parent.verticalCenter;

					height: parent.height;

					text: root.currentPath;

					readOnly: true;
				}
			}

			Rectangle {
				id: entriesPanel;

				anchors.top: navigationPanel.bottom;
				anchors.topMargin: Style.marginM;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginM;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginM;
				anchors.bottom: statusText.top;
				anchors.bottomMargin: Style.marginM;

				color: Style.baseColor;
				border.color: Style.borderColor;
				border.width: 1;

				ListView {
					id: entriesView;

					anchors.fill: parent;
					anchors.margins: Style.marginS;

					clip: true;

					model: entriesModel;

					delegate: Rectangle {
						id: entryItem;

						width: entriesView.width;
						height: Style.controlHeightM;

						color: entryMouseArea.containsMouse ? Style.highlightColor : "transparent";

						Text {
							anchors.left: parent.left;
							anchors.leftMargin: Style.marginM;
							anchors.right: parent.right;
							anchors.rightMargin: Style.marginM;
							anchors.verticalCenter: parent.verticalCenter;

							text: model.name;
							color: Style.textColor;
							font.pixelSize: Style.fontSizeM;
							font.bold: model.isDir;
							elide: Text.ElideRight;
						}

						MouseArea {
							id: entryMouseArea;

							anchors.fill: parent;

							hoverEnabled: true;

							onClicked: {
								root.selectedPath = model.path;
							}

							onDoubleClicked: {
								if (model.isDir){
									root.browse(model.path);
								}
							}
						}
					}

					CustomScrollbar {
						z: parent.z + 1;

						anchors.right: parent.right;
						anchors.top: parent.top;
						anchors.bottom: parent.bottom;

						targetItem: entriesView;
					}
				}
			}

			Text {
				id: statusText;

				anchors.left: parent.left;
				anchors.leftMargin: Style.marginM;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginM;
				anchors.bottom: parent.bottom;
				anchors.bottomMargin: Style.marginM;

				height: Style.controlHeightM;

				text: root.errorText !== "" ? root.errorText : (root.isLoading ? qsTr("Loading...") : root.selectedPath);
				color: root.errorText !== "" ? Style.errorColor : Style.textColor;
				font.pixelSize: Style.fontSizeM;
				elide: Text.ElideMiddle;
				verticalAlignment: Text.AlignVCenter;
			}
		}
	}
}
