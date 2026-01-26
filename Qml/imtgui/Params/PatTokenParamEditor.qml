import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamEditorBase {
	id: patTokenEditor
	
	typeId: "PatTokenList"
	
	editorModelComp: Component {
		PatTokenListParam {}
	}
	
	property PatTokenListParam patTokenParam: editorModel
	
	sourceComp: Component {
		Item {
			id: tokenEditorView
			width: patTokenEditor.width
			height: Math.max(Style.sizeHintS, contentColumn.height)
			
			Column {
				id: contentColumn
				width: parent.width
				spacing: Style.marginM
				
				// Header with title and description
				Column {
					width: parent.width
					spacing: Style.marginXS
					
					Text {
						text: patTokenEditor.name
						font.pixelSize: Style.fontSizeM
						font.bold: true
						color: Style.textColor
					}
					
					Text {
						text: patTokenEditor.description
						font.pixelSize: Style.fontSizeS
						color: Style.hintTextColor
						visible: patTokenEditor.description !== ""
						wrapMode: Text.WordWrap
						width: parent.width
					}
				}
				
				// Token list
				Rectangle {
					width: parent.width
					height: Math.max(Style.sizeHintXS, tokenListView.contentHeight + Style.marginM)
					color: Style.backgroundColor
					border.color: Style.borderColor
					border.width: 1
					radius: Style.radiusS
					
					ListView {
						id: tokenListView
						anchors.fill: parent
						anchors.margins: Style.marginS
						clip: true
						spacing: Style.marginS
						
						model: patTokenEditor.patTokenParam ? patTokenEditor.patTokenParam.tokens : null
						
						delegate: Rectangle {
							id: delegateRoot
							width: tokenListView.width
							height: tokenItemColumn.height + Style.marginM
							color: Style.baseColor
							border.color: Style.borderColor
							border.width: 1
							radius: Style.radiusS
							
							property bool showToken: false
							
							Column {
								id: tokenItemColumn
								anchors.fill: parent
								anchors.margins: Style.marginS
								spacing: Style.marginXS
								
								Row {
									width: parent.width
									spacing: Style.marginS
									
									Column {
										width: parent.width - deleteButton.width - Style.marginS
										spacing: Style.marginXS
										
										Text {
											text: model.name
											font.pixelSize: Style.fontSizeM
											font.bold: true
											color: Style.textColor
											width: parent.width
											elide: Text.ElideRight
										}
										
										Text {
											text: model.description
											font.pixelSize: Style.fontSizeS
											color: Style.hintTextColor
											visible: model.description !== ""
											width: parent.width
											wrapMode: Text.WordWrap
										}
										
										Row {
											spacing: Style.marginXS
											
											Text {
												text: qsTr("Token:")
												font.pixelSize: Style.fontSizeS
												color: Style.hintTextColor
											}
											
											Text {
												text: delegateRoot.showToken ? model.token : "•••••••••••••••"
												font.pixelSize: Style.fontSizeS
												color: Style.textColor
												font.family: "monospace"
											}
											
											Button {
												text: delegateRoot.showToken ? qsTr("Hide") : qsTr("Show")
												height: Style.sizeHintXXXS
												width: Style.sizeHintXXXS
												onClicked: delegateRoot.showToken = !delegateRoot.showToken
											}
										}
										
										Text {
											text: qsTr("Created: ") + new Date(model.createdDate).toLocaleString()
											font.pixelSize: Style.fontSizeXS
											color: Style.hintTextColor
										}
									}
									
									Button {
										id: deleteButton
										text: qsTr("Delete")
										width: Style.sizeHintXXXS
										height: Style.sizeHintXXXS
										anchors.verticalCenter: parent.verticalCenter
										enabled: !patTokenEditor.readOnly
										onClicked: {
											if (patTokenEditor.patTokenParam) {
												patTokenEditor.patTokenParam.removeToken(index)
											}
										}
									}
								}
							}
						}
						
						Text {
							anchors.centerIn: parent
							visible: tokenListView.count === 0
							text: qsTr("No PAT tokens configured")
							font.pixelSize: Style.fontSizeM
							color: Style.hintTextColor
						}
					}
				}
				
				// Add token button
				Button {
					text: qsTr("Add PAT Token")
					width: Style.sizeHintXS
					height: Style.sizeHintXXXS
					enabled: !patTokenEditor.readOnly
					onClicked: addTokenDialog.open()
				}
			}
			
			// Add/Edit Token Dialog
			Component {
				id: addTokenDialogComp
				
				Dialog {
					id: addDialog
					title: qsTr("Add PAT Token")
					width: Style.sizeHintXL
					
					property string tokenName: ""
					property string tokenValue: ""
					property string tokenDescription: ""
					
					Component.onCompleted: {
						clearButtons()
						addButton(Enums.apply, qsTr("Add"), false)
						addButton(Enums.cancel, qsTr("Cancel"), true)
					}
					
					onFinished: {
						if (buttonId === Enums.apply) {
							if (tokenName.trim() !== "" && tokenValue.trim() !== "") {
								if (patTokenEditor.patTokenParam) {
									patTokenEditor.patTokenParam.addToken(
										tokenName.trim(),
										tokenValue.trim(),
										tokenDescription.trim()
									)
								}
								close()
							} else {
								ModalDialogManager.showInfoDialog(qsTr("Name and Token fields are required"))
							}
						}
					}
					
					contentComp: Component {
						Column {
							width: addDialog.width - Style.marginL
							spacing: Style.marginM
							
							TextInputElementView {
								width: parent.width
								name: qsTr("Token Name")
								description: qsTr("A descriptive name for this token")
								text: addDialog.tokenName
								onEditingFinished: addDialog.tokenName = text
							}
							
							TextInputElementView {
								width: parent.width
								name: qsTr("Token Value")
								description: qsTr("The actual PAT token string")
								text: addDialog.tokenValue
								echoMode: TextInput.Password
								onEditingFinished: addDialog.tokenValue = text
							}
							
							TextInputElementView {
								width: parent.width
								name: qsTr("Description")
								description: qsTr("Optional description for this token")
								text: addDialog.tokenDescription
								onEditingFinished: addDialog.tokenDescription = text
							}
						}
					}
				}
			}
			
			property var addTokenDialog: null
			
			Component.onCompleted: {
				addTokenDialog = addTokenDialogComp.createObject(tokenEditorView)
			}
		}
	}
}
