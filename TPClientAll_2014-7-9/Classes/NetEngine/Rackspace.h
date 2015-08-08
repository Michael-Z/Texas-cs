////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 Rackspace.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#include "NativeFeatureIncludes.h"

#if _VENET_SUPPORT_Rackspace==1 && _VENET_SUPPORT_TCPInterface==1

#include "Export.h"
#include "DS_List.h"
#include "VENetTypes.h"
#include "DS_Queue.h"
#include "VEString.h"
#pragma once

namespace VENet
{

class TCPInterface;
struct Packet;

enum RackspaceEventType
{
    RET_Success_200,
    RET_Success_201,
    RET_Success_202,
    RET_Success_203,
    RET_Success_204,
    RET_Cloud_Servers_Fault_500,
    RET_Service_Unavailable_503,
    RET_Unauthorized_401,
    RET_Bad_Request_400,
    RET_Over_Limit_413,
    RET_Bad_Media_Type_415,
    RET_Item_Not_Found_404,
    RET_Build_In_Progress_409,
    RET_Resize_Not_Allowed_403,
    RET_Connection_Closed_Without_Reponse,
    RET_Unknown_Failure,
};

enum RackspaceOperationType
{
    RO_CONNECT_AND_AUTHENTICATE,
    RO_LIST_SERVERS,
    RO_LIST_SERVERS_WITH_DETAILS,
    RO_CREATE_SERVER,
    RO_GET_SERVER_DETAILS,
    RO_UPDATE_SERVER_NAME_OR_PASSWORD,
    RO_DELETE_SERVER,
    RO_LIST_SERVER_ADDRESSES,
    RO_SHARE_SERVER_ADDRESS,
    RO_DELETE_SERVER_ADDRESS,
    RO_REBOOT_SERVER,
    RO_REBUILD_SERVER,
    RO_RESIZE_SERVER,
    RO_CONFIRM_RESIZED_SERVER,
    RO_REVERT_RESIZED_SERVER,
    RO_LIST_FLAVORS,
    RO_GET_FLAVOR_DETAILS,
    RO_LIST_IMAGES,
    RO_CREATE_IMAGE,
    RO_GET_IMAGE_DETAILS,
    RO_DELETE_IMAGE,
    RO_LIST_SHARED_IP_GROUPS,
    RO_LIST_SHARED_IP_GROUPS_WITH_DETAILS,
    RO_CREATE_SHARED_IP_GROUP,
    RO_GET_SHARED_IP_GROUP_DETAILS,
    RO_DELETE_SHARED_IP_GROUP,

