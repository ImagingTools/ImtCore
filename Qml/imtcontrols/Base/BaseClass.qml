import QtQuick 2.0

QtObject {
    signal propertyChanged(string name, var sender)
    property bool enableNotifications: true

    Component.onCompleted: {
        // let list = getProperties()
        // for(let name of list){
        //     this[name+'Changed'].connect(()=>{
        //                                     console.log('**TEST**', name+'Changed')
        //                                  })
        // }
    }

    function getType(name){

    }

    function getTypeForArray(name){

    }

    function getJSONKeyForProperty(propertyId){
        return propertyId
    }

    function getProperties(){
        let list = []
        for(let key in this){
            if(key !== 'objectName' && key !== 'enableNotifications' && typeof this[key] !== "function"){
                list.push(key)
            }
        }

        return list
    }

    function toJSON(){
        let list = getProperties()

        let json = '{'
        for(let i = 0; i < list.length; i++){
            let key = list[i]
            if(typeof this[key] === 'object'){
                json += '"' + this.getJSONKeyForProperty(key) + '":' + this[key].toJSON()
            } else {
                json += '"' + this.getJSONKeyForProperty(key) + '":' + (typeof this[key] === 'string' ? '"' + this[key] + '"' : this[key])
            }
            if(i < list.length - 1) json += ','
        }
        json +='}'
        return json
    }

    function fromJSON(json){
        return fromObject(JSON.parse(json))
    }

    function fromObject(sourceObject){
        for(let key in sourceObject){
            let _key = "m_" + key
            if(typeof sourceObject[key] === "object"){
                if(Array.isArray(sourceObject[key])){
                    if(this[_key]){
                        this[_key].clear()
                    } else {
                        let obj = Qt.createComponent('BaseModel.qml').createObject(this)
                        this[_key] = obj
                    }
                    for(let sourceObjectInner of sourceObject[key]){
                        let obj = createComponent(_key).createObject()
                        obj.fromObject(sourceObjectInner)
                        this[_key].append({item: obj})
                    }
                } else {
                    let obj = createComponent(_key).createObject(this)
                    obj.fromObject(sourceObject[key])
                    this[_key] = obj
                }
            } else {
                this[_key] = sourceObject[key]
            }

        }
    }
}
