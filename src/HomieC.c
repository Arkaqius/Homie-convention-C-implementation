/*
 * HomieC.c
 *
 *  Created on: 16.03.2020
 *      Author: kulet
 */

#include "HomieC.h"

//Static variables
static const char STR_HOMIE[MAX_SINGLE_STRING] = "v4.0.0";

/*Initialize HomieDevice
 *
 * @param strName[] - DeviceName
 * @return Initialized HomieDevice
 */
void vHomie_InitHomieDevice(sHomieDevice *device, char strName[])
{

	os_strncpy(device->strHomie, STR_HOMIE, MAX_SINGLE_STRING); //set HomieVersion
	os_strncpy(device->strName, strName, MAX_SINGLE_STRING); //set Name
	device->eState = INIT; //Set init value
	device->u8NumberOfNodes = 0; //Initialize number of nodes
}

void vHomie_AddNodeToDevice(sHomieDevice *device, uint8_t nodeId, char strName[], char strType[])
{

	os_strncpy(GET_NODE(device,nodeId).strName, strName, MAX_SINGLE_STRING); //set Name
	os_strncpy(GET_NODE(device,nodeId).strType, strType, MAX_SINGLE_STRING); //set type
	GET_NODE(device,nodeId).u8NumberOfProperties = 0; //Initializa number of property
	device->u8NumberOfNodes++; //Increment number of nodes in device
}

void vHomie_AddPropertyToNode(sHomieDevice *device, uint8_t u8NodeIndex, uint8_t PropertyId, char strName[], ePropertyDataType eDataType, uint8_t u8Flags)
{
	os_strncpy(GET_PROPERTY(device,u8NodeIndex,PropertyId).strName, strName,
	MAX_SINGLE_STRING); //set Name
	GET_PROPERTY(device,u8NodeIndex,PropertyId).eDataType = eDataType; //setDataType
	GET_PROPERTY(device,u8NodeIndex,PropertyId).u8Flags = u8Flags; //setFlags
	switch (eDataType)
	{
	case STRING:
		GET_PROPERTY(device,u8NodeIndex,PropertyId).value_ptr = (char*) malloc(sizeof(char) * MAX_SINGLE_STRING);
		break;
	case INTEGER:
		GET_PROPERTY(device,u8NodeIndex,PropertyId).value_ptr = (sint64*) malloc(sizeof(sint64));
		break;
	case FLOAT:
		GET_PROPERTY(device,u8NodeIndex,PropertyId).value_ptr = (float*) malloc(sizeof(float));
		break;
	case BOOLEAN:
		GET_PROPERTY(device,u8NodeIndex,PropertyId).value_ptr = (int32_t*) malloc(sizeof(int32_t));
		break;
	case ENUM:
		//TODO
		break;
	case COLOR: //TODO

		break;
	}

	GET_NODE(device,u8NodeIndex).u8NumberOfProperties++; //Increment number of properties in node

}

void vHomie_AddSendFunction(sHomieDevice *device, void (*MQTT_Send)(char *topic, char *payload, uint32_t size))
{
	device->MQTT_Send = MQTT_Send;
}

void vHomie_MQTT_ReceivePayload(sHomieDevice *device,char *topic, char *payload, uint32_t size)
{
	sHomieProperty prop;
	sHomieParseTopicReturnStruct retVal;
	switch (vCheckTopicDepth(topic))
	{
	case eDEVICE: //TODO

		break;

	case eNODE: //TODO

		break;

	case ePROPERTY:
		retVal = sParseTopicProperty(&prop,device,topic);
		if(retVal.prop!=NULL)
		{
			if(retVal.type==SET)
			{
			iSetPropertyValue(retVal.prop,payload);

			}
			else if (retVal.type==COMMAND)
			{
				retVal.prop->HomieP_ReceiveCommand(payload);
			}
		}
		else
			;//TODO d
		break;

	default: //TODO
		break;

	}

}

