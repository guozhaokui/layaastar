
#include "astar.h"
#include <cmath>
#include <queue>
#include <vector>
#include <unordered_map>
#include <functional>

//不可行区域
#define BLOCKV 0

//可行区域
#define VALIDV 1

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::PropertyCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;
using v8::ArrayBufferView;
using v8::ArrayBuffer;

#define GETARRAYBUFFER(o,arg) \
if (args[arg]->IsArrayBuffer()) {\
    o = Local<ArrayBuffer>::Cast(args[arg]);\
}\
else if (args[arg]->IsArrayBufferView()) {\
    o= Local<ArrayBufferView>::Cast(args[arg])->Buffer();\
}\
else {\
    printf("param %d error, need ArrayBuffer\n",arg);\
    return;\
}

#define GETNUMBER(type,o,arg) \
if (args[arg]->IsNumber()) {\
    o = (type)(args[arg]->ToNumber(Isolate::GetCurrent()->GetCurrentContext()).FromMaybe(Local<Number>())->Value());\
}\
else {\
    printf("param ##arg## error: width {number}\n");\
return;\
}


Persistent<Function> AStarMap::constructor;

void js_findPath(const FunctionCallbackInfo<Value>& args) {
    if (args.Length()<5){
        printf("findPath 参数不够,要5个\n");
        return;
    }
    Isolate* isolate = args.GetIsolate();
    AStarMap* obj = node::ObjectWrap::Unwrap<AStarMap>(args.Holder());
    int stx, sty, edx, edy;
    GETNUMBER(int, stx, 0);
    GETNUMBER(int, sty, 1);
    GETNUMBER(int, edx, 2);
    GETNUMBER(int, edy, 3);

    Local<ArrayBuffer> jsUint32Buf_out;
    GETARRAYBUFFER(jsUint32Buf_out, 4);
    v8::ArrayBuffer::Contents pOutBuff = jsUint32Buf_out->GetContents();
    int len = obj->findPath(stx, sty, edx, edy,(int*)pOutBuff.Data(), (int)pOutBuff.ByteLength());

    //设置返回值
    args.GetReturnValue().Set(Number::New(isolate, len));
}

void js_SetFindRange(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    AStarMap* obj = node::ObjectWrap::Unwrap<AStarMap>(args.Holder());
    int w = 10000;
    GETNUMBER(int, w, 0);
    int h = 10000;
    GETNUMBER(int, h, 1);
    obj->setFindRange(w, h);
}

void js_SetLinearizationLen(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info) {
    AStarMap* obj = node::ObjectWrap::Unwrap<AStarMap>(info.This());
    obj->mnLinearizationLen = value->Int32Value();
}

void js_GetLinearizationLen(Local<String> property, const PropertyCallbackInfo<Value> &info) {
    Isolate *isolate = info.GetIsolate();
    AStarMap* obj = node::ObjectWrap::Unwrap<AStarMap>(info.This());
    info.GetReturnValue().Set(Number::New(isolate, obj->mnLinearizationLen));
}

void AStarMap::Init(Local<Object> exports) {
    Isolate* isolate = exports->GetIsolate();

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "AStarMap"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "findPath", js_findPath);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setFindRange", js_SetFindRange);
    tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "linearizationLen"), 
        js_GetLinearizationLen, js_SetLinearizationLen);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "AStarMap"), tpl->GetFunction());
}

