import QtQuick

Item {
    id: root
    property int rowCount: 10000
    property int cellCount: 15

    TableData {
        id: tableData
    }

    Component.onCompleted: {
        let _data = {
            rows: []
        }

        for(let ri = 0; ri < root.rowCount; ri++){
            let row = {
                cells: []
            }

            for(let ci = 0; ci < root.cellCount; ci++){
                let cell = {
                    text: 'ri'+ri+'ci'+ci,
                    textColor: 'black',
                    iconSource: '',
                    fontFamily: '',
                    fontSize: 14
                }

                row.cells.push(cell)
            }

            _data.rows.push(row)
        }

        let _dataJSON = JSON.stringify(_data)

        let time = new Date().getTime()
        let obj = JSON.parse(_dataJSON)
        console.log('simple JSON build', new Date().getTime() - time, 'ms')

        time = new Date().getTime()
        tableData.fromJSON(_dataJSON)
        console.log('tableData build', new Date().getTime() - time, 'ms')

        Qt.quit()
    }
}