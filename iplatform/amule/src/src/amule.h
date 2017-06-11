//
// This file is part of the aMule Project.
//
// Copyright (c) 2003-2011 aMule Team ( admin@amule.org / http://www.amule.org )
// Copyright (c) 2002-2011 Merkur ( devs@emule-project.net / http://www.emule-project.net )
//
// Any parts of this program derived from the xMule, lMule or eMule project,
// or contributed by third-party developers are copyrighted by their
// respective authors.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA
//


#ifndef AMULE_H
#define AMULE_H


#include <wx/app.h>		// Needed for wxApp
#include <wx/intl.h>		// Needed for wxLocale

#ifdef ENABLE_UBUS_RPC
#include "updownclient.h"	// Needed for CUpDownClient
#include "NetworkFunctions.h"
#include <wx/thread.h>
extern "C" {
#include <libubox/blobmsg_json.h>
#include <libubus.h>
}
#endif //ENABLE_UBUS_RPC

#include "Types.h"		// Needed for int32, uint16 and uint64
#include <map>
#ifndef __WXMSW__
	#include <signal.h>
//	#include <wx/unix/execute.h>
#endif // __WXMSW__


class CAbstractFile;
class CKnownFile;
class ExternalConn;
class CamuleDlg;
class CPreferences;
class CDownloadQueue;
class CUploadQueue;
class CServerConnect;
class CSharedFileList;
class CServer;
#ifdef ENABLE_FRIEND
class CFriend;
#endif
class CMD4Hash;
class CServerList;
class CListenSocket;
class CClientList;
class CKnownFileList;
#ifdef ENABLE_CANCEL_LIST 
class CCanceledFileList;
#endif
#ifdef ENABLE_SEARCH
class CSearchList;
#endif
class CClientCreditsList;
#ifdef ENABLE_FRIEND
class CFriendList;
#endif
class CClientUDPSocket;
class CIPFilter;
class UploadBandwidthThrottler;
#ifdef ENABLE_UPNP
class CUPnPControlPoint;
class CUPnPPortMapping;
#endif
class CStatistics;
class wxSocketEvent;
class wxCommandEvent;
class wxCloseEvent;
class wxFFileOutputStream;
class CTimer;
class CTimerEvent;
class wxSingleInstanceChecker;
class CHashingEvent;
class CMuleInternalEvent;
class CCompletionEvent;
class CAllocFinishedEvent;
class wxExecuteData;
class CLoggingEvent;


namespace MuleNotify {
	class CMuleGUIEvent;
}


using MuleNotify::CMuleGUIEvent;


#ifdef AMULE_DAEMON
#define AMULE_APP_BASE wxAppConsole
//zengwei mod 
//#define CORE_TIMER_PERIOD 300
#define CORE_TIMER_PERIOD 100
#else
#define AMULE_APP_BASE wxApp
#define CORE_TIMER_PERIOD 100
#endif

#define CONNECTED_ED2K (1<<0)
#define CONNECTED_KAD_NOT (1<<1)
#define CONNECTED_KAD_OK (1<<2)
#define CONNECTED_KAD_FIREWALLED (1<<3)

//zengwei add
#define AMULE_LOGFILE_SIZE (5*1024*1024)

#ifdef ENABLE_UBUS_RPC
#define UBUS_DEBUG(fmt, args...) \
	//printf("[UBUS_DEBUG](%s) %05d: "fmt"\r\n", __FUNCTION__, __LINE__, ##args)

class CamuleUbusRpc : public wxThread 
{
public:
    CamuleUbusRpc();
    ~CamuleUbusRpc();
    void EndThread();
	void AmuleUbusDownloadFin(CMD4Hash hash);
private:
	enum {RET_OK = 0, RET_ERR};
	
	static struct ubus_object m_ubus_object;
	static struct ubus_context *context;
	static struct blob_buf b;
		
	/* callback function */
	static struct ubus_method amule_ubus_object_methods[];
	static struct ubus_object_type amule_ubus_object_type;

	/* 
	 * AmuleUbusAddLinkHandle 
	 * callback function params 
	 */
	static struct blobmsg_policy amule_add_link_policy[2];
	
