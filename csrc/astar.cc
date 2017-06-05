
#include "astar.h"
#include <cmath>
#include <queue>
#include <vector>
#include <unordered_map>
#include <functional>
#include "v8args.h"

#define BLOCKV 0 //不可行区域
#define VALIDV 1 //可行区域

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

Persistent<Function> AStarMap::constructor;

void js_findPath(const FunctionCallbackInfo<Value>& args) {
    if (args.Length()<5){
        printf("error: findPath need 5 params\n");
        return;
    }
    Isolate* isolate = args.GetIsolate();
    AStarMap* pMap = node::ObjectWrap::Unwrap<AStarMap>(args.Holder());
    int stx, sty, edx, edy;
    GETI32( stx, 0);
    GETI32( sty, 1);
    GETI32( edx, 2);
    GETI32( edy, 3);

    Local<ArrayBuffer> jsUint32Buf_out;
    GETARRAYBUFFER(jsUint32Buf_out, 4);
    ArrayBuffer::Contents pOutBuff = jsUint32Buf_out->GetContents();
    int len = pMap->findPath(stx, sty, edx, edy,(int*)pOutBuff.Data(), (int)pOutBuff.ByteLength()/4);

    //设置返回值
    args.GetReturnValue().Set(Number::New(isolate, len));
}

void js_SetFindRange(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    AStarMap* pMap = node::ObjectWrap::Unwrap<AStarMap>(args.Holder());
    int w = 10000,h=10000;
    GETI32(w, 0);
    GETI32(h, 1);
    pMap->setSearchRegion(w, h);
}

void js_SetLinearizationLen(Local<String> property, Local<Value> value, 
        const PropertyCallbackInfo<void> &info) {
    AStarMap* pMap = node::ObjectWrap::Unwrap<AStarMap>(info.This());
    pMap->mnLinearizationLen = value->Int32Value();
}

void js_GetLinearizationLen(Local<String> property, const PropertyCallbackInfo<Value> &info) {
    Isolate *isolate = info.GetIsolate();
    AStarMap* pMap = node::ObjectWrap::Unwrap<AStarMap>(info.This());
    info.GetReturnValue().Set(Number::New(isolate, pMap->mnLinearizationLen));
}

void AStarMap::Init(Local<Object> exports) {
    Isolate* isolate = exports->GetIsolate();

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "AStarMap"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "findPath", js_findPath);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setSearchRegion", js_SetFindRange);
    tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "linearizationLen"), 
        js_GetLinearizationLen, js_SetLinearizationLen);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "AStarMap"), tpl->GetFunction());
}

