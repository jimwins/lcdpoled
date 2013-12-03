#include <stdio.h>
#include <unistd.h>

#include <CoreFoundation/CFRunLoop.h>

#import <CoreFoundation/CFSocket.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "usblcpdmac.h"


//                   0123456789012345678901234567890123456789
//#define LOGO_STRING "\x1f\x14\x05Raw Materials Art Supplies\x0d"
#define LOGO_STRING "\x1f\x14\x12Raw Materials\r\nArt Supplies"
#define SPACE_STRING "                                      "
#define WAKEUP_STRING "  computer wakeup   "
#define STR_LENGTH 20

LCPDDeviceDescriptorRef *deviceGot;
int deviceNumber;



int write_pd_text(LCPDDeviceDescriptorRef descr, char* text, long len)
{
	char buf[128];
	strncpy(buf, text,len);
	
	if(LCPDWrite(descr, buf, (UInt32)len))
	{
		fprintf(stderr,"--Error writing text\n");
		return -1;
	} 
	
	return 0;
}

void DisplayText(char *text, long len)
{	
	LCPDGetDeviceDescriptors(&deviceGot, &deviceNumber);
	if (!deviceNumber)
	{
		fprintf(stderr, "No LCPD devices found\n");
		LCPDReleaseDescriptors(deviceGot, deviceNumber);
		return;
	}
	
	for (int i = 0; i < deviceNumber; i++) 
	{
		write_pd_text(deviceGot[i], text, len);
	}
	
	LCPDReleaseDescriptors(deviceGot, deviceNumber);
}

void IdleCallback(CFRunLoopTimerRef timer, void *info)
{
	DisplayText(LOGO_STRING, strlen(LOGO_STRING));
}

void WakeCallback()
{
	//LCPD devices are re-attached to the system after sleep and wake. 
	//Perform tests with the new instances.
	DisplayText(WAKEUP_STRING, STR_LENGTH);
}

void receiveData(CFSocketRef s, 
				 CFSocketCallBackType type, 
				 CFDataRef address, 
				 const void *data, 
				 void *info)  
{
    CFDataRef df = (CFDataRef) data;
    CFIndex len = CFDataGetLength(df);
    if(len <= 0) return;
    
    UInt8 buffer[len];
	CFRange range = CFRangeMake(0,len);
	
    CFDataGetBytes(df, range, buffer);
	DisplayText((char *)buffer, len - 1);
}

void acceptConnection(CFSocketRef s, 
					  CFSocketCallBackType type, 
					  CFDataRef address, 
					  const void *data, 
					  void *info)  
{
    CFSocketNativeHandle csock = 
	*(CFSocketNativeHandle *)data;
    CFSocketRef sn;
    CFRunLoopSourceRef source;
    
    sn = CFSocketCreateWithNative(NULL, csock,
								  kCFSocketDataCallBack,
								  receiveData, 
								  NULL);
    source = CFSocketCreateRunLoopSource(NULL, sn, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), source,
					   kCFRunLoopDefaultMode);
    CFRelease(source);
    CFRelease(sn);
}

int main(int argc, char *argv[ ])
{	
	//register wake callback
	LCPDRegisterWakeCallback(WakeCallback, CFRunLoopGetCurrent());
	
	//output text to pole display
	DisplayText(LOGO_STRING, strlen(LOGO_STRING));

	// register a timer that fires every 15 seconds
	CFTimeInterval interval= 15.0;
	CFRunLoopTimerRef timer= CFRunLoopTimerCreate(NULL, 0, interval, 0, 0, IdleCallback, NULL);
	CFRunLoopAddTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopDefaultMode);
	CFRelease(timer);

    struct sockaddr_in sin;
    int sock, yes = 1;
    CFSocketRef s;
    CFRunLoopSourceRef source;
    
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(1888);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, 
			   &yes, sizeof(yes));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, 
			   &yes, sizeof(yes));
    bind(sock, (struct sockaddr *)&sin, sizeof(sin));
    listen(sock, 5);
    
    s = CFSocketCreateWithNative(NULL, sock, 
								 kCFSocketAcceptCallBack, 
								 acceptConnection, 
								 NULL);
    
    source = CFSocketCreateRunLoopSource(NULL, s, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), source,
					   kCFRunLoopDefaultMode);
    CFRelease(source);
    CFRelease(s);

	//receive wake notification
	CFRunLoopRun();
	
	//deregister callback
	LCPDRegisterWakeCallback(NULL, NULL);
	
	printf("\n quit the test program !\n");
	return(0);
}