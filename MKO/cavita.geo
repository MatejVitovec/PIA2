//+
Point(1) = {0, 0, 0, 0.025};
//+
Point(2) = {1, 0, 0, 0.025};
//+
Point(3) = {0, 1, 0, 0.025};
//+
Point(4) = {1, 1, 0, 0.025};
//+
Line(1) = {1, 2};
//+
Line(2) = {2, 4};
//+
Line(3) = {4, 3};
//+
Line(4) = {3, 1};
//+
Curve Loop(1) = {4, 1, 2, 3};
//+
Plane Surface(1) = {1};
//+
Physical Curve("top", 5) = {3};
//+
Physical Curve("left", 6) = {4};
//+
Physical Curve("right", 7) = {2};
//+
Physical Curve("bottom", 8) = {1};
//+
Physical Surface("inside", 9) = {1};
