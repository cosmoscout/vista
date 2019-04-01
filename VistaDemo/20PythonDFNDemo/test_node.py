import colorsys

# simple demonstration script which takes mouse x,y coordinates, computes a HSV value and outputs RGB values

def doEval():	
	# approximately normalize mouse coordinates within window
	mouse_x = dfnNode.getInPortValue("in_x") / 1000.0
	mouse_y = dfnNode.getInPortValue("in_y") / 1000.0

	h,s,v = mouse_x, mouse_y, 1.0
	r,g,b = colorsys.hsv_to_rgb(h,s,v)
	print "h,s,v = ", h,s,v
	dfnNode.setOutPortValue("out_r", r)
	dfnNode.setOutPortValue("out_g", g)
	dfnNode.setOutPortValue("out_b", b)

# 'dfnNode' is a global variable injected by the C++ PyDfnNode object which represents that object.
# it provides methods to register a callback handler, input/output port and to get/set port values (used from within the handler, see above)

print "Hello World from Python!"

dfnNode.registerEvalCallback(doEval)
dfnNode.registerInPortInt("in_x")
dfnNode.registerInPortInt("in_y")
dfnNode.registerOutPortFloat("out_r")
dfnNode.registerOutPortFloat("out_g")
dfnNode.registerOutPortFloat("out_b")


