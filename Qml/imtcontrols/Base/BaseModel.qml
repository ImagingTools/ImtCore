import QtQuick 2.0

ListModel {
    function getProperties(item){
        let list = []
        if(Qt.platform.os === 'web'){
            for(let key in item.$properties){
                if(
                    key.indexOf('m_') >= 0
                    && typeof item[key] !== "function"
                    && item[key] !== undefined
                    && item[key] !== null){
                    list.push(key)
                }
            }
        } else {
            for(let key in item){
                if(
                    key.indexOf('m_') >= 0
                    && typeof item[key] !== "function"
                    && item[key] !== undefined
                    && item[key] !== null){
                    list.push(key)
                }
            }
        }

        return list
    }

    function toJson(){
        let json = '['
        for(let i = 0; i < count; i++){
            let item = get(i).item
            let list = getProperties(item)

            json += '{'
            for(let j = 0; j < list.length; j++){
                let key = list[j]
                if(typeof item[key] === 'object'){
                    if (typeof item[key].toJson === "function"){
                        json += '"' + item.getJSONKeyForProperty(key) + '":' + item[key].toJson()
                    }
                } else {
                    let value = item[key]
                    if (value === undefined){
                        value = null
                    }
                    let safeValue = item[key]
                    if (typeof safeValue === 'string'){
                        safeValue = safeValue.replace(/\\/g, '\u005C\u005C')
                        safeValue = safeValue.replace(/\"/g,'\u005C"')
                    }
                    json += '"' + item.getJSONKeyForProperty(key) + '":' + (typeof item[key] === 'string' ? '"' + safeValue + '"' : value)
                }
                if(j < list.length - 1) json += ','
            }
            json +='}'

            if(i < count - 1) json += ','
        }
        json +=']'
        return json
    }

    function toGraphQL(){
        let graphQL = '['
        for(let i = 0; i < count; i++){
            let item = get(i).item
            let list = getProperties(item)

            graphQL += '{'
            for(let j = 0; j < list.length; j++){
                let key = list[j]
                if(typeof item[key] === 'object'){
                    graphQL += item.getJSONKeyForProperty(key) + ':' + item[key].toGraphQL()
                } else {
                    let value = item[key]
                    if (value === undefined){
                        value = null
                    }
                    graphQL += item.getJSONKeyForProperty(key) + ':' + (typeof item[key] === 'string' ? '"' + item[key] + '"' : value)
                }
                if(j < list.length - 1) graphQL += ','
            }
            graphQL +='}'

            if(i < count - 1) graphQL += ','
        }
        graphQL +=']'
        return graphQL
    }
}
