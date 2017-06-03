#ifndef _LAYA_A_START_H___
#define _LAYA_A_START_H___

#define MAPDATATYPE unsigned int
#define USEINT
#define MAXOUTNUM 100

#include <node.h>
#include <node_object_wrap.h>

struct Vec2 {
    int x, y;
};

int FindPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY,
    const MAPDATATYPE* pMap, const int nMapWidth, const int nMapHeight,
    int* pOutBuffer, const int nOutBufferSize);

void test();

/**
*/
class AStarMap :public node::ObjectWrap{
public:
    //V8 导出
    static void Init(v8::Local<v8::Object> exports);
    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Persistent<v8::Function> constructor;
    //

    AStarMap(const MAPDATATYPE* pMap, const int nMapWidth, const int nMapHeight, int nPosX, int nPosY, int nGridWidth, int nGridHeight);
    ~AStarMap();
    /**
     * 寻路。
     * @param stx,sty 起点位置。不是格子
     * @param edx,edy 目标点位置，不是格子。
     * @param maxwidth 最大寻路距离宽，超过这个就算不可行
     * @param maxheight 最大寻路距离高，超过这个就算不可行
     * @param linedist 直线化距离。对这么长的格子进行直线化。返回这么长的路径的直线化节点。
    */
    void findPath(int stx, int sty, int edx, int edy, int maxwidth, int maxheight, int linedist);

protected:
    /**
     * @param nStartX 开始位置，不是格子，是位置。
     * @param nTargetX 目标位置，不是格子，是位置。
    */
    int _findPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY);

    /**
    * @param nStartX 开始位置，是格子。 
    * @param nTargetX 目标位置，是格子
    */
    int _findPathGrid(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY);

    /**
     * 直线检测。
     * @param x1 起始格子
     * @param x2 目标格子
     * @param hitx 碰撞的位置
     * @return {bool} 是否发生碰撞。false的话，hitx，hity无意义。
    */
    bool _rayCast(const int x1, const int y1, const int x2, const int y2, int& hitx, int& hity);

    /**
    * 直线化结果。 
    */
    void linearization(Vec2* pPath,int nNodeNum, int nMaxDist);

    int* mpOutBuffer=nullptr;
    int mnOutBufferSz = 0;
    int mnWidth = 0;
    int mnHeight = 0;
    int mnGridWidth = 128;
    int mnGridHeight = 128;
    int mnPosX = 0;
    int mnPosY = 0;
    //int mnStartX = 0;
    //int mnStartY = 0;
    int mnTargetX = 0;
    int mnTargetY = 0;
    //保存结果格子和最终输出的位置节点
    Vec2* mpFindResult = nullptr;
    int   mnFindeSz = 0;
    unsigned int* mpMap=nullptr;
public:
    Vec2 mPathOut[MAXOUTNUM];
    int  mnPathOutSz = 0;
};

#endif