	/* 
	 * AmuleUbusGetDlStatusHandle
	 * AmuleUbusFileDlResumeHandle
	 * AmuleUbusFileDlPauseHandle 
	 * AmuleUbusFileDlDeleteHandle
	 * callback function params 
	 */
	static struct blobmsg_policy amule_file_op_policy[1];
	
	/* 
	 * AmuleUbusServerAddHandle
	 * AmuleUbusServerConHandle 
	 * callback function params 
	 */
	static struct blobmsg_policy amule_server_op_policy[2];
	
	/* 
	 * AmuleUbusServerAddHandle
	 * AmuleUbusServerRemoveHandle
	 * AmuleUbusServerConHandle 
	 * callback function params 
	 */
	static struct blobmsg_policy amule_pref_policy[2];

	/*
	 * callback for ubus, handle ubus client request: shutdown amule app.
	 */
	static int AmuleUbusShutdownHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 *callback for ubus, handle ubus client request: add server/file link.
	 */
	static int AmuleUbusAddLinkHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: show all download files.
	 */
	static int AmuleUbusShowDlHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	
	/*
	 * callback for ubus, handle ubus client request: get download files status.
	 */
	static int AmuleUbusGetDlStatusHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: check download files completed.
	 */
	static int AmuleUbusIsFileCompletedHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: resume file download.
	 */
	static int AmuleUbusFileDlResumeHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: stop file download.
	 */
	static int AmuleUbusFileDlPauseHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: delete file download.
	 */
	static int AmuleUbusFileDlDeleteHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: delete file download.
	 */
	static int AmuleUbusFileDlDeleteAllHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: add server.
	 */
	static int AmuleUbusServerAddHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: remove all servers.
	 */
	static int AmuleUbusServerRemoveAllHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: remove server.
	 */
	static int AmuleUbusServerRemoveHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: connect server.
	 */
	static int AmuleUbusServerConHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: disconnect server.
	 */
	static int AmuleUbusServerDisconHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: server status.
	 */
	static int AmuleUbusServerStatusHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: server list.
	 */
	static int AmuleUbusServerListHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: set preference values.
	 */
	static int AmuleUbusSetPrefHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);
	/*
	 * callback for ubus, handle ubus client request: get preference values.
	 */
	static int AmuleUbusGetPrefHandle(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method, struct blob_attr *msg);

	static void AmuleUbusReconnect(struct uloop_timeout *timeout);
	static void AmuleUbusConnectionLost(struct ubus_context *timeout);

    void* Entry();
};

#endif //ENABLE_UBUS_RPC

// Base class common to amule, aamuled and amulegui
class CamuleAppCommon
{
private:
	// Used to detect a previous running instance of aMule
	wxSingleInstanceChecker*	m_singleInstance;

	bool		CheckPassedLink(const wxString &in, wxString &out, int cat);
protected:
	char		*strFullMuleVersion;
	char		*strOSDescription;
	wxString	OSType;
	bool		enable_daemon_fork;
	bool		ec_config;
	bool		m_skipConnectionDialog;
	bool		m_geometryEnabled;
	wxString	m_geometryString;
	wxString	m_logFile;
	wxString	m_appName;
	wxString	m_PidFile;

	bool		InitCommon(int argc, wxChar ** argv);
	void		RefreshSingleInstanceChecker();
	bool		CheckMuleDirectory(const wxString& desc, const class CPath& directory, const wxString& alternative, class CPath& outDir);
public:
	wxString	ConfigDir;
	wxString	m_configFile;

	CamuleAppCommon();
	~CamuleAppCommon();
	void		AddLinksFromFile();
	// URL functions
	wxString	CreateMagnetLink(const CAbstractFile *f);
	wxString	CreateED2kLink(const CAbstractFile* f, bool add_source = false, bool use_hostname = false, bool addcryptoptions = false);
	wxString	CreateED2kAICHLink(const CKnownFile* f);
	// Who am I ?
#ifdef AMULE_DAEMON
	bool		IsDaemon() const { return true; }
#else
	bool		IsDaemon() const { return false; }
#endif

#ifdef CLIENT_GUI
	bool		IsRemoteGui() const { return true; }
#else
	bool		IsRemoteGui() const { return false; }
#endif

	const wxString&	GetMuleAppName() const { return m_appName; }
	const wxString	GetFullMuleVersion() const;
};

