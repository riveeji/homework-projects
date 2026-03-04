#include <iostream>
#include "positionIdentifier.h"

// 计算向量叉积
float crossProduct(const point& a, conslt point& b, const point& c)
{
    
    float x1 = b.x - a.x;
    float y1 = b.y - a.y;
    float x2 = c.x - a.x;
    float y2 = c.y - a.y;
    return x1 * y2 - x2 * y1;
}

// 利用叉积判断点是否在三角形内（包含边）
std::pair<bool, std::string> triangle::containsTriangle1(const point& p) const
{
    float cross1 = crossProduct(p1, p2, p);
    float cross2 = crossProduct(p2, p3, p);
    float cross3 = crossProduct(p3, p1, p);

    // 输出过程说明
    std::string reason = "叉积结果如下：\n";
    reason += "cross(p1, p2, p) = " + std::to_string(cross1) + "\n";
    reason += "cross(p2, p3, p) = " + std::to_string(cross2) + "\n";
    reason += "cross(p3, p1, p) = " + std::to_string(cross3) + "\n";

    bool isInside = (cross1 >= 0 && cross2 >= 0 && cross3 >= 0) ||
        (cross1 <= 0 && cross2 <= 0 && cross3 <= 0);

    reason += isInside ? "→ 点在三角形内或边上。" : "→ 点在三角形外部。";
    return { isInside, reason };
}

int main()
{
    // 定义三角形 ABC（逆时针）
    point A(0, 0);
    point B(4, 0);
    point C(2, 3);
    triangle tri(A, B, C);

    // 测试点 D
    point D(2, 1);  // 可替换为任意点

    // 判断 D 是否在三角形 ABC 内
    auto result = tri.containsTriangle1(D);

    std::cout << "判断结果：" << (result.first ? "在三角形内" : "不在三角形内") << std::endl;
    std::cout << result.second << std::endl;

    return 0;
}
