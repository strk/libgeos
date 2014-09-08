// lineclip tests:

	char * mytests[75][2] = {
	  // inside
	  {
		"LINESTRING (1 1,1 9,9 9,9 1)",
		"LINESTRING (1 1,1 9,9 9,9 1)"
	  },
	  // outside
	  {
		"LINESTRING (-1 -9,-1 11,9 11)",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // go in from left
	  {
		"LINESTRING (-1 5,5 5,9 9)",
		"LINESTRING (0 5,5 5,9 9)"
	  },
	  // go out from right
	  {
		"LINESTRING (5 5,8 5,12 5)",
		"LINESTRING (5 5,8 5,10 5)"
	  },
	  // go in and out
	  {
		"LINESTRING (5 -1,5 5,1 2,-3 2,1 6)",
		"MULTILINESTRING ((5 0,5 5,1 2,0 2),(0 5,1 6))"
	  },
	  // go along left edge
	  {
		"LINESTRING (0 3,0 5,0 7)",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // go out from left edge
	  {
		"LINESTRING (0 3,0 5,-1 7)",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // go in from left edge
	  {
		"LINESTRING (0 3,0 5,2 7)",
		"LINESTRING (0 5,2 7)"
	  },
	  // triangle corner at bottom left corner
	  {
		"LINESTRING (2 1,0 0,1 2)",
		"LINESTRING (2 1,0 0,1 2)"
	  },
	  // go from in to edge and back in
	  {
		"LINESTRING (3 3,0 3,0 5,2 7)",
		"MULTILINESTRING ((3 3,0 3),(0 5,2 7))"
	  },
	  // go from in to edge and then straight out
	  {
		"LINESTRING (5 5,10 5,20 5)",
		"LINESTRING (5 5,10 5)"
	  },
	  // triangle corner at left edge
	  {
		"LINESTRING (3 3,0 6,3 9)",
		"LINESTRING (3 3,0 6,3 9)"
	  },
	  // polygon completely inside
	  {
		"POLYGON ((5 5,5 6,6 6,6 5,5 5))",
		"POLYGON ((5 5,5 6,6 6,6 5,5 5))"
	  },
	  // polygon completely outside
	  {
		"POLYGON ((15 15,15 16,16 16,16 15,15 15))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // polygon surrounds the rectangle
	  {
		"POLYGON ((-1 -1,-1 11,11 11,11 -1,-1 -1))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // polygon cuts the rectangle
	  {
		"POLYGON ((-1 -1,-1 5,5 5,5 -1,-1 -1))",
		"LINESTRING (0 5,5 5,5 0)"
	  },
	  // polygon with hole cuts the rectangle
	  {
		"POLYGON ((-2 -2,-2 5,5 5,5 -2,-2 -2), (3 3,4 4,4 2,3 3))",
		"GEOMETRYCOLLECTION (POLYGON ((3 3,4 4,4 2,3 3)),LINESTRING (0 5,5 5,5 0))"
	  },
	  // rectangle cuts both the polygon and the hole
	  {
		"POLYGON ((-2 -2,-2 5,5 5,5 -2,-2 -2), (-1 -1,3 1,3 3,-1 -1))",
		"MULTILINESTRING ((0 5,5 5,5 0),(1 0,3 1,3 3,0 0))"
	  },
	  // Triangle at two corners and one edge
	  {
		"POLYGON ((0 0,10 0,5 10,0 0))",
		"LINESTRING (10 0,5 10,0 0)",
	  },
	  // Same triangle with another starting point
	  {
		"POLYGON ((5 10,0 0,10 0,5 10))",
		"LINESTRING (10 0,5 10,0 0)"
	  },
	  // Triangle intersection at corner and edge
	  {
		"POLYGON ((-5 -5,5 5,5 -5,-5 -5))",
		"LINESTRING (0 0,5 5,5 0)"
	  },
	  // Triangle intersection at adjacent edges
	  {
		"POLYGON ((-1 5,5 11,5 5,-1 5))",
		"MULTILINESTRING ((0 6,4 10),(5 10,5 5,0 5))"
	  },
	  // One triangle intersection and one inside edge
	  {
		"POLYGON ((-5 5,5 10,5 5,-5 5))",
		"LINESTRING (0.0 7.5,5 10,5 5,0 5)"
	  },
	  // Triangle intersection at center and end of the same edge
	  {
		"POLYGON ((-10 5,10 10,10 5,-10 5))",
		"MULTILINESTRING ((0.0 7.5,10 10),(10 5,0 5))"
	  },
	  // Two different edges clips
	  {
		"POLYGON ((-5 5,15 15,15 5,-5 5))",
		"MULTILINESTRING ((0.0 7.5,5 10),(10 5,0 5))"
	  },
	  // Inside triangle with all corners at edges
	  {
		"POLYGON ((0 5,5 10,10 5,0 5))",
		"POLYGON ((0 5,5 10,10 5,0 5))"
	  },
	  // Inside triangle whose base is one of the edges
	  {
		"POLYGON ((0 0,5 5,10 0,0 0))",
		"LINESTRING (0 0,5 5,10 0)"
	  },
	  // Triangle touching two corners on the outside
	  {
		"POLYGON ((-5 5,5 15,15 5,-5 5))",
		"LINESTRING (10 5,0 5)"
	  },
	  // Triangle with a diagonal and sharing two edges
	  {
		"POLYGON ((0 0,10 10,10 0,0 0))",
		"LINESTRING (0 0,10 10)"
	  },
	  // Triangle exits edge at a corner
	  {
		"POLYGON ((-5 0,5 10,5 0,-5 0))",
		"LINESTRING (0 5,5 10,5 0)"
	  },
	  // Triangle enters edge at a corner
	  {
		"POLYGON ((-5 10,5 10,0 0,-5 10))",
		"LINESTRING (5 10,0 0)"
	  },
	  // Triangle enters and exits the same edge
	  {
		"POLYGON ((-5 0,5 10,15 0,-5 0))",
		"LINESTRING (0 5,5 10,10 5)"
	  },
	  // Triangle enters at a corner and exits at another
	  {
		"POLYGON ((-5 -5,15 15,15 -5,-5 -5))",
		"LINESTRING (0 0,10 10)"
	  },
	  // From outside to nearest edge etc
	  {
		"POLYGON ((-5 -5,0 5,5 0,-5 -5))",
		"LINESTRING (0 5,5 0)"
	  },
	  // From outside to opposite edge etc
	  {
		"POLYGON ((-10 5,10 5,0 -5,-10 5))",
		"LINESTRING (0 5,10 5,5 0)"
	  },
	  // Drew all combinations I could imagine on paper, and added the following.
	  // All triangles fully inside
	  {
		"POLYGON ((0 0,0 10,10 10,0 0))",
		"LINESTRING (10 10,0 0)"
	  },
	  {
		"POLYGON ((0 5,0 10,10 10,0 5))",
		"LINESTRING (10 10,0 5)"
	  },
	  {
		"POLYGON ((0 10,10 10,5 0,0 10))",
		"LINESTRING (10 10,5 0,0 10)"
	  },
	  {
		"POLYGON ((0 10,10 10,5 5,0 10))",
		"LINESTRING (10 10,5 5,0 10)"
	  },
	  {
		"POLYGON ((0 10,5 10,0 5,0 10))",
		"LINESTRING (5 10,0 5)"
	  },
	  {
		"POLYGON ((0 10,10 5,0 5,0 10))",
		"LINESTRING (0 10,10 5,0 5)"
	  },
	  {
		"POLYGON ((0 10,10 0,0 5,0 10))",
		"LINESTRING (0 10,10 0,0 5)"
	  },
	  {
		"POLYGON ((0 10,5 0,0 5,0 10))",
		"LINESTRING (0 10,5 0,0 5)"
	  },
	  {
		"POLYGON ((0 10,5 5,0 5,0 10))",
		"LINESTRING (0 10,5 5,0 5)"
	  },
	  {
		"POLYGON ((0 10,7 7,3 3,0 10))",
		"POLYGON ((0 10,7 7,3 3,0 10))"
	  },
	  {
		"POLYGON ((0 10,5 5,5 0,0 10))",
		"POLYGON ((0 10,5 5,5 0,0 10))"
	  },
	  {
		"POLYGON ((0 10,10 5,5 0,0 10))",
		"POLYGON ((0 10,10 5,5 0,0 10))"
	  },
	  {
		"POLYGON ((2 5,5 7,7 5,2 5))",
		"POLYGON ((2 5,5 7,7 5,2 5))"
	  },
	  {
		"POLYGON ((2 5,5 10,7 5,2 5))",
		"POLYGON ((2 5,5 10,7 5,2 5))"
	  },
	  {
		"POLYGON ((0 5,5 10,5 5,0 5))",
		"POLYGON ((0 5,5 10,5 5,0 5))"
	  },
	  {
		"POLYGON ((0 5,5 10,10 5,0 5))",
		"POLYGON ((0 5,5 10,10 5,0 5))"
	  },
	  {
		"POLYGON ((0 5,5 7,10 5,0 5))",
		"POLYGON ((0 5,5 7,10 5,0 5))"
	  },
	  // No points inside, one intersection
	  {
		"POLYGON ((-5 10,0 15,0 10,-5 10))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  {
		"POLYGON ((-5 10,0 5,-5 0,-5 10))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // No points inside, two intersections
	  {
		"POLYGON ((-5 5,0 10,0 0,-5 5))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  {
		"POLYGON ((-5 5,0 10,0 5,-5 5))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  {
		"POLYGON ((-5 5,0 7,0 3,-5 5))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // One point inside
	  {
		"POLYGON ((5 5,-5 0,-5 10,5 5))",
		"LINESTRING (0.0 7.5,5 5,0.0 2.5)",
	  },
	  {
		"POLYGON ((5 0,-5 0,-5 10,5 0))",
		"LINESTRING (0 5,5 0)"
	  },
	  {
		"POLYGON((10 0,-10 0,-10 10,10 0))",
		"LINESTRING (0 5,10 0)"
	  },
	  {
		"POLYGON ((5 0,-5 5,-5 10,5 0))",
		"LINESTRING (0 5,5 0,0.0 2.5)"
	  },
	  {
		"POLYGON ((10 5,-10 0,-10 10,10 5))",
		"LINESTRING (0.0 7.5,10 5,0.0 2.5)"
	  },
	  {
		"POLYGON ((10 10,-10 0,-10 5,10 10))",
		"LINESTRING (0.0 7.5,10 10,0 5)"
	  },
	  {
		"POLYGON ((5 5,-5 -5,-5 15,5 5))",
		"LINESTRING (0 10,5 5,0 0)"
	  },
	  {
		"POLYGON ((10 5,-10 -5,-10 15,10 5))",
		"LINESTRING (0 10,10 5,0 0)"
	  },
	  {
		"POLYGON ((5 0,-5 0,-5 20,5 0))",
		"LINESTRING (0 10,5 0)",
	  },
	  {
		"POLYGON ((10 0,-10 0,-10 20,10 0))",
		"LINESTRING (0 10,10 0)"
	  },
	  {
		"POLYGON ((5 5,-10 5,0 15,5 5))",
		"LINESTRING (2.5 10.0,5 5,0 5)",
	  },
	  {
		"POLYGON ((5 5,-5 -5,0 15,5 5))",
		"LINESTRING (2.5 10.0,5 5,0 0)"
	  },
	  {
		"POLYGON ((5 5,-15 -20,-15 30,5 5))",
		"LINESTRING (1 10,5 5,1 0)",
	  },
	  // Two points inside
	  {
		"POLYGON ((5 7,5 3,-5 5,5 7))",
		"LINESTRING (0 6,5 7,5 3,0 4)"
	  },
	  {
		"POLYGON ((5 7,5 3,-5 13,5 7))",
		"LINESTRING (0 10,5 7,5 3,0 8)"
	  },
	  {
		"POLYGON ((6 6,4 4,-4 14,6 6))",
		"LINESTRING (1.0 10.0,6 6,4 4,0 9)"
	  },
	  // Polygon with hole which surrounds the rectangle
	  {
		"POLYGON ((-2 -2,-2 12,12 12,12 -2,-2 -2),(-1 -1,11 -1,11 11,-1 11,-1 -1))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // Polygon surrounding the rect, but with a hole inside the rect
	  {
		"POLYGON ((-2 -2,-2 12,12 12,12 -2,-2 -2),(1 1,9 1,9 9,1 9,1 1))",
		"POLYGON ((1 1,9 1,9 9,1 9,1 1))"
	  }
	};

// polyclip tests

	char * mytests[67][2] = {
	  // inside
	  {
		"LINESTRING (1 1,1 9,9 9,9 1)",
		"LINESTRING (1 1,1 9,9 9,9 1)"
	  },
	  // outside
	  {
		"LINESTRING (-1 -9,-1 11,9 11)",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // go in from left
	  {
		"LINESTRING (-1 5,5 5,9 9)",
		"LINESTRING (0 5,5 5,9 9)"
	  },
	  // go out from right
	  {
		"LINESTRING (5 5,8 5,12 5)",
		"LINESTRING (5 5,8 5,10 5)"
	  },
	  // go in and out
	  {
		"LINESTRING (5 -1,5 5,1 2,-3 2,1 6)",
		"MULTILINESTRING ((5 0,5 5,1 2,0 2),(0 5,1 6))"
	  },
	  // go along left edge
	  {
		"LINESTRING (0 3,0 5,0 7)",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // go out from left edge
	  {
		"LINESTRING (0 3,0 5,-1 7)",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // go in from left edge
	  {
		"LINESTRING (0 3,0 5,2 7)",
		"LINESTRING (0 5,2 7)"
	  },
	  // triangle corner at bottom left corner
	  {
		"LINESTRING (2 1,0 0,1 2)",
		"LINESTRING (2 1,0 0,1 2)"
	  },
	  // go from in to edge and back in
	  {
		"LINESTRING (3 3,0 3,0 5,2 7)",
		"MULTILINESTRING ((3 3,0 3),(0 5,2 7))"
	  },
	  // go from in to edge and then straight out
	  {
		"LINESTRING (5 5,10 5,20 5)",
		"LINESTRING (5 5,10 5)"
	  },
	  // triangle corner at left edge
	  {
		"LINESTRING (3 3,0 6,3 9)",
		"LINESTRING (3 3,0 6,3 9)"
	  },
	  // polygon completely inside
	  {
		"POLYGON ((5 5,5 6,6 6,6 5,5 5))",
		"POLYGON ((5 5,5 6,6 6,6 5,5 5))"
	  },
	  // polygon completely outside
	  {
		"POLYGON ((15 15,15 16,16 16,16 15,15 15))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // polygon surrounds the rectangle
	  {
		"POLYGON ((-1 -1,-1 11,11 11,11 -1,-1 -1))",
		"POLYGON ((0 0,0 10,10 10,10 0,0 0))"
	  },
	  // polygon cuts the rectangle
	  {
		"POLYGON ((-1 -1,-1 5,5 5,5 -1,-1 -1))",
		"POLYGON ((0 0,0 5,5 5,5 0,0 0))"
	  },
	  // polygon with hole cuts the rectangle
	  {
		"POLYGON ((-2 -2,-2 5,5 5,5 -2,-2 -2), (3 3,4 4,4 2,3 3))",
		"POLYGON ((0 0,0 5,5 5,5 0,0 0),(3 3,4 4,4 2,3 3))"
	  },
	  // rectangle cuts both the polygon and the hole
	  {
		"POLYGON ((-2 -2,-2 5,5 5,5 -2,-2 -2), (-1 -1,3 1,3 3,-1 -1))",
		"POLYGON ((0 0,0 5,5 5,5 0,1 0,3 1,3 3,0 0))"
	  },
	  // Triangle at two corners and one edge
	  {
		"POLYGON ((0 0,10 0,5 10,0 0))",
		"POLYGON ((0 0,10 0,5 10,0 0))",
	  },
	  // Same triangle with another starting point
	  {
		"POLYGON ((5 10,0 0,10 0,5 10))",
		"POLYGON ((0 0,10 0,5 10,0 0))",
	  },
	  // Triangle intersection at corner and edge
	  {
		"POLYGON ((-5 -5,5 5,5 -5,-5 -5))",
		"POLYGON ((0 0,5 5,5 0,0 0))"
	  },
	  // All triangles fully inside
	  {
		"POLYGON ((0 0,0 10,10 10,0 0))",
		"POLYGON ((0 0,0 10,10 10,0 0))"
	  },
	  {
		"POLYGON ((0 5,0 10,10 10,0 5))",
		"POLYGON ((0 5,0 10,10 10,0 5))"
	  },
	  {
		"POLYGON ((0 10,10 10,5 0,0 10))",
		"POLYGON ((0 10,10 10,5 0,0 10))"
	  },
	  {
		"POLYGON ((0 10,10 10,5 5,0 10))",
		"POLYGON ((0 10,10 10,5 5,0 10))"
	  },
	  {
		"POLYGON ((0 10,5 10,0 5,0 10))",
		"POLYGON ((0 5,0 10,5 10,0 5))"
	  },
	  {
		"POLYGON ((0 10,10 5,0 5,0 10))",
		"POLYGON ((0 5,0 10,10 5,0 5))"
	  },
	  {
		"POLYGON ((0 10,10 0,0 5,0 10))",
		"POLYGON ((0 5,0 10,10 0,0 5))"
	  },
	  {
		"POLYGON ((0 10,5 0,0 5,0 10))",
		"POLYGON ((0 5,0 10,5 0,0 5))"
	  },
	  {
		"POLYGON ((0 10,5 5,0 5,0 10))",
		"POLYGON ((0 5,0 10,5 5,0 5))"
	  },
	  {
		"POLYGON ((0 10,7 7,3 3,0 10))",
		"POLYGON ((0 10,7 7,3 3,0 10))"
	  },
	  {
		"POLYGON ((0 10,5 5,5 0,0 10))",
		"POLYGON ((0 10,5 5,5 0,0 10))"
	  },
	  {
		"POLYGON ((0 10,10 5,5 0,0 10))",
		"POLYGON ((0 10,10 5,5 0,0 10))"
	  },
	  {
		"POLYGON ((2 5,5 7,7 5,2 5))",
		"POLYGON ((2 5,5 7,7 5,2 5))"
	  },
	  {
		"POLYGON ((2 5,5 10,7 5,2 5))",
		"POLYGON ((2 5,5 10,7 5,2 5))"
	  },
	  {
		"POLYGON ((0 5,5 10,5 5,0 5))",
		"POLYGON ((0 5,5 10,5 5,0 5))"
	  },
	  {
		"POLYGON ((0 5,5 10,10 5,0 5))",
		"POLYGON ((0 5,5 10,10 5,0 5))"
	  },
	  {
		"POLYGON ((0 5,5 7,10 5,0 5))",
		"POLYGON ((0 5,5 7,10 5,0 5))"
	  },
	  // No points inside, one intersection
	  {
		"POLYGON ((-5 10,0 15,0 10,-5 10))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  {
		"POLYGON ((-5 10,0 5,-5 0,-5 10))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // No points inside, two intersections
	  {
		"POLYGON ((-5 5,0 10,0 0,-5 5))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  {
		"POLYGON ((-5 5,0 10,0 5,-5 5))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  {
		"POLYGON ((-5 5,0 7,0 3,-5 5))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // One point inside
	  {
		"POLYGON ((5 5,-5 0,-5 10,5 5))",
		"POLYGON ((0.0 2.5,0.0 7.5,5 5,0.0 2.5))"
	  },
	  {
		"POLYGON ((5 0,-5 0,-5 10,5 0))",
		"POLYGON ((0 0,0 5,5 0,0 0))"
	  },
	  {
		"POLYGON ((10 0,-10 0,-10 10,10 0))",
		"POLYGON ((0 0,0 5,10 0,0 0))"
	  },
	  {
		"POLYGON ((5 0,-5 5,-5 10,5 0))",
		"POLYGON ((0.0 2.5,0 5,5 0,0.0 2.5))"
	  },
	  {
		"POLYGON ((10 5,-10 0,-10 10,10 5))",
		"POLYGON ((0.0 2.5,0.0 7.5,10 5,0.0 2.5))"
	  },
	  {
		"POLYGON ((10 10,-10 0,-10 5,10 10))",
		"POLYGON ((0 5,0.0 7.5,10 10,0 5))"
	  },
	  {
		"POLYGON ((5 5,-5 -5,-5 15,5 5))",
		"POLYGON ((0 0,0 10,5 5,0 0))"
	  },
	  {
		"POLYGON ((10 5,-10 -5,-10 15,10 5))",
		"POLYGON ((0 0,0 10,10 5,0 0))"
	  },
	  {
		"POLYGON ((5 0,-5 0,-5 20,5 0))",
		"POLYGON ((0 0,0 10,5 0,0 0))"
	  },
	  {
		"POLYGON ((10 0,-10 0,-10 20,10 0))",
		"POLYGON ((0 0,0 10,10 0,0 0))"
	  },
	  {
		"POLYGON ((5 5,-10 5,0 15,5 5))",
		"POLYGON ((0 5,0 10,2.5 10.0,5 5,0 5))"
	  },
	  {
		"POLYGON ((5 5,-5 -5,0 15,5 5))",
		"POLYGON ((0 0,0 10,2.5 10.0,5 5,0 0))"
	  },
	  {
		"POLYGON ((5 5,-15 -20,-15 30,5 5))",
		"POLYGON ((0 0,0 10,1 10,5 5,1 0,0 0))"
	  },
	  // Two points inside
	  {
		"POLYGON ((5 7,5 3,-5 5,5 7))",
		"POLYGON ((0 4,0 6,5 7,5 3,0 4))"
	  },
	  {
		"POLYGON ((5 7,5 3,-5 13,5 7))",
		"POLYGON ((0 8,0 10,5 7,5 3,0 8))"
	  },
	  {
		"POLYGON ((6 6,4 4,-4 14,6 6))",
		"POLYGON ((0 9,0 10,1.0 10.0,6 6,4 4,0 9))"
	  },
	  // Polygon with hole which surrounds the rectangle
	  {
		"POLYGON ((-2 -2,-2 12,12 12,12 -2,-2 -2),(-1 -1,11 -1,11 11,-1 11,-1 -1))",
		"GEOMETRYCOLLECTION EMPTY"
	  },
	  // Polygon surrounding the rect, but with a hole inside the rect
	  {
		"POLYGON ((-2 -2,-2 12,12 12,12 -2,-2 -2),(1 1,9 1,9 9,1 9,1 1))",
		"POLYGON ((0 0,0 10,10 10,10 0,0 0),(1 1,9 1,9 9,1 9,1 1))"
	  },
	  // Polygon with hole cut at the right corner
	  {
		"POLYGON ((5 5,15 5,15 -5,5 -5,5 5),(8 1,8 -1,9 -1,9 1,8 1))",
		"POLYGON ((5 0,5 5,10 5,10 0,9 0,9 1,8 1,8 0,5 0))"
	  },
	  // Polygon going around a corner
	  {
		"POLYGON ((-6 5,5 5,5 -6,-6 5))",
		"POLYGON ((0 0,0 5,5 5,5 0,0 0))"
	  },
	  // Hole in a corner
	  {
		"POLYGON ((-15 -15,-15 15,15 15,15 -15,-15 -15),(-5 5,-5 -5,5 -5,5 5,-5 5))",
		"POLYGON ((0 5,0 10,10 10,10 0,5 0,5 5,0 5))"
	  },
	  // Hole going around a corner
	  {
		"POLYGON ((-15 -15,-15 15,15 15,15 -15,-15 -15),(-6 5,5 -6,5 5,-6 5))",
		"POLYGON ((0 5,0 10,10 10,10 0,5 0,5 5,0 5))"
	  },
	  // Surround the rectangle, hole outside rectangle
	  {
		"POLYGON ((-15 -15,-15 15,15 15,15 -15,-15 -15),(-5 5,-6 5,-6 6,-5 6,-5 5))",
		"POLYGON ((0 0,0 10,10 10,10 0,0 0))"
	  },
	  // Surround the rectangle, hole outside rectangle but shares edge
	  {
		"POLYGON ((-15 -15,-15 15,15 15,15 -15,-15 -15),(0 5,-1 5,-1 6,0 6,0 5))",
		"POLYGON ((0 0,0 10,10 10,10 0,0 0))"
	  }
	  
	};