class CamuleApp : public AMULE_APP_BASE, public CamuleAppCommon
{
private:
	enum APPState {
		APP_STATE_RUNNING = 0,
		APP_STATE_SHUTTINGDOWN,
		APP_STATE_STARTING
	};

public:
	CamuleApp();
	virtual	 ~CamuleApp();

	virtual bool	OnInit();
	int		OnExit();
#if wxUSE_ON_FATAL_EXCEPTION
	void		OnFatalException();
#endif
	bool		ReinitializeNetwork(wxString *msg);

	// derived classes may override those
	virtual int InitGui(bool geometry_enable, wxString &geometry_string);

	// Socket handlers
	void ListenSocketHandler(wxSocketEvent& event);
	void ServerSocketHandler(wxSocketEvent& event);
	void UDPSocketHandler(wxSocketEvent& event);

	virtual int ShowAlert(wxString msg, wxString title, int flags) = 0;

	// Barry - To find out if app is running or shutting/shut down
	bool IsRunning() const { return (m_app_state == APP_STATE_RUNNING); }
	bool IsOnShutDown() const { return (m_app_state == APP_STATE_SHUTTINGDOWN); }

	// Check ED2K and Kademlia state
	bool IsFirewalled() const;
	// Are we connected to at least one network?
	bool IsConnected() const;
	// Connection to ED2K
	bool IsConnectedED2K() const;

#ifdef ENABLE_KAD
	// What about Kad? Is it running?
	bool IsKadRunning() const;
	// Connection to Kad
	bool IsConnectedKad() const;
	// Check Kad state (TCP)
	bool IsFirewalledKad() const;
	// Check Kad state (UDP)
	bool IsFirewalledKadUDP() const;
	// Check Kad state (LAN mode)
	bool IsKadRunningInLanMode() const;
	// Kad stats
	uint32	GetKadUsers() const;
	uint32	GetKadFiles() const;
	uint32	GetKadIndexedSources() const;
	uint32	GetKadIndexedKeywords() const;
	uint32	GetKadIndexedNotes() const;
	uint32	GetKadIndexedLoad() const;
	// True IP of machine
	uint32	GetKadIPAdress() const;
	// Buddy status
	uint8	GetBuddyStatus() const;
	uint32	GetBuddyIP() const;
	uint32	GetBuddyPort() const;
#endif //ENABLE_KAD

	// Check if we should callback this client
	bool CanDoCallback(uint32 clientServerIP, uint16 clientServerPort);

	// Misc functions
#ifdef ENABLE_STAT
	void		OnlineSig(bool zero = false);
#endif
	void		Localize_mule();
	void		Trigger_New_version(wxString newMule);

	// shakraw - new EC code using wxSocketBase
	ExternalConn*	ECServerHandler;

	// return current (valid) public IP or 0 if unknown
	// If ignorelocal is true, don't use m_localip
	uint32	GetPublicIP(bool ignorelocal = false) const;
	void		SetPublicIP(const uint32 dwIP);

	uint32	GetED2KID() const;
	uint32	GetID() const;

//zengwei add debug
	void Print_DownloadRate();
	void Print_StatInfo();
	bool m_enableRatePrint;

	// Other parts of the interface and such
	CPreferences*		glob_prefs;
	CDownloadQueue*		downloadqueue;
	CUploadQueue*		uploadqueue;
	CServerConnect*		serverconnect;
	CSharedFileList*	sharedfiles;
	CServerList*		serverlist;
	CListenSocket*		listensocket;
	CClientList*		clientlist;
	CKnownFileList*		knownfiles;
#ifdef ENABLE_CANCEL_LIST 
	CCanceledFileList*	canceledfiles;
#endif
#ifdef ENABLE_SEARCH
	CSearchList*		searchlist;
#endif
	CClientCreditsList*	clientcredits;
#ifdef ENABLE_FRIEND
	CFriendList*		friendlist;
#endif
	CClientUDPSocket*	clientudp;
	CStatistics*		m_statistics;
	CIPFilter*		ipfilter;
	UploadBandwidthThrottler* uploadBandwidthThrottler;

#ifdef ENABLE_UBUS_RPC
	CamuleUbusRpc* amuleUbusRpc;
#endif //ENABLE_UBUS_RPC

#ifdef ENABLE_UPNP
	CUPnPControlPoint*	m_upnp;
	std::vector<CUPnPPortMapping> m_upnpMappings;
#endif
	wxLocale m_locale;

