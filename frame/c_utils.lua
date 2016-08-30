-------------------------------------------------------------------------------

function getNormalizedCppString (string)
	local s = string

	s = string.gsub (s, "%s*%*", "*")
	s = string.gsub (s, "%s*&", "&")
	s = string.gsub (s, "<%s*", " <")
	s = string.gsub (s, "%s+>", ">")
	s = string.gsub (s, "%(%s*", " (")
	s = string.gsub (s, "%s+%)", ")")

	return s
end

function getLinkedTextString (text)
	if not text then
		return nil
	end

	local s = ""

	for i = 1, #text.m_refTextArray do
		local refText = text.m_refTextArray [i]
		local text = getNormalizedCppString (refText.m_text)

		if refText.m_id ~= "" then
			s = s .. ":ref:`" .. text .. "<doxid-" .. refText.m_id .. ">`"
		else
			s = s .. text
		end
	end

	return s
end

function getParamString (param)
	local s = ""

	if not param.m_type.m_isEmpty then
		s = s .. getLinkedTextString (param.m_type)
	end

	if param.m_declarationName ~= "" then
		if s ~= "" then
			s = s .. " "
		end

		s = s .. getNormalizedCppString (param.m_declarationName)
	end

	if not param.m_defaultValue.m_isEmpty then
		s = s .. " = "
		s = s .. getLinkedTextString (param.m_defaultValue)
	end

	return s
end

function getFunctionParamArrayString (paramArray, indent)
	local s
	local count = #paramArray

	if count == 0 then
		s = "()"
	elseif count == 1  then
		s = "(" .. getParamString (paramArray [1]) .. ")"
	else
		s = "(\n" .. indent .. "    "

		for i = 1, count do
			s = s .. getParamString (paramArray [i])

			if i ~= count then
				s = s .. ","
			end

			s = s .. "\n" .. indent .. "    "
		end
		s = s .. ")"
	end

	return s
end

function getTemplateParamArrayString (paramArray)
	local s
	local count = #paramArray

	if count == 0 then
		s = "<>"
	else
		s = "<" .. getParamString (paramArray [1])

		for i = 2, count do
			s = s .. ", " .. getParamString (paramArray [i])
		end

		s = s .. ">"
	end

	return s
end

function getItemName (item)
	local s = item.m_name

	if item.m_templateParamArray and #item.m_templateParamArray > 0 then
		s = s .. " " .. getTemplateParamArrayString (item.m_templateParamArray)
	end

	if item.m_templateSpecParamArray and #item.m_templateSpecParamArray > 0 then
		s = s .. " " .. getTemplateParamArrayString (item.m_templateSpecParamArray)
	end

	return s
end

g_itemFileNameMap = {}

function getItemFileName (item, parentCompound)
	local s

	if item.m_compoundKind then
		s = item.m_compoundKind .. g_namespaceSep
	elseif item.m_memberKind then
		s = item.m_memberKind .. g_namespaceSep
	else
		s = "undef_"
	end

	if parentCompound.m_name then
		s = s .. string.gsub (parentCompound.m_path, "/", g_namespaceSep) .. g_namespaceSep
	end

	s = s .. item.m_name

	if g_namespaceSep ~= "-" then
		s = string.gsub (s, '-', g_namespaceSep) -- groups can contain dashes
	end

	if item.m_templateSpecParamArray and #item.m_templateSpecParamArray > 0 then
		if not s_indexFormat then
			s_indexFormat = "%0" .. #tostring (g_indexedItemCount) + 1 .. "d"
		end

		s = s .. "_spec_" .. string.format (s_indexFormat, item.m_index)
	end

	-- now, take care of name collisions (e.g. due to template specialization classes)

	local mapValue = g_itemFileNameMap [s]

	if mapValue == nil then
		mapValue = {}
		mapValue.m_item = item
		mapValue.m_index = 2
		g_itemFileNameMap [s] = mapValue
	elseif mapValue.m_item ~= item then
		s = s .. "_" .. prevMapValue.m_index
		mapValue.m_index = mapValue.m_index + 1
	end

	s = s .. ".rst"

	return s
end

