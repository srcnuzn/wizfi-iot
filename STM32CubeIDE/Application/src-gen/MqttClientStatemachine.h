/** Generated by YAKINDU Statechart Tools code generator. */

#ifndef MQTTCLIENTSTATEMACHINE_H_
#define MQTTCLIENTSTATEMACHINE_H_

#ifdef __cplusplus
extern "C" { 
#endif

/*!
* Forward declaration for the MqttClientStatemachine state machine.
*/
typedef struct MqttClientStatemachine MqttClientStatemachine;

/*!
* Forward declaration of the data structure for the MqttClientStatemachineIface interface scope.
*/
typedef struct MqttClientStatemachineIface MqttClientStatemachineIface;

/*!
* Forward declaration of the data structure for the MqttClientStatemachineIfaceSystem interface scope.
*/
typedef struct MqttClientStatemachineIfaceSystem MqttClientStatemachineIfaceSystem;

/*!
* Forward declaration of the data structure for the MqttClientStatemachineIfaceWizFi360 interface scope.
*/
typedef struct MqttClientStatemachineIfaceWizFi360 MqttClientStatemachineIfaceWizFi360;

/*!
* Forward declaration of the data structure for the MqttClientStatemachineTimeEvents interface scope.
*/
typedef struct MqttClientStatemachineTimeEvents MqttClientStatemachineTimeEvents;

#ifdef __cplusplus
}
#endif

#include "sc_types.h"
#include "sc_rxc.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*! \file Header of the state machine 'MqttClientStatemachine'.
*/

/*! Define number of states in the state enum */
#define MQTTCLIENTSTATEMACHINE_STATE_COUNT 20

/*! Define dimension of the state configuration vector for orthogonal states. */
#define MQTTCLIENTSTATEMACHINE_MAX_ORTHOGONAL_STATES 1
/*! Define maximum number of time events that can be active at once */
#define MQTTCLIENTSTATEMACHINE_MAX_PARALLEL_TIME_EVENTS 1

/*! Define indices of states in the StateConfVector */
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_UNDEFINED 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_UNDEFINED_R1_ENTRY 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_UNDEFINED_R1_RESETMODULE 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_UNDEFINED_R1_TESTMODULE 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_UNDEFINED_R1_RESTARTMODULE 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_UNDEFINED_R1_RESETSYSTEM 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_UNDEFINED_R1_START 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_UNDEFINED_R1_STOP 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_ONLINE 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_ONLINE_R1_CONNECTTOBROKER 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_ONLINE_R1_ENTRY 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_ONLINE_R1_PUBLISHTOPIC 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_ONLINE_R1_WAIT 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_OFFLINE 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_OFFLINE_R1_SETTOPIC 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_OFFLINE_R1_ENTRY 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_OFFLINE_R1_SETSTATIONMODE 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_OFFLINE_R1_CONFIGUREMQTT 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_OFFLINE_R1_ENABLEDHCP 0
#define SCVI_MQTTCLIENTSTATEMACHINE_MAIN_REGION_OFFLINE_R1_CONNECTTOACCESSPOINT 0

/*! Enumeration of all states */ 
typedef enum
{
	MqttClientStatemachine_last_state,
	MqttClientStatemachine_main_region_Undefined,
	MqttClientStatemachine_main_region_Undefined_r1_Entry,
	MqttClientStatemachine_main_region_Undefined_r1_ResetModule,
	MqttClientStatemachine_main_region_Undefined_r1_TestModule,
	MqttClientStatemachine_main_region_Undefined_r1_RestartModule,
	MqttClientStatemachine_main_region_Undefined_r1_ResetSystem,
	MqttClientStatemachine_main_region_Undefined_r1_Start,
	MqttClientStatemachine_main_region_Undefined_r1_Stop,
	MqttClientStatemachine_main_region_Online,
	MqttClientStatemachine_main_region_Online_r1_ConnectToBroker,
	MqttClientStatemachine_main_region_Online_r1_Entry,
	MqttClientStatemachine_main_region_Online_r1_PublishTopic,
	MqttClientStatemachine_main_region_Online_r1_Wait,
	MqttClientStatemachine_main_region_Offline,
	MqttClientStatemachine_main_region_Offline_r1_SetTopic,
	MqttClientStatemachine_main_region_Offline_r1_Entry,
	MqttClientStatemachine_main_region_Offline_r1_SetStationMode,
	MqttClientStatemachine_main_region_Offline_r1_ConfigureMqtt,
	MqttClientStatemachine_main_region_Offline_r1_EnableDhcp,
	MqttClientStatemachine_main_region_Offline_r1_ConnectToAccesspoint
} MqttClientStatemachineStates;


