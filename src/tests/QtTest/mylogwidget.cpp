#include "pch.h"
#include "mylogwidget.h"
#include "mylogrepresenter.h"

//.............................................................................

void
MyLogWidget::OnKeyDown (
	gui::TWidgetMsg* pMsg,
	bool* pIsHandled
	)
{
	char Data [] = "for 'x' 123 while ";

	gui::TWidgetKeyMsg* pCharMsg = (gui::TWidgetKeyMsg*) pMsg;

	switch (pCharMsg->m_Key)
	{
	case '1':
		m_pServerPeer->WritePacket (EMyLog_DeviceOpened);
		break;

	case '2':
		m_pServerPeer->WritePacket (EMyLog_DeviceClosed);
		break;

	case '3':
		m_pServerPeer->WritePacket (EMyLog_Connect);
		break;

	case '4':
		m_pServerPeer->WritePacket (EMyLog_ConnectComplete);
		break;

	case '5':
		m_pServerPeer->WritePacket (EMyLog_ConnectError);
		break;

	case '0':
		m_pServerPeer->WritePacket (EMyLog_Bin, Data, sizeof (Data));
		break;

	default:
		if (pCharMsg->m_Key >= 'A' && pCharMsg->m_Key <= 'Z')
		{
			uint_t Char = 
				(pCharMsg->m_ModifierKeys & gui::EModifierKey_Ctrl) ? pCharMsg->m_Key - 'A' + 1 :
				(pCharMsg->m_ModifierKeys & gui::EModifierKey_Shift) ? pCharMsg->m_Key : 
				tolower (pCharMsg->m_Key);

			m_pServerPeer->WritePacket (EMyLog_Bin, &Char, 1);
		}
		else if (isprint (pCharMsg->m_Key))
		{
			m_pServerPeer->WritePacket (EMyLog_Bin, &pCharMsg->m_Key, 1);
		}
		else
		{
			*pIsHandled = false;
		}
	}
}

//.............................................................................

