
#include <iostream>
#include <vector>
#include <algorithm>
#include "positionIdentifier.h"
using namespace std;

// 叉积计算辅助函数
static float cross(const point& a, const point& b, const point& p) {
    return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
}

// 三角形包含判断实现（含边界）
std::pair<bool, std::string> triangle::containsTriangle1(const point& p) const {
    float d1 = cross(p1, p2, p);
    float d2 = cross(p2, p3, p);
    float d3 = cross(p3, p1, p);

    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    if (d1 == 0 || d2 == 0 || d3 == 0) {
        // 检查是否在边上
        auto onSegment = [](const point& q, const point& r, const point& p) {
            return (p.x <= max(q.x, r.x)) && (p.x >= min(q.x, r.x)) &&
                (p.y <= max(q.y, r.y)) && (p.y >= min(q.y, r.y));
            };

        if ((d1 == 0 && onSegment(p1, p2, p)) ||
            (d2 == 0 && onSegment(p2, p3, p)) ||
            (d3 == 0 && onSegment(p3, p1, p))) {
            return { true, "点在三角形边上" };
        }
    }

    return { !(has_neg && has_pos), has_neg && has_pos ? "点在三角形外" : "点在三角形内" };
}

// 凸多边形包含判断实现（叉积法）
std::pair<bool, std::string> polygon::containsPolygon1(const point& p) const {
    if (edges.size() < 3) return { false, "非法的多边形" };

    int windingNumber = 0;
    for (const auto& edge : edges) {
        const point& a = edge.p1;
        const point& b = edge.p2;

        if (a.y <= p.y) {
            if (b.y > p.y && cross(a, b, p) > 0) {
                windingNumber++;
            }
        }
        else {
            if (b.y <= p.y && cross(a, b, p) < 0) {
                windingNumber--;
            }
        }

        // 检查是否在边上
        if (cross(a, b, p) == 0) {
            if ((p.x >= min(a.x, b.x)) && (p.x <= max(a.x, b.x)) &&
                (p.y >= min(a.y, b.y)) && (p.y <= max(a.y, b.y))) {
                return { true, "点在多边形边上" };
            }
        }
    }

    return { windingNumber != 0, windingNumber != 0 ? "点在多边形内" : "点在多边形外" };
}

int main() {
    // === 三角形测试 ===
    std::cout << "=== 三角形点包含测试 ===\n";
    triangle tri(point(0, 0), point(4, 0), point(2, 3));
    point ptInTri(2, 1), ptOutTri(5, 5);

    auto [res1, reason1] = tri.containsTriangle1(ptInTri);
    std::cout << "点(2,1) 是否在三角形内？" << (res1 ? "是" : "否") << "，原因：" << reason1 << "\n";

    auto [res2, reason2] = tri.containsTriangle1(ptOutTri);
    std::cout << "点(5,5) 是否在三角形内？" << (res2 ? "是" : "否") << "，原因：" << reason2 << "\n";


    // === 凸多边形测试 ===
    std::cout << "\n=== 凸多边形点包含测试 ===\n";
    std::vector<point> convexPoints = {
        point(0, 0), point(4, 0), point(4, 4), point(0, 4)
    };
    polygon convexPolygon(convexPoints);

    point ptInConvex(2, 2), ptOutConvex(5, 5);
    auto [res3, reason3] = convexPolygon.containsPolygon1(ptInConvex);
    std::cout << "点(2,2) 是否在凸多边形内？" << (res3 ? "是" : "否") << "，原因：" << reason3 << "\n";

    auto [res4, reason4] = convexPolygon.containsPolygon1(ptOutConvex);
    std::cout << "点(5,5) 是否在凸多边形内？" << (res4 ? "是" : "否") << "，原因：" << reason4 << "\n";

    return 0;
}