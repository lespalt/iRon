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

#include "iracing.h"
#include "Config.h"

irsdkCVar ir_SessionTime("SessionTime");    // double[1] Seconds since session start (s)
irsdkCVar ir_SessionTick("SessionTick");    // int[1] Current update number ()
irsdkCVar ir_SessionNum("SessionNum");    // int[1] Session number ()
irsdkCVar ir_SessionState("SessionState");    // int[1] Session state (irsdk_SessionState)
irsdkCVar ir_SessionUniqueID("SessionUniqueID");    // int[1] Session ID ()
irsdkCVar ir_SessionFlags("SessionFlags");    // bitfield[1] Session flags (irsdk_Flags)
irsdkCVar ir_SessionTimeRemain("SessionTimeRemain");    // double[1] Seconds left till session ends (s)
irsdkCVar ir_SessionLapsRemain("SessionLapsRemain");    // int[1] Old laps left till session ends use SessionLapsRemainEx ()
irsdkCVar ir_SessionLapsRemainEx("SessionLapsRemainEx");    // int[1] New improved laps left till session ends ()
irsdkCVar ir_SessionTimeTotal("SessionTimeTotal");    // double[1] Total number of seconds in session (s)
irsdkCVar ir_SessionLapsTotal("SessionLapsTotal");    // int[1] Total number of laps in session ()
irsdkCVar ir_SessionTimeOfDay("SessionTimeOfDay");    // float[1] Time of day in seconds (s)
irsdkCVar ir_RadioTransmitCarIdx("RadioTransmitCarIdx");    // int[1] The car index of the current person speaking on the radio ()
irsdkCVar ir_RadioTransmitRadioIdx("RadioTransmitRadioIdx");    // int[1] The radio index of the current person speaking on the radio ()
irsdkCVar ir_RadioTransmitFrequencyIdx("RadioTransmitFrequencyIdx");    // int[1] The frequency index of the current person speaking on the radio ()
irsdkCVar ir_DisplayUnits("DisplayUnits");    // int[1] Default units for the user interface 0 = english 1 = metric ()
irsdkCVar ir_DriverMarker("DriverMarker");    // bool[1] Driver activated flag ()
irsdkCVar ir_PushToPass("PushToPass");    // bool[1] Push to pass button state ()
irsdkCVar ir_ManualBoost("ManualBoost");    // bool[1] Hybrid manual boost state ()
irsdkCVar ir_ManualNoBoost("ManualNoBoost");    // bool[1] Hybrid manual no boost state ()
irsdkCVar ir_IsOnTrack("IsOnTrack");    // bool[1] 1=Car on track physics running with player in car ()
irsdkCVar ir_IsReplayPlaying("IsReplayPlaying");    // bool[1] 0=replay not playing  1=replay playing ()
irsdkCVar ir_ReplayFrameNum("ReplayFrameNum");    // int[1] Integer replay frame number (60 per second) ()
irsdkCVar ir_ReplayFrameNumEnd("ReplayFrameNumEnd");    // int[1] Integer replay frame number from end of tape ()
irsdkCVar ir_IsDiskLoggingEnabled("IsDiskLoggingEnabled");    // bool[1] 0=disk based telemetry turned off  1=turned on ()
irsdkCVar ir_IsDiskLoggingActive("IsDiskLoggingActive");    // bool[1] 0=disk based telemetry file not being written  1=being written ()
irsdkCVar ir_FrameRate("FrameRate");    // float[1] Average frames per second (fps)
irsdkCVar ir_CpuUsageFG("CpuUsageFG");    // float[1] Percent of available tim fg thread took with a 1 sec avg (%)
irsdkCVar ir_GpuUsage("GpuUsage");    // float[1] Percent of available tim gpu took with a 1 sec avg (%)
irsdkCVar ir_ChanAvgLatency("ChanAvgLatency");    // float[1] Communications average latency (s)
irsdkCVar ir_ChanLatency("ChanLatency");    // float[1] Communications latency (s)
irsdkCVar ir_ChanQuality("ChanQuality");    // float[1] Communications quality (%)
irsdkCVar ir_ChanPartnerQuality("ChanPartnerQuality");    // float[1] Partner communications quality (%)
irsdkCVar ir_CpuUsageBG("CpuUsageBG");    // float[1] Percent of available tim bg thread took with a 1 sec avg (%)
irsdkCVar ir_ChanClockSkew("ChanClockSkew");    // float[1] Communications server clock skew (s)
irsdkCVar ir_MemPageFaultSec("MemPageFaultSec");    // float[1] Memory page faults per second ()
irsdkCVar ir_PlayerCarPosition("PlayerCarPosition");    // int[1] Players position in race ()
irsdkCVar ir_PlayerCarClassPosition("PlayerCarClassPosition");    // int[1] Players class position in race ()
irsdkCVar ir_PlayerCarClass("PlayerCarClass");    // int[1] Player car class id ()
irsdkCVar ir_PlayerTrackSurface("PlayerTrackSurface");    // int[1] Players car track surface type (irsdk_TrkLoc)
irsdkCVar ir_PlayerTrackSurfaceMaterial("PlayerTrackSurfaceMaterial");    // int[1] Players car track surface material type (irsdk_TrkSurf)
irsdkCVar ir_PlayerCarIdx("PlayerCarIdx");    // int[1] Players carIdx ()
irsdkCVar ir_PlayerCarTeamIncidentCount("PlayerCarTeamIncidentCount");    // int[1] Players team incident count for this session ()
irsdkCVar ir_PlayerCarMyIncidentCount("PlayerCarMyIncidentCount");    // int[1] Players own incident count for this session ()
irsdkCVar ir_PlayerCarDriverIncidentCount("PlayerCarDriverIncidentCount");    // int[1] Teams current drivers incident count for this session ()
irsdkCVar ir_PlayerCarWeightPenalty("PlayerCarWeightPenalty");    // float[1] Players weight penalty (kg)
irsdkCVar ir_PlayerCarPowerAdjust("PlayerCarPowerAdjust");    // float[1] Players power adjust (%)
irsdkCVar ir_PlayerCarDryTireSetLimit("PlayerCarDryTireSetLimit");    // int[1] Players dry tire set limit ()
irsdkCVar ir_PlayerCarTowTime("PlayerCarTowTime");    // float[1] Players car is being towed if time is greater than zero (s)
irsdkCVar ir_PlayerCarInPitStall("PlayerCarInPitStall");    // bool[1] Players car is properly in there pitstall ()
irsdkCVar ir_PlayerCarPitSvStatus("PlayerCarPitSvStatus");    // int[1] Players car pit service status bits (irsdk_PitSvStatus)
irsdkCVar ir_PlayerTireCompound("PlayerTireCompound");    // int[1] Players car current tire compound ()
irsdkCVar ir_PlayerFastRepairsUsed("PlayerFastRepairsUsed");    // int[1] Players car number of fast repairs used ()
irsdkCVar ir_CarIdxLap("CarIdxLap");    // int[64] Laps started by car index ()
irsdkCVar ir_CarIdxLapCompleted("CarIdxLapCompleted");    // int[64] Laps completed by car index ()
irsdkCVar ir_CarIdxLapDistPct("CarIdxLapDistPct");    // float[64] Percentage distance around lap by car index (%)
irsdkCVar ir_CarIdxTrackSurface("CarIdxTrackSurface");    // int[64] Track surface type by car index (irsdk_TrkLoc)
irsdkCVar ir_CarIdxTrackSurfaceMaterial("CarIdxTrackSurfaceMaterial");    // int[64] Track surface material type by car index (irsdk_TrkSurf)
irsdkCVar ir_CarIdxOnPitRoad("CarIdxOnPitRoad");    // bool[64] On pit road between the cones by car index ()
irsdkCVar ir_CarIdxPosition("CarIdxPosition");    // int[64] Cars position in race by car index ()
irsdkCVar ir_CarIdxClassPosition("CarIdxClassPosition");    // int[64] Cars class position in race by car index ()
irsdkCVar ir_CarIdxClass("CarIdxClass");    // int[64] Cars class id by car index ()
irsdkCVar ir_CarIdxF2Time("CarIdxF2Time");    // float[64] Race time behind leader or fastest lap time otherwise (s)
irsdkCVar ir_CarIdxEstTime("CarIdxEstTime");    // float[64] Estimated time to reach current location on track (s)
irsdkCVar ir_CarIdxLastLapTime("CarIdxLastLapTime");    // float[64] Cars last lap time (s)
irsdkCVar ir_CarIdxBestLapTime("CarIdxBestLapTime");    // float[64] Cars best lap time (s)
irsdkCVar ir_CarIdxBestLapNum("CarIdxBestLapNum");    // int[64] Cars best lap number ()
irsdkCVar ir_CarIdxTireCompound("CarIdxTireCompound");    // int[64] Cars current tire compound ()
irsdkCVar ir_CarIdxQualTireCompound("CarIdxQualTireCompound");    // int[64] Cars Qual tire compound ()
irsdkCVar ir_CarIdxQualTireCompoundLocked("CarIdxQualTireCompoundLocked");    // bool[64] Cars Qual tire compound is locked-in ()
irsdkCVar ir_CarIdxFastRepairsUsed("CarIdxFastRepairsUsed");    // int[64] How many fast repairs each car has used ()
irsdkCVar ir_PaceMode("PaceMode");    // int[1] Are we pacing or not (irsdk_PaceMode)
irsdkCVar ir_CarIdxPaceLine("CarIdxPaceLine");    // int[64] What line cars are pacing in  or -1 if not pacing ()
irsdkCVar ir_CarIdxPaceRow("CarIdxPaceRow");    // int[64] What row cars are pacing in  or -1 if not pacing ()
irsdkCVar ir_CarIdxPaceFlags("CarIdxPaceFlags");    // int[64] Pacing status flags for each car (irsdk_PaceFlags)
irsdkCVar ir_OnPitRoad("OnPitRoad");    // bool[1] Is the player car on pit road between the cones ()
irsdkCVar ir_CarIdxSteer("CarIdxSteer");    // float[64] Steering wheel angle by car index (rad)
irsdkCVar ir_CarIdxRPM("CarIdxRPM");    // float[64] Engine rpm by car index (revs/min)
irsdkCVar ir_CarIdxGear("CarIdxGear");    // int[64] -1=reverse  0=neutral  1..n=current gear by car index ()
irsdkCVar ir_SteeringWheelAngle("SteeringWheelAngle");    // float[1] Steering wheel angle (rad)
irsdkCVar ir_Throttle("Throttle");    // float[1] 0=off throttle to 1=full throttle (%)
irsdkCVar ir_Brake("Brake");    // float[1] 0=brake released to 1=max pedal force (%)
irsdkCVar ir_Clutch("Clutch");    // float[1] 0=disengaged to 1=fully engaged (%)
irsdkCVar ir_Gear("Gear");    // int[1] -1=reverse  0=neutral  1..n=current gear ()
irsdkCVar ir_RPM("RPM");    // float[1] Engine rpm (revs/min)
irsdkCVar ir_Lap("Lap");    // int[1] Laps started count ()
irsdkCVar ir_LapCompleted("LapCompleted");    // int[1] Laps completed count ()
irsdkCVar ir_LapDist("LapDist");    // float[1] Meters traveled from S/F this lap (m)
irsdkCVar ir_LapDistPct("LapDistPct");    // float[1] Percentage distance around lap (%)
irsdkCVar ir_RaceLaps("RaceLaps");    // int[1] Laps completed in race ()
irsdkCVar ir_LapBestLap("LapBestLap");    // int[1] Players best lap number ()
irsdkCVar ir_LapBestLapTime("LapBestLapTime");    // float[1] Players best lap time (s)
irsdkCVar ir_LapLastLapTime("LapLastLapTime");    // float[1] Players last lap time (s)
irsdkCVar ir_LapCurrentLapTime("LapCurrentLapTime");    // float[1] Estimate of players current lap time as shown in F3 box (s)
irsdkCVar ir_LapLasNLapSeq("LapLasNLapSeq");    // int[1] Player num consecutive clean laps completed for N average ()
irsdkCVar ir_LapLastNLapTime("LapLastNLapTime");    // float[1] Player last N average lap time (s)
irsdkCVar ir_LapBestNLapLap("LapBestNLapLap");    // int[1] Player last lap in best N average lap time ()
irsdkCVar ir_LapBestNLapTime("LapBestNLapTime");    // float[1] Player best N average lap time (s)
irsdkCVar ir_LapDeltaToBestLap("LapDeltaToBestLap");    // float[1] Delta time for best lap (s)
irsdkCVar ir_LapDeltaToBestLap_DD("LapDeltaToBestLap_DD");    // float[1] Rate of change of delta time for best lap (s/s)
irsdkCVar ir_LapDeltaToBestLap_OK("LapDeltaToBestLap_OK");    // bool[1] Delta time for best lap is valid ()
irsdkCVar ir_LapDeltaToOptimalLap("LapDeltaToOptimalLap");    // float[1] Delta time for optimal lap (s)
irsdkCVar ir_LapDeltaToOptimalLap_DD("LapDeltaToOptimalLap_DD");    // float[1] Rate of change of delta time for optimal lap (s/s)
irsdkCVar ir_LapDeltaToOptimalLap_OK("LapDeltaToOptimalLap_OK");    // bool[1] Delta time for optimal lap is valid ()
irsdkCVar ir_LapDeltaToSessionBestLap("LapDeltaToSessionBestLap");    // float[1] Delta time for session best lap (s)
irsdkCVar ir_LapDeltaToSessionBestLap_DD("LapDeltaToSessionBestLap_DD");    // float[1] Rate of change of delta time for session best lap (s/s)
irsdkCVar ir_LapDeltaToSessionBestLap_OK("LapDeltaToSessionBestLap_OK");    // bool[1] Delta time for session best lap is valid ()
irsdkCVar ir_LapDeltaToSessionOptimalLap("LapDeltaToSessionOptimalLap");    // float[1] Delta time for session optimal lap (s)
irsdkCVar ir_LapDeltaToSessionOptimalLap_DD("LapDeltaToSessionOptimalLap_DD");    // float[1] Rate of change of delta time for session optimal lap (s/s)
irsdkCVar ir_LapDeltaToSessionOptimalLap_OK("LapDeltaToSessionOptimalLap_OK");    // bool[1] Delta time for session optimal lap is valid ()
irsdkCVar ir_LapDeltaToSessionLastlLap("LapDeltaToSessionLastlLap");    // float[1] Delta time for session last lap (s)
irsdkCVar ir_LapDeltaToSessionLastlLap_DD("LapDeltaToSessionLastlLap_DD");    // float[1] Rate of change of delta time for session last lap (s/s)
irsdkCVar ir_LapDeltaToSessionLastlLap_OK("LapDeltaToSessionLastlLap_OK");    // bool[1] Delta time for session last lap is valid ()
irsdkCVar ir_Speed("Speed");    // float[1] GPS vehicle speed (m/s)
irsdkCVar ir_Yaw("Yaw");    // float[1] Yaw orientation (rad)
irsdkCVar ir_YawNorth("YawNorth");    // float[1] Yaw orientation relative to north (rad)
irsdkCVar ir_Pitch("Pitch");    // float[1] Pitch orientation (rad)
irsdkCVar ir_Roll("Roll");    // float[1] Roll orientation (rad)
irsdkCVar ir_EnterExitReset("EnterExitReset");    // int[1] Indicate action the reset key will take 0 enter 1 exit 2 reset ()
irsdkCVar ir_TrackTemp("TrackTemp");    // float[1] Deprecated  set to TrackTempCrew (C)
irsdkCVar ir_TrackTempCrew("TrackTempCrew");    // float[1] Temperature of track measured by crew around track (C)
irsdkCVar ir_AirTemp("AirTemp");    // float[1] Temperature of air at start/finish line (C)
irsdkCVar ir_WeatherType("WeatherType");    // int[1] Weather type (0=constant  1=dynamic) ()
irsdkCVar ir_Skies("Skies");    // int[1] Skies (0=clear/1=p cloudy/2=m cloudy/3=overcast) ()
irsdkCVar ir_AirDensity("AirDensity");    // float[1] Density of air at start/finish line (kg/m^3)
irsdkCVar ir_AirPressure("AirPressure");    // float[1] Pressure of air at start/finish line (Hg)
irsdkCVar ir_WindVel("WindVel");    // float[1] Wind velocity at start/finish line (m/s)
irsdkCVar ir_WindDir("WindDir");    // float[1] Wind direction at start/finish line (rad)
irsdkCVar ir_RelativeHumidity("RelativeHumidity");    // float[1] Relative Humidity (%)
irsdkCVar ir_FogLevel("FogLevel");    // float[1] Fog level (%)
irsdkCVar ir_DCLapStatus("DCLapStatus");    // int[1] Status of driver change lap requirements ()
irsdkCVar ir_DCDriversSoFar("DCDriversSoFar");    // int[1] Number of team drivers who have run a stint ()
irsdkCVar ir_OkToReloadTextures("OkToReloadTextures");    // bool[1] True if it is ok to reload car textures at this time ()
irsdkCVar ir_LoadNumTextures("LoadNumTextures");    // bool[1] True if the car_num texture will be loaded ()
irsdkCVar ir_CarLeftRight("CarLeftRight");    // bitfield[1] Notify if car is to the left or right of driver (irsdk_CarLeftRight)
irsdkCVar ir_PitsOpen("PitsOpen");    // bool[1] True if pit stop is allowed for the current player ()
irsdkCVar ir_VidCapEnabled("VidCapEnabled");    // bool[1] True if video capture system is enabled ()
irsdkCVar ir_VidCapActive("VidCapActive");    // bool[1] True if video currently being captured ()
irsdkCVar ir_PitRepairLeft("PitRepairLeft");    // float[1] Time left for mandatory pit repairs if repairs are active (s)
irsdkCVar ir_PitOptRepairLeft("PitOptRepairLeft");    // float[1] Time left for optional repairs if repairs are active (s)
irsdkCVar ir_PitstopActive("PitstopActive");    // bool[1] Is the player getting pit stop service ()
irsdkCVar ir_FastRepairUsed("FastRepairUsed");    // int[1] How many fast repairs used so far ()
irsdkCVar ir_FastRepairAvailable("FastRepairAvailable");    // int[1] How many fast repairs left  255 is unlimited ()
irsdkCVar ir_LFTiresUsed("LFTiresUsed");    // int[1] How many left front tires used so far ()
irsdkCVar ir_RFTiresUsed("RFTiresUsed");    // int[1] How many right front tires used so far ()
irsdkCVar ir_LRTiresUsed("LRTiresUsed");    // int[1] How many left rear tires used so far ()
irsdkCVar ir_RRTiresUsed("RRTiresUsed");    // int[1] How many right rear tires used so far ()
irsdkCVar ir_LeftTireSetsUsed("LeftTireSetsUsed");    // int[1] How many left tire sets used so far ()
irsdkCVar ir_RightTireSetsUsed("RightTireSetsUsed");    // int[1] How many right tire sets used so far ()
irsdkCVar ir_FrontTireSetsUsed("FrontTireSetsUsed");    // int[1] How many front tire sets used so far ()
irsdkCVar ir_RearTireSetsUsed("RearTireSetsUsed");    // int[1] How many rear tire sets used so far ()
irsdkCVar ir_TireSetsUsed("TireSetsUsed");    // int[1] How many tire sets used so far ()
irsdkCVar ir_LFTiresAvailable("LFTiresAvailable");    // int[1] How many left front tires are remaining  255 is unlimited ()
irsdkCVar ir_RFTiresAvailable("RFTiresAvailable");    // int[1] How many right front tires are remaining  255 is unlimited ()
irsdkCVar ir_LRTiresAvailable("LRTiresAvailable");    // int[1] How many left rear tires are remaining  255 is unlimited ()
irsdkCVar ir_RRTiresAvailable("RRTiresAvailable");    // int[1] How many right rear tires are remaining  255 is unlimited ()
irsdkCVar ir_LeftTireSetsAvailable("LeftTireSetsAvailable");    // int[1] How many left tire sets are remaining  255 is unlimited ()
irsdkCVar ir_RightTireSetsAvailable("RightTireSetsAvailable");    // int[1] How many right tire sets are remaining  255 is unlimited ()
irsdkCVar ir_FrontTireSetsAvailable("FrontTireSetsAvailable");    // int[1] How many front tire sets are remaining  255 is unlimited ()
irsdkCVar ir_RearTireSetsAvailable("RearTireSetsAvailable");    // int[1] How many rear tire sets are remaining  255 is unlimited ()
irsdkCVar ir_TireSetsAvailable("TireSetsAvailable");    // int[1] How many tire sets are remaining  255 is unlimited ()
irsdkCVar ir_CamCarIdx("CamCarIdx");    // int[1] Active camera's focus car index ()
irsdkCVar ir_CamCameraNumber("CamCameraNumber");    // int[1] Active camera number ()
irsdkCVar ir_CamGroupNumber("CamGroupNumber");    // int[1] Active camera group number ()
irsdkCVar ir_CamCameraState("CamCameraState");    // bitfield[1] State of camera system (irsdk_CameraState)
irsdkCVar ir_IsOnTrackCar("IsOnTrackCar");    // bool[1] 1=Car on track physics running ()
irsdkCVar ir_IsInGarage("IsInGarage");    // bool[1] 1=Car in garage physics running ()
irsdkCVar ir_SteeringWheelPctTorque("SteeringWheelPctTorque");    // float[1] Force feedback % max torque on steering shaft unsigned (%)
irsdkCVar ir_SteeringWheelPctTorqueSign("SteeringWheelPctTorqueSign");    // float[1] Force feedback % max torque on steering shaft signed (%)
irsdkCVar ir_SteeringWheelPctTorqueSignStops("SteeringWheelPctTorqueSignStops");    // float[1] Force feedback % max torque on steering shaft signed stops (%)
irsdkCVar ir_SteeringWheelPctDamper("SteeringWheelPctDamper");    // float[1] Force feedback % max damping (%)
irsdkCVar ir_SteeringWheelAngleMax("SteeringWheelAngleMax");    // float[1] Steering wheel max angle (rad)
irsdkCVar ir_SteeringWheelLimiter("SteeringWheelLimiter");    // float[1] Force feedback limiter strength limits impacts and oscillation (%)
irsdkCVar ir_ShiftIndicatorPct("ShiftIndicatorPct");    // float[1] DEPRECATED use DriverCarSLBlinkRPM instead (%)
irsdkCVar ir_ShiftPowerPct("ShiftPowerPct");    // float[1] Friction torque applied to gears when shifting or grinding (%)
irsdkCVar ir_ShiftGrindRPM("ShiftGrindRPM");    // float[1] RPM of shifter grinding noise (RPM)
irsdkCVar ir_ThrottleRaw("ThrottleRaw");    // float[1] Raw throttle input 0=off throttle to 1=full throttle (%)
irsdkCVar ir_BrakeRaw("BrakeRaw");    // float[1] Raw brake input 0=brake released to 1=max pedal force (%)
irsdkCVar ir_HandbrakeRaw("HandbrakeRaw");    // float[1] Raw handbrake input 0=handbrake released to 1=max force (%)
irsdkCVar ir_SteeringWheelPeakForceNm("SteeringWheelPeakForceNm");    // float[1] Peak torque mapping to direct input units for FFB (N*m)
irsdkCVar ir_SteeringWheelMaxForceNm("SteeringWheelMaxForceNm");    // float[1] Value of strength or max force slider in Nm for FFB (N*m)
irsdkCVar ir_SteeringWheelUseLinear("SteeringWheelUseLinear");    // bool[1] True if steering wheel force is using linear mode ()
irsdkCVar ir_BrakeABSactive("BrakeABSactive");    // bool[1] true if abs is currently reducing brake force pressure ()
irsdkCVar ir_EngineWarnings("EngineWarnings");    // bitfield[1] Bitfield for warning lights (irsdk_EngineWarnings)
irsdkCVar ir_FuelLevel("FuelLevel");    // float[1] Liters of fuel remaining (l)
irsdkCVar ir_FuelLevelPct("FuelLevelPct");    // float[1] Percent fuel remaining (%)
irsdkCVar ir_PitSvFlags("PitSvFlags");    // bitfield[1] Bitfield of pit service checkboxes (irsdk_PitSvFlags)
irsdkCVar ir_PitSvLFP("PitSvLFP");    // float[1] Pit service left front tire pressure (kPa)
irsdkCVar ir_PitSvRFP("PitSvRFP");    // float[1] Pit service right front tire pressure (kPa)
irsdkCVar ir_PitSvLRP("PitSvLRP");    // float[1] Pit service left rear tire pressure (kPa)
irsdkCVar ir_PitSvRRP("PitSvRRP");    // float[1] Pit service right rear tire pressure (kPa)
irsdkCVar ir_PitSvFuel("PitSvFuel");    // float[1] Pit service fuel add amount (l)
irsdkCVar ir_PitSvTireCompound("PitSvTireCompound");    // int[1] Pit service pending tire compound ()
irsdkCVar ir_CarIdxP2P_Status("CarIdxP2P_Status");    // bool[64] Push2Pass active or not ()
irsdkCVar ir_CarIdxP2P_Count("CarIdxP2P_Count");    // int[64] Push2Pass count of usage (or remaining in Race) ()
irsdkCVar ir_ReplayPlaySpeed("ReplayPlaySpeed");    // int[1] Replay playback speed ()
irsdkCVar ir_ReplayPlaySlowMotion("ReplayPlaySlowMotion");    // bool[1] 0=not slow motion  1=replay is in slow motion ()
irsdkCVar ir_ReplaySessionTime("ReplaySessionTime");    // double[1] Seconds since replay session start (s)
irsdkCVar ir_ReplaySessionNum("ReplaySessionNum");    // int[1] Replay session number ()
irsdkCVar ir_TireLF_RumblePitch("TireLF_RumblePitch");    // float[1] Players LF Tire Sound rumblestrip pitch (Hz)
irsdkCVar ir_TireRF_RumblePitch("TireRF_RumblePitch");    // float[1] Players RF Tire Sound rumblestrip pitch (Hz)
irsdkCVar ir_TireLR_RumblePitch("TireLR_RumblePitch");    // float[1] Players LR Tire Sound rumblestrip pitch (Hz)
irsdkCVar ir_TireRR_RumblePitch("TireRR_RumblePitch");    // float[1] Players RR Tire Sound rumblestrip pitch (Hz)
irsdkCVar ir_SteeringWheelTorque_ST("SteeringWheelTorque_ST");    // float[6] Output torque on steering shaft at 360 Hz (N*m)
irsdkCVar ir_SteeringWheelTorque("SteeringWheelTorque");    // float[1] Output torque on steering shaft (N*m)
irsdkCVar ir_VelocityZ_ST("VelocityZ_ST");    // float[6] Z velocity (m/s at 360 Hz)
irsdkCVar ir_VelocityY_ST("VelocityY_ST");    // float[6] Y velocity (m/s at 360 Hz)
irsdkCVar ir_VelocityX_ST("VelocityX_ST");    // float[6] X velocity (m/s at 360 Hz)
irsdkCVar ir_VelocityZ("VelocityZ");    // float[1] Z velocity (m/s)
irsdkCVar ir_VelocityY("VelocityY");    // float[1] Y velocity (m/s)
irsdkCVar ir_VelocityX("VelocityX");    // float[1] X velocity (m/s)
irsdkCVar ir_YawRate_ST("YawRate_ST");    // float[6] Yaw rate at 360 Hz (rad/s)
irsdkCVar ir_PitchRate_ST("PitchRate_ST");    // float[6] Pitch rate at 360 Hz (rad/s)
irsdkCVar ir_RollRate_ST("RollRate_ST");    // float[6] Roll rate at 360 Hz (rad/s)
irsdkCVar ir_YawRate("YawRate");    // float[1] Yaw rate (rad/s)
irsdkCVar ir_PitchRate("PitchRate");    // float[1] Pitch rate (rad/s)
irsdkCVar ir_RollRate("RollRate");    // float[1] Roll rate (rad/s)
irsdkCVar ir_VertAccel_ST("VertAccel_ST");    // float[6] Vertical acceleration (including gravity) at 360 Hz (m/s^2)
irsdkCVar ir_LatAccel_ST("LatAccel_ST");    // float[6] Lateral acceleration (including gravity) at 360 Hz (m/s^2)
irsdkCVar ir_LongAccel_ST("LongAccel_ST");    // float[6] Longitudinal acceleration (including gravity) at 360 Hz (m/s^2)
irsdkCVar ir_VertAccel("VertAccel");    // float[1] Vertical acceleration (including gravity) (m/s^2)
irsdkCVar ir_LatAccel("LatAccel");    // float[1] Lateral acceleration (including gravity) (m/s^2)
irsdkCVar ir_LongAccel("LongAccel");    // float[1] Longitudinal acceleration (including gravity) (m/s^2)
irsdkCVar ir_dcStarter("dcStarter");    // bool[1] In car trigger car starter ()
irsdkCVar ir_dpRTireChange("dpRTireChange");    // float[1] Pitstop right tire change request ()
irsdkCVar ir_dpLTireChange("dpLTireChange");    // float[1] Pitstop left tire change request ()
irsdkCVar ir_dpFuelFill("dpFuelFill");    // float[1] Pitstop fuel fill flag ()
irsdkCVar ir_dpWindshieldTearoff("dpWindshieldTearoff");    // float[1] Pitstop windshield tearoff ()
irsdkCVar ir_dpFuelAddKg("dpFuelAddKg");    // float[1] Pitstop fuel add ammount (kg)
irsdkCVar ir_dpFastRepair("dpFastRepair");    // float[1] Pitstop fast repair set ()
irsdkCVar ir_dcBrakeBias("dcBrakeBias");    // float[1] In car brake bias adjustment ()
irsdkCVar ir_dpLFTireColdPress("dpLFTireColdPress");    // float[1] Pitstop lf tire cold pressure adjustment (Pa)
irsdkCVar ir_dpRFTireColdPress("dpRFTireColdPress");    // float[1] Pitstop rf cold tire pressure adjustment (Pa)
irsdkCVar ir_dpLRTireColdPress("dpLRTireColdPress");    // float[1] Pitstop lr tire cold pressure adjustment (Pa)
irsdkCVar ir_dpRRTireColdPress("dpRRTireColdPress");    // float[1] Pitstop rr cold tire pressure adjustment (Pa)
irsdkCVar ir_dpWeightJackerLeft("dpWeightJackerLeft");    // float[1] Pitstop left wedge/weight jacker adjustment ()
irsdkCVar ir_dpWeightJackerRight("dpWeightJackerRight");    // float[1] Pitstop right wedge/weight jacker adjustment ()
irsdkCVar ir_WaterTemp("WaterTemp");    // float[1] Engine coolant temp (C)
irsdkCVar ir_WaterLevel("WaterLevel");    // float[1] Engine coolant level (l)
irsdkCVar ir_FuelPress("FuelPress");    // float[1] Engine fuel pressure (bar)
irsdkCVar ir_FuelUsePerHour("FuelUsePerHour");    // float[1] Engine fuel used instantaneous (kg/h)
irsdkCVar ir_OilTemp("OilTemp");    // float[1] Engine oil temperature (C)
irsdkCVar ir_OilPress("OilPress");    // float[1] Engine oil pressure (bar)
irsdkCVar ir_OilLevel("OilLevel");    // float[1] Engine oil level (l)
irsdkCVar ir_Voltage("Voltage");    // float[1] Engine voltage (V)
irsdkCVar ir_ManifoldPress("ManifoldPress");    // float[1] Engine manifold pressure (bar)
irsdkCVar ir_RFcoldPressure("RFcoldPressure");    // float[1] RF tire cold pressure  as set in the garage (kPa)
irsdkCVar ir_RFtempCL("RFtempCL");    // float[1] RF tire left carcass temperature (C)
irsdkCVar ir_RFtempCM("RFtempCM");    // float[1] RF tire middle carcass temperature (C)
irsdkCVar ir_RFtempCR("RFtempCR");    // float[1] RF tire right carcass temperature (C)
irsdkCVar ir_RFwearL("RFwearL");    // float[1] RF tire left percent tread remaining (%)
irsdkCVar ir_RFwearM("RFwearM");    // float[1] RF tire middle percent tread remaining (%)
irsdkCVar ir_RFwearR("RFwearR");    // float[1] RF tire right percent tread remaining (%)
irsdkCVar ir_LFcoldPressure("LFcoldPressure");    // float[1] LF tire cold pressure  as set in the garage (kPa)
irsdkCVar ir_LFtempCL("LFtempCL");    // float[1] LF tire left carcass temperature (C)
irsdkCVar ir_LFtempCM("LFtempCM");    // float[1] LF tire middle carcass temperature (C)
irsdkCVar ir_LFtempCR("LFtempCR");    // float[1] LF tire right carcass temperature (C)
irsdkCVar ir_LFwearL("LFwearL");    // float[1] LF tire left percent tread remaining (%)
irsdkCVar ir_LFwearM("LFwearM");    // float[1] LF tire middle percent tread remaining (%)
irsdkCVar ir_LFwearR("LFwearR");    // float[1] LF tire right percent tread remaining (%)
irsdkCVar ir_RRcoldPressure("RRcoldPressure");    // float[1] RR tire cold pressure  as set in the garage (kPa)
irsdkCVar ir_RRtempCL("RRtempCL");    // float[1] RR tire left carcass temperature (C)
irsdkCVar ir_RRtempCM("RRtempCM");    // float[1] RR tire middle carcass temperature (C)
irsdkCVar ir_RRtempCR("RRtempCR");    // float[1] RR tire right carcass temperature (C)
irsdkCVar ir_RRwearL("RRwearL");    // float[1] RR tire left percent tread remaining (%)
irsdkCVar ir_RRwearM("RRwearM");    // float[1] RR tire middle percent tread remaining (%)
irsdkCVar ir_RRwearR("RRwearR");    // float[1] RR tire right percent tread remaining (%)
irsdkCVar ir_LRcoldPressure("LRcoldPressure");    // float[1] LR tire cold pressure  as set in the garage (kPa)
irsdkCVar ir_LRtempCL("LRtempCL");    // float[1] LR tire left carcass temperature (C)
irsdkCVar ir_LRtempCM("LRtempCM");    // float[1] LR tire middle carcass temperature (C)
irsdkCVar ir_LRtempCR("LRtempCR");    // float[1] LR tire right carcass temperature (C)
irsdkCVar ir_LRwearL("LRwearL");    // float[1] LR tire left percent tread remaining (%)
irsdkCVar ir_LRwearM("LRwearM");    // float[1] LR tire middle percent tread remaining (%)
irsdkCVar ir_LRwearR("LRwearR");    // float[1] LR tire right percent tread remaining (%)
irsdkCVar ir_RRSHshockDefl("RRSHshockDefl");    // float[1] RRSH shock deflection (m)
irsdkCVar ir_RRSHshockDefl_ST("RRSHshockDefl_ST");    // float[6] RRSH shock deflection at 360 Hz (m)
irsdkCVar ir_RRSHshockVel("RRSHshockVel");    // float[1] RRSH shock velocity (m/s)
irsdkCVar ir_RRSHshockVel_ST("RRSHshockVel_ST");    // float[6] RRSH shock velocity at 360 Hz (m/s)
irsdkCVar ir_LRSHshockDefl("LRSHshockDefl");    // float[1] LRSH shock deflection (m)
irsdkCVar ir_LRSHshockDefl_ST("LRSHshockDefl_ST");    // float[6] LRSH shock deflection at 360 Hz (m)
irsdkCVar ir_LRSHshockVel("LRSHshockVel");    // float[1] LRSH shock velocity (m/s)
irsdkCVar ir_LRSHshockVel_ST("LRSHshockVel_ST");    // float[6] LRSH shock velocity at 360 Hz (m/s)
irsdkCVar ir_RFSHshockDefl("RFSHshockDefl");    // float[1] RFSH shock deflection (m)
irsdkCVar ir_RFSHshockDefl_ST("RFSHshockDefl_ST");    // float[6] RFSH shock deflection at 360 Hz (m)
irsdkCVar ir_RFSHshockVel("RFSHshockVel");    // float[1] RFSH shock velocity (m/s)
irsdkCVar ir_RFSHshockVel_ST("RFSHshockVel_ST");    // float[6] RFSH shock velocity at 360 Hz (m/s)
irsdkCVar ir_LFSHshockDefl("LFSHshockDefl");    // float[1] LFSH shock deflection (m)
irsdkCVar ir_LFSHshockDefl_ST("LFSHshockDefl_ST");    // float[6] LFSH shock deflection at 360 Hz (m)
irsdkCVar ir_LFSHshockVel("LFSHshockVel");    // float[1] LFSH shock velocity (m/s)
irsdkCVar ir_LFSHshockVel_ST("LFSHshockVel_ST");    // float[6] LFSH shock velocity at 360 Hz (m/s)

