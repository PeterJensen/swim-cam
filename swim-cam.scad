// Author: Peter Jensen

$fn = 50;

include <rounded-cube.scad>
include <rounded-cylinder.scad>

// Dimensions

x = 0; y = 1; z = 2;

m = 0.2;

batteryDims   = [56.7+m, 32.4+m, 15.0];
//esp32Dims   = [27.1, 39.8, 1.9];
esp32Dims     = [39.8+m, 27.1+m, 1.9+m];
//esp32PinsDims = [20.0, 2.4, 8.0];
//esp32PinsDims = [20.0, 4.0, 8.0];
esp32PinsDims = [esp32Dims[x], 4.0, 9.0];
esp32SdDims   = [17.2+m, 14.6+m, 1.3];
//esp32CamDims  = [8.0+m, 8.0+m, 2.1];
esp32CamDims  = [8.0+m, 8.0+m, 2.4];
esp32CamD     = 8.0+m;
esp32CamZ     = 3.4;
//esp32CamConnDims = [5.8, 16.3, 1.9];
esp32CamConnDims = [6.0, 16.5, 2.5];

tapDims       = [4.0, 6.0, 1.5];
tapOffset     = 1.0;
switchInnerD  = 12.0;
//switchOuterD  = 13.9;
switchOuterD  = 16.0;
switchInnerZ  = 23.9;
switchOuterZ  = 7.0;
switchZ       = switchInnerZ + switchOuterZ;

esp32SdSpace  = 2.5;
shellX        = 2*batteryDims.x + esp32Dims.x + esp32SdSpace - 10.0;
shellD        = batteryDims.y + 4.0;
shellT        = 2.5;
shellXOffset  = -esp32SdSpace/2;
shellXOffset  = 0;
shellZOffset  = batteryDims.z-2;

shellRoofDims = [shellX, shellD/2 - 5.0, 2.0];
shellRoofTilt = 20;

shiftX = -3.5;
battery1Offsets   = [(batteryDims.x-shellX)/2+esp32Dims.x + esp32SdSpace + shiftX, 0, batteryDims[z]/2];
battery2Offsets   = [battery1Offsets.x + batteryDims.x, 0, batteryDims[z]/2];
esp32Tilt         = 20;
esp32LOffsets     = [(esp32Dims.x - shellX)/2 + shiftX, ( sin(esp32Tilt)-1)*batteryDims[y]/2+0.5, esp32Dims[y]/2+1];
esp32ROffsets     = [(esp32Dims.x - shellX)/2 + shiftX, (-sin(esp32Tilt)+1)*batteryDims[y]/2-0.5, esp32Dims[y]/2+1];
//esp32Pins1Offsets = [-(esp32Dims[x]-esp32PinsDims[x])/2+2.7, -(esp32Dims[y]-esp32PinsDims[y])/2+0.6, (esp32Dims[z]-esp32PinsDims[z])/2 + 0.9];
esp32Pins1Offsets = [0, -(esp32Dims[y]-esp32PinsDims[y])/2+0.6, (-esp32Dims[z]+esp32PinsDims[z])/2 - 5.0];
esp32Pins2Offsets = [esp32Pins1Offsets[x], -esp32Pins1Offsets[y], esp32Pins1Offsets[z]];
esp32SdOffsets    = [(esp32SdDims[x]-esp32Dims[x])/2-1.9, 0, (esp32Dims[z]+esp32SdDims[z])/2];
esp32CamOffsets   = [(esp32CamDims[x]-esp32Dims[x])/2+4.4, 0, (esp32CamDims[z]+esp32Dims[z])/2 + esp32SdDims[z]];


module battery1() {
  translate(battery1Offsets)
    cube(batteryDims, center = true);
}

module battery2() {
  translate(battery2Offsets)
    cube(batteryDims, center = true);
}

module esp32Cam() {
  cube(esp32CamDims, center = true);
  translate([0, 0, esp32CamDims[z]/2])
    cylinder(d = esp32CamD, h = esp32CamZ);
}
  
