#define INF 2000
#define true 1
#define false 0

typedef struct Point
{
    int x;
    int y;
}Point;

int max(int a, int b);
int min(int a, int b);

// Given three collinear points p, q, r, the function checks if
// point p lies on line segment 'qr'
int onSegment(Point p, Point q, Point r);

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are collinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(Point p, Point q, Point r);

// The function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
int doIntersect(Point p1, Point q1, Point p2, Point q2);

// Returns true if the point p lies inside the polygon[] with n vertices
int isInside(Point polygon[], int n, Point p);
