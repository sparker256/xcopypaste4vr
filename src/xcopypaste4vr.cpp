#include "XPLMPlugin.h"
#include "XPLMUtilities.h"
#include <XPLMProcessing.h>

// #include <cstring>
// #include <cstdlib>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <ctime>
#include <cwchar>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>


#if IBM
    #include <unistd.h>
    #include <windows.h>
#endif

void send_ctrl_c();
void send_delayed_ctrl_c();
void send_ctrl_v();
void send_delayed_ctrl_v();
void send_pipe();
void send_delayed_pipe();

enum {SEND_CTRL_C_COMMAND, SEND_DELAYED_CTRL_C_COMMAND, SEND_CTRL_V_COMMAND, SEND_DELAYED_CTRL_V_COMMAND};

int xcopypaste4vrCommandCallback(
                                   XPLMCommandRef       inCommand,
                                   XPLMCommandPhase     inPhase,
                                   void *               inRefcon);



XPLMCommandRef send_ctrl_c_cmd;
XPLMCommandRef send_delayed_ctrl_c_cmd;
XPLMCommandRef send_ctrl_v_cmd;
XPLMCommandRef send_delayed_ctrl_v_cmd;


const int C = 0x43; //C key code
const int V = 0x56; //V key code

static float DelayedControlC_Callback(float inElapsed1, float inElapsed2, int cntr, void *ref);
static float DelayedControlV_Callback(float inElapsed1, float inElapsed2, int cntr, void *ref);

PLUGIN_API int XPluginStart(char * outName, char * outSig, char * outDesc) {
    // Plugin details
    strcpy(outName, "Xcopypaste4VR plugin");
    strcpy(outSig, "sparker.example.xcopypaste4vrplugin");
    strcpy(outDesc, "A plug-in that sends delayed copy & paste commands for use in VR");

    // You probably want this on
	XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);

    send_ctrl_c_cmd = XPLMCreateCommand("bgood/xcopypaste4vr/send_ctrl_c","Send Ctrl C");
    send_delayed_ctrl_c_cmd = XPLMCreateCommand("bgood/xcopypaste4vr/send_delayed_ctrl_c","Send Delayed Ctrl C");
    send_ctrl_v_cmd = XPLMCreateCommand("bgood/xcopypaste4vr/send_ctrl_v","Send Ctrl V");
    send_delayed_ctrl_v_cmd = XPLMCreateCommand("bgood/xcopypaste4vr/send_delayed_ctrl_v","Send Delayed_Ctrl V");


    XPLMRegisterCommandHandler(
                send_ctrl_c_cmd,
                xcopypaste4vrCommandCallback,
                true,
                (void *)SEND_CTRL_C_COMMAND);

    XPLMRegisterCommandHandler(
                send_delayed_ctrl_c_cmd,
                xcopypaste4vrCommandCallback,
                true,
                (void *)SEND_DELAYED_CTRL_C_COMMAND);

    XPLMRegisterCommandHandler(
                send_ctrl_v_cmd,
                xcopypaste4vrCommandCallback,
                true,
                (void *)SEND_CTRL_V_COMMAND);

    XPLMRegisterCommandHandler(
                send_delayed_ctrl_v_cmd,
                xcopypaste4vrCommandCallback,
                true,
                (void *)SEND_DELAYED_CTRL_V_COMMAND);

	return 1;
}

PLUGIN_API void	XPluginStop(void) {
}

PLUGIN_API void XPluginDisable(void) {
}

PLUGIN_API int XPluginEnable(void) {
	return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, intptr_t inMessage, void * inParam) {
}

void send_ctrl_c() {
   XPLMDebugString("xcopypaste4vr: Send Ctrl C Function\n");

# if IBM
    XPLMDebugString("xcopypaste4vr: Send Ctrl C Function in Windows\n");
    // Hold Control down and press C
    keybd_event(VK_LCONTROL, 0, KEYEVENTF_EXTENDEDKEY, 0);
    keybd_event(C, 0, KEYEVENTF_EXTENDEDKEY, 0);
    keybd_event(C, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_LCONTROL, 0, KEYEVENTF_KEYUP, 0);
#endif

}

void send_delayed_ctrl_c() {\
    XPLMSpeakString("Sending control C in 10 seconds");
    XPLMRegisterFlightLoopCallback(DelayedControlC_Callback, 10, NULL);
}

float DelayedControlC_Callback(float inElapsed1, float inElapsed2, int cntr, void *ref)
{
# if IBM
    XPLMSpeakString("Sending control C now");
    // Hold Control down and press C
    keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY, 0);
    keybd_event(C, 0, KEYEVENTF_EXTENDEDKEY, 0);
    keybd_event(C, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    return 0;
#endif

#if LIN
  XPLMSpeakString("Sending control C now");
  int ret_val;
  ret_val = system("xdotool key ctrl+c");
  if(0 != ret_val) {
    XPLMDebugString("Copy command failed. Do you have xdotool on your system?");
  }
  return 0;
#endif


}


void send_ctrl_v() {
    XPLMDebugString("xcopypaste4vr: Send Ctrl V Function\n");

# if IBM
    // Hold Control down and press V
    keybd_event(VK_LCONTROL, 0, KEYEVENTF_EXTENDEDKEY, 0);
    keybd_event(V, 0, KEYEVENTF_EXTENDEDKEY, 0);
    keybd_event(V, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_LCONTROL, 0, KEYEVENTF_KEYUP, 0);
#endif


}


void send_delayed_ctrl_v() {
    XPLMSpeakString("Sending control V in 10 seconds");
    XPLMRegisterFlightLoopCallback(DelayedControlV_Callback, 10, NULL);
}


float DelayedControlV_Callback(float inElapsed1, float inElapsed2, int cntr, void *ref)
{
# if IBM
    XPLMSpeakString("Sending control V now");
    // Hold Control down and press V
    keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY, 0);
    keybd_event(V, 0, KEYEVENTF_EXTENDEDKEY, 0);
    keybd_event(V, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    return 0;
#endif

#if LIN
  XPLMSpeakString("Sending control V now");
  int ret_val;
  ret_val = system("xdotool key ctrl+v");
  if(0 != ret_val) {
    XPLMDebugString("Paste command failed. Do you have xdotool on your system?");
  }
  return 0;
#endif

}


int xcopypaste4vrCommandCallback(XPLMCommandRef       inCommand,
                      XPLMCommandPhase     inPhase,
                      void *               inRefcon)
{
    (void) inCommand;
    //(void) inPhase;
    //(void) inRefcon;

    if (inPhase == xplm_CommandBegin) {
        switch((intptr_t)inRefcon){
        case SEND_CTRL_C_COMMAND:
            XPLMDebugString("xcopypaste4vr: Send Ctrl C Command\n");
            send_ctrl_c();
            break;
        case SEND_DELAYED_CTRL_C_COMMAND:
            XPLMDebugString("xcopypaste4vr: Send Delayed Ctrl C Command\n");
            send_delayed_ctrl_c();
            break;
        case SEND_CTRL_V_COMMAND:
            XPLMDebugString("xcopypaste4vr: Send Ctrl V Command\n");
            send_ctrl_v();
            break;
        case SEND_DELAYED_CTRL_V_COMMAND:
            XPLMDebugString("xcopypaste4vr: Send Delayed Ctrl V Command\n");
            send_delayed_ctrl_v();
            break;
        }
    }
    return 1;
}
