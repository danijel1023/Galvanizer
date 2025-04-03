vertFile = open("Default.vert", "r")
fragFile = open("Default.frag", "r")

defShFile = open("Default.inl", "w")

defSh = "#pragma once"
defSh += "\n\n"

defSh += "static const char* g_defaultVert = R\"("
defSh += vertFile.read()
defSh += ")\";"
defSh += "\n"

defSh += "\n"

defSh += "static const char* g_defaultFrag = R\"("
defSh += fragFile.read()
defSh += ")\";"
defSh += "\n"

defShFile.write(defSh)