function getCompoundTocTree (compound, indent)
	local s = ""

	for i = 1, #compound.m_groupArray do
		local fileName = getItemFileName (compound.m_groupArray [i], compound)
		s = s .. indent .. fileName .. "\n"
	end

	for i = 1, #compound.m_namespaceArray do
		local fileName = getItemFileName (compound.m_namespaceArray [i], compound)
		s = s .. indent .. fileName .. "\n"
	end

	for i = 1, #compound.m_enumArray do
		local fileName = getItemFileName (compound.m_enumArray [i], compound)
		s = s .. indent .. fileName .. "\n"
	end

	for i = 1, #compound.m_structArray do
		local fileName = getItemFileName (compound.m_structArray [i], compound)
		s = s .. indent .. fileName .. "\n"
	end

	for i = 1, #compound.m_unionArray do
		local fileName = getItemFileName (compound.m_unionArray [i], compound)
		s = s .. indent .. fileName .. "\n"
	end

	for i = 1, #compound.m_classArray do
		local fileName = getItemFileName (compound.m_classArray [i], compound)
		s = s .. indent .. fileName .. "\n"
	end

	return s
end

function getTitle (title, underline)
	if not title or  title == "" then
		title = "<Untitled>"
	end

	return title .. "\n" .. string.rep (underline, #title)
end

function getFunctionDeclStringImpl (item, returnType, isRef, indent)
	local s = returnType

	if g_hasNewLineAfterReturnType then
		s = s .. "\n" .. indent
	else
		s = s .. " "
	end

	if isRef then
		s = s .. ":ref:`" .. getItemName (item)  .. "<doxid-" .. item.m_id .. ">` "
	else
		s = s .. getItemName (item) ..  " "
	end

	s = s .. getFunctionParamArrayString (
		item.m_paramArray,
		indent
		)

	return s
end

function getFunctionDeclString (func, isRef, indent)
	return getFunctionDeclStringImpl (
		func,
		getLinkedTextString (func.m_returnType),
		isRef,
		indent
		)
end

function getEventDeclString (event, isRef, indent)
	return getFunctionDeclStringImpl (
		event,
		"event",
		isRef,
		indent
		)
end

function getTypedefDeclString (typedef, isRef, indent)
	local s = "typedef"

	if typedef.m_argString == "" then
		s = s .. " " .. getLinkedTextString (typedef.m_type) .. " "

		if isRef then
			s = s .. ":ref:`" .. getItemName (typedef)  .. "<doxid-" .. typedef.m_id .. ">` "
		else
			s = s .. getItemName (typedef) ..  " "
		end

		return s
	end

	if g_hasNewLineAfterReturnType then
		s = s .. "\n" .. indent
	else
		s = s .. " "
	end

	s = s .. getLinkedTextString (typedef.m_type) .. "\n"

	if isRef then
		s = s .. indent .. ":ref:`" .. getItemName (typedef)  .. "<doxid-" .. typedef.m_id .. ">` ("
	else
		s = s .. indent .. getItemName (typedef) ..  " ("
	end

	if not string.find (typedef.m_argString, ",") then
		local arg = string.match (typedef.m_argString, "%(([^()]+)%)")
		if arg then
			s = s .. arg .. ")"
		else
			s = s .. ")"
		end
	else
		s = s .. "\n"

		for arg, term in string.gmatch (typedef.m_argString, "%s*([^,()]+)([,)])") do
			s = s .. indent .. "    " .. arg
			if term == "," then
				s = s .. ",\n"
			else
				s = s .. "\n"
			end
		end

		s = s .. indent .. "    )"
	end

	return s
end

function getItemBriefDocumentation (item, refPrefix)
	if item.m_briefDescription.m_isEmpty then
		return ""
	end

	local s = ""

	for i = 1, #item.m_briefDescription.m_docBlockList do
		block = item.m_briefDescription.m_docBlockList [i]
		s = s .. block.m_contents.m_plainText

		if i ~= #item.m_briefDescription.m_docBlockList then
			s = s .. "\n"
		end
	end

	s = s .. " :ref:`More...<" .. refPrefix .. "doxid-" .. item.m_id .. ">`"

	return s
end

function getItemDetailedDocumentation (item)
	local s = ""

	for i = 1, #item.m_briefDescription.m_docBlockList do
		block = item.m_briefDescription.m_docBlockList [i]
		s = s .. block.m_contents.m_plainText .. "\n"
	end

	if not item.m_briefDescription.m_isEmpty and not item.m_detailedDescription.m_isEmpty then
		s = s .. "\n"
	end

	for i = 1, #item.m_detailedDescription.m_docBlockList do
		block = item.m_detailedDescription.m_docBlockList [i]
		s = s .. block.m_contents.m_plainText .. "\n"
	end

	return s
end

function removePrimitiveTypedefs (typedefArray)
	if next (typedefArray) == nil then
		return
	end

	for i = #typedefArray, 1, -1 do
		typedef = typedefArray [i]

		local typeKind, name = string.match (
			typedef.m_type.m_plainText,
			"(%a+)%s+(%w[%w_]*)"
			)

		if name == typedef.m_name then
			table.remove (typedefArray, i)
		end
	end
end

-------------------------------------------------------------------------------
