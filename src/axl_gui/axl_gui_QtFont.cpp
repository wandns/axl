#include "pch.h"
#include "axl_gui_QtFont.h"
#include "axl_gui_QtEngine.h"

namespace axl {
namespace gui {

//.............................................................................

bool
GetFontDescFromFontInfo (
	const QFontInfo& FontInfo,
	TFontDesc* pFontDesc
	)
{
	QString FamilyName = FontInfo.family ();
	QByteArray FamilyNameUtf = FamilyName.toUtf8 ();	
	size_t Length = FamilyNameUtf.size ();
	
	if (Length >= countof (pFontDesc->m_FaceName))
		Length = countof (pFontDesc->m_FaceName) - 1;
	
	memcpy (pFontDesc->m_FaceName, FamilyNameUtf, Length);
	pFontDesc->m_FaceName [Length] = 0;	
	pFontDesc->m_PointSize = FontInfo.pointSize ();
	
	if (FontInfo.weight () >= QFont::Bold)
		pFontDesc->m_Flags |= EFontFlag_Bold;
	
	if (FontInfo.italic ())
		pFontDesc->m_Flags |= EFontFlag_Italic;
	
	if (FontInfo.underline ())
		pFontDesc->m_Flags |= EFontFlag_Underline;

	if (FontInfo.strikeOut ())
		pFontDesc->m_Flags |= EFontFlag_Strikeout;

	return true;
}

//.............................................................................

CQtFont::CQtFont ()
{
	m_pEngine = CQtEngine::GetSingleton ();
}

TSize
CQtFont::CalcTextSize (
	const char* pText,
	size_t Length
	)
{
	QFontMetrics QtFontMetrics (m_QtFont);
	
	QString QtText = QString::fromUtf8 (pText, Length);
		
	TSize Size;
	Size.m_Width = QtFontMetrics.width (QtText);
	Size.m_Height = QtFontMetrics.height ();
	return Size;
}

//.............................................................................

} // namespace gui
} // namespace axl