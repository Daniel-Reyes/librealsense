#pragma once

#include <stdint.h>
#include <stdio.h>
#include <iostream>

#include "R200_CalibRectParameters.h"

// Assume little-endian architecture
void myNtoh(unsigned char * result, const unsigned char * origin, int numBytes)
{
    const bool changeEndianness = true; //  false; // HERE!!!! WHY need to change to false?
    if (changeEndianness)
    {
        for (int i = 0; i < numBytes; i++)
        {
            result[i] = origin[numBytes - 1 - i];
        }
    }
    else
    {
        memcpy(result, origin, numBytes);
    }
}

template <class T>
static bool readFromBin(const unsigned char *& p, T & x)
{
    myNtoh((unsigned char *)&x, p, sizeof(T));
    p += sizeof(T);
    return true;
}

template <class T>
static bool readFromBin(const unsigned char *& p, T * px, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (!readFromBin(p, px[i]))
        {
            return false;
        }
    }
    return true;
}

template <class T>
static bool readFromBin(const unsigned char *& p, T * px, int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (!readFromBin(p, px[j + n * i]))
            {
                return false;
            }
        }
    }
    return true;
}

template <class T>
static bool readFromBin(const unsigned char *& p, T * px, int m, int n, int o)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < o; k++)
            {
                if (!readFromBin(p, px[k + o * (j + n * i)]))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

static bool readFromBin(const unsigned char *& p, DSCalibIntrinsicsNonRectified & cri)
{
    return readFromBin(p, cri.fx)
        && readFromBin(p, cri.fy)
        && readFromBin(p, cri.px)
        && readFromBin(p, cri.py)
        && readFromBin(p, cri.k, 5)
        && readFromBin(p, cri.w)
        && readFromBin(p, cri.h);
}

static bool readFromBin(const unsigned char *& p, DSCalibIntrinsicsRectified & crm)
{
    return readFromBin(p, crm.rfx)
        && readFromBin(p, crm.rfy)
        && readFromBin(p, crm.rpx)
        && readFromBin(p, crm.rpy)
        && readFromBin(p, crm.rw)
        && readFromBin(p, crm.rh);
}

inline bool ParseCalibrationRectifiedParametersFromMemory(DSCalibRectParameters & cal, const uint8_t * buffer)
{
    myNtoh((unsigned char *)&cal.versionNumber, buffer, sizeof(cal.versionNumber));
    
    if (cal.versionNumber <= 1)
    {
        //throw std::runtime_error("Unsupported calibration version. Use a newer firmware?");
    }

    //@tofix -- this is actually V1
        
    // Double check version number here
    const int
        mNIR = DS_MAX_NUM_INTRINSICS_RIGHT,
        mNIT = DS_MAX_NUM_INTRINSICS_THIRD,
        mNIP = DS_MAX_NUM_INTRINSICS_PLATFORM,
        mNMLR = DS_MAX_NUM_RECTIFIED_MODES_LR,
        mNMT = DS_MAX_NUM_RECTIFIED_MODES_THIRD,
        mNMP = DS_MAX_NUM_RECTIFIED_MODES_PLATFORM;
    
    const uint8_t * p = buffer;
    bool ok =
    readFromBin(p, cal.versionNumber)
        && readFromBin(p, cal.numIntrinsicsRight)
        && readFromBin(p, cal.numIntrinsicsThird)
        && readFromBin(p, cal.numIntrinsicsPlatform)
        && readFromBin(p, cal.numRectifiedModesLR)
        && readFromBin(p, cal.numRectifiedModesThird)
        && readFromBin(p, cal.numRectifiedModesPlatform)
        && readFromBin(p, cal.intrinsicsLeft)
        && readFromBin(p, cal.intrinsicsRight, mNIR)
        && readFromBin(p, cal.intrinsicsThird, mNIT)
        && readFromBin(p, cal.intrinsicsPlatform, mNIP)
        && readFromBin(p, &(cal.modesLR[0][0]), mNIR, mNMLR)
        && readFromBin(p, &(cal.modesThird[0][0][0]), mNIR, mNIT, mNMT)
        && readFromBin(p, &(cal.modesPlatform[0][0][0]), mNIR, mNIP, mNMP)
        && readFromBin(p, &(cal.Rleft[0][0]), mNIR, 9)
        && readFromBin(p, &(cal.Rright[0][0]), mNIR, 9)
        && readFromBin(p, &(cal.Rthird[0][0]), mNIR, 9)
        && readFromBin(p, &(cal.Rplatform[0][0]), mNIR, 9)              
        && readFromBin(p, cal.B, mNIR)                                  
        && readFromBin(p, &(cal.T[0][0]), mNIR, 3)                      
        && readFromBin(p, &(cal.Tplatform[0][0]), mNIR, 3)              
        && readFromBin(p, cal.Rworld, 9);
    
    return ok;
}