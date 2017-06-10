
export declare class AStarMap{
    /**
     * 直线化距离。
     * 现在的寻路是基于格子的，这样结果会返回很多的节点，并且会在明显没有障碍的地方做各种直角转弯。于是
     * 在findPath的最后加了一个直线化的处理，就是判断关键点，合并直线路径。为了提高效率，可以用这个参数
     * 来控制直线化多长的距离。单位是格子数。
     * 缺省是直线化所有的路径。
     */
    linearizationLen:number;

    /**
     * 构造一个寻路地图对象。
     * @param mapdata  {Uint32Array} 障碍数据。大小必须是w*h个，其中的数据值为1表示可行格子，值为0表示障碍。
     * @param w   {number} mapdata的宽度。x方向，横向，是指格子数
     * @param h   {number} mapdata的高度。是指格子数
     * @param posx  {number} mapdata的起始位置，就是格子[0,0]对应的位置。
     * @param posy 
     * @param gridw {number} 每个格子的宽度。x方向，横向，对2d来说单位就是是像素，表示每个格子的宽度
     * @param gridh  {number} 每个格子的高度。
     */
    constructor(mapdata:Uint32Array, w:number, h:number, posx:number, posy:number, gridw:number, gridh:number);

    /**
     * 设置寻路的最大范围，如果正常寻路效率无法满足需求，可以用这个来限制寻路范围来提高速度。
     * 不设置的话，就是全地图寻路。
     * @param maxx {number} 最大范围x。单位是实际大小（2d通常就是像素），不是格子
     * @param maxy {number} 最大范围y。单位是实际大小，不是格子
     */
    setSearchRegion(maxx:number, maxy:number);

    /**
     * 是否把中间结果保存到一个文件中（d:/temp/map.txt）用于调试。
     * @param b {number} 0表示不保存，其他表示保存
     */
    setSaveFindeResult(b:number):void;

    /**
     * 寻路。注意所有的参数都是是实际位置，而不是格子。
     * 返回的也是节点的实际位置。
     * 返回结果不含起点和终点。
     * 返回0 表示起点到终点可以直达。起点和终点都在一个格子内也是这种情况。
     * 返回-1 表示起点不可达，无法计算
     * 返回-2 表示终点不可达，即终点本身就在不可达区域
     * 返回-3 表示无法找到路径。例如终点在封死的区域
     * 
     * @param stx   {number} 起点x。是实际位置，不是格子
     * @param sty   {number} 起点y。是实际位置，不是格子
     * @param targetx  {number} 目标点x。是实际位置，不是格子
     * @param targety {number} 目标点y。是实际位置，不是格子
     * @param outbuf  {Uint32Array} 输出路径节点。个数在返回值中，每两个表示一个位置。注意这个buffer要足够大。
     *                如果只设置一次会更方便一些，但是不太安全，这个buffer是js创建的，可能被释放。
     * @return {number} outbuf的长度，对应的节点数量要/2,例如：
     *      let len = findPath(...,out);
     *      let node1x = out[0];
     *      let node1y = out[1];
     */
    findPath(stx:number, sty:number, targetx:number, targety:number, outbuf:Uint32Array):number;
}