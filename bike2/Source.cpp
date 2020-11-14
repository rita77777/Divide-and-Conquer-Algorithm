#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <time.h> 
#include <stack> 
#include <iostream> 
#define EPISLON 1e-7
using namespace std;

typedef struct point_s {
	double x;
	double y;
}point;

double closest_split_pair(point *array, int mid, int len, double min);
int is_less_than(double x, double y)
{
	return x - y < -EPISLON;
}
double square(double x)
{
	return x * x;
}
double distance(point *p1, point *p2)
{
	int x1 = p1->x;
	int y1 = p1->y;
	int x2 = p2->x;
	int y2 = p2->y;
	return sqrt(square(x2 - x1) + square(y2 - y1));
}

//point sorted array(x is in ascending order, if not sorted, may use qsort to sort it to satisfy requirement) whose element number is len
double closest_pair(point *array, int len)
{
	double left_min, right_min;
	int mid;
	double min;
	double split_min;
	if (len == 2)
	{
		return distance(&array[0], &array[1]);
	}
	else if (len == 3)
	{
		min = distance(&array[0], &array[1]);
		double dist12 = distance(&array[1], &array[2]);
		double dist02 = distance(&array[0], &array[2]);
		if (is_less_than(dist12, min))
			min = dist12;
		if (is_less_than(dist02, min))
			min = dist02;
		return min;
	}
	mid = len / 2;
	left_min = closest_pair(array, mid);
	right_min = closest_pair(&array[mid], len - mid);
	min = is_less_than(left_min, right_min) ? left_min : right_min;
	//printf("%.3f %.3f ", left_min, right_min);
	split_min = closest_split_pair(array, mid, len, min);
	return is_less_than(min, split_min) ? min : split_min;
}

double closest_split_pair(point *array, int mid, int len, double min)
{
	int left_boundary, right_boundary;
	int i, j;
	for (left_boundary = 0; left_boundary < mid; left_boundary++)
		if (is_less_than(array[mid].x - min / 2, array[left_boundary].x))
			break;

	for (right_boundary = len - 1; right_boundary >= mid; right_boundary--)
		if (is_less_than(array[right_boundary].x, array[mid].x + min / 2))
			break;

	for (i = left_boundary; i < mid; i++)
		for (j = mid; j <= right_boundary; j++)
			if (is_less_than(distance(&array[i], &array[j]), min))
				min = distance(&array[i], &array[j]);
	return min;
}
point p0;

// A utility function to find next to top in a stack 
point nextToTop(stack<point> &S)
{
	point p = S.top();
	S.pop();
	point res = S.top();
	S.push(p);
	return res;
}

// A utility function to swap two points 
void swap(point &p1, point &p2)
{
	point temp = p1;
	p1 = p2;
	p2 = temp;
}

// A utility function to return square of distance 
// between p1 and p2 
int distSq(point p1, point p2)
{
	return (p1.x - p2.x)*(p1.x - p2.x) +
		(p1.y - p2.y)*(p1.y - p2.y);
}

// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
int orientation(point p, point q, point r)
{
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear 
	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

// A function used by library function qsort() to sort an array of 
// points with respect to the first point 
int compare(const void *vp1, const void *vp2)
{
	point *p1 = (point *)vp1;
	point *p2 = (point *)vp2;

	// Find orientation 
	int o = orientation(p0, *p1, *p2);
	if (o == 0)
		return (distSq(p0, *p2) >= distSq(p0, *p1)) ? -1 : 1;

	return (o == 2) ? -1 : 1;
}
// Prints convex hull of a set of n points. 
void convexHull(point points[], int n)
{
	// Find the bottommost point 
	int ymin = points[0].y, min = 0;
	for (int i = 1; i < n; i++)
	{
		int y = points[i].y;

		// Pick the bottom-most or chose the left 
		// most point in case of tie 
		if ((y < ymin) || (ymin == y &&
			points[i].x < points[min].x))
			ymin = points[i].y, min = i;
	}

	// Place the bottom-most point at first position 
	swap(points[0], points[min]);

	// Sort n-1 points with respect to the first point. 
	// A point p1 comes before p2 in sorted output if p2 
	// has larger polar angle (in counterclockwise 
	// direction) than p1 
	p0 = points[0];
	qsort(&points[1], n - 1, sizeof(point), compare);

	// If two or more points make same angle with p0, 
	// Remove all but the one that is farthest from p0 
	// Remember that, in above sorting, our criteria was 
	// to keep the farthest point at the end when more than 
	// one points have same angle. 
	int m = 1; // Initialize size of modified array 
	for (int i = 1; i < n; i++)
	{
		// Keep removing i while angle of i and i+1 is same 
		// with respect to p0 
		while (i < n - 1 && orientation(p0, points[i],
			points[i + 1]) == 0)
			i++;


		points[m] = points[i];
		m++;  // Update size of modified array 
	}

	// If modified array of points has less than 3 points, 
	// convex hull is not possible 
	if (m < 3) return;

	// Create an empty stack and push first three points 
	// to it. 
	stack<point> S;
	S.push(points[0]);
	S.push(points[1]);
	S.push(points[2]);

	// Process remaining n-3 points 
	for (int i = 3; i < m; i++)
	{
		// Keep removing top while the angle formed by 
		// points next-to-top, top, and points[i] makes 
		// a non-left turn 
		while (orientation(nextToTop(S), S.top(), points[i]) != 2)
			S.pop();
		S.push(points[i]);
	}
	// Now stack has the output points, print contents of stack 
	while (!S.empty())
	{
		point p = S.top();
		cout << "(" << p.x << ", " << p.y << ")" << endl;
		S.pop();
	}
}
int main(void)
{
	double start = 0;
    double end = 0 ;
	//寫檔
	FILE *fp;
	fp = fopen("input_station.txt", "w+");
	srand(time(NULL));
	for (int i = 0; i < 100; i++)
	{
		int x = rand() % 1000;
		int y = rand() % 1000;
		//cout << x << endl <<y<< endl;
		fprintf(fp, "(%d,%d)\n", x, y);
	}
	fclose(fp);
	FILE *fp_r = fopen("input_station.txt", "r");
	double x = 0, y = 0;
	point point_arr[200];
	char StrLine[20];
	int index = 0;
	while (!feof(fp_r))
	{
		fgets(StrLine, 20, fp_r);  //讀取一行
		sscanf(StrLine, "(%lf,%lf)", &x, &y);
		point_arr[index].x = x;
		point_arr[index].y = y;
		index++;
	}
	fclose(fp_r);
	//在站點總數有100個底下的結果
	printf("the closest pair of station is: ");
	printf("%.3f\n", closest_pair(point_arr, 100));
	cout << "Boundary points of convex hull are: " << endl;
	start = clock();
	convexHull(point_arr, 100);
	end = clock();
	cout << (end - start) / CLOCKS_PER_SEC << endl;
	//cout << "By 皮克定理 the area of the convex hull is:" << 100 + (num / 2) - 1<<endl;
	system("PAUSE");
	return 0;
}