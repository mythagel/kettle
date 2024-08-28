
module frame(){
    $fn=128;
    
    difference(){
        cylinder(r=145/2, h=38);
        translate([0,0,-0.5]) cylinder(r=145/2 - 2, h=38+1);
    }
    
    translate([0,0,38 - 12]) difference(){
        cylinder(r=145/2, h=1);
        translate([0,0,-0.5]) cylinder(r=145/2 - 7, h=2);
    }
    
    step = 360/8;
    for (theta = [step: step: 360])
        rotate([0,0,theta]) translate([145/2 - 3/2 - 1,0, 24/2 + 2.5]) cube([3, 2, 24], center=true);
}

module base(){
    connector_d = 45;
    
    // TODO need smaller screws
    
    $fn=128;
    difference(){
        union(){
            cylinder(r=145/2 - 2.5, h=2);
            translate([0,0,2]) cylinder(r1=(78.5 + 4)/2, r2=(69+4)/2, h=5);
            
            step = 360/3;
            for (theta = [step: step: 360])
                rotate([0,0,theta]) translate([52/2,0, 5]) cylinder(r=7.5/2, h=3.3);
        }

        translate([0,0,-0.5]) cylinder(r=connector_d/2, h=10);
        translate([0,0,-0.5]) cylinder(r1=(76.5 + 4)/2, r2=(67 + 4)/2, h=5.5 + (2 - 1.5));
        
        step = 360/3;
        for (theta = [step: step: 360])
            rotate([0,0,theta]) translate([52/2,0, 5])
                {
                    cylinder(r=4/2, h=10);
                    cylinder(r=6/2, h=2.5);
                }
    }
}

base();
frame();