void AStarMap::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();

    if (args.IsConstructCall()) {
        // Invoked as constructor: `new MyObject(...)`
        if (args.Length() >= 7) {
            //double value = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
            Local<ArrayBuffer> jsUint32Buf_map;
            GETARRAYBUFFER(jsUint32Buf_map, 0);
            v8::ArrayBuffer::Contents pMapBuff = jsUint32Buf_map->GetContents();
            int ilen = (int)pMapBuff.ByteLength();
            const unsigned int* pMap = (const unsigned int*)pMapBuff.Data();

            int w;
            GETNUMBER(int, w, 1);
            int h;
            GETNUMBER(int, h, 2);
            int px;
            GETNUMBER(int, px, 3);
            int py;
            GETNUMBER(int, py, 4);
            int gw;
            GETNUMBER(int, gw, 5);
            int gh;
            GETNUMBER(int, gh, 6);
            AStarMap* obj = new AStarMap(pMap, w, h, px, py, gw, gh);

            obj->Wrap(args.This());
            args.GetReturnValue().Set(args.This());
        }
        else {
            printf("AStarMap构造参数不对，应该是7个\n");
        }
    }
    else {
        printf("不支持直接调用这个函数。\n");
        /*
        // Invoked as plain function `MyObject(...)`, turn into construct call.
        const int argc = 1;
        Local<Value> argv[argc] = { args[0] };
        Local<Context> context = isolate->GetCurrentContext();
        Local<Function> cons = Local<Function>::New(isolate, constructor);
        Local<Object> result =
            cons->NewInstance(context, argc, argv).ToLocalChecked();
        args.GetReturnValue().Set(result);
        */
    }
}

MAPDATATYPE map[] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0 
};

using PQElement = std::pair<int, int>; //<prio, value>



int CoordToNode(const int nX, const int nY, const int nMapWidth){
    return nY * nMapWidth + nX;
}

void NodeToCoord(const int nNode, Vec2& coord, const int nMapWidth){
    coord.x = nNode % nMapWidth;
    coord.y = nNode / nMapWidth;
}

int Heuristic(const int nFromX, const int nFromY, const int nToX, const int nToY){
    //return std::abs(nFromX - nToX) + std::abs(nFromY - nToY);
    int dx = nFromX - nToX;
    int dy = nFromY - nToY;

    return (dx*dx + dy*dy);
}

/**
 * 去掉了起点。
*/
int ReconstructPath(const int nStart, const int nTarget, const int nOutBufferSize,
    int* const pOutBuffer, std::unordered_map<int, int>& CameFrom){
    std::vector<int> Path;
    int nCurrent = nTarget;
    Path.push_back(nCurrent);

    while (nCurrent != nStart){
        nCurrent = CameFrom[nCurrent];
        Path.push_back(nCurrent);
    }

    const int nPathLength = Path.size() - 1; // Excluding start node

    for (int i = nPathLength - 1, j = 0; (i >= 0) && (j < nOutBufferSize); --i, ++j){
        pOutBuffer[j] = Path[i];
    }

    return nPathLength;
}

int FindPath(const int nStartX, const int nStartY,const int nTargetX, const int nTargetY,
    const MAPDATATYPE* pMap, const int nMapWidth, const int nMapHeight,
    int* pOutBuffer, const int nOutBufferSize){

    if ((nStartX == nTargetX) && (nStartY == nTargetY)){
        return 0;
    }

    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement> > OpenSet;
    std::unordered_map<int, int> CameFrom;  //用来倒着生成路径的，记录从哪个节点来的
    std::unordered_map<int, int> Cost;      //每个节点到起点的实际距离。 对应wiki的 g_score[]

    const int nStart = CoordToNode(nStartX, nStartY, nMapWidth);
    const int nTarget = CoordToNode(nTargetX, nTargetY, nMapWidth);

    if (!pMap[nTarget])
        return 0;

    OpenSet.emplace(0, nStart);
    Cost[nStart] = 0;

    static const int pMod[] = { -1, -1, 0,-1, 1,-1, -1,0, 1, 0, -1,1, 0,1, 1, 1}; // (x, y) pairs
    Vec2 coord = { 0,0 };
    bool bFound = false;
    while (!OpenSet.empty()){
        //从openset中取出最好的。
        const int nCurrent = OpenSet.top().second; 
        OpenSet.pop();

        NodeToCoord(nCurrent, coord, nMapWidth);

        if (nCurrent == nTarget){
            bFound = true;
            break;
        }

        //遍历周围8个相邻点
        for (int i = 0; i < 16; i += 2){
            const int nNewX = coord.x + pMod[i];
            const int nNewY = coord.y + pMod[i + 1];
            const int nNeighbour = CoordToNode(nNewX, nNewY, nMapWidth);

            if ((nNewX >= 0) && (nNewX < nMapWidth) &&
                (nNewY >= 0) && (nNewY < nMapHeight) && 
                pMap[nNeighbour]){

                const int nNewCost = Cost[nCurrent] + 1;
                //这里并没有使用closeset，因为如果遇到了closeset，一定满足nNewCost < Cost[nNeighbour]
                if (Cost.find(nNeighbour) == Cost.end() || nNewCost < Cost[nNeighbour]){
                    //计算f(n)
                    const int nPriority = nNewCost + Heuristic(nNewX, nNewY,nTargetX, nTargetY);
                    CameFrom[nNeighbour] = nCurrent;
                    Cost[nNeighbour] = nNewCost;
                    OpenSet.emplace(nPriority, nNeighbour);
                }
            }
        }
    }

    if (!bFound){
        return -1;
    }
    return ReconstructPath(nStart, nTarget, nOutBufferSize, pOutBuffer, CameFrom);
}

