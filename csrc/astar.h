#ifndef _LAYA_A_START_H___
#define _LAYA_A_START_H___

#define MAPDATATYPE unsigned int
#define MAXOUTNUM 100

#include <node.h>
#include <node_object_wrap.h>

struct Vec2 {
    int x, y;
};

struct  MapGrid{
    unsigned short x;
    unsigned short y;
    int     mapinfo;
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
     * @param linedist 直线化距离。对这么长的格子进行直线化。返回这么长的路径的直线化节点。
     * @param pOut 输出节点，每两个表示一个位置
     * @param nOutSZ 输出buffer的大小，用来防止buffer溢出
     * @return pOut的长度。例如有两个节点，则长度为4
    */
    int findPath(int stx, int sty, int edx, int edy, int maxwidth, int maxheight, int linedist, int* pOut, int nOutSZ);
    int findPath(int stx, int sty, int edx, int edy, int* pOut, int nOutSZ);

    /*
    * 设置最大寻路范围。单位不是格子。
    */
    void setFindRange(int w, int h) {
        mnFindRangeX = w/mnGridWidth;
        mnFindRangeY = h/mnGridHeight;
    };

protected:
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
    * 返回是pOut的长度
    */
    int linearizationAndToPos(int* pPath,int nNodeNum, int nMaxDist, int* pOut, int nOutSZ);

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
    int mnFindRangeX = 10000;       //最大寻路距离宽，超过这个就算不可行。单位是格子。
    int mnFindRangeY = 10000;       //最大寻路距离高，超过这个就算不可行。单位是格子。
    //保存结果格子
    int*  mpFindResult = nullptr;
    int   mnFindeSz = 0;
    MapGrid* mpMap=nullptr;
public:
    int mnLinearizationLen = 10000; //最大直线化距离。单位是格子
};

#endif