Session ir_session;

static bool parseYamlInt(const char *yamlStr, const char *path, int *dest)
{
    int count = 0;
    const char *s = nullptr;

    if( parseYaml(yamlStr, path, &s, &count) )
    {
        *dest = atoi( s );
        return true;
    }

    return false;
}

static bool parseYamlFloat(const char *yamlStr, const char *path, float *dest)
{
    int count = 0;
    const char *s = nullptr;

    if( parseYaml(yamlStr, path, &s, &count) )
    {
        (*dest) = (float)atof( s );
        return true;
    }

    return false;
}

static bool parseYamlStr(const char *yamlStr, const char *path, std::string& dest)
{
    int count = 0;
    const char *s = nullptr;

    if( parseYaml(yamlStr, path, &s, &count) )
    {
        // strip leading quotes
        if( *s == '"' )
        {
            s++;
            count--;
        }

        dest.assign( s, count );

        // strip trailing quotes
        if( !dest.empty() && dest[dest.length()-1]=='"' )
            dest.pop_back();

        return true;
    }

    return false;
}

ConnectionStatus ir_tick()
{
    irsdkClient& irsdk = irsdkClient::instance();

    irsdk.waitForData(16);

    if( !irsdk.isConnected() )
        return ConnectionStatus::DISCONNECTED;

    if( irsdk.wasSessionStrUpdated() )
    {
        const char* sessionYaml = irsdk.getSessionStr();
#ifdef _DEBUG
        //printf("%s\n", sessionYaml);
        FILE* fp = fopen("sessionYaml.txt","ab");
        fprintf(fp,"\n\n==== NEW SESSION STRING ======================================\n");
        fprintf(fp,"%s",sessionYaml);
        fclose(fp);
#endif
        char path[256];

        // Weekend info
        sprintf( path, "WeekendInfo:SubSessionID:" );
        parseYamlInt( sessionYaml, path, &ir_session.subsessionId );

        sprintf( path, "WeekendInfo:WeekendOptions:IsFixedSetup:" );
        parseYamlInt( sessionYaml, path, &ir_session.isFixedSetup );

        // Current session type
        std::string sessionNameStr;
        sprintf( path, "SessionInfo:Sessions:SessionNum:{%d}SessionName:", ir_SessionNum.getInt() );
        parseYamlStr( sessionYaml, path, sessionNameStr );
        if( sessionNameStr == "PRACTICE" )
            ir_session.sessionType = SessionType::PRACTICE;
        if( sessionNameStr == "QUALIFY" )
            ir_session.sessionType = SessionType::QUALIFY;
        else if( sessionNameStr == "RACE" )
            ir_session.sessionType = SessionType::RACE;

        // Driver/car info
        parseYamlInt( sessionYaml, "DriverInfo:DriverCarIdx:", &ir_session.driverCarIdx );
        parseYamlFloat( sessionYaml, "DriverInfo:DriverCarFuelMaxLtr:", &ir_session.fuelMaxLtr );
        parseYamlFloat( sessionYaml, "DriverInfo:DriverCarIdleRPM:", &ir_session.rpmIdle );
        parseYamlFloat( sessionYaml, "DriverInfo:DriverCarRedLine:", &ir_session.rpmRedline );
        parseYamlFloat( sessionYaml, "DriverInfo:DriverCarSLFirstRPM:", &ir_session.rpmSLFirst );
        parseYamlFloat( sessionYaml, "DriverInfo:DriverCarSLShiftRPM:", &ir_session.rpmSLShift );
        parseYamlFloat( sessionYaml, "DriverInfo:DriverCarSLLastRPM:", &ir_session.rpmSLLast );
        parseYamlFloat( sessionYaml, "DriverInfo:DriverCarSLBlinkRPM:", &ir_session.rpmSLBlink );

        // Per-Driver info
        for( int carIdx=0; carIdx<IR_MAX_CARS; ++carIdx )
        {
            Car& car = ir_session.cars[carIdx];

            car.isSelf = int( carIdx==ir_session.driverCarIdx );

            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}UserName:", carIdx );
            if( !parseYamlStr( sessionYaml, path, car.userName ) )
            {
                car = Car();
                continue;
            }

            // Remove line breaks in user names if we find any (saw this happen once)
            for( char& c : car.userName )
                c = (c=='\n'||c=='\r') ? ' ' : c;

            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}CarNumber:", carIdx );
            parseYamlStr( sessionYaml, path, car.carNumberStr );

            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}CarNumberRaw:", carIdx );
            parseYamlInt( sessionYaml, path, &car.carNumber );

            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}LicString:", carIdx );
            parseYamlStr( sessionYaml, path, car.licenseStr );
            car.licenseChar = car.licenseStr.empty() ? 'R' : car.licenseStr[0];
            const std::string SRstr = car.licenseStr.empty() ? "0" : std::string( car.licenseStr.begin()+1, car.licenseStr.end() );
            car.licenseSR = (float)atof( SRstr.c_str() );

            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}LicColor:", carIdx );
            parseYamlStr( sessionYaml, path, car.licenseColStr );
            unsigned licColHex = 0;
            sscanf( car.licenseColStr.c_str(), "0x%x", &licColHex );
            car.licenseCol.r = float((licColHex >> 16) & 0xff) / 255.f;
            car.licenseCol.g = float((licColHex >>  8) & 0xff) / 255.f;
            car.licenseCol.b = float((licColHex >>  0) & 0xff) / 255.f;
            car.licenseCol.a = 1;

            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}IRating:", carIdx );
            parseYamlInt( sessionYaml, path, &car.irating );

            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}CarIsPaceCar:", carIdx );
            parseYamlInt( sessionYaml, path, &car.isPaceCar );

            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}IsSpectator:", carIdx );
            parseYamlInt( sessionYaml, path, &car.isSpectator );

            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}CurDriverIncidentCount:", carIdx );
            parseYamlInt( sessionYaml, path, &car.incidentCount );

            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}CarClassEstLapTime:", carIdx );
            parseYamlFloat( sessionYaml, path, &car.carClassEstLapTime );

            car.practicePosition = 0;
            car.qualPosition = 0;
            car.racePosition = 0;
        }

        // Qualifying results info
        for( int pos=0; pos<IR_MAX_CARS; ++pos )
        {
            sprintf( path, "QualifyResultsInfo:Results:Position:{%d}CarIdx:", pos );
            int carIdx = -1;
            if( parseYamlInt( sessionYaml, path, &carIdx ) ) {
                ir_session.cars[carIdx].qualPosition = pos + 1;

                sprintf( path, "QualifyResultsInfo:Results:Position:{%d}FastestTime:", pos );
                parseYamlFloat( sessionYaml, path, &ir_session.cars[carIdx].qualTime );
            }
        }

        // Session info (may override qual results from above, but that's ok since hopefully they're the same!)
        for( int session=0; ; ++session )
        {
            std::string sessionNameStr;
            sprintf( path, "SessionInfo:Sessions:SessionNum:{%d}SessionName:", session );
            if( !parseYamlStr( sessionYaml, path, sessionNameStr ) )
                break;

            std::string str;
            sprintf( path, "SessionInfo:Sessions:SessionNum:{%d}SessionTime:", session );
            parseYamlStr( sessionYaml, path, str );
            ir_session.isUnlimitedTime = int( str=="unlimited" );

            sprintf( path, "SessionInfo:Sessions:SessionNum:{%d}SessionLaps:", session );
            parseYamlStr( sessionYaml, path, str );
            ir_session.isUnlimitedLaps = int( str=="unlimited" );

            for( int pos=1; pos<IR_MAX_CARS+1; ++pos )
            {
                int carIdx = -1;
                sprintf( path, "SessionInfo:Sessions:SessionNum:{%d}ResultsPositions:Position:{%d}CarIdx:", session, pos );
                if( parseYamlInt( sessionYaml, path, &carIdx ) )
                {
                    if( sessionNameStr == "PRACTICE" )
                        ir_session.cars[carIdx].practicePosition = pos;
                    else if( sessionNameStr == "QUALIFY" )
                        ir_session.cars[carIdx].qualPosition = pos;
                    else if( sessionNameStr == "RACE" )
                        ir_session.cars[carIdx].racePosition = pos;
                }
            }
        }

        // SoF
        double sof = 0;
        int cnt = 0;
        for( int i=0; i<IR_MAX_CARS; ++i )
        {
            const Car& car = ir_session.cars[i];

            if( car.isPaceCar || car.isSpectator || car.userName.empty() )
                continue;

            sof += car.irating;
            cnt++;
        }
        ir_session.sof = int(sof / cnt);

        ir_handleConfigChange();

    } // if session string updated

    // Track cars in pits. Reset every time we're in the 'warmup' phase (just before starting pace laps).
    const bool resetPitAge = ir_SessionState.getInt() == irsdk_StateWarmup;
    for( int carIdx=0; carIdx<IR_MAX_CARS; ++carIdx )
    {
        Car& car = ir_session.cars[carIdx];
        if( resetPitAge )
            car.lastLapInPits = 0;
        if( ir_SessionState.getInt() >= 0 /* work around getting garbage sometimes (?) */ && ir_CarIdxOnPitRoad.getBool(carIdx) )
            car.lastLapInPits = ir_CarIdxLap.getInt(carIdx);
    }

    // Check for both ir_IsOnTrack and ir_IsOnTrackCar, because I've seen iRacing report true for ir_IsOnTrack 
    // (for just a short time) even when we're not in the car in a practice session. Checking both does seem
    // to address that.
    return (ir_IsOnTrack.getBool() && ir_IsOnTrackCar.getBool()) ? ConnectionStatus::DRIVING : ConnectionStatus::CONNECTED;
}

