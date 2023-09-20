// Author: Peter Jensen

module cutBox(d, r) {
  difference() {
    translate([0, 0, -(r)/2])
      cube([d, d, r], center = true);
    rotate_extrude() {
      translate([d/2-r, 0]) circle(r = r);
      translate([0, -r]) square([d/2-r, 2*r]);
    }
  }
}

module cutBoxRing(d, r, t) {
  difference() {
    translate([0, 0, -(r)/2])
      cube([d, d, r], center = true);
    rotate_extrude() {
      translate([d/2-r, 0]) circle(r = r);
      translate([d/2-t+r, 0]) circle(r = r);
      translate([d/2-t+r, -r]) square([t-2*r, 2*r]);
    }
  }
}
  
module roundedCylinder(d, h, r) {
  difference() {
    cylinder(d = d, h = h, center = true);
    if (r != -1)
      translate([0, 0, -h/2+r])
        cutBox(d, r);
  }
}

module roundedRing(d, h, r, t) {
  difference() {
    cylinder(d = d, h = h, center=true);
    cylinder(d = d - 2*t, h = h, center=true);
    if (r != -1)
      translate([0, 0, -h/2+r])
        cutBoxRing(d, r, t);
  }
}
