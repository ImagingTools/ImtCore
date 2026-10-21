import QtQuick 2.12
import QtTest 1.12
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

TestCase {
	id: testCase

	name: "TableRowViewer"
	when: windowShown

	readonly property int columnCount: 11
	readonly property int rowCount: 60

	Component {
		id: tableComp

		Table {
			width: 1600
			height: 900
		}
	}

	Component {
		id: modelComp

		TreeItemModel {
		}
	}

	function createHeaders(){
		let headers = createTemporaryObject(modelComp, testCase)
		for (let i = 0; i < columnCount; ++i){
			let index = headers.insertNewItem()
			headers.setData("id", "col" + i, index)
			headers.setData("name", "Column " + i, index)
		}

		return headers
	}

	function createElements(tag){
		let elements = createTemporaryObject(modelComp, testCase)
		for (let row = 0; row < rowCount; ++row){
			let index = elements.insertNewItem()
			for (let column = 0; column < columnCount; ++column){
				elements.setData("col" + column, tag + row + "_" + column, index)
			}
		}

		return elements
	}

	// Rows built after the first time slice get their cells on a later tick
	function emptyRows(table){
		let result = []
		let list = table.elementsList
		for (let row = 0; row < list.count; ++row){
			let rowItem = list.itemAtIndex(row)
			if (!rowItem){
				continue
			}

			let cells = rowItem.tableRowViewer.children
			for (let i = 0; i < cells.length; ++i){
				if (cells[i].hasOwnProperty("contentItem") && cells[i].contentItem === null){
					result.push(row)
					break
				}
			}
		}

		return result
	}

	function verifyAllRowsFilled(table, label){
		tryVerify(function(){ return emptyRows(table).length === 0 }, 3000, label + ": rows without cells " + JSON.stringify(emptyRows(table)))
	}

	function test_allRowsAreFilled(){
		let table = createTemporaryObject(tableComp, testCase.parent)
		table.headers = createHeaders()
		table.elements = createElements("a")
		verifyAllRowsFilled(table, "first page")

		table.elements = createElements("b")
		verifyAllRowsFilled(table, "model replaced")

		table.contentY = 600
		verifyAllRowsFilled(table, "scrolled")
	}

	// No time slice at all: only the viewport is filled at once, the queue fills the rest one row per tick
	function test_viewportRowsFillAtOnce(){
		let table = createTemporaryObject(tableComp, testCase.parent)
		table.rowFillSliceMs = 0
		table.headers = createHeaders()
		table.elements = createElements("c")

		let list = table.elementsList
		tryVerify(function(){ return list.count === rowCount && list.itemAtIndex(0) !== null }, 3000, "rows created")

		let viewportRows = Math.floor(list.height / table.itemHeight)
		verify(viewportRows > 1, "viewport rows: " + viewportRows)

		let empty = emptyRows(table).filter(function(row){ return row < viewportRows })
		compare(empty.length, 0, "viewport rows without cells " + JSON.stringify(empty))

		verifyAllRowsFilled(table, "rows below the viewport")
	}
}
