#include "System.h"
#include "MPU6050\MPU6050.h"
#include "HMCMAG\HMCMAG.h"
#include "DCM\DCM.h"

//=====================================================
#ifndef __IMU_H
#define __IMU_H
//=====================================================
#define	IMU_OK		 0
#define IMU_STLMPU	 1
#define IMU_STLHMC	 2
#define IMU_STLBOTH	 3
#define IMU_NOTRDY	 4
//=====================================================
uint	IMUReset();
uint	IMUGetUpdate(DCMData* IMUResult);
uint	IMUGetUpdateWhenReady(DCMData* IMUResult);
//=====================================================
#endif