	void ShutDown();

	wxString GetLog(bool reset = false);
	wxString GetServerLog(bool reset = false);
	wxString GetDebugLog(bool reset = false);

	bool AddServer(CServer *srv, bool fromUser = false);
	void AddServerMessageLine(wxString &msg);
#ifdef __DEBUG__
	void AddSocketDeleteDebug(uint32 socket_pointer, uint32 creation_time);
#endif
	void SetOSFiles(const wxString new_path);

	const wxString& GetOSType() const { return OSType; }

	void ShowUserCount();

	void ShowConnectionState(bool forceUpdate = false);

#ifdef ENABLE_KAD
	void StartKad();
	void StopKad();

	/** Bootstraps kad from the specified IP (must be in hostorder). */
	void BootstrapKad(uint32 ip, uint16 port);
	/** Updates the nodes.dat file from the specified url. */
	void UpdateNotesDat(const wxString& str);
#endif //ENABLE_KAD

	void DisconnectED2K();

	bool CryptoAvailable() const;

	//zengwei add app start time
	uint64	GetStartTime()	{ return m_uptime; }
	uint64	SetStartTime(uint64 Value)	{ m_uptime = Value; }

protected:
	
	//zengwei add app start time
	uint64 m_uptime;

#ifdef __WXDEBUG__
	/**
	 * Handles asserts in a thread-safe manner.
	 */
	virtual void OnAssertFailure(const wxChar* file, int line,
		const wxChar* func, const wxChar* cond, const wxChar* msg);
#endif

	void OnUDPDnsDone(CMuleInternalEvent& evt);
	void OnSourceDnsDone(CMuleInternalEvent& evt);
	void OnServerDnsDone(CMuleInternalEvent& evt);

	void OnTCPTimer(CTimerEvent& evt);
	void OnCoreTimer(CTimerEvent& evt);

	void OnFinishedHashing(CHashingEvent& evt);
	void OnFinishedAICHHashing(CHashingEvent& evt);
	void OnFinishedCompletion(CCompletionEvent& evt);
	void OnFinishedAllocation(CAllocFinishedEvent& evt);
	void OnFinishedHTTPDownload(CMuleInternalEvent& evt);
	void OnHashingShutdown(CMuleInternalEvent&);
	void OnNotifyEvent(CMuleGUIEvent& evt);

	void SetTimeOnTransfer();

	APPState m_app_state;

	wxString m_emulesig_path;
	wxString m_amulesig_path;

	uint32 m_dwPublicIP;

	long webserver_pid;

	wxString server_msg;

	CTimer* core_timer;

private:
	virtual void OnUnhandledException();

	void CheckNewVersion(uint32 result);

	uint32 m_localip;
};


#ifndef AMULE_DAEMON


class CamuleGuiBase {
public:
	CamuleGuiBase();
	virtual	 ~CamuleGuiBase();

	wxString	m_FrameTitle;
	CamuleDlg*	amuledlg;
	int		m_FileDetailDialogActive;

	bool CopyTextToClipboard( wxString strText );
	void ResetTitle();

	virtual int InitGui(bool geometry_enable, wxString &geometry_string);
	virtual int ShowAlert(wxString msg, wxString title, int flags);

	void AddGuiLogLine(const wxString& line);
protected:
	/**
	 * This list is used to contain log messages that are to be displayed
	 * on the GUI, when it is currently impossible to do so. This is in order
	 * to allows us to queue messages till after the dialog has been created.
	 */
	std::list<wxString> m_logLines;
};


#ifndef CLIENT_GUI


class CamuleGuiApp : public CamuleApp, public CamuleGuiBase
{

    virtual int InitGui(bool geometry_enable, wxString &geometry_string);

	int OnExit();
	bool OnInit();

public:

	virtual int ShowAlert(wxString msg, wxString title, int flags);

	void ShutDown(wxCloseEvent &evt);

	wxString GetLog(bool reset = false);
	wxString GetServerLog(bool reset = false);
	void AddServerMessageLine(wxString &msg);
	DECLARE_EVENT_TABLE()
};