uint8_t iSetPropertyValue(sHomieProperty *property,char *payload)
{

	switch (property->eDataType)
		{
		case STRING:
			os_strncpy((char*)property->value_ptr,payload,MAX_SINGLE_STRING);
			(*property->HomieP_ReceiveStatus)(payload);
			break;
		case INTEGER:
			*(sint64*)property->value_ptr=(sint64)atoi(payload);
			(*property->HomieP_ReceiveStatus)((sint64*)property->value_ptr);
			break;
		case FLOAT:
			*(float*)property->value_ptr=(float)atof(payload);
			(*property->HomieP_ReceiveStatus)((float*)property->value_ptr);
			break;
		case BOOLEAN:
			if(os_strncmp(payload,"true",MAX_SINGLE_STRING)==0)
			{
				*(int32_t*)property->value_ptr=H_TRUE;
				(*property->HomieP_ReceiveStatus)((int32_t*)H_TRUE);
			}
			else if(os_strncmp(payload,"false",MAX_SINGLE_STRING)==0)
			{
				*(int32_t*)property->value_ptr=H_FALSE;
				(*property->HomieP_ReceiveStatus)((int32_t*)H_FALSE);
			}
			else
			{
				*(int32_t*)property->value_ptr=H_UNKNOWN; //TODO
			}
			break;
		case ENUM:
			*(int32_t*)property->value_ptr=getEnumIndex((property->astrEnumNames),payload);
			(*property->HomieP_ReceiveStatus)((int32_t*)property->value_ptr);
			break;
		case COLOR: //TODO

			break;
		}

	return 0;
}

void vHomie_AddPropertyReceiveStatusCb(sHomieDevice *device, uint8_t nodeId, uint8_t PropertyId, void (*HomieP_ReceiveStatus)(void *payload))
{

	GET_PROPERTY(device,nodeId,PropertyId).HomieP_ReceiveStatus = HomieP_ReceiveStatus;

}

void vHomie_AddPropertyReceiveCommandCb(sHomieDevice *device, uint8_t nodeId, uint8_t PropertyId, void (*HomieP_ReceiveCommand)(void *payload))
{

	GET_PROPERTY(device,nodeId,PropertyId).HomieP_ReceiveCommand = HomieP_ReceiveCommand;

}

void vHomie_HomieProperty_Set(sHomieDevice *device, uint8_t nodeId, uint8_t PropertyId, void *ptr_val)
{

	switch (GET_PROPERTY(device,nodeId,PropertyId).eDataType)
	{
	case STRING:
		os_strncpy((char*) GET_PROPERTY(device,nodeId,PropertyId).value_ptr, (char*) ptr_val, MAX_SINGLE_STRING);
		break;
	case INTEGER:
		*(sint64*)GET_PROPERTY(device,nodeId,PropertyId).value_ptr = *(sint64*)ptr_val;
		break;
	case FLOAT:
		*(float*)GET_PROPERTY(device,nodeId,PropertyId).value_ptr = *(float*)ptr_val;
		break;
	case BOOLEAN:
		*(int32_t*)GET_PROPERTY(device,nodeId,PropertyId).value_ptr = *(int32_t*)ptr_val;
		break;
	case ENUM:
		*(int32_t*)GET_PROPERTY(device,nodeId,PropertyId).value_ptr = *(int32_t*)ptr_val;
		break;
	case COLOR: //TODO

		break;
	}

	vSendNotification(device, nodeId, PropertyId);

}

uint8_t getEnumIndex(char enumArray[][MAX_SINGLE_STRING],char* enumName)
{
	for(int i=0;i<MAX_ENUM_NAMES;i++)
	{
		if(os_strncmp(enumArray[i],enumName,MAX_SINGLE_STRING)==0)
			return i;
	}

	return MAX_ENUM_NAMES+1;

}

