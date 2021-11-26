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

#include "iracing.h"

                                                                                 //     Unit, Type, Disk, Live, Description
irsdkCVar ir_AirDensity("AirDensity");                                           //     kg/m^3     float     1    1    Density of air at start/finish line 
irsdkCVar ir_AirPressure("AirPressure");                                         //     Hg     float     1    1    Pressure of air at start/finish line 
irsdkCVar ir_AirTemp("AirTemp");                                                 //         float     1    1    Temperature of air at start/finish line 
irsdkCVar ir_Alt("Alt");                                                         //     m     float     1    1    Altitude in meters 
irsdkCVar ir_Brake("Brake");                                                     //     %     float     1    1    0=brake released to 1=max pedal force 
irsdkCVar ir_BrakeRaw("BrakeRaw");                                               //     %     float     1    1    Raw brake input 0=brake released to 1=max pedal force 
irsdkCVar ir_CamCameraNumber("CamCameraNumber");                                 //           int     0    1    Active camera number 
irsdkCVar ir_CamCameraState("CamCameraState");                                   //     irsdk_CameraState     bitfield     0    1    State of camera system 
irsdkCVar ir_CamCarIdx("CamCarIdx");                                             //           int     0    1    Active camera's focus car index 
irsdkCVar ir_CamGroupNumber("CamGroupNumber");                                   //           int     0    1    Active camera group number 
irsdkCVar ir_Clutch("Clutch");                                                   //     %     float     1    1    0=disengaged to 1=fully engaged 
irsdkCVar ir_CpuUsageBG("CpuUsageBG");                                           //     %     float     1    1    Percent of available tim bg thread took with a 1 sec avg
irsdkCVar ir_DCDriversSoFar("DCDriversSoFar");                                   //           int     0    1    Number of team drivers who have run a stint
irsdkCVar ir_DCLapStatus("DCLapStatus");                                         //           int     0    1    Status of driver change lap requirements
irsdkCVar ir_DisplayUnits("DisplayUnits");                                       //           int     0    1    Default units for the user interface 0 = english 1 = metric
irsdkCVar ir_DriverMarker("DriverMarker");                                       //           bool     1    1    Driver activated flag
irsdkCVar ir_EngineWarnings("EngineWarnings");                                   //     irsdk_EngineWarnings     bitfield     1    1    Bitfield for warning lights
irsdkCVar ir_EnterExitReset("EnterExitReset");                                   //           int     1    1    Indicate action the reset key will take 0 enter 1 exit 2 reset
irsdkCVar ir_FogLevel("FogLevel");                                               //     %     float     1    1    Fog level
irsdkCVar ir_FrameRate("FrameRate");                                             //     fps     float     1    1    Average frames per second
irsdkCVar ir_FuelLevel("FuelLevel");                                             //     l     float     1    1    Liters of fuel remaining
irsdkCVar ir_FuelLevelPct("FuelLevelPct");                                       //     %     float     1    1    Percent fuel remaining
irsdkCVar ir_FuelPress("FuelPress");                                             //     bar     float     1    1    Engine fuel pressure
irsdkCVar ir_FuelUsePerHour("FuelUsePerHour");                                   //     kg/h     float     1    1    Engine fuel used instantaneous
irsdkCVar ir_Gear("Gear");                                                       //         int     1    1    -1=reverse  0=neutral  1..n=current gear
irsdkCVar ir_IsDiskLoggingActive("IsDiskLoggingActive");                         //         bool     0    1    0=disk based telemetry file not being written  1=being written
irsdkCVar ir_IsDiskLoggingEnabled("IsDiskLoggingEnabled");                       //         bool     0    1    0=disk based telemetry turned off  1=turned on
irsdkCVar ir_IsInGarage("IsInGarage");                                           //         bool     0    1    1=Car in garage physics running
irsdkCVar ir_IsOnTrack("IsOnTrack");                                             //         bool     1    1    1=Car on track physics running with player in car
irsdkCVar ir_IsOnTrackCar("IsOnTrackCar");                                       //         bool     1    1    1=Car on track physics running
irsdkCVar ir_IsReplayPlaying("IsReplayPlaying");                                 //         bool     0    1    0=replay not playing  1=replay playing
irsdkCVar ir_Lap("Lap");                                                         //         int     1    1    Lap count
irsdkCVar ir_LapBestLap("LapBestLap");                                           //         int     1    1    Players best lap number
irsdkCVar ir_LapBestLapTime("LapBestLapTime");                                   //     s     float     1    1    Players best lap time
irsdkCVar ir_LapBestNLapLap("LapBestNLapLap");                                   //         int     1    1    Player last lap in best N average lap time
irsdkCVar ir_LapBestNLapTime("LapBestNLapTime");                                 //     s     float     1    1    Player best N average lap time
irsdkCVar ir_LapCurrentLapTime("LapCurrentLapTime");                             //     s     float     1    1    Estimate of players current lap time as shown in F3 box
irsdkCVar ir_LapDeltaToBestLap("LapDeltaToBestLap");                             //     s     float     1    1    Delta time for best lap
irsdkCVar ir_LapDeltaToBestLap_DD("LapDeltaToBestLap_DD");                       //     s/s     float     1    1    Rate of change of delta time for best lap 
irsdkCVar ir_LapDeltaToBestLap_OK("LapDeltaToBestLap_OK");                       //         bool     1    1    Delta time for best lap is valid 
irsdkCVar ir_LapDeltaToOptimalLap("LapDeltaToOptimalLap");                       //     s     float     1    1    Delta time for optimal lap 
irsdkCVar ir_LapDeltaToOptimalLap_DD("LapDeltaToOptimalLap_DD");                 //     s/s     float     1    1    Rate of change of delta time for optimal lap 
irsdkCVar ir_LapDeltaToOptimalLap_OK("LapDeltaToOptimalLap_OK");                 //         bool     1    1    Delta time for optimal lap is valid 
irsdkCVar ir_LapDeltaToSessionBestLap("LapDeltaToSessionBestLap");               //     s     float     1    1    Delta time for session best lap 
irsdkCVar ir_LapDeltaToSessionBestLap_DD("LapDeltaToSessionBestLap_DD");         //     s/s     float     1    1    Rate of change of delta time for session best lap 
irsdkCVar ir_LapDeltaToSessionBestLap_OK("LapDeltaToSessionBestLap_OK");         //         bool     1    1    Delta time for session best lap is valid 
irsdkCVar ir_LapDeltaToSessionLastlLap("LapDeltaToSessionLastlLap");             //     s     float     1    1    Delta time for session last lap 
irsdkCVar ir_LapDeltaToSessionLastlLap_DD("LapDeltaToSessionLastlLap_DD");       //     s/s     float     1    1    Rate of change of delta time for session last lap 
irsdkCVar ir_LapDeltaToSessionLastlLap_OK("LapDeltaToSessionLastlLap_OK");       //         bool     1    1    Delta time for session last lap is valid 
irsdkCVar ir_LapDeltaToSessionOptimalLap("LapDeltaToSessionOptimalLap");         //     s     float     1    1    Delta time for session optimal lap 
irsdkCVar ir_LapDeltaToSessionOptimalLap_DD("LapDeltaToSessionOptimalLap_DD");   //     s/s     float     1    1    Rate of change of delta time for session optimal lap 
irsdkCVar ir_LapDeltaToSessionOptimalLap_OK("LapDeltaToSessionOptimalLap_OK");   //         bool     1    1    Delta time for session optimal lap is valid 
irsdkCVar ir_LapDist("LapDist");                                                 //     m     float     1    1    Meters traveled from S/F this lap 
irsdkCVar ir_LapDistPct("LapDistPct");                                           //     %     float     1    1    Percentage distance around lap 
irsdkCVar ir_LapLasNLapSeq("LapLasNLapSeq");                                     //         int     1    1    Player num consecutive clean laps completed for N average 
irsdkCVar ir_LapLastLapTime("LapLastLapTime");                                   //     s     float     1    1    Players last lap time 
irsdkCVar ir_LapLastNLapTime("LapLastNLapTime");                                 //     s     float     1    1    Player last N average lap time 
irsdkCVar ir_Lat("Lat");                                                         //     deg     double     1    1    Latitude in decimal degrees 
irsdkCVar ir_LatAccel("LatAccel");                                               //     m/s^2     float     1    1    Lateral acceleration (including gravity) 
irsdkCVar ir_Lon("Lon");                                                         //     deg     double     1    1    Longitude in decimal degrees 
irsdkCVar ir_LongAccel("LongAccel");                                             //     m/s^2     float     1    1    Longitudinal acceleration (including gravity) 
irsdkCVar ir_ManifoldPress("ManifoldPress");                                     //     bar     float     1    1    Engine manifold pressure 
irsdkCVar ir_OilLevel("OilLevel");                                               //     l     float     1    1    Engine oil level 
irsdkCVar ir_OilPress("OilPress");                                               //     bar     float     1    1    Engine oil pressure 
irsdkCVar ir_OilTemp("OilTemp");                                                 //     C     float     1    1    Engine oil temperature 
irsdkCVar ir_OnPitRoad("OnPitRoad");                                             //         bool     1    1    Is the player car on pit road between the cones 
irsdkCVar ir_Pitch("Pitch");                                                     //     rad     float     1    1    Pitch orientation 
irsdkCVar ir_PitchRate("PitchRate");                                             //     rad/s     float     1    1    Pitch rate 
irsdkCVar ir_PitOptRepairLeft("PitOptRepairLeft");                               //     s     float     1    1    Time left for optional repairs if repairs are active 
irsdkCVar ir_PitRepairLeft("PitRepairLeft");                                     //     s     float     1    1    Time left for mandatory pit repairs if repairs are active 
irsdkCVar ir_PitSvFlags("PitSvFlags");                                           //     irsdk_PitSvFlags     bitfield     1    1    Bitfield of pit service checkboxes 
irsdkCVar ir_PitSvFuel("PitSvFuel");                                             //     l     float     1    1    Pit service fuel add amount 
irsdkCVar ir_PitSvLFP("PitSvLFP");                                               //     kPa     float     1    1    Pit service left front tire pressure 
irsdkCVar ir_PitSvLRP("PitSvLRP");                                               //     kPa     float     1    1    Pit service left rear tire pressure 
irsdkCVar ir_PitSvRFP("PitSvRFP");                                               //     kPa     float     1    1    Pit service right front tire pressure 
irsdkCVar ir_PitSvRRP("PitSvRRP");                                               //     kPa     float     1    1    Pit service right rear tire pressure 
irsdkCVar ir_PlayerCarClassPosition("PlayerCarClassPosition");                   //         int     1    1    Players class position in race 
irsdkCVar ir_PlayerCarPosition("PlayerCarPosition");                             //         int     1    1    Players position in race 
irsdkCVar ir_RaceLaps("RaceLaps");                                               //         int     0    1    Laps completed in race 
irsdkCVar ir_RadioTransmitCarIdx("RadioTransmitCarIdx");                         //         int     0    1    The car index of the current person speaking on the radio 
irsdkCVar ir_RadioTransmitFrequencyIdx("RadioTransmitFrequencyIdx");             //         int     0    1    The frequency index of the current person speaking on the raio 
irsdkCVar ir_RadioTransmitRadioIdx("RadioTransmitRadioIdx");                     //         int     0    1    The radio index of the current person speaking on the radio 
irsdkCVar ir_RelativeHumidity("RelativeHumidity");                               //     %     float     1    1    Relative Humidity 
irsdkCVar ir_ReplayFrameNum("ReplayFrameNum");                                   //         int     0    1    Integer replay frame number (60 per second) 
irsdkCVar ir_ReplayFrameNumEnd("ReplayFrameNumEnd");                             //         int     0    1    Integer replay frame number from end of tape 
irsdkCVar ir_ReplayPlaySlowMotion("ReplayPlaySlowMotion");                       //         bool     0    1    0=not slow motion  1=replay is in slow motion 
irsdkCVar ir_ReplayPlaySpeed("ReplayPlaySpeed");                                 //         int     0    1    Replay playback speed 
irsdkCVar ir_ReplaySessionNum("ReplaySessionNum");                               //         int     0    1    Replay session number 
irsdkCVar ir_ReplaySessionTime("ReplaySessionTime");                             //     s     double     0    1    Seconds since replay session start 
irsdkCVar ir_Roll("Roll");                                                       //     rad     float     1    1    Roll orientation 
irsdkCVar ir_RollRate("RollRate");                                               //     rad/s     float     1    1    Roll rate 
irsdkCVar ir_RPM("RPM");                                                         //     revs/min     float     1    1    Engine rpm 
irsdkCVar ir_SessionFlags("SessionFlags");                                       //     irsdk_Flags     bitfield     0    1    Session flags 
irsdkCVar ir_SessionLapsRemain("SessionLapsRemain");                             //         int     1    1    Laps left till session ends 
irsdkCVar ir_SessionNum("SessionNum");                                           //         int     1    1    Session number 
irsdkCVar ir_SessionState("SessionState");                                       //     irsdk_SessionState     int     1    1    Session state 
irsdkCVar ir_SessionTime("SessionTime");                                         //     s     double     1    1    Seconds since session start 
irsdkCVar ir_SessionTimeRemain("SessionTimeRemain");                             //     s     double     1    1    Seconds left till session ends 
irsdkCVar ir_SessionUniqueID("SessionUniqueID");                                 //         int     1    1    Session ID 
irsdkCVar ir_ShiftGrindRPM("ShiftGrindRPM");                                     //     RPM     float     1    1    RPM of shifter grinding noise 
irsdkCVar ir_ShiftIndicatorPct("ShiftIndicatorPct");                             //     %     float     1    1    DEPRECATED use DriverCarSLBlinkRPM instead 
irsdkCVar ir_ShiftPowerPct("ShiftPowerPct");                                     //     %     float     1    1    Friction torque applied to gears when shifting or grinding 
irsdkCVar ir_Skies("Skies");                                                     //         int     1    1    Skies (0=clear/1=p cloudy/2=m cloudy/3=overcast) 
irsdkCVar ir_Speed("Speed");                                                     //     m/s     float     1    1    GPS vehicle speed 
irsdkCVar ir_SteeringWheelAngle("SteeringWheelAngle");                           //     rad     float     1    1    Steering wheel angle 
irsdkCVar ir_SteeringWheelAngleMax("SteeringWheelAngleMax");                     //     rad     float     1    1    Steering wheel max angle 
irsdkCVar ir_SteeringWheelPctDamper("SteeringWheelPctDamper");                   //     %     float     1    1    Force feedback % max damping 
irsdkCVar ir_SteeringWheelPctTorque("SteeringWheelPctTorque");                   //     %     float     1    1    Force feedback % max torque on steering shaft unsigned 
irsdkCVar ir_SteeringWheelPctTorqueSign("SteeringWheelPctTorqueSign");           //     %     float     1    1    Force feedback % max torque on steering shaft signed 
irsdkCVar ir_SteeringWheelPctTorqueSignStops("SteeringWheelPctTorqueSignStops"); //     %     float     1    1    Force feedback % max torque on steering shaft signed stops 
irsdkCVar ir_SteeringWheelPeakForceNm("SteeringWheelPeakForceNm");               //     N*m     float     0    1    Peak torque mapping to direct input units for FFB 
irsdkCVar ir_SteeringWheelTorque("SteeringWheelTorque");                         //     N*m     float     1    1    Output torque on steering shaft 
irsdkCVar ir_Throttle("Throttle");                                               //     %     float     1    1    0=off throttle to 1=full throttle 
irsdkCVar ir_ThrottleRaw("ThrottleRaw");                                         //     %     float     1    1    Raw throttle input 0=off throttle to 1=full throttle 
irsdkCVar ir_TrackTemp("TrackTemp");                                             //     C     float     1    1    Temperature of track at start/finish line 
irsdkCVar ir_TrackTempCrew("TrackTempCrew");                                     //     C     float     1    1    Temperature of track measured by crew around track 
irsdkCVar ir_VelocityX("VelocityX");                                             //     m/s     float     1    1    X velocity 
irsdkCVar ir_VelocityY("VelocityY");                                             //     m/s     float     1    1    Y velocity 
irsdkCVar ir_VelocityZ("VelocityZ");                                             //     m/s     float     1    1    Z velocity 
irsdkCVar ir_VertAccel("VertAccel");                                             //     m/s^2     float     1    1    Vertical acceleration (including gravity) 
irsdkCVar ir_Voltage("Voltage");                                                 //     V     float     1    1    Engine voltage 
irsdkCVar ir_WaterLevel("WaterLevel");                                           //     l     float     1    1    Engine coolant level 
irsdkCVar ir_WaterTemp("WaterTemp");                                             //     C     float     1    1    Engine coolant temp 
irsdkCVar ir_WeatherType("WeatherType");                                         //         int     1    1    Weather type (0=constant  1=dynamic) 
irsdkCVar ir_WindDir("WindDir");                                                 //     rad     float     1    1    Wind direction at start/finish line 
irsdkCVar ir_WindVel("WindVel");                                                 //     m/s     float     1    1    Wind velocity at start/finish line 
irsdkCVar ir_Yaw("Yaw");                                                         //     rad     float     1    1    Yaw orientation 
irsdkCVar ir_YawNorth("YawNorth");                                               //     rad     float     1    1    Yaw orientation relative to north 
irsdkCVar ir_YawRate("YawRate");                                                 //     rad/s     float     1    1    Yaw rate 
irsdkCVar ir_CFrideHeight("CFrideHeight");                                       //     m     float     1    0    CF ride height 
irsdkCVar ir_CFshockDefl("CFshockDefl");                                         //     m     float     1    1    CF shock deflection 
irsdkCVar ir_CFshockVel("CFshockVel");                                           //     m/s     float     1    1    CF shock velocity 
irsdkCVar ir_CFSRrideHeight("CFSRrideHeight");                                   //     m     float     1    0    CFSR ride height 
irsdkCVar ir_CRrideHeight("CRrideHeight");                                       //     m     float     1    0    CR ride height 
irsdkCVar ir_CRshockDefl("CRshockDefl");                                         //     m     float     1    1    CR shock deflection 
irsdkCVar ir_CRshockVel("CRshockVel");                                           //     m/s     float     1    1    CR shock velocity 
irsdkCVar ir_dcABS("dcABS");                                                     //         float     1    1    In car abs adjustment 
irsdkCVar ir_dcAntiRollFront("dcAntiRollFront");                                 //         float     1    1    In car front anti roll bar adjustment 
irsdkCVar ir_dcAntiRollRear("dcAntiRollRear");                                   //         float     1    1    In car rear anti roll bar adjustment 
irsdkCVar ir_dcBoostLevel("dcBoostLevel");                                       //         float     1    1    In car boost level adjustment 
irsdkCVar ir_dcBrakeBias("dcBrakeBias");                                         //         float     1    1    In car brake bias adjustment 
irsdkCVar ir_dcDiffEntry("dcDiffEntry");                                         //         float     1    1    In car diff entry adjustment 
irsdkCVar ir_dcDiffExit("dcDiffExit");                                           //         float     1    1    In car diff exit adjustment 
irsdkCVar ir_dcDiffMiddle("dcDiffMiddle");                                       //         float     1    1    In car diff middle adjustment 
irsdkCVar ir_dcEngineBraking("dcEngineBraking");                                 //         float     1    1    In car engine braking adjustment 
irsdkCVar ir_dcEnginePower("dcEnginePower");                                     //         float     1    1    In car engine power adjustment 
irsdkCVar ir_dcFuelMixture("dcFuelMixture");                                     //         float     1    1    In car fuel mixture adjustment 
irsdkCVar ir_dcRevLimiter("dcRevLimiter");                                       //         float     1    1    In car rev limiter adjustment 
irsdkCVar ir_dcThrottleShape("dcThrottleShape");                                 //         float     1    1    In car throttle shape adjustment 
irsdkCVar ir_dcTractionControl("dcTractionControl");                             //         float     1    1    In car traction control adjustment 
irsdkCVar ir_dcTractionControl2("dcTractionControl2");                           //         float     1    1    In car traction control 2 adjustment 
irsdkCVar ir_dcTractionControlToggle("dcTractionControlToggle");                 //          bool     1    1    In car traction control active 
irsdkCVar ir_dcWeightJackerLeft("dcWeightJackerLeft");                           //         float     1    1    In car left weight jacker adjustment 
irsdkCVar ir_dcWeightJackerRight("dcWeightJackerRight");                         //         float     1    1    In car right weight jacker adjustment 
irsdkCVar ir_dcWingFront("dcWingFront");                                         //         float     1    1    In car front wing adjustment 
irsdkCVar ir_dcWingRear("dcWingRear");                                           //         float     1    1    In car rear wing adjustment 
irsdkCVar ir_dpFNOMKnobSetting("dpFNOMKnobSetting");                             //         float     1    1    Pitstop front flap adjustment 
irsdkCVar ir_dpFUFangleIndex("dpFUFangleIndex");                                 //         float     1    1    Pitstop front upper flap adjustment 
irsdkCVar ir_dpFWingAngle("dpFWingAngle");                                       //         float     1    1    Pitstop front wing adjustment 
irsdkCVar ir_dpFWingIndex("dpFWingIndex");                                       //         float     1    1    Pitstop front wing adjustment 
irsdkCVar ir_dpLrWedgeAdj("dpLrWedgeAdj");                                       //         float     1    1    Pitstop lr spring offset adjustment 
irsdkCVar ir_dpPSSetting("dpPSSetting");                                         //         float     1    1    Pitstop power steering adjustment 
irsdkCVar ir_dpQtape("dpQtape");                                                 //         float     1    1    Pitstop qtape adjustment 
irsdkCVar ir_dpRBarSetting("dpRBarSetting");                                     //         float     1    1    Pitstop rear bar adjustment 
irsdkCVar ir_dpRFTruckarmP1Dz("dpRFTruckarmP1Dz");                               //         float     1    1    Pitstop rftruckarmP1Dz adjustment 
irsdkCVar ir_dpRRDamperPerchOffsetm("dpRRDamperPerchOffsetm");                   //          float     1    1    Pitstop right rear dampter perch offset adjustment 
irsdkCVar ir_dpRrPerchOffsetm("dpRrPerchOffsetm");                               //         float     1    1    Pitstop right rear spring offset adjustment 
irsdkCVar ir_dpRrWedgeAdj("dpRrWedgeAdj");                                       //         float     1    1    Pitstop rr spring offset adjustment 
irsdkCVar ir_dpRWingAngle("dpRWingAngle");                                       //         float     1    1    Pitstop rear wing adjustment 
irsdkCVar ir_dpRWingIndex("dpRWingIndex");                                       //         float     1    1    Pitstop rear wing adjustment 
irsdkCVar ir_dpRWingSetting("dpRWingSetting");                                   //         float     1    1    Pitstop rear wing adjustment 
irsdkCVar ir_dpTruckarmP1Dz("dpTruckarmP1Dz");                                   //         float     1    1    Pitstop truckarmP1Dz adjustment 
irsdkCVar ir_dpWedgeAdj("dpWedgeAdj");                                           //         float     1    1    Pitstop wedge adjustment 
irsdkCVar ir_LFbrakeLinePress("LFbrakeLinePress");                               //     bar     float     1    1    LF  brake line pressure 
irsdkCVar ir_LFcoldPressure("LFcoldPressure");                                   //     kPa     float     1    1    LF tire cold pressure  as set in the garage 
irsdkCVar ir_LFpressure("LFpressure");                                           //     kPa     float     1    0    LF tire pressure 
irsdkCVar ir_LFrideHeight("LFrideHeight");                                       //     m     float     1    0    LF ride height 
irsdkCVar ir_LFshockDefl("LFshockDefl");                                         //     m     float     1    1    LF shock deflection 
irsdkCVar ir_LFshockVel("LFshockVel");                                           //     m/s     float     1    1    LF shock velocity 
irsdkCVar ir_LFspeed("LFspeed");                                                 //     m/s     float     1    1    LF wheel speed 
irsdkCVar ir_LFtempCL("LFtempCL");                                               //     C     float     1    1    LF tire left carcass temperature 
irsdkCVar ir_LFtempCM("LFtempCM");                                               //     C     float     1    1    LF tire middle carcass temperature 
irsdkCVar ir_LFtempCR("LFtempCR");                                               //     C     float     1    1    LF tire right carcass temperature 
irsdkCVar ir_LFtempL("LFtempL");                                                 //     C     float     1    0    LF tire left surface temperature 
irsdkCVar ir_LFtempM("LFtempM");                                                 //     C     float     1    0    LF tire middle surface temperature 
irsdkCVar ir_LFtempR("LFtempR");                                                 //     C     float     1    0    LF tire right surface temperature 
irsdkCVar ir_LFwearL("LFwearL");                                                 //     %     float     1    1    LF tire left percent tread remaining 
irsdkCVar ir_LFwearM("LFwearM");                                                 //     %     float     1    1    LF tire middle percent tread remaining 
irsdkCVar ir_LFwearR("LFwearR");                                                 //     %     float     1    1    LF tire right percent tread remaining 
irsdkCVar ir_LRbrakeLinePress("LRbrakeLinePress");                               //     bar     float     1    1    LR  brake line pressure 
irsdkCVar ir_LRcoldPressure("LRcoldPressure");                                   //     kPa     float     1    1    LR tire cold pressure  as set in the garage 
irsdkCVar ir_LRpressure("LRpressure");                                           //     kPa     float     1    0    LR tire pressure 
irsdkCVar ir_LRrideHeight("LRrideHeight");                                       //     m     float     1    0    LR ride height 
irsdkCVar ir_LRshockDefl("LRshockDefl");                                         //     m     float     1    1    LR shock deflection 
irsdkCVar ir_LRshockVel("LRshockVel");                                           //     m/s     float     1    1    LR shock velocity 
irsdkCVar ir_LRspeed("LRspeed");                                                 //     m/s     float     1    1    LR wheel speed 
irsdkCVar ir_LRtempCL("LRtempCL");                                               //     C     float     1    1    LR tire left carcass temperature 
irsdkCVar ir_LRtempCM("LRtempCM");                                               //     C     float     1    1    LR tire middle carcass temperature 
irsdkCVar ir_LRtempCR("LRtempCR");                                               //     C     float     1    1    LR tire right carcass temperature 
irsdkCVar ir_LRtempL("LRtempL");                                                 //     C     float     1    0    LR tire left surface temperature 
irsdkCVar ir_LRtempM("LRtempM");                                                 //     C     float     1    0    LR tire middle surface temperature 
irsdkCVar ir_LRtempR("LRtempR");                                                 //     C     float     1    0    LR tire right surface temperature 
irsdkCVar ir_LRwearL("LRwearL");                                                 //     %     float     1    1    LR tire left percent tread remaining 
irsdkCVar ir_LRwearM("LRwearM");                                                 //     %     float     1    1    LR tire middle percent tread remaining 
irsdkCVar ir_LRwearR("LRwearR");                                                 //     %     float     1    1    LR tire right percent tread remaining 
irsdkCVar ir_RFbrakeLinePress("RFbrakeLinePress");                               //     bar     float     1    1    RF  brake line pressure 
irsdkCVar ir_RFcoldPressure("RFcoldPressure");                                   //     kPa     float     1    1    RF tire cold pressure  as set in the garage 
irsdkCVar ir_RFpressure("RFpressure");                                           //     kPa     float     1    0    RF tire pressure 
irsdkCVar ir_RFrideHeight("RFrideHeight");                                       //     m     float     1    0    RF ride height 
irsdkCVar ir_RFshockDefl("RFshockDefl");                                         //     m     float     1    1    RF shock deflection 
irsdkCVar ir_RFshockVel("RFshockVel");                                           //     m/s     float     1    1    RF shock velocity 
irsdkCVar ir_RFspeed("RFspeed");                                                 //     m/s     float     1    1    RF wheel speed 
irsdkCVar ir_RFtempCL("RFtempCL");                                               //     C     float     1    1    RF tire left carcass temperature 
irsdkCVar ir_RFtempCM("RFtempCM");                                               //     C     float     1    1    RF tire middle carcass temperature 
irsdkCVar ir_RFtempCR("RFtempCR");                                               //     C     float     1    1    RF tire right carcass temperature 
irsdkCVar ir_RFtempL("RFtempL");                                                 //     C     float     1    0    RF tire left surface temperature 
irsdkCVar ir_RFtempM("RFtempM");                                                 //     C     float     1    0    RF tire middle surface temperature 
irsdkCVar ir_RFtempR("RFtempR");                                                 //     C     float     1    0    RF tire right surface temperature 
irsdkCVar ir_RFwearL("RFwearL");                                                 //     %     float     1    1    RF tire left percent tread remaining 
irsdkCVar ir_RFwearM("RFwearM");                                                 //     %     float     1    1    RF tire middle percent tread remaining 
irsdkCVar ir_RFwearR("RFwearR");                                                 //     %     float     1    1    RF tire right percent tread remaining 
irsdkCVar ir_RRbrakeLinePress("RRbrakeLinePress");                               //     bar     float     1    1    R  brake line pressure 
irsdkCVar ir_RRcoldPressure("RRcoldPressure");                                   //     kPa     float     1    1    RR tire cold pressure  as set in the garage 
irsdkCVar ir_RRpressure("RRpressure");                                           //     kPa     float     1    0    RR tire pressure 
irsdkCVar ir_RRrideHeight("RRrideHeight");                                       //     m     float     1    0    RR ride height 
irsdkCVar ir_RRshockDefl("RRshockDefl");                                         //     m     float     1    1    RR shock deflection 
irsdkCVar ir_RRshockVel("RRshockVel");                                           //     m/s     float     1    1    RR shock velocity 
irsdkCVar ir_RRspeed("RRspeed");                                                 //     m/s     float     1    1    RR wheel speed 
irsdkCVar ir_RRtempCL("RRtempCL");                                               //     C     float     1    1    RR tire left carcass temperature 
irsdkCVar ir_RRtempCM("RRtempCM");                                               //     C     float     1    1    RR tire middle carcass temperature 
irsdkCVar ir_RRtempCR("RRtempCR");                                               //     C     float     1    1    RR tire right carcass temperature 
irsdkCVar ir_RRtempL("RRtempL");                                                 //     C     float     1    0    RR tire left surface temperature 
irsdkCVar ir_RRtempM("RRtempM");                                                 //     C     float     1    0    RR tire middle surface temperature 
irsdkCVar ir_RRtempR("RRtempR");                                                 //     C     float     1    0    RR tire right surface temperature 
irsdkCVar ir_RRwearL("RRwearL");                                                 //     %     float     1    1    RR tire left percent tread remaining 
irsdkCVar ir_RRwearM("RRwearM");                                                 //     %     float     1    1    RR tire middle percent tread remaining 
irsdkCVar ir_RRwearR("RRwearR");                                                 //     %     float     1    1    RR tire right percent tread remaining 
irsdkCVar ir_CarIdxClassPosition("CarIdxClassPosition");                         //         int     0    1    Cars class position in race by car index 
irsdkCVar ir_CarIdxEstTime("CarIdxEstTime");                                     //     s     float     0    1    Estimated time to reach current location on track 
irsdkCVar ir_CarIdxF2Time("CarIdxF2Time");                                       //     s     float     0    1    Race time behind leader or fastest lap time otherwise 
irsdkCVar ir_CarIdxGear("CarIdxGear");                                           //         int     0    1    -1=reverse  0=neutral  1..n=current gear by car index 
irsdkCVar ir_CarIdxLap("CarIdxLap");                                             //         int     0    1    Lap count by car index 
irsdkCVar ir_CarIdxLapDistPct("CarIdxLapDistPct");                               //     %     float     0    1    Percentage distance around lap by car index 
irsdkCVar ir_CarIdxOnPitRoad("CarIdxOnPitRoad");                                 //         bool     0    1    On pit road between the cones by car index 
irsdkCVar ir_CarIdxPosition("CarIdxPosition");                                   //         int     0    1    Cars position in race by car index 
irsdkCVar ir_CarIdxRPM("CarIdxRPM");                                             //     revs/min     float     0    1    Engine rpm by car index 
irsdkCVar ir_CarIdxSteer("CarIdxSteer");                                         //     rad     float     0    1    Steering wheel angle by car index 
irsdkCVar ir_CarIdxTrackSurface("CarIdxTrackSurface");                           //     irsdk_TrkLoc     int     0    1    Track surface type by car index 
irsdkCVar ir_CarIdxLastLapTime("CarIdxLastLapTime");                             //     s   float  ?  ?  Cars last lap time
irsdkCVar ir_CarIdxBestLapTime("CarIdxBestLapTime");                             //     s   float  ?  ?  Cars best lap time
irsdkCVar ir_CarIdxBestLapNum("CarIdxBestLapNum");                               //         int    ?  ?  Cars best lap number
irsdkCVar ir_CarIdxP2P_Status("CarIdxP2P_Status");                               //         bool   ?  ?  Push2Pass active or not
irsdkCVar ir_CarIdxP2P_Count("CarIdxP2P_Count");                                 //         int    ?  ?  Push2Pass count of usage (or remaining in Race)
irsdkCVar ir_PaceMode("PaceMode");                                               //         int    ?  ?  irsdk_PaceMode, Are we pacing or not
irsdkCVar ir_CarIdxPaceLine("CarIdxPaceLine");                                   //         int    ?  ?  What line cars are pacing in, or -1 if not pacing
irsdkCVar ir_CarIdxPaceRow("CarIdxPaceRow");                                     //         int    ?  ?  What row cars are pacing in, or -1 if not pacing
irsdkCVar ir_CarIdxPaceFlags("CarIdxPaceFlags");                                 //         int    ?  ?  irsdk_PaceFlags, Pacing status flags for each car


