#ifndef _FUNCTION_H
#define _FUNCTION_H

#define FUNCTION_MAX_POINTS 16

class Function
{
    public:
        Function();

        /**
         * Add a point (x, y) to the function
         */
        void addPoint(double x, double y);

        /**
         * Gets the xMax of the function
         */
        double getXMax();

        /**
         * Get the value for the given x
         */
        double get(double x);

        /**
         * Gets the value for the given x, modulo the size of the function
         */
        double getMod(double x);

    protected:
        /**
         * Function points
         */
        double points[FUNCTION_MAX_POINTS][2];

        /**
         * Number of points
         */
        int nbPoints;

        /**
         * The nth item of this array is the slope between the nth point and the
         * n+1th point
         */
        double ds[FUNCTION_MAX_POINTS-1];
};

#endif
