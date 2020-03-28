/*
 * HomieC.h
 *
 *  Created on: 16.03.2020
 *      Author: kulet
 */

#ifndef INCLUDE_HOMIEC_H_
#define INCLUDE_HOMIEC_H_


/*! \mainpage Homie convention C implementation
 *
 * This project implements Homie conventions v4.0.0.
 * Example can be found here
 * - \subpage Initializing 1. Initializing Homie device example
 *
 * - \subpage Adding 2. Adding interface callbacks.
 *
 * - \subpage Calling  3. Calling interface functions.
 *
 */

/*! \page Initializing Initializing
 *
 *  vHomie_InitHomieDevice(&SmartSwitch,"SmartSwitch_TableLamp"); <br>
	vHomie_AddNodeToDevice(&SmartSwitch,0,"SRR_CH1","Dimmer"); <br>
	vHomie_AddNodeToDevice(&SmartSwitch,1,"SRR_CH2","Dimmer"); <br>
	vHomie_AddENUMPropertyToNode(&SmartSwitch,0,0,"State",Names,3,PT_SET | PT_NOTIFICATION); <br>
	vHomie_AddPropertyToNode(&SmartSwitch,0,1,"DimLevel",INTEGER,PT_SET | PT_NOTIFICATION); <br>
	vHomie_AddENUMPropertyToNode(&SmartSwitch,1,0,"State",Names,3,PT_SET | PT_NOTIFICATION); <br>
	vHomie_AddPropertyToNode(&SmartSwitch,1,1,"DimLevel",INTEGER,PT_SET | PT_NOTIFICATION); <br>
 *
 */

/*! \page Calling Calling
 *
 * vHomie_CB_Connected(&SmartSwitch); <br>
   StubMQTT_Receive("homie/SmartSwitch_TableLamp/SRR_CH1/State/set","ON",2); <br>
   StubDevice_SetValueInteger(0,1,50); <br>
 *
 */

/*! \page Adding Adding interface functions.
 *
 * vHomie_AddSendFunction(&SmartSwitch,&StubMQTT_Send); <br>
 * vHomie_AddPropertyReceiveStatusCb(&SmartSwitch,0,0,&StubDevice_ReceiveValue); <br>
 */


/*! \file HomieC.h
    \brief Include files. Contains corresponding macros and definitons.

*/

#include "types.h"
#include "stdlib.h"
#include "string.h"
#include <stdio.h>


//---------------------------
// DEFINES
//---------------------------

/*! \def MAX_SINGLE_STRING
 *
 * Constant define of maximum single string buffer. It refers to topics,names and enum names.
 */
#define MAX_SINGLE_STRING 64
/*! \def MAX_NODES
 *
 * Constant define of maximum count of nodes.
 */
#define MAX_NODES 16
/*! \def MAX_PROPERTIES
 *
 * Constant define of maximum count of properties for each node.
 */
#define MAX_PROPERTIES 16
/*! \def MAX_ENUM_NAMES
 *
 * Constant define of maximum count of enumeration types.
 */
#define MAX_ENUM_NAMES 16

/*! \def GET_NODE
 *
 * Macro to get pointer to \a index node from \a device.
 */
#define GET_NODE(device,index) (device->asOwnedNodes[index])
/*! \def GET_PROPERTY
 *
 * Macro to get pointer to \a index2 property from \a index node from \a device.
 */
#define GET_PROPERTY(device,index,index2) (GET_NODE(device,index).asOwnedProperties[index2])
/*! \def GET_ENUMVAL
 *
 * Macro to get pointer \b current value of ENUM \a PropertyId property, from \a nodeId from \a device. Ensure that property is enum type (otherwise You will get wild pointer cast).
 */
#define GET_ENUMVAL(device,nodeId,PropertyId) *((int32_t*)GET_PROPERTY(device,nodeId,PropertyId).value_ptr)
/*! \def GET_ENUMPROP_AS_NAME
 *
 * Macro to get name of \b current value of ENUM \a PropertyId property, from \a nodeId from \a device. Ensure that property is enum type (otherwise You will get wild pointer cast).
 */
#define GET_ENUMPROP_AS_NAME(device,nodeId,PropertyId) GET_PROPERTY(device,nodeId,PropertyId).astrEnumNames[GET_ENUMVAL(device,nodeId,PropertyId)]
/*! \def GET_ENUMPROP_AS_NAME_INDEX
 *
 * Macro to get name of \b index value of ENUM \a PropertyId property, from \a nodeId from \a device. Ensure that property is enum type (otherwise You will get wild pointer cast).
 */