void vSendNotification(sHomieDevice *device, uint8_t nodeId, uint8_t PropertyId)
{

	char topic[MAX_SINGLE_STRING];
	char payload[MAX_SINGLE_STRING];

	os_snprintf(topic,MAX_SINGLE_STRING,"homie/%s/%s/%s",device->strName,GET_NODE(device,nodeId).strName,GET_PROPERTY(device,nodeId,PropertyId).strName);

	switch (GET_PROPERTY(device,nodeId,PropertyId).eDataType)
		{
		case STRING:
			os_strncpy(payload,(char*)GET_PROPERTY(device,nodeId,PropertyId).value_ptr,MAX_SINGLE_STRING);
			break;
		case INTEGER:
			os_snprintf(payload,MAX_SINGLE_STRING,"%lld",*((sint64_t*)GET_PROPERTY(device,nodeId,PropertyId).value_ptr));
			break;
		case FLOAT:
			os_snprintf(payload,MAX_SINGLE_STRING,"%f",*((float*)GET_PROPERTY(device,nodeId,PropertyId).value_ptr));
			break;
		case BOOLEAN:
			if(*((int32_t*)GET_PROPERTY(device,nodeId,PropertyId).value_ptr)==H_FALSE)
				os_snprintf(payload,MAX_SINGLE_STRING,"%s","false");
			else if(*((int32_t*)GET_PROPERTY(device,nodeId,PropertyId).value_ptr)==H_TRUE)
				os_snprintf(payload,MAX_SINGLE_STRING,"%s","true");
			else
				;//TODO
			break;
		case ENUM:
			os_snprintf(payload,MAX_SINGLE_STRING,"%s",GET_ENUMPROP_AS_NAME(device,nodeId,PropertyId));
			break;
		case COLOR: //TODO

			break;
		}


	device->MQTT_Send(topic,payload,os_strlen(payload));

}

eTopicDepth vCheckTopicDepth(char *topic)
{
	uint32_t retVal = 0;
	int i;
	for (i = 0; i < strlen(topic); i++)
	{
		if (topic[i] == '/')
			retVal++;
	}
	return retVal;
}

sHomieParseTopicReturnStruct sParseTopicProperty(sHomieProperty *outProp, sHomieDevice *device, char *topic)
{
	char strDevice[MAX_SINGLE_STRING];
	char strNode[MAX_SINGLE_STRING];
	char strProperty[MAX_SINGLE_STRING];
	char strSetOrCmd[MAX_SINGLE_STRING];
	char *ptr = topic;

	sHomieParseTopicReturnStruct retVal;

	int8_t array[4];
	int32_t i = 0;

	while ((ptr = os_strchr(ptr + 1, (int) '/')) != NULL)
	{
		array[i] = ptr - topic;
		i++;
	}

	memcpy(strDevice, topic + array[0] + 1, array[1] - array[0] - 1);
	strDevice[array[1] - array[0] - 1]='\0';
	memcpy(strNode, topic + array[1] + 1, array[2] - array[1] - 1);
	strNode[array[2] - array[1] - 1]='\0';
	memcpy(strProperty, topic + array[2] + 1, array[3] - array[2] - 1);
	strProperty[array[3] - array[2] - 1]='\0';
	memcpy(strSetOrCmd, topic + array[3] + 1, strlen(topic) - array[2] - 1);
	strSetOrCmd[strlen(topic) - array[2] - 1]='\0';

	retVal.prop=sFindProperty(device, strDevice, strNode, strProperty);

	if(os_strncmp(strSetOrCmd,"set",MAX_SINGLE_STRING)==0)
		retVal.type=SET;
	else if(os_strncmp(strSetOrCmd,"set",MAX_SINGLE_STRING)==0)
		retVal.type= COMMAND;
	else
		retVal.type=UNKNOW;

	return retVal;
}

sHomieProperty* sFindProperty(sHomieDevice *device, char * strDevice, char * node, char * property)
{
	int32_t i = 0;
	int32_t j = 0;

	if (os_strncmp(device->strName, strDevice, MAX_SINGLE_STRING) == 0)
	{
		for (i = 0; i < device->u8NumberOfNodes; i++)
		{
			if (os_strncmp(GET_NODE(device,i).strName, node, MAX_SINGLE_STRING) == 0)
			{
				for (j = 0; j < GET_NODE(device,i).u8NumberOfProperties; j++)
				{
					if (os_strncmp(GET_PROPERTY(device,i,j).strName, property, MAX_SINGLE_STRING) == 0)
					{
						return &GET_PROPERTY(device, i, j);
					}
				}

			}
		}
	}

	return NULL;
}

