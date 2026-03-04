#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

// 叉积判断函数：p 在向量 AB 左侧时结果为正，右侧为负，在线上为0
int positionIdentifier(int x1, int y1, int x2, int y2, int x, int y) {
    return (x2 - x1) * (y - y1) - (x - x1) * (y2 - y1);
}

// 点结构
struct Point {
    int x, y;
    Point(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}
};

// 三角形类
class Triangle {
public:
    Point a, b, c;

    Triangle(Point a_, Point b_, Point c_) : a(a_), b(b_), c(c_) {}

    // 判断点是否在三角形内（含边界）
    bool contains(const Point& p) const {
        int cross1 = positionIdentifier(a.x, a.y, b.x, b.y, p.x, p.y);
        int cross2 = positionIdentifier(b.x, b.y, c.x, c.y, p.x, p.y);
        int cross3 = positionIdentifier(c.x, c.y, a.x, a.y, p.x, p.y);

        bool has_neg = (cross1 < 0) || (cross2 < 0) || (cross3 < 0);
        bool has_pos = (cross1 > 0) || (cross2 > 0) || (cross3 > 0);

        return !(has_neg && has_pos);
    }
};

int main() {
    cout << "===== 判断多个点是否在三角形内（含边界）=====\n";

    Point a, b, c;
    cout << "请输入三角形顶点 A(x y): "; cin >> a.x >> a.y;
    cout << "请输入三角形顶点 B(x y): "; cin >> b.x >> b.y;
    cout << "请输入三角形顶点 C(x y): "; cin >> c.x >> c.y;
    Triangle tri(a, b, c);

    vector<Point> testPoints;
    int method;
    cout << "请选择输入方式（1：手动输入  2：从文件读取）: ";
    cin >> method;

    if (method == 1) {
        int n;
        cout << "请输入要判断的点个数: ";
        cin >> n;
        for (int i = 0; i < n; ++i) {
            int x, y;
            cout << "请输入第 " << i + 1 << " 个点 P(x y): ";
            cin >> x >> y;
            testPoints.emplace_back(x, y);
        }
    }
    else if (method == 2) {
        string filename;
        cout << "请输入文件名: ";
        cin >> filename;
        ifstream fin(filename);
        if (!fin) {
            cerr << "无法打开文件: " << filename << endl;
            return 1;
        }
        int x, y;
        while (fin >> x >> y) {
            testPoints.emplace_back(x, y);
        }
        fin.close();
        cout << "已读取 " << testPoints.size() << " 个点。\n";
    }
    else {
        cerr << "无效的选项，程序结束。\n";
        return 1;
    }

    int insideCount = 0;
    cout << "\n===== 判断结果 =====\n";
    for (size_t i = 0; i < testPoints.size(); ++i) {
        const Point& p = testPoints[i];
        bool inside = tri.contains(p);
        if (inside) ++insideCount;
        cout << "点 P" << i + 1 << " (" << p.x << ", " << p.y << ") "
            << (inside ? "在三角形内（含边界）" : "在三角形外") << endl;
    }

    cout << "\n总共 " << testPoints.size() << " 个点，"
        << insideCount << " 个在三角形内（含边界），"
        << testPoints.size() - insideCount << " 个在三角形外。\n";

    return 0;
}
