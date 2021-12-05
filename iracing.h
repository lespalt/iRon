/*
MIT License

Copyright (c) 2021 lespalt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "irsdk/irsdk_defines.h"
#include "irsdk/irsdk_client.h"
#include "irsdk/yaml_parser.h"
#include <string>

#define IR_MAX_CARS 64

struct Car
{
    std::string userName;
    std::string carNumber;
    std::string licenseStr;
    int license = -1;
    int irating = -1;
};

struct Session
{
    Car cars[IR_MAX_CARS];
    int driverCarIdx = -1;
};

extern irsdkCVar ir_SessionTime;
extern irsdkCVar ir_SessionTick;
extern irsdkCVar ir_SessionNum;
extern irsdkCVar ir_SessionState;
extern irsdkCVar ir_SessionUniqueID;
extern irsdkCVar ir_SessionFlags;
extern irsdkCVar ir_SessionTimeRemain;
extern irsdkCVar ir_SessionLapsRemain;
extern irsdkCVar ir_SessionLapsRemainEx;
extern irsdkCVar ir_SessionTimeTotal;
extern irsdkCVar ir_SessionLapsTotal;
extern irsdkCVar ir_SessionTimeOfDay;
extern irsdkCVar ir_RadioTransmitCarIdx;
extern irsdkCVar ir_RadioTransmitRadioIdx;
extern irsdkCVar ir_RadioTransmitFrequencyIdx;
extern irsdkCVar ir_DisplayUnits;
extern irsdkCVar ir_DriverMarker;
extern irsdkCVar ir_PushToPass;
extern irsdkCVar ir_ManualBoost;
extern irsdkCVar ir_ManualNoBoost;
extern irsdkCVar ir_IsOnTrack;
extern irsdkCVar ir_IsReplayPlaying;
extern irsdkCVar ir_ReplayFrameNum;
extern irsdkCVar ir_ReplayFrameNumEnd;
extern irsdkCVar ir_IsDiskLoggingEnabled;
extern irsdkCVar ir_IsDiskLoggingActive;
extern irsdkCVar ir_FrameRate;
extern irsdkCVar ir_CpuUsageFG;
extern irsdkCVar ir_GpuUsage;
extern irsdkCVar ir_ChanAvgLatency;
extern irsdkCVar ir_ChanLatency;
extern irsdkCVar ir_ChanQuality;
extern irsdkCVar ir_ChanPartnerQuality;
extern irsdkCVar ir_CpuUsageBG;
extern irsdkCVar ir_ChanClockSkew;
extern irsdkCVar ir_MemPageFaultSec;
extern irsdkCVar ir_PlayerCarPosition;
extern irsdkCVar ir_PlayerCarClassPosition;
extern irsdkCVar ir_PlayerCarClass;
extern irsdkCVar ir_PlayerTrackSurface;
extern irsdkCVar ir_PlayerTrackSurfaceMaterial;
extern irsdkCVar ir_PlayerCarIdx;
extern irsdkCVar ir_PlayerCarTeamIncidentCount;
extern irsdkCVar ir_PlayerCarMyIncidentCount;
extern irsdkCVar ir_PlayerCarDriverIncidentCount;
extern irsdkCVar ir_PlayerCarWeightPenalty;
extern irsdkCVar ir_PlayerCarPowerAdjust;
extern irsdkCVar ir_PlayerCarDryTireSetLimit;
extern irsdkCVar ir_PlayerCarTowTime;
extern irsdkCVar ir_PlayerCarInPitStall;
extern irsdkCVar ir_PlayerCarPitSvStatus;
extern irsdkCVar ir_PlayerTireCompound;
extern irsdkCVar ir_PlayerFastRepairsUsed;
extern irsdkCVar ir_CarIdxLap;
extern irsdkCVar ir_CarIdxLapCompleted;
extern irsdkCVar ir_CarIdxLapDistPct;
extern irsdkCVar ir_CarIdxTrackSurface;
extern irsdkCVar ir_CarIdxTrackSurfaceMaterial;
extern irsdkCVar ir_CarIdxOnPitRoad;
extern irsdkCVar ir_CarIdxPosition;
extern irsdkCVar ir_CarIdxClassPosition;
extern irsdkCVar ir_CarIdxClass;
extern irsdkCVar ir_CarIdxF2Time;
extern irsdkCVar ir_CarIdxEstTime;
extern irsdkCVar ir_CarIdxLastLapTime;
extern irsdkCVar ir_CarIdxBestLapTime;
extern irsdkCVar ir_CarIdxBestLapNum;
extern irsdkCVar ir_CarIdxTireCompound;
extern irsdkCVar ir_CarIdxQualTireCompound;
extern irsdkCVar ir_CarIdxQualTireCompoundLocked;
extern irsdkCVar ir_CarIdxFastRepairsUsed;
extern irsdkCVar ir_PaceMode;
extern irsdkCVar ir_CarIdxPaceLine;
extern irsdkCVar ir_CarIdxPaceRow;
extern irsdkCVar ir_CarIdxPaceFlags;
extern irsdkCVar ir_OnPitRoad;
extern irsdkCVar ir_CarIdxSteer;
extern irsdkCVar ir_CarIdxRPM;
extern irsdkCVar ir_CarIdxGear;
extern irsdkCVar ir_SteeringWheelAngle;
extern irsdkCVar ir_Throttle;
extern irsdkCVar ir_Brake;
extern irsdkCVar ir_Clutch;
extern irsdkCVar ir_Gear;
extern irsdkCVar ir_RPM;
extern irsdkCVar ir_Lap;
extern irsdkCVar ir_LapCompleted;
extern irsdkCVar ir_LapDist;
extern irsdkCVar ir_LapDistPct;
extern irsdkCVar ir_RaceLaps;
extern irsdkCVar ir_LapBestLap;
extern irsdkCVar ir_LapBestLapTime;
extern irsdkCVar ir_LapLastLapTime;
extern irsdkCVar ir_LapCurrentLapTime;
extern irsdkCVar ir_LapLasNLapSeq;
extern irsdkCVar ir_LapLastNLapTime;
extern irsdkCVar ir_LapBestNLapLap;
extern irsdkCVar ir_LapBestNLapTime;
extern irsdkCVar ir_LapDeltaToBestLap;
extern irsdkCVar ir_LapDeltaToBestLap_DD;
extern irsdkCVar ir_LapDeltaToBestLap_OK;
extern irsdkCVar ir_LapDeltaToOptimalLap;
extern irsdkCVar ir_LapDeltaToOptimalLap_DD;
extern irsdkCVar ir_LapDeltaToOptimalLap_OK;
extern irsdkCVar ir_LapDeltaToSessionBestLap;
extern irsdkCVar ir_LapDeltaToSessionBestLap_DD;
extern irsdkCVar ir_LapDeltaToSessionBestLap_OK;
extern irsdkCVar ir_LapDeltaToSessionOptimalLap;
extern irsdkCVar ir_LapDeltaToSessionOptimalLap_DD;
extern irsdkCVar ir_LapDeltaToSessionOptimalLap_OK;
extern irsdkCVar ir_LapDeltaToSessionLastlLap;
extern irsdkCVar ir_LapDeltaToSessionLastlLap_DD;
extern irsdkCVar ir_LapDeltaToSessionLastlLap_OK;
extern irsdkCVar ir_Speed;
extern irsdkCVar ir_Yaw;
extern irsdkCVar ir_YawNorth;
extern irsdkCVar ir_Pitch;
extern irsdkCVar ir_Roll;
extern irsdkCVar ir_EnterExitReset;
extern irsdkCVar ir_TrackTemp;
extern irsdkCVar ir_TrackTempCrew;
extern irsdkCVar ir_AirTemp;
extern irsdkCVar ir_WeatherType;
extern irsdkCVar ir_Skies;
extern irsdkCVar ir_AirDensity;
extern irsdkCVar ir_AirPressure;
extern irsdkCVar ir_WindVel;
extern irsdkCVar ir_WindDir;
extern irsdkCVar ir_RelativeHumidity;
extern irsdkCVar ir_FogLevel;
extern irsdkCVar ir_DCLapStatus;
extern irsdkCVar ir_DCDriversSoFar;
extern irsdkCVar ir_OkToReloadTextures;
extern irsdkCVar ir_LoadNumTextures;
extern irsdkCVar ir_CarLeftRight;
extern irsdkCVar ir_PitsOpen;
extern irsdkCVar ir_VidCapEnabled;
extern irsdkCVar ir_VidCapActive;
extern irsdkCVar ir_PitRepairLeft;
extern irsdkCVar ir_PitOptRepairLeft;
extern irsdkCVar ir_PitstopActive;
extern irsdkCVar ir_FastRepairUsed;
extern irsdkCVar ir_FastRepairAvailable;
extern irsdkCVar ir_LFTiresUsed;
extern irsdkCVar ir_RFTiresUsed;
extern irsdkCVar ir_LRTiresUsed;
extern irsdkCVar ir_RRTiresUsed;
extern irsdkCVar ir_LeftTireSetsUsed;
extern irsdkCVar ir_RightTireSetsUsed;
extern irsdkCVar ir_FrontTireSetsUsed;
extern irsdkCVar ir_RearTireSetsUsed;
extern irsdkCVar ir_TireSetsUsed;
extern irsdkCVar ir_LFTiresAvailable;
extern irsdkCVar ir_RFTiresAvailable;
extern irsdkCVar ir_LRTiresAvailable;
extern irsdkCVar ir_RRTiresAvailable;
extern irsdkCVar ir_LeftTireSetsAvailable;
extern irsdkCVar ir_RightTireSetsAvailable;
extern irsdkCVar ir_FrontTireSetsAvailable;
extern irsdkCVar ir_RearTireSetsAvailable;
extern irsdkCVar ir_TireSetsAvailable;
extern irsdkCVar ir_CamCarIdx;
extern irsdkCVar ir_CamCameraNumber;
extern irsdkCVar ir_CamGroupNumber;
extern irsdkCVar ir_CamCameraState;
extern irsdkCVar ir_IsOnTrackCar;
extern irsdkCVar ir_IsInGarage;
extern irsdkCVar ir_SteeringWheelPctTorque;
extern irsdkCVar ir_SteeringWheelPctTorqueSign;
extern irsdkCVar ir_SteeringWheelPctTorqueSignStops;
extern irsdkCVar ir_SteeringWheelPctDamper;
extern irsdkCVar ir_SteeringWheelAngleMax;
extern irsdkCVar ir_SteeringWheelLimiter;
extern irsdkCVar ir_ShiftIndicatorPct;
extern irsdkCVar ir_ShiftPowerPct;
extern irsdkCVar ir_ShiftGrindRPM;
extern irsdkCVar ir_ThrottleRaw;
extern irsdkCVar ir_BrakeRaw;
extern irsdkCVar ir_HandbrakeRaw;
extern irsdkCVar ir_SteeringWheelPeakForceNm;
extern irsdkCVar ir_SteeringWheelMaxForceNm;
extern irsdkCVar ir_SteeringWheelUseLinear;
extern irsdkCVar ir_BrakeABSactive;
extern irsdkCVar ir_EngineWarnings;
extern irsdkCVar ir_FuelLevel;
extern irsdkCVar ir_FuelLevelPct;
extern irsdkCVar ir_PitSvFlags;
extern irsdkCVar ir_PitSvLFP;
extern irsdkCVar ir_PitSvRFP;
extern irsdkCVar ir_PitSvLRP;
extern irsdkCVar ir_PitSvRRP;
extern irsdkCVar ir_PitSvFuel;
extern irsdkCVar ir_PitSvTireCompound;
extern irsdkCVar ir_CarIdxP2P_Status;
extern irsdkCVar ir_CarIdxP2P_Count;
extern irsdkCVar ir_ReplayPlaySpeed;
extern irsdkCVar ir_ReplayPlaySlowMotion;
extern irsdkCVar ir_ReplaySessionTime;
extern irsdkCVar ir_ReplaySessionNum;
extern irsdkCVar ir_TireLF_RumblePitch;
extern irsdkCVar ir_TireRF_RumblePitch;
extern irsdkCVar ir_TireLR_RumblePitch;
extern irsdkCVar ir_TireRR_RumblePitch;
extern irsdkCVar ir_SteeringWheelTorque_ST;
extern irsdkCVar ir_SteeringWheelTorque;
extern irsdkCVar ir_VelocityZ_ST;
extern irsdkCVar ir_VelocityY_ST;
extern irsdkCVar ir_VelocityX_ST;
extern irsdkCVar ir_VelocityZ;
extern irsdkCVar ir_VelocityY;
extern irsdkCVar ir_VelocityX;
extern irsdkCVar ir_YawRate_ST;
extern irsdkCVar ir_PitchRate_ST;
extern irsdkCVar ir_RollRate_ST;
extern irsdkCVar ir_YawRate;
extern irsdkCVar ir_PitchRate;
extern irsdkCVar ir_RollRate;
extern irsdkCVar ir_VertAccel_ST;
extern irsdkCVar ir_LatAccel_ST;
extern irsdkCVar ir_LongAccel_ST;
extern irsdkCVar ir_VertAccel;
extern irsdkCVar ir_LatAccel;
extern irsdkCVar ir_LongAccel;
extern irsdkCVar ir_dcStarter;
extern irsdkCVar ir_dpRTireChange;
extern irsdkCVar ir_dpLTireChange;
extern irsdkCVar ir_dpFuelFill;
extern irsdkCVar ir_dpWindshieldTearoff;
extern irsdkCVar ir_dpFuelAddKg;
extern irsdkCVar ir_dpFastRepair;
extern irsdkCVar ir_dcBrakeBias;
extern irsdkCVar ir_dpLFTireColdPress;
extern irsdkCVar ir_dpRFTireColdPress;
extern irsdkCVar ir_dpLRTireColdPress;
extern irsdkCVar ir_dpRRTireColdPress;
extern irsdkCVar ir_dpWeightJackerLeft;
extern irsdkCVar ir_dpWeightJackerRight;
extern irsdkCVar ir_WaterTemp;
extern irsdkCVar ir_WaterLevel;
extern irsdkCVar ir_FuelPress;
extern irsdkCVar ir_FuelUsePerHour;
extern irsdkCVar ir_OilTemp;
extern irsdkCVar ir_OilPress;
extern irsdkCVar ir_OilLevel;
extern irsdkCVar ir_Voltage;
extern irsdkCVar ir_ManifoldPress;
extern irsdkCVar ir_RFcoldPressure;
extern irsdkCVar ir_RFtempCL;
extern irsdkCVar ir_RFtempCM;
extern irsdkCVar ir_RFtempCR;
extern irsdkCVar ir_RFwearL;
extern irsdkCVar ir_RFwearM;
extern irsdkCVar ir_RFwearR;
extern irsdkCVar ir_LFcoldPressure;
extern irsdkCVar ir_LFtempCL;
extern irsdkCVar ir_LFtempCM;
extern irsdkCVar ir_LFtempCR;
extern irsdkCVar ir_LFwearL;
extern irsdkCVar ir_LFwearM;
extern irsdkCVar ir_LFwearR;
extern irsdkCVar ir_RRcoldPressure;
extern irsdkCVar ir_RRtempCL;
extern irsdkCVar ir_RRtempCM;
extern irsdkCVar ir_RRtempCR;
extern irsdkCVar ir_RRwearL;
extern irsdkCVar ir_RRwearM;
extern irsdkCVar ir_RRwearR;
extern irsdkCVar ir_LRcoldPressure;
extern irsdkCVar ir_LRtempCL;
extern irsdkCVar ir_LRtempCM;
extern irsdkCVar ir_LRtempCR;
extern irsdkCVar ir_LRwearL;
extern irsdkCVar ir_LRwearM;
extern irsdkCVar ir_LRwearR;
extern irsdkCVar ir_RRSHshockDefl;
extern irsdkCVar ir_RRSHshockDefl_ST;
extern irsdkCVar ir_RRSHshockVel;
extern irsdkCVar ir_RRSHshockVel_ST;
extern irsdkCVar ir_LRSHshockDefl;
extern irsdkCVar ir_LRSHshockDefl_ST;
extern irsdkCVar ir_LRSHshockVel;
extern irsdkCVar ir_LRSHshockVel_ST;
extern irsdkCVar ir_RFSHshockDefl;
extern irsdkCVar ir_RFSHshockDefl_ST;
extern irsdkCVar ir_RFSHshockVel;
extern irsdkCVar ir_RFSHshockVel_ST;
extern irsdkCVar ir_LFSHshockDefl;
extern irsdkCVar ir_LFSHshockDefl_ST;
extern irsdkCVar ir_LFSHshockVel;
extern irsdkCVar ir_LFSHshockVel_ST;

extern Session ir_session;

// Return whether we're (still) connected, and keep the session updated.
// Will block for around 16 milliseconds.
bool ir_tick();

// Print all the variables the sim supports.
void ir_printVariables();