/*! Type declaration of the data structure for the MqttClientStatemachineIface interface scope. */
struct MqttClientStatemachineIface
{
	sc_integer dT;
	sc_integer publishInterval;
	sc_integer watchdogTimer;
};



/*! Type declaration of the data structure for the MqttClientStatemachineIfaceSystem interface scope. */
struct MqttClientStatemachineIfaceSystem
{
	sc_observable reset;
	sc_boolean reset_raised;
};



/*! Type declaration of the data structure for the MqttClientStatemachineIfaceWizFi360 interface scope. */
struct MqttClientStatemachineIfaceWizFi360
{
	sc_observable setStationMode;
	sc_boolean setStationMode_raised;
	sc_observable enableDhcp;
	sc_boolean enableDhcp_raised;
	sc_observable connectToAccessPoint;
	sc_boolean connectToAccessPoint_raised;
	sc_observable configureMqtt;
	sc_boolean configureMqtt_raised;
	sc_observable setTopic;
	sc_boolean setTopic_raised;
	sc_observable connectToBroker;
	sc_boolean connectToBroker_raised;
	sc_observable publishTopic;
	sc_boolean publishTopic_raised;
	sc_observable testModule;
	sc_boolean testModule_raised;
	sc_observable restartModule;
	sc_boolean restartModule_raised;
	sc_observable resetModule;
	sc_boolean resetModule_raised;
	sc_observable start;
	sc_boolean start_raised;
	sc_observable stop;
	sc_boolean stop_raised;
	sc_boolean ok_raised;
	sc_boolean error_raised;
	sc_boolean fail_raised;
	sc_boolean ready_raised;
	sc_boolean wifiConnected;
	sc_integer wifiFailCounter;
	sc_integer testFailCounter;
	sc_integer restartFailCounter;
	sc_integer resetFailCounter;
};



/*! Type declaration of the data structure for the MqttClientStatemachineTimeEvents interface scope. */
struct MqttClientStatemachineTimeEvents
{
	sc_boolean mqttClientStatemachine_main_region_Undefined_r1_Entry_tev0_raised;
	sc_boolean mqttClientStatemachine_main_region_Undefined_r1_ResetModule_tev0_raised;
	sc_boolean mqttClientStatemachine_main_region_Undefined_r1_TestModule_tev0_raised;
	sc_boolean mqttClientStatemachine_main_region_Undefined_r1_RestartModule_tev0_raised;
	sc_boolean mqttClientStatemachine_main_region_Online_r1_ConnectToBroker_tev0_raised;
	sc_boolean mqttClientStatemachine_main_region_Online_r1_PublishTopic_tev0_raised;
	sc_boolean mqttClientStatemachine_main_region_Online_r1_Wait_tev0_raised;
	sc_boolean mqttClientStatemachine_main_region_Offline_r1_SetTopic_tev0_raised;
	sc_boolean mqttClientStatemachine_main_region_Offline_r1_Entry_tev0_raised;
	sc_boolean mqttClientStatemachine_main_region_Offline_r1_SetStationMode_tev0_raised;
	sc_boolean mqttClientStatemachine_main_region_Offline_r1_ConfigureMqtt_tev0_raised;
	sc_boolean mqttClientStatemachine_main_region_Offline_r1_EnableDhcp_tev0_raised;
	sc_boolean mqttClientStatemachine_main_region_Offline_r1_ConnectToAccesspoint_tev0_raised;
};





typedef struct MqttClientStatemachineIfaceWizFi360EvBuf MqttClientStatemachineIfaceWizFi360EvBuf;
struct MqttClientStatemachineIfaceWizFi360EvBuf {
	sc_boolean ok_raised;
	sc_boolean error_raised;
	sc_boolean fail_raised;
	sc_boolean ready_raised;
};