void ir_handleConfigChange()
{
    std::vector<std::string> buddies = g_cfg.getStringVec( "General", "buddies", {} );
    std::vector<std::string> flagged = g_cfg.getStringVec( "General", "flagged", {} );

    for( int carIdx=0; carIdx<IR_MAX_CARS; ++carIdx )
    {
        Car& car = ir_session.cars[carIdx];

        car.isBuddy = 0;
        for( const std::string& name : buddies ) {
            if( name == car.userName )
                car.isBuddy = 1;
        }

        car.isFlagged = 0;
        for( const std::string& name : flagged ) {
            if( name == car.userName )
                car.isFlagged = 1;
        }
    }
}

bool ir_isPreStart()
{
    // To find out whether we're pacing, it isn't enough to check ir_PaceMode, because
    // iRacing doesn't set it to irsdk_PaceModeNotPacing as one would expect. So in addition we check
    // the session state, since initial pacing apparently counts as "parade laps".
    return (ir_PaceMode.getInt() == irsdk_PaceModeSingleFileStart || ir_PaceMode.getInt() == irsdk_PaceModeDoubleFileStart) &&
           (ir_SessionState.getInt() == irsdk_StateParadeLaps || ir_SessionState.getInt() == irsdk_StateWarmup || ir_SessionState.getInt() == irsdk_StateGetInCar);
}

