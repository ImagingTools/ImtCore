import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtappApplicationSdl 1.0

// Read-only tree of the server and the services it depends on: runtime status (green - connected, red - not connected, gray - unknown) and version.
// m_version of each service info holds the text to show, see PageAboutProvider
ParamEditorBase {
	id: serviceDependencyView

	typeId: "ServiceDependency"
	readOnly: true

	editorModelComp: Component {
		ServiceInfo {}
	}

	property int indentWidth: Style.marginXL
	property int versionX: Style.sizeHintXXS

	onEditorModelChanged: {
		serviceDependencyView.updateRows()
	}

	ListModel {
		id: rowsModel
	}

	function updateRows(){
		rowsModel.clear()
		if (serviceDependencyView.editorModel){
			serviceDependencyView.addRow(serviceDependencyView.editorModel, 0, true)
		}
	}

	function addRow(serviceInfo, level, isLast){
		let hasChildren = serviceInfo.m_dependencies && serviceInfo.m_dependencies.getItemsCount() > 0
		let name = serviceInfo.m_applicationName !== "" ? serviceInfo.m_applicationName : serviceInfo.m_serviceId

		rowsModel.append({"name": name, "level": level, "isLast": isLast, "hasChildren": hasChildren, "status": serviceInfo.m_status, "version": serviceInfo.m_version, "credentialStatus": String(serviceInfo.m_credentialStatus)})

		if (hasChildren){
			let count = serviceInfo.m_dependencies.getItemsCount()
			for (let i = 0; i < count; ++i){
				serviceDependencyView.addRow(serviceInfo.m_dependencies.get(i).item, level + 1, i === count - 1)
			}
		}
	}

	function getStatusColor(status){
		if (status === "CONNECTED"){
			return Style.successColor
		}

		if (status === "DISCONNECTED"){
			return Style.dangerColor
		}

		return Style.inactiveTextColor
	}

	function getStatusText(status, version){
		if (status === "CONNECTED"){
			return version !== "" ? version : qsTr("Connected")
		}

		if (status === "DISCONNECTED"){
			return qsTr("Not connected")
		}

		return qsTr("Unknown")
	}

	function getCredentialText(credentialStatus){
		if (credentialStatus === "MISSING"){
			return qsTr("no server token")
		}

		if (credentialStatus === "REJECTED"){
			return qsTr("server token is refused")
		}

		return ""
	}

	sourceComp: Component {
		ElementView {
			id: elementView
			width: serviceDependencyView.width
			name: serviceDependencyView.name

			bottomComp: Component {
				Column {
					id: rowsColumn

					Repeater {
						model: rowsModel

						delegate: Item {
							id: rowItem
							width: rowsColumn.width
							height: Style.controlHeightS

							property int dotX: model.level * serviceDependencyView.indentWidth
							property int lineX: (model.level - 1) * serviceDependencyView.indentWidth + Style.iconSizeXS / 2

							// Line from the parent down to this row, the last child ends it at its own level
							Rectangle {
								x: rowItem.lineX
								width: 1
								height: model.isLast ? rowItem.height / 2 : rowItem.height
								visible: model.level > 0
								color: Style.borderColor
							}

							Rectangle {
								x: rowItem.lineX
								y: rowItem.height / 2
								width: rowItem.dotX - rowItem.lineX - Style.spacingXS
								height: 1
								visible: model.level > 0
								color: Style.borderColor
							}

							// Line from this row down to its first child
							Rectangle {
								x: rowItem.dotX + Style.iconSizeXS / 2
								y: rowItem.height / 2 + Style.iconSizeXS / 2
								width: 1
								height: rowItem.height / 2 - Style.iconSizeXS / 2
								visible: model.hasChildren
								color: Style.borderColor
							}

							Rectangle {
								id: statusIndicator
								x: rowItem.dotX
								anchors.verticalCenter: rowItem.verticalCenter
								width: Style.iconSizeXS
								height: Style.iconSizeXS
								radius: Style.iconSizeXS / 2
								color: serviceDependencyView.getStatusColor(model.status)
							}

							BaseText {
								anchors.verticalCenter: rowItem.verticalCenter
								anchors.left: statusIndicator.right
								anchors.leftMargin: Style.spacingS
								width: serviceDependencyView.versionX - x - Style.spacingS
								elide: Text.ElideRight
								font.bold: true
								text: model.name
							}

							BaseText {
								anchors.verticalCenter: rowItem.verticalCenter
								x: serviceDependencyView.versionX
								width: rowItem.width - x
								elide: Text.ElideRight
								color: model.status === "DISCONNECTED" || serviceDependencyView.getCredentialText(model.credentialStatus) !== "" ? Style.dangerColor : Style.textColor
								text: serviceDependencyView.getStatusText(model.status, model.version) + (serviceDependencyView.getCredentialText(model.credentialStatus) !== "" ? " - " + serviceDependencyView.getCredentialText(model.credentialStatus) : "")
							}
						}
					}
				}
			}
		}
	}
}