module esp32Base() {
  translate(esp32Pins1Offsets)
    cube(esp32PinsDims, center = true);
  translate(esp32Pins2Offsets)
    cube(esp32PinsDims, center = true);
  translate(esp32SdOffsets)
    cube(esp32SdDims, center = true);
  translate(esp32CamOffsets)
    esp32Cam();
  cube(esp32Dims, center = true);
  translate([(esp32Dims[x] - esp32CamConnDims[x])/2 - 11.5, 0, (esp32CamConnDims[z] + esp32Dims[z])/2])
    cube(esp32CamConnDims, center = true);
}

module esp32L() {
  translate(esp32LOffsets)
    rotate([-90-esp32Tilt, 0, 0])
      rotate([0, 180, 0])
        esp32Base();
}

module esp32R() {
  translate(esp32ROffsets)
    rotate([90+esp32Tilt, 0, 0])
      rotate([0, 180, 0])
        esp32Base();
}

module switchBase() {
  cylinder(d = switchInnerD, h = switchInnerZ);
  translate([0, 0, switchInnerZ])
    cylinder(d = switchOuterD, h = switchOuterZ);
}

module switch1() {
  translate([12.0, 0, 5.0])
    switchBase();
}

sm = 0.4;
switch2Dims = [6.3+sm, 6.3+sm, 3.6];
switch2D    = 3.5;
switch2Z    = 2.0;

switchSpacing = 12.0;

module switch2() {
//  translate([27.0, 0, 28.0]) {
  translate([onOffSwitchDims.x/2 + 2*switchSpacing, 0, 28.0]) {
    cube(switch2Dims, center = true);
    translate([0, 0, switch2Dims[z]/2])
      cylinder(d = switch2D, h = switch2Z);
  }
}
  
module switch2a() {
  translate([(shellX + shellD - switchZ)/2 - 0.0, 0, 22])
    rotate([0, 85, 0])
      translate([0, 0, -switchZ/2])
        switchBase();
}

module inside() {
  battery1();
  battery2();
  esp32L();
  esp32R();
  //switch1();
  switch2();
  onOffSwitch();
  led();
//  esp32Base();
}

module shellCyl() {
  difference() {
    rotate([0, 90, 0])
      translate([0, 0, -shellX/2])
        difference() {
          cylinder(d = shellD, h = shellX);
          cylinder(d = shellD-2*shellT, h = shellX);
        };
    translate([0, 0, -shellD/4])
      cube([shellX+1, shellD, shellD/2], center = true);
  }
}

module shellSphere() {
  difference() {
    sphere(d = shellD);
    sphere(d = shellD-2*shellT);
    translate([shellD/2, 0, 0])
      cube(shellD, center = true);
    translate([0, 0, -shellD/2])
      cube(shellD, center = true);
  }
}
  
module shellTop() {
  shellCyl();
  translate([-shellX/2, 0, 0])
    shellSphere();
  translate([shellX/2, 0, 0])
    rotate([0, 0, 180])
      shellSphere();
  translate([0, 0, shellD/2-7])
    shellRoof(shellRoofDims);
}

module shellCap() {
  translate([0, 0, -batteryDims[z]/2])
    difference() {
      cylinder(d = shellD, h = batteryDims[z]);
      cylinder(d = shellD - 2*shellT, h = batteryDims[z]);
      translate([shellD/2, 0, batteryDims[z]/2])
        cube([shellD, shellD, batteryDims[z]], center = true);
      translate([(tapDims[x]-shellD)/2, 0, tapDims[z]/2 + tapOffset])
        cube(tapDims, center = true);
    }
}

shellBottomOffsets = [0, 0, -batteryDims[z]/2];

module shellBottom() {
  translate(shellBottomOffsets) {
    difference() {
      cube([shellX, shellD, batteryDims[z]], center = true);
      cube([shellX, shellD - 2*shellT, batteryDims[z]], center = true);
      translate([0, (shellD - tapDims[x])/2, (tapDims[z] - batteryDims[z])/2 + tapOffset])
        rotate([0, 0, 90])
          cube(tapDims, center = true);
      translate([0, (-shellD + tapDims[x])/2, (tapDims[z] - batteryDims[z])/2 + tapOffset])
        rotate([0, 0, 90])
          cube(tapDims, center = true);
    }
    translate([-shellX/2, 0, 0])
      shellCap();
    translate([shellX/2, 0, 0])
      rotate([0, 0, 180])
        shellCap();
  }
}

