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
    int     mapinfo;
    unsigned short     cost;        //用short会快一点10ms/千次。但是限制大小最大为256x256
    unsigned short     camefrom;    //用short会快一点10ms/千次。但是限制大小最大为256x256
    unsigned short x;
    unsigned short y;
};

/**
 *@param nOutBufferSize pOutBuffer的大小，是length，不是byteLength
*/
int FindPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY,
    const MapGrid* pMap, const int nMapWidth, const int nMapHeight,
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

    AStarMap(const MAPDATATYPE* pMap, const int nMapWidth, const int nMapHeight, 
        int nPosX, int nPosY, int nGridWidth, int nGridHeight);

    ~AStarMap();

    /**
     * 寻路。
     * @param stx,sty 起点位置。不是格子
     * @param edx,edy 目标点位置，不是格子。
     * @param maxwidth 最大x距离
     * @param maxheight 最大y距离
     * @param linedist 直线化距离。对这么长的格子进行直线化。返回这么长的路径的直线化节点。
     * @param pOut 输出节点，每两个表示一个位置
     * @param nOutSZ 输出buffer的大小，用来防止buffer溢出
     * @return pOut的长度。例如有两个节点，则长度为4
    */
    int findPath(int stx, int sty, int edx, int edy, int maxwidth, int maxheight, int linedist, 
        int* pOut, int nOutSZ);
    int findPath(int stx, int sty, int edx, int edy, int* pOut, int nOutSZ);

    /*
    * 设置最大寻路范围。单位不是格子。
    */
    void setSearchRegion(int w, int h) {
        mnSearchRegionW = w/mnGridWidth;
        mnSearchRegionH = h/mnGridHeight;
    };

    //调试用
    int findPathGrid(int sx, int sy, int ex, int ey);
    void saveAsTxt(int stx, int sty, int edx, int edy, int* pOut, int nOutSz);
protected:

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
    * TODO 是否要考虑起点，现在是不考虑的。
    * pPath按照起点到终点的顺序，但是不含起点。
    *
    * 问题：如果是斜着靠边走的话，由于画线算法和8个方向的走法可能不一致，可能会导致一直认为有碰撞的情况？ 应该不会吧
    */
    int linearizationAndToPos(int* pPath,int nNodeNum, int nMaxDist, int* pOut, int nOutSZ);

protected:
    int     mnGridWidth = 128;
    int     mnGridHeight = 128;
    int     mnPosX = 0;
    int     mnPosY = 0;
    //int   mnStartX = 0;
    //int   mnStartY = 0;
    int     mnTargetX = 0;
    int     mnTargetY = 0;
    int     mnSearchRegionW = 10000;       //最大寻路距离宽，超过这个就算不可行。单位是格子。
    int     mnSearchRegionH = 10000;       //最大寻路距离高，超过这个就算不可行。单位是格子。
    //保存结果格子
    int*    mpFindResult = nullptr;
    int     mnFindResultCapacity = 0;
    int     mnFindSz = 0;
    MapGrid* mpWorkMap = nullptr;
//DEBUG
public:
    MapGrid* mpMap = nullptr;
public:
    int     mnWidth = 0;
    int     mnHeight = 0;
    int     mnLinearizationLen = 10000; //最大直线化距离。单位是格子
};

#endif