Session ir_session;

static bool parseYamlInt(const char *yamlStr, const char *path, int *dest)
{
    if(dest)
    {
        (*dest) = 0;

        if(yamlStr && path)
        {
            int count;
            const char *strPtr;

            if(parseYaml(yamlStr, path, &strPtr, &count))
            {
                (*dest) = atoi(strPtr);
                return true;
            }
        }
    }

    return false;
}

static bool parseYamlStr(const char *yamlStr, const char *path, std::string& dest)
{
    if(yamlStr && path)
    {
        int count;
        const char *strPtr;

        if(parseYaml(yamlStr, path, &strPtr, &count))
        {
            // strip leading quotes
            if(*strPtr == '"')
            {
                strPtr++;
                count--;
            }

            dest.assign( strPtr, count );

            // strip trailing quotes
            if( !dest.empty() && dest[dest.length()-1]=='"' )
                dest.pop_back();

            return true;
        }
    }

    return false;
}

bool ir_tick()
{
    irsdkClient& irsdk = irsdkClient::instance();

    irsdk.waitForData(16);

    if( !irsdk.isConnected() )
        return false;

    if( irsdk.wasSessionStrUpdated() )
    {
        const char* sessionYaml = irsdk.getSessionStr();

        char path[256];

        parseYamlInt( sessionYaml, "DriverInfo:DriverCarIdx:", &ir_session.driverCarIdx );
        printf("Driver Car Idx: %d\n", ir_session.driverCarIdx );

        for( int i=0; i<IR_MAX_CARS; ++i )
        {
            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}UserName:", i );
            parseYamlStr( sessionYaml, path, ir_session.cars[i].userName );

            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}CarNumber:", i );
            parseYamlStr( sessionYaml, path, ir_session.cars[i].carNumber );

            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}LicString:", i );
            parseYamlStr( sessionYaml, path, ir_session.cars[i].licenseStr );

            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}LicLevel:", i );
            parseYamlInt( sessionYaml, path, &ir_session.cars[i].license );

            sprintf( path, "DriverInfo:Drivers:CarIdx:{%d}IRating:", i );
            parseYamlInt( sessionYaml, path, &ir_session.cars[i].irating );
        }
    }

    for( int i=0; i<ir_CarIdxEstTime.getCount(); ++i )
    {
        /*
        if( ir_CarIdxEstTime.isValid() && ir_CarIdxLap.getInt(i)>=0 ) {
            printf("pos: %d, lap: %d, inpit: %d, rel: %.2f   %s  [%d %s] %d", ir_CarIdxPosition.getInt(i), ir_CarIdxLap.getInt(i), (int)ir_CarIdxOnPitRoad.getBool(i),
                ir_CarIdxEstTime.getFloat(i)-ir_CarIdxEstTime.getFloat(ir_session.driverCarIdx), 
                ir_session.cars[i].userName.c_str(), ir_session.cars[i].license, ir_session.cars[i].licenseStr.c_str(), ir_session.cars[i].irating );
            if( ir_session.driverCarIdx == i )
                printf( "  <=====\n");
            else printf("\n");
        }
        */

    }
    //printf("\n\n");

    return true;
}
