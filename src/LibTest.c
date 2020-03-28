/*
 ============================================================================
 Name        : LibTest.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "HomieC.h"

sHomieDevice  SmartSwitch;

//MQTT Layer STUBS
void StubMQTT_Send(char *topic, char *payload, uint32_t size)
{
	printf("StubMQTT_Send: %s -> %s (%d)\n",topic,payload,size);
}

void StubMQTT_Receive(char *topic, char *payload, uint32_t size)
{
	printf("StubMQTT_Receive: %s -> %s (%d)\n",topic,payload,size);
	vHomie_MQTT_ReceivePayload(&SmartSwitch,topic,payload,size);
}

//DEVICE Layer STUBS

void StubDevice_ReceiveValue(void *ptr)
{
	printf("StubDevice_ReceiveValue %d,ENUM %s\n",*(int32_t*)ptr,getEnumPropValAsString(&SmartSwitch,0,0));
}

void StubDevice_SetValueInteger(uint8_t nodeId, uint8_t PropertyId, sint64_t val)
{
	printf("StubDevice_SetValueInteger %lld\n",val);
	vHomie_HomieProperty_Set(&SmartSwitch,nodeId,PropertyId,(sint64_t*)&val);
}

int main(void) {

	char Names[3][MAX_SINGLE_STRING]={"ON","OFF","UNKNOWN"};

	vHomie_InitHomieDevice(&SmartSwitch,"SmartSwitch_TableLamp");
	vHomie_AddNodeToDevice(&SmartSwitch,0,"SRR_CH1","Dimmer");
	vHomie_AddNodeToDevice(&SmartSwitch,1,"SRR_CH2","Dimmer");
	vHomie_AddENUMPropertyToNode(&SmartSwitch,0,0,"State",Names,3,PT_SET | PT_NOTIFICATION);
	vHomie_AddPropertyToNode(&SmartSwitch,0,1,"DimLevel",INTEGER,PT_SET | PT_NOTIFICATION);
	vHomie_AddENUMPropertyToNode(&SmartSwitch,1,0,"State",Names,3,PT_SET | PT_NOTIFICATION);
	vHomie_AddPropertyToNode(&SmartSwitch,1,1,"DimLevel",INTEGER,PT_SET | PT_NOTIFICATION);

	vHomie_AddSendFunction(&SmartSwitch,&StubMQTT_Send);
	vHomie_AddPropertyReceiveStatusCb(&SmartSwitch,0,0,&StubDevice_ReceiveValue);


	vHomie_CB_Connected(&SmartSwitch);
	//TC1 Receive payload INT

	StubMQTT_Receive("homie/SmartSwitch_TableLamp/SRR_CH1/State/set","ON",2);

	//TC1 Set property INT

	StubDevice_SetValueInteger(0,1,50);

	return 0;


}
