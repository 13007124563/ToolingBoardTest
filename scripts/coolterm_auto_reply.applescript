-- CoolTerm 测试治具协议自动回包（3.1–3.7，V1.1）
-- 与协议文档及 scripts/protocol_test.py 对齐

on normalizeHex(h)
	set h to my replaceText(h, " ", "")
	set h to my replaceText(h, return, "")
	set h to my replaceText(h, linefeed, "")
	return h
end normalizeHex

on replaceText(theText, searchStr, replaceStr)
	set AppleScript's text item delimiters to searchStr
	set parts to text items of theText
	set AppleScript's text item delimiters to replaceStr
	set newText to parts as text
	set AppleScript's text item delimiters to ""
	return newText
end replaceText

on bufferHasPattern(theBuf, thePat)
	return (theBuf contains thePat)
end bufferHasPattern

on replyForBuffer(winID, theBuf, caseList)
	repeat with oneCase in caseList
		set reqPat to item 1 of contents of oneCase
		if my bufferHasPattern(theBuf, reqPat) then
			set rspHex to item 2 of contents of oneCase
			set consumeBytes to item 3 of contents of oneCase
			tell application "CoolTerm"
				WriteHex {winID, rspHex}
				ReadHex {winID, consumeBytes}
			end tell
			return true
		end if
	end repeat
	return false
end replyForBuffer

on run
	set protocolCases to {¬
		{"027F080101FF0643", "02 7F 15 01 01 00 56 31 2E 30 2E 30 2E 32 36 30 35 33 30 5E DD", 8}, ¬
		{"027F080102FF06B3", "02 7F 11 01 02 00 00 04 BA 00 01 F1 00 01 49 A3 09", 8}, ¬
		{"027F090103FF005F02", "02 7F 0B 01 03 00 00 04 B4 A9 52", 9}, ¬
		{"027F090103FF019EC2", "02 7F 0B 01 03 00 00 04 B4 A9 52", 9}, ¬
		{"027F090104FF00EEC3", "02 7F 0B 01 04 00 00 01 F0 1F F1", 9}, ¬
		{"027F090104FF012F03", "02 7F 0B 01 04 00 00 01 F0 1F F1", 9}, ¬
		{"027F090105FF00BF03", "02 7F 0B 01 05 00 00 04 B5 E0 92", 9}, ¬
		{"027F090105FF017EC3", "02 7F 0B 01 05 00 00 04 B5 E0 92", 9}, ¬
		{"027F090106FF004F03", "02 7F 0B 01 06 00 00 01 F0 66 31", 9}, ¬
		{"027F090106FF018EC3", "02 7F 0B 01 06 00 00 01 F0 66 31", 9}, ¬
		{"027F090107FF001EC3", "02 7F 09 01 07 00 00 5F 33", 9}, ¬
		{"027F090107FF01DF03", "02 7F 09 01 07 00 00 5F 33", 9} ¬
	}
	
	tell application "CoolTerm"
		set winID to WindowID (0)
		if winID < 0 then
			display alert "请先打开 CoolTerm 窗口"
			return
		end if
		if not (Connect winID) then
			display alert "串口连接失败，请检查端口和占用"
			return
		end if
	end tell
	
	repeat
		tell application "CoolTerm"
			Poll winID
			set avail to BytesAvailable (winID)
			if avail >= 8 then
				set buf to my normalizeHex(LookAheadHex (winID))
			else
				set buf to ""
			end if
		end tell
		
		if buf is not "" then
			my replyForBuffer(winID, buf, protocolCases)
		end if
		
		delay 0.02
	end repeat
end run