typedef struct MqttClientStatemachineTimeEventsEvBuf MqttClientStatemachineTimeEventsEvBuf;
struct MqttClientStatemachineTimeEventsEvBuf {
	sc_boolean MqttClientStatemachine_main_region_Undefined_r1_Entry_time_event_0_raised;
	sc_boolean MqttClientStatemachine_main_region_Undefined_r1_ResetModule_time_event_0_raised;
	sc_boolean MqttClientStatemachine_main_region_Undefined_r1_TestModule_time_event_0_raised;
	sc_boolean MqttClientStatemachine_main_region_Undefined_r1_RestartModule_time_event_0_raised;
	sc_boolean MqttClientStatemachine_main_region_Online_r1_ConnectToBroker_time_event_0_raised;
	sc_boolean MqttClientStatemachine_main_region_Online_r1_PublishTopic_time_event_0_raised;
	sc_boolean MqttClientStatemachine_main_region_Online_r1_Wait_time_event_0_raised;
	sc_boolean MqttClientStatemachine_main_region_Offline_r1_SetTopic_time_event_0_raised;
	sc_boolean MqttClientStatemachine_main_region_Offline_r1_Entry_time_event_0_raised;
	sc_boolean MqttClientStatemachine_main_region_Offline_r1_SetStationMode_time_event_0_raised;
	sc_boolean MqttClientStatemachine_main_region_Offline_r1_ConfigureMqtt_time_event_0_raised;
	sc_boolean MqttClientStatemachine_main_region_Offline_r1_EnableDhcp_time_event_0_raised;
	sc_boolean MqttClientStatemachine_main_region_Offline_r1_ConnectToAccesspoint_time_event_0_raised;
};

typedef struct MqttClientStatemachineEvBuf MqttClientStatemachineEvBuf;
struct MqttClientStatemachineEvBuf {
	MqttClientStatemachineIfaceWizFi360EvBuf ifaceWizFi360;
	MqttClientStatemachineTimeEventsEvBuf timeEvents;
};


/*! 
 * Type declaration of the data structure for the MqttClientStatemachine state machine.
 * This data structure has to be allocated by the client code. 
 */
struct MqttClientStatemachine
{
	MqttClientStatemachineStates stateConfVector[MQTTCLIENTSTATEMACHINE_MAX_ORTHOGONAL_STATES];
	MqttClientStatemachineIface iface;
	MqttClientStatemachineIfaceSystem ifaceSystem;
	MqttClientStatemachineIfaceWizFi360 ifaceWizFi360;
	MqttClientStatemachineTimeEvents timeEvents;
	MqttClientStatemachineEvBuf current;
	sc_boolean isExecuting;
};



/*! Initializes the MqttClientStatemachine state machine data structures. Must be called before first usage.*/
extern void mqttClientStatemachine_init(MqttClientStatemachine* handle);


/*! Performs a 'run to completion' step. */
extern void mqttClientStatemachine_run_cycle(MqttClientStatemachine* handle);

/*! Activates the state machine. */
extern void mqttClientStatemachine_enter(MqttClientStatemachine* handle);

/*! Deactivates the state machine. */
extern void mqttClientStatemachine_exit(MqttClientStatemachine* handle);



/*! Raises a time event. */
extern void mqttClientStatemachine_raise_time_event(MqttClientStatemachine* handle, sc_eventid evid);

/*! Gets the value of the variable 'dT' that is defined in the default interface scope. */ 
extern sc_integer mqttClientStatemachine_get_dT(const MqttClientStatemachine* handle);
/*! Sets the value of the variable 'dT' that is defined in the default interface scope. */ 
extern void mqttClientStatemachine_set_dT(MqttClientStatemachine* handle, sc_integer value);
/*! Gets the value of the variable 'publishInterval' that is defined in the default interface scope. */ 
extern sc_integer mqttClientStatemachine_get_publishInterval(const MqttClientStatemachine* handle);
/*! Sets the value of the variable 'publishInterval' that is defined in the default interface scope. */ 
extern void mqttClientStatemachine_set_publishInterval(MqttClientStatemachine* handle, sc_integer value);
/*! Gets the value of the variable 'watchdogTimer' that is defined in the default interface scope. */ 
extern sc_integer mqttClientStatemachine_get_watchdogTimer(const MqttClientStatemachine* handle);
/*! Sets the value of the variable 'watchdogTimer' that is defined in the default interface scope. */ 
extern void mqttClientStatemachine_set_watchdogTimer(MqttClientStatemachine* handle, sc_integer value);
/*! Returns the observable for the out event 'reset' that is defined in the interface scope 'System'. */ 
extern sc_observable* mqttClientStatemachine_System_get_reset(MqttClientStatemachine* handle);

/*! Checks if the out event 'reset' that is defined in the interface scope 'System' has been raised. */ 
extern sc_boolean mqttClientStatemachine_System_is_raised_reset(const MqttClientStatemachine* handle);

/*! Returns the observable for the out event 'setStationMode' that is defined in the interface scope 'WizFi360'. */ 
extern sc_observable* mqttClientStatemachine_WizFi360_get_setStationMode(MqttClientStatemachine* handle);

/*! Checks if the out event 'setStationMode' that is defined in the interface scope 'WizFi360' has been raised. */ 
extern sc_boolean mqttClientStatemachine_WizFi360_is_raised_setStationMode(const MqttClientStatemachine* handle);

