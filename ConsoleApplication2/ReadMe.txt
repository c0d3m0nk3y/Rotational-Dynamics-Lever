###############################################################################
Stephen Thomas															P138766
###############################################################################
Implementing a 2D lever system as a basis for working into a 3D physics engine
###############################################################################
Controls:
	Move fulcrum right: d
				 left: a

	Move right side weight right: e
						   left: q

	Move left side weight right: c
						  left: z

	Toggle physics mode on/off: s

	Toggle move rate fast/slow: x

	Toggle force direction(yellow ball) up/down: w

Legend:
Blue ball: Weight load
Yellow ball: Force load
Red bar: Lever
White ball: Fulcrum


WL = Weight load
FL = Force load
F = Fulcrum
^ or v = force direction
First class lever:

WL				FL
------------------
		F		v


Second class lever:


^		WL
------------------
FL				F

Third class lever:

WL		 ^		F
-----------------
		 FL






