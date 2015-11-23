// Fill out your copyright notice in the Description page of Project Settings.

#ifndef _PERLIN_H_
#define _PERLIN_H_

#include "BlankProject.h"

class BLANKPROJECT_API Perlin {
public:
    ~Perlin();

    void init();

    // Generates a Perlin (smoothed) noise value between -1 and 1, at the given 3D position.
    float noise(float sample_x, float sample_y, float sample_z);


private:
    int *p; // Permutation table
    // Gradient vectors
    float *Gx;
    float *Gy;
    float *Gz;
};

#endif