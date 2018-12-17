'TD0G Pancake Printer gCode Processor
'Written by Tyler Gerritsen 2018-12-12
'vtgerritsen@gmail.com

'Compatible with PancakePainter 1.2.0 beta
'Written for IREnE Firmware 1.0



'###################################################################################

			'Script Configuration

'###################################################################################

			'Version
			version = 0.1

			'Travel Speed (mm/min)
			travelSpeed = 1500
			
			'Print Speed (mm/min)
			printSpeed = 700
			
			'Extrusion Factor
			extrusionFactor = 1.3

			
'###################################################################################

			'Changelog

'###################################################################################


'0.1
'	2018-12-12
'	Functional
	

'###################################################################################

			'Script

'###################################################################################

xMax = -1000000.0
xMin = 1000000.0
yMax = -1000000.0
yMin = 1000000.0

If WScript.Arguments.Count > 0 Then
	sFolder = Left(WScript.ScriptFullName, InStrRev(WScript.ScriptFullName, "\"))
	Set fso = CreateObject("Scripting.FileSystemObject")
	set iFile = fso.OpenTextFile(WScript.Arguments(0))
	
'Get limits
	do until iFile.AtEndOfStream
		newArg = iFile.ReadLine
		newArgParse = split(newArg)
		if left(newArg, 4) = "G00 " or left(newArg, 4) = "G01 " then
			if left(newArg, 9) <> "G00 X1 Y1" then
				for i = 0 to ubound(newArgParse)
					if left(newArgParse(i), 1) = "X" then
						xval = cDbl(right(newArgParse(i), len(newArgParse(i)) - 1))
						if xval > xMax then xMax = xval
						if xval < xMin then xMin = xval
					elseif left(newArgParse(i), 1) = "Y" then
						yval = cDbl(right(newArgParse(i), len(newArgParse(i)) - 1))
						if yval > yMax then yMax = yval
						if yval < yMin then yMin = yval
					end if	
				next
			end if
		end if
	loop
	iFile.close
	
'Write oFile
	printingOn = 0
	speedSet = 0
	printDistance = 0
	oldX = 0.0
	oldY = 0.0
	newX = 0.0
	newY = 0.0
	

	set iFile = fso.OpenTextFile(WScript.Arguments(0))	
	set oFile = fso.CreateTextFile(replace(WScript.Arguments(0), ".","_e."))	
	
'Header
	oFile.writeline(iFile.ReadLine)
	oFile.writeline(iFile.ReadLine)
	oFile.writeLine()
	oFile.write("; Edited using TD0G Pancake Gcode Processor v")
	oFile.writeLine(version)
	oFile.writeLine()
	oFile.write("; Print offset (X, Y) = ")
	oFile.write(xMin)
	oFile.write(", ")
	oFile.writeLine(yMin)
	oFile.write("; Max X = ")
	oFile.write(xMax - xMin)
	oFile.writeLine(" mm")
	oFile.write("; Max Y = ")
	oFile.write(yMax - yMin)
	oFile.writeLine(" mm")
	oFile.writeLine()
	oFile.write("; Travel Speed = ")
	oFile.write(travelSpeed)
	oFile.writeLine(" mm/min")
	oFile.write("; Print Speed = ")
	oFile.write(printSpeed)
	oFile.writeLine(" mm/min")
	oFile.write("; Extrusion Factor = ")
	oFile.writeLine(extrusionFactor)
	oFile.writeLine()
	
'GCODE
	oFile.writeline("G91")
	oFile.writeLine("M3")
	if extrusionFactor <> 0 then
		oFile.write("M221 S")
		oFile.writeLine(extrusionFactor)
	end if
	do until iFile.AtEndOfStream																		
		newArg = iFile.ReadLine
		if left(newArg, 1) <> "W" and left(newArg, 3) <> "G21" and left(newArg, 3) <> "G4 " and left(newArg, 4) <> "G28 " and left(newArg, 9) <> "G00 X1 Y1" and left(newArg, 1) <> ";" and len(newArg) > 0 then
			if left(newArg, 4) = "G00 " or left(newArg, 4) = "G01 " or left(newArg, 3) = "G1 " or left(newArg, 3) = "G0 " then
				if inStr(newArg, "Y") > 0 or inStr(newArg, "X") > 0 then
					newArgParse = split(newArg)
					if printingOn = 1 then 
						oFile.write("G1")
						if speedSet = 0 or speedSet = 1 then
							oFile.write(" F")
							oFile.write(printSpeed)
							speedSet = speedSet + 2
						end if
					else
						oFile.write("G0")
						if speedSet = 0 or speedSet = 2 then
							oFile.write(" F")
							oFile.write(travelSpeed)
							speedSet = speedSet + 1
						end if
					end if
					for i = 1 to ubound(newArgParse)
						if left(newArgParse(i), 1) = "X" then
							xval = cDbl(right(newArgParse(i), len(newArgParse(i)) - 1))
							xval = xval - xMin
							oFile.write(" X")
							oFile.write(round(xval, 3))
							newX = xval
						elseif left(newArgParse(i), 1) = "Y" then
							yval = cDbl(right(newArgParse(i), len(newArgParse(i)) - 1))
							yval = yval - yMin
							oFile.write(" Y")
							oFile.write(round(yval, 3))
							newY = yval
						end if
					next
					if printingOn = 1 then
						printDistance = printDistance + sqr((oldX - newX) * (oldX - newX) + (oldY - newY) * (oldY - newY))
					end if
					oFile.writeLine("")
					oldX = newX
					oldY = newY
				end if
			else
				if newArg = "M3" or left(newArg, 4) = "M107" then
					printingOn = 0
				elseif newArg = "M5" or left(newArg, 4) = "M106" then
					printingOn = 1
				end if
				oFile.writeLine(newArg)
			end if
		end if
	loop
	oFile.writeLine("")
	oFile.write("; Print distance = ")
	oFile.write(round(printDistance, 3))
	oFile.write(" mm")
	iFile.close
	oFile.close
	
end if
