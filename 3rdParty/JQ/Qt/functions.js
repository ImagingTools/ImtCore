module.exports = {
    qsTr: function(str){return str},
    binding: function(func, lazy=false){
        func.bound = true
        func.lazy = lazy
        return func
    },
    locale: function(){
        return {
            decimalPoint: (1.1).toLocaleString().substring(1, 2)
        }
    },
    point(x, y){
        return {
            x: x,
            y: y,
        }
    },
    createComponent(source){
        let path = source.replaceAll('qrc:/', '').replaceAll('.qml', '').split('/')
        let className = path[path.length-1]

        let cls = null
        try {
            cls = eval(className)
        } catch (error) {
            while(path.length){
                if(cls){
                    let name = path.shift()
                    if(name in cls){
                        cls = cls[name]
                    }
                } else {
                    cls = JQModules[path.shift()]
                }
            }
        }

        return JQModules.QtQml.Component.create(null, cls)
    },
}