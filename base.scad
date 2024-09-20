$fn=128;

module dial()
{
    difference() {
        union() {
            cylinder(r=14, h=8);
            union() {
                difference() {
                    translate([0,0,6]) cylinder(r1=16, r2=15, h=2);

                    for (theta = [0: 7.5/2: 360])
                        rotate([0,-21,theta]) translate([16+1.2,0,0]) rotate([-15,0,0]) translate([0,0,-2]) cylinder(r=0.5, h=20, $fn=12);
                }
            }
        }
    
        translate([8,0,11]) sphere(r=5);
    }
}

module rounded_cube(x, y, z, r) {
	dx = x - (r*2);
	dy = y - (r*2);
	hull() {
		translate([-dx/2,-dy/2]) cylinder(r=r, h=z);
		translate([-dx/2,dy/2]) cylinder(r=r, h=z);
		translate([dx/2,-dy/2]) cylinder(r=r, h=z);
		translate([dx/2,dy/2]) cylinder(r=r, h=z);
	}
}


module base ()
{
    h = 30;
    difference() {
        hull() {
            cylinder(r=145/2, h=h);
            rotate([0,0,30]) translate([0,-145/2 - 6.5, 18]) rotate([75,0,0]) rounded_cube(72 + 12, 36, 5, 5);
        }
        translate([0,0,2]) cylinder(r=140/2, h=h+20);

        

        // display
        rotate([0,0,30]) translate([0,-145/2 - 6.5, 18]) rotate([75,0,0]) translate([18,0,5-2]) rounded_cube(32, 32, 5, 1.5);
        
        intersection() {
            translate([0,0,2]) rotate([0,0,30]) translate([0,-145/2,40/2]) cube([38*2, 60, 40], center=true);
            rotate([0,0,30]) translate([0,-145/2 - 6.5, 18]) rotate([75,0,0]) translate([0,0,-26.9]) rounded_cube(36*2, 34, 30, 2.5);
        }
        //rotate([0,0,30]) translate([-32/2,-145/2 - 6.5, 10]) rotate([75,0,0]) translate([0,0,-12]) cube([32, 32, 20]);

        // dial
        rotate([0,0,30]) translate([0,-145/2 - 6.5, 18]) rotate([75,0,0]) translate([-18,0,0]) cylinder(r=15, h=8);
        rotate([0,0,30]) translate([0,-145/2 - 6.5, 18]) rotate([75,0,0]) translate([-18,0,-15]) cylinder(r=5, h=20);
    }
}




module top(){
    difference(){
        union(){
            cylinder(r=140/2, h=2);
            translate([0,0,2]) cylinder(r1=78.5/2, r2=69/2, h=5);
        }

        translate([0,0,-0.5]) cylinder(r=39/2, h=10);
        translate([0,0,-0.5]) cylinder(r1=76.5/2, r2=67/2, h=5.5);
    }
}

module display() {
    cube([32, 50, 1.37]);
    translate([0,0,5.7]) cube([32, 32, 7]);
    translate([0,32,1.37]) cube([32, 10, 7]);
}

//cylinder(r=38/2, h=30);

/*hull(){
    cylinder(r=14, h=3);
    translate([15,0,3/2]) cube([28, 28, 3], center=true);
}*/

//if (false)
difference(){
    union(){
        rotate([0,0,30]) translate([0,-145/2 - 6.5, 18]) rotate([75,0,0]) translate([-18,0,0]) rotate([0,0,0]) dial();
        base();
        translate([0,0,30-2]) top();
        
         // SSR
        %translate([-40,0,22.5/2 + 2]) rotate([0,0,90]) cube([60, 45, 22.5], center=true);
        translate([-40,0,5/2])
        difference() {
            translate([0,0,0]) rotate([0,0,90]) cube([60+3, 45+3, 5], center=true);
            translate([0,0,1]) rotate([0,0,90]) cube([60+1, 45+1, 5+1], center=true);
        }


        // PSU
        %translate([0,50, 20/2 + 2]) cube([58.3, 25.7, 20], center=true);
        translate([0,50,(20+2)/2]) difference()
        {
            cube([58.3+3, 25.7+3, 20+2], center=true);
            translate([0,0,1]) cube([58.3+1, 25.7+1, 20+2], center=true);
        }

        // Speaker
        %rotate([0,0,-40]) translate([0, -145/2 + 5, 30]) rotate([90,0,180]) cylinder(r=33.8/2, h=3.8);

        // ATMega
        % translate([40,0,10/2 + 2]) cube([40,40,10], center=true);
        translate([40,0,10/2]) difference()
        {
            cube([40+3,40+3,10], center=true);
            translate([0,0,1]) cube([40+1, 40+1, 10], center=true);
        }
        
        rotate([0,0,30]) translate([0,-145/2,0]) rotate([75,0,0]) rotate([0,0,90]) translate([0,-34,0]) display();

    }
    //translate([0,0,120]) cube([200, 200, 200], center=true);
}
