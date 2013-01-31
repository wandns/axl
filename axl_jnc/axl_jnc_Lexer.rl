#include "stdafx.h"
#include "axl_jnc_Lexer.h"

// warning C4065: switch statement contains 'default' but no 'case' labels

#pragma warning (disable: 4065)

namespace axl {
namespace jnc {

//.............................................................................

%%{

machine axl_jnc; 
write data;

# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# prepush / postpop (for fcall/fret)
#

prepush 
{
	stack = PrePush ();
}

postpop
{
	PostPop ();
}

# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# standard definitions
#

dec    = [0-9];
hex    = [0-9a-fA-F];
oct    = [0-7];
bin    = [01];
id     = [_a-zA-Z] [_a-zA-Z0-9]*;
ws     = [ \t\r]+;
nl     = '\n' @{ NewLine (p + 1); };
esc    = '\\' [^\n];
lit_dq = '"' ([^"\n\\] | esc)* (["\\] | nl);
lit_sq = "'" ([^'\n\\] | esc)* (['\\] | nl);

# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# main machine 
#

main := |*

# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

# global declarations & pragmas

'namespace'    { CreateToken (EToken_Namespace); };
'using'        { CreateToken (EToken_Using); };
'pack'         { CreateToken (EToken_Pack); };
'endian'       { CreateToken (EToken_Endian); };

# storage specifiers

'typedef'      { CreateToken (EToken_Typedef); };
'static'       { CreateToken (EToken_Static); };
'virtual'      { CreateToken (EToken_Virtual); };
'novirtual'    { CreateToken (EToken_NoVirtual); };

# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

# access specifiers

'public'       { CreateToken (EToken_Public); };
'publicread'   { CreateToken (EToken_PublicRead); };
'protected'    { CreateToken (EToken_Protected); };

# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

# type modifiers

'const'        { CreateToken (EToken_Const); };
'volatile'     { CreateToken (EToken_Volatile); };
'signed'       { CreateToken (EToken_Signed); };
'unsigned'     { CreateToken (EToken_Unsigned); };
'littleendian' { CreateToken (EToken_LittleEndian); };
'bigendian'    { CreateToken (EToken_BigEndian); };
'safe'         { CreateToken (EToken_Safe); };
'unsafe'       { CreateToken (EToken_Unsafe); };
'nonull'       { CreateToken (EToken_NoNull); };
'strong'       { CreateToken (EToken_Strong); };
'weak'         { CreateToken (EToken_Weak); };
'cdecl'        { CreateToken (EToken_Cdecl); };
'stdcall'      { CreateToken (EToken_Stdcall); };
'function'     { CreateToken (EToken_Function); };
'property'     { CreateToken (EToken_Property); };
'event'        { CreateToken (EToken_Event); };
'bindable'     { CreateToken (EToken_Bindable); };
'autoget'      { CreateToken (EToken_AutoGet); };
'indexed'      { CreateToken (EToken_Indexed); };
'closure'      { CreateToken (EToken_Closure); };
'thin'         { CreateToken (EToken_Thin); };

# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

# type specifiers

'auto'         { CreateToken (EToken_Auto); };
'void'         { CreateToken (EToken_Void); };
'object'       { CreateToken (EToken_Object); };
'variant'      { CreateToken (EToken_Variant); };
'bool'         { CreateToken (EToken_Bool); };
'int8'         { CreateToken (EToken_Int8); };
'int16'        { CreateToken (EToken_Int16); };
'int32'        { CreateToken (EToken_Int32); };
'int64'        { CreateToken (EToken_Int64); };
'float'        { CreateToken (EToken_Float); };
'double'       { CreateToken (EToken_Double); };
'int'          { CreateToken (EToken_Int); };
'char'         { CreateToken (EToken_Char); };
'short'        { CreateToken (EToken_Short); };
'long'         { CreateToken (EToken_Long); };
'wchar_t'      { CreateToken (EToken_WChar); };

# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

# named type specifiers

'enum'         { CreateToken (EToken_Enum); };
'enumc'        { CreateToken (EToken_EnumC); };
'struct'       { CreateToken (EToken_Struct); };
'union'        { CreateToken (EToken_Union); };
'class'        { CreateToken (EToken_Class); };
'interface'    { CreateToken (EToken_Interface); };
'autoev'       { CreateToken (EToken_AutoEv); };

# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

# special members

'get'          { CreateToken (EToken_Get); };
'set'          { CreateToken (EToken_Set); };
'this'         { CreateToken (EToken_This); };
'preconstruct' { CreateToken (EToken_PreConstruct); };
'operator'     { CreateToken (EToken_Operator); };
'postfix'      { CreateToken (EToken_Postfix); };
'propvalue'    { CreateToken (EToken_PropValue); };
'onchange'     { CreateToken (EToken_OnChange); };

# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

# operators

'new'          { CreateToken (EToken_New); };
'stack'        { CreateToken (EToken_Stack); };
'heap'         { CreateToken (EToken_Heap); };
'sizeof'       { CreateToken (EToken_SizeOf); };
'typeof'       { CreateToken (EToken_TypeOf); };
'snapshot'     { CreateToken (EToken_Snapshot); };
'dynamic'      { CreateToken (EToken_Dynamic); };

# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

# statements

'if'           { CreateToken (EToken_If); };
'else'         { CreateToken (EToken_Else); };
'for'          { CreateToken (EToken_For); };
'while'        { CreateToken (EToken_While); };
'do'           { CreateToken (EToken_Do); };
'break'        { CreateToken (EToken_Break); };
'continue'     { CreateToken (EToken_Continue); };
'return'       { CreateToken (EToken_Return); };
'switch'       { CreateToken (EToken_Switch); };
'case'         { CreateToken (EToken_Case); };
'default'      { CreateToken (EToken_Default); };

# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

# constants

'true'         { CreateToken (EToken_True); };
'false'        { CreateToken (EToken_False); };
'null'         { CreateToken (EToken_Null); };

# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

# symbol tokens

'++'           { CreateToken (EToken_Inc); };
'--'           { CreateToken (EToken_Dec); };
'->'           { CreateToken (EToken_Ptr); };	
'<<'           { CreateToken (EToken_Shl); };
'>>'           { CreateToken (EToken_Shr); };
'=='           { CreateToken (EToken_Eq); };
'!='           { CreateToken (EToken_Ne); };
'<='           { CreateToken (EToken_Le); };
'>='           { CreateToken (EToken_Ge); };	
'*='           { CreateToken (EToken_MulAssign); };
'/='           { CreateToken (EToken_DivAssign); };
'%='           { CreateToken (EToken_ModAssign); };
'+='           { CreateToken (EToken_AddAssign); };
'-='           { CreateToken (EToken_SubAssign); };
'<<='          { CreateToken (EToken_ShlAssign); };
'>>='          { CreateToken (EToken_ShrAssign); };
'&='           { CreateToken (EToken_AndAssign); };
'^='           { CreateToken (EToken_XorAssign); };
'|='           { CreateToken (EToken_OrAssign); };	
'@='           { CreateToken (EToken_AtAssign); };	
':='           { CreateToken (EToken_ClassAssign); };	
'&&'           { CreateToken (EToken_LogAnd); };	
'||'           { CreateToken (EToken_LogOr); };
'...'          { CreateToken (EToken_Ellipsis); };

# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

# common tokens

id             { CreateStringToken (EToken_Identifier); };
lit_sq         { CreateCharToken (EToken_Integer); };
lit_dq         { CreateStringToken (EToken_Literal, 1, 1); };
dec+           { CreateIntegerToken (10); };
'0' [Xx] hex+  { CreateIntegerToken (16, 2); };

'//' [^\n]*    ;
'/*' (any | nl)* :>> '*/' ;

ws | nl        ;

any            { CreateToken (ts [0]); };

*|;

}%%

//.............................................................................

void 
CLexer::Init ()
{
	%% write init;
}

bool
CLexer::Exec ()
{
	%% write exec;

	bool Result = cs != axl_jnc_error;
	if (!Result)
	{
		CToken* pToken = CreateToken (EToken_Error);
		pToken->m_Data.m_Error = err::CError ("lexer error");
	}

	return Result;
}

//.............................................................................

} // namespace jnc {
} // namespace axl {
