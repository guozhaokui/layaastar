// hello.cc
#include <node.h>
#include <stdio.h>
#include <algorithm>
#include "astar.h"
#include "v8args.h"

namespace laya {
    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Value;
    using v8::ArrayBufferView;
    using v8::ArrayBuffer;
    using v8::FunctionTemplate;
    using v8::ObjectTemplate;

    void _test2(const FunctionCallbackInfo<Value>& args) {
        test();
    }


    void init(Local<Object> exports) {
        NODE_SET_METHOD(exports, "testastar", _test2);
        AStarMap::Init(exports);
    }

    //必须要有一个初始化函数。
    //NODE_MODULE(模块名，初始化函数)
    NODE_MODULE(addon, init)

}