module shellRoof(dims) {
  yOffset = cos(shellRoofTilt)*dims.y/2 - sin(shellRoofTilt)*dims.z/2;
  zOffset = sin(shellRoofTilt)*dims.y/2 + dims.z/2;
  translate([0, -yOffset, zOffset])
    rotate([shellRoofTilt, 0, 0])
      cube(dims, center = true);
  translate([0, yOffset, zOffset])
    rotate([-shellRoofTilt, 0, 0])
      cube(dims, center = true);
}

module shell() {
  difference() {
    translate([shellXOffset, 0, shellZOffset]) {
      shellTop();
      shellBottom();
    }
    inside();
    translate([0, 0, -batteryDims[z]])
      battery1();
    translate([0, 0, -batteryDims[z]])
      battery2();
  }
}

shellBaseDims = [shellX, shellD, 5.0];

module shellBaseShape(dims, r = -1) {
  cube(dims, center = true);
  translate([dims[x]/2, 0, 0])
    roundedCylinder(d = dims[y], h = dims[z], r = r);
  translate([-dims[x]/2, 0, 0])
    roundedCylinder(d = dims[y], h = dims[z], r = r);
}

shellBaseRimT = 1.0;

module shellBaseRimShape(dims) {
  difference() {
    shellBaseShape(dims);
    shellBaseShape([dims.x, dims.y-2*shellBaseRimT, dims.z]);
  }
}

shellBaseRimDims = [shellBaseDims[x]-0.2, shellBaseDims[y]-2*shellT-0.2, 1.5];

module shellBaseRim() {
  translate([0, 0, shellBaseRimDims[z]/2 + shellBaseDims[z]/2])
    shellBaseRimShape(shellBaseRimDims);
}

magnetD = 16.4;
magnetZ = 5.2;

module magnet() {
  cylinder(d = magnetD, h = magnetZ, center = true);
}

module magnets() {
  qx = (shellX+shellD)/16;
  translate([-6.0*qx, 0, 0])
    magnet();
  translate([-1.5*qx, 0, 0])
    magnet();
  translate([1.5*qx, 0, 0])
    magnet();
  translate([6.0*qx, 0, 0])
    magnet();
}

shellRailDims = [shellX, 5.0, shellBaseDims.z + 4.0];
//railCutDims = [shellBaseDims.x+shellD, shellBaseDims.y - 2*shellRailDims.y, shellRailDims.z];
railCutDims = [shellBaseDims.x+shellD, shellBaseDims.y - 2*shellRailDims.y, shellRailDims.z];

module shellBase() {
  radius = 2;
  module halfCyl() {
    difference() {
//      roundedCylinder(d = shellBaseDims.y, h = shellRailDims.z, r = 3);
      roundedRing(d = shellBaseDims.y, h = shellRailDims.z, r = radius, t = shellRailDims.y);
      translate([shellBaseDims.y/2, 0, 0])
        cube([shellBaseDims.y, shellBaseDims.y, shellRailDims.z], center = true);
    }
  }
  zOffset = (shellBaseDims.z - shellRailDims.z)/2;
  module railAndCaps() {
    translate([0, (shellRailDims.y - shellBaseDims.y)/2, zOffset])
      roundedCube(shellRailDims, r = radius, axis=[[0, 1, 1, 0],0,0]);
    translate([0, (-shellRailDims.y + shellBaseDims.y)/2, zOffset])
      roundedCube(shellRailDims, r = radius, axis=[[0, 1, 1, 0],0,0]);
    translate([-shellRailDims.x/2, 0, zOffset])
      halfCyl();
    translate([shellRailDims.x/2, 0, zOffset])
      rotate([0, 0, 180]) halfCyl();
  }
  roofDims = [railCutDims.x, railCutDims.y/2+2, 6];
  difference() {
    shellBaseShape(shellBaseDims);
    translate([0, 0, -shellBaseDims.z-roofDims.z/2-0.7])
      shellRoof(roofDims);
  }
  railAndCaps();
    
//    union() {
//      shellBaseShape(shellBaseDims);
//      railAndCaps();
//    }
//    translate([0, 0, -shellBaseDims[z]/2 + magnetZ/2])
//      magnets();
//    translate([shellD/4, 0, (-railCutDims.z - shellBaseDims.z)/2]) {
//    translate([0, 0, (-railCutDims.z - shellBaseDims.z)/2]) {
//      cube(railCutDims, center = true);
//      translate([0, 0, railCutDims.z/2-roofDims.z+0.3])
//        shellRoof(roofDims);
//    }
//  }
}

