const GroupProperty = require("./GroupProperty")
const AnchorLine = require("./AnchorLine")
const Real = require("./Real")
const Var = require("./Var")
const Signal = require("./Signal")
const QtFunctions = require("../Qt/functions")


class Anchors extends GroupProperty {
    static meta = {
        fill: { type: Var, value: undefined, signalName: 'fillChanged' },
        centerIn: { type: Var, value: undefined, signalName: 'centerInChanged' },

        left: { type: Var, value: undefined, signalName: 'leftChanged' },
        right: { type: Var, value: undefined, signalName: 'rightChanged' },
        top: { type: Var, value: undefined, signalName: 'topChanged' },
        bottom: { type: Var, value: undefined, signalName: 'bottomChanged' },
        horizontalCenter: { type: Var, value: undefined, signalName: 'horizontalCenterChanged' },
        verticalCenter: { type: Var, value: undefined, signalName: 'verticalCenterChanged' },

        leftMargin: { type: Real, value: 0, signalName: 'leftMarginChanged' },
        topMargin: { type: Real, value: 0, signalName: 'topMarginChanged' },
        rightMargin: { type: Real, value: 0, signalName: 'rightMarginChanged' },
        bottomMargin: { type: Real, value: 0, signalName: 'bottomMarginChanged' },
        margins: { type: Real, value: 0, signalName: 'marginsChanged' },
        // horizontalCenterOffset: { type: real, value: 0, signalName: 'numberChanged' },
        // verticalCenterOffset: { type: real, value: 0, signalName: 'numberChanged' },

        fillChanged: {type:Signal, slotName:'onFillChanged', args:[]},
        centerInChanged: {type:Signal, slotName:'onCenterInChanged', args:[]},


        leftMarginChanged: {type:Signal, slotName:'onLeftMarginChanged', args:[]},
        topMarginChanged: {type:Signal, slotName:'onTopMarginChanged', args:[]},
        rightMarginChanged: {type:Signal, slotName:'onRightMarginChanged', args:[]},
        bottomMarginChanged: {type:Signal, slotName:'onBottomMarginChanged', args:[]},

        leftChanged: {type:Signal, slotName:'onLeftChanged', args:[]},
        topChanged: {type:Signal, slotName:'onTopChanged', args:[]},
        rightChanged: {type:Signal, slotName:'onRightChanged', args:[]},
        bottomChanged: {type:Signal, slotName:'onBottomChanged', args:[]},
        horizontalCenterChanged: {type:Signal, slotName:'onHorizontalCenterChanged', args:[]},
        verticalCenterChanged: {type:Signal, slotName:'onVerticalCenterChanged', args:[]},

        marginsChanged: {type:Signal, slotName:'onMarginsChanged', args:[]},
    }

    SLOT_fillChanged(oldValue, newValue){
        let target = newValue
        let self = this.__parent.__proxy

        if(self.parent === target){
            self.x = ()=>{return this.__proxy.leftMargin}
            self.y = ()=>{return this.__proxy.topMargin}
            self.width = ()=>{return target.width - (this.__proxy.leftMargin + this.__proxy.rightMargin)}
            self.height = ()=>{return target.height - (this.__proxy.topMargin + this.__proxy.bottomMargin)}
        } else {
            self.x = ()=>{return target.x + this.__proxy.leftMargin}
            self.y = ()=>{return target.y + this.__proxy.topMargin}
            self.width = ()=>{return target.width - (this.__proxy.leftMargin + this.__proxy.rightMargin)}
            self.height = ()=>{return target.height - (this.__proxy.topMargin + this.__proxy.bottomMargin)}
        }
    }

    SLOT_centerInChanged(oldValue, newValue){
        let target = newValue
        let self = this.__parent.__proxy

        if(target){
            if(self.parent === target){
                self.x = ()=>{return target.width / 2 - self.width / 2}
                self.y = ()=>{return target.height / 2 - self.height / 2}
            } else {
                self.x = ()=>{return target.x + target.width / 2 - self.width / 2}
                self.y = ()=>{return target.y + target.height / 2 - self.height / 2}
            }
        }
        
    }

    SLOT_marginsChanged(oldValue, newValue){
        this.__proxy.leftMargin = this.margins
        this.__proxy.topMargin = this.margins
        this.__proxy.rightMargin = this.margins
        this.__proxy.bottomMargin = this.margins
    }