/*! Returns the observable for the out event 'enableDhcp' that is defined in the interface scope 'WizFi360'. */ 
extern sc_observable* mqttClientStatemachine_WizFi360_get_enableDhcp(MqttClientStatemachine* handle);

/*! Checks if the out event 'enableDhcp' that is defined in the interface scope 'WizFi360' has been raised. */ 
extern sc_boolean mqttClientStatemachine_WizFi360_is_raised_enableDhcp(const MqttClientStatemachine* handle);

/*! Returns the observable for the out event 'connectToAccessPoint' that is defined in the interface scope 'WizFi360'. */ 
extern sc_observable* mqttClientStatemachine_WizFi360_get_connectToAccessPoint(MqttClientStatemachine* handle);

/*! Checks if the out event 'connectToAccessPoint' that is defined in the interface scope 'WizFi360' has been raised. */ 
extern sc_boolean mqttClientStatemachine_WizFi360_is_raised_connectToAccessPoint(const MqttClientStatemachine* handle);

/*! Returns the observable for the out event 'configureMqtt' that is defined in the interface scope 'WizFi360'. */ 
extern sc_observable* mqttClientStatemachine_WizFi360_get_configureMqtt(MqttClientStatemachine* handle);

/*! Checks if the out event 'configureMqtt' that is defined in the interface scope 'WizFi360' has been raised. */ 
extern sc_boolean mqttClientStatemachine_WizFi360_is_raised_configureMqtt(const MqttClientStatemachine* handle);

/*! Returns the observable for the out event 'setTopic' that is defined in the interface scope 'WizFi360'. */ 
extern sc_observable* mqttClientStatemachine_WizFi360_get_setTopic(MqttClientStatemachine* handle);

/*! Checks if the out event 'setTopic' that is defined in the interface scope 'WizFi360' has been raised. */ 
extern sc_boolean mqttClientStatemachine_WizFi360_is_raised_setTopic(const MqttClientStatemachine* handle);

/*! Returns the observable for the out event 'connectToBroker' that is defined in the interface scope 'WizFi360'. */ 
extern sc_observable* mqttClientStatemachine_WizFi360_get_connectToBroker(MqttClientStatemachine* handle);

/*! Checks if the out event 'connectToBroker' that is defined in the interface scope 'WizFi360' has been raised. */ 
extern sc_boolean mqttClientStatemachine_WizFi360_is_raised_connectToBroker(const MqttClientStatemachine* handle);

/*! Returns the observable for the out event 'publishTopic' that is defined in the interface scope 'WizFi360'. */ 
extern sc_observable* mqttClientStatemachine_WizFi360_get_publishTopic(MqttClientStatemachine* handle);

/*! Checks if the out event 'publishTopic' that is defined in the interface scope 'WizFi360' has been raised. */ 
extern sc_boolean mqttClientStatemachine_WizFi360_is_raised_publishTopic(const MqttClientStatemachine* handle);

/*! Returns the observable for the out event 'testModule' that is defined in the interface scope 'WizFi360'. */ 
extern sc_observable* mqttClientStatemachine_WizFi360_get_testModule(MqttClientStatemachine* handle);

/*! Checks if the out event 'testModule' that is defined in the interface scope 'WizFi360' has been raised. */ 
extern sc_boolean mqttClientStatemachine_WizFi360_is_raised_testModule(const MqttClientStatemachine* handle);

/*! Returns the observable for the out event 'restartModule' that is defined in the interface scope 'WizFi360'. */ 
extern sc_observable* mqttClientStatemachine_WizFi360_get_restartModule(MqttClientStatemachine* handle);

/*! Checks if the out event 'restartModule' that is defined in the interface scope 'WizFi360' has been raised. */ 
extern sc_boolean mqttClientStatemachine_WizFi360_is_raised_restartModule(const MqttClientStatemachine* handle);

/*! Returns the observable for the out event 'resetModule' that is defined in the interface scope 'WizFi360'. */ 
extern sc_observable* mqttClientStatemachine_WizFi360_get_resetModule(MqttClientStatemachine* handle);

/*! Checks if the out event 'resetModule' that is defined in the interface scope 'WizFi360' has been raised. */ 
extern sc_boolean mqttClientStatemachine_WizFi360_is_raised_resetModule(const MqttClientStatemachine* handle);

/*! Returns the observable for the out event 'start' that is defined in the interface scope 'WizFi360'. */ 
extern sc_observable* mqttClientStatemachine_WizFi360_get_start(MqttClientStatemachine* handle);

