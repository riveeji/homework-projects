#include <iostream>
#include <vector>
#include "标头.h"
using namespace std;

struct Point {
    int x, y;
    Point(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}
};

class Triangle {
public:
    Point a, b, c;
    Triangle(Point a_, Point b_, Point c_) : a(a_), b(b_), c(c_) {}

    // 判断点 p 是否在三角形内
    bool contains(Point p) {
        int cross1 = positionIdentifier(a.x, a.y, b.x, b.y, p.x, p.y);
        int cross2 = positionIdentifier(b.x, b.y, c.x, c.y, p.x, p.y);
        int cross3 = positionIdentifier(c.x, c.y, a.x, a.y, p.x, p.y);

        bool has_neg = (cross1 < 0) || (cross2 < 0) || (cross3 < 0);
        bool has_pos = (cross1 > 0) || (cross2 > 0) || (cross3 > 0);

        return !(has_neg && has_pos);
    }
};

int main() {
    cout << "===== 三角形包含点判断程序（批量版）=====\n";

    // 读取三角形顶点
    Point a, b, c;
    cout << "请输入三角形顶点 A(x y): "; cin >> a.x >> a.y;
    cout << "请输入三角形顶点 B(x y): "; cin >> b.x >> b.y;
    cout << "请输入三角形顶点 C(x y): "; cin >> c.x >> c.y;

    Triangle tri(a, b, c);

    // 输入要判断的点
    int n;
    cout << "请输入要判断的点个数: ";
    cin >> n;

    vector<Point> testPoints;
    for (int i = 0; i < n; ++i) {
        int x, y;
        cout << "请输入第 " << i + 1 << " 个点 P(x y): ";
        cin >> x >> y;
        testPoints.emplace_back(x, y);
    }

    cout << "\n===== 判断结果 =====\n";
    for (int i = 0; i < n; ++i) {
        bool inside = tri.contains(testPoints[i]);
        cout << "点 P" << i + 1 << " (" << testPoints[i].x << ", " << testPoints[i].y << ") "
            << (inside ? "在三角形内（含边界）" : "在三角形外") << endl;
    }

    return 0;
}