//shellLidOffsets = [shellXOffset, 0, 0];
shellLidOffsets = [0, 0, 0];

module shellLid() {
  translate(shellLidOffsets) {
    shellBase();
    shellBaseRim();
    taps();
  }
}

module tap() {
  h = 4;
  rotate([90,0,0])
    translate([0, -2, -h/2])
      linear_extrude(height = 4)
        polygon([[0,0], [0,4],[2,4],[3,3],[2,2],[2,0]]);
}

module taps() {
  xOffset = shellBaseRimDims.x/2+shellBaseRimDims.y/2-2;
  yOffset = shellBaseRimDims.y/2-2;
  zOffset = (shellBaseDims.z + 2)/2-0.5;
  translate([xOffset, 0, zOffset]) tap();
  translate([-xOffset, 0, zOffset]) rotate([0, 0, 180]) tap();
  translate([0, yOffset, zOffset]) rotate([0, 0, 90]) tap();
  translate([0, -yOffset, zOffset]) rotate([0, 0, -90]) tap();
  
}

shellBarDims = [shellX, railCutDims.y-4.0, shellRailDims.z];
strapDims = [28.0, shellBarDims.y, 2.5];
zipDims   = [5.0, shellBarDims.y, 2.0];

module shellBar() {
  radius = 2;
  xStrapOffset = shellBarDims.x/2 - strapDims.x/2 ;
  zStrapOffset = (shellBarDims.z - strapDims.z)/2 - 1.5;
  xZipOffset = xStrapOffset - (strapDims.x + zipDims.x)/2 - 5.5;
  zZipOffset = (zipDims.z - shellBarDims.z)/2 + 1;
  module roundEnd() {
    translate([0, 0, -shellBarDims.z/4])
      roundedCylinder(d = shellBarDims.y, h = shellBarDims.z/2, r = radius);
    translate([0, 0, shellBarDims.z/4])
      rotate([180, 0, 0])
        roundedCylinder(d = shellBarDims.y, h = shellBarDims.z/2, r = radius);
  }
  module barRoundedEnds() {
    roundedCube(shellBarDims, r = radius, axis=[1,0,0]);
    translate([shellBarDims.x/2, 0, 0])  roundEnd();
    translate([-shellBarDims.x/2, 0, 0]) roundEnd();
  }
  module strap() {
    cube(strapDims, center = true);
  }
  module zip() {
    cube(zipDims, center = true);
  }
  module zipPlate() {
    cube([zipDims.x, zipDims.y+40, 80], center = true);
  }
  
  difference() {
    barRoundedEnds();
//      translate([0, 0, (shellBarDims.z - magnetZ)/2])
//        magnets();
    translate([xStrapOffset, 0, zStrapOffset])
      strap();
    translate([-xStrapOffset, 0, zStrapOffset])
      strap();
    translate([xZipOffset, 0, zZipOffset])
      zip();
    translate([-xZipOffset, 0, zZipOffset])
      zip();
  }
//  translate([xZipOffset, 0, 0])
//    zipPlate();
//  translate([-xZipOffset, 0, 0])
//    zipPlate();
}

onOffSwitchDims = [14.8, 8.7, 9.0];
onOffSwitchTopDims = [15.5, 10.8, 5.0];

module onOffSwitch() {
  translate([0, 0, 24.5]) {
    cube(onOffSwitchDims, center = true);
    translate([0, 0, (onOffSwitchDims.z + onOffSwitchTopDims.z)/2])
      cube(onOffSwitchTopDims, center = true);
  }
}

module led() {
  translate([onOffSwitchDims.x/2 + switchSpacing, 0, 24.5])
    cylinder(d = 3.5, h = 7);
}

module main() {
  shell();
  
  //translate([0, 0, -12]) shellLid();
  //translate([0, 0, -30]) shellBar();
  //onOffSwitch();
  //inside();
}

main();
