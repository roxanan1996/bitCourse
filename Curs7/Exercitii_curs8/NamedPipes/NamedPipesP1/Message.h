#pragma once

#define MESSAGE_TYPE_INT 0
#define MESSAGE_TYPE_STRING 1
typedef struct _Message
{
	int messageType;
	int intValue;
	wchar_t stringValue[256];
} Message, *LPMessage;