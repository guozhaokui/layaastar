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
    #define VERSION 201801182

    void _test2(const FunctionCallbackInfo<Value>& args) {
        test();
    }
    void _getVersion(const FunctionCallbackInfo<Value>& args) {
        args.GetReturnValue().Set(VERSION);
    }

    void init(Local<Object> exports) {
        printf("LayaAStar Version:%d\n", VERSION);
        NODE_SET_METHOD(exports, "testastar", _test2);
        NODE_SET_METHOD(exports, "getVersion", _getVersion);
        AStarMap::Init(exports);
    }

    //必须要有一个初始化函数。
    //NODE_MODULE(模块名，初始化函数)
    NODE_MODULE(addon, init)

}