#pragma once

#include <cocos2d.h>
#include "Venus3D.h"

#define SAVE_DATA_FILE_NAME "save.dat"

class GameSaveData : public VeSingleton<GameSaveData>
{
public:
	typedef VeMemberDelegate<GameSaveData, VE_RESULT> WriteDelegate;

	GameSaveData();

	~GameSaveData();

	void Load();

	void Save();

	VeUInt32 ReadParam(const VeChar8* pcName, VeUInt32 u32Default = 0);

	void WriteParam(const VeChar8* pcName, VeUInt32 u32Param);

	VeFloat32 ReadParam(const VeChar8* pcName, VeFloat32 f32Default = 0.0f);

	void WriteParam(const VeChar8* pcName, VeFloat32 f32Param);

private:
	void OnWriteOver(VE_RESULT kResult);

	void InitEncryptionSystem();

	VeDirectory* m_pkDataDir;
	WriteDelegate m_kWriteDelegate;

	VeStringMap<VeUInt32> m_kDataMap;
	VeVector< VePair<VeString,VeUInt32> > m_kDataArray;

};

#define g_pSaveData GameSaveData::GetSingletonPtr()
