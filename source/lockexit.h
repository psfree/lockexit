/**
 * @file applet.h
 * @brief Applet (applet) service IPC wrapper.
 * @author yellows8
 * @copyright libnx Authors
 */
#pragma once
#include <string.h>
#include <switch.h>
#include "atomics.h"

/// applet hook function.
typedef void (*AppletHookFn)(AppletHookType hook, void* param);

/// applet hook cookie.
typedef struct AppletHookCookie AppletHookCookie;

Result appletInitialize(void);
void appletExit(void);
Result appletGetAppletResourceUserId(u64 *out);

void appletNotifyRunning(u8 *out);
Result appletCreateManagedDisplayLayer(u64 *out);

Result appletGetDesiredLanguage(u64 *LanguageCode);

/**
 * @brief Controls whether screenshot-capture is allowed.
 * @param val 0 = disable, 1 = enable.
 */
Result appletSetScreenShotPermission(s32 val);

Result appletSetScreenShotImageOrientation(s32 val);

/**
 * @brief Processes the current applet status. Generally used within a main loop.
 * @return Whether the application should continue running.
 */
bool appletMainLoop(void);


/**
 * @brief Sets up an applet status hook.
 * @param cookie Hook cookie to use.
 * @param callback Function to call when applet's status changes.
 * @param param User-defined parameter to pass to the callback.
 */
void appletHook(AppletHookCookie* cookie, AppletHookFn callback, void* param);

/**
 * @brief Removes an applet status hook.
 * @param cookie Hook cookie to remove.
 */
void appletUnhook(AppletHookCookie* cookie);

/// These return state which is updated by appletMainLoop() when notifications are received.
u8  appletGetOperationMode(void);
u32 appletGetPerformanceMode(void);
u8  appletGetFocusState(void);


Result _appletDisallowToEnterSleep(void);
Result _appletAllowToEnterSleep(void);