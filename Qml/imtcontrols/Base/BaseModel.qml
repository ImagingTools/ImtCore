import QtQuick 2.0

ListModel {
    function getProperties(item){
        let list = []
        for(let key in item){
            if(key !== 'objectName' && key !== 'enableNotifications' && typeof item[key] !== "function" && item[key] !== undefined && item[key] !== null){
                list.push(key)
            }
        }

        return list
    }

    function toJSON(){
        let json = '['
        for(let i = 0; i < count; i++){
            let item = get(i).item
            let list = getProperties(item)

            json += '{'
            for(let j = 0; j < list.length; j++){
                let key = list[j]
                if(typeof item[key] === 'object'){
                    json += '"' + item.getJSONKeyForProperty(key) + '":' + item[key].toJSON()
                } else {
                    json += '"' + item.getJSONKeyForProperty(key) + '":' + (typeof item[key] === 'string' ? '"' + item[key] + '"' : item[key])
                }
                if(j < list.length - 1) json += ','
            }
            json +='}'

            if(i < count - 1) json += ','
        }
        json +=']'
        return json
    }
}
