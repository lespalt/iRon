/*
MIT License

Copyright (c) 2021-2022 L. E. Spalt

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
#include "util.h"

#define IR_MAX_CARS 64

enum class ConnectionStatus
{
    UNKNOWN = 0,
    DISCONNECTED,
    CONNECTED,
    DRIVING
};
static const char* const ConnectionStatusStr[] = {"UNKNOWN","DISCONNECTED","CONNECTED","DRIVING"};

enum class SessionType
{
    UNKNOWN = 0,
    PRACTICE,
    QUALIFY,
    RACE
};
static const char* const SessionTypeStr[] = {"UNKNOWN","PRACTICE","QUALIFY","RACE"};

struct Car
{    
    std::string     userName;
    int             carNumber = 0;
    std::string     carNumberStr;
    std::string     licenseStr;
    char            licenseChar = 'R';
    float           licenseSR = 0;
    std::string     licenseColStr;
    float4          licenseCol = float4(0,0,0,1);
    int             irating = 0;
    int             isSelf = 0;
    int             isPaceCar = 0;
    int             isSpectator = 0;
    int             isBuddy = 0;
    int             isFlagged = 0;
    int             incidentCount = 0;
    float           carClassEstLapTime = 0;
    int             practicePosition = 0;
    int             qualPosition = 0;
    float           qualTime = 0;
    int             racePosition = 0;
    int             lastLapInPits = 0;
};

struct Session
{
    SessionType     sessionType = SessionType::UNKNOWN;
    Car             cars[IR_MAX_CARS];
    int             driverCarIdx = -1;
    int             sof = 0;
    int             subsessionId = 0;
    int             isFixedSetup = 0;
    int             isUnlimitedTime = 0;
    int             isUnlimitedLaps = 0;
    float           fuelMaxLtr = 0;
    float           rpmIdle = 0;
    float           rpmRedline = 0;
    float           rpmSLFirst = 0;
    float           rpmSLShift = 0;
    float           rpmSLLast = 0;
    float           rpmSLBlink = 0;
};

extern irsdkCVar ir_SessionTime;    // double[1] Seconds since session start (s)
extern irsdkCVar ir_SessionTick;    // int[1] Current update number ()
extern irsdkCVar ir_SessionNum;    // int[1] Session number ()
extern irsdkCVar ir_SessionState;    // int[1] Session state (irsdk_SessionState)
extern irsdkCVar ir_SessionUniqueID;    // int[1] Session ID ()
extern irsdkCVar ir_SessionFlags;    // bitfield[1] Session flags (irsdk_Flags)
extern irsdkCVar ir_SessionTimeRemain;    // double[1] Seconds left till session ends (s)
extern irsdkCVar ir_SessionLapsRemain;    // int[1] Old laps left till session ends use SessionLapsRemainEx ()
extern irsdkCVar ir_SessionLapsRemainEx;    // int[1] New improved laps left till session ends ()
extern irsdkCVar ir_SessionTimeTotal;    // double[1] Total number of seconds in session (s)
extern irsdkCVar ir_SessionLapsTotal;    // int[1] Total number of laps in session ()
extern irsdkCVar ir_SessionTimeOfDay;    // float[1] Time of day in seconds (s)
extern irsdkCVar ir_RadioTransmitCarIdx;    // int[1] The car index of the current person speaking on the radio ()
extern irsdkCVar ir_RadioTransmitRadioIdx;    // int[1] The radio index of the current person speaking on the radio ()
extern irsdkCVar ir_RadioTransmitFrequencyIdx;    // int[1] The frequency index of the current person speaking on the radio ()
extern irsdkCVar ir_DisplayUnits;    // int[1] Default units for the user interface 0 = english 1 = metric ()
extern irsdkCVar ir_DriverMarker;    // bool[1] Driver activated flag ()
extern irsdkCVar ir_PushToPass;    // bool[1] Push to pass button state ()
extern irsdkCVar ir_ManualBoost;    // bool[1] Hybrid manual boost state ()
extern irsdkCVar ir_ManualNoBoost;    // bool[1] Hybrid manual no boost state ()
extern irsdkCVar ir_IsOnTrack;    // bool[1] 1=Car on track physics running with player in car ()
extern irsdkCVar ir_IsReplayPlaying;    // bool[1] 0=replay not playing  1=replay playing ()
extern irsdkCVar ir_ReplayFrameNum;    // int[1] Integer replay frame number (60 per second) ()
extern irsdkCVar ir_ReplayFrameNumEnd;    // int[1] Integer replay frame number from end of tape ()
extern irsdkCVar ir_IsDiskLoggingEnabled;    // bool[1] 0=disk based telemetry turned off  1=turned on ()
extern irsdkCVar ir_IsDiskLoggingActive;    // bool[1] 0=disk based telemetry file not being written  1=being written ()
extern irsdkCVar ir_FrameRate;    // float[1] Average frames per second (fps)
extern irsdkCVar ir_CpuUsageFG;    // float[1] Percent of available tim fg thread took with a 1 sec avg (%)
extern irsdkCVar ir_GpuUsage;    // float[1] Percent of available tim gpu took with a 1 sec avg (%)
extern irsdkCVar ir_ChanAvgLatency;    // float[1] Communications average latency (s)
extern irsdkCVar ir_ChanLatency;    // float[1] Communications latency (s)
extern irsdkCVar ir_ChanQuality;    // float[1] Communications quality (%)
extern irsdkCVar ir_ChanPartnerQuality;    // float[1] Partner communications quality (%)
extern irsdkCVar ir_CpuUsageBG;    // float[1] Percent of available tim bg thread took with a 1 sec avg (%)
extern irsdkCVar ir_ChanClockSkew;    // float[1] Communications server clock skew (s)
extern irsdkCVar ir_MemPageFaultSec;    // float[1] Memory page faults per second ()
extern irsdkCVar ir_PlayerCarPosition;    // int[1] Players position in race ()
extern irsdkCVar ir_PlayerCarClassPosition;    // int[1] Players class position in race ()
extern irsdkCVar ir_PlayerCarClass;    // int[1] Player car class id ()
extern irsdkCVar ir_PlayerTrackSurface;    // int[1] Players car track surface type (irsdk_TrkLoc)
extern irsdkCVar ir_PlayerTrackSurfaceMaterial;    // int[1] Players car track surface material type (irsdk_TrkSurf)
extern irsdkCVar ir_PlayerCarIdx;    // int[1] Players carIdx ()
extern irsdkCVar ir_PlayerCarTeamIncidentCount;    // int[1] Players team incident count for this session ()
extern irsdkCVar ir_PlayerCarMyIncidentCount;    // int[1] Players own incident count for this session ()
extern irsdkCVar ir_PlayerCarDriverIncidentCount;    // int[1] Teams current drivers incident count for this session ()
extern irsdkCVar ir_PlayerCarWeightPenalty;    // float[1] Players weight penalty (kg)
extern irsdkCVar ir_PlayerCarPowerAdjust;    // float[1] Players power adjust (%)
extern irsdkCVar ir_PlayerCarDryTireSetLimit;    // int[1] Players dry tire set limit ()
extern irsdkCVar ir_PlayerCarTowTime;    // float[1] Players car is being towed if time is greater than zero (s)
extern irsdkCVar ir_PlayerCarInPitStall;    // bool[1] Players car is properly in there pitstall ()
extern irsdkCVar ir_PlayerCarPitSvStatus;    // int[1] Players car pit service status bits (irsdk_PitSvStatus)
extern irsdkCVar ir_PlayerTireCompound;    // int[1] Players car current tire compound ()
extern irsdkCVar ir_PlayerFastRepairsUsed;    // int[1] Players car number of fast repairs used ()
extern irsdkCVar ir_CarIdxLap;    // int[64] Laps started by car index ()
extern irsdkCVar ir_CarIdxLapCompleted;    // int[64] Laps completed by car index ()
extern irsdkCVar ir_CarIdxLapDistPct;    // float[64] Percentage distance around lap by car index (%)
extern irsdkCVar ir_CarIdxTrackSurface;    // int[64] Track surface type by car index (irsdk_TrkLoc)
extern irsdkCVar ir_CarIdxTrackSurfaceMaterial;    // int[64] Track surface material type by car index (irsdk_TrkSurf)
extern irsdkCVar ir_CarIdxOnPitRoad;    // bool[64] On pit road between the cones by car index ()
extern irsdkCVar ir_CarIdxPosition;    // int[64] Cars position in race by car index ()
extern irsdkCVar ir_CarIdxClassPosition;    // int[64] Cars class position in race by car index ()
extern irsdkCVar ir_CarIdxClass;    // int[64] Cars class id by car index ()
extern irsdkCVar ir_CarIdxF2Time;    // float[64] Race time behind leader or fastest lap time otherwise (s)
extern irsdkCVar ir_CarIdxEstTime;    // float[64] Estimated time to reach current location on track (s)
extern irsdkCVar ir_CarIdxLastLapTime;    // float[64] Cars last lap time (s)
extern irsdkCVar ir_CarIdxBestLapTime;    // float[64] Cars best lap time (s)
extern irsdkCVar ir_CarIdxBestLapNum;    // int[64] Cars best lap number ()
extern irsdkCVar ir_CarIdxTireCompound;    // int[64] Cars current tire compound ()
extern irsdkCVar ir_CarIdxQualTireCompound;    // int[64] Cars Qual tire compound ()
extern irsdkCVar ir_CarIdxQualTireCompoundLocked;    // bool[64] Cars Qual tire compound is locked-in ()
extern irsdkCVar ir_CarIdxFastRepairsUsed;    // int[64] How many fast repairs each car has used ()
extern irsdkCVar ir_PaceMode;    // int[1] Are we pacing or not (irsdk_PaceMode)
extern irsdkCVar ir_CarIdxPaceLine;    // int[64] What line cars are pacing in  or -1 if not pacing ()
extern irsdkCVar ir_CarIdxPaceRow;    // int[64] What row cars are pacing in  or -1 if not pacing ()
extern irsdkCVar ir_CarIdxPaceFlags;    // int[64] Pacing status flags for each car (irsdk_PaceFlags)
extern irsdkCVar ir_OnPitRoad;    // bool[1] Is the player car on pit road between the cones ()
extern irsdkCVar ir_CarIdxSteer;    // float[64] Steering wheel angle by car index (rad)
extern irsdkCVar ir_CarIdxRPM;    // float[64] Engine rpm by car index (revs/min)
extern irsdkCVar ir_CarIdxGear;    // int[64] -1=reverse  0=neutral  1..n=current gear by car index ()
extern irsdkCVar ir_SteeringWheelAngle;    // float[1] Steering wheel angle (rad)
extern irsdkCVar ir_Throttle;    // float[1] 0=off throttle to 1=full throttle (%)
extern irsdkCVar ir_Brake;    // float[1] 0=brake released to 1=max pedal force (%)
extern irsdkCVar ir_Clutch;    // float[1] 0=disengaged to 1=fully engaged (%)
extern irsdkCVar ir_Gear;    // int[1] -1=reverse  0=neutral  1..n=current gear ()
extern irsdkCVar ir_RPM;    // float[1] Engine rpm (revs/min)
extern irsdkCVar ir_Lap;    // int[1] Laps started count ()
extern irsdkCVar ir_LapCompleted;    // int[1] Laps completed count ()
extern irsdkCVar ir_LapDist;    // float[1] Meters traveled from S/F this lap (m)
extern irsdkCVar ir_LapDistPct;    // float[1] Percentage distance around lap (%)
extern irsdkCVar ir_RaceLaps;    // int[1] Laps completed in race ()
extern irsdkCVar ir_LapBestLap;    // int[1] Players best lap number ()
extern irsdkCVar ir_LapBestLapTime;    // float[1] Players best lap time (s)
extern irsdkCVar ir_LapLastLapTime;    // float[1] Players last lap time (s)
extern irsdkCVar ir_LapCurrentLapTime;    // float[1] Estimate of players current lap time as shown in F3 box (s)
extern irsdkCVar ir_LapLasNLapSeq;    // int[1] Player num consecutive clean laps completed for N average ()
extern irsdkCVar ir_LapLastNLapTime;    // float[1] Player last N average lap time (s)
extern irsdkCVar ir_LapBestNLapLap;    // int[1] Player last lap in best N average lap time ()
extern irsdkCVar ir_LapBestNLapTime;    // float[1] Player best N average lap time (s)
extern irsdkCVar ir_LapDeltaToBestLap;    // float[1] Delta time for best lap (s)
extern irsdkCVar ir_LapDeltaToBestLap_DD;    // float[1] Rate of change of delta time for best lap (s/s)
extern irsdkCVar ir_LapDeltaToBestLap_OK;    // bool[1] Delta time for best lap is valid ()
extern irsdkCVar ir_LapDeltaToOptimalLap;    // float[1] Delta time for optimal lap (s)
extern irsdkCVar ir_LapDeltaToOptimalLap_DD;    // float[1] Rate of change of delta time for optimal lap (s/s)
extern irsdkCVar ir_LapDeltaToOptimalLap_OK;    // bool[1] Delta time for optimal lap is valid ()
extern irsdkCVar ir_LapDeltaToSessionBestLap;    // float[1] Delta time for session best lap (s)
extern irsdkCVar ir_LapDeltaToSessionBestLap_DD;    // float[1] Rate of change of delta time for session best lap (s/s)
extern irsdkCVar ir_LapDeltaToSessionBestLap_OK;    // bool[1] Delta time for session best lap is valid ()
extern irsdkCVar ir_LapDeltaToSessionOptimalLap;    // float[1] Delta time for session optimal lap (s)
extern irsdkCVar ir_LapDeltaToSessionOptimalLap_DD;    // float[1] Rate of change of delta time for session optimal lap (s/s)
extern irsdkCVar ir_LapDeltaToSessionOptimalLap_OK;    // bool[1] Delta time for session optimal lap is valid ()
extern irsdkCVar ir_LapDeltaToSessionLastlLap;    // float[1] Delta time for session last lap (s)
extern irsdkCVar ir_LapDeltaToSessionLastlLap_DD;    // float[1] Rate of change of delta time for session last lap (s/s)
extern irsdkCVar ir_LapDeltaToSessionLastlLap_OK;    // bool[1] Delta time for session last lap is valid ()
extern irsdkCVar ir_Speed;    // float[1] GPS vehicle speed (m/s)
extern irsdkCVar ir_Yaw;    // float[1] Yaw orientation (rad)
extern irsdkCVar ir_YawNorth;    // float[1] Yaw orientation relative to north (rad)
extern irsdkCVar ir_Pitch;    // float[1] Pitch orientation (rad)
extern irsdkCVar ir_Roll;    // float[1] Roll orientation (rad)
extern irsdkCVar ir_EnterExitReset;    // int[1] Indicate action the reset key will take 0 enter 1 exit 2 reset ()
extern irsdkCVar ir_TrackTemp;    // float[1] Deprecated  set to TrackTempCrew (C)
extern irsdkCVar ir_TrackTempCrew;    // float[1] Temperature of track measured by crew around track (C)
extern irsdkCVar ir_AirTemp;    // float[1] Temperature of air at start/finish line (C)
extern irsdkCVar ir_WeatherType;    // int[1] Weather type (0=constant  1=dynamic) ()
extern irsdkCVar ir_Skies;    // int[1] Skies (0=clear/1=p cloudy/2=m cloudy/3=overcast) ()
extern irsdkCVar ir_AirDensity;    // float[1] Density of air at start/finish line (kg/m^3)
extern irsdkCVar ir_AirPressure;    // float[1] Pressure of air at start/finish line (Hg)
extern irsdkCVar ir_WindVel;    // float[1] Wind velocity at start/finish line (m/s)
extern irsdkCVar ir_WindDir;    // float[1] Wind direction at start/finish line (rad)
extern irsdkCVar ir_RelativeHumidity;    // float[1] Relative Humidity (%)
extern irsdkCVar ir_FogLevel;    // float[1] Fog level (%)
extern irsdkCVar ir_DCLapStatus;    // int[1] Status of driver change lap requirements ()
extern irsdkCVar ir_DCDriversSoFar;    // int[1] Number of team drivers who have run a stint ()
extern irsdkCVar ir_OkToReloadTextures;    // bool[1] True if it is ok to reload car textures at this time ()
extern irsdkCVar ir_LoadNumTextures;    // bool[1] True if the car_num texture will be loaded ()
extern irsdkCVar ir_CarLeftRight;    // bitfield[1] Notify if car is to the left or right of driver (irsdk_CarLeftRight)
extern irsdkCVar ir_PitsOpen;    // bool[1] True if pit stop is allowed for the current player ()
extern irsdkCVar ir_VidCapEnabled;    // bool[1] True if video capture system is enabled ()
extern irsdkCVar ir_VidCapActive;    // bool[1] True if video currently being captured ()
extern irsdkCVar ir_PitRepairLeft;    // float[1] Time left for mandatory pit repairs if repairs are active (s)
extern irsdkCVar ir_PitOptRepairLeft;    // float[1] Time left for optional repairs if repairs are active (s)
extern irsdkCVar ir_PitstopActive;    // bool[1] Is the player getting pit stop service ()
extern irsdkCVar ir_FastRepairUsed;    // int[1] How many fast repairs used so far ()
extern irsdkCVar ir_FastRepairAvailable;    // int[1] How many fast repairs left  255 is unlimited ()
extern irsdkCVar ir_LFTiresUsed;    // int[1] How many left front tires used so far ()
extern irsdkCVar ir_RFTiresUsed;    // int[1] How many right front tires used so far ()
extern irsdkCVar ir_LRTiresUsed;    // int[1] How many left rear tires used so far ()
extern irsdkCVar ir_RRTiresUsed;    // int[1] How many right rear tires used so far ()
extern irsdkCVar ir_LeftTireSetsUsed;    // int[1] How many left tire sets used so far ()
extern irsdkCVar ir_RightTireSetsUsed;    // int[1] How many right tire sets used so far ()
extern irsdkCVar ir_FrontTireSetsUsed;    // int[1] How many front tire sets used so far ()
extern irsdkCVar ir_RearTireSetsUsed;    // int[1] How many rear tire sets used so far ()
extern irsdkCVar ir_TireSetsUsed;    // int[1] How many tire sets used so far ()
extern irsdkCVar ir_LFTiresAvailable;    // int[1] How many left front tires are remaining  255 is unlimited ()
extern irsdkCVar ir_RFTiresAvailable;    // int[1] How many right front tires are remaining  255 is unlimited ()
extern irsdkCVar ir_LRTiresAvailable;    // int[1] How many left rear tires are remaining  255 is unlimited ()
extern irsdkCVar ir_RRTiresAvailable;    // int[1] How many right rear tires are remaining  255 is unlimited ()
extern irsdkCVar ir_LeftTireSetsAvailable;    // int[1] How many left tire sets are remaining  255 is unlimited ()
extern irsdkCVar ir_RightTireSetsAvailable;    // int[1] How many right tire sets are remaining  255 is unlimited ()
extern irsdkCVar ir_FrontTireSetsAvailable;    // int[1] How many front tire sets are remaining  255 is unlimited ()
extern irsdkCVar ir_RearTireSetsAvailable;    // int[1] How many rear tire sets are remaining  255 is unlimited ()
extern irsdkCVar ir_TireSetsAvailable;    // int[1] How many tire sets are remaining  255 is unlimited ()
extern irsdkCVar ir_CamCarIdx;    // int[1] Active camera's focus car index ()
extern irsdkCVar ir_CamCameraNumber;    // int[1] Active camera number ()
extern irsdkCVar ir_CamGroupNumber;    // int[1] Active camera group number ()
extern irsdkCVar ir_CamCameraState;    // bitfield[1] State of camera system (irsdk_CameraState)
extern irsdkCVar ir_IsOnTrackCar;    // bool[1] 1=Car on track physics running ()
extern irsdkCVar ir_IsInGarage;    // bool[1] 1=Car in garage physics running ()
extern irsdkCVar ir_SteeringWheelPctTorque;    // float[1] Force feedback % max torque on steering shaft unsigned (%)
extern irsdkCVar ir_SteeringWheelPctTorqueSign;    // float[1] Force feedback % max torque on steering shaft signed (%)
extern irsdkCVar ir_SteeringWheelPctTorqueSignStops;    // float[1] Force feedback % max torque on steering shaft signed stops (%)
extern irsdkCVar ir_SteeringWheelPctDamper;    // float[1] Force feedback % max damping (%)
extern irsdkCVar ir_SteeringWheelAngleMax;    // float[1] Steering wheel max angle (rad)
extern irsdkCVar ir_SteeringWheelLimiter;    // float[1] Force feedback limiter strength limits impacts and oscillation (%)
extern irsdkCVar ir_ShiftIndicatorPct;    // float[1] DEPRECATED use DriverCarSLBlinkRPM instead (%)
extern irsdkCVar ir_ShiftPowerPct;    // float[1] Friction torque applied to gears when shifting or grinding (%)
extern irsdkCVar ir_ShiftGrindRPM;    // float[1] RPM of shifter grinding noise (RPM)
extern irsdkCVar ir_ThrottleRaw;    // float[1] Raw throttle input 0=off throttle to 1=full throttle (%)
extern irsdkCVar ir_BrakeRaw;    // float[1] Raw brake input 0=brake released to 1=max pedal force (%)
extern irsdkCVar ir_HandbrakeRaw;    // float[1] Raw handbrake input 0=handbrake released to 1=max force (%)
extern irsdkCVar ir_SteeringWheelPeakForceNm;    // float[1] Peak torque mapping to direct input units for FFB (N*m)
extern irsdkCVar ir_SteeringWheelMaxForceNm;    // float[1] Value of strength or max force slider in Nm for FFB (N*m)
extern irsdkCVar ir_SteeringWheelUseLinear;    // bool[1] True if steering wheel force is using linear mode ()
extern irsdkCVar ir_BrakeABSactive;    // bool[1] true if abs is currently reducing brake force pressure ()
extern irsdkCVar ir_EngineWarnings;    // bitfield[1] Bitfield for warning lights (irsdk_EngineWarnings)
extern irsdkCVar ir_FuelLevel;    // float[1] Liters of fuel remaining (l)
extern irsdkCVar ir_FuelLevelPct;    // float[1] Percent fuel remaining (%)
extern irsdkCVar ir_PitSvFlags;    // bitfield[1] Bitfield of pit service checkboxes (irsdk_PitSvFlags)
extern irsdkCVar ir_PitSvLFP;    // float[1] Pit service left front tire pressure (kPa)
extern irsdkCVar ir_PitSvRFP;    // float[1] Pit service right front tire pressure (kPa)
extern irsdkCVar ir_PitSvLRP;    // float[1] Pit service left rear tire pressure (kPa)
extern irsdkCVar ir_PitSvRRP;    // float[1] Pit service right rear tire pressure (kPa)
extern irsdkCVar ir_PitSvFuel;    // float[1] Pit service fuel add amount (l)
extern irsdkCVar ir_PitSvTireCompound;    // int[1] Pit service pending tire compound ()
extern irsdkCVar ir_CarIdxP2P_Status;    // bool[64] Push2Pass active or not ()
extern irsdkCVar ir_CarIdxP2P_Count;    // int[64] Push2Pass count of usage (or remaining in Race) ()
extern irsdkCVar ir_ReplayPlaySpeed;    // int[1] Replay playback speed ()
extern irsdkCVar ir_ReplayPlaySlowMotion;    // bool[1] 0=not slow motion  1=replay is in slow motion ()
extern irsdkCVar ir_ReplaySessionTime;    // double[1] Seconds since replay session start (s)
extern irsdkCVar ir_ReplaySessionNum;    // int[1] Replay session number ()
extern irsdkCVar ir_TireLF_RumblePitch;    // float[1] Players LF Tire Sound rumblestrip pitch (Hz)
extern irsdkCVar ir_TireRF_RumblePitch;    // float[1] Players RF Tire Sound rumblestrip pitch (Hz)
extern irsdkCVar ir_TireLR_RumblePitch;    // float[1] Players LR Tire Sound rumblestrip pitch (Hz)
extern irsdkCVar ir_TireRR_RumblePitch;    // float[1] Players RR Tire Sound rumblestrip pitch (Hz)
extern irsdkCVar ir_SteeringWheelTorque_ST;    // float[6] Output torque on steering shaft at 360 Hz (N*m)
extern irsdkCVar ir_SteeringWheelTorque;    // float[1] Output torque on steering shaft (N*m)
extern irsdkCVar ir_VelocityZ_ST;    // float[6] Z velocity (m/s at 360 Hz)
extern irsdkCVar ir_VelocityY_ST;    // float[6] Y velocity (m/s at 360 Hz)
extern irsdkCVar ir_VelocityX_ST;    // float[6] X velocity (m/s at 360 Hz)
extern irsdkCVar ir_VelocityZ;    // float[1] Z velocity (m/s)
extern irsdkCVar ir_VelocityY;    // float[1] Y velocity (m/s)
extern irsdkCVar ir_VelocityX;    // float[1] X velocity (m/s)
extern irsdkCVar ir_YawRate_ST;    // float[6] Yaw rate at 360 Hz (rad/s)
extern irsdkCVar ir_PitchRate_ST;    // float[6] Pitch rate at 360 Hz (rad/s)
extern irsdkCVar ir_RollRate_ST;    // float[6] Roll rate at 360 Hz (rad/s)
extern irsdkCVar ir_YawRate;    // float[1] Yaw rate (rad/s)
extern irsdkCVar ir_PitchRate;    // float[1] Pitch rate (rad/s)
extern irsdkCVar ir_RollRate;    // float[1] Roll rate (rad/s)
extern irsdkCVar ir_VertAccel_ST;    // float[6] Vertical acceleration (including gravity) at 360 Hz (m/s^2)
extern irsdkCVar ir_LatAccel_ST;    // float[6] Lateral acceleration (including gravity) at 360 Hz (m/s^2)
extern irsdkCVar ir_LongAccel_ST;    // float[6] Longitudinal acceleration (including gravity) at 360 Hz (m/s^2)
extern irsdkCVar ir_VertAccel;    // float[1] Vertical acceleration (including gravity) (m/s^2)
extern irsdkCVar ir_LatAccel;    // float[1] Lateral acceleration (including gravity) (m/s^2)
extern irsdkCVar ir_LongAccel;    // float[1] Longitudinal acceleration (including gravity) (m/s^2)
extern irsdkCVar ir_dcStarter;    // bool[1] In car trigger car starter ()
extern irsdkCVar ir_dpRTireChange;    // float[1] Pitstop right tire change request ()
extern irsdkCVar ir_dpLTireChange;    // float[1] Pitstop left tire change request ()
extern irsdkCVar ir_dpFuelFill;    // float[1] Pitstop fuel fill flag ()
extern irsdkCVar ir_dpWindshieldTearoff;    // float[1] Pitstop windshield tearoff ()
extern irsdkCVar ir_dpFuelAddKg;    // float[1] Pitstop fuel add ammount (kg)
extern irsdkCVar ir_dpFastRepair;    // float[1] Pitstop fast repair set ()
extern irsdkCVar ir_dcBrakeBias;    // float[1] In car brake bias adjustment ()
extern irsdkCVar ir_dpLFTireColdPress;    // float[1] Pitstop lf tire cold pressure adjustment (Pa)
extern irsdkCVar ir_dpRFTireColdPress;    // float[1] Pitstop rf cold tire pressure adjustment (Pa)
extern irsdkCVar ir_dpLRTireColdPress;    // float[1] Pitstop lr tire cold pressure adjustment (Pa)
extern irsdkCVar ir_dpRRTireColdPress;    // float[1] Pitstop rr cold tire pressure adjustment (Pa)
extern irsdkCVar ir_dpWeightJackerLeft;    // float[1] Pitstop left wedge/weight jacker adjustment ()
extern irsdkCVar ir_dpWeightJackerRight;    // float[1] Pitstop right wedge/weight jacker adjustment ()
extern irsdkCVar ir_WaterTemp;    // float[1] Engine coolant temp (C)
extern irsdkCVar ir_WaterLevel;    // float[1] Engine coolant level (l)
extern irsdkCVar ir_FuelPress;    // float[1] Engine fuel pressure (bar)
extern irsdkCVar ir_FuelUsePerHour;    // float[1] Engine fuel used instantaneous (kg/h)
extern irsdkCVar ir_OilTemp;    // float[1] Engine oil temperature (C)
extern irsdkCVar ir_OilPress;    // float[1] Engine oil pressure (bar)
extern irsdkCVar ir_OilLevel;    // float[1] Engine oil level (l)
extern irsdkCVar ir_Voltage;    // float[1] Engine voltage (V)
extern irsdkCVar ir_ManifoldPress;    // float[1] Engine manifold pressure (bar)
extern irsdkCVar ir_RFcoldPressure;    // float[1] RF tire cold pressure  as set in the garage (kPa)
extern irsdkCVar ir_RFtempCL;    // float[1] RF tire left carcass temperature (C)
extern irsdkCVar ir_RFtempCM;    // float[1] RF tire middle carcass temperature (C)
extern irsdkCVar ir_RFtempCR;    // float[1] RF tire right carcass temperature (C)
extern irsdkCVar ir_RFwearL;    // float[1] RF tire left percent tread remaining (%)
extern irsdkCVar ir_RFwearM;    // float[1] RF tire middle percent tread remaining (%)
extern irsdkCVar ir_RFwearR;    // float[1] RF tire right percent tread remaining (%)
extern irsdkCVar ir_LFcoldPressure;    // float[1] LF tire cold pressure  as set in the garage (kPa)
extern irsdkCVar ir_LFtempCL;    // float[1] LF tire left carcass temperature (C)
extern irsdkCVar ir_LFtempCM;    // float[1] LF tire middle carcass temperature (C)
extern irsdkCVar ir_LFtempCR;    // float[1] LF tire right carcass temperature (C)
extern irsdkCVar ir_LFwearL;    // float[1] LF tire left percent tread remaining (%)
extern irsdkCVar ir_LFwearM;    // float[1] LF tire middle percent tread remaining (%)
extern irsdkCVar ir_LFwearR;    // float[1] LF tire right percent tread remaining (%)
extern irsdkCVar ir_RRcoldPressure;    // float[1] RR tire cold pressure  as set in the garage (kPa)
extern irsdkCVar ir_RRtempCL;    // float[1] RR tire left carcass temperature (C)
extern irsdkCVar ir_RRtempCM;    // float[1] RR tire middle carcass temperature (C)
extern irsdkCVar ir_RRtempCR;    // float[1] RR tire right carcass temperature (C)
extern irsdkCVar ir_RRwearL;    // float[1] RR tire left percent tread remaining (%)
extern irsdkCVar ir_RRwearM;    // float[1] RR tire middle percent tread remaining (%)
extern irsdkCVar ir_RRwearR;    // float[1] RR tire right percent tread remaining (%)
extern irsdkCVar ir_LRcoldPressure;    // float[1] LR tire cold pressure  as set in the garage (kPa)
extern irsdkCVar ir_LRtempCL;    // float[1] LR tire left carcass temperature (C)
extern irsdkCVar ir_LRtempCM;    // float[1] LR tire middle carcass temperature (C)
extern irsdkCVar ir_LRtempCR;    // float[1] LR tire right carcass temperature (C)
extern irsdkCVar ir_LRwearL;    // float[1] LR tire left percent tread remaining (%)
extern irsdkCVar ir_LRwearM;    // float[1] LR tire middle percent tread remaining (%)
extern irsdkCVar ir_LRwearR;    // float[1] LR tire right percent tread remaining (%)
extern irsdkCVar ir_RRSHshockDefl;    // float[1] RRSH shock deflection (m)
extern irsdkCVar ir_RRSHshockDefl_ST;    // float[6] RRSH shock deflection at 360 Hz (m)
extern irsdkCVar ir_RRSHshockVel;    // float[1] RRSH shock velocity (m/s)
extern irsdkCVar ir_RRSHshockVel_ST;    // float[6] RRSH shock velocity at 360 Hz (m/s)
extern irsdkCVar ir_LRSHshockDefl;    // float[1] LRSH shock deflection (m)
extern irsdkCVar ir_LRSHshockDefl_ST;    // float[6] LRSH shock deflection at 360 Hz (m)
extern irsdkCVar ir_LRSHshockVel;    // float[1] LRSH shock velocity (m/s)
extern irsdkCVar ir_LRSHshockVel_ST;    // float[6] LRSH shock velocity at 360 Hz (m/s)
extern irsdkCVar ir_RFSHshockDefl;    // float[1] RFSH shock deflection (m)
extern irsdkCVar ir_RFSHshockDefl_ST;    // float[6] RFSH shock deflection at 360 Hz (m)
extern irsdkCVar ir_RFSHshockVel;    // float[1] RFSH shock velocity (m/s)
extern irsdkCVar ir_RFSHshockVel_ST;    // float[6] RFSH shock velocity at 360 Hz (m/s)
extern irsdkCVar ir_LFSHshockDefl;    // float[1] LFSH shock deflection (m)
extern irsdkCVar ir_LFSHshockDefl_ST;    // float[6] LFSH shock deflection at 360 Hz (m)
extern irsdkCVar ir_LFSHshockVel;    // float[1] LFSH shock velocity (m/s)
extern irsdkCVar ir_LFSHshockVel_ST;    // float[6] LFSH shock velocity at 360 Hz (m/s)

extern Session ir_session;

// Keep the session data updated.
// Will block for around 16 milliseconds.
ConnectionStatus ir_tick();

// Let the session data tracking know that the config has changed.
void ir_handleConfigChange();

// Return whether we're in the process of getting in the car, waiting for others
// to grid, or doing pace laps before the actual race start.
bool ir_isPreStart();

// Estimate time for a full lap.
float ir_estimateLaptime();

// Get the best known position, from the latest session we can find.
int ir_getPosition( int carIdx );

// Get lap delta to P0 car if available.
int ir_getLapDeltaToLeader( int carIdx, int ldrIdx );

// Print all the variables the sim supports.
void ir_printVariables();
