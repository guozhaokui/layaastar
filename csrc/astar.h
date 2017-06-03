#ifndef _LAYA_A_START_H___
#define _LAYA_A_START_H___

#define MAPDATATYPE unsigned int

int FindPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY,
    const MAPDATATYPE* pMap, const int nMapWidth, const int nMapHeight,
    int* pOutBuffer, const int nOutBufferSize);

void test();

/**
* 为了提高效率，加了一个保护边。所以不能直接返回格子数据。如果返回位置则没有关系。
*/
class AStarMap {
public:
    AStarMap(const MAPDATATYPE* pMap, const int nMapWidth, const int nMapHeight, int nPosX, int nPosY, int nGridWidth, int nGridHeight);
    ~AStarMap();

protected:
    /**
     * @param nStartX 开始位置，不是格子，是位置。
     * @param nTargetX 目标位置，不是格子，是位置。
    */
    int _findPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY);

    /**
    * @param nStartX 开始位置，是格子。 是外部格子，与内部格子的xy都差1
    * @param nTargetX 目标位置，是格子
    */
    int _findPathGrid(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY);

    /**
     * 直线检测。
     * @param stx 格子，是本地
    */
    int _rayCast(const int stx, const int sty, const int edx, const int edy);

    int* mpOutBuffer=nullptr;
    int mnOutBufferSz = 0;
    int mnWidth = 0;
    int mnHeight = 0;
    int mnGridWidth = 128;
    int mnGridHeight = 128;
    int mnPosX = 0;
    int mnPosY = 0;
    MAPDATATYPE* mpMap=nullptr;

};

#endif