float ir_estimateLaptime()
{
    float best = ir_LapBestLapTime.getFloat();
    if( best > 0 )
        return best;
    return ir_session.cars[ir_session.driverCarIdx].carClassEstLapTime;
}

int ir_getPosition( int carIdx )
{
    // Try the different sources we have for position data, in descending order of importance
    int pos = ir_CarIdxPosition.getInt(carIdx);
    if( pos > 0 )
        return pos;

    pos = ir_session.cars[carIdx].racePosition;
    if( pos > 0 )
        return pos;

    pos = ir_session.cars[carIdx].qualPosition;
    if( pos > 0 )
        return pos;

    pos = ir_session.cars[carIdx].practicePosition;
    if( pos > 0 )
        return pos;

    return 0;
}

int ir_getLapDeltaToLeader( int carIdx, int ldrIdx )
{
    if( ir_session.sessionType!=SessionType::RACE || ir_isPreStart() || carIdx < 0 || ldrIdx < 0 )
        return 0;

    const int carLapCount = std::max( ir_CarIdxLap.getInt(carIdx), ir_CarIdxLapCompleted.getInt(carIdx) );
    const int ldrLapCount = std::max( ir_CarIdxLap.getInt(ldrIdx), ir_CarIdxLapCompleted.getInt(ldrIdx) );

   // if( carLapCount < 0 )
     //   return ir_CarIdxLapCompleted.getInt(carIdx) - ir_CarIdxLapCompleted.getInt(ldrIdx);

    const float carPctAroundLap = ir_CarIdxLapDistPct.getFloat( carIdx );
    const float ldrPctAroundLap = ir_CarIdxLapDistPct.getFloat( ldrIdx );

    if( carPctAroundLap < 0 || ldrPctAroundLap < 0 )
        return 0;

    int lapDelta = carLapCount - ldrLapCount;

    if( carPctAroundLap > ldrPctAroundLap )
        lapDelta += 1;

    return lapDelta;
}

void ir_printVariables()
{
    if( !irsdk_isConnected() )
        return;

    const irsdk_header* header = irsdk_getHeader();

    printf("IRSDK Variables:\n");
    for( int i=0; i<header->numVars; ++i )
    {
        const irsdk_varHeader* var = irsdk_getVarHeaderEntry(i);
        std::string type;
        switch( var->type )
        {
        case irsdk_char: type="char"; break;
        case irsdk_bool: type="bool"; break;
        case irsdk_int: type="int"; break;
        case irsdk_bitField: type="bitfield"; break;
        case irsdk_float: type="float"; break;
        case irsdk_double: type="double"; break;
        }
        printf( "irsdkCVar ir_%s(\"%s\");    // %s[%d] %s (%s)\n",
            var->name, var->name, type.c_str(), var->count, var->desc, var->unit );
    }
}