void AStarMap::New(const FunctionCallbackInfo<Value>& args) {
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

            int w,h,px,py,gw,gh;
            GETI32( w, 1);
            GETI32( h, 2);
            GETI32( px, 3);
            GETI32( py, 4);
            GETI32( gw, 5);
            GETI32( gh, 6);
            if (w*h * 4 > ilen) {
                printf("error: map size != width x height!\n");
                return;
            }
            AStarMap* obj = new AStarMap(pMap, w, h, px, py, gw, gh);

            obj->Wrap(args.This());
            args.GetReturnValue().Set(args.This());
        }
        else {
            printf("Error: AStarMap constructor need 7 params\n");
        }
    }
    else {
        printf("warn: not support call AStarMap function\n");
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

using IntPair = std::pair<int, int>; //<prio, value>

inline int Heuristic(const int nFromX, const int nFromY, const int nToX, const int nToY){
    int dx = nFromX - nToX;
    int dy = nFromY - nToY;
#ifdef F_ADD
    if (dx < 0)dx = -dx;
    if (dy < 0)dy = -dy;
    return dx + dy;
#else
    /*
    if (dx < 0)dx = -dx;
    if (dy < 0)dy = -dy;
    if (dx < 60 && dy < 60)
        return sqrtv[dy][dx];
    */
    return (dx*dx + dy*dy);
#endif
}

/**
 * 根据来源构建路径。注意去掉了起点。
 * @param nOutBufferSize intbuffer的大小，实际大小是*4
*/
int ReconstructPath(const int nStart, const int nTarget, const int nOutBufferSize,
        int* const pOutBuffer, MapGrid* pMap){
    std::vector<int> Path;
    int nCurrent = nTarget;
    Path.push_back(nCurrent);

    while (nCurrent != nStart){
        //nCurrent = CameFrom[nCurrent];
        nCurrent = pMap[nCurrent].camefrom;
        Path.push_back(nCurrent);
    }

    const int nPathLength = Path.size() - 1; // Excluding start node

    for (int i = nPathLength - 1, j = 0; (i >= 0) && (j < nOutBufferSize); --i, ++j){
        pOutBuffer[j] = Path[i];
    }

    return nPathLength;
}

int FindPath(const int nStartX, const int nStartY,const int nTargetX, const int nTargetY,
        MapGrid* pMap, const int nMapWidth, const int nMapHeight,
        int* pOutBuffer, const int nOutBufferSize){

    if ((nStartX == nTargetX) && (nStartY == nTargetY)){
        return 0;
    }

    std::priority_queue<IntPair, std::vector<IntPair>, std::greater<IntPair> > OpenSet;
    //std::unordered_map<int, int> CameFrom;  //用来倒着生成路径的，记录从哪个节点来的
    //std::unordered_map<int, int> Cost;      //每个节点到起点的实际距离。 对应wiki的 g_score[]

    const int nStart = nStartX + nStartY*nMapWidth;
    const int nTarget = nTargetX + nTargetY*nMapWidth;

    if (!pMap[nTarget].mapinfo)
        return 0;

    OpenSet.emplace(0, nStart);
    pMap[nStart].cost = 0;
    //Cost[nStart] = 0;

    //static const int pMod[] = { -1, -1, 0,-1, 1,-1, -1,0, 1, 0, -1,1, 0,1, 1, 1}; // (x, y) pairs
    static const int pMod[] = { -1,0, 1,0, 0,-1, 0,1}; // (x, y) pairs
    Vec2 coord = { 0,0 };
    bool bFound = false;
    while (!OpenSet.empty()){
        //从openset中取出最好的。
        const int nCurrent = OpenSet.top().second; 
        OpenSet.pop();
        coord.x = pMap[nCurrent].x;
        coord.y = pMap[nCurrent].y;

        if (nCurrent == nTarget){
            bFound = true;
            break;
        }

        //遍历周围8个相邻点
        //for (int i = 0; i < 16; i += 2){
        for (int i = 0; i < 8; i += 2) {
            const int nNewX = coord.x + pMod[i];
            const int nNewY = coord.y + pMod[i + 1];
            const int nNeighbour = nNewX+nNewY*nMapWidth;

            if ((nNewX >= 0) && (nNewX < nMapWidth) &&
                (nNewY >= 0) && (nNewY < nMapHeight) && 
                pMap[nNeighbour].mapinfo){

                //const int nNewCost = Cost[nCurrent] + 1;
                const int nNewCost = pMap[nCurrent].cost + 1;
                //这里并没有使用closeset，因为如果遇到了closeset，一定满足nNewCost > Cost[nNeighbour]
                //if (Cost.find(nNeighbour) == Cost.end() || nNewCost < Cost[nNeighbour]){
                if( nNewCost < pMap[nNeighbour].cost){
                    //计算f(n)
                    const int nPriority = nNewCost + Heuristic(nNewX, nNewY,nTargetX, nTargetY);
                    //CameFrom[nNeighbour] = nCurrent;
                    pMap[nNeighbour].camefrom = nCurrent;
                    //Cost[nNeighbour] = nNewCost;
                    pMap[nNeighbour].cost = nNewCost;
                    OpenSet.emplace(nPriority, nNeighbour);
                }
            }
        }
    }

    if (!bFound){
        return -1;
    }
    return ReconstructPath(nStart, nTarget, nOutBufferSize, pOutBuffer, pMap);
}

void AStarMap::saveAsTxt(int stx, int sty, int edx, int edy, int* pOut, int nOutSz) {
    int sz = mnWidth*mnHeight;
    unsigned char *oMap = new unsigned char[sz];
    for (int i = 0; i < sz; i++) {
        oMap[i] = mpMap[i].mapinfo;
    }
    for (int i = 0; i < mnFindSz; i++) {
        oMap[mpFindResult[i]] = '*';
    }
    if (pOut) {
        for (int i = 0; i < nOutSz/2; i++) {
            int x = (pOut[i * 2] - mnPosX) / mnGridWidth;
            int y = (pOut[i * 2 + 1] - mnPosY) / mnGridHeight;
            oMap[x + y*mnWidth] = '@';
        }
    }
    oMap[sty * mnWidth + stx] = 'S';
    oMap[edy * mnWidth + edx] = 'E';
    FILE* fp = fopen("d:/temp/map.txt", "w");
    if (fp) {
        int p = 0;
        for (int y = 0; y < mnHeight; y++) {
            for (int x = 0; x < mnWidth; x++) {
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

    AStarMap m1(map, 60, 60, 0, 0, 128, 128);
    MapGrid* pmap = m1.mpMap;

    for (int i = 0; i < 10000; i++) {
        int stx = rand() % 60;
        int sty = rand() % 60;
        int edx = rand() % 60;
        int edy = rand() % 60;
        //stx = 1; sty = 1;
        //edx = 49; edy = 18;

        //int nPathLen = FindPath(stx, sty, edx, edy, pmap, 60, 60, obuf, outsz);
        int nPathLen = m1.findPathGrid(stx, sty, edx, edy);
        //m1.saveAsTxt(stx, sty, edx, edy,nullptr,0);
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
    MapGrid* pDest = mpMap;
    const MAPDATATYPE* pSrc = pMap;
    //int maxcost = 0xffffff;
    unsigned int maxcost = 0xffff;

    int nn = mnWidth*mnHeight;
    for (int i = 0; i < nn; i++) {
        pDest->mapinfo = *pSrc++;
        pDest->x = i%nMapWidth;
        pDest->y = i/nMapWidth;
        pDest->cost = maxcost;
        pDest->camefrom = 0;        //这个初始化值不会被用到
        pDest++;
    }

    mpWorkMap = new MapGrid[mnWidth*mnHeight];

    mnGridWidth = nGridWidth;
    mnGridHeight = nGridHeight;
    mnPosX = nPosX;
    mnPosY = nPosY;
    mnFindResultCapacity = 1000;
    mnFindSz = 0;
    mpFindResult = new int[mnFindResultCapacity];
}

AStarMap::~AStarMap() {
    if (mpMap) {
        delete[] mpMap;
    }
    if (mpFindResult) {
        delete[] mpFindResult;
    }
    if (mpWorkMap) {
        delete[] mpWorkMap;
    }
}

/*
* http://www.cnblogs.com/pheye/archive/2010/08/14/1799803.html
*/
bool AStarMap::_rayCast(const int x1, const int y1, const int x2, const int y2, int& hitx, int& hity) {
    int dx = x2 - x1; 
    int dy = y2 - y1; 
    int ux = ((dx > 0) << 1) - 1;//x的增量方向，取或-1
    int uy = ((dy > 0) << 1) - 1;//y的增量方向，取或-1
    int x = x1, y = y1, eps;//eps为累加误差
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
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
            MapGrid* pLastGrid = mpMap + pPath[i-1];
            Vec2& o = pPathOut[nNodeOutNum++];
            //转成实际位置并记录
            o.x = pLastGrid->x*mnGridWidth+mnPosX;
            o.y = pLastGrid->y*mnGridHeight+mnPosY;
            //实际上应该是从lastx,lasty开始，但是这样可能导致一直不动，反正相邻两个一定可行，
            //所以从curx,cury开始
            lastx = pLastGrid->x;
            lasty = pLastGrid->y;
        }
    }
    return nNodeOutNum * 2;
}

int AStarMap::findPath(int stx, int sty, int edx, int edy, int* pOut, int nOutSZ) {
    return findPath(stx, sty, edx, edy, mnSearchRegionW, mnSearchRegionH, mnLinearizationLen, pOut, nOutSZ);
}

int AStarMap::findPathGrid(int sx, int sy, int ex, int ey) {
    memcpy(mpWorkMap, mpMap, mnWidth*mnHeight*sizeof(MapGrid));
    mnFindSz =  FindPath(sx, sy, ex, ey, mpWorkMap, mnWidth, mnHeight, (int*)mpFindResult, mnFindResultCapacity);
    return mnFindSz;
}

int AStarMap::findPath(int stx, int sty, int edx, int edy, int maxwidth, int maxheight, 
        int linedist, int* pOut, int nOutSZ) {
    if (nOutSZ < 2) {
        printf("error: findPath pOut is too small \n");
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
    memcpy(mpWorkMap, mpMap, mnWidth*mnHeight*sizeof(MapGrid));
    int pn = mnFindSz = FindPath(sx, sy, ex, ey, mpWorkMap, mnWidth, mnHeight, (int*)mpFindResult, mnFindResultCapacity);
    //直线化
    int onum = linearizationAndToPos(mpFindResult, pn, linedist, pOut, nOutSZ);
    //saveAsTxt(sx,sy,ex,ey,pOut,onum);
    return onum;
}