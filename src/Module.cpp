#include "pch.h"
#include "Module.h"

//.............................................................................

void
RefText::luaExport (lua::LuaState* luaState)
{
	luaState->createTable ();

	luaState->setMemberString ("m_refKind", getRefKindString (m_refKind));
	luaState->setMemberString ("m_text", m_text);
	luaState->setMemberString ("m_id", m_id);
	luaState->setMemberString ("m_external", m_external);
	luaState->setMemberString ("m_tooltip", m_tooltip);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void
LinkedText::luaExport (lua::LuaState* luaState)
{
	normalize ();

	luaState->createTable ();
	
	luaState->setMemberBoolean ("m_isEmpty", m_plainText.isEmpty ());
	luaState->setMemberString ("m_plainText", m_plainText);
	
	luaExportList (luaState, m_refTextList);
	luaState->setMember ("m_refTextArray");
}

void
LinkedText::normalize ()
{
	m_plainText.clear ();

	sl::Iterator <RefText> it = m_refTextList.getHead ();
	while (it)
	{
		if (!it->m_text.isEmpty ())
		{
			m_plainText += it->m_text;
			it++;
		}
		else
		{
			sl::Iterator <RefText> next = it.getNext ();
			m_refTextList.erase (it);
			it = next;
		}
	}
}

//.............................................................................

void
DocBlock::luaExportMembers (lua::LuaState* luaState)
{
	luaState->setMemberInteger ("m_docBlockKind", m_docBlockKind);
	luaState->setMemberString ("m_title", m_title);
}

void
DocParagraphBlock::luaExport (lua::LuaState* luaState)
{
	luaState->createTable ();
	
	DocBlock::luaExportMembers (luaState);

	m_contents.luaExport (luaState);
	luaState->setMember ("m_contents");
}

void
DocSectionBlock::luaExport (lua::LuaState* luaState)
{
	luaState->createTable ();
	
	DocBlock::luaExportMembers (luaState);
	luaState->setMemberString ("m_id", m_id);
	
	luaExportList (luaState, m_childBlockList);
	luaState->setMember ("m_childBlockList");
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void
Description::luaExport (lua::LuaState* luaState)
{
	luaState->createTable ();

	luaState->setMemberBoolean ("m_isEmpty", isEmpty ());
	
	luaExportList (luaState, m_docBlockList);
	luaState->setMember ("m_docBlockList");
}

//.............................................................................

void
Param::luaExport (lua::LuaState* luaState)
{
	luaState->createTable ();

	luaState->setMemberString ("m_declarationName", m_declarationName);
	luaState->setMemberString ("m_definitionName", m_definitionName);
	luaState->setMemberString ("m_array", m_array);

	m_type.luaExport (luaState);
	luaState->setMember ("m_type");

	m_defaultValue.luaExport (luaState);
	luaState->setMember ("m_defaultValue");

	m_typeConstraint.luaExport (luaState);
	luaState->setMember ("m_typeConstraint");

	m_briefDescription.luaExport (luaState);
	luaState->setMember ("m_briefDescription");
}

//.............................................................................

void
EnumValue::luaExport (lua::LuaState* luaState)
{
	luaState->createTable ();

	luaState->setMemberString ("m_protectionKind", getProtectionKindString (m_protectionKind));
	luaState->setMemberString ("m_id", m_id);
	luaState->setMemberString ("m_name", m_name);

	m_initializer.luaExport (luaState);
	luaState->setMember ("m_initializer");

	m_briefDescription.luaExport (luaState);
	luaState->setMember ("m_briefDescription");

	m_detailedDescription.luaExport (luaState);
	luaState->setMember ("m_detailedDescription");
}

//.............................................................................

const char*
getMemberFlagString (MemberFlag flag)
{
	const char* stringTable [] = 
	{
		"static",          // MemberFlag_Static        = 0x00000001,
		"const",           // MemberFlag_Const         = 0x00000002,
		"explicit",        // MemberFlag_Explicit      = 0x00000004,
		"inline",          // MemberFlag_Inline        = 0x00000008,
		"volatile",        // MemberFlag_Volatile      = 0x00000010,
		"mutable",         // MemberFlag_Mutable       = 0x00000020,
		"readable",        // MemberFlag_Readable      = 0x00000040,
		"writable",        // MemberFlag_Writeable     = 0x00000080,
		"initonly",        // MemberFlag_InitOnly      = 0x00000100,
		"settable",        // MemberFlag_Settable      = 0x00000200,
		"gettable",        // MemberFlag_Gettable      = 0x00000400,
		"final",           // MemberFlag_Final         = 0x00000800,
		"sealed",          // MemberFlag_Sealed        = 0x00001000,
		"new",             // MemberFlag_New           = 0x00002000,
		"add",             // MemberFlag_Add           = 0x00004000,
		"remove",          // MemberFlag_Remove        = 0x00008000,
		"raise",           // MemberFlag_Raise         = 0x00010000,
		"optional",        // MemberFlag_Optional      = 0x00020000,
		"required",        // MemberFlag_Required      = 0x00040000,
		"accessor",        // MemberFlag_Accessor      = 0x00080000,
		"attribute",       // MemberFlag_Attribute     = 0x00100000,
		"property",        // MemberFlag_Property      = 0x00200000,
		"readonly",        // MemberFlag_ReadOnly      = 0x00400000,
		"bound",           // MemberFlag_Bound         = 0x00800000,
		"removable",       // MemberFlag_Removable     = 0x01000000,
		"contrained",      // MemberFlag_Contrained   = 0x02000000,
		"transient",       // MemberFlag_Transient     = 0x04000000,
		"maybevoid",       // MemberFlag_MaybeVoid     = 0x08000000,
		"maybedefault",    // MemberFlag_MaybeDefault  = 0x10000000,
		"maybeambiguous",  // MemberFlag_MaybeAmbiguos = 0x20000000,
	};

	return (size_t) flag < countof (stringTable) ? 
		stringTable [flag] :
		"<undefined>";
}

inline
MemberFlag
getFirstMemberFlag (uint_t flags)
{
	return (MemberFlag) (1 << sl::getLoBitIdx (flags));
}

sl::String
getMemberFlagString (uint_t flags)
{
	if (!flags)
		return sl::String ();

	sl::String string;

	while (flags)
	{
		MemberFlag flag = getFirstMemberFlag (flags);
		flags &= ~flag;

		string += getMemberFlagString (flag);
		string += ' ';
	}

	string.reduceLength (1);
	return string;
}

//.............................................................................

Member::Member ()
{
	m_parentCompound = NULL;
	m_doxyGroupCompound = NULL;
	m_memberKind = MemberKind_Undefined;
	m_protectionKind = ProtectionKind_Undefined;
	m_virtualKind = VirtualKind_Undefined;
	m_flags = 0;
}

void
Member::luaExport (lua::LuaState* luaState)
{
	luaState->createTable ();

	luaState->setMemberString ("m_memberKind", getMemberKindString (m_memberKind));
	luaState->setMemberString ("m_protectionKind", getProtectionKindString (m_protectionKind));
	luaState->setMemberString ("m_virtualKind", getVirtualKindString (m_virtualKind));
	luaState->setMemberString ("m_flags", getMemberFlagString (m_flags));
	luaState->setMemberString ("m_id", m_id);
	luaState->setMemberString ("m_name", m_name);
	luaState->setMemberString ("m_modifiers", m_modifiers);

	switch (m_memberKind)
	{
	case MemberKind_Typedef:
		m_type.luaExport (luaState);
		luaState->setMember ("m_type");
		luaState->setMemberString ("m_argString", m_argString);
		luaExportList (luaState, m_paramList);
		luaState->setMember ("m_paramArray");
		break;

	case MemberKind_Enum:
		luaExportList (luaState, m_enumValueList);
		luaState->setMember ("m_enumValueArray");
		break;

	case MemberKind_Variable:
		m_type.luaExport (luaState);
		luaState->setMember ("m_type");

		luaState->setMemberString ("m_bitField", m_bitField);

		m_initializer.luaExport (luaState);
		luaState->setMember ("m_initializer");
		break;

	case MemberKind_Function:
		m_type.luaExport (luaState);
		luaState->setMember ("m_returnType");

		m_exceptions.luaExport (luaState);
		luaState->setMember ("m_exceptions");

		luaExportList (luaState, m_templateParamList);
		luaState->setMember ("m_templateParamArray");

		luaExportList (luaState, m_templateSpecParamList);
		luaState->setMember ("m_templateSpecParamArray");

		luaExportList (luaState, m_paramList);
		luaState->setMember ("m_paramArray");
		break;

	case MemberKind_Property:
		m_type.luaExport (luaState);
		luaState->setMember ("m_returnType");

		luaExportList (luaState, m_paramList);
		luaState->setMember ("m_paramArray");
		break;

	case MemberKind_Event:
		m_type.luaExport (luaState);
		luaState->setMember ("m_type");

		luaExportList (luaState, m_paramList);
		luaState->setMember ("m_paramArray");
		break;

	case MemberKind_Alias:
		m_initializer.luaExport (luaState);
		luaState->setMember ("m_initializer");
		break;
	}

	m_briefDescription.luaExport (luaState);
	luaState->setMember ("m_briefDescription");

	m_detailedDescription.luaExport (luaState);
	luaState->setMember ("m_detailedDescription");

	m_inBodyDescription.luaExport (luaState);
	luaState->setMember ("m_inBodyDescription");
}

//.............................................................................

Compound::Compound ()
{
	m_selfNamespace = NULL;
	m_parentNamespace = NULL;
	m_doxyGroupCompound = NULL;
	m_compoundKind = CompoundKind_Undefined;
	m_languageKind = LanguageKind_Undefined;
	m_protectionKind = ProtectionKind_Undefined;
	m_isFinal = false;
	m_isSealed = false;
	m_isAbstract = false;
}

void
Compound::luaExport (lua::LuaState* luaState)
{
	unqualifyName ();
	unspecializeName ();

	luaState->createTable ();
	luaState->setMemberString ("m_compoundKind", getCompoundKindString (m_compoundKind));	
	luaState->setMemberString ("m_id", m_id);
	luaState->setMemberString ("m_name", m_name);
	luaState->setMemberString ("m_title", m_title);
		
	switch (m_compoundKind)
	{
	case CompoundKind_Group:
		ASSERT (m_doxyGroupCompound);
		m_doxyGroupCompound->m_briefDescription.luaExport (luaState);
		luaState->setMember ("m_briefDescription");
		m_doxyGroupCompound->m_detailedDescription.luaExport (luaState);
		luaState->setMember ("m_detailedDescription");
		break;

	case CompoundKind_Namespace:
		m_briefDescription.luaExport (luaState);
		luaState->setMember ("m_briefDescription");
		m_detailedDescription.luaExport (luaState);
		luaState->setMember ("m_detailedDescription");
		break;

	case CompoundKind_Struct:
	case CompoundKind_Union:
	case CompoundKind_Class:
	case CompoundKind_Interface:
		luaExportList (luaState, m_templateParamList);
		luaState->setMember ("m_templateParamArray");
		luaExportList (luaState, m_templateSpecParamList);
		luaState->setMember ("m_templateSpecParamArray");
		m_briefDescription.luaExport (luaState);
		luaState->setMember ("m_briefDescription");
		m_detailedDescription.luaExport (luaState);
		luaState->setMember ("m_detailedDescription");
		break;
	}

	preparePath ();
	luaState->setMemberString ("m_path", m_path);

	m_selfNamespace->luaExportMembers (luaState);
}

void
Compound::unqualifyName ()
{
	const char* unqualifiedName = m_name; 
	const char* p = m_name;
	const char* end = p + m_name.getLength ();

	for (; p < end; p++)
	{
		switch (*p)
		{
		case '<':
			p = end;
			break;

		case ':':
		case '.':
			unqualifiedName = p + 1;
			break;
		}
	}

	if (unqualifiedName != m_name)
		m_name = unqualifiedName;
}

void
Compound::unspecializeName ()
{
	size_t i = m_name.find ('<');
	if (i == -1)
		return;

	size_t unspecializedNameLength = i;

	const char* p = m_name.cc () + unspecializedNameLength + 1;
	const char* end = m_name.cc () + m_name.getLength ();

	const char* p0 = p;
	size_t level = 0;

	for (; p < end; p++)
	{
		switch (*p)
		{
		case '<':
			level++;
			break;

		case '>':
			level--;
			if (level == -1)
			{
				createTemplateSpecParam (sl::StringRef (p0, p - p0));
				p = end; // outta here
			}

			break;

		case ',':
			if (!level)
			{
				createTemplateSpecParam (sl::StringRef (p0, p - p0));
				p0 = p + 1;
			}

			break;
		}
	
	}

	m_name.setReducedLength (unspecializedNameLength);
	m_name.trimRight ();

	const char* x = strchr (m_name, '>');
	ASSERT (!x);
}

Param*
Compound::createTemplateSpecParam (const sl::StringRef& name)
{
	Param* param = AXL_MEM_NEW (Param);
	param->m_declarationName = name;
	param->m_declarationName.trim ();

	m_templateSpecParamList.insertTail (param);
	return param;
}

void
Compound::preparePath ()
{
	if (!m_path.isEmpty ())
		return;

	if (!m_parentNamespace)
	{
		if (m_compoundKind != CompoundKind_Group) // groups don't contribute to path
			m_path = m_name;
	}
	else
	{
		m_parentNamespace->m_compound->preparePath ();
		m_path = m_parentNamespace->m_compound->m_path;

		if (m_compoundKind != CompoundKind_Group) // groups don't contribute to path
		{
			m_path += '/';
			m_path += m_name;
		}
	}
}

//.............................................................................

bool
NamespaceContents::add (Compound* compound)
{
	switch (compound->m_compoundKind)
	{
	case CompoundKind_Namespace:
		ASSERT (compound->m_selfNamespace);
		m_namespaceArray.append (compound->m_selfNamespace);
		break;

	case CompoundKind_Class:
		ASSERT (compound->m_selfNamespace);
		m_classArray.append (compound->m_selfNamespace);
		break;

	case CompoundKind_Struct:
		ASSERT (compound->m_selfNamespace);
		m_structArray.append (compound->m_selfNamespace);
		break;

	case CompoundKind_Union:
		ASSERT (compound->m_selfNamespace);
		m_unionArray.append (compound->m_selfNamespace);
		break;

	case CompoundKind_Interface:
		ASSERT (compound->m_selfNamespace);
		m_classArray.append (compound->m_selfNamespace);
		break;

	case CompoundKind_DoxyGroup:
	case CompoundKind_Group:
		// groups are added implicitly, via group members

	case CompoundKind_Protocol:
	case CompoundKind_Category:
	case CompoundKind_Exception:
	case CompoundKind_File:
	case CompoundKind_Page:
	case CompoundKind_Example:
	case CompoundKind_Dir:
		// not used in doxyrest
	
	default:
		return false;
	}

	return true;
}

bool
NamespaceContents::add (Member* member)
{
	switch (member->m_memberKind)
	{
	case MemberKind_Property:
		m_propertyArray.append (member);
		break;

	case MemberKind_Event:
		m_eventArray.append (member);
		break;

	case MemberKind_Variable:
		m_variableArray.append (member);
		break;

	case MemberKind_Typedef:
		m_typedefArray.append (member);
		break;

	case MemberKind_Enum:
		m_enumArray.append (member);
		break;

	case MemberKind_Function:
		m_functionArray.append (member);
		break;

	case MemberKind_Alias:
		m_aliasArray.append (member);
		break;

	case MemberKind_Footnote:
		m_footnoteArray.append (member);
		break;

	case MemberKind_Define:
	case MemberKind_Signal:
	case MemberKind_Prototype:
	case MemberKind_Friend:
	case MemberKind_Dcop:
	case MemberKind_Slot:
	case MemberKind_Interface:
	case MemberKind_Service:
	case MemberKind_EnumValue:
		
	default:
		return false;
	}

	return true;
}

void
NamespaceContents::luaExportMembers (lua::LuaState* luaState)
{
	luaExportArraySetParent (luaState, m_groupArray, "m_parent");
	luaState->setMember ("m_groupArray");

	luaExportArraySetParent (luaState, m_namespaceArray, "m_parent");
	luaState->setMember ("m_namespaceArray");
	
	luaExportArraySetParent (luaState, m_enumArray, "m_parent");
	luaState->setMember ("m_enumArray");
	
	luaExportArraySetParent (luaState, m_structArray, "m_parent");
	luaState->setMember ("m_structArray");
	
	luaExportArraySetParent (luaState, m_unionArray, "m_parent");
	luaState->setMember ("m_unionArray");
	
	luaExportArraySetParent (luaState, m_classArray, "m_parent");
	luaState->setMember ("m_classArray");
	
	luaExportArraySetParent (luaState, m_typedefArray, "m_parent");
	luaState->setMember ("m_typedefArray");
	
	luaExportArraySetParent (luaState, m_variableArray, "m_parent");
	luaState->setMember ("m_variableArray");

	luaExportArraySetParent (luaState, m_functionArray, "m_parent");
	luaState->setMember ("m_functionArray");
	
	luaExportArraySetParent (luaState, m_propertyArray, "m_parent");
	luaState->setMember ("m_propertyArray");
	
	luaExportArraySetParent (luaState, m_eventArray, "m_parent");
	luaState->setMember ("m_eventArray");

	luaExportArraySetParent (luaState, m_aliasArray, "m_parent");
	luaState->setMember ("m_aliasArray");

	luaExportArraySetParent (luaState, m_footnoteArray, "m_parent");
	luaState->setMember ("m_footnoteArray");
}

//.............................................................................

void
GlobalNamespace::clear ()
{
	m_groupArray.clear ();
	m_namespaceArray.clear ();
	m_enumArray.clear ();
	m_structArray.clear ();
	m_unionArray.clear ();
	m_classArray.clear ();
	m_typedefArray.clear ();
	m_variableArray.clear ();
	m_functionArray.clear ();
	m_propertyArray.clear ();
	m_eventArray.clear ();
	m_aliasArray.clear ();
	m_namespaceList.clear ();
}

bool
GlobalNamespace::build (
	Module* module,
	ProtectionKind protectionFilter
	)
{
	clear ();

	// loop #1 assign groups
	
	size_t count = module->m_doxyGroupArray.getCount ();
	for (size_t i = 0; i < count; i++)
	{
		Compound* compound = module->m_doxyGroupArray [i];

		sl::Iterator <Member> memberIt = compound->m_memberList.getHead ();
		for (; memberIt; memberIt++)
		{
			Member* member = module->findMember (memberIt->m_id);
			if (member)
				member->m_doxyGroupCompound = compound;
		}

		sl::Iterator <Ref> refIt = compound->m_innerRefList.getHead ();
		for (; refIt; refIt++)
		{
			Compound* innerCompound =  module->findCompound (refIt->m_id);
			if (innerCompound)
				innerCompound->m_doxyGroupCompound = compound;
		}
	}

	// loop #2 initializes namespaces
	
	count = module->m_namespaceArray.getCount ();
	for (size_t i = 0; i < count; i++)
	{
		Compound* compound = module->m_namespaceArray [i];

		Namespace* nspace = AXL_MEM_NEW (Namespace);
		m_namespaceList.insertTail (nspace);
		nspace->m_compound = compound;
		compound->m_selfNamespace = nspace;
	}

	// loop #3 resolves inner references and add members

	for (size_t i = 0; i < count; i++)
	{
		Compound* compound = module->m_namespaceArray [i];
		Namespace* nspace = compound->m_selfNamespace;
		ASSERT (nspace);

		sl::Iterator <Member> memberIt = compound->m_memberList.getHead ();
		for (; memberIt; memberIt++)
		{
			if (memberIt->m_protectionKind > protectionFilter)
				continue;

			Namespace* targetNspace = memberIt->m_doxyGroupCompound ? 
				getSubGroupNamespace (module, nspace, nspace, memberIt->m_doxyGroupCompound) : 
				nspace;

			targetNspace->add (*memberIt);
		}

		sl::Iterator <Ref> refIt = compound->m_innerRefList.getHead ();
		for (; refIt; refIt++)
		{
			Compound* innerCompound = module->findCompound (refIt->m_id);
			if (!innerCompound)
			{
				err::setFormatStringError ("can't find compound refid: %s\n", refIt->m_id.cc ());
				return false;
			}

			if (innerCompound->m_protectionKind > protectionFilter)
				continue;

			Namespace* targetNspace = innerCompound->m_doxyGroupCompound ? 
				getSubGroupNamespace (module, nspace, nspace, innerCompound->m_doxyGroupCompound) : 
				nspace;

			targetNspace->add (innerCompound);
			innerCompound->m_parentNamespace = nspace; // namespace, not group!
		}
	}

	// loop #3 adds leftovers to the global namespace

	sl::Iterator <Compound> compoundIt = module->m_compoundList.getHead ();
	for (; compoundIt; compoundIt++)
	{
		sl::Iterator <Member> memberIt;

		switch (compoundIt->m_compoundKind)
		{
		case CompoundKind_DoxyGroup:
		case CompoundKind_Group:
			// groups are added implicitly, via group members
			break;

		case CompoundKind_File:
			memberIt = compoundIt->m_memberList.getHead ();
			for (; memberIt; memberIt++)
			{
				if (memberIt->m_protectionKind > protectionFilter)
					continue;

				NamespaceContents* targetNspace = memberIt->m_doxyGroupCompound ? 
					(NamespaceContents*) getSubGroupNamespace (module, this, NULL, memberIt->m_doxyGroupCompound) : 
					this;

				targetNspace->add (*memberIt);
			}
			break;

		default:
			if (!compoundIt->m_parentNamespace && compoundIt->m_protectionKind <= protectionFilter)
			{
				NamespaceContents* targetNspace = compoundIt->m_doxyGroupCompound ? 
					(NamespaceContents*) getSubGroupNamespace (module, this, NULL, compoundIt->m_doxyGroupCompound) : 
					this;

				targetNspace->add (*compoundIt);
			}
		}
	}

	return true;
}

void
GlobalNamespace::luaExport (lua::LuaState* luaState)
{
	luaState->createTable ();
	luaExportMembers (luaState);

	luaState->setMemberString ("m_path", "");
	luaState->setMemberString ("m_id", "global");
	luaState->setMemberString ("m_compoundKind", "namespace");

	// global namespace has no description, but we still want valid m_briefDescription/m_detailedDescription

	Description emptyDescription;

	emptyDescription.luaExport (luaState);
	luaState->setMember ("m_briefDescription");

	emptyDescription.luaExport (luaState);
	luaState->setMember ("m_detailedDescription");

	luaState->setGlobal ("g_globalNamespace");
}

Namespace* 
GlobalNamespace::getSubGroupNamespace (
	Module* module,
	NamespaceContents* parent,
	Namespace* parentNamespace,
	Compound* doxyGroupCompound
	)
{	
	if (doxyGroupCompound->m_doxyGroupCompound) // re-parent to super-group
		parent = getSubGroupNamespace (module, parent, parentNamespace, doxyGroupCompound->m_doxyGroupCompound);

	sl::StringHashTableMapIterator <Namespace*> localMapIt = parent->m_groupMap.visit (doxyGroupCompound->m_id);
	if (localMapIt->m_value)
		return localMapIt->m_value;

	Compound* compound = AXL_MEM_NEW (Compound);
	compound->m_compoundKind = CompoundKind_Group;
	compound->m_doxyGroupCompound = doxyGroupCompound;
	compound->m_name = doxyGroupCompound->m_name;
	compound->m_title = doxyGroupCompound->m_title;
	compound->m_parentNamespace = parentNamespace;
	module->m_compoundList.insertTail (compound);

	sl::StringHashTableMapIterator <size_t> globalMapIt = module->m_groupMap.visit (doxyGroupCompound->m_id);
	if (!globalMapIt->m_value)
	{
		compound->m_id = doxyGroupCompound->m_id;
		globalMapIt->m_value = 2; // start with 2
	}
	else
	{
		compound->m_id.format ("%s_%d", doxyGroupCompound->m_id.cc (), globalMapIt->m_value);
		globalMapIt->m_value++;
	}

	Namespace* nspace = AXL_MEM_NEW (Namespace);
	m_namespaceList.insertTail (nspace);
	nspace->m_compound = compound;
	compound->m_selfNamespace = nspace;
	
	localMapIt->m_value = nspace;
	parent->m_groupArray.append (nspace);
	return nspace;
}

//.............................................................................