#define GET_ENUMPROP_AS_NAME_INDEX(device,nodeId,PropertyId,index) GET_PROPERTY(device,nodeId,PropertyId).astrEnumNames[index]
/*! \def GET_PAYLOAD_TYPE_NAME
 *
 * Macro to get string name of enum #ePropertyDataType.
 */
#define GET_PAYLOAD_TYPE_NAME(i) ePropertyDataTypeName[i]

/*! \def PT_SET
 * Flag indicates that property is settable.
 */
#define PT_SET 1
/*! \def PT_NOTIFICATION
 * Flag indicates that property notify about itself.
 */
#define PT_NOTIFICATION 2
/*! \def PT_COMMAND
 * Flag indicates that property contains command function.
 */
#define PT_COMMAND 4

//---------------------------
// STRUCT DEFINITIONS
//---------------------------

/*! \enum ePropertyDataType
 * Enumeration of property type.
 *
 * \var ePropertyDataType STRING
 * String type, null terminated, MAX_SINGLE_STRING_LENGTH length.
 *
 * \var ePropertyDataType INTEGER
 * Integer type, long long type (usually 64bits).
 *
 *  \var ePropertyDataType FLOAT
 * Float type, single precision.
 *
 * \var ePropertyDataType BOOLEAN
 * Logic type, true or false values.
 *
 *  \var ePropertyDataType ENUM
 * Enumeration type, values defines druing initialization. Value is stores as int32.
 *
 * \var ePropertyDataType COLOR
 * Not supported.
 *
 */
typedef enum
{
	STRING=0,
	INTEGER,
	FLOAT,
	BOOLEAN,
	ENUM,
	COLOR
}ePropertyDataType;

/*!
 * Static string array used to translate Propert type enum to string name.
 */
static char ePropertyDataTypeName[6][16]={"string","integer","float","boolean","enum","color"};

/*! \enum ePayloadType
 * Enumeration of payload type.
 *
 * \var ePayloadType UNKNOW
 * Unknown, unsupported received payload type.
 *
 * \var ePayloadType SET
 * Set payload type. It changes property value.
 *
 *  \var ePayloadType COMMAND
 * Command payload type.
 *
 */
typedef enum
{
	UNKNOW=-1,
	SET=0,
	COMMAND
}ePayloadType;

/*! \enum eDeviceState
 * Enumeration of device state.
 *
 * \var eDeviceState INIT
 * Init state.
 *
 * \var eDeviceState READY
 * Ready state.
 *
 *  \var eDeviceState DISCONNECTED
 * Disconnected state.
 *
 *  \var eDeviceState SLEEPING
 * Sleeping state.
 *
 *  \var eDeviceState LOST
 * Lost state.
 *
 *  \var eDeviceState ALERT
 * Alert state.
 */
typedef enum
{
	INIT,
	READY,
	DISCONNECTED,
	SLEEPING,
	LOST,
	ALERT
}eDeviceState;

/*! \enum eBoolean
 * Boolean type.
 */

typedef enum
{
	H_FALSE=0,
	H_TRUE=1,
	H_UNKNOWN=-1
}eBoolean;

/*! \enum eTopicDepth
 * TopicDepth type.
 */
typedef enum
{
	eDEVICE=2,
	eNODE,
	ePROPERTY
}eTopicDepth;


/*! \struct sHomieProperty
 *
 * Homie property struct. Property value is stores inside it.
 *
 */
typedef struct
{
	char strName[MAX_SINGLE_STRING]; /**< Property name. MAX_SINGLE_STRING_LENGTH length. */
	ePropertyDataType eDataType; /**< Property type.*/
	uint8_t u8Flags; /**< Property flags. Combinations of PT_SET, PT_NOTIFY and PT_COMMAND.*/
	char astrEnumNames[MAX_ENUM_NAMES][MAX_SINGLE_STRING];  /**< Array of string for enumeration names.*/
	uint8_t iEnumSize; /**< Enumeration type size.*/
	void *value_ptr; /**< Pointer to stored property value*/
	//Functions
	void (*HomieP_ReceiveStatus)(void *ptr); /**< Function pointer to notify device about received property update*/
	void (*HomieP_ReceiveCommand)(void  *ptr); /**< Function pointer to notify device about received command payload*/
}sHomieProperty;

/*! \struct sHomieNode
 *
 * Homie node struct. Stores up to MAX_PROPERTIES properties/
 *
 */
