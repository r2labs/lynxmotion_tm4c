#ifndef __GRADIENTMAP_HPP__
#define __GRADIENTMAP_HPP__

#include <map>
#include <math.h>
#define DEBUG 1
#ifdef DEBUG
#include <iostream>
#include <numeric>
#include <vector>
#endif

template <typename T>
class bilinear_interpolator {
public:
    T get(T x, T y, std::map<T, std::map<T, T> > m) {

        auto it1 = m.lower_bound(x);
        auto it2 = it1->second.lower_bound(y);

        T x_lower, x_higher, y_lower, y_higher;

        if (it1->first == x) {
            x_lower = x;
            x_higher = x;
        } else {
            x_higher = it1->first;
            it1--;
            x_lower = it1->first;
        }

        if (it2->first == y) {
            y_lower = y;
            y_higher = y;
        } else {
            y_higher = it2->first;
            it2--;
            y_lower = it2->first;
        }

        float xy[4];
        xy[0] = m[x_lower][y_lower];
        xy[1] = m[x_higher][y_lower];
        xy[2] = m[x_lower][y_higher];
        xy[3] = m[x_higher][y_higher];

        if (xy[0] == 0 || xy[1] == 0 || xy[2] == 0 || xy[3] == 0) {
            return (T)barycentric(x, y, x_lower, x_higher, y_lower, y_higher, xy);
        }

        return (T)(bilinear(x, y, x_lower, x_higher, y_lower, y_higher, xy));
    }

private:
    float lerp(float x, float x_min, float x_max, float y_min, float y_max) {
        if (x >= x_max) { return y_max; }
        else if (x <= x_min) { return y_min; }
        return y_min + (y_max - y_min)*((x - x_min)/(x_max - x_min));
    }

    float barycentric(T x, T y, T x1, T x2, T y1, T y2, T z[4]) {

        /* set up coordinate systems */
        std::vector<T> xs, ys, zs;
        if (z[0] != 0) {
            xs.push_back(x1);
            ys.push_back(y1);
            zs.push_back(z[0]);
        } if (z[1] != 0) {
            xs.push_back(x2);
            ys.push_back(y1);
            zs.push_back(z[1]);
        } if (z[2] != 0) {
            xs.push_back(x1);
            ys.push_back(y2);
            zs.push_back(z[2]);
        } if (z[3] != 0) {
            xs.push_back(x2);
            ys.push_back(y2);
            zs.push_back(z[3]);
        }

        // float area_tot = fabs((xs[0]*(ys[1] - ys[2]) + xs[1]*(ys[2] - ys[0]) + xs[2]*(ys[0] - ys[1]))/2);
        float area_tot = t_area(xs[0], xs[1], xs[2], ys[0], ys[1], ys[2]);
        float area_0 = t_area(xs[1], xs[2], x, ys[1], ys[2], y);
        float area_1 = t_area(xs[0], xs[2], x, ys[0], ys[2], y);
        float area_2 = t_area(xs[0], xs[1], x, ys[0], ys[1], y);

        float pct_0 = area_0/area_tot;
        float pct_1 = area_1/area_tot;
        float pct_2 = area_2/area_tot;

        return ((pct_0 * zs[0]) + (pct_1 * zs[1]) + (pct_2 * zs[2]));
    }

    float t_area(T x1, T x2, T x3, T y1, T y2, T y3) {
        return fabs((x1*(y2 - y3) + x2*(y3 - y1) + x3*(y1 - y2))/2);
    }

    float bilinear(T x, T y, T x_lower, T x_higher, T y_lower, T y_higher,
                   float xy[4]) {
        float x1_lerp = lerp(x, x_lower, x_higher, xy[0], xy[2]);
        float x2_lerp = lerp(x, x_lower, x_higher, xy[1], xy[3]);
        float xy_lerp = lerp(y, y_lower, y_higher, x1_lerp, x2_lerp);

#ifdef DEBUG
        std::cout << "wanted: " << x << ", " << y << "\n" <<
            "got_x: " << x_lower << ", " << x_higher << "\n" <<
            "got_y: " << y_lower << ", " << y_higher << "\n";

        std::cout << xy[0] << "\t" << xy[2] << "\n"
                  << xy[1] << "\t" << xy[3] << "\n";

        std::cout << "x1lerp: " << x1_lerp << "\nx2lerp: " << x2_lerp
                  << "\nxylerp: " << xy_lerp << "\n";
#endif

        return xy_lerp;
    }
};

#endif /* __GRADIENTMAP_HPP__ */
