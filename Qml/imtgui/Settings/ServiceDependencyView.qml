import QtQuick 2.12
import QtQuick.Dialogs 1.3
import Qt.labs.platform 1.1
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

	property int indentWidth: Style.iconSizeXS * 2 + Style.spacingS
	property int nameRight: 0
	property int versionX: nameRight + Style.spacingXL

	function reportNameRight(value){
		if (value > serviceDependencyView.nameRight){
			serviceDependencyView.nameRight = value
		}
	}

	onEditorModelChanged: {
		serviceDependencyView.updateRows()
	}

	ListModel {
		id: rowsModel
	}

	// The statuses are part of the GetApplicationInfo response, so refresh it while the tree is shown
	Timer {
		interval: 2000
		repeat: true
		running: serviceDependencyView.visible
		onTriggered: {
			ApplicationInfoProvider.updateModel()
		}
	}

	RemoteFileController {
		id: remoteFileController
		prefix: "/files/"
	}

	// The page gets the tree once as JSON, so the open page is refreshed from the provider
	Connections {
		target: ApplicationInfoProvider

		function onUpdated(){
			serviceDependencyView.refreshFromProvider()
		}

		function onUpdateFailed(){
			serviceDependencyView.markDependenciesDisconnected()
		}
	}

	// The server did not answer: the services it depends on cannot be reached through it, the server itself is shown as it was
	function markDependenciesDisconnected(){
		for (let i = 0; i < rowsModel.count; ++i){
			if (rowsModel.get(i).level > 0){
				rowsModel.setProperty(i, "status", ServiceStatusEnum.s_dISCONNECTED)
				rowsModel.setProperty(i, "credentialStatus", ServerCredentialStatusEnum.s_nOT_USED)
			}
		}
	}

	function formatVersion(version, versionKind, buildTime){
		let text = version ? String(version) : ""
		if (versionKind){
			text = text === "" ? String(versionKind) : text + " - " + String(versionKind)
		}
		if (buildTime){
			let buildTimeText = Qt.formatDateTime(new Date(String(buildTime)), "dd.MM.yyyy hh:mm")
			text = text === "" ? buildTimeText : text + " (" + buildTimeText + ")"
		}

		return text
	}

	function refreshFromProvider(){
		let info = ApplicationInfoProvider.serverApplicationInfo
		if (!info){
			return
		}

		rowsModel.clear()
		serviceDependencyView.nameRight = 0
		serviceDependencyView.addRawRow(info, 0, true, true)
	}

	// Same as addRow, but for the raw GetApplicationInfo data, where the version is not formatted yet
	function addRawRow(info, level, isLast, isRoot){
		let hasChildren = info.m_dependencies && info.m_dependencies.getItemsCount() > 0
		let name = info.m_applicationName ? String(info.m_applicationName) : (info.m_serviceId ? String(info.m_serviceId) : "")
		let status = isRoot ? ServiceStatusEnum.s_cONNECTED : (info.m_status ? String(info.m_status) : ServiceStatusEnum.s_uNKNOWN)
		let credentialStatus = info.m_credentialStatus ? String(info.m_credentialStatus) : ServerCredentialStatusEnum.s_nOT_USED
		let logId = info.m_logId ? String(info.m_logId) : (isRoot ? "GetServerLog" : "")

		rowsModel.append({"name": name, "level": level, "isLast": isLast, "hasChildren": hasChildren, "status": status, "version": serviceDependencyView.formatVersion(info.m_version, info.m_versionKind, info.m_buildTime), "credentialStatus": credentialStatus, "logId": logId})

		if (hasChildren){
			let count = info.m_dependencies.getItemsCount()
			for (let i = 0; i < count; ++i){
				serviceDependencyView.addRawRow(info.m_dependencies.get(i).item, level + 1, i === count - 1, false)
			}
		}
	}

	function updateRows(){
		rowsModel.clear()
		serviceDependencyView.nameRight = 0
		if (serviceDependencyView.editorModel){
			serviceDependencyView.addRow(serviceDependencyView.editorModel, 0, true)
		}
	}

	function addRow(serviceInfo, level, isLast){
		let hasChildren = serviceInfo.m_dependencies && serviceInfo.m_dependencies.getItemsCount() > 0
		let name = serviceInfo.m_applicationName !== "" ? serviceInfo.m_applicationName : serviceInfo.m_serviceId

		rowsModel.append({"name": name, "level": level, "isLast": isLast, "hasChildren": hasChildren, "status": serviceInfo.m_status, "version": serviceInfo.m_version, "credentialStatus": String(serviceInfo.m_credentialStatus), "logId": serviceInfo.m_logId ? String(serviceInfo.m_logId) : (level === 0 ? "GetServerLog" : "")})

		if (hasChildren){
			let count = serviceInfo.m_dependencies.getItemsCount()
			for (let i = 0; i < count; ++i){
				serviceDependencyView.addRow(serviceInfo.m_dependencies.get(i).item, level + 1, i === count - 1)
			}
		}
	}

	function getStatusColor(status){
		if (status === ServiceStatusEnum.s_cONNECTED){
			return Style.successColor
		}

		if (status === ServiceStatusEnum.s_dISCONNECTED){
			return Style.dangerColor
		}

		return Style.inactiveTextColor
	}

	function getStatusText(status, version){
		if (status === ServiceStatusEnum.s_cONNECTED){
			return version !== "" ? version : qsTr("Connected")
		}

		if (status === ServiceStatusEnum.s_dISCONNECTED){
			return qsTr("Not connected")
		}

		return qsTr("Unknown")
	}

	function getCredentialText(credentialStatus){
		if (credentialStatus === ServerCredentialStatusEnum.s_mISSING){
			return qsTr("no server token")
		}

		if (credentialStatus === ServerCredentialStatusEnum.s_rEJECTED){
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
							height: Style.controlHeightM + Style.spacingXS

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
								id: nameText
								anchors.verticalCenter: rowItem.verticalCenter
								anchors.left: statusIndicator.right
								anchors.leftMargin: Style.spacingS
								width: Math.max(0, statusText.x - x - Style.spacingS)
								elide: Text.ElideRight
								font.bold: true
								text: model.name

								property int rightEdge: rowItem.dotX + Style.iconSizeXS + Style.spacingS + implicitWidth
								onRightEdgeChanged: serviceDependencyView.reportNameRight(rightEdge)
								Component.onCompleted: serviceDependencyView.reportNameRight(rightEdge)

								MouseArea {
									id: nameMouseArea
									anchors.fill: parent
									acceptedButtons: Qt.NoButton
								}

								TooltipArea {
									anchors.fill: parent
									mouseArea: nameMouseArea
									text: parent.truncated ? parent.text : ""
								}
							}

							BaseText {
								id: statusText
								anchors.verticalCenter: rowItem.verticalCenter
								x: Math.max(0, Math.min(serviceDependencyView.versionX, rowItem.width - Style.sizeHintBXS - (downloadLogButton.visible ? downloadLogButton.width + Style.spacingS : 0)))
								width: Math.max(0, rowItem.width - x - (downloadLogButton.visible ? downloadLogButton.width + Style.spacingS : 0))
								elide: Text.ElideRight
								color: model.status === ServiceStatusEnum.s_dISCONNECTED || serviceDependencyView.getCredentialText(model.credentialStatus) !== "" ? Style.dangerColor : Style.textColor
								text: serviceDependencyView.getStatusText(model.status, model.version) + (serviceDependencyView.getCredentialText(model.credentialStatus) !== "" ? " - " + serviceDependencyView.getCredentialText(model.credentialStatus) : "")

								MouseArea {
									id: statusMouseArea
									anchors.fill: parent
									acceptedButtons: Qt.NoButton
								}

								TooltipArea {
									anchors.fill: parent
									mouseArea: statusMouseArea
									text: parent.truncated ? parent.text : ""
								}
							}

							Button {
								id: downloadLogButton
								anchors.verticalCenter: rowItem.verticalCenter
								anchors.right: rowItem.right
								width: Style.sizeHintBXS
								height: Style.controlHeightM
								visible: model.status === ServiceStatusEnum.s_cONNECTED && model.logId !== ""
								enabled: remoteFileController.state !== "Loading"

								text: qsTr("Download Log")
									remoteFileController.getFile(model.logId, String(model.name).replace(/[\\/:*?"<>|\x00-\x1f]/g, "_") + "Log.txt")
									remoteFileController.getFile(model.logId, model.name + "Log.txt")
								}
							}
						}
					}
				}
			}
		}
	}
}
