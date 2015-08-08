////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Include File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeMouse.inl
//  Version:     v1.00
//  Created:     7/12/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
VE_INLINE void VeMouse::OnCursorMove(VeSizeT stData)
{
	g_pInputManager->AppendMessage(VeInputMessage::DT_MOUSE,
		VeInputMessage::MIT_CURSOR_MOVED, g_pTime->GetTimeUInt(), stData);
}
//--------------------------------------------------------------------------
VE_INLINE bool VeMouse::GetStateFlag(State eBtn)
{
	return VE_MASK_HAS_ANY(m_u32State, eBtn);
}
//--------------------------------------------------------------------------
