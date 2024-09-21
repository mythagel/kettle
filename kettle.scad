    $fn=256;
    
module frame(){
    
    difference(){
        cylinder(r=145/2, h=38);
        translate([0,0,-0.5]) cylinder(r=145/2 - 2, h=38+1);
    }
    
    translate([0,0,38 - 12]) translate([0,0,-9]) difference(){
        cylinder(r=145/2, h=10);
        translate([0,0,-0.5]) cylinder(r2=145/2 - 7, r1=145/2, h=10+1);
    }
}

module base(){
    connector_d = 45;
    
    // TODO need smaller screws
    

    difference(){
        union(){
            cylinder(r=145/2, h=2);
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

difference()
{
    union(){
        base();
        frame();
    }

    translate([0,0,0]) difference(){
        cylinder(r=145.5/2, h=5);
        translate([0.0,0]) cylinder(r2=145.5/2, r1=140/2, h=5);
    }
}