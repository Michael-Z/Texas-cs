////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeMesh.h
//  Version:     v1.00
//  Created:     10/1/2013 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

class VE_MAIN_API VeSubMesh : public VeMemObject
{
public:
	struct Scheme
	{
		VE_INLINE Scheme();
	
		VE_INLINE ~Scheme();

		VeRenderNodeDataPtr m_spNodeData;
		VeRenderNode* m_pkRenderNode;
	};

	VeVector<VeROBufferPtr> m_kStreamBufferArray;
	VeROBufferPtr m_kIndexBuffer;
	VeVector<Scheme> m_kSchemeArray;
};

//class VE_MAIN_API VeMesh : public VeRefObject
//{
//public:
//	//VeMesh
//
//};

#ifndef VE_NO_INLINE
#	include "VeMesh.inl"
#endif
