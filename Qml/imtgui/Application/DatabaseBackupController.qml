import QtQuick 2.12
import Qt.labs.platform 1.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

// Local backup/restore for the server configurators: the PostgreSQL tools run on this machine, nothing goes through the server.
Item {
	id: backupController

	height: content.height

	property DatabaseAccessSettings databaseParams: null
	property string operation
	property string restoreFilePath
	property string errorText

	function connectionArguments(){
		return ["-h", backupController.databaseParams.m_host, "-U", backupController.databaseParams.m_username, "-p", String(backupController.databaseParams.m_port)]
	}

	function quotedDatabaseName(){
		return "\"" + backupController.databaseParams.m_dbName.replace(/"/g, "\"\"") + "\""
	}

	function filePathFromUrl(url){
		let path = decodeURIComponent(url.toString().replace(/^file:\/\//, ""))
		if (/^\/[A-Za-z]:/.test(path)){
			path = path.substring(1)
		}

		return path
	}

	function runStep(stepId, program, args){
		backupController.operation = stepId
		process.setEnviroment(["PGPASSWORD=" + backupController.databaseParams.m_password])
		process.start(program, args)
	}

	function startBackup(filePath){
		Events.sendEvent("StartLoading")
		backupController.errorText = ""
		backupController.runStep("Backup", "pg_dump", backupController.connectionArguments().concat(["-Fc", "-b", "-f", filePath, backupController.databaseParams.m_dbName]))
	}

	function startRestore(filePath){
		Events.sendEvent("StartLoading")
		backupController.errorText = ""
		backupController.restoreFilePath = filePath
		// The database is dropped in the next step, so an unreadable archive must be rejected first.
		backupController.runStep("Validate", "pg_restore", ["-l", filePath])
	}

	function finish(succeeded, message){
		backupController.operation = ""
		Events.sendEvent("StopLoading")

		if (succeeded){
			PopupManager.addSuccessMessage(message, true)
		}
		else{
			PopupManager.addErrorMessage(message + "\n" + backupController.errorText, true)
		}
	}

	function handleStepFinished(){
		if (process.exitCode !== 0){
			backupController.finish(false, backupController.operation === "Backup" ? qsTr("Error when trying to create a database backup") : qsTr("Error when trying to restore the database"))
			return
		}

		if (backupController.operation === "Backup"){
			backupController.finish(true, qsTr("Database backup was created"))
		}
		else if (backupController.operation === "Validate"){
			let databaseName = backupController.quotedDatabaseName()
			backupController.runStep("Recreate", "psql", backupController.connectionArguments().concat(["-d", "postgres", "-v", "ON_ERROR_STOP=1", "-c", "DROP DATABASE IF EXISTS " + databaseName + " WITH (FORCE)", "-c", "CREATE DATABASE " + databaseName]))
		}
		else if (backupController.operation === "Recreate"){
			backupController.runStep("Restore", "pg_restore", backupController.connectionArguments().concat(["-d", backupController.databaseParams.m_dbName, backupController.restoreFilePath]))
		}
		else if (backupController.operation === "Restore"){
			backupController.finish(true, qsTr("Database restore was successful"))
		}
	}

	Process {
		id: process
	}

	Connections {
		target: process

		function onStandardError(error){
			backupController.errorText += error
		}

		function onFinished(){
			backupController.handleStepFinished()
		}
	}

	FileDialog {
		id: backupFileDialog
		title: qsTr("Save backup file")
		fileMode: FileDialog.SaveFile
		defaultSuffix: "backup"
		nameFilters: [qsTr("Backup files (*.backup)"), qsTr("All files (*)")]

		onAccepted: {
			backupController.startBackup(backupController.filePathFromUrl(backupFileDialog.file))
		}
	}

	FileDialog {
		id: restoreFileDialog
		title: qsTr("Select backup file")
		fileMode: FileDialog.OpenFile
		nameFilters: [qsTr("Backup files (*.backup)"), qsTr("All files (*)")]

		onAccepted: {
			ModalDialogManager.openDialog(restoreConfirmationDialogComp, {"title": qsTr("Restore database"), "message": qsTr("The current database will be replaced with the content of the selected backup. Continue?")})
		}
	}

	Component {
		id: restoreConfirmationDialogComp

		MessageDialog {
			onFinished: {
				if (buttonId == Enums.yes){
					backupController.startRestore(backupController.filePathFromUrl(restoreFileDialog.file))
				}
			}
		}
	}

	Column {
		id: content
		width: backupController.width
		spacing: Style.marginXL

		GroupHeaderView {
			width: content.width
			title: qsTr("Backup Information")
			groupView: group
		}

		GroupElementView {
			id: group
			width: content.width

			ButtonElementView {
				id: backupButton
				width: group.width
				name: qsTr("Backup data")
				description: qsTr("The backup is created on this computer with the PostgreSQL tools (pg_dump)")
				text: qsTr("Backup")
				enabled: backupController.operation === "" && backupController.databaseParams !== null
				onClicked: {
					backupFileDialog.open()
				}
			}

			ButtonElementView {
				width: group.width
				name: qsTr("Restore data from backup")
				description: qsTr("The database is replaced on this computer with the PostgreSQL tools (pg_restore)")
				text: qsTr("Restore")
				enabled: backupButton.enabled
				onClicked: {
					restoreFileDialog.open()
				}
			}
		}
	}
}
