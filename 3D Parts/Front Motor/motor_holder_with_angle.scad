$fn = 64;

angle = -15;

module motor_attach() {
  translate([0, 0, -4]) {
    rotate([0, 0, 45])
    difference() {
      cylinder(4, d=27, true);
      cylinder(4, d=8, true);
      translate([9.5, 0, 0]) cylinder(4, d=3.5, true);
      translate([-9.5, 0, 0]) cylinder(4, d=3.5, true);
      rotate([0, 0, 90]) {
        translate([8, 0, 0]) cylinder(4, d=3.5, true);
        translate([-8, 0, 0]) cylinder(4, d=3.5, true);
      }      
    }    
  }
}

module quarter() {
  translate([-1.5, -2, -2]) difference() {
    cube([3, 4, 4]);
    rotate([0, 90, 0]) cylinder(3, d=4, true);
  }
}

//        translate([-1.5, 0, 9.5/2]) 
//         rotate([0, 90, 0]) 
//           cylinder(3, d=1.5, true); 

//         translate([-1.5, 0, 9.5/2]) 
//           rotate([0, 90, 0]) 
//             cylinder(3, d=4, true);


max1 = angle < -10 ? 14 : (angle < -5 ? 10 : 8);

module hole(hight) {
  translate([0, 0, hight/2-(hight- 2.5)])
  difference() {
    union() {
      cube([3, 4, hight], true);
      translate([-1.5, 0, hight/2]) rotate([0, 90, 0])
        cylinder(3, d=4, true);
    }
    translate([-1.5, 0, hight/2]) rotate([0, 90, 0]) 
      cylinder(3, d=1.5, true);
  }
}

module holders() {
  translate([0, 0, -4]) {
    translate([11.5+1.5, 0, 8/2])
          cube([3, 4, 8], true);
    union() {
      translate([-(11.5+1.5),0,max1/2])
          cube([3, 4, max1], true);
      translate([-(11.5 + 1.5), 8, 7/2]) {
          difference() {
            cube([3, 20, 7], true);
            translate([-1.5, 8, -(7/2 - 2)]) 
              rotate([0, 90, 0]) 
                cylinder(3, d=1.5, true); 
            translate([2, -6, 7/2]) {
              rotate([-90, 0, 90])
              linear_extrude(4) polygon([[0,0],[16,0],[16,7-4]]);
            }
            translate([0, 10, -7/2])
              rotate([-90, 0, 0]) quarter();
            translate([1.5, 10, 7/2-3]) fillet();
          }
      }
    }
  }
}

difference()
//union() 
{
  translate([0,0,0]) {
    rotate([0,angle,0]) translate([-29/2, 0, -3]) {
      motor_attach();
      holders();
    }

    translate([-29/2, 0, -3]) {
      translate([11.5+1.5, 0, 3]) hole(5);
      translate([-(11.5+1.5), 0, 3]) hole(7);
    }
  }
  
  rotate([0,angle,0]) {
    translate([-(1 + 29), 0, (8-6)/2]) cube([2,4,6], true);
    translate([-4, 0, 0.2]) cube([2,4,5], true);
  }
  translate([-(1 + 29), 0, 0]) cube([2,4,8], true);
  translate([-4, 0, 0]) cube([2,4,5], true);
}

module fillet() {
  difference() {
    translate([-1.5,-(1.61966/2),0]) cube([3, 2, 1.61966*2],true);
    rotate([-10.61966,0,0]) 
      rotate([100.61966/2,0,0]) 
        translate([-3,-2.5990596,0]) 
          rotate([0,90,0]) 
            cylinder(3, r=2, true);


  }
}

//translate([30,0,0]) {
//  difference() {
//    translate([-1.5,-(1.61966/2),0]) cube([3, 1.61966, 1.61966*2],true);
//    rotate([-10.61966,0,0])
//    difference() {
//      rotate([90,10.61966,-90]) 
//        linear_extrude(3) 
//          polygon([[0,0],[16,3],[16,-5],[0,-5]]);
//      rotate([100.61966/2,0,0]) translate([-3,-2.599,0]) 
//        rotate([0,90,0]) 
//          cylinder(3, r=2, true);
//    }
//  }//
//}