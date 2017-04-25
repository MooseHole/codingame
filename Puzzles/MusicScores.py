import sys
import math

# Auto-generated code below aims at helping you parse
# the standard input according to the problem statement.

w, h = [int(i) for i in input().split()]
image = input()


# Fill the sheet
sheet = [[False for i  in range(h)] for j in range(w)]
imageTok = image.split()
cursor = 0
fill = False;
for tok in imageTok:
	if tok == "W":
		fill = False
	elif tok == "B":
		fill = True
	else:
		cursorEnd = cursor + int(tok);
		while cursor < cursorEnd:
			y = int(cursor / w)
			x = cursor - (y * w)
			sheet[x][y] = fill
			cursor = cursor + 1

# Find line positions.  Assume all 5(6) lines have same height and spacing
linePositions = [[-1, -1] for i in range(2)]
state = False
for x in range(w):
	if linePositions[1][1] >= 0:
		break
	for y in range(h):
		if linePositions[1][1] >= 0:
			break
		if state != sheet[x][y]:
			state = sheet[x][y]
			# Set lines
			for line in range(2):
				if state and linePositions[line][0] < 0:
					linePositions[line][0] = y
					break
				if not state and linePositions[line][1] < 0:
					linePositions[line][1] = y - linePositions[line][0]
					break

# Erase lines
begin = linePositions[0][0]
spacing = linePositions[1][0] - linePositions[0][0]
height = linePositions[0][1]
for x in range(w):
	for line in range(6):
		for pix in range(height):
			sheet[x][begin + spacing * line + pix] = False
			

# Middle C to High G
tones = []
for tone in range(-1, 11):
	tones.append([begin + tone * (spacing / 2)])
tones[0].append("G")
tones[1].append("F")
tones[2].append("E")
tones[3].append("D")
tones[4].append("C")
tones[5].append("B")
tones[6].append("A")
tones[7].append("G")
tones[8].append("F")
tones[9].append("E")
tones[10].append("D")
tones[11].append("C")
	
# Find notes
noteStartedX = -1
noteEndedX = -1
tune = []
for x in range(w):
	state = False
	blackPartBegin = 0
	columnProfile = []
	
	# Figure out the profile for this column
	for y in range(h):
		if state != sheet[x][y]:
			state = sheet[x][y]
			if state:
				blackPartBegin = y
			if not state:
				columnProfile.append([blackPartBegin, y - blackPartBegin])

	# If no notes here
	if columnProfile == []:
		i = 0
		if noteStartedX >= 0:
			noteEndedX = x-1
	else:
		i = 0
		# If staff, treat same as blank
		if len(columnProfile) > 2:
			if noteStartedX >= 0:
				noteEndedX = x-1
		else:
			# If beginning of a note
			if noteStartedX < 0:
				noteStartedX = x

	# If have the entire note
	if noteStartedX >= 0 and noteEndedX >= 0:
		# Find center of note's pixels
		values = 0
		total = 0
		for noteX in range(noteStartedX, noteEndedX):
			for noteY in range(h):
				if sheet[noteX][noteY]:
					total = total + noteY
					values = values + 1
		centerY = int(total / values)

		# Fine the tone based on the note's center Y
		lowestDifference = 99999999
		chosenTone = "TONE ERROR"
		for tone in tones:
			difference = abs(tone[0] - centerY)
			if difference < lowestDifference:
				lowestDifference = difference
				chosenTone = tone[1]
				
		# Determine quarter or half
		centerX = int(noteStartedX + ((noteEndedX - noteStartedX) / 2))
		noteLength = "LENGTH ERROR"
		for checkY in range(h):
			if sheet[noteStartedX][checkY]:
				# If there is black at the beginning and middle of the note
				if sheet[centerX][checkY]:
					noteLength = "Q"
				else:
					noteLength = "H"

		# Record this note
		tune.append([chosenTone, noteLength])
		
		# Reset for next note
		noteStartedX = -1
		noteEndedX = -1
		

# Write an action using print
# To debug: print("Debug messages...", file=sys.stderr)

# Format the output
output = ""
for note in tune:
	if output != "":
		output += " "
	output += note[0] + note[1]

# Print the answer
print(output)
