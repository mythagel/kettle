$fn=128;
d=25.4 * 2.5;

// Aluminium
difference() {
    cylinder(r=d/2, h=5.5);

    translate([0, 0,-0.5]) cylinder(r=51/2, h=5.5+1);
    
    rotate([0,0,0]) translate([20, 0,-0.5]) cylinder(r=5/2, h=5.5+1);
    rotate([0,0,-131.5]) translate([30, 0,-0.5]) cylinder(r=5/2, h=5.5+1);
    rotate([0,0,131.5]) translate([30, 0,-0.5]) cylinder(r=5/2, h=5.5+1);
    
    delta_theta = 40;
    rotate([0,0,delta_theta]) translate([57/2, 0,-0.5]) cylinder(r=2.5/2, h=5.5+1);
    rotate([0,0,delta_theta+120*1]) translate([57/2, 0,-0.5]) cylinder(r=2.5/2, h=5.5+1);
    rotate([0,0,delta_theta+120*2]) translate([57/2, 0,-0.5]) cylinder(r=2.5/2, h=5.5+1);
}

// FIXME accurately calculate screw post positions

/*
translate([-19.8,0,0]) rotate([0,0,90]) cube([50, 1, 4], center=true);
translate([-19.8,0,0]) rotate([0,0,90]) cube([40, 1, 15], center=true);

translate([-23,16,0]) rotate([0,0,65]) cube([14.4, 1, 15], center=true);

*/