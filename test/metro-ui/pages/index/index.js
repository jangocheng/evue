export default {
    data: {
        title: 'World'
    },
    onclick: function(obj, x, y) {
        print("=====onclick=====");
        print(obj);
        print(x);
        print(y);
    },
    onpress: function(obj, x, y) {
        print("=====onpress=====");
        print(obj);
        print(x);
        print(y);
    },
    onrelease: function(obj, x, y) {
        print("=====onrelease=====");
        print(obj);
        print(x);
        print(y);
    },
    onlongpress: function(obj, x, y) {
        print("=====longpress=====");
        print(obj);
        print(x);
        print(y);
    },
    swiperChange: function(index) {
        // print("=====swiperChange==========");
        print(index);
    },
    onswipe: function(direction) {
        print(direction);
    }
}