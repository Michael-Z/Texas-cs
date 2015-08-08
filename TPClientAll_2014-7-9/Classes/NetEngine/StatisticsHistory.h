#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_StatisticsHistory==1

#pragma once

#include "PluginInterface2.h"
#include "VEMemoryOverride.h"
#include "NativeTypes.h"
#include "DS_List.h"
#include "VENetTypes.h"
#include "DS_OrderedList.h"
#include "VEString.h"
#include "DS_Queue.h"
#include "DS_Hash.h"
#include <float.h>

namespace VENet
{
class VEPeerInterface;

typedef double SHValueType;
#define SH_TYPE_MAX DBL_MAX

class VE_DLL_EXPORT StatisticsHistory
{
public:
    STATIC_FACTORY_DECLARATIONS(StatisticsHistory)

    enum SHErrorCode
    {
        SH_OK,
        SH_UKNOWN_OBJECT,
        SH_UKNOWN_KEY,
        SH_INVALID_PARAMETER,
    };

    enum SHSortOperation
    {
        SH_DO_NOT_SORT,

        SH_SORT_BY_RECENT_SUM_ASCENDING,
        SH_SORT_BY_RECENT_SUM_DESCENDING,
        SH_SORT_BY_LONG_TERM_SUM_ASCENDING,
        SH_SORT_BY_LONG_TERM_SUM_DESCENDING,
        SH_SORT_BY_RECENT_SUM_OF_SQUARES_ASCENDING,
        SH_SORT_BY_RECENT_SUM_OF_SQUARES_DESCENDING,
        SH_SORT_BY_RECENT_AVERAGE_ASCENDING,
        SH_SORT_BY_RECENT_AVERAGE_DESCENDING,
        SH_SORT_BY_LONG_TERM_AVERAGE_ASCENDING,
        SH_SORT_BY_LONG_TERM_AVERAGE_DESCENDING,
        SH_SORT_BY_RECENT_HIGHEST_ASCENDING,
        SH_SORT_BY_RECENT_HIGHEST_DESCENDING,
        SH_SORT_BY_RECENT_LOWEST_ASCENDING,
        SH_SORT_BY_RECENT_LOWEST_DESCENDING,
        SH_SORT_BY_LONG_TERM_HIGHEST_ASCENDING,
        SH_SORT_BY_LONG_TERM_HIGHEST_DESCENDING,
        SH_SORT_BY_LONG_TERM_LOWEST_ASCENDING,
        SH_SORT_BY_LONG_TERM_LOWEST_DESCENDING,
    };

    enum SHDataCategory
    {
        DC_DISCRETE,

        DC_CONTINUOUS
    };

    struct TimeAndValue;
    struct TimeAndValueQueue;

    struct TrackedObjectData
    {
        TrackedObjectData();
        TrackedObjectData(uint64_t _objectId, int _objectType, void *_userData);
        uint64_t objectId;
        int objectType;
        void *userData;
    };

    StatisticsHistory();
    virtual ~StatisticsHistory();
    void SetDefaultTimeToTrack(Time defaultTimeToTrack);
    Time GetDefaultTimeToTrack(void) const;
    bool AddObject(TrackedObjectData tod);
    bool RemoveObject(uint64_t objectId, void **userData);
    void RemoveObjectAtIndex(unsigned int index);
    void Clear(void);
    unsigned int GetObjectCount(void) const;
    StatisticsHistory::TrackedObjectData * GetObjectAtIndex(unsigned int index) const;
    unsigned int GetObjectIndex(uint64_t objectId) const;
    bool AddValueByObjectID(uint64_t objectId, VEString key, SHValueType val, Time curTime, bool combineEqualTimes);
    void AddValueByIndex(unsigned int index, VEString key, SHValueType val, Time curTime, bool combineEqualTimes);
    SHErrorCode GetHistoryForKey(uint64_t objectId, VEString key, TimeAndValueQueue **values, Time curTime) const;
    bool GetHistorySorted(uint64_t objectId, SHSortOperation sortType, DataStructures::List<TimeAndValueQueue *> &values) const;
    void MergeAllObjectsOnKey(VEString key, TimeAndValueQueue *tavqOutput, SHDataCategory dataCategory) const;
    void GetUniqueKeyList(DataStructures::List<VEString> &keys);

    struct TimeAndValue
    {
        Time time;
        SHValueType val;
    };

    struct TimeAndValueQueue
    {
        TimeAndValueQueue();
        ~TimeAndValueQueue();

        DataStructures::Queue<TimeAndValue> values;

        Time timeToTrackValues;
        VEString key;

        SHValueType recentSum;
        SHValueType recentSumOfSquares;
        SHValueType longTermSum;
        SHValueType longTermCount;
        SHValueType longTermLowest;
        SHValueType longTermHighest;

        void SetTimeToTrackValues(Time t);
        Time GetTimeToTrackValues(void) const;
        SHValueType GetRecentSum(void) const;
        SHValueType GetRecentSumOfSquares(void) const;
        SHValueType GetLongTermSum(void) const;
        SHValueType GetRecentAverage(void) const;
        SHValueType GetRecentLowest(void) const;
        SHValueType GetRecentHighest(void) const;
        SHValueType GetRecentStandardDeviation(void) const;
        SHValueType GetLongTermAverage(void) const;
        SHValueType GetLongTermLowest(void) const;
        SHValueType GetLongTermHighest(void) const;
        SHValueType GetSumSinceTime(Time t) const;
        Time GetTimeRange(void) const;

        static void MergeSets( const TimeAndValueQueue *lhs, SHDataCategory lhsDataCategory, const TimeAndValueQueue *rhs, SHDataCategory rhsDataCategory, TimeAndValueQueue *output );

        void ResizeSampleSet( int approximateSamples, DataStructures::Queue<StatisticsHistory::TimeAndValue> &blendedSamples, SHDataCategory dataCategory, Time timeClipStart=0, Time timeClipEnd=0 );

        void Clear(void);

        TimeAndValueQueue& operator = ( const TimeAndValueQueue& input );

        void CullExpiredValues(Time curTime);
        static SHValueType Interpolate(TimeAndValue t1, TimeAndValue t2, Time time);
        SHValueType sortValue;
    };

protected:
    struct TrackedObject;
public:
    static int TrackedObjectComp( const uint64_t &key, TrackedObject* const &data );
protected:

    struct TrackedObject
    {
        TrackedObject();
        ~TrackedObject();
        TrackedObjectData trackedObjectData;
        DataStructures::Hash<VENet::VEString, TimeAndValueQueue*, 32, VENet::VEString::ToInteger> dataQueues;
    };

    DataStructures::OrderedList<uint64_t, TrackedObject*,TrackedObjectComp> objects;

    Time timeToTrack;
};

class VE_DLL_EXPORT StatisticsHistoryPlugin : public PluginInterface2
{
public:
    STATIC_FACTORY_DECLARATIONS(StatisticsHistoryPlugin)

    StatisticsHistory statistics;

    StatisticsHistoryPlugin();
    virtual ~StatisticsHistoryPlugin();
    void SetTrackConnections(bool _addNewConnections, int newConnectionsObjectType, bool _removeLostConnections);

protected:
    virtual void Update(void);
    virtual void OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason );
    virtual void OnNewConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, bool isIncoming);

    bool addNewConnections;
    bool removeLostConnections;
    int newConnectionsObjectType;
};

}

#endif