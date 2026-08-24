-- CoolTerm 自动回包（修正语法版）
-- 用法：脚本编辑器 → 打开 → 运行；或 osascript coolterm_auto_reply.applescript

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

-- 匹配逻辑放在 tell 外，避免 CoolTerm 词典冲突
on isQueryVersionRequest(theBuf)
	return (theBuf contains "027F080101FF0643")
end isQueryVersionRequest

on run
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
		
		repeat
			Poll winID
			
			set avail to BytesAvailable (winID)
			if avail >= 8 then
				set buf to my normalizeHex(LookAheadHex (winID))
				
				if my isQueryVersionRequest(buf) then
					-- 回包: V1.0.0
					WriteHex {winID, "02 7F 0E 01 01 00 56 31 2E 30 2E 30 CF D8"}
					ReadHex {winID, 8}
				end if
			end if
			
			delay 0.02
		end repeat
	end tell
end run
