#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "positionIdentifier.h"
using namespace std;

// 叉积函数（带过程输出）
static float cross(const point& a, const point& b, const point& p, const string& edgeName = "") {
    float result = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
    if (!edgeName.empty()) {
        cout << "计算边" << edgeName << "叉积: (" << b.x << "-" << a.x << ")(" << p.y << "-" << a.y
            << ") - (" << b.y << "-" << a.y << ")(" << p.x << "-" << a.x << ") = " << result;
        cout << " (" << (result > 0 ? "正" : (result < 0 ? "负" : "零")) << ")" << endl;
    }
    return result;
}

// 三角形判断（含边界）
std::pair<bool, std::string> triangle::containsTriangle1(const point& p) const {
    cout << "\n=== 三角形判断 ===" << endl;
    cout << "顶点: A(" << p1.x << "," << p1.y << "), B(" << p2.x << "," << p2.y
        << "), C(" << p3.x << "," << p3.y << ")" << endl;

    float d1 = cross(p1, p2, p, "AB");
    float d2 = cross(p2, p3, p, "BC");
    float d3 = cross(p3, p1, p, "CA");

    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    auto checkEdge = [](const point& a, const point& b, const point& p, const string& edgeName) {
        bool inX = p.x >= min(a.x, b.x) && p.x <= max(a.x, b.x);
        bool inY = p.y >= min(a.y, b.y) && p.y <= max(a.y, b.y);
        cout << "检查边" << edgeName << "范围: x[" << min(a.x, b.x) << "," << max(a.x, b.x)
            << "], y[" << min(a.y, b.y) << "," << max(a.y, b.y) << "] -> "
            << (inX && inY ? "在范围内" : "超出范围") << endl;
        return inX && inY;
        };

    if (d1 == 0 && checkEdge(p1, p2, p, "AB")) return { true, "点在边AB上" };
    if (d2 == 0 && checkEdge(p2, p3, p, "BC")) return { true, "点在边BC上" };
    if (d3 == 0 && checkEdge(p3, p1, p, "CA")) return { true, "点在边CA上" };

    if (!has_neg || !has_pos) {
        cout << "所有叉积同号 -> 点在三角形内" << endl;
        return { true, "点在三角形内" };
    }
    else {
        cout << "叉积符号不一致 -> 点在三角形外" << endl;
        return { false, "点在三角形外" };
    }
}

// 凸多边形判断
std::pair<bool, std::string> polygon::containsPolygon1(const point& p) const {
    cout << "\n=== 凸多边形判断 ===" << endl;
    cout << "顶点数: " << edges.size() << endl;

    if (edges.size() < 3) {
        cout << "顶点数不足3个 -> 非法多边形" << endl;
        return { false, "非法多边形" };
    }

    int winding = 0;
    for (size_t i = 0; i < edges.size(); ++i) {
        const point& a = edges[i].p1;
        const point& b = edges[(i + 1) % edges.size()].p1;
        cout << "\n处理边" << i + 1 << ": (" << a.x << "," << a.y << ")->(" << b.x << "," << b.y << ")" << endl;

        float cp = cross(a, b, p, "E" + to_string(i + 1));

        // 检查是否在边上
        if (cp == 0) {
            bool inX = p.x >= min(a.x, b.x) && p.x <= max(a.x, b.x);
            bool inY = p.y >= min(a.y, b.y) && p.y <= max(a.y, b.y);
            cout << "检查边范围: x[" << min(a.x, b.x) << "," << max(a.x, b.x)
                << "], y[" << min(a.y, b.y) << "," << max(a.y, b.y) << "] -> "
                << (inX && inY ? "在边上" : "不在边上") << endl;
            if (inX && inY) return { true, "点在多边形边上" };
        }

        // 更新环绕数
        if (a.y <= p.y) {
            if (b.y > p.y && cp > 0) {
                cout << "从下向上穿过 -> 环绕数+1" << endl;
                winding++;
            }
        }
        else {
            if (b.y <= p.y && cp < 0) {
                cout << "从上向下穿过 -> 环绕数-1" << endl;
                winding--;
            }
        }
        cout << "当前环绕数: " << winding << endl;
    }

    cout << "最终环绕数: " << winding << " -> ";
    if (winding != 0) {
        cout << "点在多边形内" << endl;
        return { true, "点在多边形内" };
    }
    else {
        cout << "点在多边形外" << endl;
        return { false, "点在多边形外" };
    }
}

int main() {
    // 测试三角形
    triangle tri(point(0, 0), point(4, 0), point(2, 3));
    point testPt1(2, 1), testPt2(1, 0);

    cout << "===== 三角形测试 =====" << endl;
    auto [triRes1, triReason1] = tri.containsTriangle1(testPt1);
    cout << "点(2,1): " << (triRes1 ? "在" : "不在") << " (" << triReason1 << ")\n";

    auto [triRes2, triReason2] = tri.containsTriangle1(testPt2);
    cout << "点(1,0): " << (triRes2 ? "在" : "不在") << " (" << triReason2 << ")\n";

    // 测试凸多边形
    vector<point> polyPoints = { point(0,0), point(4,0), point(4,4), point(0,4) };
    polygon poly(polyPoints);
    point testPt3(2, 2), testPt4(5, 5);

    cout << "\n===== 凸多边形测试 =====" << endl;
    auto [polyRes1, polyReason1] = poly.containsPolygon1(testPt3);
    cout << "点(2,2): " << (polyRes1 ? "在" : "不在") << " (" << polyReason1 << ")\n";

    auto [polyRes2, polyReason2] = poly.containsPolygon1(testPt4);
    cout << "点(5,5): " << (polyRes2 ? "在" : "不在") << " (" << polyReason2 << ")\n";

    return 0;
}