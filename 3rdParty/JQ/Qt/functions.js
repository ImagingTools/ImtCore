module.exports = {
    md5(inputString) {
        var hc="0123456789abcdef";
        function rh(n) {var j,s="";for(j=0;j<=3;j++) s+=hc.charAt((n>>(j*8+4))&0x0F)+hc.charAt((n>>(j*8))&0x0F);return s;}
        function ad(x,y) {var l=(x&0xFFFF)+(y&0xFFFF);var m=(x>>16)+(y>>16)+(l>>16);return (m<<16)|(l&0xFFFF);}
        function rl(n,c)            {return (n<<c)|(n>>>(32-c));}
        function cm(q,a,b,x,s,t)    {return ad(rl(ad(ad(a,q),ad(x,t)),s),b);}
        function ff(a,b,c,d,x,s,t)  {return cm((b&c)|((~b)&d),a,b,x,s,t);}
        function gg(a,b,c,d,x,s,t)  {return cm((b&d)|(c&(~d)),a,b,x,s,t);}
        function hh(a,b,c,d,x,s,t)  {return cm(b^c^d,a,b,x,s,t);}
        function ii(a,b,c,d,x,s,t)  {return cm(c^(b|(~d)),a,b,x,s,t);}
        function sb(x) {
            var i;var nblk=((x.length+8)>>6)+1;var blks=new Array(nblk*16);for(i=0;i<nblk*16;i++) blks[i]=0;
            for(i=0;i<x.length;i++) blks[i>>2]|=x.charCodeAt(i)<<((i%4)*8);
            blks[i>>2]|=0x80<<((i%4)*8);blks[nblk*16-2]=x.length*8;return blks;
        }
        var i,x=sb(""+inputString),a=1732584193,b=-271733879,c=-1732584194,d=271733878,olda,oldb,oldc,oldd;
        for(i=0;i<x.length;i+=16) {olda=a;oldb=b;oldc=c;oldd=d;
            a=ff(a,b,c,d,x[i+ 0], 7, -680876936);d=ff(d,a,b,c,x[i+ 1],12, -389564586);c=ff(c,d,a,b,x[i+ 2],17,  606105819);
            b=ff(b,c,d,a,x[i+ 3],22,-1044525330);a=ff(a,b,c,d,x[i+ 4], 7, -176418897);d=ff(d,a,b,c,x[i+ 5],12, 1200080426);
            c=ff(c,d,a,b,x[i+ 6],17,-1473231341);b=ff(b,c,d,a,x[i+ 7],22,  -45705983);a=ff(a,b,c,d,x[i+ 8], 7, 1770035416);
            d=ff(d,a,b,c,x[i+ 9],12,-1958414417);c=ff(c,d,a,b,x[i+10],17,     -42063);b=ff(b,c,d,a,x[i+11],22,-1990404162);
            a=ff(a,b,c,d,x[i+12], 7, 1804603682);d=ff(d,a,b,c,x[i+13],12,  -40341101);c=ff(c,d,a,b,x[i+14],17,-1502002290);
            b=ff(b,c,d,a,x[i+15],22, 1236535329);a=gg(a,b,c,d,x[i+ 1], 5, -165796510);d=gg(d,a,b,c,x[i+ 6], 9,-1069501632);
            c=gg(c,d,a,b,x[i+11],14,  643717713);b=gg(b,c,d,a,x[i+ 0],20, -373897302);a=gg(a,b,c,d,x[i+ 5], 5, -701558691);
            d=gg(d,a,b,c,x[i+10], 9,   38016083);c=gg(c,d,a,b,x[i+15],14, -660478335);b=gg(b,c,d,a,x[i+ 4],20, -405537848);
            a=gg(a,b,c,d,x[i+ 9], 5,  568446438);d=gg(d,a,b,c,x[i+14], 9,-1019803690);c=gg(c,d,a,b,x[i+ 3],14, -187363961);
            b=gg(b,c,d,a,x[i+ 8],20, 1163531501);a=gg(a,b,c,d,x[i+13], 5,-1444681467);d=gg(d,a,b,c,x[i+ 2], 9,  -51403784);
            c=gg(c,d,a,b,x[i+ 7],14, 1735328473);b=gg(b,c,d,a,x[i+12],20,-1926607734);a=hh(a,b,c,d,x[i+ 5], 4,    -378558);
            d=hh(d,a,b,c,x[i+ 8],11,-2022574463);c=hh(c,d,a,b,x[i+11],16, 1839030562);b=hh(b,c,d,a,x[i+14],23,  -35309556);
            a=hh(a,b,c,d,x[i+ 1], 4,-1530992060);d=hh(d,a,b,c,x[i+ 4],11, 1272893353);c=hh(c,d,a,b,x[i+ 7],16, -155497632);
            b=hh(b,c,d,a,x[i+10],23,-1094730640);a=hh(a,b,c,d,x[i+13], 4,  681279174);d=hh(d,a,b,c,x[i+ 0],11, -358537222);
            c=hh(c,d,a,b,x[i+ 3],16, -722521979);b=hh(b,c,d,a,x[i+ 6],23,   76029189);a=hh(a,b,c,d,x[i+ 9], 4, -640364487);
            d=hh(d,a,b,c,x[i+12],11, -421815835);c=hh(c,d,a,b,x[i+15],16,  530742520);b=hh(b,c,d,a,x[i+ 2],23, -995338651);
            a=ii(a,b,c,d,x[i+ 0], 6, -198630844);d=ii(d,a,b,c,x[i+ 7],10, 1126891415);c=ii(c,d,a,b,x[i+14],15,-1416354905);
            b=ii(b,c,d,a,x[i+ 5],21,  -57434055);a=ii(a,b,c,d,x[i+12], 6, 1700485571);d=ii(d,a,b,c,x[i+ 3],10,-1894986606);
            c=ii(c,d,a,b,x[i+10],15,   -1051523);b=ii(b,c,d,a,x[i+ 1],21,-2054922799);a=ii(a,b,c,d,x[i+ 8], 6, 1873313359);
            d=ii(d,a,b,c,x[i+15],10,  -30611744);c=ii(c,d,a,b,x[i+ 6],15,-1560198380);b=ii(b,c,d,a,x[i+13],21, 1309151649);
            a=ii(a,b,c,d,x[i+ 4], 6, -145523070);d=ii(d,a,b,c,x[i+11],10,-1120210379);c=ii(c,d,a,b,x[i+ 2],15,  718787259);
            b=ii(b,c,d,a,x[i+ 9],21, -343485551);a=ad(a,olda);b=ad(b,oldb);c=ad(c,oldc);d=ad(d,oldd);
        }
        return rh(a)+rh(b)+rh(c)+rh(d);
    },
    rgba(r, g, b, a){
        let _a = Math.trunc(a*255)
        let _r = Math.trunc(r*255)
        let _g = Math.trunc(g*255)
        let _b = Math.trunc(b*255)

        _a = (_a < 16 ? '0' : '') + _a.toString(16)
        _r = (_r < 16 ? '0' : '') + _r.toString(16)
        _g = (_g < 16 ? '0' : '') + _g.toString(16)
        _b = (_b < 16 ? '0' : '') + _b.toString(16)

        return `#${_r}${_g}${_b}${_a}`
    },
    rgbToHsl(r, g, b) {
        r /= 255, g /= 255, b /= 255;
      
        var max = Math.max(r, g, b), min = Math.min(r, g, b);
        var h, s, l = (max + min) / 2;
      
        if (max == min) {
          h = s = 0;
        } else {
          var d = max - min;
          s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
      
          switch (max) {
            case r: h = (g - b) / d + (g < b ? 6 : 0); break;
            case g: h = (b - r) / d + 2; break;
            case b: h = (r - g) / d + 4; break;
          }
      
          h /= 6;
        }
      
        return [ h, s, l ];
    },
    hslToRgb(h, s, l) {
        var r, g, b;
      
        if (s == 0) {
          r = g = b = l; // achromatic
        } else {
          function hue2rgb(p, q, t) {
            if (t < 0) t += 1;
            if (t > 1) t -= 1;
            if (t < 1/6) return p + (q - p) * 6 * t;
            if (t < 1/2) return q;
            if (t < 2/3) return p + (q - p) * (2/3 - t) * 6;
            return p;
          }
      
          var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
          var p = 2 * l - q;
      
          r = hue2rgb(p, q, h + 1/3);
          g = hue2rgb(p, q, h);
          b = hue2rgb(p, q, h - 1/3);
        }
      
        return [ r * 255, g * 255, b * 255 ];
    },
    rgbToHsv(r, g, b) {
        r /= 255, g /= 255, b /= 255;
      
        var max = Math.max(r, g, b), min = Math.min(r, g, b);
        var h, s, v = max;
      
        var d = max - min;
        s = max == 0 ? 0 : d / max;
      
        if (max == min) {
          h = 0; // achromatic
        } else {
          switch (max) {
            case r: h = (g - b) / d + (g < b ? 6 : 0); break;
            case g: h = (b - r) / d + 2; break;
            case b: h = (r - g) / d + 4; break;
          }
      
          h /= 6;
        }
      
        return [ h, s, v ];
    },
    hsvToRgb(h, s, v) {
        var r, g, b;
      
        var i = Math.floor(h * 6);
        var f = h * 6 - i;
        var p = v * (1 - s);
        var q = v * (1 - f * s);
        var t = v * (1 - (1 - f) * s);
      
        switch (i % 6) {
          case 0: r = v, g = t, b = p; break;
          case 1: r = q, g = v, b = p; break;
          case 2: r = p, g = v, b = t; break;
          case 3: r = p, g = q, b = v; break;
          case 4: r = t, g = p, b = v; break;
          case 5: r = v, g = p, b = q; break;
        }
      
        return [ r * 255, g * 255, b * 255 ];
    },
    toRGBA(value){
        if(!value){
            return {
                r: 255,
                g: 255,
                b: 255,
                a: 0,
            }
        }
        let hexColor = '#ffffff'
        let alpha = false

        if(value[0] === '#') {
            if(value.length === 4){
                hexColor = `#${value[1]}${value[1]}${value[2]}${value[2]}${value[3]}${value[3]}`
            } else if(value.length === 7) {
                hexColor = value
            } else if(value.length === 5) {
                alpha = true
                hexColor = `#${value[1]}${value[1]}${value[2]}${value[2]}${value[3]}${value[3]}${value[4]}${value[4]}`
            } else if(value.length === 9) {
                alpha = true
                hexColor = value
            } else {
                hexColor = value
            }
        } else {
            return {
                r: 255,
                g: 255,
                b: 255,
                a: 0,
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
    },
    darker(baseColor, factor = 1.5){
        let rgba = this.toRGBA(baseColor)
        let hsv = this.rgbToHsv(rgba.r, rgba.g, rgba.b)
        rgba = this.hsvToRgb(hsv[0], hsv[0], hsv[0] / factor)
        let rh = rgba[0] > 15 ? rgba[0].toString(16) : '0' + rgba[0].toString(16)
        let gh = rgba[1] > 15 ? rgba[1].toString(16) : '0' + rgba[1].toString(16)
        let bh = rgba[2] > 15 ? rgba[2].toString(16) : '0' + rgba[2].toString(16)
        return `#${rh}${gh}${bh}`
    },
    lighter(baseColor, factor = 2.0){
        let rgba = this.toRGBA(baseColor)
        let hsv = this.rgbToHsv(rgba.r, rgba.g, rgba.b)
        rgba = this.hsvToRgb(hsv[0], hsv[0], hsv[0] * factor)
        let rh = rgba[0] > 15 ? rgba[0].toString(16) : '0' + rgba[0].toString(16)
        let gh = rgba[1] > 15 ? rgba[1].toString(16) : '0' + rgba[1].toString(16)
        let bh = rgba[2] > 15 ? rgba[2].toString(16) : '0' + rgba[2].toString(16)
        return `#${rh}${gh}${bh}`
    },
    tint(baseColor, tintColor){

    },
    openUrlExternally: function(url){
        return window.open(url, '_blank')
    },
    qsTr: function(sourceText){
        return global.JSContext.translate(sourceText)
    },
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
    rect(x, y, width, height){
        return {
            x: x,
            y: y,
            width: width,
            height: height,
            left: x,
            top: y,
            right: x + width,
            bottom: y + height
        }
    },
    atob: function(data){
        return atob(data)
    },
    btoa: function(data){
        return btoa(data)
    },
    createComponent(source, currentModule){
        let path = source.replaceAll('qrc:/', '').replaceAll('.qml', '').split('/')
        let className = path[path.length-1]

        if(className in JQModules.QtQml) return JQModules.QtQml.Component.create(null, {}, JQModules.QtQml[className])
        if(className in JQModules.QtQml.Models) return JQModules.QtQml.Component.create(null, {}, JQModules.QtQml.Models[className])

        let cls = currentModule

        for(let i = 0; i < path.length; i++){
            if(cls){
                let name = path[i]
                if(name in cls){
                    cls = cls[name]
                } else if(name in JQModules){
                    cls = JQModules[name]
                }
            } else {
                cls = JQModules[path[i]]
            }
        }

        if(!cls){
            try {
                cls = eval(className)
            } catch (error) {
    
            }
        }
        

        if(cls && cls.isAssignableFrom && cls.isAssignableFrom(JQModules.QtBase.BaseObject)) return JQModules.QtQml.Component.create(null, {}, cls)

        // let cls = currentModule
        // try {
        //     cls = eval(className)
        // } catch (error) {
        //     for(let i = 0; i < path.length; i++){
        //         if(cls){
        //             let name = path[i]
        //             if(name in cls){
        //                 cls = cls[name]
        //             }
        //         } else {
        //             cls = JQModules[path[i]]
        //         }
        //     }
        // }

        // if(cls && cls.isAssignableFrom && cls.isAssignableFrom(JQModules.QtBase.BaseObject)) return JQModules.QtQml.Component.create(null, {}, cls)

        // cls = null
        // try {
        //     cls = eval(className)
        // } catch (error) {
        //     for(let i = 0; i < path.length; i++){
        //         if(cls){
        //             let name = path[i]
        //             if(name in cls){
        //                 cls = cls[name]
        //             }
        //         } else {
        //             cls = JQModules[path[i]]
        //         }
        //     }
        // }

        // if(cls && cls.isAssignableFrom && cls.isAssignableFrom(JQModules.QtBase.BaseObject)) return JQModules.QtQml.Component.create(null, {}, cls)
    },

    get localStorage(){return global.localStorage},
}