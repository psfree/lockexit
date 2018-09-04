#include <switch.h>
extern __attribute__((weak)) u32 __nx_applet_type;
static Service g_appletSrv;
static Service g_appletProxySession;
static Service g_appletISelfController;

static Result _appletGetHandle(Service* srv, Handle* handle_out, u64 cmd_id) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = cmd_id;

    Result rc = serviceIpcDispatch(srv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            *handle_out = r.Handles[0];
        }
    }

    return rc;
}

static Result _appletGetSession(Service* srv, Service* srv_out, u64 cmd_id) {
    Result rc;
    Handle handle;

    rc = _appletGetHandle(srv, &handle, cmd_id);

    if (R_SUCCEEDED(rc)) {
        serviceCreate(srv_out, handle);
    }

    return rc;
}

static Result _appletGetSessionProxy(Service* srv_out, u64 cmd_id, Handle prochandle, u8 *AppletAttribute) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
        u64 reserved;
    } *raw;

    ipcSendPid(&c);
    ipcSendHandleCopy(&c, prochandle);
    if (AppletAttribute) ipcAddSendBuffer(&c, AppletAttribute, 0x80, 0);

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = cmd_id;
    raw->reserved = 0;

    Result rc = serviceIpcDispatch(&g_appletSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            serviceCreate(srv_out, r.Handles[0]);
        }
    }

    return rc;
}

Result initstuff(void) {
	Result rc = 0;
	
    switch (__nx_applet_type) {
    case AppletType_Default:
        __nx_applet_type = AppletType_Application;
        // Fallthrough.
    case AppletType_Application:
        rc = smGetService(&g_appletSrv, "appletOE");
        break;
    default:
        rc = smGetService(&g_appletSrv, "appletAE");
        break;
    }

    if (R_SUCCEEDED(rc)) {
        #define AM_BUSY_ERROR 0x19280

        do {
            u32 cmd_id;

            switch(__nx_applet_type) {
            case AppletType_Application:       cmd_id = 0;   break;
            case AppletType_SystemApplet:      cmd_id = 100; break;
            case AppletType_LibraryApplet:     cmd_id = 200; break;
            case AppletType_OverlayApplet:     cmd_id = 300; break;
            case AppletType_SystemApplication: cmd_id = 350; break;
            // TODO: Replace error code
            default: fatalSimple(MAKERESULT(Module_Libnx, LibnxError_AppletCmdidNotFound));
            }

            rc = _appletGetSessionProxy(&g_appletProxySession, cmd_id, CUR_PROCESS_HANDLE, NULL);

            if (rc == AM_BUSY_ERROR) {
                svcSleepThread(10000000);
            }

        } while (rc == AM_BUSY_ERROR);
    }
	rc = _appletGetSession(&g_appletProxySession, &g_appletISelfController, 1);
	return 0;
}

static Result _appletCmdNoIO(Service* session, u64 cmd_id) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = cmd_id;

    Result rc = serviceIpcDispatch(session);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }

    return rc;
}

Result _appletSelfExit(void) {
    return _appletCmdNoIO(&g_appletISelfController, 0);
}

Result _appletLockExit(void) {
    return _appletCmdNoIO(&g_appletISelfController, 1);
}

Result _appletUnlockExit(void) {
    return _appletCmdNoIO(&g_appletISelfController, 2);
}
