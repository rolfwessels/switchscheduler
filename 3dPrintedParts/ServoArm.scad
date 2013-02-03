
module cilinder()
{
	depth = 4.5;
	diameter = 3.2;
	dw = depth/2;
	dwI = dw/2;

	armDepth = depth/1.5;
	armHeigth = 18;
	armWidth = (diameter)*2;


	armGripsWidth = 5;
	armGripsHeigth = armWidth*1.5;

	difference() {
		union() {
			translate([-armHeigth/2,-armWidth/2,0])
     			cube([armHeigth,armWidth,armDepth]);
			translate([-armHeigth/2-armGripsWidth,-armWidth/2,0])
				cube([armGripsWidth,armGripsHeigth,armDepth]);
			translate([armHeigth/2,-armWidth/2,0])
				cube([armGripsWidth,armGripsHeigth,armDepth]);
			cylinder(depth,diameter+dw,diameter+dw);

			translate([-15,-armWidth/2,0])
				cube([armGripsWidth,armGripsHeigth,armDepth]);
			translate([+10,-armWidth/2,0])
				cube([armGripsWidth,armGripsHeigth,armDepth]);
		}
		translate([0,0,dwI])
			cylinder(depth,diameter,diameter);
		translate([0,0,-0.1])
		cylinder(depth,diameter-dwI,diameter-dwI);
	}
	
	
}

cilinder();

