// 观察者构造函数
function Observer(data) {
    this.data = data;
    this.walk(data);
}

Observer.prototype.walk = function(obj) {
    val = undefined;
    var keys = Object.keys(obj);
    for (let index = 0; index < keys.length; index++) {
        var key = keys[index];

        // 通过 hasOwnProperty 过滤掉一个对象本身拥有的属性 
        if (obj.hasOwnProperty(key)) {
            val = obj[key];
            // 递归调用 循环所有对象出来
            if (typeof val === 'object') {
                new Observer(val);
            }
            print('<=====> ', keys)
            this.convert(key, val);
        }
    }
}

Observer.prototype.convert = function(key, val) {
    print('convert', key, val);
    Object.defineProperty(this.data, key, {
        get: function() {
            print("=============get=========", key, val);
            return val;
        },
        set: function(newVal) {
            print("=============set=========", key, newVal, val);
            if (newVal == val) return;
            val = newVal;
            print("=============propertyChanged=========", key, newVal, val, propertyChanged);
            propertyChanged(key);
        }
    })
};

new Observer(globalThis.data.data);