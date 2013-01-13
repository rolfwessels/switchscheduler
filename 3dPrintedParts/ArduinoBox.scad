include<arduino.scad>

module Relay()
{
	width = 24;
	height = 20;
	depth = 33;
	color("white")		
		cube([width,height,depth]);
}

module RelayAndBase()
{
	width = 67;
	height = 30;
	depth = 20;
	level0w = 38;
	level0d = 4;
	level1w = 45;
	level1d = 10;
	screwRad = 2/2;
	
	union() {
		//the black bottom
		color("black")
		difference() {
			union() {
				cube([width,height,depth]);
				translate([(width-level1w)/2,0,depth])
					cube([level1w,height,level1d]);
				
			}
			//bottom gap
			translate([(width-level0w)/2,0,0])
					cube([level0w,height,level0d]);
			//screw holes
			translate([width/2,3,0])
				cylinder(r=screwRad, h=depth+level1d+1, $fn=25);
			translate([width/2,height-3,0])
				cylinder(r=screwRad, h=depth+level1d+1, $fn=25);

		}
		//the replay
		
		translate([(width-24)/2,5,depth+level1d])
			Relay();
	}
}

module OtherElements()
{
	translate(relayOffset)
		RelayAndBase();
	translate(arduinoOffset)
		Arduino(0, 0, 0);

}

module ScrewHolder() {
	arduinoRad = 1.2;
	outer = 2;
	depth = 7;
	difference() {
		cylinder(r=arduinoRad+outer, h=depth, $fn=25);
		cylinder(r=arduinoRad, h=depth+1, $fn=25);
	}
}

module BoxScrewHoldes()
{
	down = -1*arduinoOffsetY;
	left = 67/2;
	
		translate(relayOffset)
		union() {
			
			translate([left,13,0])
				ScrewHolder();
			translate([left,37,0])
				ScrewHolder();
		}
		
		//arduino
		translate(arduinoOffset) // from other elements
		union() {
			
			translate([-1,-38.4,down])
				ScrewHolder();
			translate([0,10,down])
				ScrewHolder();
			translate([51,-33.3,down])
				ScrewHolde();
			translate([51,-5.2,down])
				ScrewHolder();
			translate([51,-33.2,down])
				ScrewHolder();
			
		}
	
		
}
module ScrewHolderAndTaper() {
	
	depth = 7;
	mod = 1.5;
	union() {
		ScrewHolder();
		translate([0,0,-depth*mod])
		cylinder(r2=3.2, h=depth*mod, $fn=25);
		
	}
}

module BoxBottom()
{
	width = boxSize[0];
	height = boxSize[1];
	depth = boxSize[2];

	difference() {
		cube(boxSize);
		translate([boxWidth,boxWidth,boxWidth])
			cube([width-(boxWidth*2),height-(boxWidth*2),depth-boxWidth+10]);
		}
	offset = 4.2;
	depthSc = depth-7;
	translate([offset,offset,depthSc])
		ScrewHolderAndTaper();
	translate([width-offset,offset,depthSc])
		ScrewHolderAndTaper();
	translate([offset,height-offset,depthSc])
		ScrewHolderAndTaper();
	translate([width-offset,height-offset,depthSc])
		ScrewHolderAndTaper();
	
}

module PowerHoles()
{
	translate(arduinoOffset) // from other elements
	union() {
		//usb
		translate([-23,-9,3]) cube([15,13,12]);
		//power
		translate([-23,-39,4]) cube([15,11,11]);

		//power
		translate([-23,-39,4]) cube([15,8,11]);
	}
	powercableHole = [15,5,8];		
	translate(relayOffset)
	union() {
		//power
		translate([-10,41,10]) cube(powercableHole);
		translate([-10,34,10]) cube(powercableHole);
	}
	
}
module BoxTopScrewHoles()
{
	width = boxSize[0];
	height = boxSize[1];
	depth = boxSize[2];

	offset = 4.2;
	depthSc = depth;
	myheight = boxWidth*2;
	mod = 1;

	translate([offset,offset,depthSc])
		cylinder(r=1.2, h=myheight, $fn=25);
	translate([width-offset,offset,depthSc])
		cylinder(r=1.2, h=myheight, $fn=25);
	translate([offset,height-offset,depthSc])
		cylinder(r=1.2, h=myheight, $fn=25);
	translate([width-offset,height-offset,depthSc])
		cylinder(r=1.2, h=myheight, $fn=25);
}
module BoxTop()
{
	width = boxSize[0];
	height = boxSize[1];
	depth = boxSize[2];
	difference() {
		translate([0,0,depth])
			cube([width,height,boxWidth]);
		translate([26,14,depth])
			cube([26,23,33]);
		BoxTopScrewHoles();
	}
	
	
}


module Box() {
	color("green") BoxScrewHoldes();
	difference() {
		color("green") BoxBottom();
		color("green") PowerHoles();
	}
//	translate([20,20,10]) color("lightgreen") BoxTop();

}

relayOffset = [5,0,0];
arduinoOffsetY = 5;
arduinoOffset = [16,90,arduinoOffsetY];
boxSize = [80,110,40];

boxWidth = 3;
translate([0,10,2]) OtherElements();
 Box();
