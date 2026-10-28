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
}
