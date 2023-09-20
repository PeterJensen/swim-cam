// Author: Peter Jensen

module roundCut(dims, r) {
  difference() {
    translate([-1, -1, -dims.z/2-0.5])
      cube([r+1, r+1, dims.z+1]);
    translate([r, r, 0])
      cylinder(h = dims.z+1.5, r = r, center = true);
  }
}

module pillCylinder(h, r) {
  cylinder(h = h-2*r, r = r, center=true);
  translate([0, 0, h/2-r]) sphere(r = r);
  translate([0, 0, -h/2+r]) sphere(r = r);
}
  
module roundCut2(dims, r) {
  difference() {
    translate([-1, -1, -dims.z/2-0.5])
      cube([r+1, r+1, dims.z+1]);
    translate([r, r, 0])
      pillCylinder(h = dims.z, r = r);
  }
}

module cornerCut(r) {
  difference() {
    cube([r, r, r], center = true);
    translate([r/2, r/2, r/2]) sphere(r=r);
  }
}

module cornerCut4(dims, r) {
  translate([(dims.x-r)/2, (r-dims.y)/2, (dims.z-r)/2]) rotate([-90, 0, 90]) cornerCut(r);
  translate([(dims.x-r)/2, (r-dims.y)/2, (r-dims.z)/2]) rotate([0, 0, 90]) cornerCut(r);
  translate([(dims.x-r)/2, (dims.y-r)/2, (dims.z-r)/2]) rotate([-90, 0, 180]) cornerCut(r);
  translate([(dims.x-r)/2, (dims.y-r)/2, (r-dims.z)/2]) rotate([0, 0, -180]) cornerCut(r);
}  

module roundCuts(dims, edges, r) {
  echo ("roundCuts");
  if (abs(edges) != undef) {  
    translate([-dims.x/2, -dims.y/2, 0]) roundCut(dims, r);
    translate([dims.x/2, dims.y/2, 0]) rotate([0, 0, 180]) roundCut(dims, r);
    translate([dims.x/2, -dims.y/2, 0]) rotate([0, 0, 90]) roundCut(dims, r);
    translate([-dims.x/2, dims.y/2, 0]) rotate([0, 0, -90]) roundCut(dims, r);
  }
  else {
    if (edges[0]) translate([-dims.x/2, -dims.y/2, 0]) roundCut(dims, r);
    if (edges[2]) translate([dims.x/2, dims.y/2, 0]) rotate([0, 0, 180]) roundCut(dims, r);
    if (edges[1]) translate([dims.x/2, -dims.y/2, 0]) rotate([0, 0, 90]) roundCut(dims, r);
    if (edges[3]) translate([-dims.x/2, dims.y/2, 0]) rotate([0, 0, -90]) roundCut(dims, r);
  }
}

module roundedCube(dims, r, axis=[1, 1, 1]) {
  difference() {
    cube(dims, center = true);
    if (axis.x)
      rotate([0, 90, 0]) roundCuts([dims.z, dims.y, dims.x], axis.x, r);
    if (axis.y)
      rotate([90, 0, 0]) roundCuts([dims.x, dims.z, dims.y], axis.y, r);
    if (axis.z)
      roundCuts(dims, axis.z, r);
    if (axis.x && axis.y && axis.z) {
      cornerCut4(dims, r);
      rotate([0, 0, 180]) cornerCut4(dims, r);
    }
  }
}

//$fn=40;
//dims=[20,30,40];
//r = 3;

//cornerCut(dims, r);
//translate([-dims.x/2, -dims.y/2, 0]) 
//  roundCut2(dims, r);
//roundCuts(dims, 1, r);

//difference() {
//  roundedCube(dims, r, axis=[1,1,1]);
//  cornerCut4(dims, r);
//  rotate([0, 0, 180]) cornerCut4(dims, r);
//}

//pillCylinder(dims.z, r);
//cylinder(h = dims.z-2*r, r = 5, center=true);
//translate([0, 0, dims.z/2-r]) sphere(r = 5);
//translate([0, 0, -dims.z/2+r]) sphere(r = 5);