export var Qt = {
    NoButton: 0x00000000,
    AllButtons: 0x07ffffff,
    LeftButton: 0x00000001,
    RightButton: 0x00000002,
    MiddleButton: 0x00000004,

    NoModifier: 0x00000000,
    ShiftModifier: 0x02000000,
    ControlModifier: 0x04000000,
    AltModifier: 0x08000000,
    MetaModifier: 0x10000000,

    ArrowCursor: 'default',
    UpArrowCursor: 'none',
    CrossCursor: 'crosshair',
    IBeamCursor: 'text',
    WaitCursor: 'wait',
    BusyCursor: 'progress',
    ForbiddenCursor: 'not-allowed',
    PointingHandCursor: 'pointer',
    WhatsThisCursor: 'help',
    DragMoveCursor: 'none',
    DragLinkCursor: 'none',
    SizeVerCursor: 'n-resize',
    SizeHorCursor: 'e-resize',
    SizeBDiagCursor: 'ne-resize',
    SizeFDiagCursor: 'nw-resize',
    SizeAllCursor: 'all-scroll',
    SplitVCursor: 'row-resize',
    SplitHCursor: 'col-resize',
    OpenHandCursor: 'grab',
    ClosedHandCursor: 'grabbing',
    DragCopyCursor: 'none',

    WindowShortcut: 0,
    ApplicationShortcut: 1,

    Unchecked: 0,
    PartiallyChecked: 1,
    Checked: 2,

    point: function(x, y){
        return {}
    },
    
    openUrlExternally: function(url){
        return window.open(url, '_blank')
    },
    qsTr: function(sourceText){
        let translate = Core.context.languages[`${Core.context.application}_${Core.context.language}`]
        let result = translate ? translate[sourceText] : sourceText
        return result ? result : sourceText
    },

    $colors: {
        "aliceblue":"#f0f8ff","antiquewhite":"#faebd7","aqua":"#00ffff","aquamarine":"#7fffd4","azure":"#f0ffff","beige":"#f5f5dc","bisque":"#ffe4c4","black":"#000000","blanchedalmond":"#ffebcd","blue":"#0000ff","blueviolet":"#8a2be2","brown":"#a52a2a","burlywood":"#deb887",
        "cadetblue":"#5f9ea0","chartreuse":"#7fff00","chocolate":"#d2691e","coral":"#ff7f50","cornflowerblue":"#6495ed","cornsilk":"#fff8dc","crimson":"#dc143c","cyan":"#00ffff",
        "darkblue":"#00008b","darkcyan":"#008b8b","darkgoldenrod":"#b8860b","darkgray":"#a9a9a9","darkgreen":"#006400","darkkhaki":"#bdb76b","darkmagenta":"#8b008b","darkolivegreen":"#556b2f",
        "darkorange":"#ff8c00","darkorchid":"#9932cc","darkred":"#8b0000","darksalmon":"#e9967a","darkseagreen":"#8fbc8f","darkslateblue":"#483d8b","darkslategray":"#2f4f4f","darkturquoise":"#00ced1",
        "darkviolet":"#9400d3","deeppink":"#ff1493","deepskyblue":"#00bfff","dimgray":"#696969","dodgerblue":"#1e90ff","firebrick":"#b22222","floralwhite":"#fffaf0","forestgreen":"#228b22","fuchsia":"#ff00ff",
        "gainsboro":"#dcdcdc","ghostwhite":"#f8f8ff","gold":"#ffd700","goldenrod":"#daa520","gray":"#808080","green":"#008000","greenyellow":"#adff2f","honeydew":"#f0fff0","hotpink":"#ff69b4","indianred ":"#cd5c5c","indigo":"#4b0082","ivory":"#fffff0","khaki":"#f0e68c",
        "lavender":"#e6e6fa","lavenderblush":"#fff0f5","lawngreen":"#7cfc00","lemonchiffon":"#fffacd","lightblue":"#add8e6","lightcoral":"#f08080","lightcyan":"#e0ffff","lightgoldenrodyellow":"#fafad2",
        "lightgrey":"#d3d3d3","lightgreen":"#90ee90","lightpink":"#ffb6c1","lightsalmon":"#ffa07a","lightseagreen":"#20b2aa","lightskyblue":"#87cefa","lightslategray":"#778899","lightsteelblue":"#b0c4de",
        "lightyellow":"#ffffe0","lime":"#00ff00","limegreen":"#32cd32","linen":"#faf0e6","magenta":"#ff00ff","maroon":"#800000","mediumaquamarine":"#66cdaa","mediumblue":"#0000cd","mediumorchid":"#ba55d3","mediumpurple":"#9370d8","mediumseagreen":"#3cb371","mediumslateblue":"#7b68ee",
        "mediumspringgreen":"#00fa9a","mediumturquoise":"#48d1cc","mediumvioletred":"#c71585","midnightblue":"#191970","mintcream":"#f5fffa","mistyrose":"#ffe4e1","moccasin":"#ffe4b5",
        "navajowhite":"#ffdead","navy":"#000080","oldlace":"#fdf5e6","olive":"#808000","olivedrab":"#6b8e23","orange":"#ffa500","orangered":"#ff4500","orchid":"#da70d6",
        "palegoldenrod":"#eee8aa","palegreen":"#98fb98","paleturquoise":"#afeeee","palevioletred":"#d87093","papayawhip":"#ffefd5","peachpuff":"#ffdab9","peru":"#cd853f","pink":"#ffc0cb","plum":"#dda0dd","powderblue":"#b0e0e6","purple":"#800080",
        "rebeccapurple":"#663399","red":"#ff0000","rosybrown":"#bc8f8f","royalblue":"#4169e1","saddlebrown":"#8b4513","salmon":"#fa8072","sandybrown":"#f4a460","seagreen":"#2e8b57","seashell":"#fff5ee","sienna":"#a0522d","silver":"#c0c0c0","skyblue":"#87ceeb","slateblue":"#6a5acd","slategray":"#708090","snow":"#fffafa","springgreen":"#00ff7f","steelblue":"#4682b4",
        "tan":"#d2b48c","teal":"#008080","thistle":"#d8bfd8","tomato":"#ff6347","turquoise":"#40e0d0","violet":"#ee82ee","wheat":"#f5deb3","white":"#ffffff","whitesmoke":"#f5f5f5","yellow":"#ffff00","yellowgreen":"#9acd32"
    },
    $colorToRGBA(color){
        if(!color){
            return {
                r: 255,
                g: 255,
                b: 255,
                a: 0,
            }
        }
        let hexColor = '#ffffff'
        let alpha = false
        if(this.$colors[color]){
            hexColor = this.$colors[color]
        } else {
            if(color[0] === '#') {
                if(color.length === 4){
                    hexColor = `#${color[1]}${color[1]}${color[2]}${color[2]}${color[3]}${color[3]}`
                } else if(color.length === 7) {
                    hexColor = color
                } else if(color.length === 5) {
                    alpha = true
                    hexColor = `#${color[1]}${color[1]}${color[2]}${color[2]}${color[3]}${color[3]}${color[4]}${color[4]}`
                } else if(color.length === 9) {
                    alpha = true
                    hexColor = color
                } else {
                    hexColor = color
                }
            } else {
                return {
                    r: 255,
                    g: 255,
                    b: 255,
                    a: 0,
                }
            }
        }
        if(alpha){
            return {
                r: parseInt(hexColor[3]+hexColor[4], 16),
                g: parseInt(hexColor[5]+hexColor[6], 16),
                b: parseInt(hexColor[7]+hexColor[8], 16),
                a: parseInt(hexColor[1]+hexColor[2], 16)/255,
            }
        } else {
            return {
                r: parseInt(hexColor[1]+hexColor[2], 16),
                g: parseInt(hexColor[3]+hexColor[4], 16),
                b: parseInt(hexColor[5]+hexColor[6], 16),
                a: 1,
            }
        }
        

    }
}