    RO_NONE,
};

class VE_DLL_EXPORT RackspaceEventCallback
{
public:
    RackspaceEventCallback() {}
    virtual ~RackspaceEventCallback() {}
    virtual void OnAuthenticationResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnListServersResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnListServersWithDetailsResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnCreateServerResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnGetServerDetails(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnUpdateServerNameOrPassword(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnDeleteServer(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnListServerAddresses(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnShareServerAddress(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnDeleteServerAddress(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnRebootServer(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnRebuildServer(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnResizeServer(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnConfirmResizedServer(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnRevertResizedServer(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnListFlavorsResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnGetFlavorDetailsResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnListImagesResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnCreateImageResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnGetImageDetailsResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnDeleteImageResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnListSharedIPGroups(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnListSharedIPGroupsWithDetails(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnCreateSharedIPGroup(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnGetSharedIPGroupDetails(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;
    virtual void OnDeleteSharedIPGroup(RackspaceEventType eventType, const char *htmlAdditionalInfo)=0;

    virtual void OnConnectionAttemptFailure(RackspaceOperationType operationType, const char *url)=0;
};

class VE_DLL_EXPORT RackspaceEventCallback_Default : public RackspaceEventCallback
{
public:
    virtual void ExecuteDefault(const char *callbackName, RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        (void) callbackName;
        (void) eventType;
        (void) htmlAdditionalInfo;
    }

    virtual void OnAuthenticationResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnAuthenticationResult", eventType, htmlAdditionalInfo);
    }
    virtual void OnListServersResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnListServersResult", eventType, htmlAdditionalInfo);
    }
    virtual void OnListServersWithDetailsResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnListServersWithDetailsResult", eventType, htmlAdditionalInfo);
    }
    virtual void OnCreateServerResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnCreateServerResult", eventType, htmlAdditionalInfo);
    }
    virtual void OnGetServerDetails(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnGetServerDetails", eventType, htmlAdditionalInfo);
    }
    virtual void OnUpdateServerNameOrPassword(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnUpdateServerNameOrPassword", eventType, htmlAdditionalInfo);
    }
    virtual void OnDeleteServer(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnDeleteServer", eventType, htmlAdditionalInfo);
    }
    virtual void OnListServerAddresses(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnListServerAddresses", eventType, htmlAdditionalInfo);
    }
    virtual void OnShareServerAddress(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnShareServerAddress", eventType, htmlAdditionalInfo);
    }
    virtual void OnDeleteServerAddress(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnDeleteServerAddress", eventType, htmlAdditionalInfo);
    }
    virtual void OnRebootServer(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnRebootServer", eventType, htmlAdditionalInfo);
    }
    virtual void OnRebuildServer(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnRebuildServer", eventType, htmlAdditionalInfo);
    }
    virtual void OnResizeServer(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnResizeServer", eventType, htmlAdditionalInfo);
    }
    virtual void OnConfirmResizedServer(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnConfirmResizedServer", eventType, htmlAdditionalInfo);
    }
    virtual void OnRevertResizedServer(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnRevertResizedServer", eventType, htmlAdditionalInfo);
    }
    virtual void OnListFlavorsResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnListFlavorsResult", eventType, htmlAdditionalInfo);
    }
    virtual void OnGetFlavorDetailsResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnGetFlavorDetailsResult", eventType, htmlAdditionalInfo);
    }
    virtual void OnListImagesResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnListImagesResult", eventType, htmlAdditionalInfo);
    }
    virtual void OnCreateImageResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnCreateImageResult", eventType, htmlAdditionalInfo);
    }
    virtual void OnGetImageDetailsResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnGetImageDetailsResult", eventType, htmlAdditionalInfo);
    }
    virtual void OnDeleteImageResult(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnDeleteImageResult", eventType, htmlAdditionalInfo);
    }
    virtual void OnListSharedIPGroups(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnListSharedIPGroups", eventType, htmlAdditionalInfo);
    }
    virtual void OnListSharedIPGroupsWithDetails(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnListSharedIPGroupsWithDetails", eventType, htmlAdditionalInfo);
    }
    virtual void OnCreateSharedIPGroup(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnCreateSharedIPGroup", eventType, htmlAdditionalInfo);
    }
    virtual void OnGetSharedIPGroupDetails(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnGetSharedIPGroupDetails", eventType, htmlAdditionalInfo);
    }
    virtual void OnDeleteSharedIPGroup(RackspaceEventType eventType, const char *htmlAdditionalInfo)
    {
        ExecuteDefault("OnDeleteSharedIPGroup", eventType, htmlAdditionalInfo);
    }

    virtual void OnConnectionAttemptFailure(RackspaceOperationType operationType, const char *url)
    {
        (void) operationType;
        (void) url;
    }
};

class VE_DLL_EXPORT Rackspace
{
public:
    Rackspace();
    ~Rackspace();

    SystemAddress Authenticate(TCPInterface *_tcpInterface, const char *_authenticationURL, const char *_rackspaceCloudUsername, const char *_apiAccessKey);

    void ListServers(void);

    void ListServersWithDetails(void);

    void CreateServer(VENet::VEString name, VENet::VEString imageId, VENet::VEString flavorId);

    void GetServerDetails(VENet::VEString serverId);

    void UpdateServerNameOrPassword(VENet::VEString serverId, VENet::VEString newName, VENet::VEString newPassword);

    void DeleteServer(VENet::VEString serverId);

    void ListServerAddresses(VENet::VEString serverId);

    void ShareServerAddress(VENet::VEString serverId, VENet::VEString ipAddress);

    void DeleteServerAddress(VENet::VEString serverId, VENet::VEString ipAddress);

    void RebootServer(VENet::VEString serverId, VENet::VEString rebootType);

    void RebuildServer(VENet::VEString serverId, VENet::VEString imageId);

    void ResizeServer(VENet::VEString serverId, VENet::VEString flavorId);

    void ConfirmResizedServer(VENet::VEString serverId);

    void RevertResizedServer(VENet::VEString serverId);

    void ListFlavors(void);

    void GetFlavorDetails(VENet::VEString flavorId);

    void ListImages(void);

    void CreateImage(VENet::VEString serverId, VENet::VEString imageName);

    void GetImageDetails(VENet::VEString imageId);

    void DeleteImage(VENet::VEString imageId);

    void ListSharedIPGroups(void);

    void ListSharedIPGroupsWithDetails(void);

    void CreateSharedIPGroup(VENet::VEString name, VENet::VEString optionalServerId);
    void GetSharedIPGroupDetails(VENet::VEString groupId);
    void DeleteSharedIPGroup(VENet::VEString groupId);

    void AddEventCallback(RackspaceEventCallback *callback);
    void RemoveEventCallback(RackspaceEventCallback *callback);
    void ClearEventCallbacks(void);

    void OnReceive(Packet *packet);

    void OnClosedConnection(SystemAddress systemAddress);

    static const char * EventTypeToString(RackspaceEventType eventType);

    void AddOperation(RackspaceOperationType type, VENet::VEString httpCommand, VENet::VEString operation, VENet::VEString xml);
protected:

    DataStructures::List<RackspaceEventCallback*> eventCallbacks;

    struct RackspaceOperation
    {
        RackspaceOperationType type;
        SystemAddress connectionAddress;
        bool isPendingAuthentication;
        VENet::VEString incomingStream;
        VENet::VEString httpCommand;
        VENet::VEString operation;
        VENet::VEString xml;
    };

    TCPInterface *tcpInterface;

    DataStructures::List<RackspaceOperation> operations;
    bool HasOperationOfType(RackspaceOperationType t);
    unsigned int GetOperationOfTypeIndex(RackspaceOperationType t);

    VENet::VEString serverManagementURL;
    VENet::VEString serverManagementDomain;
    VENet::VEString serverManagementPath;
    VENet::VEString storageURL;
    VENet::VEString storageDomain;
    VENet::VEString storagePath;
    VENet::VEString cdnManagementURL;
    VENet::VEString cdnManagementDomain;
    VENet::VEString cdnManagementPath;

    VENet::VEString storageToken;
    VENet::VEString authToken;
    VENet::VEString rackspaceCloudUsername;
    VENet::VEString apiAccessKey;

    bool ExecuteOperation(RackspaceOperation &ro);
    void ReadLine(const char *data, const char *stringStart, VENet::VEString &output);
    bool ConnectToServerManagementDomain(RackspaceOperation &ro);


};

}

#endif