typedef struct
{
	char strName[MAX_SINGLE_STRING]; /**< Node name. MAX_SINGLE_STRING_LENGTH length. */
	char strType[MAX_SINGLE_STRING]; /**< Node type. MAX_SINGLE_STRING_LENGTH length. */
	sHomieProperty asOwnedProperties[MAX_PROPERTIES]; /**< Array of connected properties/ */
	uint8_t u8NumberOfProperties; /**< Number of connected properties (dont mess with MAX_PROPERTIES) */
}sHomieNode;

/*! \struct sHomieParseTopicReturnStruct
 *
 * Return value struct of HomieParseTopic function.
 *
 */

typedef struct
{

	uint8_t type; /**< Topic type */
	sHomieProperty *prop; /**< Pointer to property */
}sHomieParseTopicReturnStruct;

/*! \struct sHomieDevice
 *
 * Homie device struct.
 *
 */
typedef struct
{
	char strHomie[MAX_SINGLE_STRING]; /**< Homie version, fixed to 4.0.0 */
	char strName[MAX_SINGLE_STRING];  /**< Device name. MAX_SINGLE_STRING_LENGTH length. */
	uint8_t u8NumberOfNodes; /**< Count of owned nodes. Max is MAX_NODES constat. */
	eDeviceState eState; /**< Current state of Homie device. */
	sHomieNode asOwnedNodes[MAX_NODES]; /**< Array of owned nodes */
	//Functions
	void (*MQTT_Send)(char *topic, char *payload, uint32_t size); /**< Function pointer to send via MQTT layer*/
}sHomieDevice;

//Initializing functions

/**
 * \defgroup Init Initializing functions
 */

/**
 * \defgroup Cb Callbacks binding functions.
 */

/**
 * \defgroup Int Function to interface library.
 */

/**
 * \defgroup Internal Internal functions.
 */

/*!
 * \brief Initialize Homie Device, without binding function pointers.
 * \param device Pointer to sHomieDevice struct, which will be initialize.
 * \param strName Device name
 * \return void
 * \ingroup Init
*/
void vHomie_InitHomieDevice(sHomieDevice *device, char strName[]);

/*!
 * \brief Add node to device and initialize it.
 * \param device Pointer to sHomieDevice struct.
 * \param nodeId Index in array where node will be store. Have to be <= MAX_NODES constant.
 * \param strName Node name
 * \param strType Node type
 * \return void
 * \ingroup Init
*/
void vHomie_AddNodeToDevice(sHomieDevice *device, uint8_t nodeId , char strName[],char strType[]);
/*!
 * \brief Add property to node and initialize it. For enum type property look for vHomie_AddENUMPropertyToNode . No function pointers is binded.
 * \param device Pointer to sHomieDevice struct.
 * \param u8NodeIndex Index in array to which node property will be added. Have to be <= MAX_NODES constant.
 * \param PropertyId Index in array where property will be store. Have to be <= MAX_PROPERTIES constant.
*  \param strName Property name.
 * \param eDataType Property type
 * \param u8Flags Combination on PT_xxx constants. Stores information which action property can take/receive.
 * \return void
 * \ingroup Init
*/
void vHomie_AddPropertyToNode(sHomieDevice *device,uint8_t u8NodeIndex, uint8_t PropertyId, char strName[],ePropertyDataType eDataType,uint8_t u8Flags);
/*!
 * \brief Add enum type property to node and initialize it. No function pointers is binded.
 * \param device Pointer to sHomieDevice struct.
 * \param u8NodeIndex Index in array to which node property will be added. Have to be <= MAX_NODES constant.
 * \param PropertyId Index in array where property will be store. Have to be <= MAX_PROPERTIES constant.
 * \param strName Property name
 * \param strNames Array of string that contains enumeration names. First is enum 0.
 * \param uiNumberOfNames Enumeration size
 * \param u8Flags Combination on PT_xxx constants. Stores information which action property can take/receive.
 * \return void
 * \ingroup Init
*/
void vHomie_AddENUMPropertyToNode(sHomieDevice *device,uint8_t u8NodeIndex, uint8_t PropertyId, char strName[],char strNames[][MAX_SINGLE_STRING],uint8_t uiNumberOfNames, uint8_t u8Flags);
/*!
 * \brief Function to bind receive notification function.
 * \param device Pointer to sHomieDevice struct.
 * \param nodeId Index in array to which node property will be added. Have to be <= MAX_NODES constant.
 * \param PropertyId Index in array where property will be store. Have to be <= MAX_PROPERTIES constant.
 * \param HomieP_ReceiveStatus Pointer to function that will be called if property receive update notifiacation.
 * \return void
 * \ingroup Cb
*/
void vHomie_AddPropertyReceiveStatusCb(sHomieDevice *device,uint8_t nodeId,uint8_t PropertyId,void (*HomieP_ReceiveStatus)(void *payload));
/*!
 * \brief Function to bind receive command function.
 * \param device Pointer to sHomieDevice struct.
 * \param nodeId Index in array to which node property will be added. Have to be <= MAX_NODES constant.
 * \param PropertyId Index in array where property will be store. Have to be <= MAX_PROPERTIES constant.
 * \param HomieP_ReceiveCommand Pointer to function that will be called if property receive command
 * \return void
 * \ingroup Cb
*/
void vHomie_AddPropertyReceiveCommandCb(sHomieDevice *device,uint8_t nodeId,uint8_t PropertyId,void (*HomieP_ReceiveCommand)(void *payload));
/*!
 * \brief Function to bind send function from MQTT layer.
 * \param device Pointer to sHomieDevice struct.
 * \param MQTT_Send Pointer to function that will be called if pHomie device need to send MQTT message.
 * \return void
 * \ingroup Cb
*/
void vHomie_AddSendFunction(sHomieDevice *device,void (*MQTT_Send)(char *topic, char *payload, uint32_t size));
//InterfaceFunctions
/*!
 * \brief Function to transport received MQTT message to Homie device.
 * \param device Pointer to sHomieDevice struct.
 * \param topic Received topic.
 * \param payload Received payload
 * \param size Size of receibved payload.
 * \return void
 * \ingroup Int
*/
void vHomie_MQTT_ReceivePayload(sHomieDevice *device,char *topic, char *payload, uint32_t size);
/*!
 * \brief Function to set property value.Notification will be sent automatically.
 * \param device Pointer to sHomieDevice struct.
 * \param nodeId Index of node.
 * \param PropertyId Index of property.
 * \param ptr_val Poiter to value. Poiter have to be same type like property.
 * \return void
 * \ingroup Int
*/
void vHomie_HomieProperty_Set(sHomieDevice *device,uint8_t nodeId,uint8_t PropertyId,void *ptr_val);
/*!
 * \brief Function to indicate that Homie can send initial massages. Ensure that MQTT layer connect to broker.
 * \param device Pointer to sHomieDevice struct.
 * \return void
 * \ingroup Int
*/
void vHomie_CB_Connected(sHomieDevice *device);
void vHomie_CB_Disconnected(sHomieDevice *device); //TODO
void vHomie_CB_Sleep(sHomieDevice *device); //TODO
void vHomie_CB_Alert(sHomieDevice *device); //TODO
void vHomie_CB_Lost(sHomieDevice *device); //TODO



