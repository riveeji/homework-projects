#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include <iostream>
#include <vector>
#include <fstream>
#include "positionIdentifier.h" 
using namespace std;

// 替换原有Point结构体
using Point = point;

// 叉积判断函数
float positionIdentifier(float x1, float y1, float x2, float y2, float x, float y) {
    return (x2 - x1) * (y - y1) - (x - x1) * (y2 - y1);
}

// 判断点是否在三角形内（不含边界）
bool containsWithoutEdge(const triangle& tri, const Point& p) {
    // 获取三个边的叉积符号
    float c1 = positionIdentifier(tri.p1.x, tri.p1.y, tri.p2.x, tri.p2.y, p.x, p.y);
    float c2 = positionIdentifier(tri.p2.x, tri.p2.y, tri.p3.x, tri.p3.y, p.x, p.y);
    float c3 = positionIdentifier(tri.p3.x, tri.p3.y, tri.p1.x, tri.p1.y, p.x, p.y);

    //全正或全负
    return (c1 > 0 && c2 > 0 && c3 > 0) || (c1 < 0 && c2 < 0 && c3 < 0);
}

int main() {
    cout << "===== 判断多个点是否在几何体内 =====" << endl;

    // 选择几何体类型
    int shapeType;
    cout << "请选择几何体类型（1：三角形，2：凸多边形，3：凹多边形，4：四面体）: ";
    cin >> shapeType;

    if (shapeType == 1) { // 三角形
        Point a, b, c;
        cout << "输入三角形顶点A(x y): "; cin >> a.x >> a.y;
        cout << "输入三角形顶点B(x y): "; cin >> b.x >> b.y;
        cout << "输入三角形顶点C(x y): "; cin >> c.x >> c.y;
        triangle tri(a, b, c);

        bool includeEdge;
        cout << "包含边线？(1: 是, 0: 否): ";
        cin >> includeEdge;

        // 输入测试点（同原逻辑）
        vector<Point> testPoints;
        // ...（输入逻辑与原代码相同，省略）

        // 判断点
        int insideCount = 0;
        for (const auto& p : testPoints) {
            bool inside;
            if (includeEdge) {
                auto result = tri.containsTriangle1(p);
                inside = result.first;
            }
            else {
                inside = containsWithoutEdge(tri, p);
            }
            if (inside) ++insideCount;
            // 输出结果
        }
        // 输出统计（同原逻辑）
    }
    else if (shapeType == 2) { // 凸多边形
        vector<Point> polyPoints;
        int n;
        cout << "输入凸多边形顶点数: ";
        cin >> n;
        for (int i = 0; i < n; ++i) {
            float x, y;
            cout << "顶点" << i + 1 << "(x y): ";
            cin >> x >> y;
            polyPoints.emplace_back(x, y);
        }
        polygon poly(polyPoints);

        // 输入测试点（同原逻辑）
        vector<Point> testPoints;
        // ...（输入逻辑省略）

        // 判断点（使用凸多边形方法）
        for (const auto& p : testPoints) {
            auto result = poly.containsPolygon1(p);
            cout << (result.first ? "内部" : "外部") << endl;
        }
    }
    // 其他形状类似，需补充输入和调用对应方法

    return 0;
}