void vHomie_AddENUMPropertyToNode(sHomieDevice *device,uint8_t u8NodeIndex, uint8_t PropertyId, char strName[],char strNames[][MAX_SINGLE_STRING],uint8_t uiNumberOfNames, uint8_t u8Flags)
{
		os_strncpy(GET_PROPERTY(device,u8NodeIndex,PropertyId).strName, strName,
		MAX_SINGLE_STRING); //set Name
		GET_PROPERTY(device,u8NodeIndex,PropertyId).u8Flags = u8Flags; //setFlags
		GET_PROPERTY(device,u8NodeIndex,PropertyId).value_ptr = (int32_t*) malloc(sizeof(int32_t));
		GET_PROPERTY(device,u8NodeIndex,PropertyId).eDataType = ENUM;
		GET_PROPERTY(device,u8NodeIndex,PropertyId).iEnumSize = uiNumberOfNames;
		for(int i=0;i<uiNumberOfNames;i++)
		{
			os_strncpy(GET_PROPERTY(device,u8NodeIndex,PropertyId).astrEnumNames[i],strNames[i],MAX_SINGLE_STRING);
		}

		os_strncpy(GET_PROPERTY(device,u8NodeIndex,PropertyId).astrEnumNames[MAX_ENUM_NAMES+1],"INVALID",MAX_SINGLE_STRING);


		GET_NODE(device,u8NodeIndex).u8NumberOfProperties++; //Increment number of properties in node
}


char* getEnumPropValAsString(sHomieDevice *device, uint8_t nodeId, uint8_t PropertyId)
{

	return GET_PROPERTY(device,nodeId,PropertyId).astrEnumNames[GET_ENUMVAL(device,nodeId,PropertyId)];
}



void vHomie_CB_Connected(sHomieDevice *device){
	vSendInitMessages(device);
	device->eState=READY;
}

