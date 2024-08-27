#include <iostream>
#include <cmath>
#include <cstring>

struct Point {
    int x, y;

    // Default constructor
    Point() : x(0), y(0) {}

    // Parameterized constructor
    Point(int _x, int _y) : x(_x), y(_y) {}
};

class Shape {
    int vertices;
    Point** points;

public:
    // Constructor with initialization list
    Shape(int _vertices) : vertices(_vertices) {
        points = new Point*[vertices + 1];
    }

    // Destructor to release allocated memory
    ~Shape() {
        for (int i = 0; i <= vertices; ++i) {
            delete points[i];
        }
        delete[] points;
    }

    void addPoints(Point pts[]) {
        for (int i = 0; i <= vertices; i++) {
            points[i] = new Point(pts[i % vertices]);
        }
    }

    double area() {
        int temp = 0;
        for (int i = 0; i < vertices; i++) {
            // Accessing members of pointers using '->' for lhs
            int lhs = points[i]->x * points[(i + 1) % vertices]->y;
            // Accessing members of pointers using '->' for rhs
            int rhs = points[(i + 1) % vertices]->x * points[i]->y;
            temp += (lhs - rhs);
        }
        double area = std::abs(temp) / 2.0;
        return area;
    }
};

int main() {
    // Creating points using the three different methods of defining structs:
    Point tri1; // Default constructor
    tri1.x = 0; tri1.y = 0;

    Point tri2(1, 2); // Parameterized constructor

    Point tri3 = {2, 0}; // Aggregate initialization

    // Adding points to the triangle
    Point triPts[3] = {tri1, tri2, tri3};
    Shape* tri = new Shape(3);
    tri->addPoints(triPts);

    // Creating points using the preferred struct definition:
    Point quad1(0, 0);
    Point quad2(0, 2);
    Point quad3(2, 2);
    Point quad4(2, 0);

    // Adding points to the quadrilateral
    Point quadPts[4] = {quad1, quad2, quad3, quad4};
    Shape* quad = new Shape(4);
    quad->addPoints(quadPts);

    // Printing out the area of tri and quad
    std::cout << "Area of triangle: " << tri->area() << std::endl;
    std::cout << "Area of quadrilateral: " << quad->area() << std::endl;

    // Cleaning up allocated memory
    delete tri;
    delete quad;

    return 0;
}