void saveasTxt(int stx, int sty, int edx, int edy, int* pathbuf, int bufsz) {
    unsigned char *oMap = new unsigned char[sizeof(map)];
    memcpy(oMap, map, sizeof(map));
    for (int i = 0; i < bufsz; i++) {
        oMap[pathbuf[i]] = '*';
    }
    oMap[sty * 60 + stx] = 'S';
    oMap[edy * 60 + edx] = 'E';
    FILE* fp = fopen("d:/temp/map.txt", "w");
    if (fp) {
        int p = 0;
        for (int y = 0; y < 60; y++) {
            for (int x = 0; x < 60; x++) {
                unsigned char cc = oMap[p];
                switch (cc) {
                case 1:
                    cc = ' ';
                    break;
                case 0:
                    cc = '#';
                    break;
                default:
                    break;
                }
                fprintf(fp,"%c",cc);
                p++;
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
    }
    delete[] oMap;
}


void test() {
    printf("test astar\n");
    int obuf[10000];
    int outsz=10000;

    for (int i = 0; i < 10000; i++) {
        int stx = rand() % 60;
        int sty = rand() % 60;
        int edx = rand() % 60;
        int edy = rand() % 60;
        //stx = 1; sty = 1;
        //edx = 49; edy = 18;
        int nPathLen = FindPath(stx, sty, edx, edy, map, 60, 60, obuf, outsz);
        //saveasTxt(stx, sty, edx, edy, obuf, nPathLen);
    }
    printf("OK\n");
}

/**

*/


AStarMap::AStarMap(const MAPDATATYPE* pMap, const int nMapWidth, const int nMapHeight,
    int nPosX, int nPosY, int nGridWidth, int nGridHeight) {
    
    mnWidth = nMapWidth;
    mnHeight = nMapHeight;
    mpMap = new MapGrid[mnWidth*mnHeight];
    int nn = mnWidth*mnHeight;
    MapGrid* pDest = mpMap;
    const MAPDATATYPE* pSrc = pMap;
    for (int i = 0; i < nn; i++) {
        pDest->mapinfo = *pSrc++;
        pDest->x = i%nMapWidth;
        pDest->y = i/nMapWidth;
        pDest++;
    }
    mnGridWidth = nGridWidth;
    mnGridHeight = nGridHeight;
    mnPosX = nPosX;
    mnPosY = nPosY;
    mnFindeSz = 100;
    mpFindResult = new int[mnFindeSz];
}

AStarMap::~AStarMap() {
    if (mpMap) {
        delete[] mpMap;
    }
    if (mpFindResult) {
        delete[] mpFindResult;
    }
}

int  AStarMap::_findPathGrid(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY) {
    int sx = nStartX ;
    int sy = nStartY ;
    int ex = nTargetX;
    int ey = nTargetY;

}

/*
* http://www.cnblogs.com/pheye/archive/2010/08/14/1799803.html
*/
bool AStarMap::_rayCast(const int x1, const int y1, const int x2, const int y2, int& hitx, int& hity) {
    int dx = x2 - x1; if (dx < 0) dx = -dx;
    int dy = y2 - y1; if (dy < 0) dy = -dy;
    int ux = ((dx > 0) << 1) - 1;//x的增量方向，取或-1
    int uy = ((dy > 0) << 1) - 1;//y的增量方向，取或-1
    int x = x1, y = y1, eps;//eps为累加误差

    eps = 0;
    if (dx > dy) {
        for (x = x1; x != x2; x += ux) {
            int grid = x + y*mnWidth;
            if (mpMap[grid].mapinfo == BLOCKV) {
                hitx = x; hity = y;
                return true;
            }
            eps += dy;
            if ((eps << 1) >= dx) {
                y += uy; eps -= dx;
            }
        }
    }
    else {
        for (y = y1; y != y2; y += uy) {
            int grid = x + y*mnWidth;
            if (mpMap[grid].mapinfo == BLOCKV) {
                hitx = x; hity = y;
                return true;
            }
            eps += dx;
            if ((eps << 1) >= dy) {
                x += ux; eps -= dy;
            }
        }
    }
    return false;
}

/*
    TODO 是否要考虑起点，现在是不考虑的。
    pPath按照起点到终点的顺序，但是不含起点。

    问题：如果是斜着靠边走的话，由于画线算法和8个方向的走法可能不一致，可能会导致一直认为有碰撞的情况？ 应该不会吧
*/
int AStarMap::linearizationAndToPos(int* pPath, int nNodeNum, int nMaxDist, int* pOut, int nOutSZ) {
    if (nMaxDist < 0) nMaxDist = 10000;
    //两个的直接忽略，认为没有碰撞。
    if (nNodeNum <= 2) {
        pOut[0] = mnTargetX;
        pOut[1] = mnTargetY;
        return 2;
    }

    int maxn = nMaxDist < nOutSZ/2 ? nMaxDist : nOutSZ/2;
    //至少3个点。假设起点和第一个点在一条直线上，所以忽略起点。
    Vec2* pPathOut = (Vec2*)pOut;
    int nOutNodeSz = 0;
    int nNodeOutNum = 0;
    int curx, cury;
    int hitx, hity;
    MapGrid* cgrid = mpMap+pPath[0];
    int lastx= cgrid->x, lasty= cgrid->y;//起点后的第一个点
    for (int i = 1; i < nNodeNum && nNodeOutNum < maxn; i++) {
        MapGrid* pGrid = mpMap + pPath[i];
        curx = pGrid->x;
        cury = pGrid->y;
        if (_rayCast(lastx, lasty, curx, cury, hitx, hity)) {
            //如果发生碰撞，说明应该取上一个点
            Vec2& o = pPathOut[nNodeOutNum++];
            o.x = lastx; o.y = lasty;
            //实际上应该是从lastx,lasty开始，但是这样可能导致一直不动，反正相邻两个一定可行，
            //所以从curx,cury开始
            lastx = curx;
            lasty = cury;
        }
    }
    return nNodeOutNum * 2;
}

int AStarMap::findPath(int stx, int sty, int edx, int edy, int* pOut, int nOutSZ) {
    return findPath(stx, sty, edx, edy, mnFindRangeX, mnFindRangeY, mnLinearizationLen, pOut, nOutSZ);
}

int AStarMap::findPath(int stx, int sty, int edx, int edy, int maxwidth, int maxheight, 
    int linedist, int* pOut, int nOutSZ) {
    if (nOutSZ < 2) {
        printf("findPath 提供的输出空间太小\n");
        return 0;
    }

    mnTargetX = edx;
    mnTargetY = edy;
    //转成格子
    int sx = stx / mnGridWidth;
    int sy = sty / mnGridHeight;
    int ex = edx / mnGridWidth;
    int ey = edy / mnGridHeight;
    int hitx = 0, hity = 0;
    if (!_rayCast(sx, sy, ex, ey, hitx, hity)) {
        //注意不包含起点
        pOut[0] = edx;
        pOut[1] = edy;
        return 2;
    }
    //寻路
    //int pn = FindPath(sx, sy, ex, ey, mpMap, mnWidth, mnHeight, (int*)mpFindResult, mnFindeSz);
    //直线化
    //linearization(mpFindResult, pn, linedist, pOut, nOutSZ);
}