DECLARE_APP(CamuleGuiApp)
extern CamuleGuiApp *theApp;


#else /* !CLIENT_GUI */


#include "amule-remote-gui.h"


#endif // CLIENT_GUI


#define CALL_APP_DATA_LOCK


#else /* ! AMULE_DAEMON */

// wxWidgets 2.8 requires special code for event handling and sockets.
// 2.9 doesn't, so standard event loop and sockets can be used
//
// Windows: aMuled compiles with 2.8 (without the special code),
// but works only with 2.9

#if !wxCHECK_VERSION(2, 9, 0)
	#ifdef __WXMSW__
		// MSW: can't run amuled with 2.8 anyway, just get it compiled
		#define AMULED_DUMMY
	#else
		#define AMULED28
	#endif
#endif

#ifdef AMULED28
#include <wx/socket.h>

class CSocketSet;


class CAmuledGSocketFuncTable : public GSocketGUIFunctionsTable
{
private:
	CSocketSet *m_in_set, *m_out_set;

	wxMutex m_lock;
public:
	CAmuledGSocketFuncTable();

	void AddSocket(GSocket *socket, GSocketEvent event);
	void RemoveSocket(GSocket *socket, GSocketEvent event);
	void RunSelect();

	virtual bool OnInit();
	virtual void OnExit();
	virtual bool CanUseEventLoop();
	virtual bool Init_Socket(GSocket *socket);
	virtual void Destroy_Socket(GSocket *socket);
	virtual void Install_Callback(GSocket *socket, GSocketEvent event);
	virtual void Uninstall_Callback(GSocket *socket, GSocketEvent event);
	virtual void Enable_Events(GSocket *socket);
	virtual void Disable_Events(GSocket *socket);
};


#endif // AMULED28

// no AppTraits used on Windows
#ifndef __WXMSW__

typedef std::map<int, class wxEndProcessData *> EndProcessDataMap;

#include <wx/apptrait.h>

class CDaemonAppTraits : public wxConsoleAppTraits
{
private:
	struct sigaction m_oldSignalChildAction;
	struct sigaction m_newSignalChildAction;

#ifdef AMULED28
	CAmuledGSocketFuncTable *m_table;
	wxMutex m_lock;
	std::list<wxObject *> m_sched_delete;
public:
	CDaemonAppTraits(CAmuledGSocketFuncTable *table);
	virtual GSocketGUIFunctionsTable* GetSocketGUIFunctionsTable();
	virtual void ScheduleForDestroy(wxObject *object);
	virtual void RemoveFromPendingDelete(wxObject *object);

	void DeletePending();
#else	// AMULED28
public:
	CDaemonAppTraits();
#endif	// !AMULED28

	virtual int WaitForChild(wxExecuteData& execData);

#if defined(__WXMAC__) && !wxCHECK_VERSION(2, 9, 0)
	virtual wxStandardPathsBase& GetStandardPaths();
#endif
};

void OnSignalChildHandler(int signal, siginfo_t *siginfo, void *ucontext);
pid_t AmuleWaitPid(pid_t pid, int *status, int options, wxString *msg);
#endif // __WXMSW__


class CamuleDaemonApp : public CamuleApp
{
private:
#ifdef AMULED28
	bool m_Exit;
	CAmuledGSocketFuncTable *m_table;
#endif
	bool OnInit();
	int OnRun();
	int OnExit();

	virtual int InitGui(bool geometry_enable, wxString &geometry_string);

#ifndef __WXMSW__
	struct sigaction m_oldSignalChildAction;
	struct sigaction m_newSignalChildAction;
public:
	wxAppTraits *CreateTraits();
#endif // __WXMSW__

public:

#ifdef AMULED28
	CamuleDaemonApp();

	void ExitMainLoop() { m_Exit = true; }
#endif

#ifdef AMULED_DUMMY
	void ExitMainLoop() {}
#endif

	bool CopyTextToClipboard(wxString strText);

	virtual int ShowAlert(wxString msg, wxString title, int flags);

	DECLARE_EVENT_TABLE()
};

DECLARE_APP(CamuleDaemonApp)
extern CamuleDaemonApp *theApp;

#endif /* ! AMULE_DAEMON */

#endif // AMULE_H
// File_checked_for_headers