/*! Checks if the out event 'start' that is defined in the interface scope 'WizFi360' has been raised. */ 
extern sc_boolean mqttClientStatemachine_WizFi360_is_raised_start(const MqttClientStatemachine* handle);

/*! Returns the observable for the out event 'stop' that is defined in the interface scope 'WizFi360'. */ 
extern sc_observable* mqttClientStatemachine_WizFi360_get_stop(MqttClientStatemachine* handle);

/*! Checks if the out event 'stop' that is defined in the interface scope 'WizFi360' has been raised. */ 
extern sc_boolean mqttClientStatemachine_WizFi360_is_raised_stop(const MqttClientStatemachine* handle);

/*! Raises the in event 'ok' that is defined in the interface scope 'WizFi360'. */ 
extern void mqttClientStatemachine_WizFi360_raise_ok(MqttClientStatemachine* handle);
/*! Raises the in event 'error' that is defined in the interface scope 'WizFi360'. */ 
extern void mqttClientStatemachine_WizFi360_raise_error(MqttClientStatemachine* handle);
/*! Raises the in event 'fail' that is defined in the interface scope 'WizFi360'. */ 
extern void mqttClientStatemachine_WizFi360_raise_fail(MqttClientStatemachine* handle);
/*! Raises the in event 'ready' that is defined in the interface scope 'WizFi360'. */ 
extern void mqttClientStatemachine_WizFi360_raise_ready(MqttClientStatemachine* handle);
/*! Gets the value of the variable 'wifiConnected' that is defined in the interface scope 'WizFi360'. */ 
extern sc_boolean mqttClientStatemachine_WizFi360_get_wifiConnected(const MqttClientStatemachine* handle);
/*! Sets the value of the variable 'wifiConnected' that is defined in the interface scope 'WizFi360'. */ 
extern void mqttClientStatemachine_WizFi360_set_wifiConnected(MqttClientStatemachine* handle, sc_boolean value);
/*! Gets the value of the variable 'wifiFailCounter' that is defined in the interface scope 'WizFi360'. */ 
extern sc_integer mqttClientStatemachine_WizFi360_get_wifiFailCounter(const MqttClientStatemachine* handle);
/*! Sets the value of the variable 'wifiFailCounter' that is defined in the interface scope 'WizFi360'. */ 
extern void mqttClientStatemachine_WizFi360_set_wifiFailCounter(MqttClientStatemachine* handle, sc_integer value);
/*! Gets the value of the variable 'testFailCounter' that is defined in the interface scope 'WizFi360'. */ 
extern sc_integer mqttClientStatemachine_WizFi360_get_testFailCounter(const MqttClientStatemachine* handle);
/*! Sets the value of the variable 'testFailCounter' that is defined in the interface scope 'WizFi360'. */ 
extern void mqttClientStatemachine_WizFi360_set_testFailCounter(MqttClientStatemachine* handle, sc_integer value);
/*! Gets the value of the variable 'restartFailCounter' that is defined in the interface scope 'WizFi360'. */ 
extern sc_integer mqttClientStatemachine_WizFi360_get_restartFailCounter(const MqttClientStatemachine* handle);
/*! Sets the value of the variable 'restartFailCounter' that is defined in the interface scope 'WizFi360'. */ 
extern void mqttClientStatemachine_WizFi360_set_restartFailCounter(MqttClientStatemachine* handle, sc_integer value);
/*! Gets the value of the variable 'resetFailCounter' that is defined in the interface scope 'WizFi360'. */ 
extern sc_integer mqttClientStatemachine_WizFi360_get_resetFailCounter(const MqttClientStatemachine* handle);
/*! Sets the value of the variable 'resetFailCounter' that is defined in the interface scope 'WizFi360'. */ 
extern void mqttClientStatemachine_WizFi360_set_resetFailCounter(MqttClientStatemachine* handle, sc_integer value);

/*!
 * Checks whether the state machine is active (until 2.4.1 this method was used for states).
 * A state machine is active if it was entered. It is inactive if it has not been entered at all or if it has been exited.
 */
extern sc_boolean mqttClientStatemachine_is_active(const MqttClientStatemachine* handle);

/*!
 * Checks if all active states are final. 
 * If there are no active states then the state machine is considered being inactive. In this case this method returns false.
 */
extern sc_boolean mqttClientStatemachine_is_final(const MqttClientStatemachine* handle);

/*! Checks if the specified state is active (until 2.4.1 the used method for states was called isActive()). */
extern sc_boolean mqttClientStatemachine_is_state_active(const MqttClientStatemachine* handle, MqttClientStatemachineStates state);


#ifdef __cplusplus
}
#endif 

#endif /* MQTTCLIENTSTATEMACHINE_H_ */