//Utilities
/*!
 * \brief Internal function.
 * \ingroup Internal
*/
void vSendNotification(sHomieDevice *device,uint8_t nodeId,uint8_t PropertyId);
/*!
 * \brief Internal function.
 * \ingroup Internal
*/
eTopicDepth vCheckTopicDepth(char *topic);
/*!
 * \brief Internal function.
 * \ingroup Internal
*/
sHomieParseTopicReturnStruct sParseTopicProperty(sHomieProperty *outProp, sHomieDevice *device, char *topic);
/*!
 * \brief Internal function.
 * \ingroup Internal
*/
sHomieProperty* sFindProperty(sHomieDevice *device, char * strDevice, char * node, char * property);
/*!
 * \brief Internal function.
 * \ingroup Internal
*/
uint8_t iSetPropertyValue(sHomieProperty *property,char *payload);
/*!
 * \brief Internal function.
 * \ingroup Internal
*/
uint8_t getEnumIndex(char enumArray[][MAX_SINGLE_STRING],char* enumName);
/*!
 * \brief Internal function.
 * \ingroup Internal
*/
char* getEnumPropValAsString(sHomieDevice *device, uint8_t nodeId, uint8_t PropertyId);
/*!
 * \brief Internal function.
 * \ingroup Internal
*/
void vSendInitMessages(sHomieDevice *device);
/*!
 * \brief Internal function.
 * \ingroup Internal
*/
void strGetNodeList(sHomieDevice *device, char *retVal);
/*!
 * \brief Internal function.
 * \ingroup Internal
*/
void strGetPropertiesList(sHomieDevice *device,uint8_t Nodeindex,char *retVal);
/*!
 * \brief Internal function.
 * \ingroup Internal
*/
void strGetEnumList(sHomieDevice *device,uint8_t Nodeindex,uint8_t Propertyindex,char *retVal);
/*!
 * \brief Internal function.
 * \ingroup Internal
*/
#endif /* INCLUDE_HOMIEC_H_ */
