/***********************************************************************
 *
 * POS Pole Display Library
 * Copyright (C) by Logic Controls, LLC
 * http://www.logiccontrols.com/
 *
 ***********************************************************************/

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/usb/IOUSBLib.h>

/* ioctl values */
#define IOCTL_GET_HARD_VERSION	1
#define IOCTL_GET_DRV_VERSION	2

//typedef struct LCPDDeviceDescriptor * LCPDDeviceDescriptorRef;
typedef void* LCPDDeviceDescriptorRef;

// get descriptors for present LPCD devices
IOReturn LCPDGetDeviceDescriptors(LCPDDeviceDescriptorRef **foundInterfaces,
								  int *foundCount);

// ioctl
UInt32 LCPDioctl(LCPDDeviceDescriptorRef descr, UInt32 command, char *buffer);

// write data to bulk pipe
UInt32 LCPDWrite(LCPDDeviceDescriptorRef descr, char *buffer, UInt32 size);

// release the descriptors obtained from LCPDGetDeviceDescriptors
void LCPDReleaseDescriptors(LCPDDeviceDescriptorRef *descr, UInt32 count);

// register Wake-notification callback in the runloop
typedef void (*LCPDWakeCallback) ();
bool LCPDRegisterWakeCallback(LCPDWakeCallback, CFRunLoopRef);
