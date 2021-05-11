import QtQuick 2.0

JSONListModel {
    id: container
    
    function get(row){
	return container.model.get(row)
    }
    
    function GetData(key, row){
        if(row === undefined)
	row = 0
        if(row === null)
	row = 0
        var modelObject = get(row)
        
        var retVal = modelObject[key]
//        var retVal = modelObject.firstName
        return  retVal
//        return container.model.get(0).firstName
    }
}