    SLOT_horizontalCenterChanged(oldValue, newValue){
        let target = newValue
        let self = this.__parent.__proxy

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent.__proxy

            if(self.parent === target){
                if(targetFloat === AnchorLine.Left){
                    self.x = ()=>{return -self.width / 2}
                } else if(targetFloat === AnchorLine.Right){
                    self.x = ()=>{return target.width - self.width / 2}
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.x = ()=>{return target.width / 2 - self.width / 2}
                }
            } else {
                if(targetFloat === AnchorLine.Left){
                    self.x = ()=>{return target.x - self.width / 2}
                } else if(targetFloat === AnchorLine.Right){
                    self.x = ()=>{return target.x + target.width - self.width / 2}
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.x = ()=>{return target.x + target.width / 2 - self.width / 2}
                }
            }
        }
    }

    SLOT_verticalCenterChanged(oldValue, newValue){
        let target = newValue
        let self = this.__parent.__proxy

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent.__proxy

            if(self.parent === target){
                if(targetFloat === AnchorLine.Top){
                    self.y = ()=>{return -self.height / 2}
                } else if(targetFloat === AnchorLine.Bottom){
                    self.y = ()=>{return target.height - self.height / 2}
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.y = ()=>{return target.height / 2 - self.height / 2}
                }
            } else {
                if(targetFloat === AnchorLine.Top){
                    self.y = ()=>{return target.y - self.height / 2}
                } else if(targetFloat === AnchorLine.Bottom){
                    self.y = ()=>{return target.y + target.height - self.height / 2}
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.y = ()=>{return target.y + target.height / 2 - self.height / 2}
                }               
            }
        }
    }

    SLOT_leftChanged(oldValue, newValue){
        if(newValue && newValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy 
                if(self){
                    if(this.__for_x === 2){
                        self.x = self.x
                        self.width = self.width
                    } else if(this.__for_x === 1){
                        self.x = self.x
                    }
                }
                this.left = null
            }
            destructionFunc.meta = {
                name: 'anchors_left__destruction',
                destruction: true,
                parent: newValue.__parent.__proxy
            }
            newValue.__parent.__proxy.__addLink()
            if(newValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = newValue.__parent.__proxy
                newValue.__parent.__proxy['Component.destruction'].connect(destructionFunc)
            }
        }
        if(oldValue && oldValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy 
                if(self){
                    if(this.__for_x === 2){
                        self.x = self.x
                        self.width = self.width
                    } else if(this.__for_x === 1){
                        self.x = self.x
                    }
                }
                this.left = null
            }
            destructionFunc.meta = {
                name: 'anchors_left__destruction',
                destruction: true,
                parent: oldValue.__parent.__proxy
            }
            if(oldValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = oldValue.__parent.__proxy
                oldValue.__parent.__proxy['Component.destruction'].disconnect(destructionFunc)
            }
            oldValue.__parent.__proxy.__removeLink()
        }

        this.onLeftRightChanged()
    }

    SLOT_rightChanged(oldValue, newValue){
        if(newValue && newValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy 
                if(self){
                    if(this.__for_x === 2){
                        self.x = self.x
                        self.width = self.width
                    } else if(this.__for_x === 1){
                        self.x = self.x
                    }
                }
                this.right = null
            }
            destructionFunc.meta = {
                name: 'anchors_right__destruction',
                destruction: true,
                parent: newValue.__parent.__proxy
            }
            newValue.__parent.__proxy.__addLink()
            if(newValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = newValue.__parent.__proxy
                newValue.__parent.__proxy['Component.destruction'].connect(destructionFunc)
            }
        }
        if(oldValue && oldValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy 
                if(self){
                    if(this.__for_x === 2){
                        self.x = self.x
                        self.width = self.width
                    } else if(this.__for_x === 1){
                        self.x = self.x
                    }
                }
                this.right = null
            }
            destructionFunc.meta = {
                name: 'anchors_right__destruction',
                destruction: true,
                parent: oldValue.__parent.__proxy
            }
            if(oldValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = oldValue.__parent.__proxy
                oldValue.__parent.__proxy['Component.destruction'].disconnect(destructionFunc)
            }
            oldValue.__parent.__proxy.__removeLink()
        }

        this.onLeftRightChanged()
    }

    onLeftRightChanged(){
        let left = this.__proxy.left
        let right = this.__proxy.right
        let self = this.__parent.__proxy     

        this.__for_x = 0

        if(self && left && right){
            this.__for_x = 2

            let leftFloat = left.__float
            left = left.__parent.__proxy

            let rightFloat = right.__float
            right = right.__parent.__proxy

            if(self.parent === left){
                if(leftFloat === AnchorLine.Left){
                    self.x = ()=>{return this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.Right){
                    self.x = ()=>{return left.width + this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.HorizontalCenter){
                    self.x = ()=>{return left.width / 2 + this.__proxy.leftMargin}
                }
            } else {
                if(leftFloat === AnchorLine.Left){
                    self.x = ()=>{return left.x + this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.Right){
                    self.x = ()=>{return left.x + left.width + this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.HorizontalCenter){
                    self.x = ()=>{return left.x + left.width / 2 + this.__proxy.leftMargin}
                }
            }

            if(self.parent === right){
                if(rightFloat === AnchorLine.Left){
                    self.width = ()=>{return -this.__proxy.rightMargin - self.x}
                } else if(rightFloat === AnchorLine.Right){
                    self.width = ()=>{return right.width - this.__proxy.rightMargin - self.x}
                } else if(rightFloat === AnchorLine.HorizontalCenter){
                    self.width = ()=>{return right.width / 2 - this.__proxy.rightMargin - self.x}
                }
            } else {
                if(rightFloat === AnchorLine.Left){
                    self.width = ()=>{return right.x - this.__proxy.rightMargin - self.x}
                } else if(rightFloat === AnchorLine.Right){
                    self.width = ()=>{return right.x + right.width - this.__proxy.rightMargin - self.x}
                } else if(rightFloat === AnchorLine.HorizontalCenter){
                    self.width = ()=>{return right.x + right.width / 2 - this.__proxy.rightMargin - self.x}
                }
            }
        } else if(self && left){
            this.__for_x = 1

            let leftFloat = left.__float
            left = left.__parent.__proxy

            if(self.parent === left){
                if(leftFloat === AnchorLine.Left){
                    self.x = ()=>{return this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.Right){
                    self.x = ()=>{return left.width + this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.HorizontalCenter){
                    self.x = ()=>{return left.width / 2 + this.__proxy.leftMargin}
                }
            } else {
                if(leftFloat === AnchorLine.Left){
                    self.x = ()=>{return left.x + this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.Right){
                    self.x = ()=>{return left.x + left.width + this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.HorizontalCenter){
                    self.x = ()=>{return left.x + left.width / 2 + this.__proxy.leftMargin}
                }            
            }
        } else if(self && right){
            this.__for_x = 1

            let rightFloat = right.__float
            right = right.__parent.__proxy

            if(self.parent === right){
                if(rightFloat === AnchorLine.Left){
                    self.x = ()=>{return -self.width - this.__proxy.rightMargin}
                } else if(rightFloat === AnchorLine.Right){
                    self.x = ()=>{return right.width - self.width - this.__proxy.rightMargin}
                } else if(rightFloat === AnchorLine.HorizontalCenter){
                    self.x = ()=>{return right.width / 2 - self.width - this.__proxy.rightMargin}
                }
            } else {
                if(rightFloat === AnchorLine.Left){
                    self.x = ()=>{return right.x - self.width - this.__proxy.rightMargin}
                } else if(rightFloat === AnchorLine.Right){
                    self.x = ()=>{return right.x + right.width - self.width - this.__proxy.rightMargin}
                } else if(rightFloat === AnchorLine.HorizontalCenter){
                    self.x = ()=>{return right.x + right.width / 2 - self.width - this.__proxy.rightMargin}
                }              
            }
        }
    }

    SLOT_topChanged(oldValue, newValue){
        if(newValue && newValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy 
                if(self){
                    if(this.__for_y === 2){
                        self.y = self.y
                        self.height = self.height
                    } else if(this.__for_y === 1){
                        self.y = self.y
                    }
                }
                this.top = null
            }
            destructionFunc.meta = {
                name: 'anchors_top__destruction',
                destruction: true,
                parent: newValue.__parent.__proxy
            }
            newValue.__parent.__proxy.__addLink()
            if(newValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = newValue.__parent.__proxy
                newValue.__parent.__proxy['Component.destruction'].connect(destructionFunc)
            }
        }
        if(oldValue && oldValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy
                if(self){ 
                    if(this.__for_y === 2){
                        self.y = self.y
                        self.height = self.height
                    } else if(this.__for_y === 1){
                        self.y = self.y
                    }
                }
                this.top = null
            }
            destructionFunc.meta = {
                name: 'anchors_top__destruction',
                destruction: true,
                parent: oldValue.__parent.__proxy
            }
            if(oldValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = oldValue.__parent.__proxy
                oldValue.__parent.__proxy['Component.destruction'].disconnect(destructionFunc)
            }
            oldValue.__parent.__proxy.__removeLink()
        }

        this.onTopBottomChanged()
    }

    SLOT_bottomChanged(oldValue, newValue){
        this.onTopBottomChanged()
        
        if(newValue && newValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy 
                if(self){
                    if(this.__for_y === 2){
                        self.y = self.y
                        self.height = self.height
                    } else if(this.__for_y === 1){
                        self.y = self.y
                    }
                }
                this.bottom = null
            }
            destructionFunc.meta = {
                name: 'anchors_bottom__destruction',
                destruction: true,
                parent: newValue.__parent.__proxy
            }
            newValue.__parent.__proxy.__addLink()
            if(newValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = newValue.__parent.__proxy
                newValue.__parent.__proxy['Component.destruction'].connect(destructionFunc)
            }
        }
        if(oldValue && oldValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy 
                if(self){
                    if(this.__for_y === 2){
                        self.y = self.y
                        self.height = self.height
                    } else if(this.__for_y === 1){
                        self.y = self.y
                    }
                    this.bottom = null
                }
                
            }
            destructionFunc.meta = {
                name: 'anchors_bottom__destruction',
                destruction: true,
                parent: oldValue.__parent.__proxy
            }
            if(oldValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = oldValue.__parent.__proxy
                oldValue.__parent.__proxy['Component.destruction'].disconnect(destructionFunc)
            }
            oldValue.__parent.__proxy.__removeLink()
        }
    }

    onTopBottomChanged(){
        let top = this.__proxy.top
        let bottom = this.__proxy.bottom
        let self = this.__parent.__proxy

        this.__for_y = 0

        if(self && top && bottom){
            this.__for_y = 2

            let topFloat = top.__float
            top = top.__parent.__proxy

            let bottomFloat = bottom.__float
            bottom = bottom.__parent.__proxy

            if(self.parent === top){
                if(topFloat === AnchorLine.Top){
                    self.y = ()=>{return this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.Bottom){
                    self.y = ()=>{return top.height + this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.VerticalCenter){
                    self.y = ()=>{return top.height / 2 + this.__proxy.topMargin}
                }
            } else {
                if(topFloat === AnchorLine.Top){
                    self.y = ()=>{return top.y + this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.Bottom){
                    self.y = ()=>{return top.y + top.height + this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.VerticalCenter){
                    self.y = ()=>{return top.y + top.height / 2 + this.__proxy.topMargin}
                }
            }

            if(self.parent === bottom){
                if(bottomFloat === AnchorLine.Top){
                    self.height = ()=>{return -this.__proxy.bottomMargin - self.y}
                } else if(bottomFloat === AnchorLine.Bottom){
                    self.height = ()=>{return bottom.height - this.__proxy.bottomMargin - self.y}
                } else if(bottomFloat === AnchorLine.VerticalCenter){
                    self.height = ()=>{return bottom.height / 2 - this.__proxy.bottomMargin - self.y}
                }
            } else {
                if(bottomFloat === AnchorLine.Top){
                    self.height = ()=>{return bottom.y - this.__proxy.bottomMargin - self.y}
                } else if(bottomFloat === AnchorLine.Bottom){
                    self.height = ()=>{return bottom.y + bottom.height - this.__proxy.bottomMargin - self.y}
                } else if(bottomFloat === AnchorLine.VerticalCenter){
                    self.height = ()=>{return bottom.y + bottom.height / 2 - this.__proxy.bottomMargin - self.y}
                }
            }
        } else if(self && top){
            this.__for_y = 1

            let topFloat = top.__float
            top = top.__parent.__proxy

            if(self.parent === top){
                if(topFloat === AnchorLine.Top){
                    self.y = ()=>{return this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.Bottom){
                    self.y = ()=>{return top.height + this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.VerticalCenter){
                    self.y = ()=>{return top.height / 2 + this.__proxy.topMargin}
                }
            } else {
                if(topFloat === AnchorLine.Top){
                    self.y = ()=>{return top.y + this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.Bottom){
                    self.y = ()=>{return top.y + top.height + this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.VerticalCenter){
                    self.y = ()=>{return top.y + top.height / 2 + this.__proxy.topMargin}
                }               
            }
        } else if(self && bottom){
            this.__for_y = 1

            let bottomFloat = bottom.__float
            bottom = bottom.__parent.__proxy

            if(self.parent === bottom){
                if(bottomFloat === AnchorLine.Top){
                    self.y = ()=>{return -self.height - this.__proxy.bottomMargin}
                } else if(bottomFloat === AnchorLine.Bottom){
                    self.y = ()=>{return bottom.height - self.height - this.__proxy.bottomMargin}
                } else if(bottomFloat === AnchorLine.VerticalCenter){
                    self.y = ()=>{return bottom.height / 2 - self.height - this.__proxy.bottomMargin}
                }
            } else {
                if(bottomFloat === AnchorLine.Top){
                    self.y = ()=>{return bottom.y - self.height - this.__proxy.bottomMargin}
                } else if(bottomFloat === AnchorLine.Bottom){
                    self.y = ()=>{return bottom.y + bottom.height - self.height - this.__proxy.bottomMargin}
                } else if(bottomFloat === AnchorLine.VerticalCenter){
                    self.y = ()=>{return bottom.y + bottom.height / 2 - self.height - this.__proxy.bottomMargin}
                }               
            }
        }
    }
}



module.exports = Anchors