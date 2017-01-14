#ifdef TrigLookup_h
#define TrigLookup_h

#include "TrigLookup.h"

int sinList[] = {0.00, 0.02, 0.03, 0.05, 0.07, 0.09, 0.10, 0.12, 0.14, 0.16, 0.17, 0.19, 0.21, 0.22, 0.24, 0.26, 0.28, 0.29, 0.31, 0.33, 0.34, 0.36, 0.37, 0.39, 0.41, 0.42, 0.44, 0.45, 0.47, 0.48, 0.50, 0.52, 0.53, 0.54, 0.56, 0.57, 0.59, 0.60, 0.62, 0.63, 0.64, 0.66, 0.67, 0.68, 0.69, 0.71, 0.72, 0.73, 0.74, 0.75, 0.77, 0.78, 0.79, 0.80, 0.81, 0.82, 0.83, 0.84, 0.85, 0.86, 0.87, 0.87, 0.88, 0.89, 0.90, 0.91, 0.91, 0.92, 0.93, 0.93, 0.94, 0.95, 0.95, 0.96, 0.96, 0.97, 0.97, 0.97, 0.98, 0.98, 0.98, 0.99, 0.99, 0.99, 0.99, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 0.99, 0.99, 0.99, 0.99, 0.98, 0.98, 0.98, 0.97, 0.97, 0.97, 0.96, 0.96, 0.95, 0.95, 0.94, 0.93, 0.93, 0.92, 0.91, 0.91, 0.90, 0.89, 0.88, 0.87, 0.87, 0.86, 0.85, 0.84, 0.83, 0.82, 0.81, 0.80, 0.79, 0.78, 0.77, 0.75, 0.74, 0.73, 0.72, 0.71, 0.69, 0.68, 0.67, 0.66, 0.64, 0.63, 0.62, 0.60, 0.59, 0.57, 0.56, 0.54, 0.53, 0.52, 0.50, 0.48, 0.47, 0.45, 0.44, 0.42, 0.41, 0.39, 0.37, 0.36, 0.34, 0.33, 0.31, 0.29, 0.28, 0.26, 0.24, 0.22, 0.21, 0.19, 0.17, 0.16, 0.14, 0.12, 0.10, 0.09, 0.07, 0.05, 0.03, 0.02, -0.00, -0.02, -0.03, -0.05, -0.07, -0.09, -0.10, -0.12, -0.14, -0.16, -0.17, -0.19, -0.21, -0.22, -0.24, -0.26, -0.28, -0.29, -0.31, -0.33, -0.34, -0.36, -0.37, -0.39, -0.41, -0.42, -0.44, -0.45, -0.47, -0.48, -0.50, -0.52, -0.53, -0.54, -0.56, -0.57, -0.59, -0.60, -0.62, -0.63, -0.64, -0.66, -0.67, -0.68, -0.69, -0.71, -0.72, -0.73, -0.74, -0.75, -0.77, -0.78, -0.79, -0.80, -0.81, -0.82, -0.83, -0.84, -0.85, -0.86, -0.87, -0.87, -0.88, -0.89, -0.90, -0.91, -0.91, -0.92, -0.93, -0.93, -0.94, -0.95, -0.95, -0.96, -0.96, -0.97, -0.97, -0.97, -0.98, -0.98, -0.98, -0.99, -0.99, -0.99, -0.99, -1.00, -1.00, -1.00, -1.00, -1.00, -1.00, -1.00, -1.00, -1.00, -1.00, -1.00, -0.99, -0.99, -0.99, -0.99, -0.98, -0.98, -0.98, -0.97, -0.97, -0.97, -0.96, -0.96, -0.95, -0.95, -0.94, -0.93, -0.93, -0.92, -0.91, -0.91, -0.90, -0.89, -0.88, -0.87, -0.87, -0.86, -0.85, -0.84, -0.83, -0.82, -0.81, -0.80, -0.79, -0.78, -0.77, -0.75, -0.74, -0.73, -0.72, -0.71, -0.69, -0.68, -0.67, -0.66, -0.64, -0.63, -0.62, -0.60, -0.59, -0.57, -0.56, -0.54, -0.53, -0.52, -0.50, -0.48, -0.47, -0.45, -0.44, -0.42, -0.41, -0.39, -0.37, -0.36, -0.34, -0.33, -0.31, -0.29, -0.28, -0.26, -0.24, -0.22, -0.21, -0.19, -0.17, -0.16, -0.14, -0.12, -0.10, -0.09, -0.07, -0.05, -0.03, -0.02, 0.00};

float lookupSin(float input)
{
  return sinList[(int)input];
}

float lookupCos(float input)
{
  return sinList[phaseshift((int) input)];
}

int phaseshift(int value)
{
  int out = value + 90;
  if(out > 360)
  {
    out =- 360;
  }
  return out;
}

#endif