void vSendInitMessages(sHomieDevice *device){

	char topic[MAX_SINGLE_STRING];
	char payload[MAX_SINGLE_STRING];
	char retVal[MAX_SINGLE_STRING];

	//Send device status
	os_snprintf(topic,MAX_SINGLE_STRING,"homie/%s/$homie",device->strName);
	os_snprintf(payload,MAX_SINGLE_STRING,"%s",device->strHomie);
	device->MQTT_Send(topic,payload,os_strlen(payload));

	os_snprintf(topic,MAX_SINGLE_STRING,"homie/%s/$name ",device->strName);
	os_snprintf(payload,MAX_SINGLE_STRING,"%s",device->strName);
	device->MQTT_Send(topic,payload,os_strlen(payload));

	os_snprintf(topic,MAX_SINGLE_STRING,"homie/%s/$state ",device->strName);
	os_snprintf(payload,MAX_SINGLE_STRING,"%s","ready");
	device->MQTT_Send(topic,payload,os_strlen(payload));

	os_snprintf(topic,MAX_SINGLE_STRING,"homie/%s/$nodes ",device->strName);
	strGetNodeList(device,retVal);
	os_snprintf(payload,MAX_SINGLE_STRING,"%s",retVal);
	device->MQTT_Send(topic,payload,os_strlen(payload));

	//Send nodes

	for(int i=0;i<device->u8NumberOfNodes;i++)
	{
		os_snprintf(topic,MAX_SINGLE_STRING,"homie/%s/%s/$name ",device->strName,GET_NODE(device,i).strName);
		os_snprintf(payload,MAX_SINGLE_STRING,"%s",GET_NODE(device,i).strName);
		device->MQTT_Send(topic,payload,os_strlen(payload));

		os_snprintf(topic,MAX_SINGLE_STRING,"homie/%s/%s/$type ",device->strName,GET_NODE(device,i).strName);
		os_snprintf(payload,MAX_SINGLE_STRING,"%s",GET_NODE(device,i).strType);
		device->MQTT_Send(topic,payload,os_strlen(payload));

		os_snprintf(topic,MAX_SINGLE_STRING,"homie/%s/%s/$properties ",device->strName,GET_NODE(device,i).strName);
		strGetPropertiesList(device,i,retVal);
		os_snprintf(payload,MAX_SINGLE_STRING,"%s",retVal);
		device->MQTT_Send(topic,payload,os_strlen(payload));


	}

	//Send properties

	for(int i=0;i<device->u8NumberOfNodes;i++)
		{
			for(int j=0;j<GET_NODE(device,i).u8NumberOfProperties;j++)
			{
				os_snprintf(topic,MAX_SINGLE_STRING,"homie/%s/%s/%s/$name ",device->strName,GET_NODE(device,i).strName,GET_PROPERTY(device,i,j).strName);
				os_snprintf(payload,MAX_SINGLE_STRING,"%s",GET_PROPERTY(device,i,j).strName);
				device->MQTT_Send(topic,payload,os_strlen(payload));

				os_snprintf(topic,MAX_SINGLE_STRING,"homie/%s/%s/%s/$datatype ",device->strName,GET_NODE(device,i).strName,GET_PROPERTY(device,i,j).strName);
				os_snprintf(payload,MAX_SINGLE_STRING,"%s",GET_PAYLOAD_TYPE_NAME(GET_PROPERTY(device,i,j).eDataType));
				device->MQTT_Send(topic,payload,os_strlen(payload));

				if(GET_PROPERTY(device,i,j).u8Flags&PT_SET)
				{
					os_snprintf(topic,MAX_SINGLE_STRING,"homie/%s/%s/%s/$settable ",device->strName,GET_NODE(device,i).strName,GET_PROPERTY(device,i,j).strName);
					os_snprintf(payload,MAX_SINGLE_STRING,"%s","true");
					device->MQTT_Send(topic,payload,os_strlen(payload));
				}

				if(GET_PROPERTY(device,i,j).u8Flags&PT_COMMAND)
				{
					os_snprintf(topic,MAX_SINGLE_STRING,"homie/%s/%s/%s/$commendable ",device->strName,GET_NODE(device,i).strName,GET_PROPERTY(device,i,j).strName);
					os_snprintf(payload,MAX_SINGLE_STRING,"%s","true");
					device->MQTT_Send(topic,payload,os_strlen(payload));
				}

				if(GET_PROPERTY(device,i,j).eDataType==ENUM)
					{
						os_snprintf(topic,MAX_SINGLE_STRING,"homie/%s/%s/%s/$unit ",device->strName,GET_NODE(device,i).strName,GET_PROPERTY(device,i,j).strName);
						strGetEnumList(device,i,j,retVal);
						os_snprintf(payload,MAX_SINGLE_STRING,"%s",retVal);
						device->MQTT_Send(topic,payload,os_strlen(payload));
					}

			}

		}


}

void strGetEnumList(sHomieDevice *device,uint8_t Nodeindex,uint8_t Propertyindex,char *retVal){

	retVal[0]='\0';

		for(int i=0;i<GET_PROPERTY(device,Nodeindex,Propertyindex).iEnumSize;i++)
		{
			if(i!=0)
				strncat(retVal,",",MAX_SINGLE_STRING);
			strncat(retVal,GET_ENUMPROP_AS_NAME_INDEX(device,Nodeindex,Propertyindex,i),MAX_SINGLE_STRING);

		}
}

void strGetPropertiesList(sHomieDevice *device,uint8_t Nodeindex,char *retVal){

	retVal[0]='\0';

	for(int i=0;i<GET_NODE(device,Nodeindex).u8NumberOfProperties;i++)
	{
		if(i!=0)
			strncat(retVal,",",MAX_SINGLE_STRING);
		strncat(retVal,GET_PROPERTY(device,Nodeindex,i).strName,MAX_SINGLE_STRING);

	}

}

void strGetNodeList(sHomieDevice *device, char *retVal)
{

	retVal[0]='\0';

	for(int i=0;i<device->u8NumberOfNodes;i++)
	{
		if(i!=0)
			strncat(retVal,",",MAX_SINGLE_STRING);
		strncat(retVal,GET_NODE(device,i).strName,MAX_SINGLE_STRING);
	}
}


