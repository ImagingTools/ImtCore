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
}