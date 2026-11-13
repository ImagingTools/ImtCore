import QtQuick 2.12
import QtTest 1.12
import imtdocgui 1.0

TestCase {
	id: testCase

	name: "SimpleCollectionPageCommands"
	when: windowShown

	property var customTexts: ["First long command", "Second long command", "Third long command", "Fourth long command"]

	QtObject {
		id: permissions

		property bool canManageMembers: true

		function hasAnyPermission(ids){
			return true
		}
	}

	ListModel {
		id: emptyModel
	}

	Component {
		id: defaultPageComp

		SimpleCollectionPage {
			width: 1200
			height: 400
			listHeaderTitle: "Members of the organization"
			createButtonText: "Create a new member"
			stateManager: permissions
			listModel: emptyModel
		}
	}

	Component {
		id: customPageComp

		SimpleCollectionPage {
			id: customPage

			width: 1200
			height: 400
			listHeaderTitle: "Members of the organization"
			stateManager: permissions
			listModel: emptyModel
			headerButtonsComponent: Component {
				Flow {
					id: customCommands
					width: Math.min(customPage.commandsLineWidth(customCommands), customPage.contentWidth)
					spacing: 16

					Repeater {
						model: testCase.customTexts

						Text {
							font.pixelSize: 14
							text: modelData
						}
					}
				}
			}
		}
	}

	// The editor showing the headers of the page (TenantEditor)
	Component {
		id: headerHostComp

		QtObject {
			property int updateCount: 0

			function updateSubHeaders(){
				updateCount++
			}
		}
	}

	Component {
		id: hostedPageComp

		SimpleCollectionPage {
			width: 1200
			height: 400
			entityName: "Member"
			listHeaderTitle: "Members of the organization"
			createButtonText: "Create a new member"
			stateManager: permissions
			listModel: emptyModel
			customEditorComponent: Item {
			}
		}
	}

	function findItems(item, check, result){
		if (check(item))
			result.push(item)
		for (let i = 0; i < item.children.length; i++)
			findItems(item.children[i], check, result)
		return result
	}

	function isShown(item){
		for (let p = item; p; p = p.parent){
			if (!p.visible || p.opacity === 0)
				return false
		}
		return true
	}

	function rectOf(item, page){
		let topLeft = item.mapToItem(page, 0, 0)
		return {x: topLeft.x, y: topLeft.y, right: topLeft.x + item.width, bottom: topLeft.y + item.height}
	}

	function intersects(a, b){
		return a.x < b.right && b.x < a.right && a.y < b.bottom && b.y < a.bottom
	}

	function checkLayout(page, commandTexts){
		let header = findItems(page, function(item){ return item.implicitContentWidth !== undefined }, [])[0]
		verify(header, "the page has a header")
		let headerTexts = findItems(header, function(item){ return item.elide !== undefined && isShown(item) }, [])
		verify(headerTexts.length > 0, "the header shows its title")

		let commands = findItems(page, function(item){ return commandTexts.indexOf(item.text) >= 0 && item.elide !== undefined && isShown(item) }, [])
		compare(commands.length, commandTexts.length, "all commands are shown")

		for (let i = 0; i < commands.length; i++){
			let command = rectOf(commands[i], page)
			verify(command.x >= 0 && command.right <= page.width + 1,
				   commands[i].text + " is inside the page: " + command.x + ".." + command.right + " of " + page.width)
			for (let h = 0; h < headerTexts.length; h++){
				verify(!intersects(command, rectOf(headerTexts[h], page)), commands[i].text + " does not cover " + headerTexts[h].text)
			}
			for (let j = 0; j < i; j++){
				verify(!intersects(command, rectOf(commands[j], page)), commands[i].text + " does not cover " + commands[j].text)
			}
		}
	}

	function test_defaultCommands_data(){
		return [{tag: "wide", width: 1200}, {tag: "middle", width: 500}, {tag: "narrow", width: 260}]
	}

	function test_defaultCommands(data){
		let page = createTemporaryObject(defaultPageComp, testCase.parent)
		page.width = data.width
		waitForRendering(page)
		checkLayout(page, ["Remove", "Edit", "Create a new member"])
	}

	function test_customCommands_data(){
		return [{tag: "wide", width: 1200}, {tag: "middle", width: 700}, {tag: "narrow", width: 300}]
	}

	function test_customCommands(data){
		let page = createTemporaryObject(customPageComp, testCase.parent)
		page.width = data.width
		waitForRendering(page)
		checkLayout(page, testCase.customTexts)
	}

	// Widened from the width that put the commands under the header: they return to its line, one line high
	function test_commandsReturnToTheHeaderLine(){
		let page = createTemporaryObject(defaultPageComp, testCase.parent, {"listHeaderTitle": "Members"})
		page.width = 260
		waitForRendering(page)
		page.width = 1200
		waitForRendering(page)

		let header = findItems(page, function(item){ return item.implicitContentWidth !== undefined }, [])[0]
		let headerRect = rectOf(header, page)
		let commands = findItems(page, function(item){ return ["Remove", "Edit", "Create a new member"].indexOf(item.text) >= 0 && item.elide !== undefined && isShown(item) }, [])
		compare(commands.length, 3)
		for (let i = 0; i < commands.length; i++){
			let command = rectOf(commands[i], page)
			let center = (command.y + command.bottom) / 2
			verify(center > headerRect.y && center < headerRect.bottom, commands[i].text + " is on the header line: " + center + " in " + headerRect.y + ".." + headerRect.bottom)
		}
	}

	function openHostedPage(width){
		let host = createTemporaryObject(headerHostComp, testCase)
		let page = createTemporaryObject(hostedPageComp, testCase.parent, {"headerHost": host})
		page.width = width
		waitForRendering(page)
		return {"host": host, "page": page}
	}

	// With the headers shown by the host the commands share the line of the search box while they fit, and go above it otherwise
	function test_hostedCommands_data(){
		return [{tag: "wide", width: 1200, sameLine: true}, {tag: "narrow", width: 420, sameLine: false}]
	}

	function test_hostedCommands(data){
		let context = openHostedPage(data.width)
		let page = context.page

		let headerTexts = findItems(page, function(item){ return item.text === "Members of the organization" && isShown(item) }, [])
		compare(headerTexts.length, 0, "The page shows no header of its own")

		let search = findItems(page, function(item){ return item.placeHolderText !== undefined && isShown(item) }, [])[0]
		verify(search, "the search box is shown")
		let searchRect = rectOf(search, page)

		let commandTexts = ["Remove", "Edit", "Create a new member"]
		let commands = findItems(page, function(item){ return commandTexts.indexOf(item.text) >= 0 && item.elide !== undefined && isShown(item) }, [])
		compare(commands.length, commandTexts.length, "all commands are shown")

		for (let i = 0; i < commands.length; i++){
			let command = rectOf(commands[i], page)
			verify(command.x >= 0 && command.right <= page.width + 1, commands[i].text + " is inside the page")
			verify(!intersects(command, searchRect), commands[i].text + " does not cover the search box")
			let center = (command.y + command.bottom) / 2
			if (data.sameLine)
				verify(center > searchRect.y && center < searchRect.bottom, commands[i].text + " is on the line of the search box: " + center + " in " + searchRect.y + ".." + searchRect.bottom)
			else
				verify(command.bottom <= searchRect.y, commands[i].text + " is above the search box")
		}
	}

	function test_editorStateIsReportedToTheHost(){
		let context = openHostedPage(1200)
		let page = context.page
		verify(!page.editorOpen)

		page.openCreate()

		verify(page.editorOpen)
		compare(page.editorTitle, "Create New Member")
		verify(context.host.updateCount > 0, "The host is told")

		let updates = context.host.updateCount
		let answers = []
		page.requestCloseEditor(function(closed){
			answers.push(closed)
		})

		compare(answers, [true])
		verify(!page.editorOpen)
		compare(page.editorTitle, "")
		verify(context.host.updateCount > updates)
	}

	function test_requestCloseEditorOnTheList(){
		let context = openHostedPage(1200)
		let answers = []

		context.page.requestCloseEditor(function(closed){
			answers.push(closed)
		})

		compare(answers, [true])
	}
}
